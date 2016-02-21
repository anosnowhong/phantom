/******************************************************************************/
/*                                                                            */ 
/* MODULE  : timer.h                                                          */ 
/*                                                                            */ 
/* PURPOSE : Timer functions.                                                 */ 
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
/* V2.4  JNI 10/Apr/2015 - Tweaks to "Every" functions and increase handles.  */
/*                                                                            */ 
/******************************************************************************/

#ifndef TIMER_H
#define TIMER_H

/******************************************************************************/

double seconds2milliseconds( double seconds );
double milliseconds2seconds( double milliseconds );
double seconds2minutes( double seconds );
double minutes2seconds( double minutes );
double minutes2hours( double minutes );
double seconds2hours( double seconds );

/******************************************************************************/

double  HPC_count2msec( INT64 count );
INT64   HPC_msec2count( double msec );

double  HPC_msec( void );
double  HPC_msec( INT64 t0, INT64 t1 );

BOOL    HPC_frequency( LARGE_INTEGER *PF );
BOOL    HPC_frequency( INT64 &frequency );

void    HPC_patch( void );

BOOL    HPC_count( LARGE_INTEGER *PC );
BOOL    HPC_count( INT64 &count );
BOOL    HPC_count( double &count );

INT64   HPC_integer( void );
double  HPC_double( void );

void    HPC_performance( double *resolution, double *maximum );

extern  BOOL  HPC_SkipPatch;
extern  int   HPC_SkipCountTotal;
extern  int   HPC_SkipCount;

void HPC_SkipCountReset( void );

/******************************************************************************/

class   TIMER                          // TIMER class...
{
private:

    USHORT    TimerMode;               // Mode of operation for timer...
#define TIMER_MODE_NORMAL    0x0000    // Don't do anything when it expires.
#define TIMER_MODE_RESET     0x0001    // Reset the timer when it expires.
#define TIMER_MODE_FREE      0x0002    // Free the timer when it expires.
#define TIMER_MODE_NAME      0x0004    // Make sure name is unique.

    STRING    ObjectName;              // String name for timer.
    INT64     TimerMark;               // Mark time.

public:

    TIMER( char *name, USHORT mode );
    TIMER( char *name );
    TIMER( USHORT mode );
    TIMER( void );
   ~TIMER( void );

    void Init( char *name, USHORT mode );
    void Name( char *name );
    char *Name( void );
    USHORT Mode( void );
    INT64 Mark( void );
    void Reset( void );
    void Reset( double msec );
    void ResetSeconds( double sec );
    double Elapsed( void );
    double ElapsedSeconds( void );
    double ElapsedMinutes( void );
    double ElapsedHours( void );
    BOOL Expired( double msec );
    BOOL ExpiredSeconds( double sec );
    BOOL Range( double msec1, double msec2 );
    BOOL Range( double msec[] );
    BOOL RangeSeconds( double sec1, double sec2 );
    BOOL RangeSeconds( double sec[] );
    void Wait( double msec );
    void WaitSeconds( double sec );
};

/******************************************************************************/

/******************************************************************************/

void    TIMER_Delete( TIMER **object );

/******************************************************************************/

int     TIMER_messgf( const char *mask, ... );
int     TIMER_errorf( const char *mask, ... );
int     TIMER_debugf( const char *mask, ... );

BOOL    TIMER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    TIMER_API_stop( void );
BOOL    TIMER_API_check( void );

void    TIMER_init( void );

/******************************************************************************/

double  TIMER_milliseconds( void );
double  TIMER_seconds( void );

/******************************************************************************/

void    TIMER_delay( double msec );
void    TIMER_delay( double msec, void (*func)( void ) );

/******************************************************************************/

#define TIMER_EVERY_MAX       128      // Maximum number of "Every" timers (V1.4).
#define TIMER_EVERY_INVALID    -1      // Invalid "Every" timer handle. 

struct  TIMER_EveryTAG                 // Handle TAG for "every X msec" timer (V1.4)...
{
    BOOL      used;
    double    last;
    STRING    name;
};

