/******************************************************************************/
/*                                                                            */
/* MODULE  : PHANToM.h                                                        */
/*                                                                            */
/* PURPOSE : PHANToM haptic interface functions.                              */
/*                                                                            */
/* DATE    : 19/Sep/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V2.0  JNI 19/Sep/2000 - Development taken over and module re-worked.       */
/*                                                                            */
/* V4.0  JNI 11/Oct/2002 - Replaced direct I/O interface with ROBOT object to */
/*                         allow PHANTOMs to run on various I/O cards.        */
/*                                                                            */
/* V4.1  JNI 19/Sep/2000 - Minor change to support multiple Sensoray cards.   */
/*                                                                            */
/******************************************************************************/
#include <window.h>
#include <daqft.h>

#ifndef PHANTOM
#define PHANTOM

/******************************************************************************/

#define PHANTOM_DEBUG() if( !PHANTOM_API_start(printf,printf,printf) ) { printf("Cannot start PHANTOM API.\n"); exit(0); }

/******************************************************************************/

#define SPECIFIC_HEAT_Cu     385.0     // Used model of motor temperature...
#define SPECIFIC_HEAT_Fe     434.0

/******************************************************************************/

#define PHANTOM_LINK         2         // Number of links in the arm.
#define PHANTOM_AXIS         3         // Number of axis.
#define PHANTOM_MARKER       2         // Number of OptoTrak markers.

#define PHANTOM_LINK_1       0
#define PHANTOM_LINK_2       1

#define PHANTOM_MARKER_1     0
#define PHANTOM_MARKER_2     1

#define PHANTOM_AXIS_1       0
#define PHANTOM_AXIS_2       1
#define PHANTOM_AXIS_3       2

#define PHANTOM_TEMP_MAX   100.0       // Absolute limit on maximum temperature for motors.
#define PHANTOM_FORCE_MAX  100.0       // Absolute limit for maximum force output (100 N = 10 kg).

#define PHANTOM_MTX_AXIS   3,1         // Matrix dimensions for angles...
#define PHANTOM_MTX_A1     1,1         // Axis 1 element.
#define PHANTOM_MTX_A2     2,1         // Axis 2 element.
#define PHANTOM_MTX_A3     3,1         // Axis 3 element.

#define PHANTOM_EU_INIT    0x8FFFFFF   // Initialize value for Encoder Units.

/******************************************************************************/
/* Configuration file information...                                          */
/******************************************************************************/

struct  PHANTOM_CnfgFile
{
    char  *EXT;
    BOOL   HDR;

    void (*load)( int ID );
    BOOL (*test)( int ID );
};

#define PHANTOM_CNFG         2
#define PHANTOM_CNFG_CAL     0
#define PHANTOM_CNFG_CFG     1

/******************************************************************************/
/* Configuration file information...                                          */
/******************************************************************************/

struct  PHANTOM_ItemHandle                       // Handle information structure.
{
    ROBOT *Robot;                                // Robot controller (V3.0).
    ROBOT_Hardware Hardware;                     // Details of controller hardware...

    BOOL started;                                // Currently running (amplifier on)?

    BOOL HWpanic;                                // Hardware panic switch activated?
    BOOL SWpanic;                                // Software panic activated (V2.9)?

    STRING name;                                 // PHANTOM name.
    STRING cnfg[PHANTOM_CNFG];                   // Configuration files.

    int  ControlType;                            // Control process function type. (V2.6)
#define PHANTOM_CONTROL_NONE         0           // No control function installed.
#define PHANTOM_CONTROL_RAW          1           // Raw encoder and motor units.
#define PHANTOM_CONTROL_MP           2           // Posiiton (matrix).
#define PHANTOM_CONTROL_MPF          3           // Position, force (matrix).
#define PHANTOM_CONTROL_MPVF         4           // Position, velocity, force (matrix).
#define PHANTOM_CONTROL_MPVAF        5           // Position, velocity, accelaration, force (matrix).
#define PHANTOM_CONTROL_DP           6           // Posiiton (double[]).
#define PHANTOM_CONTROL_DPF          7           // Position, force (double[]).
#define PHANTOM_CONTROL_DPVF         8           // Position, velocity, force (double[]).
#define PHANTOM_CONTROL_DPVAF        9           // Position, velocity, accelaration, force (double[]).

