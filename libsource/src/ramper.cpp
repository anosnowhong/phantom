/******************************************************************************/
/*                                                                            */
/* MODULE  : RAMPER.cpp                                                       */
/*                                                                            */
/* PURPOSE : Ramp linearly up or down between 0 and 1.                        */
/*                                                                            */
/* DATE    : 24/Oct/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 24/Oct/2002 - Initial development of module.                     */
/*                                                                            */
/* V2.0  JNI 21/Jul/2015 - Increase update rate from 100Hz to 1000Hz.         */
/*                                                                            */
/* V2.1  JNI 09/Sep/2015 - Ramp down automatically after optional hold time.  */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "RAMPER"
#define MODULE_TEXT     "Ramping API"
#define MODULE_DATE     "09/09/2015"
#define MODULE_VERSION  "2.1"
#define MODULE_LEVEL     3

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

BOOL RAMPER_API_started=FALSE;

RAMPER *RAMPER_Item[RAMPER_MAX];

/******************************************************************************/

RAMPER::RAMPER( void )
{
    // Initialize object...
    Init();
}

/******************************************************************************/

RAMPER::RAMPER( char *name )
{
    // Initialize object...
    Init(name);
}

/******************************************************************************/

RAMPER::~RAMPER( void )
{
    Stop();
    Close();
}

/******************************************************************************/

void RAMPER::Init( void )
{
    // Make sure API is running...
    if( !RAMPER_API_check() )
    {
        return;
    }

    memset(ObjectName,0,STRLEN);

    OpenFlag = FALSE;
    StartFlag = FALSE;

    RampTime = 0.0;
    HoldTime = 0.0;
    RampStep = 0.0;
    RampValue = 0.0;
    RampSign = 0.0;
    RampFlag = NULL;
    RampReset = FALSE;
}

/******************************************************************************/

void RAMPER::Init( char *name )
{
    Init();
    strncpy(ObjectName,name,STRLEN);
}

/******************************************************************************/

void RAMPER::One( void )
{
    RampValue = 1.0;
    RampSign = 0.0;
}

/******************************************************************************/

void RAMPER::Zero( void )
{
    RampValue = 0.0;
    RampSign = 0.0;
}

/******************************************************************************/

