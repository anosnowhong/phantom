/******************************************************************************/
/*                                                                            */ 
/* MODULE  : timer.cpp                                                        */ 
/*                                                                            */ 
/* PURPOSE : Timer functions with millisecond range.                          */ 
/*                                                                            */ 
/* DATE    : 13/May/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 13/May/2000 - Initial development.                               */ 
/*                                                                            */ 
/* V1.1  JNI 16/Dec/2000 - Improved resolution of timers by switching from    */ 
/*                         long integers to double precision floats.          */ 
/*                                                                            */ 
/* V1.2  JNI 10/May/2001 - Added frequency timer functions.                   */ 
/*                                                                            */ 
/* V1.3  JNI 17/Jan/2002 - Allow access to TIMER handles to be monitored.     */
/*                                                                            */ 
/*                       - Added function to allow elapsed time to be set.    */
/*                                                                            */ 
/* V1.4  JNI 22/Feb/2002 - Added "Every" functions to execute something every */
/*                         X number of msecs.                                 */ 
/*                                                                            */ 
/* V2.0  JNI 27/Jun/2002 - Converted handle-based timers to a class.          */
/*                                                                            */ 
/* V2.1  JNI 19/Jul/2002 - Created additional classes.                        */
/*                                                                            */ 
/* V2.2  JNI 17/May/2004 - Moved high-performance counter interface to a      */
/*                         separate psuedo-module HPC.                        */ 
/*                                                                            */ 
/* V2.3  JNI 18/Sep/2009 - Removed old handle-based timer functions.          */
/*                                                                            */ 
/* V2.4  JNI 10/Apr/2015 - Tweaks to "Every" functions and increase handles.  */
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     "TIMER" 
#define MODULE_TEXT     "High Resolution Timer API"
#define MODULE_DATE     "10/04/2015"
#define MODULE_VERSION  "2.4"
#define MODULE_LEVEL    0

/******************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

// #define TIMER_DEBUG

/******************************************************************************/

double  seconds2milliseconds( double seconds )
{
double  milliseconds;

    milliseconds = seconds * 1000.0;

    return(milliseconds);
}

/******************************************************************************/

double  milliseconds2seconds( double milliseconds )
{
double  seconds;

    seconds = milliseconds / 1000.0;

    return(seconds);
}

/******************************************************************************/

double seconds2minutes( double seconds )
{
double minutes;

    minutes = seconds / 60.0;

    return(minutes);
}

/******************************************************************************/

double minutes2seconds( double minutes )
{
double seconds;

    seconds = minutes * 60.0;

    return(seconds);
}

/******************************************************************************/

double minutes2hours( double minutes )
{
double hours;

    hours = minutes / 60.0;

    return(hours);
}

/******************************************************************************/

double seconds2hours( double seconds )
{
double hours;

    hours = minutes2hours(seconds2minutes(seconds));

    return(hours);
}

/******************************************************************************/

CRITICAL_SECTION HPC_CS;

double  HPC_CountPerMSec=0.0;               // Clock ticks / millisecond.
double  HPC_MSecPerCount=0.0;               // Milliseconds / clock tick.

double  HPC_Resolution=0.0;                 // HPC resolution (msec).
double  HPC_Maximum=0.0;                    // HPC maximum (msec).

/******************************************************************************/

double HPC_count2msec( INT64 count )
{
double msec=0.0;

    if( TIMER_API_check() )
    {
        msec = HPC_MSecPerCount * (double)count;
    }

    return(msec);
}

/******************************************************************************/

INT64 HPC_msec2count( double msec )
{
INT64 count=0;

    if( TIMER_API_check() )
    {
        count = (INT64)(HPC_CountPerMSec * msec);
    }

    return(count);
}

/******************************************************************************/

double HPC_msec( void )
{
INT64 count;
double msec=0.0;

    if( HPC_count(count) )
    {
        msec = HPC_count2msec(count);
    }

    return(msec);
}

/******************************************************************************/

double HPC_msec( INT64 t0, INT64 t1 )
{
double msec=0.0;

    msec = HPC_count2msec(t1 - t0);

    return(msec);
}

/******************************************************************************/