void    TIMER_EveryInit( void );
short   TIMER_EveryHandle( void );
short   TIMER_EveryFind( char *name );
BOOL    TIMER_EveryCheck( short item );
short   TIMER_EveryStart( char *name );
BOOL    TIMER_EveryTime( short item, double msec );
BOOL    TIMER_EveryTime( char *name, double msec, BOOL create );
void    TIMER_EveryReset( short item );
void    TIMER_EveryReset( char *name );
void    TIMER_EveryStop( short item );
void    TIMER_EveryStop( char *name );

#define TIMER_Every(M)       TIMER_EveryTime(STR_stringf("%s[%d]",__FILE__,__LINE__),(M),TRUE)
#define TIMER_EverySecond(S) TIMER_EveryTime(STR_stringf("%s[%d]",__FILE__,__LINE__),seconds2milliseconds(S),TRUE)
#define TIMER_EveryMinute(M) TIMER_EveryTime(STR_stringf("%s[%d]",__FILE__,__LINE__),seconds2milliseconds(minutes2seconds(M)),TRUE)
#define TIMER_EveryHz(F)     TIMER_EveryTime(STR_stringf("%s[%d]",__FILE__,__LINE__),1000.0 / (F),TRUE)

/******************************************************************************/

// The TIMER objects below require especially large numbers of data points...
#define TIMER_DATA 50000

/******************************************************************************/

class   TIMER_Frequency
{
private:
    
    STRING ObjectName;

    TIMER *timer;

    BOOL done;

    BOOL first;
    int count;
    double elapsed_last;

    double frequency;

    double frequency_mean;
    double frequency_sd;
    double frequency_min;
    double frequency_max;

    double interval_mean;
    double interval_sd;
    double interval_min;
    double interval_max;

    DATAPROC *fdata;
    DATAPROC *idata;

public:

    TIMER_Frequency( char *name, int points );
    TIMER_Frequency( char *name );
    TIMER_Frequency( void );
   ~TIMER_Frequency( void );

    void Init( void );
    void Init( char *name, int points );
    void Zero( void );
    void Name( char *name );
    void Reset( char *name );
    void Reset( void );
    double Loop( void );
    void Calculate( void );
    char *Name( void );
    int Count( void );
    double Elapsed( void );
    double ElapsedSeconds( void );

    double Frequency( void );
    double FrequencyMean( void );
    double FrequencySD( void );
    double FrequencyMin( void );
    double FrequencyMax( void );

    double IntervalMean( void );
    double IntervalSD( void );
    double IntervalMin( void );
    double IntervalMax( void );

    void Results( STRING name, double &freq, double &int_mn, double &int_sd, int &count );
    void Results( double &freq, double &int_mn, double &int_sd );
    void Results( PRINTF prnf );
    void Results( BOOL SaveFlag );
    void Results( void );

    DATAPROC *fData( void );
    DATAPROC *iData( void );
};

/******************************************************************************/

class   TIMER_Interval
{
private:

    STRING ObjectName;

    TIMER *timer;

    BOOL done;
    BOOL before;

    int count;
    double interval;
    double min;
    double max;
    double sd;

    DATAPROC *data;

public:

    TIMER_Interval( char *name, int points );
    TIMER_Interval( char *name );
    TIMER_Interval( void );
   ~TIMER_Interval( void );

    void Init( void );
    void Init( char *name, int points );
    void Name( char *name );
    void Reset( char *name );
    void Reset( void );
    void Before( void );
    double After( void );
    void Calculate( void );
    char *Name( void );
    double Result( void );
    double IntervalMean( void );
    double IntervalMin( void );
    double IntervalMax( void );
    double IntervalSD( void );
    int Count( void );

    void Results( STRING name, double &int_mn, double &int_min, double &int_max, double &int_sd, int &count );
    void Results( STRING name, double &int_mn, double &int_sd, int &count );
    void Results( STRING name, double &int_mn );
    void Results( PRINTF prnf );
    void Results( BOOL SaveFlag );
    void Results( void );

    DATAPROC *Data( void );
};

/******************************************************************************/

#endif

