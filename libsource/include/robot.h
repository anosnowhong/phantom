/******************************************************************************/
/*                                                                            */
/* MODULE  : ROBOT.h                                                          */
/*                                                                            */
/* PURPOSE : Robotic manipulandum / haptic interface functions.               */
/*                                                                            */
/* DATE    : 19/Sep/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V2.0  JNI 19/Sep/2000 - Development taken over and module re-worked.       */
/*                                                                            */
/* V4.0  JNI 11/Oct/2002 - Replaced direct I/O interface with ROBOT object to */
/*                         support different I/O types.                       */
/*                                                                            */
/* V4.1  JNI 11/Mar/2004 - Minor change to support multiple Sensoray cards.   */
/*                                                                            */
/* V4.3  JNI 07/Oct/2004 - Changed module name from PHANTOM to ROBOT.         */
/*                                                                            */
/* V5.4  JNI 20/Apr/2010 - Added Robot type variable to configuration file to */
/*                         allow single-axis lever actuators of the type used */
/*                         in force matching / sensory cancellation paradimgs.*/
/*                                                                            */
/******************************************************************************/

#ifndef ROBOT_H
#define ROBOT_H

/******************************************************************************/

#include "robot-phantom.h"

/******************************************************************************/

#define ROBOT_DEBUG() if( !ROBOT_API_start(printf,printf,printf) ) { printf("Cannot start ROBOT API.\n"); exit(0); }

/******************************************************************************/

extern  PRINTF  ROBOT_PRN_messgf;      // General messages printf function.
extern  PRINTF  ROBOT_PRN_errorf;      // Error messages printf function.
extern  PRINTF  ROBOT_PRN_debugf;      // Debug information printf function.

/******************************************************************************/

#define SPECIFIC_HEAT_Cu     385.0     // Used model of motor temperature...
#define SPECIFIC_HEAT_Fe     434.0

/******************************************************************************/

#define ROBOT_LINK         3         // Number of links in the arm.
#undef  ROBOT_DOFS
#define ROBOT_DOFS         3         // Total number of degrees of freedeom.
#define ROBOT_AXIS         3         // Number of axis.
#define ROBOT_MARKER       2         // Number of OptoTrak markers.

#define ROBOT_LINK_1       0
#define ROBOT_LINK_2       1

#define ROBOT_MARKER_1     0
#define ROBOT_MARKER_2     1

#define ROBOT_AXIS_1       0
#define ROBOT_AXIS_2       1
#define ROBOT_AXIS_3       2

#define ROBOT_TEMP_MAX   100.0       // Absolute limit on maximum temperature for motors.
#define ROBOT_FORCE_MAX  100.0       // Absolute limit for maximum force output (100 N = 10 kg).

#define ROBOT_MTX_AXIS   3,1         // Matrix dimensions for angles...
#define ROBOT_MTX_A1     1,1         // Axis 1 element.
#define ROBOT_MTX_A2     2,1         // Axis 2 element.
#define ROBOT_MTX_A3     3,1         // Axis 3 element.

#define ROBOT_EU_INIT    0x8FFFFFF   // Initialize value for Encoder Units.

/******************************************************************************/
/* Configuration file information...                                          */
/******************************************************************************/

struct  ROBOT_CnfgFile
{
    char  *EXT;
    BOOL   HDR;

    void (*load)( int ID );
    BOOL (*test)( int ID );
};

#define ROBOT_CNFG         2
#define ROBOT_CNFG_CFG     0
#define ROBOT_CNFG_CAL     1

/******************************************************************************/

struct ROBOT_Sensor
{
    ASENSOR *asensor;

    int DeviceType;
    int ChannelCount;
    int ChannelList[ASENSOR_CHANNELS];
    int ChannelIndex[ASENSOR_CHANNELS];
    
    double ZRotation;
    matrix ROMX;
};

/******************************************************************************/
/* Structures for handling robot information.                                 */
/******************************************************************************/

struct  ROBOT_EncoderWorking
{
    double count_xraw;
    double count_x;
    double count_dx;
    double count_ddx;
};

struct  ROBOT_AxisWorking
{
    struct ROBOT_EncoderWorking Encoder;
};

struct  ROBOT_ItemHandle                         // Handle information structure.
{
    ROBOT *Robot;                                // Robot controller (V3.0).
    ROBOT_Hardware Hardware;                     // Details of controller hardware.

    ROBOT_AxisWorking Axis[ROBOT_DOFS];          // Working variables for each axis.

