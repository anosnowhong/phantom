/******************************************************************************/

#define RAMP_DEBUG() if( !RAMP_API_start(printf,printf,printf) ) { printf("Cannot start RAMP API.\n"); exit(0); }

/******************************************************************************/

// LoopTask frequency and time step...
#define RAMP_LOOPTASK   100.0
#define RAMP_TIMESTEP   1.0 / RAMP_LOOPTASK

#define RAMP_TIME       2.0

/******************************************************************************/

class RAMP
{
private:
    BOOL OpenFlag;

    STRING ObjectName;

    double RampTime;
    double RampValue;
    BOOL (*RampFlag)( void );
    double RampStep;
    BOOL RampReset;

    BOOL StartedFlag;

public:

    RAMP( char *name );
   ~RAMP( void );

    void Init( void );
    BOOL Init( char *name );

    BOOL Open( void );
    BOOL Started( void );

    BOOL Start( double ramp, BOOL (*flag)( void ) );
    BOOL Start( double ramp );
    BOOL Start( void );
    void Stop( void );

    void LoopTask( void );

    void Reset( void );
    void Reset( BOOL flag );

    BOOL Ramp( void );
    double RampCurrent( void );
    double RampDouble( double value );
    long RampLong( long value );
};

/******************************************************************************/

void RAMP_LoopTask( void );

/******************************************************************************/

#define RAMP_ITEM       16
#define RAMP_INVALID    -1

/******************************************************************************/

int  RAMP_Handle( RAMP *item );
BOOL RAMP_Create( RAMP *item );
void RAMP_Delete( RAMP *item );

/******************************************************************************/

BOOL RAMP_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
BOOL RAMP_API_check( void );
void RAMP_API_stop( void );

/******************************************************************************/

int RAMP_errorf( const char *mask, ... );
int RAMP_messgf( const char *mask, ... );
int RAMP_debugf( const char *mask, ... );

/******************************************************************************/

