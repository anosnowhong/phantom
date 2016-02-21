/******************************************************************************/
/*                                                                            */ 
/* MODULE  : STIM.cpp                                                         */ 
/*                                                                            */ 
/* PURPOSE : Parallel port (LPT) Interface API for Muscle Stimulator.         */ 
/*                                                                            */ 
/* DATE    : 23/Mar/2001                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 23/Mar/2001 - Initial development.                               */ 
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     "STIM"
#define MODULE_TEXT     "Muscle Stimulator API"
#define MODULE_DATE     "23/03/2001"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL    4

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

PRINTF  STIM_PRN_messgf=NULL;                    // General messages printf function.
PRINTF  STIM_PRN_errorf=NULL;                    // Error messages printf function.
PRINTF  STIM_PRN_debugf=NULL;                    // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     STIM_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(STIM_PRN_messgf,buff));
}

/******************************************************************************/

int     STIM_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(STIM_PRN_errorf,buff));
}

/******************************************************************************/

int     STIM_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(STIM_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    STIM_API_started=FALSE;
BOOL    STIM_opened=FALSE;
BOOL    STIM_charged=FALSE;
USHORT  STIM_LPTn=LPT_INVALID;
BYTE    STIM_chip[STIM_CHIP_MAX] = { STIM_DATA_CHIP_1,STIM_DATA_CHIP_2 };
BYTE    STIM_mask;
int     STIM_CH[STIM_CHANNEL_MAX];
TIMER   STIM_TIMER_CHARGE("STIM_Charge");
TIMER   STIM_TIMER_DURATION("STIM_Duration");
TIMER   STIM_TIMER_INTERVAL("STIM_Interval");
TIMER   STIM_TIMER_PULSE("STIM_Pulse");

double  STIM_PWT=0.0;
double  STIM_PWM;
long    STIM_PC=0L;

/******************************************************************************/

BYTE    STIM_LPT_byte[LPT_PORT_MAX];

/******************************************************************************/

void    STIM_LPT( USHORT port, BYTE byte )
{
    LPT_byte(STIM_LPTn,port,byte);
    STIM_LPT_byte[port] = byte;
    STIM_TraceLoop();
}

/******************************************************************************/

BYTE    STIM_LPT( USHORT port )
{
    return(LPT_byte(STIM_LPTn,port));
}

/******************************************************************************/

BOOL    STIM_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok;

    STIM_PRN_messgf = messgf;               // General API message print function.
    STIM_PRN_errorf = errorf;               // API error message print function.
    STIM_PRN_debugf = debugf;               // Debug information print function.

    if( STIM_API_started )                  // Start the API once...
    {
        return(TRUE);
    }

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(STIM_API_stop);          // Install stop function.
        STIM_API_started = TRUE;            // Set started flag.

        MODULE_start(STIM_PRN_messgf);      // Register module.
    }
    else
    {
        STIM_errorf("STIM_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    STIM_API_stop( void )
{
int     ID;

    if( !STIM_API_started )                 // API not started in the first place...
    {
         return;
    }

    STIM_close();

    if( STIM_PC > 0L )
    {
        STIM_PWM = STIM_PWT / (double)STIM_PC;
        STIM_debugf("Mean pulse width %.4lf msec.\n",STIM_PWM);
    }

    STIM_API_started = FALSE;          // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    STIM_API_check( void )
{
BOOL    ok=TRUE;

    if( !STIM_API_started )            // API not started...
    {                                  // Start module automatically...
        ok = STIM_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        STR_printf(ok,STIM_debugf,STIM_errorf,"STIM_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

BOOL    STIM_check( BOOL charge )
{
    if( !STIM_opened )
    {
        STIM_errorf("STIM_check() Stimulator Interface not opened.\n");
        return(FALSE);
    }

    if( charge && !STIM_charged )      // Wait for stimulator to charge...
    {
        STIM_debugf("STIM_check() Waiting to charge.\n");
//      while( !STIM_TIMER_CHARGE.Expired(STIM_CHARGE) );
        STIM_charged = TRUE;           // Should be charged now.
    }

    return(TRUE);
}

/******************************************************************************/

BOOL    STIM_open( int LPTn, BOOL trace )
{
int     CH[STIM_CHANNEL_MAX],channel,timer;

    if( !STIM_API_check() )                 // Make sure API is running...
    {
        return(FALSE);
    }

    if( LPT_base(LPTn) == LPT_INVALID )     // Valid LPT number?
    {
        STIM_errorf("STIM_open(LPTn=%d,trace=%s) Invalid LPT port.\n",LPTn,STR_YesNo(trace));
        return(FALSE);
    }

    STIM_LPTn = LPTn;

    if( trace )
    {
        STIM_TraceStart();
    }

    STIM_mask = 0x00;                       // Zero chip-select mask.
    STIM_LPT(LPT_PORT_DATA,0x00);           // Zero data port.
    STIM_enable(TRUE);                      // Set enable line.
    STIM_opened = TRUE;                     // Interface enabled.
    STIM_charged = FALSE;                   // Stimulator not charged.
    STIM_TIMER_CHARGE.Reset();              // Start charge timer from now.

    for( channel=0; (channel < STIM_CHANNEL_MAX); channel++ )
    {
        STIM_CH[channel] = -1;
        CH[channel] = 0;
    }

    STIM_amplitude(CH);                     // Zero POTs.

    STIM_debugf("STIM_open(LPTn=%d,trace=%s) OK.\n",LPTn,STR_YesNo(trace));

    return(TRUE);
}

/******************************************************************************/

BOOL    STIM_open( BOOL trace )
{
    return(STIM_open(STIM_LPT_DEFAULT,trace));
}

/******************************************************************************/

BOOL    STIM_open( void )
{
    return(STIM_open(STIM_LPT_DEFAULT,FALSE));
}

/******************************************************************************/

void    STIM_close( void )
{
int     timer;

    if( !STIM_API_check() )                 // Make sure API is running...
    {
        return;
    }

    if( !STIM_opened )
    {
        return;
    }

    if( LOOPTASK_running(STIM_loop) )       // Stop LoopTask if it's running...
    {
        LOOPTASK_stop(STIM_loop);
    }

    STIM_enable(FALSE);                     // Clear enable line.
    STIM_TraceStop();                       // Stop debug trace.

    STIM_debugf("STIM_close()\n");

    STIM_opened = FALSE;
}

/******************************************************************************/

#define STIM_HACK_DELAY      0.010

void    STIM_enable( BOOL enable )
{
BYTE    ctrl;

    ctrl = STIM_LPT(LPT_PORT_CTRL);

    if( enable )             // Set enable bit...
    {
        ctrl |= STIM_CTRL_ENABLE;
    }
    else                     // Clear enable bit...
    {
        ctrl &= ~STIM_CTRL_ENABLE;
    }

    STIM_debugf("STIM_enable(%s) 0x%02X\n",STR_YesNo(enable),ctrl);

    STIM_LPT(LPT_PORT_CTRL,ctrl);

    TIMER_delay(STIM_HACK_DELAY); // Hack delay...
}

/******************************************************************************/

void    STIM_serialbit( BYTE bit )
{
BYTE    mask;                 

    mask = STIM_mask;             //  Set chip select mask...

    if( bit )                     //  Set data bit... 
    {
        mask |= STIM_DATA_XMIT;
    }

//  Clock low-high-low to send data...
    mask &= ~STIM_DATA_CLOCK;     // Clock low.
    STIM_LPT(LPT_PORT_DATA,mask);

    TIMER_delay(STIM_HACK_DELAY); // Hack delay...

    mask |=  STIM_DATA_CLOCK;     // Clock high.
    STIM_LPT(LPT_PORT_DATA,mask);

    TIMER_delay(STIM_HACK_DELAY); // Hack delay...

    mask &= ~STIM_DATA_CLOCK;     // Clock low.
    STIM_LPT(LPT_PORT_DATA,mask);

    TIMER_delay(STIM_HACK_DELAY); // Hack delay...
}

/******************************************************************************/

void    STIM_serialbyte( BYTE data )
{
int     b;

    STIM_debugf("STIM_serialbyte(0x%02X)\n",data);

    for( b=7; (b >= 0); b-- )
    {
        STIM_serialbit((data >> b) & 0x01);
    }
}

/******************************************************************************/

void    STIM_select( int chip, BOOL select )
{
    if( (chip < STIM_CHIP_MIN) || (chip > STIM_CHIP_MAX) )
    {
        STIM_errorf("STIM_select(chip=%d,select=%s) Invalid chip.\n",chip,STR_YesNo(select));
        return;
    }

    STIM_debugf("STIM_select(chip=%d,select=%s)\n",chip,STR_YesNo(select));

    STIM_mask = select ? STIM_chip[chip-1] : 0x00;
    STIM_LPT(LPT_PORT_DATA,STIM_mask);
}

/******************************************************************************/

void    STIM_POT( int chip, BYTE potA, BYTE potB )
{
    STIM_debugf("STIM_POT(chip=%d,A=0x%02X,B=0x%02X)\n",chip,potA,potB);

//  First select chip by taking reset line high...
    STIM_select(chip,TRUE);

    TIMER_delay(STIM_HACK_DELAY); // Hack delay...

//  Send zero to stack-select bit (see DS1267 spec's)...
    STIM_serialbit(0);

//  Send values (8 bits each) for POTs A and B...
    STIM_serialbyte(potB);
    STIM_serialbyte(potA);

    TIMER_delay(STIM_HACK_DELAY); // Hack delay...

//  Now de-select chip...
    STIM_select(chip,FALSE);

    TIMER_delay(STIM_HACK_DELAY); // Hack delay...
}

/******************************************************************************/

void    STIM_amplitude( int CH1, int CH2, int CH3, int CH4 )
{
int     CH[STIM_CHANNEL_MAX];
int     channel=0;

    CH[channel++] = CH1;
    CH[channel++] = CH2;
    CH[channel++] = CH3;
    CH[channel++] = CH4;

    STIM_amplitude(CH);
}

/******************************************************************************/

void    STIM_amplitude( int CH[] )
{
int     channel,chip,pot;
BOOL    change;

//  Make sure interface is running (FALSE = don't wait for charging)...
    if( !STIM_check(FALSE) )
    {
        return;
    }

    for( chip=STIM_CHIP_MIN; (chip <= STIM_CHIP_MAX); chip++ )
    {
        for( change=FALSE,pot=0; (pot < STIM_CHIP_POT); pot++ )
        {
            channel = ((chip-1) * STIM_CHIP_POT) + pot;

            if( STIM_CH[channel] != CH[channel] )
            {
                STIM_CH[channel] = CH[channel];
                change = TRUE;
            }
        }

        if( !change )
        {
            continue;
        }

        channel = ((chip-1) * STIM_CHIP_POT);
        STIM_POT(chip,(BYTE)CH[channel+0],(BYTE)CH[channel+1]);
    }
}

/******************************************************************************/

void   STIM_pulse( void )
{
    STIM_pulse(STIM_PULSE_MIN);
}

/******************************************************************************/

void    STIM_pulse( double IPI )
{
BYTE    mask;

//  Make sure interface is running (TRUE = wait for charging)...
    if( !STIM_check(TRUE) )
    {
        return;
    }

//  Make sure we don't pulse above maximum frequency...
    while( !STIM_TIMER_INTERVAL.Expired(IPI) );

    STIM_TIMER_PULSE.Reset();

    mask = STIM_DATA_TRIGGER;     // Take trigger line high...
    STIM_LPT(LPT_PORT_DATA,mask);

    STIM_TIMER_INTERVAL.Reset();

    TIMER_delay(0.050);           // Hack delay for trigger pulse.

    mask = 0x00;                  // Reset trigger line low...
    STIM_LPT(LPT_PORT_DATA,mask);

    STIM_PWT += STIM_TIMER_PULSE.Elapsed();
    STIM_PC++;
}

/******************************************************************************/
/* Pulse Train variables and functions...                                     */
/******************************************************************************/

double  STIM_begin;                    // Time to start pulse train.
double  STIM_end;                      // Time to end pulse train.
double  STIM_frequency;                // Frequency of pulse train.
double  STIM_duration;                 // Duration.
int     STIM_mode=STIM_MODE_INIT;      // Pulse train mode (single or multiple).
int     STIM_fmod;                     // Frequency modulus.
long    STIM_count;                    // Loop counter.
long    STIM_pulses;                   // Pulse counter.

/******************************************************************************/

void    STIM_loop( void )
{
double  elapsed;

//  No trigger events pending...
    if( STIM_mode <= STIM_MODE_NONE )
    {
        return;
    }

    elapsed = STIM_TIMER_DURATION.Elapsed();

//  Check time to begin trigger pulse...
    if( elapsed < STIM_begin )
    {
        return;
    }

//  Single pulse? Do it now and get out...
    if( STIM_mode == STIM_MODE_SINGLE )
    {
        STIM_pulse();
        STIM_pulses++;

        STIM_mode = STIM_MODE_NONE;
        return;
    }

//  Check end time...
    if( elapsed >= STIM_end )
    {
        STIM_mode = STIM_MODE_NONE;
        return;
    }

//  Time pulse based on frequency...
    if( (STIM_count % STIM_fmod) == 0 )
    {
        STIM_pulse();
        STIM_pulses++;
    }

    STIM_count++;
}

/******************************************************************************/

void    STIM_trigger( int mode, double latency, double frequency, double duration )
{
    STIM_duration = 0.0;
    STIM_count = 0L;
    STIM_pulses = 0L;

//  Make sure interface is running (TRUE = wait for charging)...
    if( !STIM_check(TRUE) )
    {
        return;
    }

    if( STIM_mode == STIM_MODE_INIT )
    {
        if( !LOOPTASK_start(STIM_loop,STIM_LOOPTASK) )
        {
            STIM_errorf("STIM_trigger() Cannot start LOOPTASK.\n");
            return;
        }

        STIM_mode = STIM_MODE_NONE;
    }

    if( !LOOPTASK_fvalid(STIM_LOOPTASK,frequency) )
    {
        STIM_errorf("STIM_trigger() Frequency (%.0lf Hz) invalid.\n",frequency);
        return;
    }

    STIM_fmod = LOOPTASK_fmodulus(STIM_LOOPTASK,frequency);
    STIM_mode = mode;
    STIM_begin = latency;
    STIM_frequency = frequency;
    STIM_duration = duration;
    STIM_end = latency + duration;

    STIM_TIMER_DURATION.Reset();
}

/******************************************************************************/

void    STIM_single( double latency )
{
    STIM_trigger(STIM_MODE_SINGLE,latency,0.0,0.0);
}

/******************************************************************************/

void    STIM_multiple( double latency, double frequency, double duration )
{
    STIM_trigger(STIM_MODE_MULTIPLE,latency,frequency,duration);
}

/******************************************************************************/

BOOL    STIM_done( void )
{
BOOL    done;

    done = (STIM_mode <= STIM_MODE_NONE);

    return(done);
}

/******************************************************************************/

void    STIM_wait( void )
{
double  freq=0.0;

    while( !STIM_done() );

    if( STIM_duration != 0.0 )
    {
        freq = (double)STIM_pulses / STIM_duration * 1000.0;
        STIM_debugf("%ld pulse(s) in %.2lf msecs (%.1lf Hz).\n",STIM_pulses,STIM_duration,freq);
    }
}

/******************************************************************************/
/* STIMulator Trace - Monitor LPT port for debugging I/O.                     */
/******************************************************************************/

//##ModelId=3EC4C36902C9
struct  STIM_TraceTAG          
{
	//##ModelId=3EC4C36902DD
    double t;

	//##ModelId=3EC4C36902E7
    BYTE   data;
	//##ModelId=3EC4C36902F1
    BYTE   ctrl;
};

#define STIM_TRACE_MAX  10000
struct  STIM_TraceTAG  *STIM_TraceBuff=NULL;
int     STIM_TraceItem;
BOOL    STIM_TraceStarted=FALSE;
TIMER   STIM_TraceTimer("STIM_TraceTimer");
char   *STIM_TraceFile="STIM.TXT";

/******************************************************************************/

void    STIM_TraceStart( void )
{
    STIM_TraceTimer.Reset();

    if( (STIM_TraceBuff=(struct STIM_TraceTAG *)calloc(STIM_TRACE_MAX,sizeof(struct STIM_TraceTAG))) == NULL ) 
    {
        STIM_errorf("STIM_TraceStart() Cannot allocate memory.\n");
        return;
    }

    STIM_TraceItem = 0;
    STIM_TraceStarted = TRUE;

    STIM_debugf("STIM_TraceStart() OK.\n");

    return;
}

/******************************************************************************/

void    STIM_TraceLoop( void )
{
BYTE    data,ctrl;

    if( !STIM_TraceStarted )
    {
        return;
    }

    data = STIM_LPT_byte[LPT_PORT_DATA];
    ctrl = STIM_LPT_byte[LPT_PORT_CTRL];

    if( STIM_TraceItem >= STIM_TRACE_MAX )
    {
        return;
    }

    STIM_TraceBuff[STIM_TraceItem].t = STIM_TraceTimer.Elapsed();
    STIM_TraceBuff[STIM_TraceItem].data = data;
    STIM_TraceBuff[STIM_TraceItem].ctrl = ctrl;
    STIM_TraceItem++;
}

/******************************************************************************/

void    STIM_TraceStop( void )
{
int     item;
char   *d,*c,t,e;
BOOL    ok=TRUE;
FILE   *FP=NULL;

    if( !STIM_TraceStarted )
    {
        return;
    }

    STIM_TraceStarted = FALSE;

    if( (FP=fopen(STIM_TraceFile,"w")) == NULL )
    {
        STIM_errorf("STIM_TraceStop() Cannot open file %s\n",STIM_TraceFile);
        ok = FALSE;
    }

    for( item=0; ((item < STIM_TraceItem) && ok); item++ )
    {
        d = BIT_text(STIM_TraceBuff[item].data);
        c = BIT_text(STIM_TraceBuff[item].ctrl);

        t = (STIM_TraceBuff[item].data & STIM_DATA_TRIGGER) ? '1' : '0';
        e = (STIM_TraceBuff[item].ctrl & STIM_CTRL_ENABLE ) ? '1' : '0';

        ok = (fprintf(FP,"%10.6lf %-4.4s %c %c\n",STIM_TraceBuff[item].t / 1000.0,d,t,e) > 0);
    }

    if( ok )
    {
        STIM_debugf("STIM_TraceStop() %d record(s) written to %s\n",STIM_TraceItem,STIM_TraceFile);
    }

    if( FP != NULL )
    {
        fclose(FP);
    }

    free(STIM_TraceBuff);
    STIM_TraceBuff = NULL;

    STIM_debugf("STIM_TraceStop()\n");
}

/******************************************************************************/

