/******************************************************************************/
/*                                                                            */
/* MODULE  : ROBOT-Object.h                                                   */
/*                                                                            */
/* PURPOSE : Robot object - hardware interface.                               */
/*                                                                            */
/* DATE    : 01/Aug/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 01/Aug/2002 - Initial development.                               */
/*                                                                            */
/* V2.0  JNI 25/Apr/2007 - Changes for Kalman Filter on Encoders.             */
/*                                                                            */
/******************************************************************************/

#define ROBOT_DOFS 6

/******************************************************************************/

struct ROBOT_Hardware_Controller
{
    int CardType;
    DWORD Address;
    int SubAddress;
    int ControllerType;
};

struct ROBOT_Hardware_Encoder
{
    int Type;
    int Channel;
    int Sign;
    double KF_v;
    double KF_w;
    double KF_dt;
};

struct ROBOT_Hardware_Motor
{
    int Type;
    int Channel;
};

struct ROBOT_Hardware_Axis
{
    struct ROBOT_Hardware_Motor Motor;
    struct ROBOT_Hardware_Encoder Encoder;
    double Gearing;
};

struct ROBOT_Hardware
{
    struct ROBOT_Hardware_Controller Controller;
    int AxisCount;
    struct ROBOT_Hardware_Axis Axis[ROBOT_DOFS];
};

/******************************************************************************/

void ROBOT_HardwareDetails( struct ROBOT_Hardware *hardware, PRINTF prnf );

/******************************************************************************/

struct ENCODER
{
    // Internal flags...
    BOOL OpenFlag;
    BOOL StartedFlag;

    // Type of encoder...
    int Type;
#define ENCODER_TYPE_NONE              0
#define ENCODER_TYPE_DRC_T23B          1
#define ENCODER_TYPE_MAXON_HEDS5500    2
#define ENCODER_TYPE_IED_58SA          3
#define ENCODER_TYPE_IED_40SA          4
#define ENCODER_TYPE_IED_58SAx4        5
#define ENCODER_TYPE_IED_40SAx4        6
#define ENCODER_TYPE_IED_60HA          7
#define ENCODER_TYPE_IED_60HAx4        8

    // Controller channel for encoder.
    int Channel;

    int Sign;

    // Counter units counts per revolution.
    double UnitsPerRevolution;

    // Gearing ratio.
    double Gearing;

    // Last count and change in count.
    long Count;
    long dCount;

    // Kalman Filter parameters.
    KALMANFILTER *KF;
    double KF_v;
    double KF_w;
    double KF_dt;

    // Kalman Filter current values.
    double KF_xraw;
    double KF_x;
    double KF_dx;
};

/******************************************************************************/

extern struct STR_TextItem ENCODER_TypeText[];

/******************************************************************************/

struct MOTOR
{
    // Internal flags...
    BOOL OpenFlag;
    BOOL StartedFlag;

    // Type of motor...
    int Type;
#define MOTOR_TYPE_NONE         0
#define MOTOR_TYPE_MAXON_RE35   1
#define MOTOR_TYPE_MAXON_RE25   2
#define MOTOR_TYPE_MAXON_RE75   3
#define MOTOR_TYPE_MAXON_RE40   4

    // Controller channel for motor.
    int Channel;

    // DAC units per motor torque (Nm).
    double UnitsPerNm;

    // Gearing ratio.
    double Gearing;

    // Current DAC units.
    long Units;
};

/******************************************************************************/

extern struct STR_TextItem MOTOR_TypeText[];

/******************************************************************************/

class ROBOT
{
public:
    BOOL OpenFlag;
    BOOL StartedFlag;

    STRING ObjectName;

    ROBOT_Hardware Hardware;

    CONTROLLER *Controller;

    // Frequency of LoopTask functions.
    double Frequency;

    // Object print functions...
    int Errorf( const char *mask, ... );
    int Messgf( const char *mask, ... );
    int Debugf( const char *mask, ... );
    int Printf( BOOL ok, const char *mask, ... );

    ROBOT( struct ROBOT_Hardware *hardware, char *name );
    ROBOT( void );

   ~ROBOT( void );

    void Init( struct ROBOT_Hardware *hardware, char *name );

    BOOL Open( void );
    void Close( void );

    BOOL Start( void );
    void Stop( void );

    BOOL Started( void );

    BOOL ControllerOpen( void );
    void ControllerClose( void );

    SENSORAY *Sensoray();

    BOOL Activated( void );
    BOOL JustActivated( void );
    BOOL JustDeactivated( void );
    BOOL Safe( void );

    void LoopTask( void );
    void LoopTaskFrequency( double frequency );

    // Frequency timer.
    TIMER_Frequency *LoopTaskTimer;

    // Encoder functions...
    ENCODER Encoder[ROBOT_DOFS];
    void EncoderInit( int axis );
    void EncoderInit( void );
    void EncoderInit( int axis, int type, int channel, int sign, double gearing, double kf_w, double kf_v, double kf_dt );
    BOOL EncoderOpen( int axis );
    BOOL EncoderOpen( void );
    void EncoderClose( int axis );
    void EncoderClose( void );
    BOOL EncoderStarted( int axis );
    void EncoderReset( int axis );
    void EncoderReset( void );
    void EncoderRead( int axis, double &xraw, double &x, double &dx );
    void EncoderRead( double xraw[], double x[], double dx[] );
    void EncoderRevolutions( int axis, double &rev_xraw, double &rev_x, double &rev_dx );
    void EncoderRevolutions( double rev_xraw[], double rev_x[], double rev_dx[] );
    void EncoderRadians( int axis, double &rad_xraw, double &rad_x, double &rad_dx );
    void EncoderRadians( double rad_xraw[], double rad_x[], double rad_dx[] );
    void EncoderDegrees( int axis, double &deg_xraw, double &deg_x, double &deg_dx );
    void EncoderDegrees( double deg_xraw[], double deg_x[], double deg_dx[] );
    char *EncoderText( int axis );

    // Motor functions...
    MOTOR Motor[ROBOT_DOFS];
    void MotorInit( int axis );
    void MotorInit( void );
    void MotorInit( int axis, int type, int channel, double gearing );
    BOOL MotorOpen( int axis );
    BOOL MotorOpen( void );
    void MotorClose( int axis );
    void MotorClose( void );
    BOOL MotorStarted( int axis );
    void MotorReset( int axis );
    void MotorReset( void );
    void MotorSet( int axis, long units );
    void MotorSet( long units[] );
    char *MotorText( int axis );

    // Ramping functions...
    RAMPER *Ramp;

    // Temperature tracking information and functions...
    TEMPTRAK *TempTrak;

    int TempTrakState;
#define ROBOT_TEMPTRAK_INIT       0
#define ROBOT_TEMPTRAK_RUNNING    1
#define ROBOT_TEMPTRAK_COOLING    2
#define ROBOT_TEMPTRAK_RESUME     3
#define ROBOT_TEMPTRAK_DISABLED   4

    BOOL TempTrakOpen( void );
    void TempTrakClose();
    BOOL TempTrakStarted();
    BOOL TempTrakStart();
    BOOL TempTrakStop();
    BOOL TempTrakCooling( void );
    void TempTrakLoopTask( void );
};

/******************************************************************************/