    // Application-defined force calculation functions. (V2.6)
    void (*ControlRaw )( long EU[], long MU[] );
    void (*ControlMP   )( matrix &p );
    void (*ControlMPF  )( matrix &p, matrix &f );
    void (*ControlMPVF )( matrix &p, matrix &v, matrix &f );
    void (*ControlMPVAF)( matrix &p, matrix &v, matrix &a, matrix &f );
    void (*ControlDP   )( double p[] );
    void (*ControlDPF  )( double p[], double f[] );
    void (*ControlDPVF )( double p[], double v[], double f[] );
    void (*ControlDPVAF)( double p[], double v[], double a[], double f[] );

    WINDOW *MotionWindow;

    DAQFT *FT;
};

/******************************************************************************/

#define PHANTOM_MAX          3                   // Maximum number of handles.
#define PHANTOM_INVALID     -1                   // Invalid handle value.

/******************************************************************************/

extern  struct  PHANTOM_ItemHandle  PHANTOM_Item[PHANTOM_MAX];
extern  int     PHANTOM_ID;

extern  float   PHANTOM_link[PHANTOM_MAX][PHANTOM_LINK];
extern  float   PHANTOM_marker[PHANTOM_MAX][PHANTOM_MARKER];

extern  float   PHANTOM_base[PHANTOM_MAX][AXIS_XYZ];
extern  float   PHANTOM_angle[PHANTOM_MAX][PHANTOM_AXIS];

extern  matrix  PHANTOM_JT[PHANTOM_MAX];

// DAQ F/T variables...
extern  STRING  PHANTOM_FT_Name[PHANTOM_MAX];
extern  int     PHANTOM_FT_ADC[PHANTOM_MAX][DAQFT_GAUGES];
extern  double  PHANTOM_FT_ZRotation[PHANTOM_MAX];

/******************************************************************************/
/* Function prototypes...                                                     */
/******************************************************************************/

int     PHANTOM_messgf( const char *mask, ... );
int     PHANTOM_errorf( const char *mask, ... );
int     PHANTOM_debugf( const char *mask, ... );

void    PHANTOM_Init( int ID );
int     PHANTOM_Free( void );
void    PHANTOM_Use( int ID );

int     PHANTOM_DOF( int ID );
BOOL    PHANTOM_3D( int ID );
BOOL    PHANTOM_2D( int ID );

BOOL    PHANTOM_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    PHANTOM_API_stop( void );

void    PHANTOM_CnfgHDR( int ID, int cnfg );
void    PHANTOM_CnfgCAL( int ID );
void    PHANTOM_CnfgCFG( int ID );
void    PHANTOM_CnfgVAR( int ID, int cnfg );

BOOL    PHANTOM_TestCAL( int ID );
BOOL    PHANTOM_TestCFG( int ID );
BOOL    PHANTOM_TestVAR( int ID, int cnfg );

BOOL    PHANTOM_CnfgSave( int ID, int cnfg );
BOOL    PHANTOM_CnfgLoad( int ID, int cnfg );
BOOL    PHANTOM_CnfgLoad( int ID, char *name );
void    PHANTOM_CnfgPrnt( int ID, PRINTF prnf, int cnfg );
void    PHANTOM_CnfgPrnt( int ID, PRINTF prnf );

int     PHANTOM_Open( char *name );
void    PHANTOM_Close( int ID );
void    PHANTOM_CloseAll( void );