    BOOL started;                                // Currently running (amplifier on)?

    BOOL HWpanic;                                // Hardware panic switch activated?
    BOOL SWpanic;                                // Software panic activated (V2.9)?

    BOOL safe;                                   // Software safe flag (FALSE when max force exceeded).

    STRING name;                                 // ROBOT name.
    STRING cnfg[ROBOT_CNFG];                     // Configuration files.

    int  ManipulandumType;                       // Robot manipulandum type (V5.4)
#define ROBOT_MANIPULANDUM_MULTIAXIS   0         // Multiple-axis (2D or 3D) manipulandum (vBOT, Phantom).
#define ROBOT_MANIPULANDUM_SINGLEAXIS  1         // One or more single-axis manipulanda (force levers).

    int  ControlType;                            // Control process function type. (V2.6)
#define ROBOT_CONTROL_NONE         0             // No control function installed.
#define ROBOT_CONTROL_RAW          1             // Raw encoder and motor units.
#define ROBOT_CONTROL_MP           2             // Posiiton (matrix).
#define ROBOT_CONTROL_MPF          3             // Position, force (matrix).
#define ROBOT_CONTROL_MPVF         4             // Position, velocity, force (matrix).
#define ROBOT_CONTROL_MPVAF        5             // Position, velocity, accelaration, force (matrix).
#define ROBOT_CONTROL_DP           6             // Posiiton (double[]).
#define ROBOT_CONTROL_DPF          7             // Position, force (double[]).
#define ROBOT_CONTROL_DPVF         8             // Position, velocity, force (double[]).
#define ROBOT_CONTROL_DPVAF        9             // Position, velocity, accelaration, force (double[]).
#define ROBOT_CONTROL_VOID        10             // Void.

    int FilterType;
#define ROBOT_FILTER_NONE          0
#define ROBOT_FILTER_QUADRATICFIT  1
#define ROBOT_FILTER_KALMANFILTER  2
#define ROBOT_FILTER_EKF           3

    // Application-defined force calculation functions. (V2.6)
    void (*ControlRaw  )( long EU[], long MU[] );
    void (*ControlMP   )( matrix &p );
    void (*ControlMPF  )( matrix &p, matrix &f );
    void (*ControlMPVF )( matrix &p, matrix &v, matrix &f );
    void (*ControlMPVAF)( matrix &p, matrix &v, matrix &a, matrix &f );
    void (*ControlDP   )( double p[] );
    void (*ControlDPF  )( double p[], double f[] );
    void (*ControlDPVF )( double p[], double v[], double f[] );
    void (*ControlDPVAF)( double p[], double v[], double a[], double f[] );
    void (*ControlVoid )( void );

    // Quadratic fit window for calculating velocity and acceleration...
    WINFIT *QuadraticFitWindow;

    // DAQ sensors.
    class ASENSOR *DAQFT;
    int            DAQFT_Sensor;

    class ASENSOR *Accelerometer;
    int            Accelerometer_Sensor;

    class ASENSOR *PhotoTransistor;
    int            PhotoTransistor_Sensor;

#define ROBOT_SENSOR_MAX 3
    struct ROBOT_Sensor SensorList[ROBOT_SENSOR_MAX];
    int                 SensorChannelCount;
    int                 SensorChannelList[SENSORAY_ADC_CHANNELS];

    class EKF *EKF;
};

/******************************************************************************/

#define ROBOT_MAX          3                   // Maximum number of handles.
#define ROBOT_INVALID     -1                   // Invalid handle value.

/******************************************************************************/

extern  struct  ROBOT_ItemHandle  ROBOT_Item[ROBOT_MAX];
extern  int     ROBOT_ID;

extern  BOOL    ROBOT_reset[ROBOT_MAX];

extern  double  ROBOT_link[ROBOT_MAX][ROBOT_LINK];
extern  double  ROBOT_marker[ROBOT_MAX][ROBOT_MARKER];

extern  double  ROBOT_PositionOffset[ROBOT_MAX][AXIS_XYZ];
extern  double  ROBOT_AngleOffset[ROBOT_MAX][ROBOT_DOFS];

extern  matrix  ROBOT_JT[ROBOT_MAX];

// DAQ F/T variables...
extern  STRING  ROBOT_SensorName[ROBOT_MAX][ROBOT_SENSOR_MAX];
extern  double  ROBOT_SensorZRotation[ROBOT_MAX][ROBOT_SENSOR_MAX];

