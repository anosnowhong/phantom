/******************************************************************************/
/*                                                                            */ 
/* MODULE  : LoopTask.cpp                                                     */ 
/*                                                                            */ 
/* PURPOSE : Loop Task (for "back-ground" execution) functions.               */ 
/*                                                                            */ 
/* DATE    : 14/Oct/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 14/Oct/2000 - Initial development.                               */ 
/*                                                                            */ 
/* V1.1  JNI 10/Apr/2001 - Added flag to enable task loop time checking.      */ 
/*                                                                            */ 
/* V1.2  JNI 30/Aug/2001 - Added name string and timing stuff to figure out   */ 
/*                         if LOOPTASK are executed when they should be.      */ 
/*                                                                            */ 
/* V2.0  JNI 18/May/2002 - Reverse engineered "os_extender" & now directly    */
/*                         use windows MultiMedia timer.                      */ 
/*                                                                            */ 
/* V2.1  JNI 14/Nov/2002 - Moved the MultiMedia timer to a separate module    */
/*                         ("MMTIMER").                                       */
/*                                                                            */ 
/*                       - Allow frequency of base function to be changed.    */ 
/*                                                                            */ 
/* V2.2  JNI 02/Jun/2003 - Added priority parameter for tasks.                */ 
/*                                                                            */ 
/* V2.3  JNI 16/Aug/2003 - Added real-time variables for determining actual   */ 
/*                         time between iterations (dt).                      */ 
/*                                                                            */ 
/* V2.4  JNI 16/Jul/2004 - Separate HPC functions.                            */ 
/*                                                                            */ 
/*                       - Skip bug patch.                                    */ 
/*                                                                            */ 
/*                       - Global time variables (based on loop counts).      */ 
/*                                                                            */ 
/* V2.5  JNI 24/Mar/2005 - Added LOOPTASK main loop suspend / resume.         */
/*                                                                            */ 
/* V2.6  JNI 15/Oct/2009 - Added total latency global variable.               */
/*                                                                            */ 
/* V2.7  JNI 24/Oct/2011 - Added latency global variable.                     */
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     "LOOPTASK"
#define MODULE_TEXT     "BackGround Loop Task API"
#define MODULE_DATE     "24/10/2011"
#define MODULE_VERSION  "2.7"
#define MODULE_LEVEL    1

/******************************************************************************/

#include <motor.h>                           // Includes everything we need.

/******************************************************************************/

BOOL    LOOPTASK_API_started=FALSE;
BOOL    LOOPTASK_API_debug=FALSE;
BOOL    LOOPTASK_Execute=FALSE;
BOOL    LOOPTASK_Call=TRUE;                 // This must be true for functions to be called.
long    LOOPTASK_CountTotal=0;              // Loop counter - continuous.
long    LOOPTASK_CountPerSecond=0;          // Loop counter - reset every second.
double  LOOPTASK_Time;                      // Time in milliseconds calculated from loop count.
double  LOOPTASK_TimeSeconds;               // Time in seconds calculated from loop count.
double  LOOPTASK_TPV=0.0;                   // Total Period Variance.
BOOL    LOOPTASK_Check=FALSE;               // Check time of task loop. (V1.1)
double  LOOPTASK_bFrequency=1000.0;         // Base frequency (default 1000 Hz).
double  LOOPTASK_bPeriod;                   // Base period (default 1 msec).
BOOL    LOOPTASK_SuspendFlag=FALSE;         // Flag to suspend / resume LOOPTASK main loop. (V2.5)
BOOL    LOOPTASK_LoopFlag=FALSE;            // LOOPTASK main loop is currently running flag. (V2.5)

// The following variables contain information about the currently executing function (V2.3).
// They are available to the application and should be reference only from within the function.
int     LOOPTASK_Current;                   // Currently executing item.
double  LOOPTASK_Elapsed;                   // Elapsed milliseconds for executing item. (V2.3)
double  LOOPTASK_ElapsedSeconds;            // Elapsed seconds for executing item. (V2.3)
double  LOOPTASK_dt;                        // Time since last iteration ("dt") of executing item. (V2.3)
long    LOOPTASK_Counter;                   // Number of itertaions. (V2.3)
double  LOOPTASK_Period;                    // Period (interval) in milliseconds. (V2.3)
double  LOOPTASK_Frequency;                 // Frequency of executing item in Hz. (V2.3)
BOOL    LOOPTASK_First;                     // TRUE for the first time function is called. (V2.3)
double  LOOPTASK_Latency;                   // Latency of LoopTask fucntion in previous iteration (V2.7)
double  LOOPTASK_TotalLatency;              // The total latency of all installed LoopTask functions (V2.6)