BOOL    PHANTOM_Start( int ID, void (*func)( long EU[], long MU[] ) );
BOOL    PHANTOM_Start( int ID, void (*func)( matrix &p ) );
BOOL    PHANTOM_Start( int ID, void (*func)( matrix &p, matrix &f ) );
BOOL    PHANTOM_Start( int ID, void (*func)( matrix &p, matrix &v, matrix &f ) );
BOOL    PHANTOM_Start( int ID, void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) );
BOOL    PHANTOM_Start( int ID );
BOOL    PHANTOM_Start( int ID, int type, void *force_calc );
BOOL    PHANTOM_Start( int ID, void (*func)( double p[] ) );
BOOL    PHANTOM_Start( int ID, void (*func)( double p[], double f[] ) );
BOOL    PHANTOM_Start( int ID, void (*func)( double p[], double v[], double f[] ) );
BOOL    PHANTOM_Start( int ID, void (*func)( double p[], double v[], double a[], double f[] ) );
int     PHANTOM_Start( char *name, void (*func)( long EU[], long MU[] ) );
int     PHANTOM_Start( char *name, void (*func)( matrix &p ) );
int     PHANTOM_Start( char *name, void (*func)( matrix &p, matrix &f ) );
int     PHANTOM_Start( char *name, void (*func)( matrix &p, matrix &v, matrix &f ) );
int     PHANTOM_Start( char *name, void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) );
int     PHANTOM_Start( char *name, int type, void *func );
int     PHANTOM_Start( char *name );
void    PHANTOM_Stop( int ID );

void    PHANTOM_Exit( void );

BOOL    PHANTOM_Check( void );
BOOL    PHANTOM_Check( int ID );

void    PHANTOM_Info( int ID, PRINTF prnf );

BOOL    PHANTOM_PanicOn2Off( int ID );
BOOL    PHANTOM_PanicOff2On( int ID );
void    PHANTOM_PanicNow( int ID );

BOOL    PHANTOM_PanicStart( int ID );
void    PHANTOM_PanicStop( int ID );
void    PHANTOM_PanicLoop( int ID );

BOOL    PHANTOM_Cooling( int ID );
BOOL    PHANTOM_Started( int ID );
BOOL    PHANTOM_Safe( int ID );
BOOL    PHANTOM_Enabled( int ID );
BOOL    PHANTOM_Panic( int ID );
BOOL    PHANTOM_Ramped( int ID );
ROBOT  *PHANTOM_Robot( int ID );
BOOL    PHANTOM_Ping( int ID );

BOOL    PHANTOM_EnabledYes2No( int ID );
BOOL    PHANTOM_EnabledNo2Yes( int ID );

BOOL    PHANTOM_Disable( int ID );

void    PHANTOM_RobotLoop( int ID );

void    PHANTOM_EncoderLoop( int ID );

void    PHANTOM_EncoderReset( int ID, int axis );
void    PHANTOM_EncoderReset( int ID );
void    PHANTOM_EncoderGet( int ID, int axis );
long    PHANTOM_EncoderRead( int ID, int axis );
long    PHANTOM_Encoder( int ID, int axis );
void    PHANTOM_Encoder( int ID, long EU[] );

float   PHANTOM_ConvEU2Rev( int ID, int encoder, long EU );
float   PHANTOM_ConvEU2Rad( int ID, int encoder, float EU );
float   PHANTOM_ConvEU2Rad( int ID, int encoder, long  EU );
long    PHANTOM_ConvNm2MU( int ID, int motor, float Nm );
float   PHANTOM_ConvMU2Nm( int ID, int motor, long MU );

BOOL    PHANTOM_ControlStart( int ID );
void    PHANTOM_ControlStop( int ID );
void    PHANTOM_ControlLoop( int ID );

void    PHANTOM_ControlSet( int ID, int type, void *func );

void    PHANTOM_Control( int ID, matrix &p, matrix &v, matrix &a, matrix &f );
void    PHANTOM_Control( int ID, long EU[], long MU[] );

void    PHANTOM_TorqueNm( int ID, float Nm1, float Nm2, float Nm3 );
void    PHANTOM_TorqueNm( int ID, float Nm[] );
void    PHANTOM_TorqueMU( int ID, long MU[] );
void    PHANTOM_TorqueMU( int ID, int axis, long MU );

float   PHANTOM_MotorTorqueNm( int ID, int axis );
long    PHANTOM_MotorTorqueMU( int ID, int axis );
void    PHANTOM_MotorTorqueNm( int ID, float Nm[] );
void    PHANTOM_MotorTorqueMU( int ID, long MU[] );

void    PHANTOM_MotorReset( int ID, int axis );
void    PHANTOM_MotorReset( int ID );

void    PHANTOM_MotorPut( int ID, int axis, long MU );

