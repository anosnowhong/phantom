/******************************************************************************/
/*                                                                            */
/* MODULE  : CONTROLLER.cpp                                                   */
/*                                                                            */
/* PURPOSE : Robot controller interface.                                      */
/*                                                                            */
/* DATE    : 16/Aug/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 16/Aug/2002 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 26/Apr/2007 - Sensoray encoder functions to SENSORAY module.     */
/*                                                                            */
/* V1.2  JNI 07/Nov/2007 - Extra safety feature which makes sure a robot can  */
/*                         not start while its activation switch is engaged.  */
/*                                                                            */
/* V1.3  JNI 06/Jun/2009 - Added OptoTrak as a host-card type.                */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "CONTROLLER"
#define MODULE_TEXT     "Motor/Encoder Controller API"
#define MODULE_DATE     "06/06/2009"
#define MODULE_VERSION  "1.3"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

struct  STR_TextItem  CONTROLLER_HostCardText[] = 
{
    { CONTROLLER_HOSTCARD_PHANTOMISA,"PhantomISA" },
    { CONTROLLER_HOSTCARD_SENSORAY,"Sensoray" },
    { CONTROLLER_HOSTCARD_SIMULATE,"Simulate" },
    { CONTROLLER_HOSTCARD_MOUSE,"Mouse" },
    { CONTROLLER_HOSTCARD_OPTOTRAK,"OptoTrak" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem  CONTROLLER_ControllerTypeText[] = 
{
    { CONTROLLER_TYPE_NONE,"NONE" },
    { CONTROLLER_TYPE_SMALL,"SMALL" },
    { CONTROLLER_TYPE_BIG,"BIG" },
    { CONTROLLER_TYPE_GENERAL,"GENERAL" },
    { CONTROLLER_TYPE_VBOT,"VBOT" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

BOOL    CONTROLLER_API_started = FALSE;

/******************************************************************************/

PRINTF  CONTROLLER_PRN_messgf=NULL;         // General messages printf function.
PRINTF  CONTROLLER_PRN_errorf=NULL;         // Error messages printf function.
PRINTF  CONTROLLER_PRN_debugf=NULL;         // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     CONTROLLER_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(CONTROLLER_PRN_messgf,buff));
}

/******************************************************************************/

int     CONTROLLER_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(CONTROLLER_PRN_errorf,buff));
}

/******************************************************************************/

int     CONTROLLER_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(CONTROLLER_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    CONTROLLER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok=TRUE;

    if( CONTROLLER_API_started )               // Start the API once...
    {
        return(TRUE);
    }

    CONTROLLER_PRN_messgf = messgf;            // General API message print function.
    CONTROLLER_PRN_errorf = errorf;            // API error message print function.
    CONTROLLER_PRN_debugf = debugf;            // Debug information print function.

    if( ok )
    {
        ATEXIT_API(CONTROLLER_API_stop);       // Install stop function.
        CONTROLLER_API_started = TRUE;         // Set started flag.

        MODULE_start(CONTROLLER_PRN_messgf);   // Register module.
    }
    else
    {
        CONTROLLER_errorf("CONTROLLER_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    CONTROLLER_API_stop( void )
{
int     ID;

    if( !CONTROLLER_API_started )      // API not started in the first place...
    {
         return;
    }

    CONTROLLER_API_started = FALSE;    // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL CONTROLLER_API_check( void )
{
BOOL ok=TRUE;

    if( !CONTROLLER_API_started )      // API not started...
    {                                  // Start module automatically...
        ok = CONTROLLER_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        CONTROLLER_debugf("CONTROLLER_API_check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/******************************************************************************/

CONTROLLER::CONTROLLER( int card, DWORD address, int controller, char *name )
{
    Init(card,address,controller,name);
}

/******************************************************************************/

CONTROLLER::CONTROLLER( int card, DWORD address, int subaddress, int controller, char *name )
{
    Init(card,address,subaddress,controller,name);
}

/******************************************************************************/

CONTROLLER::~CONTROLLER( void )
{
    // Make sure controller type is closed...
    Close();

    if( Phantom != NULL )
    {
        delete Phantom;
        Phantom = NULL;
    }

    if( Sensoray != NULL )
    {
        delete Sensoray;
        Sensoray = NULL;
    }
}

/******************************************************************************/

void    CONTROLLER::Init( int card, DWORD address, int controller, char *name )
{
    Init(card,address,0,controller,name);
}

/******************************************************************************/

void    CONTROLLER::Init( int card, DWORD address, int subaddress, int controller, char *name )
{
    // Make sure API started...
    if( !CONTROLLER_API_check() )
    {
        return;
    }

    // Set variables...
    HostCard = card;
    Address = address;
    SubAddress = subaddress;
    ControllerType = controller;
    OpenFlag = FALSE;
    StartedFlag = FALSE;
    LoopTaskFlag = FALSE;
    ActivatedFlag = FALSE;
    strncpy(ObjectName,name,STRLEN);
    //printf("CONTROLLER::Init(card=[%d],address=0x%05X,subaddress=0x%02X,controller=[%d],name=%s)\n",card,address,subaddress,controller,name);

    ActivatedLast[CONTROLLER_ACTIVATED_NO2YES] = FALSE;
    ActivatedLast[CONTROLLER_ACTIVATED_YES2NO] = FALSE;

    Sensoray = NULL;
    Phantom = NULL;

    // Latency timers for various I/O functions...
    LatencyEncoder = NULL;
    LatencyMotor = NULL;
    LatencyActivated = NULL;
    LatencySafe = NULL;

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           Phantom = new PHANTOMISA((int)address,name);
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           Sensoray = new SENSORAY(address,subaddress,name);
           break;
    }
}

/******************************************************************************/

BOOL    CONTROLLER::Open( void )
{
BOOL    ok=FALSE;

    if( OpenFlag )
    {
        return(TRUE);
    }

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           ok = Phantom->Open();
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           ok = Sensoray->Open();
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           ok = TRUE;
           break;
    }

    // Reset controller timer.
    Timer.Reset();

    STR_printf(ok,CONTROLLER_debugf,CONTROLLER_errorf,"CONTROLLER::Open() %s.\n",STR_OkFailed(ok));

    if( ok )
    {
        OpenFlag = TRUE;
        Stop();
    }

    return(ok);
}

/******************************************************************************/

void    CONTROLLER::Close( void )
{
BOOL    ok;

    if( !OpenFlag )
    {
        return;
    }

    if( StartedFlag )
    {
        ok = Stop();
    }

    OpenFlag = FALSE;

    // Close latency timers...
    LatencyClose();

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           Phantom->Close();
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           Sensoray->Close();
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           break;
    }
}

/******************************************************************************/

char   *CONTROLLER::Name( void )
{
    return(ObjectName);
}

/******************************************************************************/

char   *CONTROLLER::HostCardText( void )
{
char   *text;

    text = STR_TextCode(CONTROLLER_HostCardText,HostCard);

    return(text);
}

/******************************************************************************/

char   *CONTROLLER::ControllerTypeText( void )
{
char   *text;

    text = STR_TextCode(CONTROLLER_ControllerTypeText,ControllerType);

    return(text);
}

/******************************************************************************/

int CONTROLLER::HostCardType( void )
{
int type;

    type = HostCard;

    return(type);
}

/******************************************************************************/

int CONTROLLER::ControllerTypeType( void )
{
int type;

    type = ControllerType;

    return(type);
}

/******************************************************************************/

BOOL CONTROLLER::ControllerHardware( BOOL flag )
{
BOOL ok=FALSE;

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           ok = PhantomController(flag);
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           ok = SensorayController(flag);
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           ok = TRUE;
           break;
    }

    return(ok);
}

/******************************************************************************/

BOOL CONTROLLER::Controller( BOOL flag )
{
BOOL ok=FALSE,activated;
TIMER WaitTimer;
double wait;

    if( !OpenFlag )
    {
        return(FALSE);
    }

    // Make sure we have to do something.
    if( flag == StartedFlag )
    {
        return(TRUE);
    }

    // Reset various flags.
    LoopTaskFlag = FALSE;
    ActivatedFlag = FALSE;

    // Perform the hardware part of the function.
    ok = ControllerHardware(flag);

    // If turning controller on, check if activation switch engaged (V1.2).
    if( ok && flag )
    {
        WaitTimer.Reset();

        do
        {
            TIMER_delay(1.0);
            activated = Activated();
            wait = WaitTimer.ElapsedSeconds();
        }
        while( (wait <= 1.0) && !activated );

        if( activated )
        {
            CONTROLLER_errorf("CONTROLLER::Controller(ON) Activation switch must be released (%.2lf sec).\n",wait);
            CONTROLLER_errorf("CONTROLLER::Controller(ON) Press ESCape to abort.\n");

            while( activated && !KB_ESC() )
            {
                TIMER_delay(1.0);
                activated = Activated();
            }

            if( activated )
            {
                ControllerHardware(OFF);
                ok = FALSE;
            }
        }
    }

    if( ok )
    {
        StartedFlag = flag;
    }
    else
    {
        StartedFlag = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL CONTROLLER::Start( void )
{
BOOL ok=TRUE;

    ok = Controller(ON);

    STR_printf(ok,CONTROLLER_debugf,CONTROLLER_errorf,"CONTROLLER::Start() %s.\n",STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    CONTROLLER::Stop( void )
{
BOOL    ok=TRUE;

    ok = Controller(OFF);

    return(ok);
}

/******************************************************************************/

BOOL    CONTROLLER::Started( void )
{
BOOL    flag=FALSE;

    if( OpenFlag )
    {
        flag = StartedFlag;
    }

    return(flag);
}

/******************************************************************************/

BOOL CONTROLLER::Safe( void )
{
BOOL safe=FALSE;

    if( OpenFlag )
    {
        if( LoopTaskFlag )
        {
            safe = SafeIndirect();
        }
        else
        {
            safe = SafeDirect();
        }
    }

    return(safe);
}

/******************************************************************************/

BOOL CONTROLLER::SafeIndirect( void )
{
BOOL safe=FALSE;

    if( OpenFlag )
    {
        safe = SafeFlag;
    }

    return(safe);
}

/******************************************************************************/

BOOL CONTROLLER::SafeDirect( void )
{
BOOL safe=FALSE;
int i,c;

    if( !OpenFlag )
    {
        return(FALSE);
    }

    LatencyBefore(LatencySafe);

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           if( !(safe=PhantomSafe()) )
           {
               safe = PhantomSafe();
           }
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           if( !(safe=SensoraySafe()) )
           {
               if( !(safe=SensoraySafe()) )
               {
                   safe = SensoraySafe();
               }
           }
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           safe = TRUE;
           break;
    }

    LatencyAfter(LatencySafe);

    return(safe);
}

/******************************************************************************/

void CONTROLLER::LoopTask( void )
{
    LoopTaskFlag = TRUE;

    ActivatedFlag = ActivatedDirect();
    SafeFlag = SafeDirect();
}

/******************************************************************************/

BOOL CONTROLLER::Activated( void )
{
BOOL activated=FALSE;

    if( OpenFlag )
    {
        if( LoopTaskFlag )
        {
            activated = ActivatedIndirect();
        }
        else
        {
            activated = ActivatedDirect();
        }
    }

    return(activated);
}

/******************************************************************************/

BOOL CONTROLLER::ActivatedIndirect( void )
{
BOOL activated=FALSE;

    if( OpenFlag )
    {
        activated = ActivatedFlag;
    }

    return(activated);
}

/******************************************************************************/

BOOL CONTROLLER::ActivatedDirect( void )
{
BOOL activated=FALSE;

    if( !OpenFlag )
    {
        return(FALSE);
    }

    LatencyBefore(LatencyActivated);

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           if( !(activated = PhantomActivated()) )
           {
               activated = PhantomActivated();
           }
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           if( !(activated = SensorayActivated()) )
           {
               activated = SensorayActivated();
           }
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
           if( Timer.ExpiredSeconds(2.0) )
           {
               activated = TRUE;
           }
           break;

        case CONTROLLER_HOSTCARD_OPTOTRAK :
           if( Timer.ExpiredSeconds(3.0) )
           {
               activated = TRUE;
           }
           break;

        case CONTROLLER_HOSTCARD_MOUSE :
           if( Timer.ExpiredSeconds(2.0) )
           {
               activated = !MOUSE_ButtonDepressed();
           }
           break;
    }

    LatencyAfter(LatencyActivated);

    return(activated);
}

/******************************************************************************/

BOOL    CONTROLLER::ActivatedChange( int type )
{
BOOL    flag=FALSE,activated;

    activated = Activated();

    if( ActivatedLast[type] == activated )
    {
        return(FALSE);
    }

    ActivatedLast[type] = activated;

    switch( type )
    {
        case CONTROLLER_ACTIVATED_NO2YES :
           flag = activated;
           break;

        case CONTROLLER_ACTIVATED_YES2NO :
           flag = !activated;
           break;
    }

    return(flag);
}

/******************************************************************************/

BOOL    CONTROLLER::JustActivated( void )
{
BOOL    flag;

    flag = ActivatedChange(CONTROLLER_ACTIVATED_NO2YES);

    return(flag);
}

/******************************************************************************/

BOOL    CONTROLLER::JustDeactivated( void )
{
BOOL    flag;

    flag = ActivatedChange(CONTROLLER_ACTIVATED_YES2NO);

    return(flag);
}

/******************************************************************************/

BOOL    CONTROLLER::EncoderOpen( int channel, int multiplier )
{
BOOL    ok=FALSE;

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           ok = PhantomEncoderOpen(channel);
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           ok = SensorayEncoderOpen(channel,multiplier);
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           ok = TRUE;
           break;
    }

    return(ok);
}

/******************************************************************************/

void    CONTROLLER::EncoderReset( int channel )
{
    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           PhantomEncoderReset(channel);
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           SensorayEncoderReset(channel);
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           break;
    }
}

/******************************************************************************/

long    CONTROLLER::EncoderGet( int channel )
{
long    EU=0;

    LatencyBefore(LatencyEncoder);

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           EU = PhantomEncoderGet(channel);
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           EU = SensorayEncoderGet(channel);
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           EU = 0;
           break;
    }

    LatencyAfter(LatencyEncoder);

    return(EU);
}

/******************************************************************************/

BOOL    CONTROLLER::MotorOpen( int channel )
{
BOOL    ok=FALSE;

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           ok = PhantomMotorOpen(channel);
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           ok = SensorayMotorOpen(channel);
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           ok = TRUE;
           break;
    }

    return(ok);
}

/******************************************************************************/

void    CONTROLLER::MotorReset( int channel )
{
    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           PhantomMotorReset(channel);
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           SensorayMotorReset(channel);
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           break;
    }
}

/******************************************************************************/

void    CONTROLLER::MotorSet( int channel, long units )
{
    LatencyBefore(LatencyMotor);

    switch( HostCard )
    {
        case CONTROLLER_HOSTCARD_PHANTOMISA :
           PhantomMotorSet(channel,units);
           break;

        case CONTROLLER_HOSTCARD_SENSORAY :
           SensorayMotorSet(channel,units);
           break;

        case CONTROLLER_HOSTCARD_SIMULATE :
        case CONTROLLER_HOSTCARD_MOUSE :
        case CONTROLLER_HOSTCARD_OPTOTRAK :
           break;
    }

    LatencyAfter(LatencyMotor);
}

/******************************************************************************/

BOOL    CONTROLLER::PhantomController( BOOL enable )
{
BOOL    ok=TRUE;

    ok = Phantom->Controller(enable);

    return(ok);
}

/******************************************************************************/

BOOL    CONTROLLER::PhantomActivated( void )
{
BOOL    activated=TRUE;

    activated = Phantom->Activated();

    return(activated);
}

/******************************************************************************/

BOOL    CONTROLLER::PhantomSafe( void )
{
BOOL    safe=TRUE;

    safe = Phantom->Safe();

    return(safe);
}

/******************************************************************************/

BOOL    CONTROLLER::PhantomEncoderOpen( int channel )
{
BOOL    ok=TRUE;

    return(ok);
}

/******************************************************************************/

void    CONTROLLER::PhantomEncoderReset( int channel )
{
    Phantom->EncoderReset(channel);
}

/******************************************************************************/

long    CONTROLLER::PhantomEncoderGet( int channel )
{
long    EU=0;

    EU = Phantom->EncoderGet(channel);

    return(EU);
}

/******************************************************************************/

BOOL    CONTROLLER::PhantomMotorOpen( int channel )
{
BOOL    ok=TRUE;

    return(ok);
}

/******************************************************************************/

void    CONTROLLER::PhantomMotorReset( int channel )
{
    PhantomMotorSet(channel,0);
}

/******************************************************************************/

void    CONTROLLER::PhantomMotorSet( int channel, long units )
{
    Phantom->MotorSet(channel,units);
}

/******************************************************************************/

BOOL    CONTROLLER::SensorayController( BOOL enable )
{
BOOL    ok=TRUE;

    if( !Sensoray->Opened() )
    {
        return(FALSE);
    }

    if( enable )
    {
        Sensoray->DO_Set(23,ON);
        Sensoray->DO_Set(22,OFF);
    }
    else
    {
        Sensoray->DO_Set(23,OFF);
        Sensoray->DO_Set(22,ON);
    }

    return(ok);
}

/******************************************************************************/

BOOL    CONTROLLER::SensoraySafe( void )
{
BOOL    safe;

    if( !Sensoray->Opened() )
    {
        return(FALSE);
    }

   switch( ControllerType )
   {
       case CONTROLLER_TYPE_VBOT :
          safe = !Sensoray->DI_Get(20) && Sensoray->DI_Get(19);
          break;

       default :
          safe = TRUE;
          break;
    }

    return(safe);
}

/******************************************************************************/

BOOL    CONTROLLER::SensorayActivated( void )
{
BOOL    activated;

    if( !Sensoray->Opened() )
    {
        return(FALSE);
    }

    activated = Sensoray->DI_Get(21);

    return(activated);
}

/******************************************************************************/

BOOL CONTROLLER::SensorayEncoderOpen( int channel, int multiplier )
{
BOOL ok;

    if( !Sensoray->Opened() )
    {
        CONTROLLER_errorf("SensorayEncoderOpen(channel=%d) Sensoray I/O card not open.\n",channel);
        return(FALSE);
    }

    ok = Sensoray->EncoderOpen(channel,multiplier);

    STR_printf(ok,CONTROLLER_debugf,CONTROLLER_errorf,"SensorayEncoderOpen(channel=%d) %s.\n",channel,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

void CONTROLLER::SensorayEncoderReset( int channel )
{
    if( !Sensoray->Opened() )
    {
        return;
    }

    Sensoray->EncoderReset(channel);
}

/******************************************************************************/

long CONTROLLER::SensorayEncoderGet( int channel )
{
long EU;

    if( !Sensoray->Opened() )
    {
        return(0);
    }

    EU = Sensoray->EncoderCount(channel);

    return(EU);
}

/******************************************************************************/

BOOL CONTROLLER::SensorayMotorOpen( int channel )
{
BOOL ok=TRUE;
int min=-500,max=500;

    if( !Sensoray->Opened() )
    {
        return(FALSE);
    }

//  Sensoray->DAC_Range(channel,min,max);

    return(ok);
}

/******************************************************************************/

void CONTROLLER::SensorayMotorReset( int channel )
{
    SensorayMotorSet(channel,0);
}

/******************************************************************************/

void CONTROLLER::SensorayMotorSet( int channel, long units )
{
    if( !Sensoray->Opened() )
    {
        return;
    }

    Sensoray->WriteDAC(channel,units);
}

/******************************************************************************/

void CONTROLLER::LatencyOpen( void )
{
    if( LatencyEncoder == NULL )
    {
        LatencyEncoder = new TIMER_Interval(STR_stringf("%s-ENC",Name()));
    }

    if( LatencyMotor == NULL )
    {
        LatencyMotor = new TIMER_Interval(STR_stringf("%s-MOT",Name()));
    }

    if( LatencyActivated == NULL )
    {
        LatencyActivated = new TIMER_Interval(STR_stringf("%s-RUN",Name()));
    }

    if( LatencySafe == NULL )
    {
        LatencySafe = new TIMER_Interval(STR_stringf("%s-CHK",Name()));
    }
}

/******************************************************************************/

void CONTROLLER::LatencyClose( void )
{
    LatencyStop();

    if( LatencyEncoder != NULL )
    {
        delete LatencyEncoder;
        LatencyEncoder = NULL;
    }

    if( LatencyMotor != NULL )
    {
        delete LatencyMotor;
        LatencyMotor = NULL;
    }

    if( LatencyActivated != NULL )
    {
        delete LatencyActivated;
        LatencyActivated = NULL;
    }

    if( LatencySafe != NULL )
    {
        delete LatencySafe;
        LatencySafe = NULL;
    }
}

/******************************************************************************/

void CONTROLLER::LatencyStart( void )
{
    LatencyOpen();
    LatencyFlag = TRUE;
}

/******************************************************************************/

void CONTROLLER::LatencyStop( void )
{
    LatencyFlag = FALSE;
}

/******************************************************************************/

void CONTROLLER::LatencyReset( void )
{
    if( LatencyEncoder != NULL )
    {
        LatencyEncoder->Reset();
    }

    if( LatencyMotor != NULL )
    {
        LatencyMotor->Reset();
    }

    if( LatencyActivated != NULL )
    {
        LatencyActivated->Reset();
    }

    if( LatencySafe != NULL )
    {
        LatencySafe->Reset();
    }
}

/******************************************************************************/

void CONTROLLER::LatencyBefore( TIMER_Interval *t )
{
    if( (t != NULL) && LatencyFlag )
    {
        t->Before();
    }
}

/******************************************************************************/

void CONTROLLER::LatencyAfter( TIMER_Interval *t )
{
    if( (t != NULL) && LatencyFlag )
    {
        t->After();
    }
}

/******************************************************************************/

void CONTROLLER::LatencyResults( void )
{
    if( LatencyEncoder != NULL )
    {
        LatencyEncoder->Results();
    }

    if( LatencyMotor != NULL )
    {
        LatencyMotor->Results();
    }

    if( LatencyActivated != NULL )
    {
        LatencyActivated->Results();
    }

    if( LatencySafe != NULL )
    {
        LatencySafe->Results();
    }
}

/******************************************************************************/

