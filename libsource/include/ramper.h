/******************************************************************************/
/*                                                                            */
/* MODULE  : RAMPER.h                                                         */
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

#define RAMPER_DEBUG() if( !RAMPER_API_start(printf,printf,printf) ) { printf("Cannot start RAMPER API.\n"); exit(0); }

/******************************************************************************/

// LoopTask frequency and time step...
#define RAMPER_LOOPTASK   1000.0
#define RAMPER_TIMESTEP   1.0 / RAMPER_LOOPTASK

/******************************************************************************/

class RAMPER
{
private:
    BOOL OpenFlag;
    BOOL StartFlag;

    STRING ObjectName;

    double RampTime;
    double RampValue;
    BOOL (*RampFlag)( void );
    double RampStep;
    double RampSign;
    BOOL   RampReset;

    double HoldTime;
    TIMER  HoldTimer;

public:

    RAMPER( void );
    RAMPER( char *name );
   ~RAMPER( void );

    void Init( void );
    void Init( char *name );

    void RampTimeSet( double ramptime );
    void HoldTimeSet( double holdtime );

    BOOL Opened( void );
    BOOL Started( void );

    BOOL Open( double ramptime, double holdtime, BOOL (*flag)( void ) );
    BOOL Open( double ramptime, double holdtime );
    BOOL Open( double ramptime );
    BOOL Open( void );
    void Close();

    BOOL Start( double ramptime, double holdtime, BOOL (*flag)( void ) );
    BOOL Start( double ramptime, double holdtime );
    BOOL Start( double ramptime );
    BOOL Start( void );
    void Stop( void );

    void LoopTask( void );

    void One( void );
    void Zero( void );

    void Reset( void );
    void Reset( BOOL flag );

    void Direction( double direction );
    void Up( void );
    void Down( void );

    BOOL Ramp( void );
    double RampCurrent( void );
    double RampDouble( double value );
    long RampLong( long value );
    matrix RampMatrix( matrix &mtx );
    BOOL RampComplete();
};

/******************************************************************************/

void RAMPER_LoopTask( void );

/******************************************************************************/

#define RAMPER_MAX        10
#define RAMPER_INVALID    -1

/******************************************************************************/

int  RAMPER_Handle( RAMPER *item );
BOOL RAMPER_Create( RAMPER *item );
void RAMPER_Delete( RAMPER *item );

/******************************************************************************/

BOOL RAMPER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
BOOL RAMPER_API_check( void );
void RAMPER_API_stop( void );

/******************************************************************************/

int RAMPER_errorf( const char *mask, ... );
int RAMPER_messgf( const char *mask, ... );
int RAMPER_debugf( const char *mask, ... );

/******************************************************************************/

