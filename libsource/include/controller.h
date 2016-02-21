/******************************************************************************/
/*                                                                            */
/* MODULE  : CONTROLLER.h                                                     */
/*                                                                            */
/* PURPOSE : Robot controller interface.                                      */
/*                                                                            */
/* DATE    : 16/Aug/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 16/Aug/2002 - Initial development.                               */
/*                                                                            */
/* V1.3  JNI 06/Jun/2009 - Added OptoTrak as a host-card type.                */
/*                                                                            */
/******************************************************************************/

int     CONTROLLER_messgf( const char *mask, ... );
int     CONTROLLER_errorf( const char *mask, ... );
int     CONTROLLER_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    CONTROLLER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    CONTROLLER_API_stop( void );
BOOL    CONTROLLER_API_check( void );

/******************************************************************************/

extern  struct  STR_TextItem  CONTROLLER_HostCardText[];
extern  struct  STR_TextItem  CONTROLLER_ControllerTypeText[];

/******************************************************************************/

class CONTROLLER
{

friend class ROBOT;

private:

    int    HostCard;
#define CONTROLLER_HOSTCARD_MAX         4
#define CONTROLLER_HOSTCARD_PHANTOMISA  0
#define CONTROLLER_HOSTCARD_SENSORAY    1
#define CONTROLLER_HOSTCARD_SIMULATE    2
#define CONTROLLER_HOSTCARD_MOUSE       3
#define CONTROLLER_HOSTCARD_OPTOTRAK    4

    DWORD  Address;
    int    SubAddress;

    int    ControllerType;
#define CONTROLLER_TYPE_NONE      0
#define CONTROLLER_TYPE_SMALL     1
#define CONTROLLER_TYPE_BIG       2
#define CONTROLLER_TYPE_GENERAL   3
#define CONTROLLER_TYPE_VBOT      4

    STRING ObjectName;

    BOOL   OpenFlag;
    BOOL   StartedFlag;
    BOOL   LoopTaskFlag;
    BOOL   ActivatedFlag;
    BOOL   SafeFlag;

#define CONTROLLER_ACTIVATED_LAST      2
#define CONTROLLER_ACTIVATED_NO2YES    0
#define CONTROLLER_ACTIVATED_YES2NO    1
    BOOL   ActivatedLast[CONTROLLER_ACTIVATED_LAST];

    PHANTOMISA *Phantom;
    SENSORAY   *Sensoray;

    TIMER Timer;

public:

    CONTROLLER( int card, DWORD address, int controller, char *name );
    CONTROLLER( int card, DWORD address, int subaddress, int controller, char *name );
   ~CONTROLLER( void );

    void Init( int card, DWORD address, int controller, char *name );
    void Init( int card, DWORD address, int subaddress, int controller, char *name );

    BOOL Open( void );
    void Close( void );

    char *Name( void );
    char *HostCardText( void );
    char *ControllerTypeText( void );
    int HostCardType( void );
    int ControllerTypeType( void );

    BOOL ControllerHardware( BOOL flag );
    BOOL Controller( BOOL flag );

    void LoopTask( void );

    BOOL Start( void );
    BOOL Stop( void );

    BOOL Started( void );

    BOOL Safe( void );
    BOOL SafeDirect( void );
    BOOL SafeIndirect( void );

    BOOL Activated( void );
    BOOL ActivatedDirect( void );
    BOOL ActivatedIndirect( void );

    BOOL ActivatedChange( int type );
    BOOL JustActivated( void );
    BOOL JustDeactivated( void );

    BOOL EncoderOpen( int channel, int multiplier );
    void EncoderReset( int channel );
    long EncoderGet( int channel );

    BOOL MotorOpen( int channel );
    void MotorReset( int channel );
    void MotorSet( int channel, long units );

    // Phantom-specific functions...
    BOOL PhantomController( BOOL enable );
    BOOL PhantomActivated( void );
    BOOL PhantomSafe( void );

    BOOL PhantomEncoderOpen( int channel );
    void PhantomEncoderReset( int channel );
    long PhantomEncoderGet( int channel );

    BOOL PhantomMotorOpen( int channel );
    void PhantomMotorReset( int channel );
    void PhantomMotorSet( int channel, long units );

    // Sensoray-specific functions...
    BOOL SensorayController( BOOL enable );
    BOOL SensorayActivated( void );
    BOOL SensoraySafe( void );

    BOOL SensorayEncoderOpen( int channel, int multiplier );
    void SensorayEncoderReset( int channel );
    long SensorayEncoderGet( int channel );

    BOOL SensorayMotorOpen( int channel );
    void SensorayMotorReset( int channel );
    void SensorayMotorSet( int channel, long units );

    // Latency timers...

    TIMER_Interval *LatencyEncoder;
    TIMER_Interval *LatencyMotor;
    TIMER_Interval *LatencyActivated;
    TIMER_Interval *LatencySafe;

    BOOL LatencyFlag;

    BOOL LatencyStarted( void );
    void LatencyOpen( void );
    void LatencyClose( void );
    void LatencyStart( void );
    void LatencyStop( void );
    void LatencyReset( void );
    void LatencyBefore( TIMER_Interval *t );
    void LatencyAfter( TIMER_Interval *t );
    void LatencyResults( void );

};

/******************************************************************************/