BOOL HPC_frequency( LARGE_INTEGER *PF )
{
BOOL ok;

    // Get number of counts per second (frequency) of high performance counter.
    if( !(ok=QueryPerformanceFrequency(PF)) )
    {
        TIMER_errorf("QueryPerformanceFrequency(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

BOOL HPC_frequency( INT64 &frequency )
{
LARGE_INTEGER PF;
BOOL ok;

    // Get number of counts per second (frequency) of high performance counter...
    if( (ok=HPC_frequency(&PF)) )
    {
        frequency = PF.QuadPart;
    }

    return(ok);
}

/******************************************************************************/

BOOL HPC_count( LARGE_INTEGER *PC )
{
BOOL ok;

    // Get current count of high performance counter.
    if( !(ok=QueryPerformanceCounter(PC)) )   
    {
        TIMER_errorf("QueryPerformanceCounter(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

BOOL    HPC_SkipPatch=FALSE;                // HPC skip bug patch flag.
INT64   HPC_SkipAdjust=0;                   // Adjustment for HPC skip bug.
int     HPC_SkipCountTotal=0;               // Total number of skips detected.
int     HPC_SkipCount=0;                    // Number of skips detected.

/******************************************************************************/

void HPC_SkipCountReset( void )
{
    EnterCriticalSection(&HPC_CS);
    HPC_SkipCount = 0;
    LeaveCriticalSection(&HPC_CS);
}

/******************************************************************************/

void HPC_patch( void )
{
static BOOL   first=TRUE;
static double tlast=0.0;
static INT64  clast=0;
static INT64  slop=10;
LARGE_INTEGER PC;
INT64 elapsed[2];
double tnow;
INT64 cnow;

    EnterCriticalSection(&HPC_CS);

    // Get current value of HPC...
    if( !HPC_count(&PC) )
    {
        LeaveCriticalSection(&HPC_CS);
        return;
    }

    tnow = LOOPTASK_Time;    // Actual time based on LOOPTASK.
    cnow = PC.QuadPart;      // HPC counts.

    // If first call, just set last count...
    if( first )
    {
        tlast = tnow;
        clast = cnow;
        first = FALSE;
        LeaveCriticalSection(&HPC_CS);
        return;
    }

    // Elapsed counts since last here...
    elapsed[0] = cnow - clast;

    // Actual elapsed counts based on LOOPTASK.
    if( tnow == tlast )
    {
        elapsed[1] = HPC_msec2count(LOOPTASK_period() / 2.0);
    }
    else
    {
        elapsed[1] = HPC_msec2count(tnow - tlast);
    }

    // Has the HPC jumped...
    if( elapsed[0] > (slop * elapsed[1]) )
    {
        HPC_SkipAdjust += (elapsed[0] - elapsed[1]);
        HPC_SkipCountTotal++;
        HPC_SkipCount++;
    }

    // Last values...
    tlast = tnow;
    clast = cnow;

    LeaveCriticalSection(&HPC_CS);
}

/******************************************************************************/

BOOL HPC_count( INT64 &count )
{
LARGE_INTEGER PC;
BOOL ok;

    if( HPC_SkipPatch )
    {
        HPC_patch();
    }

    // Get current count of high performance counter...
    if( (ok=HPC_count(&PC)) )   
    {
        count = PC.QuadPart - HPC_SkipAdjust;
    }

    return(ok);
}

/******************************************************************************/

BOOL HPC_count( double &count )
{
INT64 i;
BOOL ok;

    // Get current count of high performance counter...
    if( (ok=HPC_count(i)) )   
    {
        count = (double)i;
    }

    return(ok);
}

/******************************************************************************/

INT64 HPC_integer( void )
{
INT64 count;
BOOL ok;

    ok = HPC_count(count);

    return(count);
}

/******************************************************************************/

double HPC_double( void )
{
double count;
BOOL ok;

    ok = HPC_count(count);

    return(count);
}

/******************************************************************************/

void  HPC_performance( double *resolution, double *maximum )
{
    if( resolution != NULL )
    {
       *resolution = HPC_Resolution;
    }

    if( maximum != NULL )
    {
       *maximum = HPC_Maximum;
    }
}

/******************************************************************************/

BOOL    TIMER_API_started=FALSE;
struct  TIMER_EveryTAG  TIMER_EveryItem[TIMER_EVERY_MAX];
TIMER   TIMER_EveryTimer("TIMER_EveryTimer");

/******************************************************************************/

void TIMER_Delete( TIMER **object )
{
    if( *object != NULL )
    {
        delete *object;
        *object = NULL;
    }
}

/******************************************************************************/

PRINTF  TIMER_PRN_messgf=NULL;                   // General messages printf function.
PRINTF  TIMER_PRN_errorf=NULL;                   // Error messages printf function.
PRINTF  TIMER_PRN_debugf=NULL;                   // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     TIMER_messgf( const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(TIMER_PRN_messgf,buff));
}

/******************************************************************************/

int     TIMER_errorf( const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(TIMER_PRN_errorf,buff));
}

/******************************************************************************/

int     TIMER_debugf( const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(TIMER_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    TIMER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok=FALSE;
INT64   freq;

    TIMER_PRN_messgf = messgf;         // General API message print function.
    TIMER_PRN_errorf = errorf;         // API error message print function.
    TIMER_PRN_debugf = debugf;         // Debug information print function.

    if( TIMER_API_started )            // Start the API once...
    {
        return(TRUE);
    }

    TIMER_EveryInit();                 // Initialize "Every" handle array (V1.4).

    if( HPC_frequency(freq) )             // Get frequency of high performance counter...
    {
        HPC_CountPerMSec = (double)freq / 1000.0;      // Counts per millisecond.
        HPC_MSecPerCount = 1.0 / HPC_CountPerMSec;      // Milliseconds per tick.

        ok = TRUE;
    }

    if( !ok )
    {
        TIMER_errorf("TIMER_API_start(...) Failed.\n");
        return(FALSE);
    }

    InitializeCriticalSection(&HPC_CS);

    ATEXIT_API(TIMER_API_stop);             // Install stop function.
    TIMER_API_started = TRUE;               // Set started flag.

    MODULE_start(TIMER_PRN_messgf);         // Register module.

    // Timer performance is limited by tick period...
    HPC_Resolution = HPC_MSecPerCount;

    // Timer maximum is limited by use of double precision floats...
    HPC_Maximum = MAX_DOUBLE;

    TIMER_messgf("HPC High Performance Counter (Timer)...\n");
    TIMER_messgf("HPC Frequency: %.0lf (ticks / msec).\n",HPC_CountPerMSec);       
    TIMER_messgf("HPC Resolution: %.10lg (msec).\n",HPC_Resolution);
    TIMER_messgf("HPC Maximum: %.0lf (msec).\n",HPC_Maximum);

    return(ok);
}

/******************************************************************************/

void    TIMER_API_stop( void )
{
    if( !TIMER_API_started )           // API not started in the first place...
    {
         return;
    }

    if( HPC_SkipPatch )
    {
        TIMER_errorf("TIMER_API_stop() Skips=%d Adjust=%d.\n",HPC_SkipCountTotal,(int)HPC_SkipAdjust);
    }

    DeleteCriticalSection(&HPC_CS);

    MODULE_stop();
    TIMER_API_started = FALSE;
}

/******************************************************************************/

BOOL    TIMER_API_check( void )
{
BOOL    ok=TRUE;

    if( !TIMER_API_started )            // Start module automatically...
    {
        ok = TIMER_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        STR_printf(ok,TIMER_debugf,TIMER_errorf,"TIMER_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/
/* Timer class functions...                                                   */
/******************************************************************************/

TIMER::TIMER( char *name, USHORT mode  )
{
    Init(name,mode);
}

/******************************************************************************/

TIMER::TIMER( char *name )
{
    Init(name,TIMER_MODE_NORMAL);
}

/******************************************************************************/

TIMER::TIMER( USHORT mode )
{
    Init(NULL,mode);
}

/******************************************************************************/

TIMER::TIMER( void )
{
    Init(NULL,TIMER_MODE_NORMAL);
}

/******************************************************************************/

TIMER::~TIMER( void )
{
}

/******************************************************************************/

void    TIMER::Init( char *name, USHORT mode )
{
    // Make sure TIMER API is started...
    if( !TIMER_API_check() )
    {
        return;
    }

    Name(name);
    TimerMode = mode;
    TimerMark = 0;

    // Set timer to now..
    Reset();

    TIMER_debugf("TIMER::TIMER(...) %.*s mark=%.3lf\n",STRLEN,ObjectName,HPC_count2msec(TimerMark));
}

/******************************************************************************/

void    TIMER::Name( char *name )
{
    memset(ObjectName,0,STRLEN);

    if( name != NULL )
    {
        strncpy(ObjectName,name,STRLEN);
    }
}

/******************************************************************************/

char   *TIMER::Name( void )
{
    return(ObjectName);
}

/******************************************************************************/

USHORT  TIMER::Mode( void )
{
    return(TimerMode);
}

/******************************************************************************/

INT64    TIMER::Mark( void )
{
    return(TimerMark);
}

/******************************************************************************/

void    TIMER::Reset( void )
{
INT64 count;

    // Reset timer to current time...
    if( HPC_count(count) )
    {
        TimerMark = count;
#ifdef TIMER_DEBUG
        TIMER_debugf("TIMER_reset() %.*s\n",STRLEN,ObjectName);
#endif
    }

}

/******************************************************************************/

void    TIMER::Reset( double msec )
{
INT64 count;

    // Reset timer to current time...
    if( HPC_count(count) )
    {
        TimerMark = count - HPC_msec2count(msec);
#ifdef TIMER_DEBUG
        TIMER_debugf("TIMER_Reset(msec=%.4lf) %.*s\n",msec,STRLEN,ObjectName);
#endif
    }
}

/******************************************************************************/

void    TIMER::ResetSeconds( double sec )
{
    Reset(seconds2milliseconds(sec));
}

/******************************************************************************/

double  TIMER::Elapsed( void )
{
double elapsed;
INT64 count;

    // Get the current value from the high performance counter...
    if( !HPC_count(count) )
    {
        TIMER_errorf("HPC_count(...) %.*s Failed.\n",STRLEN,ObjectName);
        return(0.0);
    }

    // Difference between timer mark and current count...
    elapsed = HPC_msec(TimerMark,count);

    return(elapsed);
}

/******************************************************************************/

double TIMER::ElapsedSeconds( void )
{
double sec;

    sec = milliseconds2seconds(Elapsed());

    return(sec);
}

/******************************************************************************/

double TIMER::ElapsedMinutes( void )
{
double min;

    min = ElapsedSeconds() / 60.0;

    return(min);
}

/******************************************************************************/

double TIMER::ElapsedHours( void )
{
double hr;

    hr = ElapsedMinutes() / 60.0;

    return(hr);
}

/******************************************************************************/

BOOL TIMER::Expired( double msec )
{
double elapsed;
BOOL expired;

    // Check elapsed time...
    elapsed = Elapsed();
    expired = (elapsed >= msec);

    // Has timer expired...
    if( expired )
    {
#ifdef TIMER_DEBUG
        TIMER_debugf("TIMER::Expired(%.*s) EXPIRED %.3lf msec=%.3lf\n",
                     STRLEN,ObjectName,
                     elapsed,
                     msec);
#endif

        if( TimerMode & TIMER_MODE_RESET )
        {                              // Automatically reset timer upon expiration...
            Reset();
        }
    }

    return(expired);
}

/******************************************************************************/

BOOL TIMER::ExpiredSeconds( double sec )
{
BOOL expired;

    expired = Expired(seconds2milliseconds(sec));

    return(expired);
}

/******************************************************************************/

BOOL TIMER::Range( double msec1, double msec2 )
{
double elapsed;
BOOL range;

    elapsed = Elapsed();
    range = (elapsed >= msec1) && (elapsed <= msec2);

    return(range);
}

/******************************************************************************/

BOOL TIMER::Range( double msec[] )
{
BOOL range;

    range = Range(msec[0],msec[1]);

    return(range);
}

/******************************************************************************/

BOOL TIMER::RangeSeconds( double sec1, double sec2 )
{
BOOL range;

    range = Range(seconds2milliseconds(sec1),seconds2milliseconds(sec2));

    return(range);
}

/******************************************************************************/

BOOL TIMER::RangeSeconds( double sec[] )
{
BOOL range;

    range = Range(seconds2milliseconds(sec[0]),seconds2milliseconds(sec[1]));

    return(range);
}


/******************************************************************************/

void    TIMER::Wait( double msec )
{
    while( !Expired(msec) );
}

/******************************************************************************/

void    TIMER::WaitSeconds( double sec )
{
    while( !ExpiredSeconds(sec) );
}

/******************************************************************************/
/* Time functions.                                                            */
/******************************************************************************/

double  TIMER_milliseconds( void )
{
_timeb  now;
double  msec;

   _ftime(&now);
    msec = ((double)now.time * 1000.0) + (double)now.millitm;

    return(msec);
}

/******************************************************************************/

double  TIMER_seconds( void )
{
_timeb  now;
double  sec;

   _ftime(&now);
    sec = (double)now.time + ((double)now.millitm / 1000.0);

    return(sec);
}

/******************************************************************************/
/* Delay functions.                                                           */ 
/******************************************************************************/

void    TIMER_delay( double msec )
{
    TIMER_delay(msec,NULL);
}

/******************************************************************************/

void    TIMER_delay( double msec, void (*func)( void ) )
{
double  mark,current;

    // Set mark time...
    mark = HPC_msec();

    // Loop until expire time is reached...
    do
    {
        current = HPC_msec();
        if( func != NULL ) (*func)();
    }
    while( (msec + mark) > HPC_msec() );
}

/******************************************************************************/
/* Frequency Timer...                                                    V1.2 */
/******************************************************************************/

TIMER_Frequency::TIMER_Frequency( char *name, int points )
{
    Init(name,points);
}

/******************************************************************************/

TIMER_Frequency::TIMER_Frequency( char *name )
{
    Init(name,TIMER_DATA);
}

/******************************************************************************/

TIMER_Frequency::TIMER_Frequency( void )
{
    Init(NULL,TIMER_DATA);
}

/******************************************************************************/

TIMER_Frequency::~TIMER_Frequency( void )
{
    if( timer != NULL )
    {
        delete timer;
        timer = NULL;
    }

    if( fdata != NULL )
    {
        delete fdata;
        fdata = NULL;
    }

    if( idata != NULL )
    {
        delete idata;
        idata = NULL;
    }
}

/******************************************************************************/

void    TIMER_Frequency::Init( void )
{
    memset(ObjectName,0,STRLEN);

    timer = NULL;
    fdata = NULL;
    idata = NULL;

    Zero();
}

/******************************************************************************/

void    TIMER_Frequency::Init( char *name, int points )
{
    Init(); 
    Name(name);

    timer = new TIMER(ObjectName);
    fdata = new DATAPROC(STR_stringf("%s-F",ObjectName),points);
    idata = new DATAPROC(STR_stringf("%s-I",ObjectName),points);

    Reset();
}

/******************************************************************************/

void    TIMER_Frequency::Zero( void )
{
    first = TRUE;
    count = 0;
    done = FALSE;
    elapsed_last = 0.0;
    frequency_mean = 0.0;
    frequency_sd = 0.0;
    frequency_min = 0.0;
    frequency_max = 0.0;
    interval_mean = 0.0;
    interval_sd = 0.0;
    interval_min = 0.0;
    interval_max = 0.0;
}

/******************************************************************************/

void    TIMER_Frequency::Name( char *name )
{
    if( name != NULL )
    {
        strncpy(ObjectName,name,STRLEN);
    }
}

/******************************************************************************/

void    TIMER_Frequency::Reset( char *name )
{
    Name(name);
    Zero();

    timer->Reset();
    fdata->Reset();
    idata->Reset();
}

/******************************************************************************/

void    TIMER_Frequency::Reset( void )
{
    Reset(NULL);
}

/******************************************************************************/

double  TIMER_Frequency::Loop( void )
{
double dt,elapsed;

    if( first )
    {
        first = FALSE;
        timer->Reset();
        elapsed_last = 0.0;
        return(0.0);
    }

    elapsed = timer->Elapsed();

    dt = elapsed - elapsed_last;
    idata->Data(dt);

    if( dt > 0.0 )
    {
        fdata->Data(1.0/(dt/1000.0));
    }

    elapsed_last = elapsed;
    count++;

    if( elapsed > 0.0 )
    {
        frequency = (double)count / (elapsed/1000.0);
    }

    return(dt);
}

/******************************************************************************/

double TIMER_Frequency::Elapsed( void )
{
double elapsed;

    elapsed = timer->Elapsed()-elapsed_last;

    return(elapsed);
}

/******************************************************************************/

double TIMER_Frequency::ElapsedSeconds( void )
{
double elapsed;

    elapsed = Elapsed() / 1000.0;

    return(elapsed);
}

/******************************************************************************/

void    TIMER_Frequency::Calculate( void )
{
    if( done )
    {
        return;
    }

    frequency_mean = fdata->Mean();;
    frequency_sd   = fdata->SD();
    frequency_min  = fdata->Min();
    frequency_max  = fdata->Min();

    interval_mean = idata->Mean();
    interval_sd   = idata->SD();
    interval_min  = idata->Min();
    interval_max  = idata->Max();

    done = TRUE;
}

/******************************************************************************/

char   *TIMER_Frequency::Name( void )
{
    return(ObjectName);
}

/******************************************************************************/

int     TIMER_Frequency::Count( void )
{
    return(count);
}

/******************************************************************************/

double  TIMER_Frequency::Frequency( void )
{
    return(frequency);
}

/******************************************************************************/

double  TIMER_Frequency::FrequencyMean( void )
{
    Calculate();
    return(frequency_mean);
}

/******************************************************************************/

double  TIMER_Frequency::FrequencySD( void )
{
    Calculate();
    return(frequency_sd);
}

/******************************************************************************/

double  TIMER_Frequency::FrequencyMin( void )
{
    Calculate();
    return(frequency_min);
}

/******************************************************************************/

double  TIMER_Frequency::FrequencyMax( void )
{
    Calculate();
    return(frequency_max);
}

/******************************************************************************/

double  TIMER_Frequency::IntervalMean( void )
{
    Calculate();
    return(interval_mean);
}

/******************************************************************************/

double  TIMER_Frequency::IntervalSD( void )
{
    Calculate();
    return(interval_sd);
}

/******************************************************************************/

double  TIMER_Frequency::IntervalMin( void )
{
    Calculate();
    return(interval_min);
}

/******************************************************************************/

double  TIMER_Frequency::IntervalMax( void )
{
    Calculate();
    return(interval_max);
}

/******************************************************************************/

void    TIMER_Frequency::Results( STRING n, double &freq, double &int_mn, double &int_sd, int &count )
{
    if( n != NULL )
    {
        strncpy(n,Name(),STRLEN);
    }

    freq = FrequencyMean();
    int_mn = IntervalMean();
    int_sd = IntervalSD();
    count = Count();
}

/******************************************************************************/

void    TIMER_Frequency::Results( double &freq, double &int_mn, double &int_sd )
{
    freq = FrequencyMean();
    int_mn = IntervalMean();
    int_sd = IntervalSD();
}

/******************************************************************************/

void    TIMER_Frequency::Results( PRINTF prnf )
{
    if( Count() > 0 )
    {
        prnf("%.*s Frequency=%.2lfHz Period=%.3lf%c%.3lfmsec (n=%d)\n",STRLEN,Name(),Frequency(),IntervalMean(),PLUSMINUS,IntervalSD(),Count());
        prnf("Period Range: %.3lf %.3lf (msec)\n",IntervalMin(),IntervalMax());
    }
}

/******************************************************************************/

void    TIMER_Frequency::Results( void )
{
    Results(printf);
}

/******************************************************************************/

void TIMER_Frequency::Results( BOOL SaveFlag )
{
BOOL ok;

    Results();

    if( SaveFlag && (Count() > 0) )
    {
        ok = iData()->Save();
    }
}

/******************************************************************************/

DATAPROC *TIMER_Frequency::fData( void )
{
    return(fdata);
}

/******************************************************************************/

DATAPROC *TIMER_Frequency::iData( void )
{
    return(idata);
}

/******************************************************************************/
/* Functions to time something "every" X msecs. (V1.4)                        */
/******************************************************************************/

void    TIMER_EveryInit( void )
{
int     item;

    for( item=0; (item < TIMER_EVERY_MAX); item++ )
    {
        memset(&TIMER_EveryItem[item],0,sizeof(struct TIMER_EveryTAG));
        TIMER_EveryItem[item].used = FALSE;
    }
}

/******************************************************************************/

short   TIMER_EveryHandle( void )
{
short   item,handle;

    if( !TIMER_API_check() )                // Make sure the TIMER API is started.
    {
        return(TIMER_EVERY_INVALID);
    }

    for( handle=TIMER_EVERY_INVALID,item=0; (item < TIMER_EVERY_MAX); item++ )
    {
         if( TIMER_EveryItem[item].used )
         {
             continue;
         }

         handle = item;
         break;
    }

    return(handle);
}

/******************************************************************************/

short   TIMER_EveryFind( char *name )
{
short   item,find;

    if( !TIMER_API_check() )                // Make sure the TIMER API is started.
    {
        return(TIMER_EVERY_INVALID);
    }

    for( find=TIMER_EVERY_INVALID,item=0; (item < TIMER_EVERY_MAX); item++ )
    {
        if( TIMER_EveryItem[item].used )
        {
            if( strncmp(TIMER_EveryItem[item].name,name,STRLEN) == 0 )
            {
                find = item;
            }
        }
    }

    return(find);
}

/******************************************************************************/

BOOL    TIMER_EveryCheck( short item )
{
BOOL    ok=TRUE;

    if( !TIMER_API_check() )
    {
        ok = FALSE;
    }
    else
    if( item == TIMER_EVERY_INVALID )
    {
        ok = FALSE;
    }
    else
    if( !TIMER_EveryItem[item].used )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

short   TIMER_EveryStart( char *name )
{
short   item;

    if( (item=TIMER_EveryFind(name)) != TIMER_EVERY_INVALID )
    {
        return(item);
    }

    if( (item=TIMER_EveryHandle()) == TIMER_EVERY_INVALID )
    {
        TIMER_errorf("TIMER_EveryStart(name=%s) No free handles.\n",name);
        return(TIMER_EVERY_INVALID);
    }

     memset(&TIMER_EveryItem[item],0,sizeof(struct TIMER_EveryTAG));
     TIMER_EveryItem[item].used = TRUE;
     TIMER_EveryItem[item].last = 0.0;
     strncpy(TIMER_EveryItem[item].name,name,STRLEN);

     TIMER_debugf("TIMER_EveryStart() %.*s[%02d]\n",STRLEN,name,item);

     return(item);
}

/******************************************************************************/

void    TIMER_EveryReset( short item )
{
    if( TIMER_EveryCheck(item) )
    {
        TIMER_EveryItem[item].last = TIMER_EveryTimer.Elapsed();
    }    
}

/******************************************************************************/

void    TIMER_EveryReset( char *name )
{
    TIMER_EveryReset(TIMER_EveryFind(name));
}

/******************************************************************************/

BOOL    TIMER_EveryTime( short item, double msec )
{
BOOL    ok=FALSE;

    if( !TIMER_EveryCheck(item) )
    {
        return(FALSE);
    }

    if( TIMER_EveryItem[item].last == 0.0 )
    {
        ok = TRUE;
    }
    else
    if( (TIMER_EveryTimer.Elapsed()-TIMER_EveryItem[item].last) >= msec )
    {
        ok = TRUE;
    }

    if( ok )
    {
        TIMER_EveryReset(item);
    }

    return(ok);
}

/******************************************************************************/

BOOL    TIMER_EveryTime( char *name, double msec, BOOL create )
{
short   item;
BOOL    ok;

    item = create ? TIMER_EveryStart(name) : TIMER_EveryFind(name);
    ok = TIMER_EveryTime(item,msec);

    return(ok);
}

/******************************************************************************/

void    TIMER_EveryStop( short item )
{
    if( TIMER_EveryCheck(item) )
    {
        memset(&TIMER_EveryItem[item],0,sizeof(struct TIMER_EveryTAG));
        TIMER_EveryItem[item].used = FALSE;
    }
}

/******************************************************************************/

void    TIMER_EveryStop( char *name )
{
    TIMER_EveryStop(TIMER_EveryFind(name));
}

/******************************************************************************/
/* Interval Timer...                                                          */
/******************************************************************************/

TIMER_Interval::TIMER_Interval( char *name, int points )
{
    Init(name,points);
}

/******************************************************************************/

TIMER_Interval::TIMER_Interval( char *name )
{
    Init(name,TIMER_DATA);
}

/******************************************************************************/

TIMER_Interval::TIMER_Interval( void )
{
    Init(NULL,TIMER_DATA);
}

/******************************************************************************/

TIMER_Interval::~TIMER_Interval( void )
{
    delete timer;
    delete data;
}

/******************************************************************************/

void    TIMER_Interval::Init( void )
{
    memset(ObjectName,0,STRLEN);
    timer = NULL;
    data = NULL;
}

/******************************************************************************/

void    TIMER_Interval::Init( char *name, int points )
{
    Init();
    Name(name);

    timer = new TIMER(STR_stringf("%s",ObjectName));
    data = new DATAPROC(STR_stringf("%s",ObjectName),points);

    Reset();
}

/******************************************************************************/

void    TIMER_Interval::Name( STRING name )
{
    if( name != NULL )
    {
        strncpy(ObjectName,name,STRLEN);
    }
}

/******************************************************************************/

void    TIMER_Interval::Reset( char *name )
{
    Name(name);
    data->Reset();
    done = FALSE;
    before = FALSE;
}

/******************************************************************************/

void    TIMER_Interval::Reset( void )
{
    Reset(NULL);
}

/******************************************************************************/

void    TIMER_Interval::Before( void )
{
    before = TRUE;
    timer->Reset();
}

/******************************************************************************/

double  TIMER_Interval::After( void )
{
double latency=0.0;

    if( before )
    {
        latency = timer->Elapsed();
        data->Data(latency);
        before = FALSE;
    }

    return(latency);
}

/******************************************************************************/

void    TIMER_Interval::Calculate( void )
{
    if( done )
    {
        return;
    }

    count = data->Count();
    interval = data->Mean();
    min = data->Min();
    max = data->Max();
    sd = data->SD();

    done = TRUE;
}

/******************************************************************************/

char   *TIMER_Interval::Name( void )
{
    return(ObjectName);
}

/******************************************************************************/

double  TIMER_Interval::Result( void )
{
double  interval=0.0;

    interval = data->GetD(DATAPROC_GET_LAST);

    return(interval);
}

/******************************************************************************/

double  TIMER_Interval::IntervalMean( void )
{
    Calculate();
    return(interval);
}

/******************************************************************************/

double  TIMER_Interval::IntervalMin( void )
{
    Calculate();
    return(min);
}


/******************************************************************************/
double  TIMER_Interval::IntervalMax( void )
{
    Calculate();
    return(max);
}

/******************************************************************************/

double  TIMER_Interval::IntervalSD( void )
{
    Calculate();
    return(sd);
}

/******************************************************************************/

int     TIMER_Interval::Count( void )
{
    count = data->Count();
    return(count);
}

/******************************************************************************/

void    TIMER_Interval::Results( STRING n, double &int_mn, double &int_min, double &int_max, double &int_sd, int &count )
{
    if( n != NULL )
    {
        strncpy(n,Name(),STRLEN);
    }

    int_mn = IntervalMean();
    int_min = IntervalMin();
    int_max = IntervalMax();
    int_sd = IntervalSD();
    count = Count();
}

/******************************************************************************/

void    TIMER_Interval::Results( STRING n, double &int_mn, double &int_sd, int &count )
{
    if( n != NULL )
    {
        strncpy(n,Name(),STRLEN);
    }

    int_mn = IntervalMean();
    int_sd = IntervalSD();
    count = Count();
}

/******************************************************************************/

void    TIMER_Interval::Results( STRING n, double &int_mn )
{
    if( n != NULL )
    {
        strncpy(n,Name(),STRLEN);
    }

    int_mn = IntervalMean();
}

/******************************************************************************/

void    TIMER_Interval::Results( PRINTF prnf )
{
    if( Count() > 0 )
    {
        prnf("%s Interval=%.3lf%c%.3lfmsec Min=%.3lf Max=%.3lf (n=%d)\n",Name(),IntervalMean(),PLUSMINUS,IntervalSD(),IntervalMin(),IntervalMax(),Count());
    }
}

/******************************************************************************/

void    TIMER_Interval::Results( void )
{
    Results(printf);
}

/******************************************************************************/

void TIMER_Interval::Results( BOOL SaveFlag )
{
BOOL ok;

    Results();

    if( SaveFlag && (Count() > 0) )
    {
        ok = Data()->Save();
    }
}

/******************************************************************************/

DATAPROC *TIMER_Interval::Data( void )
{
    return(data);
}

/******************************************************************************/