/******************************************************************************/
/* Function prototypes...                                                     */
/******************************************************************************/

int     ROBOT_messgf( const char *mask, ... );
int     ROBOT_errorf( const char *mask, ... );
int     ROBOT_debugf( const char *mask, ... );

void    ROBOT_Init( int ID );
int     ROBOT_Free( void );
void    ROBOT_Use( int ID );
void    ROBOT_Defaults( int ID );

int     ROBOT_TDOF( int ID );
int     ROBOT_RDOF( int ID );
int     ROBOT_DOF( int ID );
BOOL    ROBOT_3D( int ID );
BOOL    ROBOT_2D( int ID );

BOOL    ROBOT_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    ROBOT_API_stop( void );

void    ROBOT_CnfgHDR( int ID, int cnfg );
void    ROBOT_CnfgCAL( int ID );
void    ROBOT_CnfgCFG( int ID );
void    ROBOT_CnfgVAR( int ID, int cnfg );

BOOL    ROBOT_TestCAL( int ID );
BOOL    ROBOT_TestCFG( int ID );
BOOL    ROBOT_TestVAR( int ID, int cnfg );

BOOL    ROBOT_CnfgSave( int ID, int cnfg );
BOOL    ROBOT_CnfgLoadType( int ID, int cnfg );
BOOL    ROBOT_CnfgLoad( int ID, char *name );
void    ROBOT_CnfgPrnt( int ID, PRINTF prnf, int cnfg );
void    ROBOT_CnfgPrnt( int ID, PRINTF prnf );

void    ROBOT_ControllerClose( int ID );
BOOL    ROBOT_ControllerOpen( int ID );

int     ROBOT_Open( char *name );
void    ROBOT_Close( int ID );
void    ROBOT_CloseAll( void );

BOOL    ROBOT_Start( int ID, void (*func)( long EU[], long MU[] ) );
BOOL    ROBOT_Start( int ID, void (*func)( matrix &p ) );
BOOL    ROBOT_Start( int ID, void (*func)( matrix &p, matrix &f ) );
BOOL    ROBOT_Start( int ID, void (*func)( matrix &p, matrix &v, matrix &f ) );
BOOL    ROBOT_Start( int ID, void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) );
BOOL    ROBOT_Start( int ID, void (*func)( void ) );
BOOL    ROBOT_Start( int ID );
BOOL    ROBOT_Start( int ID, int type, void *force_calc );
BOOL    ROBOT_Start( int ID, void (*func)( double p[] ) );
BOOL    ROBOT_Start( int ID, void (*func)( double p[], double f[] ) );
BOOL    ROBOT_Start( int ID, void (*func)( double p[], double v[], double f[] ) );
BOOL    ROBOT_Start( int ID, void (*func)( double p[], double v[], double a[], double f[] ) );
int     ROBOT_Start( char *name, void (*func)( long EU[], long MU[] ) );
int     ROBOT_Start( char *name, void (*func)( matrix &p ) );
int     ROBOT_Start( char *name, void (*func)( matrix &p, matrix &f ) );
int     ROBOT_Start( char *name, void (*func)( matrix &p, matrix &v, matrix &f ) );
int     ROBOT_Start( char *name, void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) );
int     ROBOT_Start( char *name, void (*func)( void ) );
int     ROBOT_Start( char *name, int type, void *func );
int     ROBOT_Start( char *name );
void    ROBOT_Stop( int ID );

BOOL    ROBOT_Check( void );
BOOL    ROBOT_Check( int ID );

void    ROBOT_Info( int ID, PRINTF prnf );

BOOL    ROBOT_PanicOn2Off( int ID );
BOOL    ROBOT_PanicOff2On( int ID );
void    ROBOT_PanicNow( int ID );

BOOL    ROBOT_PanicStart( int ID );
void    ROBOT_PanicStop( int ID );
void    ROBOT_PanicLoop( int ID );

BOOL    ROBOT_Cooling( int ID );
BOOL    ROBOT_Started( int ID );
BOOL    ROBOT_Safe( int ID );
void    ROBOT_UnSafe( int ID );
BOOL    ROBOT_Panic( int ID );
BOOL    ROBOT_Ramped( int ID );
ROBOT  *ROBOT_Robot( int ID );
BOOL    ROBOT_Ping( int ID );
BOOL    ROBOT_Activated( int ID );
BOOL    ROBOT_JustActivated( int ID );
BOOL    ROBOT_JustDeactivated( int ID );