BOOL RAMPER::Opened( void )
{
BOOL flag=FALSE;

    if( OpenFlag )
    {
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

BOOL RAMPER::Started( void )
{
BOOL flag=FALSE;

    if( Opened() )
    {
        flag = StartFlag;
    }

    return(flag);
}

/******************************************************************************/

void RAMPER::RampTimeSet( double ramptime )
{
    if( !Opened() )
    {
        return;
    }
    
    RampTime = ramptime;

    if( RampTime != 0.0 )
    {
        RampStep = (RAMPER_TIMESTEP / RampTime);
    }
    else
    {
        RampStep = 0.0;
    }

    printf("RAMPER::RampTimeSet(RampTime=%.3lfsec,RampStep=%.4lf)\n",RampTime,RampStep);
}

/******************************************************************************/

void RAMPER::HoldTimeSet( double holdtime )
{
    if( !Opened() )
    {
        return;
    }
    
    HoldTime = holdtime;
}

/******************************************************************************/

BOOL RAMPER::Open( double ramptime, double holdtime, BOOL (*flag)( void ) )
{
    if( Opened() )
    {
        return(TRUE);
    }

    if( !RAMPER_Create(this) )
    {
        return(FALSE);
    }

    OpenFlag = TRUE;

    RampTimeSet(ramptime);
    HoldTimeSet(holdtime);
    RampFlag = flag;
    Zero();

    RAMPER_debugf("RAMPER(%s) Opened.\n",ObjectName);

    return(TRUE);
}

/******************************************************************************/

BOOL RAMPER::Open( double ramptime, double holdtime )
{
BOOL ok;

    ok = Open(ramptime,holdtime,NULL);

    return(ok);
}

/******************************************************************************/

BOOL RAMPER::Open( double ramptime )
{
double holdtime=0.0;
BOOL ok;

    ok = Open(ramptime,holdtime,NULL);

    return(ok);
}

/******************************************************************************/

void RAMPER::Close( void )
{
    if( !Opened() )
    {
        return;
    }

    Stop();

    OpenFlag = FALSE;
    Reset();

    RAMPER_Delete(this);

    RAMPER_debugf("RAMPER(%s) Close.\n",ObjectName);
}

/******************************************************************************/

BOOL RAMPER::Start( double ramptime, double holdtime, BOOL (*flag)( void ) )
{
BOOL ok=FALSE;

    ok = Open(ramptime,holdtime,flag);

    if( ok && !LOOPTASK_running(RAMPER_LoopTask) )
    {
        ok = LOOPTASK_start(RAMPER_LoopTask,RAMPER_LOOPTASK);
    }

    if( !ok )
    {
        return(FALSE);
    }

    Reset();

    if( Started() )
    {
        return(TRUE);
    }

    StartFlag = TRUE;

    RAMPER_debugf("RAMPER(%s) Started.\n",ObjectName);

    return(ok);
}

/******************************************************************************/

BOOL RAMPER::Start( double ramptime, double holdtime )
{
BOOL ok;

    ok = Start(ramptime,holdtime,NULL);

    return(ok);
}

/******************************************************************************/

BOOL RAMPER::Start( double ramptime )
{
double holdtime=0.0;
BOOL ok;

    ok = Start(ramptime,holdtime,NULL);

    return(ok);
}

/******************************************************************************/

BOOL RAMPER::Start( void )
{
BOOL ok;

    ok = Start(0.0,NULL);

    return(ok);
}

/******************************************************************************/

void RAMPER::Stop( void )
{
    if( !Started() )
    {
        return;
    }

    StartFlag = FALSE;
    Reset();

    RAMPER_debugf("RAMPER(%s) Stopped.\n",ObjectName);
}

/******************************************************************************/

void RAMPER::Reset( BOOL flag )
{
    if( (RampReset=flag) )
    {
        Reset();
    }
}

/******************************************************************************/

void RAMPER::Reset( void )
{
    Up();

    if( RampValue != 0.0 )
    {
        RampValue = 0.0;
        RAMPER_debugf("RAMPER(%s) Reset.\n",ObjectName);
    }
}

/******************************************************************************/

void RAMPER::Direction( double sign )
{
    RampSign = sign;
}

/******************************************************************************/

void RAMPER::Up( void )
{
    if( RampTime == 0.0 )
    {
        One();
        return;
    }

    Direction(+1.0);
}

/******************************************************************************/

void RAMPER::Down( void )
{
    if( RampTime == 0.0 )
    {
        Zero();
        return;
    }

    Direction(-1.0);
}

/******************************************************************************/

double RAMPER::RampCurrent( void )
{
double value=0.0;

    if( Started() )
    {
        value = RampValue;
    }

    return(value);
}

/******************************************************************************/

double RAMPER::RampDouble( double value )
{
double result;

    result = RampCurrent() * value;

    return(result);
}

/******************************************************************************/

long RAMPER::RampLong( long value )
{
long result;

    result = (long)(RampCurrent() * (double)value);

    return(result);
}

/******************************************************************************/

matrix RAMPER::RampMatrix( matrix &mtx )
{
MTXRETN result;

    result = mtx * RampCurrent();

    return(result);
}

/******************************************************************************/

BOOL RAMPER::Ramp( void )
{
BOOL flag=TRUE;

    if( !Started() )
    {
        flag = FALSE;
    }
    else
    if( RampReset )
    {
        flag = FALSE;
    }
    else
    if( RampFlag != NULL )
    {
        flag = (*RampFlag)();
    }

    return(flag);
}

/******************************************************************************/

void RAMPER::LoopTask( void )
{
BOOL ramp=TRUE;

BOOL flag;

    if( !Started() )
    {
        ramp = FALSE;
    }
    else
    if( !Ramp() )
    {
        ramp = FALSE;
    }

    if( !ramp )
    {
        Reset();
        return;
    }

    // Ramp up...
    if( (RampSign == +1.0) && (RampValue < 1.0) )
    {
        RampValue += RampStep;

        if( RampValue >= 1.0 )
        {
            RampValue = 1.0;
            RampSign = 0.0;
            HoldTimer.Reset();
            RAMPER_debugf("RAMPER(%s) Finished.\n",ObjectName);
        }
    }

    // Hold time...
    if( (RampValue == 1.0) && (HoldTime != 0.0) && HoldTimer.ExpiredSeconds(HoldTime) )
    {
        Down();
    }

    // Ramp down...
    if( (RampSign == -1.0) && (RampValue > 0.0) )
    {
        RampValue -= RampStep;

        if( RampValue <= 0.0 )
        {
            RampValue = 0.0;
            RampSign = 0.0;
            RAMPER_debugf("RAMPER(%s) Finished.\n",ObjectName);
        }
    }
}

/******************************************************************************/

BOOL RAMPER::RampComplete( void )
{
BOOL flag=TRUE;

    // Ramping up...
    if( (RampSign == +1.0) && (RampValue < 1.0) )
    {
        flag = FALSE;
    }

    // Ramping down...
    if( (RampSign == -1.0) && (RampValue > 0.0) )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

PRINTF  RAMPER_PRN_messgf=NULL;                  // General messages printf function.
PRINTF  RAMPER_PRN_errorf=NULL;                  // Error messages printf function.
PRINTF  RAMPER_PRN_debugf=NULL;                  // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     RAMPER_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(RAMPER_PRN_messgf,buff));
}

/******************************************************************************/

int     RAMPER_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(RAMPER_PRN_errorf,buff));
}