// Various timers for LoopTask execution statistics (V1.2)...
TIMER  *LOOPTASK_TimerLoop=NULL;
TIMER  *LOOPTASK_TimerExecute=NULL;
TIMER  *LOOPTASK_TimerInterval=NULL;
TIMER  *LOOPTASK_TimerRunning=NULL;

// This is the list of LoopTask functions...
struct  LOOPTASK_Item   LOOPTASK_list[LOOPTASK_LIST];

/******************************************************************************/

PRINTF LOOPTASK_PRN_messgf=NULL;            // General messages printf function.
PRINTF LOOPTASK_PRN_errorf=NULL;            // Error messages printf function.
PRINTF LOOPTASK_PRN_debugf=NULL;            // Debug information printf function.

/******************************************************************************/

struct  STR_TextItem  LOOPTASK_PriorityText[] =
{
    { LOOPTASK_PRIORITY_HIGHEST,"Highest" },
    { LOOPTASK_PRIORITY_HIGH,"High" },
    { LOOPTASK_PRIORITY_NORMAL,"Normal" },
    { LOOPTASK_PRIORITY_LOW,"Low" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

// These define the slots for various priorities...
int LOOPTASK_PriorityFirst[] = { 0,1,5,LOOPTASK_LIST-1 };
int LOOPTASK_PriorityLast[] = { LOOPTASK_LIST-1,LOOPTASK_LIST-1,LOOPTASK_LIST-1,0 };

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int LOOPTASK_messgf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(LOOPTASK_PRN_messgf,buff));
}

/******************************************************************************/

int LOOPTASK_errorf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(LOOPTASK_PRN_errorf,buff));
}

/******************************************************************************/

int LOOPTASK_debugf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(LOOPTASK_PRN_debugf,buff));
}

/******************************************************************************/

void LOOPTASK_MMTIMER_Task( void  )
{
    if( LOOPTASK_API_started && !LOOPTASK_SuspendFlag )
    {
        LOOPTASK_LoopFlag = TRUE;
        LOOPTASK_loop();
        LOOPTASK_LoopFlag = FALSE;
    }
}

/******************************************************************************/

BOOL LOOPTASK_MMTIMER_Start( UINT period )
{
BOOL ok;

    // Start MultiMedia timer...
    ok = MMTIMER_Start(period,LOOPTASK_MMTIMER_Task);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_MMTIMER_Stop( void )
{
BOOL ok;

    ok = MMTIMER_Stop();

    return(ok);
}

/******************************************************************************/

void LOOPTASK_init( int item )
{
    memset(&LOOPTASK_list[item],0,sizeof(struct LOOPTASK_Item));
    LOOPTASK_list[item].func = NULL;
}

/******************************************************************************/

void LOOPTASK_init( void )
{
int item;

    for( item=0; (item < LOOPTASK_LIST); item++ )
    {
        LOOPTASK_init(item);
    }
}

/******************************************************************************/

BOOL    LOOPTASK_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
int     item;
BOOL    ok=TRUE;

    LOOPTASK_PRN_messgf = messgf;      // General API message print function.
    LOOPTASK_PRN_errorf = errorf;      // API error message print function.
    LOOPTASK_PRN_debugf = debugf;      // Debug information print function.

    LOOPTASK_API_debug = (debugf != NULL);

    if( LOOPTASK_API_started )         // Start the API once...
    {
        return(TRUE);
    }

    // Initialize LoopTask list...
    LOOPTASK_init();

    // Zero loop counters...
    LOOPTASK_CountTotal = 0;
    LOOPTASK_CountPerSecond = 0;

    // Period of base task in milliseconds...
    LOOPTASK_bPeriod = 1000.0 / LOOPTASK_bFrequency;

    // Start Windows MultiMedia timer...
    if( !LOOPTASK_MMTIMER_Start((UINT)LOOPTASK_bPeriod) )
    {
        LOOPTASK_MMTIMER_Stop();
        LOOPTASK_errorf("LOOPTASK_API_start(...) Failed.\n");

        return(FALSE);
    }

    LOOPTASK_debugf("LOOPTASK_MMTIMER_Start() Frequency=%.1lf Hz.\n",LOOPTASK_bFrequency);

    // Start general LOOPTASK timers...
    LOOPTASK_TimerLoop     = new TIMER("LOOPTASK LOOP");
    LOOPTASK_TimerRunning  = new TIMER("LOOPTASK RUNNING");
    LOOPTASK_TimerExecute  = new TIMER("LOOPTASK EXECUTE");
    LOOPTASK_TimerInterval = new TIMER("LOOPTASK INTERVAL");

    ATEXIT_API(LOOPTASK_API_stop);     // Install stop function.
    LOOPTASK_API_started = TRUE;       // Set started flag.

    // Register module start...
    MODULE_start(LOOPTASK_PRN_debugf);

    // Start executing base task...
    LOOPTASK_Execute = TRUE;

    return(TRUE);
}