BOOL    ROBOT_Disable( int ID );

void    ROBOT_RobotLoop( int ID );

void    ROBOT_EncoderRead( int ID, int axis );
void    ROBOT_EncoderLoop( int ID );
BOOL    ROBOT_EncoderReset( int ID, int axis );
BOOL    ROBOT_EncoderReset( int ID );
void    ROBOT_Encoder( int ID, int axis, double &xraw, double &x, double &dx, double &ddx );
void    ROBOT_Encoder( int ID, double xraw[], double x[], double dx[], double ddx[] );
void    ROBOT_Encoder( int ID, double xraw[] );

double  ROBOT_ConvEU2Rad( int ID, int encoder, double EU );
double  ROBOT_ConvEU2Rad( int ID, int encoder, long  EU );
long    ROBOT_ConvNm2MU( int ID, int motor, double Nm );
double  ROBOT_ConvMU2Nm( int ID, int motor, long MU );

BOOL    ROBOT_ControlStart( int ID );
void    ROBOT_ControlStop( int ID );
void    ROBOT_ControlLoop( int ID );

void    ROBOT_ControlSet( int ID, int type, void *func );

void    ROBOT_Control( int ID, matrix &p, matrix &v, matrix &a, matrix &f );
void    ROBOT_Control( int ID, long EU[], long MU[] );

long    ROBOT_MotorGetMU( int ID, int axis );
void    ROBOT_MotorGetMU( int ID, long MU[] );
double  ROBOT_MotorGetNm( int ID, int axis );
void    ROBOT_MotorGetNm( int ID, double Nm[] );
void    ROBOT_MotorGetNm( int ID, matrix &Nm );

void    ROBOT_MotorReset( int ID, int axis );
void    ROBOT_MotorReset( int ID );

void    ROBOT_MotorSetMU( int ID, int axis, long MU );
void    ROBOT_MotorSetMU( int ID, long MU[] );
void    ROBOT_MotorSetNm( int ID, int axis, double Nm );
void    ROBOT_MotorSetNm( int ID, double Nm[] );

double  ROBOT_MapAPI2XYZ( int ID, int axis, double api[] );
void    ROBOT_MapXYZ2API( int ID, int axis, double xyz, double api[] );
void    ROBOT_MapAPI2XYZ( int ID, double api[], double xyz[] );
void    ROBOT_MapXYZ2API( int ID, double xyz[], double api[] );

void    ROBOT_API2XYZ( int ID, double api[], double xyz[], double offset[] );
void    ROBOT_XYZ2API( int ID, double xyz[], double api[], double offset[] );
void    ROBOT_API2XYZ( int ID, double api[], double xyz[] );
void    ROBOT_XYZ2API( int ID, double xyz[], double api[] );

void    ROBOT_Force( int ID, matrix &f );

double  ROBOT_RampValue( int ID );
void    ROBOT_RampZero( int ID );
long    ROBOT_RampMU( int ID, long MU );
BOOL    ROBOT_RampFlag( int ID );
BOOL    ROBOT_RampTimeSet( int ID, double ramptime );

double  ROBOT_TempMotor( int ID, int motor );
void    ROBOT_TempMotor( int ID, double T[] );

BOOL    ROBOT_TempStart( int ID );
void    ROBOT_TempStop( int ID );
BOOL    ROBOT_TempWait( int ID );

BOOL    ROBOT_LoopTaskStart( int ID );
void    ROBOT_LoopTaskStop( int ID );
double  ROBOT_LoopTaskGetFrequency( int ID );
double  ROBOT_LoopTaskGetPeriod( int ID );

void    ROBOT_AnglesRaw( int ID, matrix &AP, matrix &AV, matrix &AA );
void    ROBOT_AnglesRaw( int ID, double angpos[], double angvel[], double angacc[] );
void    ROBOT_AnglesRaw( int ID, matrix &AP );
void    ROBOT_AnglesRaw( int ID, double angpos[] );

void    ROBOT_Angles( int ID, matrix &AP, matrix &AV, matrix &AA );
void    ROBOT_Angles( int ID, matrix &AP );
void    ROBOT_Angles( int ID, double angpos[], double angvel[], double angacc[] );
void    ROBOT_Angles( int ID, double angpos[] );

void    ROBOT_Angles( int ID, matrix &AP, matrix &AV, matrix &AA, BOOL offset );
void    ROBOT_Angles( int ID, double angpos[], double angvel[], double angacc[], BOOL offset );