float   PHANTOM_XYZ( int ID, int axis, float xyz[] );
void    PHANTOM_XYZ( int ID, int axis, float value, float xyz[] );
void    PHANTOM_XYZ( int ID, float xyz[], float &x, float &y, float &z );
void    PHANTOM_XYZ( int ID, float x, float y, float z, float xyz[] );

void    PHANTOM_Force( int ID, float fx, float fy, float fz );
void    PHANTOM_Force( int ID, matrix &f );

double  PHANTOM_RampValue( int ID );
void    PHANTOM_RampZero( int ID );
long    PHANTOM_RampMU( int ID, long MU );
BOOL    PHANTOM_RampFlag( int ID );

double  PHANTOM_TempMotor( int ID, int motor );
void    PHANTOM_TempMotor( int ID, double T[] );

BOOL    PHANTOM_TempStart( int ID );
void    PHANTOM_TempStop( int ID );
BOOL    PHANTOM_TempWait( int ID );

BOOL    PHANTOM_LoopTaskStart( int ID );
void    PHANTOM_LoopTaskStop( int ID );

void    PHANTOM_AngleOffset( int ID, float a1, float a2, float a3 );

void    PHANTOM_AnglesRaw( int ID, float &a1, float &a2, float &a3 );
void    PHANTOM_AnglesRaw( int ID, double &a1, double &a2, double &a3 );
void    PHANTOM_AnglesRaw( int ID, matrix &angles );
void    PHANTOM_Angles( int ID, float &a1, float &a2, float &a3 );
void    PHANTOM_Angles( int ID, double &a1, double &a2, double &a3 );
void    PHANTOM_Angles( int ID, matrix &angles );
void    PHANTOM_Angles( int ID, float &a1, float &a2, float &a3, BOOL offset );
void    PHANTOM_Angles( int ID, double &a1, double &a2, double &a3, BOOL offset );
void    PHANTOM_Angles( int ID, matrix &angles, BOOL offset );
void    PHANTOM_Angles( int ID, float angle[], BOOL offset );

void    PHANTOM_PosnCalc( int ID, float angle[], float link[], float &x, float &y, float &z, BOOL adjust );

void    PHANTOM_PosnRaw( int ID, float &x, float &y, float &z );
void    PHANTOM_PosnRaw( int ID, double &x, double &y, double &z );
void    PHANTOM_PosnRaw( int ID, matrix &posn );
void    PHANTOM_Posn( int ID, float &x, float &y, float &z );
void    PHANTOM_Posn( int ID, double &x, double &y, double &z );
void    PHANTOM_Posn( int ID, matrix &posn );
void    PHANTOM_Posn( int ID, float &x, float &y, float &z, BOOL adjust );
void    PHANTOM_Posn( int ID, double &x, double &y, double &z, BOOL adjust );
void    PHANTOM_Posn( int ID, matrix &posn, BOOL adjust );

void    PHANTOM_Marker( int ID, float angle[], float &x, float &y, float &z, int marker );
void    PHANTOM_Marker( int ID, float a1, float a2, float a3, float &x, float &y, float &z, int marker );
void    PHANTOM_Marker( int ID, float &x, float &y, float &z, int marker );
void    PHANTOM_Marker( int ID, double &x, double &y, double &z, int marker );
void    PHANTOM_Marker( int ID, matrix &posn, int marker );

void    PHANTOM_QuadraticMotion( int ID, int dof, matrix &v, matrix &a );
void    PHANTOM_QuadraticMotion( int ID, matrix &p, matrix &v, matrix &a );

