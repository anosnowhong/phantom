/******************************************************************************/
/*                                                                            */
/* MODULE  : SNMS.cpp                                                         */
/*                                                                            */
/* PURPOSE : Stanmore NeuroMuscular Stimulator API                            */
/*                                                                            */
/* DATE    : 28/Dec/2001                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 28/Dec/2001 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "SNMS"
#define MODULE_TEXT     "Stanmore NM Stimulator API"
#define MODULE_DATE     "28/12/2001"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL    2

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

int     SNMS_comH=COM_INVALID;
BYTE    SNMS_Pakt[SNMS_PACKET_MAX];
TIMER   SNMS_timer("SNMS_timer",TIMER_MODE_RESET);

/******************************************************************************/

BOOL    SNMS_API_started = FALSE;

/******************************************************************************/

PRINTF  SNMS_PRN_messgf=NULL;                   // General messages printf function.
PRINTF  SNMS_PRN_errorf=NULL;                   // Error messages printf function.
PRINTF  SNMS_PRN_debugf=NULL;                   // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     SNMS_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(SNMS_PRN_messgf,buff));
}

/******************************************************************************/

int     SNMS_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(SNMS_PRN_errorf,buff));
}

/******************************************************************************/

int     SNMS_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(SNMS_PRN_debugf,buff));
}

/******************************************************************************/

void    SNMS_API_stop( void )
{
    if( !SNMS_API_started  )           // API not started in the first place...
    {
         return;
    }

    SNMS_API_started = FALSE;          // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    SNMS_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
    if( SNMS_API_started )                  // Start the API once...
    {
        return(TRUE);
    }

    // Set API print functions...
    SNMS_PRN_messgf = messgf;               // General API message print function.
    SNMS_PRN_errorf = errorf;               // API error message print function.
    SNMS_PRN_debugf = debugf;               // Debug information print function.

    // Initialize variables...
    SNMS_comH = COM_INVALID;

    // Standard API stuff...
    atexit(SNMS_API_stop);                  // Install stop function.
    SNMS_API_started = TRUE;                // Set started flag.
    MODULE_start(SNMS_PRN_messgf);          // Register module.

    return(TRUE);
}

/******************************************************************************/

BOOL    SNMS_API_check( void )
{
BOOL    ok=TRUE;

    if( SNMS_API_started )             // API already started...
    {
        return(TRUE);
    }

    // Not aready started, so start API now...
    ok = SNMS_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
    SNMS_debugf("SNMS_API_check() Start %s.\n",ok ? "OK" : "Failed");

    return(ok);
}

/******************************************************************************/

void    SNMS_DCB( DCB *dcb )
{
    dcb->ByteSize = 8;
    dcb->Parity = NOPARITY;
    dcb->StopBits = ONESTOPBIT;
    dcb->fAbortOnError = TRUE;
    dcb->fDtrControl = DTR_CONTROL_DISABLE;
    dcb->fDsrSensitivity = FALSE;
    dcb->fOutxDsrFlow = FALSE;
    dcb->fOutxCtsFlow = FALSE;
    dcb->fRtsControl = RTS_CONTROL_DISABLE;
}

/******************************************************************************/

#define SNMS_OPEN_COM        0
#define SNMS_OPEN_SETUP      1
#define SNMS_OPEN_LABMODE    2

char   *SNMS_OpenStep[] = { "OPEN RS232","CONFIGURE RS232","LABMODE" };

BOOL    SNMS_Open( int comX, BYTE channels[] )
{
BOOL    ok;
int     step,last;

    // First check if SNMS is already open...
    if( SNMS_Check() )
    {
        return(TRUE);
    }

    last = (channels != NULL) ? SNMS_OPEN_LABMODE : SNMS_OPEN_SETUP;

    // Perform various steps to open SNMS...
    for( ok=TRUE,step=SNMS_OPEN_COM; ((step <= last) && ok); step++ )
    {
        switch( step )
        {
            case SNMS_OPEN_COM :       // Open SNMS RS232 port...
               ok = ((SNMS_comH=COM_Open(comX)) != COM_INVALID);
               break;

            case SNMS_OPEN_SETUP :     // Set COM parameters...
               ok = COM_Setup(SNMS_comH,SNMS_BAUD,SNMS_DCB);
               break;

            case SNMS_OPEN_LABMODE :   // Enter LabMode...
               ok = SNMS_LabMode(channels);
               break;
        }

        STR_printf(ok,SNMS_errorf,SNMS_debugf,"SNMS_Open(COM%d,...) %s %s.\n",comX,SNMS_OpenStep[step],STR_OkFailed(ok));
    }

    // Something failed, so close up...
    if( !ok )
    {
        SNMS_Close();
    }

    return(ok);
}

/******************************************************************************/

BOOL    SNMS_Open( int comX )
{
BOOL    ok;

    ok = SNMS_Open(comX,NULL);

    return(ok);
}