void    ROBOT_ManipulandumPosition( int ID, double angle[], double link[], double xyz[], double offset[] );

void    ROBOT_AngularToCartesianPosition( int ID, double angpos[], double link[], double xyz[], double offset[] );
void    ROBOT_AngularToCartesianVelocity( int ID, double angvel[], double xyz[] );
void    ROBOT_AngularToCartesianAcceleration( int ID, double angacc[], double xyz[] );

#define ROBOT_Posn ROBOT_Position
void    ROBOT_Position( int ID, double p_xyz[], double v_xyz[], double a_xyz[] );
void    ROBOT_Position( int ID, double p_xyz[] );
void    ROBOT_Position( int ID, matrix &P, matrix &V, matrix &A );
void    ROBOT_Position( int ID, matrix &P );

void    ROBOT_PositionMarker( int ID, int marker, double angle[], double xyz[] );
void    ROBOT_PositionMarker( int ID, int marker, double xyz[] );
void    ROBOT_PositionMarker( int ID, int marker, matrix &P );

void    ROBOT_QuadraticFit( int ID, matrix &P, matrix &p, matrix &v, matrix &a );
void    ROBOT_QuadraticFit( int ID, double praw_xyz[], double p_xyz[], double v_xyz[], double a_xyz[] );
void    ROBOT_dXdt( int ID, matrix &P, matrix &v );

UINT    ROBOT_OptoTrakFrameLast( int ID );

double ROBOT_LoopTaskLatencyGet( int ID );

/*****************************************************************************/

BOOL    ROBOT_Start( void (*func)( long EU[], long MU[] ) );
BOOL    ROBOT_Start( void (*func)( matrix &p ) );
BOOL    ROBOT_Start( void (*func)( matrix &p, matrix &f ) );
BOOL    ROBOT_Start( void (*func)( matrix &p, matrix &v, matrix &f ) );
BOOL    ROBOT_Start( void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) );
BOOL    ROBOT_Start( void (*func)( void ) );
BOOL    ROBOT_Start( void );
void    ROBOT_Stop( void );
void    ROBOT_Close( void );
BOOL    ROBOT_PanicOn2Off( void );
BOOL    ROBOT_PanicOff2On( void );
void    ROBOT_PanicNow( void );
BOOL    ROBOT_Cooling( void );
BOOL    ROBOT_Safe( void );
void    ROBOT_UnSafe( void );
BOOL    ROBOT_Panic( void );
BOOL    ROBOT_Started( void );
ROBOT  *ROBOT_Robot( void );
double  ROBOT_LoopTaskGetFrequency( void );
double  ROBOT_LoopTaskGetPeriod( void );

BOOL    ROBOT_EncoderReset( void );
void    ROBOT_Encoder( int axis, double &xraw, double &x, double &dx, double &ddx );
void    ROBOT_Encoder( double xraw[], double x[], double dx[], double ddx[] );
void    ROBOT_Encoder( double xraw[] );

void    ROBOT_MotorReset( void );

long    ROBOT_MotorGetMU( int axis );
void    ROBOT_MotorGetMU( long MU[] );
double  ROBOT_MotorGetNm( int axis );
void    ROBOT_MotorGetNm( double Nm[] );
void    ROBOT_MotorGetNm( matrix &Nm );

double  ROBOT_TempMotor( int motor );
void    ROBOT_TempMotor( double T[] );

void    ROBOT_AnglesRaw( matrix &AP, matrix &AV, matrix &AA );
void    ROBOT_AnglesRaw( matrix &AP );
void    ROBOT_AnglesRaw( double angpos[], double angvel[], double angacc[] );
void    ROBOT_AnglesRaw( double angpos[] );
void    ROBOT_Angles( matrix &AP, matrix &AV, matrix &AA );
void    ROBOT_Angles( matrix &AP );
void    ROBOT_Angles( double angpos[], double angvel[], double angacc[] );
void    ROBOT_Angles( double angpos[] );
void    ROBOT_Angles( matrix &AP, matrix &AV, matrix &AA, BOOL offset );
void    ROBOT_Angles( matrix &AP, BOOL offset );
void    ROBOT_Angles( double angpos[], BOOL offset );
void    ROBOT_Angles( double angpos[], double angvel[], double angacc[], BOOL offset );
void    ROBOT_Position( double p_xyz[], double v_xyz[], double a_xyz );
void    ROBOT_Position( double p_xyz[] );
void    ROBOT_Position( matrix &P, matrix &V, matrix &A );
void    ROBOT_Position( matrix &P );
void    ROBOT_PositionMarker( int marker, matrix &P );
void    ROBOT_PositionMarker( int marker, double xyz[] );
double  ROBOT_RampValue( void );
void    ROBOT_RampZero( void );
long    ROBOT_RampMU( long MU );
int     ROBOT_TDOF( void );
int     ROBOT_RDOF( void );
int     ROBOT_DOF( void );
BOOL    ROBOT_3D( void );
BOOL    ROBOT_2D( void );
BOOL    ROBOT_Activated( void );
BOOL    ROBOT_JustActivated( void );
BOOL    ROBOT_JustDeactivated( void );