/******************************************************************************/

void    LOOPTASK_API_stop( void )
{
double  elapsed,mpv=0.0,fHz=0.0;

    // Check if API running...
    if( !LOOPTASK_API_started )
    {
         return;
    }

    LOOPTASK_API_started = FALSE;

    if( LOOPTASK_API_debug )
    {
        elapsed = LOOPTASK_TimerRunning->ElapsedSeconds();

        if( LOOPTASK_CountTotal > 1 )
        {
            fHz = (double)LOOPTASK_CountTotal / elapsed;              // Frequency of base task.
            mpv = LOOPTASK_TPV / (double)(LOOPTASK_CountTotal-1);     // Mean Period Variance.
        }

        LOOPTASK_debugf("LOOPTASK_API_stop() %ld loops %.1lf seconds (%.2lf Hz) %.3lf MPV(msec).\n",LOOPTASK_CountTotal,elapsed,fHz,mpv);
    }

    // Flag to stop execution...
    LOOPTASK_Execute = FALSE;

    // Stop all LOOKTASKs...
    LOOPTASK_stop();

    // Stop Windows MultiMedia event function...
    if( !LOOPTASK_MMTIMER_Stop() )
    {
        LOOPTASK_errorf("LOOPTASK_MMTIMER_stop() Failed.\n");
    }

    // Stop API timers...
    TIMER_Delete(&LOOPTASK_TimerLoop);
    TIMER_Delete(&LOOPTASK_TimerRunning);
    TIMER_Delete(&LOOPTASK_TimerExecute);
    TIMER_Delete(&LOOPTASK_TimerInterval);

    // Register module stop...
    MODULE_stop();
}

/******************************************************************************/