BOOL    PHANTOM_Start( void (*func)( long EU[], long MU[] ) );
BOOL    PHANTOM_Start( void (*func)( matrix &p ) );
BOOL    PHANTOM_Start( void (*func)( matrix &p, matrix &f ) );
BOOL    PHANTOM_Start( void (*func)( matrix &p, matrix &v, matrix &f ) );
BOOL    PHANTOM_Start( void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) );
BOOL    PHANTOM_Start( void );
void    PHANTOM_Stop( void );
void    PHANTOM_Close( void );
BOOL    PHANTOM_PanicOn2Off( void );
BOOL    PHANTOM_PanicOff2On( void );
void    PHANTOM_PanicNow( void );
BOOL    PHANTOM_Cooling( void );
BOOL    PHANTOM_Safe( void );
BOOL    PHANTOM_Panic( void );
BOOL    PHANTOM_Started( void );
ROBOT  *PHANTOM_Robot( void );
void    PHANTOM_EncoderReset( void );
long    PHANTOM_Encoder( int axis );
void    PHANTOM_Encoder( long EU[] );
void    PHANTOM_MotorReset( void );
float   PHANTOM_MotorTorqueNm( int axis );
long    PHANTOM_MotorTorqueMU( int axis );
void    PHANTOM_MotorTorqueNm( float Nm[] );
void    PHANTOM_MotorTorqueMU( long MU[] );
double  PHANTOM_TempMotor( int motor );
void    PHANTOM_TempMotor( double T[] );
void    PHANTOM_AngleOffset( float a1, float a2, float a3 );
void    PHANTOM_AnglesRaw( float &a1, float &a2, float &a3 );
void    PHANTOM_AnglesRaw( double &a1, double &a2, double &a3 );
void    PHANTOM_AnglesRaw( matrix &posn );
void    PHANTOM_Angles( float &a1, float &a2, float &a3 );
void    PHANTOM_Angles( double &a1, double &a2, double &a3 );
void    PHANTOM_Angles( matrix &posn );
void    PHANTOM_Angles( float &a1, float &a2, float &a3, BOOL offset );
void    PHANTOM_Angles( double &a1, double &a2, double &a3, BOOL offset );
void    PHANTOM_Angles( matrix &posn, BOOL offset );
void    PHANTOM_Angles( float angle[], BOOL offset );
void    PHANTOM_PosnRaw( float &x, float &y, float &z );
void    PHANTOM_PosnRaw( double &x, double &y, double &z );
void    PHANTOM_PosnRaw( matrix &posn );
void    PHANTOM_Posn( float &x, float &y, float &z );
void    PHANTOM_Posn( double &x, double &y, double &z );
void    PHANTOM_Posn( matrix &posn );
void    PHANTOM_Posn( float &x, float &y, float &z, BOOL adjust );
void    PHANTOM_Posn( double &x, double &y, double &z, BOOL adjust );
void    PHANTOM_Posn( matrix &posn, BOOL adjust );
void    PHANTOM_Marker( float angle[], float &x, float &y, float &z, int marker );
void    PHANTOM_Marker( float a1, float a2, float a3, float &x, float &y, float &z, int marker );
void    PHANTOM_Marker( float &x, float &y, float &z, int marker );
void    PHANTOM_Marker( double &x, double &y, double &z, int marker );
void    PHANTOM_Marker( matrix &posn, int marker );
double  PHANTOM_RampValue( void );
void    PHANTOM_RampZero( void );
long    PHANTOM_RampMU( long MU );
int     PHANTOM_DOF( void );
BOOL    PHANTOM_3D( void );
BOOL    PHANTOM_2D( void );

/*****************************************************************************/

long    PHANTOM_Force2TorqueMU( int axis, float length, float force );
float   PHANTOM_EU2Angle( int axis, long EU );

void    PHANTOM_RodForce( int axis, float force );
float   PHANTOM_RodAngle( int axis );

/******************************************************************************/

BOOL    PHANTOM_FT_Installed( int ID );
BOOL    PHANTOM_FT_Opened( int ID );
BOOL    PHANTOM_FT_Open( int ID );
void    PHANTOM_FT_Close( int ID );
void    PHANTOM_FT_Read( int ID, matrix &f, matrix &t );
void    PHANTOM_FT_BiasReset( int ID );
BOOL    PHANTOM_FT_BiasWait( int ID );

BOOL    PHANTOM_FT_Installed( void );
BOOL    PHANTOM_FT_Opened( void );
BOOL    PHANTOM_FT_Open( void );
void    PHANTOM_FT_Close( void );
void    PHANTOM_FT_Read( matrix &f, matrix &t );
void    PHANTOM_FT_BiasReset( void );
BOOL    PHANTOM_FT_BiasWait( void );

/******************************************************************************/

#endif
