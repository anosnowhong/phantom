/******************************************************************************/
/*                                                                            */ 
/* MODULE  : LoopTask.h                                                       */ 
/*                                                                            */ 
/* PURPOSE : Loop Task (for "back-ground" execution) functions.               */ 
/*                                                                            */ 
/* DATE    : 14/Oct/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 14/Oct/2000 - Initial development.                               */ 
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

#define LOOPTASK_DEBUG() if( !LOOPTASK_API_start(printf,printf,printf) ) { printf("Cannot start LOOPTASK API.\n"); exit(0); }
#define LOOPTASK_CHECK() { extern BOOL LOOPTASK_Check; LOOPTASK_Check=TRUE; }

/******************************************************************************/

typedef void (*LOOPTASK)( void );      // LOOPTASK function pointer.

/******************************************************************************/

#define LOOPTASK_LIST        20        // Maximum number of concurrent tasks.
#define LOOPTASK_INVALID     -1        // Invalid item handle value.

/******************************************************************************/

struct  LOOPTASK_Item                  // Information for task.
{
    LOOPTASK  func;                    // Pointer to loop task function.
    BOOL      skip;                    // Skip execution (task suspended).
    double    freq;                    // Frequency of execution (Hz).
    double    period;                  // Period (milliseconds).
    long      loops;                   // Maximum number of loops to execute.
    int       fdivider;                // Frequency divider for base frequency.
    long      count;                   // Number of times executed.
    double    msec;                    // Fixed millisecond counter (based on frequency).
    BOOL      first;                   // TRUE for the first time function is called.

    double    tnow;                    // Current real-time elapsed (based on real-time).
    double    tlast;                   // Time of last iteration.
    double    dt;                      // Time since last iteration.

    TIMER    *timer;                   // Timing stuff (V1.2)...
    double    last;                    // Time of last execution.
    double    tpv;                     // Total period variance.
    double    pmin;                    // Period minimum.
    double    pmax;                    // Period maximum.
    double    et;                      // Execution time (V2.7).
    double    tet;                     // Total execution time.

    STRING    name;                    // Optional name (V1.2).

    BOOL      terminate;               // Terminate before next execution.

    int       priority;                // Task priority (V2.2).
#define LOOPTASK_PRIORITY_HIGHEST 0
#define LOOPTASK_PRIORITY_HIGH    1
#define LOOPTASK_PRIORITY_NORMAL  2
#define LOOPTASK_PRIORITY_LOW     3
#define LOOPTASK_PRIORITY_DEFAULT LOOPTASK_PRIORITY_HIGH
};

/******************************************************************************/

BOOL    LOOPTASK_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    LOOPTASK_API_stop( void );
BOOL    LOOPTASK_API_check( void );

/******************************************************************************/

int     LOOPTASK_messgf( const char *mask, ... );
int     LOOPTASK_errorf( const char *mask, ... );
int     LOOPTASK_debugf( const char *mask, ... );

/*****************************************************************************/

void    LOOPTASK_MMTIMER_Task( void );
BOOL    LOOPTASK_MMTIMER_Start( UINT period );
BOOL    LOOPTASK_MMTIMER_Stop( void );

/******************************************************************************/

#define LOOPTASK_FREQUENCY(F)  { double fHz=(F); LOOPTASK_frequency(fHz); }

void    LOOPTASK_frequency( double &frequency );
double  LOOPTASK_frequency( void );

double  LOOPTASK_period( void );

int     LOOPTASK_free( int first, int last );
int     LOOPTASK_free( int priority );

int     LOOPTASK_find( LOOPTASK func );

int     LOOPTASK_fmodulus( double baseHz, double &taskHz );
BOOL    LOOPTASK_fvalid( double baseHz, double taskHz );

void    LOOPTASK_init( int item );
void    LOOPTASK_init( void );

BOOL    LOOPTASK_start( char *name, LOOPTASK func, double freq, long loops, int priority );
BOOL    LOOPTASK_start( char *name, LOOPTASK func, double freq, long loops );
BOOL    LOOPTASK_start( char *name, LOOPTASK func, double freq, double duration, int priority  );
BOOL    LOOPTASK_start( char *name, LOOPTASK func, double freq, double duration );
BOOL    LOOPTASK_start( char *name, LOOPTASK func, double freq, int priority  );
BOOL    LOOPTASK_start( char *name, LOOPTASK func, double freq );
BOOL    LOOPTASK_start( char *name, LOOPTASK func, int priority  );
BOOL    LOOPTASK_start( char *name, LOOPTASK func );

BOOL    LOOPTASK_start( LOOPTASK func, double freq, long loops, int priority );
BOOL    LOOPTASK_start( LOOPTASK func, double freq, long loops );
BOOL    LOOPTASK_start( LOOPTASK func, double freq, double duration, int priority );
BOOL    LOOPTASK_start( LOOPTASK func, double freq, double duration );
BOOL    LOOPTASK_start( LOOPTASK func, double freq, int priority );
BOOL    LOOPTASK_start( LOOPTASK func, double freq );
BOOL    LOOPTASK_start( LOOPTASK func, int priority );
BOOL    LOOPTASK_start( LOOPTASK func );

void    LOOPTASK_stop( LOOPTASK func );
void    LOOPTASK_stop( int item );
void    LOOPTASK_stop( void );

void    LOOPTASK_terminate( LOOPTASK func );
void    LOOPTASK_terminate( int item );
void    LOOPTASK_terminate( void );

void    LOOPTASK_suspend( LOOPTASK func );
void    LOOPTASK_resume( LOOPTASK func );

void    LOOPTASK_suspend( void );
void    LOOPTASK_resume( void );

void    LOOPTASK_timing( int item );

long    LOOPTASK_count( LOOPTASK func );
BOOL    LOOPTASK_running( LOOPTASK func );
double  LOOPTASK_frequency( LOOPTASK func );

void    LOOPTASK_loop( void );

BOOL    LOOPTASK_check( int item );

void    LOOPTASK_reset( void );
void    LOOPTASK_reset( int item );
void    LOOPTASK_reset( LOOPTASK func );

double  LOOPTASK_msec( void );
double  LOOPTASK_msec( int item );
double  LOOPTASK_msec( LOOPTASK func );

double  LOOPTASK_second( void );
double  LOOPTASK_second( int item );
double  LOOPTASK_second( LOOPTASK func );

/******************************************************************************/

// Time variables based on loop counts...
extern double LOOPTASK_Time;
extern double LOOPTASK_TimeSeconds;

// The following variables contain information about the currently executing function (V2.3).
// They are available to the application and should be reference only from within the function.
extern  int     LOOPTASK_Current;
extern  double  LOOPTASK_Period;
extern  double  LOOPTASK_Frequency;
extern  BOOL    LOOPTASK_First;
extern  long    LOOPTASK_Counter;
extern  double  LOOPTASK_Elapsed;
extern  double  LOOPTASK_ElapsedSeconds;
extern  double  LOOPTASK_dt;
extern  double  LOOPTASK_Latency;
extern  double  LOOPTASK_TotalLatency;

/******************************************************************************/

void LOOPTASK_TimerSkipPatch( void );

/******************************************************************************/

void PRIORITY_SetHighest( void );
void PRIORITY_SetNormal( void );

/******************************************************************************/