BOOL    LOOPTASK_API_check( void )
{
BOOL    ok=TRUE;

    // Check is API started...
    if( !LOOPTASK_API_started )
    {
        ok = LOOPTASK_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        LOOPTASK_debugf("LOOPTASK_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

void LOOPTASK_frequency( double &frequency )
{
UINT period;
double f,d,dmin;

    // Find the closest frequency that matches a whole-number millisecond period...
    for( dmin=MAX_DOUBLE,period=1; (period <= 20); period++ )
    {
        // Frequency for this period...
        f = 1000.0 / (double)period;

        // Absolute difference between this and requested frequency...
        d = fabs(frequency - f);

        // Find the minimum difference and use this frequency...
        if( d < dmin )
        {
            dmin = d;
            LOOPTASK_bFrequency = f;
        }
    }

    frequency = LOOPTASK_bFrequency;
}

/******************************************************************************/

double LOOPTASK_frequency( void )
{
    return(LOOPTASK_bFrequency);
}

/******************************************************************************/

double LOOPTASK_period( void )
{
    return(LOOPTASK_bPeriod);
}

/******************************************************************************/

int     LOOPTASK_free( int first, int last )
{
int     item,find,increment=1;

    if( first > last )
    {
        increment = -1;
    }

    for( find=LOOPTASK_INVALID,item=first; (item != (last+increment)); item+=increment )
    {
        if( LOOPTASK_list[item].func == NULL )
        {
            find = item;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

int     LOOPTASK_free( int priority )
{
int     item;

    // Make sure API is running...
    if( !LOOPTASK_API_check() )
    {
        return(LOOPTASK_INVALID);
    }

    // Loop through decending priorities until we find a free slot...
    for( item=LOOPTASK_INVALID; ((priority <= LOOPTASK_PRIORITY_LOW) && (item == LOOPTASK_INVALID)); priority++ )
    {
        item = LOOPTASK_free(LOOPTASK_PriorityFirst[priority],LOOPTASK_PriorityLast[priority]);
    }

    return(item);
}

/******************************************************************************/

int     LOOPTASK_find( LOOPTASK func )
{
int     item,find;

    // Make sure API is running...
    if( !LOOPTASK_API_check() )
    {
        return(LOOPTASK_INVALID);
    }

    for( find=LOOPTASK_INVALID,item=0; (item < LOOPTASK_LIST); item++ )
    {
        if( LOOPTASK_list[item].func == func )
        {
            find = item;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

int LOOPTASK_fmodulus( double baseHz, double &taskHz )
{
int fdivider;
double actualHz;

    // Calculate divider for this task frequency...
    fdivider = (int)(baseHz / taskHz);
    actualHz = baseHz / (double)fdivider;

    // Allow for "rounding" errors...
    if( fabs(actualHz-taskHz) > 1.0 ) 
    {
        return(LOOPTASK_INVALID);
    }

    taskHz = actualHz;

    return(fdivider);
}

/******************************************************************************/

BOOL LOOPTASK_fvalid( double baseHz, double taskHz )
{
BOOL ok;

    ok = (LOOPTASK_fmodulus(baseHz,taskHz) != LOOPTASK_INVALID);

    return(ok);
}

/******************************************************************************/

void LOOPTASK_LoopWait( void )
{
static TIMER timeout("LOOPTASK_LoopWait");

    timeout.Reset();
    while( LOOPTASK_LoopFlag )
    {
        if( timeout.Expired(10.0) )
        {
            LOOPTASK_errorf("LOOPTASK_LoopWait() TimeOut.\n");
            break;
        }
    }
}

/******************************************************************************/

BOOL LOOPTASK_start( char *name, LOOPTASK func, double freq, long loops, int priority )
{
int item,fdivider;

    // Make sure API is running...
    if( !LOOPTASK_API_check() )
    {
        return(FALSE);
    }

    // Make sure this function is not already running...
    if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
    {
        LOOPTASK_errorf("LOOPTASK_start(...) Task already running.\n");
        return(FALSE);
    }

    if( (item=LOOPTASK_free(priority)) == LOOPTASK_INVALID )
    {
        LOOPTASK_errorf("LOOPTASK_start(...) No free handles.\n");
        return(FALSE);
    }

    // Use base frequency if zero...
    if( freq == 0.0 )
    {
        freq = LOOPTASK_bFrequency;
    }

    // Check if task frequency is valid...
    if( !LOOPTASK_fvalid(LOOPTASK_bFrequency,freq) )
    {
        LOOPTASK_errorf("LOOPTASK_start(...) Frequency (%.1lf Hz) invalid.\n",freq);
        return(FALSE);
    }

    // Make sure we're not inside LOOPTASK main loop...
    LOOPTASK_LoopWait();

    // Suspend LOOPTASK main loop while we add new item...
    LOOPTASK_suspend();

    LOOPTASK_init(item);

    if( name == NULL )
    {
        strncpy(LOOPTASK_list[item].name,STR_stringf("[%02d]",item),STRLEN);
    }
    else
    {
        strncpy(LOOPTASK_list[item].name,name,STRLEN);
    }

    LOOPTASK_list[item].skip = FALSE;
    LOOPTASK_list[item].freq = freq;
    LOOPTASK_list[item].fdivider = LOOPTASK_fmodulus(LOOPTASK_bFrequency,freq);
    LOOPTASK_list[item].period = 1000.0 / freq;
    LOOPTASK_list[item].loops = loops;
    LOOPTASK_list[item].tpv = 0.0;
    LOOPTASK_list[item].pmin = LOOPTASK_list[item].period;
    LOOPTASK_list[item].pmax = LOOPTASK_list[item].period;
    LOOPTASK_list[item].tet = 0.0;
    LOOPTASK_list[item].timer = new TIMER(LOOPTASK_list[item].name);
    LOOPTASK_list[item].priority = priority;
    LOOPTASK_list[item].first = TRUE;
    LOOPTASK_list[item].func = func;

    // Reset application-level counters and timer related variables...
    LOOPTASK_reset(item);

    // Resume LOOPTASK main loop...
    LOOPTASK_resume();

    LOOPTASK_debugf("LOOPTASK_start(%s,%.1lfHz,loop=%ld,priority=%s) OK.\n",
                    LOOPTASK_list[item].name,
                    freq,
                    loops,
                    STR_TextCode(LOOPTASK_PriorityText,priority));

    return(TRUE);
}

/******************************************************************************/

BOOL LOOPTASK_start( char *name, LOOPTASK func, double freq, long loops )
{
int priority=LOOPTASK_PRIORITY_DEFAULT;
BOOL ok;

    ok = LOOPTASK_start(name,func,freq,loops,priority);

    return(ok);
}

/******************************************************************************/

BOOL    LOOPTASK_start( char *name, LOOPTASK func, double freq, double duration, int priority )
{
long loops;
BOOL ok;

    loops = (long)(duration * freq);
    ok = LOOPTASK_start(name,func,freq,loops,priority);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_start( char *name, LOOPTASK func, double freq, double duration )
{
int priority=LOOPTASK_PRIORITY_DEFAULT;
BOOL ok;

    ok = LOOPTASK_start(name,func,freq,duration,priority);

    return(ok);
}

/******************************************************************************/

BOOL    LOOPTASK_start( char *name, LOOPTASK func, double freq, int priority )
{
BOOL ok;

    ok = LOOPTASK_start(name,func,freq,0L,priority);

    return(ok);
}

/******************************************************************************/

BOOL    LOOPTASK_start( char *name, LOOPTASK func, double freq )
{
int priority=LOOPTASK_PRIORITY_DEFAULT;
BOOL ok;

    ok = LOOPTASK_start(name,func,freq,0L,priority);

    return(ok);
}

/******************************************************************************/

BOOL    LOOPTASK_start( char *name, LOOPTASK func, int priority )
{
BOOL ok;

    ok = LOOPTASK_start(name,func,LOOPTASK_bFrequency,0L,priority);

    return(ok);
}

/******************************************************************************/

BOOL    LOOPTASK_start( char *name, LOOPTASK func )
{
int priority=LOOPTASK_PRIORITY_DEFAULT;
BOOL ok;

    ok = LOOPTASK_start(name,func,LOOPTASK_bFrequency,0L,priority);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_start( LOOPTASK func, double freq, long loops, int priority )
{
BOOL ok;

    ok = LOOPTASK_start(NULL,func,freq,loops,priority);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_start( LOOPTASK func, double freq, long loops )
{
BOOL ok;

    ok = LOOPTASK_start(NULL,func,freq,loops);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_start( LOOPTASK func, double freq, double duration, int priority )
{
BOOL ok;

    ok = LOOPTASK_start(NULL,func,freq,duration,priority);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_start( LOOPTASK func, double freq, double duration )
{
BOOL ok;

    ok = LOOPTASK_start(NULL,func,freq,duration);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_start( LOOPTASK func, double freq, int priority )
{
BOOL ok;

    ok = LOOPTASK_start(NULL,func,freq,priority);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_start( LOOPTASK func, double freq )
{
BOOL ok;

    ok = LOOPTASK_start(NULL,func,freq);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_start( LOOPTASK func, int priority )
{
BOOL ok;

    ok = LOOPTASK_start(NULL,func,priority);

    return(ok);
}

/******************************************************************************/

BOOL LOOPTASK_start( LOOPTASK func )
{
BOOL ok;

    ok = LOOPTASK_start(NULL,func);

    return(ok);
}

/******************************************************************************/

void    LOOPTASK_suspend( LOOPTASK func )
{
int     item;

    if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
    {
        LOOPTASK_list[item].skip = TRUE;
    }
}

/******************************************************************************/

void    LOOPTASK_resume( LOOPTASK func )
{
int     item;

    if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
    {
        LOOPTASK_list[item].skip = FALSE;
    }
}

/******************************************************************************/

void    LOOPTASK_suspend( void )
{
    LOOPTASK_SuspendFlag = TRUE;
}

/******************************************************************************/

void    LOOPTASK_resume( void )
{
    LOOPTASK_SuspendFlag = FALSE;
}

/******************************************************************************/

void    LOOPTASK_timing( int item )
{
double  elapsed,fHz=0.0,met=0.0,mpv=0.0;

    elapsed = LOOPTASK_list[item].timer->ElapsedSeconds();

    if( LOOPTASK_list[item].count > 1L )
    {
        fHz = (double)LOOPTASK_list[item].count / elapsed;
        mpv = LOOPTASK_list[item].tpv / (double)(LOOPTASK_list[item].count-1);  // Mean Period Variance.
        met = LOOPTASK_list[item].tet / (double)LOOPTASK_list[item].count;      // Mean Execution Time.
    }

    LOOPTASK_debugf("[%02d] %6.1f T %6.1lf A Hz | %5.3lf MET | P %6.1lf %6.1lf V %3.1lf %.20s\n",
                     item,
                     LOOPTASK_list[item].freq,
                     fHz,
                     met,
                     LOOPTASK_list[item].pmin,LOOPTASK_list[item].pmax,
                     mpv,
                     LOOPTASK_list[item].name);
}

/******************************************************************************/

void    LOOPTASK_stop( LOOPTASK func )
{
int     item;

    if( !LOOPTASK_API_check() )        // Make sure API is running...
    {
        return;
    }

    if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
    {
        LOOPTASK_stop(item);
    }
}

/******************************************************************************/

void    LOOPTASK_stop( int item )
{
    if( LOOPTASK_list[item].func == NULL )
    {
        return;
    }

    // Make sure we're not inside LOOPTASK main loop...
    LOOPTASK_LoopWait();

    // Suspend LOOPTASK main loop while we stop this item...
    LOOPTASK_suspend();

    // Calculate timing if debug mode (V1.2)...
    if( LOOPTASK_API_debug )
    {
        LOOPTASK_timing(item);
    }

    // Clear function handle...
    LOOPTASK_list[item].func = NULL;

    // Resume LOOPTASK main loop...
    LOOPTASK_resume();

    // Free timer (if allocated)...
    TIMER_Delete(&LOOPTASK_list[item].timer);
}

/******************************************************************************/

void    LOOPTASK_stop( void )
{
int     item;

    for( item=0; (item < LOOPTASK_LIST); item++ )
    {
        LOOPTASK_stop(item);
    }
}

/******************************************************************************/

void LOOPTASK_terminate( LOOPTASK func )
{
int item;

    if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
    {
        LOOPTASK_terminate(item);
    }
}

/******************************************************************************/

void LOOPTASK_terminate( int item )
{
    if( LOOPTASK_check(item) )
    {
        LOOPTASK_list[item].terminate = TRUE;
    }
}

/******************************************************************************/

void LOOPTASK_terminate( void )
{
    LOOPTASK_terminate(LOOPTASK_Current);
}

/******************************************************************************/

long    LOOPTASK_count( LOOPTASK func )
{
int     item;
long    count=0;

    if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
    {
        count = LOOPTASK_list[item].count;
    }

    return(count);
}

/******************************************************************************/

double  LOOPTASK_frequency( LOOPTASK func )
{
int     item;
double  freq=0;

    if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
    {
        freq = LOOPTASK_list[item].freq;
    }

    return(freq);
}

/******************************************************************************/

void    LOOPTASK_reset( LOOPTASK func )
{
int     item;

    if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
    {
        LOOPTASK_reset(item);
    }
}

/******************************************************************************/

void LOOPTASK_reset( void )
{
    LOOPTASK_reset(LOOPTASK_Current);
}

/******************************************************************************/

void    LOOPTASK_reset( int item )
{
    if( LOOPTASK_check(item) )
    {
        LOOPTASK_list[item].count = 0;
        LOOPTASK_list[item].msec = 0.0;
        LOOPTASK_list[item].tnow = 0.0;
        LOOPTASK_list[item].tlast = 0.0;
        LOOPTASK_list[item].dt = LOOPTASK_list[item].period;
    }
}

/******************************************************************************/

BOOL    LOOPTASK_running( LOOPTASK func )
{
int     item;
BOOL    running=FALSE;

    if( LOOPTASK_Execute )
    {
        if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
        {
            running = TRUE;
        }
    }

    return(running);
}

/******************************************************************************/

void    LOOPTASK_loop( void )
{
double  mainloop,elapsed,period;
int     item;

    // If this flag is set, don't execute task loop... (V1.1)
    if( !LOOPTASK_Execute )
    {
        return;
    }

    mainloop = LOOPTASK_TimerInterval->Elapsed();
    LOOPTASK_TimerInterval->Reset();
    LOOPTASK_TimerLoop->Reset();

    for( item=0; ((item < LOOPTASK_LIST) && !LOOPTASK_SuspendFlag); item++ )
    {
        // Check if LoopTask entry in use...
        if( LOOPTASK_list[item].func == NULL )
        {
            continue;
        }

        // Check if LoopTask terminate requested...
        if( LOOPTASK_list[item].terminate )
        {
            LOOPTASK_stop(item);
            continue;
        }

        // Check if LoopTask function suspended...
        if( LOOPTASK_list[item].skip )
        {
            continue;
        }

        // Check if it's time to execution this LoopTask function...
        if( (LOOPTASK_CountPerSecond % LOOPTASK_list[item].fdivider) != 0 )
        {
            continue;
        }

        // Calculate period timing variance for debugging purposes (V1.2)...
        if( LOOPTASK_API_debug )
        {
            elapsed = LOOPTASK_list[item].timer->Elapsed();

            if( LOOPTASK_list[item].count > 0 )
            {                                   
                period = elapsed - LOOPTASK_list[item].last;

                if( period < LOOPTASK_list[item].pmin )
                {
                    LOOPTASK_list[item].pmin = period;
                }

                if( period > LOOPTASK_list[item].pmax )
                {
                    LOOPTASK_list[item].pmax = period;
                }

                LOOPTASK_list[item].tpv += fabs(period - LOOPTASK_list[item].period);
            }

            LOOPTASK_list[item].last = elapsed;
        }

        // Set current time and calculate dt (V2.3)...
        LOOPTASK_list[item].tnow = LOOPTASK_list[item].timer->Elapsed();

        if( LOOPTASK_list[item].count > 0 )
        {
            LOOPTASK_list[item].dt = LOOPTASK_list[item].tnow - LOOPTASK_list[item].tlast;
        }

        LOOPTASK_list[item].tlast = LOOPTASK_list[item].tnow;

        // Set current task ID (V2.1) & other global variables...
        LOOPTASK_Current = item;
        LOOPTASK_Elapsed = LOOPTASK_list[item].tnow;
        LOOPTASK_ElapsedSeconds = milliseconds2seconds(LOOPTASK_list[item].tnow);
        LOOPTASK_dt = LOOPTASK_list[item].dt;
        LOOPTASK_Counter = LOOPTASK_list[item].count;
        LOOPTASK_Period = LOOPTASK_list[item].period;
        LOOPTASK_Frequency = LOOPTASK_list[item].freq;
        LOOPTASK_First = LOOPTASK_list[item].first;
        LOOPTASK_Latency = LOOPTASK_list[item].et; // Latency of previous iteration (V2.7).

        // Start execution timer (V1.2)...
        LOOPTASK_TimerExecute->Reset();

        // Execute the task function...
        if( LOOPTASK_Call )
        {
            (*LOOPTASK_list[item].func)();
        }

         // E)xecution T)ime (V2.7).
        LOOPTASK_list[item].et = LOOPTASK_TimerExecute->Elapsed();

        // T)otal E)xecution T)ime.
        LOOPTASK_list[item].tet += LOOPTASK_list[item].tet;

        // Increment loop counter...
        LOOPTASK_list[item].count++;

        // Increment millisecond counter...
        LOOPTASK_list[item].msec += (double)LOOPTASK_list[item].period;

        // Clear current task ID...
        LOOPTASK_Current = LOOPTASK_INVALID;

        // Clear first-time flag... (V2.3)
        LOOPTASK_list[item].first = FALSE;

        if( LOOPTASK_list[item].loops == 0 )
        {
            continue;
        }

        // Time to stop the task...
        if( LOOPTASK_list[item].count >= LOOPTASK_list[item].loops )
        {
            LOOPTASK_stop(item);
        }
    }

    elapsed = LOOPTASK_TimerLoop->Elapsed();

    // Check if task loop time is excessive... (V1.1)
    if( LOOPTASK_Check )
    {
        if( elapsed >= LOOPTASK_bPeriod )
        {
            LOOPTASK_errorf("LOOPTASK: Execute time (%.1lf msec) exceeds limit (%.1lf msec).\n",elapsed,LOOPTASK_bPeriod);
            LOOPTASK_Execute = FALSE;
        }
    }

    if( ++LOOPTASK_CountPerSecond >= (int)LOOPTASK_bFrequency )
    {
        LOOPTASK_CountPerSecond = 0;
    }

    // Interval variance for main loop (V1.2)...
    if( LOOPTASK_CountTotal > 0 )
    {
        LOOPTASK_TPV += fabs(mainloop - LOOPTASK_bPeriod);
    }

    LOOPTASK_CountTotal++;
    LOOPTASK_Time = LOOPTASK_bPeriod * (double)LOOPTASK_CountTotal;
    LOOPTASK_TimeSeconds = milliseconds2seconds(LOOPTASK_Time);
    LOOPTASK_TotalLatency = elapsed;
}

/******************************************************************************/

BOOL LOOPTASK_check( int item )
{
BOOL flag=TRUE;

    if( !LOOPTASK_API_check() )
    {
        flag = FALSE;
    }
    else
    if( item == LOOPTASK_INVALID )
    {
        flag = FALSE;
    }
    else
    if( LOOPTASK_list[item].func == NULL )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

double LOOPTASK_msec( int item )
{
double msec=0.0;

    if( LOOPTASK_check(item) )
    {
        msec = LOOPTASK_list[item].msec;
    }

    return(msec);
}

/******************************************************************************/

double LOOPTASK_msec( LOOPTASK func )
{
int item;
double msec=0.0;

    if( (item=LOOPTASK_find(func)) != LOOPTASK_INVALID )
    {
        msec = LOOPTASK_msec(item);
    }

    return(msec);
}

/******************************************************************************/

double LOOPTASK_msec( void )
{
double msec;

    msec = LOOPTASK_msec(LOOPTASK_Current);

    return(msec);
}

/******************************************************************************/

double LOOPTASK_second( int item )
{
double second;

    second = milliseconds2seconds(LOOPTASK_msec(item));

    return(second);
}

/******************************************************************************/

double LOOPTASK_second( LOOPTASK func )
{
double second;

    second = milliseconds2seconds(LOOPTASK_msec(func));

    return(second);
}

/******************************************************************************/

double LOOPTASK_second( void )
{
double second;

    second = milliseconds2seconds(LOOPTASK_msec());

    return(second);
}

/******************************************************************************/

void LOOPTASK_TimerSkipPatch( void )
{
BOOL ok;

    ok = LOOPTASK_API_check();
    HPC_SkipPatch = TRUE;
    STR_printf(ok,LOOPTASK_debugf,LOOPTASK_errorf,"LOOPTASK_TimerSkipTask() %s.\n",STR_OkFailed(ok));
}

/******************************************************************************/

BOOL   PRIORITY_ChangedFlag=FALSE;
HANDLE PRIORITY_HandleProcess;
HANDLE PRIORITY_HandleThread;
DWORD  PRIORITY_PriorityProcess;
int    PRIORITY_PriorityThread;

/******************************************************************************/

void PRIORITY_SetHighest( void )
{
    // Get handles for current process and thread.
    PRIORITY_HandleProcess = GetCurrentProcess();
    PRIORITY_HandleThread = GetCurrentThread();

    // Get priority settings for current process and thread.
    PRIORITY_PriorityProcess = GetPriorityClass(PRIORITY_HandleProcess);
    PRIORITY_PriorityThread = GetThreadPriority(PRIORITY_HandleThread);

    // Set priority for current process and thread.
    if( !SetPriorityClass(PRIORITY_HandleProcess,REALTIME_PRIORITY_CLASS) )
    {
        printf("SetPriorityClass(...,REALTIME_PRIORITY_CLASS) Failed.\n");
    }

    if( !SetThreadPriority(PRIORITY_HandleThread,THREAD_PRIORITY_TIME_CRITICAL) )
    {
        printf("SetThreadPriority(...,THREAD_PRIORITY_TIME_CRITICAL) Failed.\n");
    }

    PRIORITY_ChangedFlag = TRUE;
}

/******************************************************************************/

void PRIORITY_SetNormal( void )
{
    if( PRIORITY_ChangedFlag )
    {
        PRIORITY_ChangedFlag = FALSE;
        SetPriorityClass(PRIORITY_HandleProcess,PRIORITY_PriorityProcess);
        SetThreadPriority(PRIORITY_HandleThread,PRIORITY_PriorityThread);
    }
}

/******************************************************************************/

