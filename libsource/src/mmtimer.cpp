/******************************************************************************/
/*                                                                            */ 
/* MODULE  : MMTimer.h                                                        */ 
/*                                                                            */ 
/* PURPOSE : Windows MultiMedia Timer functions.                              */ 
/*                                                                            */ 
/* DATE    : 14/Nov/2002                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 14/Nov/2002 - Separate module created for MultiMedia Timer.      */ 
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     "MMTIMER"
#define MODULE_TEXT     "MultiMedia Timer  API"
#define MODULE_DATE     "14/11/2002"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL    0

/******************************************************************************/

#include <motor.h>                          // Includes everything we need.

/******************************************************************************/

BOOL MMTIMER_API_started=FALSE;

/******************************************************************************/

PRINTF MMTIMER_PRN_messgf=NULL;             // General messages printf function.
PRINTF MMTIMER_PRN_errorf=NULL;             // Error messages printf function.
PRINTF MMTIMER_PRN_debugf=NULL;             // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int MMTIMER_messgf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(MMTIMER_PRN_messgf,buff));
}

/******************************************************************************/

int MMTIMER_errorf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(MMTIMER_PRN_errorf,buff));
}

/******************************************************************************/

int MMTIMER_debugf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(MMTIMER_PRN_debugf,buff));
}

/******************************************************************************/

BOOL MMTIMER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL ok=TRUE;

    // API messages...
    MMTIMER_PRN_messgf = messgf;
    MMTIMER_PRN_errorf = errorf;
    MMTIMER_PRN_debugf = debugf;

    // Start API once...
    if( MMTIMER_API_started )
    {
        return(TRUE);
    }

    // Install stop function...
    ATEXIT_API(MMTIMER_API_stop);
    MMTIMER_API_started = TRUE;

    // Register module start...
    MODULE_start(MMTIMER_PRN_debugf);

    return(TRUE);
}

/******************************************************************************/

void MMTIMER_API_stop( void )
{
BOOL ok;

    // Check if API is running...
    if( !MMTIMER_API_started )
    {
         return;
    }

    // Stop MultiMedia Timer...
    ok = MMTIMER_Stop();

    MMTIMER_API_started = FALSE;

    // Register module stop...
    MODULE_stop();
}

/******************************************************************************/

BOOL MMTIMER_API_check( void )
{
BOOL ok;

    // Check if API is running...
    if( MMTIMER_API_started )
    {
        return(TRUE);
    }

    ok = MMTIMER_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
    MMTIMER_debugf("MMTIMER_API_check() Start %s.\n",STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL     MMTIMER_BeginPeriod=FALSE; 
BOOL     MMTIMER_Started=FALSE;
MMRESULT MMTIMER_Handle=NULL;
void   (*MMTIMER_Func)(void)=NULL;
double   MMTIMER_Period;
double   MMTIMER_Frequency;

/******************************************************************************/

void __stdcall MMTIMER_Task( unsigned int uID, unsigned int uMsg, unsigned long dwUser, unsigned long dw1, unsigned long dw2 )
{
    if( MMTIMER_Func != NULL )
    {
      (*MMTIMER_Func)();
    }
}

/******************************************************************************/

BOOL MMTIMER_Start( UINT period, void (*func)( void ) )
{
MMRESULT rc;
TIMECAPS TC;
BOOL ok;
UINT resolution;

    if( !MMTIMER_API_check() )
    {
        return(FALSE);
    }

    // Determine resolution of multimedia timer device...
    rc = timeGetDevCaps(&TC,sizeof(TC));

    // Timer resolution returned in TIMECAPS structure...
    // UNIT TIMECAPS.wPeriodMin
    // UNIT TIMECAPS.wPeriodMax

    ok = (rc == TIMERR_NOERROR);
    MMTIMER_debugf("MMIMER Start...\n");
    STR_printf(ok,MMTIMER_debugf,MMTIMER_errorf,"timeGetDevCaps(...) %s.\n",STR_OkFailed(ok));

    if( !ok )
    {
        return(FALSE);
    }

    MMTIMER_debugf("TC min=%u max=%u (msec).\n",TC.wPeriodMin,TC.wPeriodMax);

    resolution = 1;
 
    // Being minimum timer resolution (msec)...
    rc = timeBeginPeriod(resolution);

    ok = (rc == TIMERR_NOERROR);
    STR_printf(ok,MMTIMER_debugf,MMTIMER_errorf,"timeBeginPeriod(1) %s.\n",STR_OkFailed(ok));

    if( !ok )
    {
        return(FALSE);
    }

    MMTIMER_BeginPeriod = TRUE;

    // Pointer to MultiMedia timer function...
    MMTIMER_Func = func;

    // Set timer event (start timer function)...
    MMTIMER_Handle = timeSetEvent(period,           // uDelay (period in msec).
                                  0,                // uResolution (0 = maximum).
                                  MMTIMER_Task,     // lpTimeProc.
                                  0L,               // dwUser (User-supplised callback data).
                                  TIME_PERIODIC | TIME_CALLBACK_FUNCTION);     // fuEvent.

    ok = (MMTIMER_Handle != NULL);
    STR_printf(ok,MMTIMER_debugf,MMTIMER_errorf,"timeSetEvent(...) %s.\n",STR_OkFailed(ok));

    if( !ok )
    {
        return(FALSE);
    }

    MMTIMER_Started = TRUE;
    MMTIMER_Period = (double)period / 1000.0;
    MMTIMER_Frequency = 1.0 / MMTIMER_Period;

    return(TRUE);
}

/******************************************************************************/

BOOL MMTIMER_Start( UINT period )
{
BOOL ok;

    ok = MMTIMER_Start(period,NULL);

    return(ok);
}

/******************************************************************************/

BOOL MMTIMER_Stop( void )
{
MMRESULT rc;
BOOL ok=TRUE;

    MMTIMER_debugf("MMTIMER Stop...\n");

    // Kill event...
    if( MMTIMER_Started )
    {
        rc = timeKillEvent(MMTIMER_Handle);

        ok = (rc == TIMERR_NOERROR);
        MMTIMER_debugf("timeKillEvent(...) %s.\n",STR_OkFailed(ok));

        MMTIMER_Started = FALSE;
    }

    // End minimum timer resolution (msec)...
    if( MMTIMER_BeginPeriod )
    {
        rc = timeEndPeriod(1);

        ok = (rc == TIMERR_NOERROR);
        MMTIMER_debugf("timeEndPeriod(1) %s.\n",STR_OkFailed(ok));

        MMTIMER_BeginPeriod = FALSE;
    }

    // Reset other variables...
    MMTIMER_Func = NULL;

    return(ok);
}

/******************************************************************************/

HANDLE  MMTIMER_ThreadID=NULL;
int     MMTIMER_ThreadPriority;

/******************************************************************************/

void    MMTIMER_PriorityLow( void )
{
    MMTIMER_ThreadID = GetCurrentThread();
    MMTIMER_ThreadPriority = GetThreadPriority(MMTIMER_ThreadID);

    if( !SetThreadPriority(MMTIMER_ThreadID,THREAD_PRIORITY_NORMAL) )
    {
        MMTIMER_errorf("MMTIMER_PriorityLow() Failed.\n");
    }
}

/******************************************************************************/

void    MMTIMER_PriorityResume( void )
{
    if( !SetThreadPriority(MMTIMER_ThreadID,MMTIMER_ThreadPriority) )
    {
        MMTIMER_errorf("MMTIMER_PriorityResume() Failed.\n");
    }
}

/******************************************************************************/