/******************************************************************************/

void    SNMS_Close( BOOL Exit )
{
BOOL    ok;

    if( SNMS_Check() )                 // SNMS is open, so close it...
    {
        if( Exit )
        {
            ok = SNMS_Escape();        // Exit SNMS LabMode...
        }

        ok = COM_Close(SNMS_comH);     // Close RS232 port...
        SNMS_comH = COM_INVALID;
    }
}

/******************************************************************************/

void    SNMS_Close( void )
{
    SNMS_Close(TRUE);
}

/******************************************************************************/

BOOL    SNMS_Check( void )
{
BOOL    ok=TRUE;

    if( !SNMS_API_check() )
    {
        ok = FALSE;
    }
    else
    if( SNMS_comH == COM_INVALID )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    SNMS_Send( BYTE *buff, int size )
{
BOOL    ok;
int     b;

    if( !SNMS_Check() )
    {
        return(FALSE);
    }

    if( (ok=COM_Write(SNMS_comH,buff,size)) )
    {
        SNMS_debugf("SNMS_Send(...)");

        for( b=0; (b < size); b++ )
        {
SNMS_debugf(" [%s]",BIT_text(buff[b]));
//            SNMS_debugf(" 0x%02X",buff[b]);
//            if( isprint(buff[b]) ) SNMS_debugf("[%c]",buff[b]);
        }

        SNMS_debugf("\n");
    }

    return(ok);
}

/******************************************************************************/

BOOL    SNMS_Recv( void )
{
BOOL    ok=TRUE,complete;
BYTE    byte;
int     got;

    if( !SNMS_Check() )
    {
        return(FALSE);
    }

    if( (ok=COM_Read(SNMS_comH,&byte,1,got,complete,0L)) )
    {
        if( complete )
        {
            SNMS_debugf("RECV 0x%02X",byte);
            if( isprint(byte) ) SNMS_debugf("[%c]",byte);
            SNMS_debugf("\n");
        }
    }

    return(ok);
}

/******************************************************************************/

void    SNMS_Proc( void )
{
BOOL    ok;
BYTE    null=0x00;

    ok = SNMS_Recv();

    if( SNMS_timer.Expired(500.0) )
    {
//      ok = SNMS_Send(&null,1);
    }
}

/******************************************************************************/

BYTE   *SNMS_Packet( int size )
{
int     p;

    for( p=0; (p < size); p++ )
    {
        SNMS_Pakt[p] = 0x00;
    }

    return(SNMS_Pakt);
}

/******************************************************************************/

BOOL    SNMS_Pulse( int channel, int current, int width, int units )
{
BOOL    ok;
BYTE   *pakt;
int     p,wh5,wl6,ch3,w=0,charge;

    // 3 bit version of channel number...
    ch3 = (channel & BITS_3);

    // Divide pulse width into 6 low and 5 high bits...
    wl6 = (width & BITS_6);
    wh5 = (width >> 6) & BITS_5;

    // Calculate pulse width in 4 microsecond units...
    switch( units )
    {
        case SNMS_WIDTH_UNITS_HALFMICROSEC :
           w = width;
           break;

        case SNMS_WIDTH_UNITS_4MICROSEC :
           w = width / 8;
           break;
    }

    // Calculate "charge" field...
    charge = ((w / 8) * current) / 256;

    // "charge" is represented by 3 bits...
    if( charge > 7 )
    {
        charge = 7;
    }

    // Get a clean packet buffer...
    pakt = SNMS_Packet(SNMS_PACKET_PULSE);

    // Construct 4-byte pulse packet...
    pakt[0] |= (ch3 << 4);                  // Channel number in bits 6,5,4...
    pakt[0] |= (current & BITS_4);          // Current in bits 3,2,1,0...

    pakt[1] |= SNMS_LABMODE_PULSE;          // Set bit 6 to indicate pulse packet...
    pakt[1] |= units;                       // Pulse width units...
    pakt[1] |= wh5;                         // High 5 bits of pulse width...

    pakt[2] |= SNMS_LABMODE_PULSE;          // Set bit 6 to indicate pulse packet...
    pakt[2] |= wl6;                         // Low 6 bits of pulse width...

    pakt[3] |= SNMS_PACKET_TERMINATE;       // Set bit 7 to terminate packet...
    pakt[3] |= SNMS_LABMODE_PULSE;          // Set bit 6 to indicate pulse packet...
    pakt[3] |= (charge << 3);               // Bits 5,4,3 set to "charge" (current * pulse width)...
    pakt[3] |= ch3;                         // Bits 2,1,0 copy of channel number...

    // Send packet...
    ok = SNMS_Send(pakt,SNMS_PACKET_PULSE);

    return(ok);
}

/******************************************************************************/

void    SNMS_PulseError( int channel, float current_mAmp, float width_msec, char *text )
{
    SNMS_errorf("SNMS_Pulse(channel=%d,current=%.2f(mAmp),width=%.2f(msec)) %s.\n",channel,current_mAmp,width_msec,text);
}

/******************************************************************************/

BOOL    SNMS_Pulse( int channel, float current_mAmp, float width_msec )
{
BOOL    ok;
int     I=0,w=0,u;

    // Check parameters first so we don't fry people...
    if( (current_mAmp == 0.0) || (width_msec == 0.0) )
    {
        return(TRUE);
    }

    // Check range of pulse width...
    if( (width_msec < 0.0) || (width_msec > SNMS_WIDTH_MAX_4MICROSEC) )
    {
        SNMS_PulseError(channel,current_mAmp,width_msec,"Invalid pulse width");
        return(FALSE);
    }

    // Check range of channel...
    if( (channel < 0) || (channel > SNMS_CHANNELS) )
    {
        SNMS_PulseError(channel,current_mAmp,width_msec,"Invalid channel");
        return(FALSE);
    }

    // Pulse width, select units...
    if( width_msec < SNMS_WIDTH_MAX_HALFMICROSEC )
    {   // Convert to half microsecond units...
        w = (int)(width_msec / 0.5E-3);
        u = SNMS_WIDTH_UNITS_HALFMICROSEC;
    }
    else
    if( width_msec < SNMS_WIDTH_MAX_4MICROSEC )
    {   // Convert to 4 microsecond units...
        w = (int)(width_msec / 4.0E-3);
        u = SNMS_WIDTH_UNITS_4MICROSEC;
    }

    // Pulse current...
    I = (int)(current_mAmp * SNMS_CURRENT_MAMP2INT);

    if( (I <= 0) || (I > SNMS_CURRENT_MAX_INT) )
    {
        SNMS_PulseError(channel,current_mAmp,width_msec,"Invalid current.\n");
        return(FALSE);
    }

    // Do the pulse...
    ok = SNMS_Pulse(channel,I,w,u);

    return(ok);    
}

/******************************************************************************/

#define SNMS_LABMODE_START        0
#define SNMS_LABMODE_CHANNELS     1
#define SNMS_LABMODE_REPLY        2

#define SNMS_LABMODE_RECV        32

BOOL    SNMS_LabMode( BYTE channels[] )
{
BOOL    ok,complete;
int     stage,got;
char   *labmode="LABMODE\r";
BYTE   *pakt,buff[SNMS_LABMODE_RECV+1];

    for( ok=TRUE,stage=SNMS_LABMODE_START; ((stage <= SNMS_LABMODE_REPLY) && ok); stage++ )
    {
        switch( stage )
        {
            case SNMS_LABMODE_START :       // Send LabMode start string...
               ok = SNMS_Send((BYTE *)labmode,strlen(labmode));
               break;

            case SNMS_LABMODE_CHANNELS :    // Send 3-byte LabMode channel-select packet...
               pakt = SNMS_Packet(SNMS_PACKET_LABMODE);    // Get packet buffer...

               pakt[0] = SNMS_LABMODE_START_FCB;           // First Compulsory Byte...
               pakt[1] = BIT_byte(channels);               // Bit mask for active channels...
               pakt[2] = SNMS_LABMODE_START_SCB;           // Second Compulsory Byte...

               ok = SNMS_Send(pakt,SNMS_PACKET_LABMODE);   // Send packet...
               break;

            case SNMS_LABMODE_REPLY :       // Get reply back from stimulator...
               if( !(ok=COM_Read(SNMS_comH,buff,SNMS_LABMODE_RECV,got,complete,500L)) )
               {
                   break;
               }

               if( got == 0 )               // Nothing came back...
               {
                   SNMS_errorf("SNMS_LabMode(...) No reply.\n");
                   ok = FALSE;
                   break;
               }

               buff[got] = 0x00;

               if( strstr(_strupr((char *)buff),"ERROR") )
               {                            // The word "ERROR" in the reply is bad news...
                   SNMS_errorf("SMNM_LabMode(...) Error.\n");
                   SNMS_errorf("%s\n",buff);
                   ok = FALSE;
               }
               break;
        }

        if( ok && (stage < SNMS_LABMODE_REPLY) )
        {
            TIMER_delay(100.0);
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    SNMS_Escape( void )
{
BOOL    ok;
BYTE   *pakt;

    // Get a clean packet buffer...
    pakt = SNMS_Packet(SNMS_PACKET_CONTROL);

    // Construct 2-byte Escape (control) packet...
    pakt[0] = SNMS_CONTROL_ESCAPE_1;
    pakt[1] = SNMS_CONTROL_ESCAPE_2;

    // Send packet...
    ok = SNMS_Send(pakt,SNMS_PACKET_CONTROL);

    return(ok);
}

/******************************************************************************/