UINT    ROBOT_OptoTrakFrameLast( void );

double ROBOT_LoopTaskLatencyGet( void );

/*****************************************************************************/

BOOL ROBOT_SensorConfigured( int ID );
BOOL ROBOT_SensorCheck( int ID );
BOOL ROBOT_SensorOpened( int ID, int item );
BOOL ROBOT_SensorOpened( int ID );
BOOL ROBOT_SensorOpened_DAQFT( int ID );
BOOL ROBOT_SensorOpened_Accelerometer( int ID );
BOOL ROBOT_SensorOpened_PhotoTransistor( int ID );
BOOL ROBOT_SensorOpen( int ID );
void ROBOT_SensorClose( int ID );
void ROBOT_SensorRead( int ID, double raw_volts[] );
void ROBOT_SensorRead( int ID );
BOOL ROBOT_Sensor_DAQFT( int ID, matrix &f, matrix &t );
BOOL ROBOT_Sensor_DAQFT( int ID, matrix &f );
BOOL ROBOT_Sensor_Accelerometer( int ID, matrix &a );
BOOL ROBOT_Sensor_PhotoTransistor( int ID, double &volts );
BOOL ROBOT_SensorBiasWait_DAQFT( int ID );
BOOL ROBOT_SensorBiasWait_Accelerometer( int ID );
BOOL ROBOT_SensorBiasReset( int ID, int item, int type, BOOL save );
BOOL ROBOT_SensorBiasReset_DAQFT( int ID, int type, BOOL save );
BOOL ROBOT_SensorBiasReset_DAQFT( int ID );
BOOL ROBOT_SensorBiasResetAnterograde_DAQFT( int ID );
BOOL ROBOT_SensorBiasResetRetrograde_DAQFT( int ID );
BOOL ROBOT_SensorBiasReset_Accelerometer( int ID, int type, BOOL save );
BOOL ROBOT_SensorBiasReset_Accelerometer( int ID );
BOOL ROBOT_SensorBiasResetAnterograde_Accelerometer( int ID );
BOOL ROBOT_SensorBiasResetRetrograde_Accelerometer( int ID );

BOOL ROBOT_SensorConfigured( void );
BOOL ROBOT_SensorOpened( void );
BOOL ROBOT_SensorOpened_DAQFT( void );
BOOL ROBOT_SensorOpened_Accelerometer( void );
BOOL ROBOT_SensorOpened_PhotoTransistor( void );
BOOL ROBOT_SensorOpen( void );
void ROBOT_SensorClose( void );
void ROBOT_SensorRead( double raw_volts[] );
void ROBOT_SensorRead( void );
BOOL ROBOT_Sensor_DAQFT( matrix &f, matrix &t );
BOOL ROBOT_Sensor_DAQFT( matrix &f );
BOOL ROBOT_Sensor_Accelerometer( matrix &a );
BOOL ROBOT_Sensor_PhotoTransistor( double &volts );
BOOL ROBOT_SensorBiasWait_DAQFT( void );
BOOL ROBOT_SensorBiasWait_Accelerometer( void );
BOOL ROBOT_SensorBiasResetAnterograde_DAQFT( void );
BOOL ROBOT_SensorBiasResetRetrograde_DAQFT( void );
BOOL ROBOT_SensorBiasReset_DAQFT( void );
BOOL ROBOT_SensorBiasResetAnterograde_Accelerometer( void );
BOOL ROBOT_SensorBiasResetRetrograde_Accelerometer( void );
BOOL ROBOT_SensorBiasReset_Accelerometer( void );

void ROBOT_Forces2MotorTorques( int ID, matrix &f, matrix &t );
void ROBOT_Forces2MotorTorques( matrix &f, matrix &t );

/******************************************************************************/

#endif