/******************************************************************************/

int     RAMPER_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(RAMPER_PRN_debugf,buff));
}

/******************************************************************************/

BOOL RAMPER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL ok=TRUE;
int handle;

    RAMPER_PRN_messgf = messgf;     // General API message print function.
    RAMPER_PRN_errorf = errorf;     // API error message print function.
    RAMPER_PRN_debugf = debugf;     // Debug information print function.

    if( RAMPER_API_started )
    {
        return(TRUE);
    }

    for( handle=0; (handle < RAMPER_MAX); handle++ )
    {
        RAMPER_Item[handle] = NULL;
    }

    ok = LOOPTASK_start(RAMPER_LoopTask,RAMPER_LOOPTASK);

    if( ok )
    {
        ATEXIT_API(RAMPER_API_stop);       // Install stop function.
        RAMPER_API_started = TRUE;         // Set started flag.

        MODULE_start(RAMPER_PRN_debugf);   // Register module.
    }
    else
    {
        RAMPER_errorf("RAMPER_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void RAMPER_API_stop( void )
{
int handle;

    // Make sure API is running...
    if( !RAMPER_API_started )
    {
        return;
    }

    for( handle=0; (handle < RAMPER_MAX); handle++ )
    {
        if( RAMPER_Item[handle] != NULL )
        {
            RAMPER_Item[handle]->Stop();
        }
    }

    LOOPTASK_stop(RAMPER_LoopTask);

    RAMPER_API_started = FALSE;      // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL RAMPER_API_check( void )
{
BOOL ok=TRUE;

    // Start API if it's not already running...
    if( !RAMPER_API_started )
    {                                  // Start module automatically...
        ok = RAMPER_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        STR_printf(ok,RAMPER_debugf,RAMPER_errorf,"RAMPER_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

void RAMPER_LoopTask( void )
{
int handle;

    for( handle=0; (handle < RAMPER_MAX); handle++ )
    {
        if( RAMPER_Item[handle] != NULL )
        {
            RAMPER_Item[handle]->LoopTask();
        }
    }
}

/******************************************************************************/

int RAMPER_Handle( RAMPER *item )
{
int handle;

    if( !RAMPER_API_check() )
    {
        return(RAMPER_INVALID);
    }

    for( handle=0; (handle < RAMPER_MAX); handle++ )
    {
        if( RAMPER_Item[handle] == item )
        {
            break;
        }
    }

    if( handle >= RAMPER_MAX )
    {
        handle = RAMPER_INVALID;
    }

    return(handle);
}

/******************************************************************************/

BOOL RAMPER_Create( RAMPER *item )
{
BOOL ok=FALSE;
int handle;

    if( (handle=RAMPER_Handle(NULL)) != RAMPER_INVALID )
    {
        RAMPER_Item[handle] = item;
        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

void RAMPER_Delete( RAMPER *item )
{
int handle;

    if( (handle=RAMPER_Handle(item)) != RAMPER_INVALID )
    {
        RAMPER_Item[handle] = NULL;
    }
}

/******************************************************************************/

