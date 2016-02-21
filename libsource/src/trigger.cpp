/******************************************************************************/
/*                                                                            */
/* MODULE  : TRIGGER.cpp                                                      */
/*                                                                            */
/* PURPOSE : Parallel port (LPT) Trigger API.                                 */
/*                                                                            */
/* DATE    : 22/Nov/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 22/Nov/2000 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 30/May/2003 - Use milliseconds rather than loop counts.          */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "TRIGGER"
#define MODULE_TEXT     "Parallel Port Trigger API"
#define MODULE_DATE     "06/01/2006"
#define MODULE_VERSION  "1.2"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

#include "sequencer.cpp"

/******************************************************************************/

int     TRIGGER_port=TRIGGER_PORT_INVALID;       // Parallel port number.
UCHAR   TRIGGER_data=0x00;                       // Current value at parallel port.
int     TRIGGER_ISA=LPT_INVALID;                 // ISA port address for LPT.
BOOL    TRIGGER_API_started=FALSE;
double  TRIGGER_frequency;
double  TRIGGER_period;
struct  TRIGGER_EventItem    TRIGGER_Event[TRIGGER_EVENTS];

/******************************************************************************/

PRINTF  TRIGGER_PRN_messgf=NULL;                 // General messages printf function.
PRINTF  TRIGGER_PRN_errorf=NULL;                 // Error messages printf function.
PRINTF  TRIGGER_PRN_debugf=NULL;                 // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     TRIGGER_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(TRIGGER_PRN_messgf,buff));
}

/******************************************************************************/

int     TRIGGER_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(TRIGGER_PRN_errorf,buff));
}

/******************************************************************************/

int     TRIGGER_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(TRIGGER_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    TRIGGER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok=TRUE;

    if( TRIGGER_API_started )               // Start the API once...
    {
        return(TRUE);
    }

    TRIGGER_PRN_messgf = messgf;            // General API message print function.
    TRIGGER_PRN_errorf = errorf;            // API error message print function.
    TRIGGER_PRN_debugf = debugf;            // Debug information print function.

    TRIGGER_init();

    if( !LOOPTASK_start(TRIGGER_task) )
    {
        TRIGGER_errorf("TRIGGER_API_start(...) Cannot start LOOPTASK.\n");
        ok = FALSE;
    }

    if( ok )
    {
        ATEXIT_API(TRIGGER_API_stop);       // Install stop function.
        TRIGGER_API_started = TRUE;         // Set started flag.

        TRIGGER_frequency = LOOPTASK_frequency(TRIGGER_task);
        TRIGGER_period = 1000.0 * (1.0 / TRIGGER_frequency);

        MODULE_start(TRIGGER_PRN_messgf);   // Register module.

        TRIGGER_debugf("Frequency %.1lf Hz, Period %.1lf msec.\n",TRIGGER_frequency,TRIGGER_period);
    }
    else
    {
        TRIGGER_errorf("TRIGGER_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    TRIGGER_API_stop( void )
{
    if( !TRIGGER_API_started )         // API not started in the first place...
    {
        return;
    }

    TRIGGER_close();

    LOOPTASK_stop(TRIGGER_task);

    TRIGGER_API_started = FALSE;       // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    TRIGGER_API_check( void )
{
BOOL    ok=TRUE;

    if( !TRIGGER_API_started )         // API not started...
    {                                  // Start module automatically...
        ok = TRIGGER_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        TRIGGER_debugf("TRIGGER_API_check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/******************************************************************************/

void    TRIGGER_init( void )
{
int     ID;

    for( ID=0; (ID < TRIGGER_EVENTS); ID++ )
    {
        memset(&TRIGGER_Event[ID],0x00,sizeof(struct TRIGGER_EventItem));
        TRIGGER_Event[ID].used = FALSE;
    }
}

/******************************************************************************/

int     TRIGGER_free( void )
{
int     ID;

    for( ID=0; ((ID < TRIGGER_EVENTS) && TRIGGER_Event[ID].used); ID++ );

    if( ID >= TRIGGER_EVENTS )
    {
        ID = TRIGGER_INVALID;
    }

    return(ID);
}

/******************************************************************************/

BOOL    TRIGGER_ID( int ID )
{
BOOL    ok=TRUE;

    if( !TRIGGER_API_check() )
    {
        ok = FALSE;
    }
    else
    if( ID == TRIGGER_INVALID )
    {
        ok = FALSE;
    }
    if( (ID < 0) || (ID >= TRIGGER_EVENTS) )
    {
        ok = FALSE;
    }
    else
    {
        ok = TRIGGER_Event[ID].used;
    }

    return(ok);
}

/******************************************************************************/

void    TRIGGER_put( UCHAR data )
{
    if( TRIGGER_port != TRIGGER_PORT_INVALID )
    {
        LPT_data(TRIGGER_port,data);
        TRIGGER_data = data;
    }
}

/******************************************************************************/

UCHAR   TRIGGER_get( void )
{
    return(TRIGGER_data);
}

/******************************************************************************/

void    TRIGGER_1( int channel )
{
UCHAR   chan,data,mask;

    chan = (1 << channel);
    data = TRIGGER_get();
    mask = data | chan;

    TRIGGER_put(mask);
    TRIGGER_debugf("TRIGGER_1(chan=%d) port=%d(0x%04X) mask=0x%02X\n",channel,TRIGGER_port,TRIGGER_ISA,mask);
}

/******************************************************************************/

void    TRIGGER_0( int channel )
{
UCHAR   chan,data,mask;

    chan = (1 << channel);
    data = TRIGGER_get();
    mask = data & ~chan;

    TRIGGER_put(mask);
    TRIGGER_debugf("TRIGGER_0(chan=%d) port=%d(0x%04X) mask=0x%02X\n",channel,TRIGGER_port,TRIGGER_ISA,mask);
}

/******************************************************************************/

BOOL    TRIGGER_state( int channel )
{
UCHAR   mask,chan;

    chan = (1 << channel);
    mask = TRIGGER_get();

    return((mask & chan) != 0);
}

/******************************************************************************/

BOOL    TRIGGER_status( int ID )
{
BOOL    state,status;

    if( !TRIGGER_ID(ID) )
    {
        return(FALSE);
    }

    state = TRIGGER_state(TRIGGER_Event[ID].channel);

    switch( TRIGGER_Event[ID].pulse_type )
    {
        case TRIGGER_PULSE_LO2HI :
           status = state;
           break;

        case TRIGGER_PULSE_HI2LO :
           status = !state;
           break;
    }

    return(status);
}

/******************************************************************************/

void    TRIGGER_on( int ID )
{
    if( !TRIGGER_ID(ID) )
    {
        return;
    }

    switch( TRIGGER_Event[ID].pulse_type )
    {
        case TRIGGER_PULSE_LO2HI :
           TRIGGER_1(TRIGGER_Event[ID].channel);
           break;

        case TRIGGER_PULSE_HI2LO :
           TRIGGER_0(TRIGGER_Event[ID].channel);
           break;
    }

    TRIGGER_Event[ID].state = ON;
}

/******************************************************************************/

void    TRIGGER_off( int ID )
{
    if( !TRIGGER_ID(ID) )
    {
        return;
    }

    switch( TRIGGER_Event[ID].pulse_type )
    {
        case TRIGGER_PULSE_LO2HI :
           TRIGGER_0(TRIGGER_Event[ID].channel);
           break;

        case TRIGGER_PULSE_HI2LO :
           TRIGGER_1(TRIGGER_Event[ID].channel);
           break;
    }

    TRIGGER_Event[ID].state = OFF;
}

/******************************************************************************/

void    TRIGGER_task( void )
{
int     ID;
BOOL    state;

    // Loop for each trigger event
    for( ID=0; (ID < TRIGGER_EVENTS); ID++ )
    {
        // Check if event is currently in use...
        if( !TRIGGER_Event[ID].used )
        {
            continue;
        }

        // Check if event is currently running...
        if( !TRIGGER_Event[ID].running )
        {
            continue;
        }

        // Save current state of event...
        state = TRIGGER_Event[ID].state;

        // If not generating a pulse, check if it's time to start one...
        if( !TRIGGER_Event[ID].state )
        {
            if( TRIGGER_Event[ID].tmark == 0.0 )
            {   // ON trigger when mark is zero...
                state = ON;
            }
            else
            if( TRIGGER_Event[ID].tmark >= TRIGGER_Event[ID].period )
            {   // ON trigger subsequently when mark equals the period...
                state = ON;
                TRIGGER_Event[ID].tmark = 0.0;
            }
        }

        // If generating a pulse, check if it's time to stop it...
        if( TRIGGER_Event[ID].state )
        {
            if( TRIGGER_Event[ID].tmark >= TRIGGER_Event[ID].width )
            {   // OFF trigger when mark equals the pulse width...
                state = OFF;
            }
        }

        // Increment msec counter...
        TRIGGER_Event[ID].tmark += TRIGGER_period;

        // Increment loop counter...
        TRIGGER_Event[ID].loop++;

        // Check if event processing resulted in a change in state...
        if( state == TRIGGER_Event[ID].state )
        {
            continue;
        }

        // Process state change...

        // Turn pulse on...
        if( state )
        {
            TRIGGER_on(ID);
            TRIGGER_Event[ID].pulses++;
            continue;
        }

        // Turn pulse off...
        TRIGGER_off(ID);

        // Check number of pulses...
        if( TRIGGER_Event[ID].pulses == TRIGGER_Event[ID].total )
        {
            // What do we do now that the event has finished...
            switch( TRIGGER_Event[ID].reset_type )
            {
                case TRIGGER_RESET_SINGLE :
                   // Single, so kill the event...
                   TRIGGER_Event[ID].kill = TRUE;
                   break;

                case TRIGGER_RESET_MULTIPLE :
                   // Multiple, so keep the event...
                   TRIGGER_Event[ID].running = FALSE;
                   break;
            }
        }

        // Kill event if required...
        if( TRIGGER_Event[ID].kill )
        {
            TRIGGER_stop(ID);
        }
    }
}

/******************************************************************************/

BOOL    TRIGGER_open( int port )
{
BOOL    ok=TRUE;
UCHAR   data;

    // Make sure API is running.
    if( !TRIGGER_API_check() )
    {
        return(FALSE);
    }

    // Check if TRIGGER port is already open.
    if( TRIGGER_port != TRIGGER_PORT_INVALID )
    {
        return(TRUE);
    }

    if( (TRIGGER_ISA=LPT_base(port)) == LPT_INVALID )
    {
        TRIGGER_errorf("TRIGGER_open(port=%d) Invalid port number.\n",port);
        ok = FALSE;
    }
    else
    {
        TRIGGER_port = port;

        TRIGGER_debugf("TRIGGER_open(port=%d) ISA=0x%04X.\n",port,TRIGGER_ISA);

        // Clear channels...
        TRIGGER_put(0x00);
    }

    return(ok);
}

/******************************************************************************/

void    TRIGGER_close( void )
{
    if( !TRIGGER_API_check() )         // Make sure API is running.
    {
        return;
    }

    TRIGGER_stopall();

    TRIGGER_port = TRIGGER_PORT_INVALID;
    TRIGGER_ISA = LPT_INVALID;
}

/******************************************************************************/

int     TRIGGER_setup( int channel, int pulse_type, int reset_type, double width_msec, double period_msec, int pulses )
{
int     ID;

    if( TRIGGER_port == TRIGGER_PORT_INVALID )
    {
        TRIGGER_errorf("TRIGGER_setup(...) Invalid ISA port.\n");
        return(TRIGGER_INVALID);
    }

    // Get a free event handle...
    if( (ID=TRIGGER_free()) == TRIGGER_INVALID )
    {
        TRIGGER_errorf("TRIGGER_setup(...) No free handles.\n");
        return(TRIGGER_INVALID);
    }

    // Set values for event...
    memset(&TRIGGER_Event[ID],0x00,sizeof(struct TRIGGER_EventItem));
    TRIGGER_Event[ID].used = TRUE;
    TRIGGER_Event[ID].running = FALSE;
    TRIGGER_Event[ID].kill = FALSE;
    TRIGGER_Event[ID].channel = channel;
    TRIGGER_Event[ID].pulse_type = pulse_type;
    TRIGGER_Event[ID].reset_type = reset_type;
    TRIGGER_Event[ID].tmark = 0.0;
    TRIGGER_Event[ID].width = width_msec;
    TRIGGER_Event[ID].period = period_msec;
    TRIGGER_Event[ID].loop = 0;
    TRIGGER_Event[ID].pulses = 0;
    TRIGGER_Event[ID].total = (pulses == 0) ? 1 : pulses;

//  TRIGGER_0(ID);
//  TRIGGER_Event[ID].state = TRIGGER_status(ID);

    // Make sure channel is in OFF state...
    TRIGGER_off(ID);

    TRIGGER_debugf("TRIGGER_setup(ch=%d,pulse=%d,reset=%d,width=%.0lf,period=%.0lf,pulses=%ld) ID=%d\n",
                    channel,
                    pulse_type,
                    reset_type,
                    width_msec,
                    period_msec,
                    pulses,
                    ID);

    return(ID);
}

/******************************************************************************/

int     TRIGGER_setup( int port, int channel, int pulse_type, int reset_type, double width_msec, double period_msec, int pulses )
{
int     ID=TRIGGER_INVALID;

    if( TRIGGER_open(port) )
    {
        ID = TRIGGER_setup(channel,pulse_type,reset_type,width_msec,period_msec,pulses);
    }

    return(ID);
}

/******************************************************************************/

int     TRIGGER_setup( int channel, int pulse_type, int reset_type, double width_msec )
{
int     ID;

    ID = TRIGGER_setup(channel,pulse_type,reset_type,width_msec,0.0,0);

    return(ID);
}

/******************************************************************************/

int     TRIGGER_setup( int port, int channel, int pulse_type, int reset_type, double width_msec )
{
int     ID;

    ID = TRIGGER_setup(port,channel,pulse_type,reset_type,width_msec,0.0,0);

    return(ID);
}

/******************************************************************************/

int     TRIGGER_start( int channel, int pulse_type, int reset_type, double width_msec, double period_msec, int pulses )
{
int     ID;

    if( (ID=TRIGGER_setup(channel,pulse_type,reset_type,width_msec,period_msec,pulses)) == TRIGGER_INVALID )
    {
        return(TRIGGER_INVALID);
    }

    TRIGGER_start(ID);

    return(ID);
}

/******************************************************************************/

int     TRIGGER_start( int channel, double width_msec )
{
int     ID;

    ID = TRIGGER_start(channel,TRIGGER_PULSE_LO2HI,TRIGGER_RESET_SINGLE,width_msec,0.0,0);

    return(ID);
}

/******************************************************************************/

void     TRIGGER_start( int ID )
{
    if( !TRIGGER_ID(ID) )
    {
        return;
    }

    if( TRIGGER_Event[ID].running )
    {
        return;
    }

    TRIGGER_Event[ID].running = TRUE;
    TRIGGER_Event[ID].tmark = 0.0;
    TRIGGER_Event[ID].loop = 0;
    TRIGGER_Event[ID].pulses = 0;
}

/******************************************************************************/

void     TRIGGER_stop( int ID )
{
    if( !TRIGGER_ID(ID) )
    {
        return;
    }

    TRIGGER_Event[ID].used = FALSE;

    TRIGGER_debugf("TRIGGER_stop(ID=%d)\n",ID);
}

/******************************************************************************/

void     TRIGGER_stopall( void )
{
int      ID;

    for( ID=0; (ID < TRIGGER_EVENTS); ID++ )
    {
        if( TRIGGER_Event[ID].used )
        {
            TRIGGER_stop(ID);
        }
    }
}

/******************************************************************************/

BOOL    TRIGGER_running( int ID )
{
BOOL    running=FALSE;

    if( TRIGGER_ID(ID) )
    {
        running = TRIGGER_Event[ID].running;
    }

    return(running);
}

/******************************************************************************/

void    TRIGGER_pulse( int channel, double width_msec )
{
int     ID;

    ID = TRIGGER_start(channel,width_msec);
}

/******************************************************************************/

void    TRIGGER_pulse( int port, int channel, double width_msec )
{
    if( TRIGGER_open(port) )
    {
        TRIGGER_pulse(channel,width_msec);
    }
}

/******************************************************************************/

