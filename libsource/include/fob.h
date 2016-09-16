/******************************************************************************/
/*                                                                            */
/* MODULE  : FOB.h                                                            */
/*                                                                            */
/* PURPOSE : Flock of Birds API functions.                                    */
/*                                                                            */
/* DATE    : 04/Nov/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 04/Nov/2000 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#ifndef FOB_H
#define FOB_H

/******************************************************************************/

#pragma pack(1)    // Pack the following structures on one-byte boundaries.

/******************************************************************************/

#define FOB_DEBUG() if( !FOB_API_start(printf,printf,printf) ) { printf("Cannot start FOB API.\n"); exit(0); }

/******************************************************************************/

#define FOB_PORT_MAX        16         // Maximum number of ports.
#define FOB_PORT_INVALID    -1         // Invalid handle value.
#define FOB_PORT_ALL        -2         // Value for "All current ports".

#define FOB_BIRD_MAX        32         // Maximum number of birds.
#define FOB_BIRD_NULL       -1         // Invalid bird value.
#define FOB_BIRD_ALL        -2         // Value for "All current birds".

#define FOB_ADDR_NULL        0         // NULL address.

#define FOB_FBB_NORMAL      14         // Number of birds on FBB in normal address mode.
#define FOB_FBB_EXPANDED    30         // Number of birds on FBB in expanded address mode.
#define FOB_FBB_SUPER      126         // Number of birds on FBB in super-expanded address mode.

/******************************************************************************/
/* FOB raw data structures (inspired by initial BIRD.DLL implementation).     */
/******************************************************************************/

typedef struct tagFOB_RawPosition      // Bird position structure... 
{
    short     nX;                      // X-coordinate
    short     nY;                      // Y-coordinate
    short     nZ;                      // Z-coordinate
}
FOB_RawPosition;


typedef struct tagFOB_RawAngles        // Bird angles structure...
{
    short     nAzimuth;                // Azimuth angle
    short     nElevation;              // Elevation angle
    short     nRoll;                   // Roll angle
}
FOB_RawAngles;


typedef struct tagFOB_RawMatrix        // Bird matrix structure...
{
#define FOB_MATRIX_N         9
    short     n[FOB_MATRIX_N];         // Array of matrix (3 x 3) elements
}
FOB_RawMatrix;


typedef struct tagFOB_RawQuaternion    // Bird quaternion structure....
{
#define FOB_QUATERNION_N     4
    short     nQ[FOB_QUATERNION_N];    // Q[0,1,2,3]
}
FOB_RawQuaternion;


typedef struct tagFOB_RawReading       // Bird reading structure...
{
    FOB_RawPosition     position;      // Raw position values.
    FOB_RawAngles       angles;        // Raw angle values.
    FOB_RawMatrix       matrix;        // Raw rotation matrix.
    FOB_RawQuaternion   quaternion;    // Raw quaternion values.
}
FOB_RawReading;


typedef struct tagFOB_RawFrame         // FOB frame structure...
{
    DWORD               frame;         // Frame number in current session.
    BOOL                fresh;         // Fresh data flag.
                                       // One reading for each bird in flock...
    FOB_RawReading      reading[FOB_BIRD_MAX+1];
}
FOB_RawFrame;

#define FOB_FRAME_WORD   128           // Number of words in FOB frame.

/******************************************************************************/
/* Various FOB codes...                                                       */
/******************************************************************************/

// FOB data formats...
#define FOB_DATAFORMAT_NOBIRDDATA      0
#define FOB_DATAFORMAT_POSITION        1
#define FOB_DATAFORMAT_ANGLES          2
#define FOB_DATAFORMAT_MATRIX          3
#define FOB_DATAFORMAT_POSNANGLES      4
#define FOB_DATAFORMAT_POSNMATRIX      5
#define FOB_DATAFORMAT_QUATERNION      6
#define FOB_DATAFORMAT_POSNQUATERNION  7

// FOB hemisphere codes...
#define FOB_HEMISPHERE_INIT            0xFF
#define FOB_HEMISPHERE_FRONT           0
#define FOB_HEMISPHERE_REAR            1
#define FOB_HEMISPHERE_UPPER           2
#define FOB_HEMISPHERE_LOWER           3
#define FOB_HEMISPHERE_LEFT            4
#define FOB_HEMISPHERE_RIGHT           5
#define FOB_HEMISPHERE_MAX             6

// FOB values for hemi-axis and hemi-sign...
#define FOB_HEMI_AXIS_X      0x00
#define FOB_HEMI_AXIS_Z      0x0C
#define FOB_HEMI_AXIS_Y      0x06

#define FOB_HEMI_SIGN_P      0x00
#define FOB_HEMI_SIGN_N      0x01

// FOB group mode...
#define FOB_GROUPMODE_OFF              0
#define FOB_GROUPMODE_ON               1

// Sudden output change lock...
#define FOB_OUTPUTCHANGE_UNLOCK        0
#define FOB_OUTPUTCHANGE_LOCK          1

// Units for angles...
#define FOB_ANGLEUNIT_DEGREES          0
#define FOB_ANGLEUNIT_RADIANS          1

// FOB error codes...
#define FOB_ERROR_NONE                 0
#define FOB_ERROR_RAM                  1
#define FOB_ERROR_EEPROMWRITE          2
#define FOB_ERROR_EEPROMCORRUPT        3
#define FOB_ERROR_TRANSMITCORRUPT      4
#define FOB_ERROR_SENSORCORRUPT        5
#define FOB_ERROR_RS232COMMAND         6
#define FOB_ERROR_NOTFBBMASTER         7
#define FOB_ERROR_NOBIRDS              8
#define FOB_ERROR_BIRDINIT             9
#define FOB_ERROR_FBBRECVBIRD         10
#define FOB_ERROR_RS232RECV           11
#define FOB_ERROR_FBBBRECVHOST        12
#define FOB_ERROR_FBBRECVSLAVE        13
#define FOB_ERROR_FBBCOMMAND          14
#define FOB_ERROR_FBBRUNTIME          15
#define FOB_ERROR_CPUSPEED            16
#define FOB_ERROR_NODATA              17
#define FOB_ERROR_BAUDRATE            18
#define FOB_ERROR_SLAVEACK            19
#define FOB_ERROR_CRTSYNC             28
#define FOB_ERROR_NOTRANSMIT          29
#define FOB_ERROR_NOEXTTRANSMIT       30
#define FOB_ERROR_CPUTIME             31
#define FOB_ERROR_SATURATED           32
#define FOB_ERROR_SLAVECONFIG         33
#define FOB_ERROR_WATCHDOG            34
#define FOB_ERROR_OVERTEMP            35
#define FOB_ERROR_GENERAL             -1

// FOB data collection modes...
#define FOB_DATAMODE_DRC          0
#define FOB_DATAMODE_POINT        1
#define FOB_DATAMODE_STREAM       2

// Fixed position scales (inches)...
#define FOB_SCALE_36INCH       36              
#define FOB_SCALE_72INCH       72

// Fixed position scales (flag)...
#define FOB_SCALEFLAG_36INCH   0x0000              
#define FOB_SCALEFLAG_72INCH   0x0001

// Bird measurement rate divisor...
#define FOB_BIRDRATE_DIVISOR   256.0

// FOB commands...
#define FOB_CMD_ANGLEALIGN     0x4A
#define FOB_CMD_HEMISPHERE     0x4C
#define FOB_CMD_BUTTONMODE     0x4D
#define FOB_CMD_BUTTONREAD     0x4E
#define FOB_CMD_CHANGEVALUE    0x50
#define FOB_CMD_EXAMINEVALUE   0x4F
#define FOB_CMD_FACTORYTEST    0x7A
#define FOB_CMD_FBBRESET       0x2F
#define FOB_CMD_MATRIX         0x58
#define FOB_CMD_NEXTTRANS      0x30
#define FOB_CMD_POSN           0x56
#define FOB_CMD_ANGLES         0x57
#define FOB_CMD_POSNANGLES     0x59
#define FOB_CMD_POSNMATRIX     0x5A
#define FOB_CMD_QUATERNION     0x5C
#define FOB_CMD_POSNQUATERNION 0x5D
#define FOB_CMD_REFERENCEFRAME 0x48
#define FOB_CMD_RUN            0x46
#define FOB_CMD_SLEEP          0x47
#define FOB_CMD_SYNC           0x41
#define FOB_CMD_XOFF           0x13
#define FOB_CMD_XON            0x11
#define FOB_CMD_POINT          0x42
#define FOB_CMD_STREAM         0x40

// FOB FBB addressing...
#define FOB_FBB_ADDRESS        0xF0

// FOB Parameter (value) numbers...
#define FOB_VAL_BIRDSTATUS     0x00
#define FOB_VAL_SOFTWARE       0x01
#define FOB_VAL_BIRDSPEED      0x02
#define FOB_VAL_POSNSCALE      0x03
#define FOB_VAL_FILTERSTATUS   0x04
#define FOB_VAL_DCALPHAMIN     0x05
#define FOB_VAL_BIRDRATECOUNT  0x06
#define FOB_VAL_BIRDRATE       0x07
#define FOB_VAL_DATAREADY      0x08
#define FOB_VAL_DATAREADYCHAR  0x09
#define FOB_VAL_ERRORCODE      0x0A
#define FOB_VAL_ONERROR        0x0B
#define FOB_VAL_DCTABLEVM      0x0C
#define FOB_VAL_DCALPHAMAX     0x0D
#define FOB_VAL_OUTPUTCHANGE   0x0E
#define FOB_VAL_SYSTEMID       0x0F
#define FOB_VAL_EXTERRORCODE   0x10
#define FOB_VAL_XYZFRAME       0x11
#define FOB_VAL_TRANSMITMODE   0x12
#define FOB_VAL_FBBMODE        0x13
#define FOB_VAL_FILTERFREQ     0x14
#define FOB_VAL_FBBADDRESS     0x15
#define FOB_VAL_HEMISPHERE     0x16
#define FOB_VAL_ANGLEALIGN2    0x17
#define FOB_VAL_REFFRAME2      0x18
#define FOB_VAL_BIRDSERIAL     0x19
#define FOB_VAL_SENSORSERIAL   0x1A
#define FOB_VAL_TRANSMITSERIAL 0x1B
#define FOB_VAL_FBBHOSTDELAY   0x20
#define FOB_VAL_GROUPMODE      0x23
#define FOB_VAL_FLOCKSTATUS    0x24
#define FOB_VAL_AUTOCONFIG     0x32

// Bit fields for parameter FOB_VAL_BIRDSTATUS...
#define FOB_BIRDSTATUS_MASTER     0x8000    // B15
#define FOB_BIRDSTATUS_CONFIG     0x4000    // B14
#define FOB_BIRDSTATUS_ERROR      0x2000    // B13
#define FOB_BIRDSTATUS_RUNNING    0x1000    // B12
#define FOB_BIRDSTATUS_HOSTSYNC   0x0800    // B11
#define FOB_BIRDSTATUS_EXPANDED   0x0400    // B10
#define FOB_BIRDSTATUS_CRTSYNC    0x0200    // B9 
#define FOB_BIRDSTATUS_NOSYNC     0x0100    // B8 
#define FOB_BIRDSTATUS_TESTMODE   0x0080    // B7 
#define FOB_BIRDSTATUS_XOFF       0x0040    // B6 
#define FOB_BIRDSTATUS_SLEEP      0x0020    // B5 
#define FOB_BIRDSTATUS_FORMAT     0x001E    // B1-4
#define FOB_BIRDSTATUS_POS        0x0002    // B1-4   
#define FOB_BIRDSTATUS_ANG        0x0004    // B1-4
#define FOB_BIRDSTATUS_MTX        0x0006    // B1-4
#define FOB_BIRDSTATUS_POSANG     0x0008    // B1-4
#define FOB_BIRDSTATUS_POSMTX     0x000A    // B1-4
#define FOB_BIRDSTATUS_QTN        0x000C    // B1-4
#define FOB_BIRDSTATUS_POSQTN     0x0010    // B1-4
#define FOB_BIRDSTATUS_STREAM     0x0001    // B0

// Bit fields for parameter FOB_VAL_FLOCKSTATUS...
#define FOB_FLOCKSTATUS_USED      0x80      // B7
#define FOB_FLOCKSTATUS_RUNNING   0x40      // B6
#define FOB_FLOCKSTATUS_SENSOR    0x20      // B5
#define FOB_FLOCKSTATUS_ERT       0x10      // B4
#define FOB_FLOCKSTATUS_TRANSMIT  0x01      // B0

// Values for command FOB_CMD_SYNC...
#define FOB_SYNC_NONE   0x00
#define FOB_SYNC_CRT1   0x01
#define FOB_SYNC_CRT2   0x02
#define FOB_SYNC_HOST   0x08
#define FOB_SYNC_TEST   0xFF

// Values for parameter FOB_VAL_FILTER...
#define FOB_FILTER_ACNARROWOFF    0x0004    // B2
#define FOB_FILTER_ACWIDEOFF      0x0002    // B1
#define FOB_FILTER_DCOFF          0x0001    // B0

// Values for parameter FOB_VAL_TRANSMITMODE...
#define FOB_TRANSMITMODE_NONPULSED     0
#define FOB_TRANSMITMODE_PULSED        1
#define FOB_TRANSMITMODE_COOLDOWN      2

// Values for filter configuration...
#define FOB_FILTER_NONE           0
#define FOB_FILTER_ACNARROW       1
#define FOB_FILTER_ACWIDE         2
#define FOB_FILTER_DC             3

// Size of various filter arrays...
#define FOB_FILTER_MAX            3         // Number of different filters.
#define FOB_DC_TABLE              7         // Size of DC filter table.

// FOB Rotation Angles (Note: Different assignment than in FOB manual)...
#define FOB_ANGLE_AZIM       SPMX_EAMX_Z
#define FOB_ANGLE_ELEV       SPMX_EAMX_Y
#define FOB_ANGLE_ROLL       SPMX_EAMX_X

#define FOB_ANGLE_AZIM_C     FOB_ANGLE_AZIM,1
#define FOB_ANGLE_ELEV_C     FOB_ANGLE_ELEV,1
#define FOB_ANGLE_ROLL_C     FOB_ANGLE_ROLL,1

/******************************************************************************/
/* FOB configuration variables...                                             */
/******************************************************************************/

extern int    FOB_comX[FOB_PORT_MAX];
extern int    FOB_birds;
extern long   FOB_baudrate;
extern float  FOB_framerate;
extern WORD   FOB_scale;
extern BYTE   FOB_dataformat[FOB_BIRD_MAX];
extern BYTE   FOB_datamode;
extern BYTE   FOB_groupmode;
extern BYTE   FOB_hemisphere[FOB_BIRD_MAX];
extern BYTE   FOB_angleunit;
extern WORD   FOB_filterstatus;
extern WORD   FOB_DCalphamin[FOB_DC_TABLE];
extern WORD   FOB_DCalphamax[FOB_DC_TABLE];
extern WORD   FOB_DCtableVm[FOB_DC_TABLE];
extern BYTE   FOB_outputchange;
extern int    FOB_axis;

extern float  FOB_frameHz;
extern float  FOB_framemsec;

extern struct FOB_PortHandle FOB_Port[FOB_PORT_MAX];
extern FOB_RawFrame          FOB_RawData[FOB_PORT_MAX];

/******************************************************************************/
/* FOB-CMDS.CPP  FOB RS232 commands...                                        */
/******************************************************************************/

struct  FOB_FlockStatusBird       // Status for each bird (address) in flock.
{
    BYTE addr;                    // FBB address.
    BOOL awake;                   // Bird awake (auto-configured and not sleeping).
    BOOL bird;                    // Sensor (bird) present.
    BOOL xmit;                    // Transmitter present.
    BOOL ERT;                     // Extended Range Transmitter.
};

struct  FOB_StatusFBB             // FBB ("Auto-Config") status.
{
    BYTE mode;                    // Mode flag...
#define FOB_FBB_STANDALONE   0
#define FOB_FBB_MASTERSLAVE  1

    WORD devices;                 // Devices present (bit mapped).
    WORD dependent;               // Devices dependent on MASTER (bit mapped).
};

struct  FOB_BirdFBB               // FBB Status for each bird (address) in flock.
{
    BYTE addr;                    // FBB address.
    BOOL dependent;               // Dependent on MASTER flag.
};

/******************************************************************************/

UCHAR   FOB_FBB( BYTE addr );          // Return a FBB bird address.

#define FOB_CMD_BUFF   256             // FOB RS232 command buffer size.

//      These functions sequentially build a FOB command string...
void    FOB_CmdBuffInit( void );                      // Reset the buffer.
void    FOB_CmdBuffByte( BYTE data );                 // But a BYTE in the buffer.
void    FOB_CmdBuffWord( WORD data );                 // Put a WORD in the buffer.
void    FOB_CmdBuffData( void *data, int size );      // Put some data in the buffer.
void    FOB_CmdBuffFBB( int ID, int bird );           // Put a bird FBB address in the buffer.
void    FOB_CmdBuffCmd( BYTE cmd );                   // Put a FOB command byte in the buffer.
int     FOB_CmdBuffSize( void );                      // Return the current size of the buffer.
//      These functions send (and receive) the current FOB command string.
BOOL    FOB_CmdBuffWrite( int comH );
BOOL    FOB_CmdBuffSend( int ID );
BOOL    FOB_CmdBuffSend( int ID, void *recv, int size );

//      Various time delays (msec), waiting for things to happen...
#define FOB_WAIT_AUTOCONFIG  600.0                    // Auto-configure.
#define FOB_WAIT_RTS         250.0                    // RTS reset.
#define FOB_WAIT_RESET      1750.0                    // Reset.

//      Various timeouts (msec), waiting for frame data...
#define FOB_TIMEOUT_FIRST     20.0                    // Wait for first byte of frame.
#define FOB_TIMEOUT_REST      20.0                    // Wait for the rest of frame.

//      The following functions implement specific FOB commands...
BOOL    FOB_CmdDataFormat( int ID );
BOOL    FOB_CmdConfig( int ID );
BOOL    FOB_CmdAutoConfig( int ID );
BOOL    FOB_CmdRun( int ID );
BOOL    FOB_CmdResetViaFBB( int ID );
BOOL    FOB_CmdResetViaRTS( int ID, BOOL nowait );
BOOL    FOB_CmdSleep( int ID );
BOOL    FOB_CmdSync( int ID, BYTE mode, float &voltage, float &scanrate );
BOOL    FOB_CmdSync( int ID, BYTE mode );
BOOL    FOB_CmdPoint( int ID, int bird );
BOOL    FOB_CmdStream( int ID );
BOOL    FOB_CmdHemisphere( int ID, int bird, BYTE axis, BYTE sign );
BOOL    FOB_CmdHemisphere( int ID, int bird, int hemisphere );

//      General GET / SET functions for a FOB parameter...
BOOL    FOB_CmdGetValue( int ID, int bird, BYTE value, void *buff, int size );
BOOL    FOB_CmdSetValue( int ID, int bird, BYTE value, void *buff, int size );

//      General GET / SET functions a FOB parameter BYTE or WORD or whatever...
BOOL    FOB_ValByte( int ID, int bird, BYTE value, int func, BYTE *buff );
BOOL    FOB_ValWord( int ID, int bird, BYTE value, int func, WORD *buff );
BOOL    FOB_ValBuff( int ID, int bird, BYTE value, int func, void *buff, int size );
#define FOB_VAL_GET     0
#define FOB_VAL_SET     1

//      GET / SET a specific FOB parameter...
BOOL    FOB_GetBirdStatus( int ID, int bird, WORD &status );
BOOL    FOB_GetFlockStatus( int ID, BYTE status[] );
BOOL    FOB_ValFilterStatus( int ID, int bird, int func, WORD &filter );
BOOL    FOB_ValFilterFreq( int ID, int bird, int func, BYTE &freq );
BOOL    FOB_ValDCAlphaMin( int ID, int bird, int func, WORD buff[] );
BOOL    FOB_ValDCAlphaMax( int ID, int bird, int func, WORD buff[] );
BOOL    FOB_ValDCTableVm( int ID, int bird, int func, WORD buff[] );
BOOL    FOB_ValDataReady( int ID, int bird, int func, BYTE &dr );
BOOL    FOB_ValDataReadyChar( int ID, int bird, int func, BYTE &drc );
BOOL    FOB_ValOutputChange( int ID, int bird, int func, BYTE &flag );
BOOL    FOB_ValGroupMode( int ID, int func, BYTE &flag );
BOOL    FOB_ValScale( int ID, int bird, int func, WORD &scale );
BOOL    FOB_ValBirdRate( int ID, int bird, int func, float &rate );
BOOL    FOB_ValHemisphere( int ID, int bird, int func, int &hemisphere );
BOOL    FOB_GetErrorCode( int ID, int bird, BYTE &code, BOOL &fatal );
BOOL    FOB_GetErrorCode( int ID, int bird, BYTE &code );
BOOL    FOB_ValTransmitMode( int ID, int func, BYTE &mode );
BOOL    FOB_GetBirdSpeed( int ID, WORD &speed );

void    FOB_PutFlockStatus( BYTE addr, BYTE status, struct FOB_FlockStatusBird *bird );
int     FOB_GetFlockStatus( int ID, struct FOB_FlockStatusBird status[] );

int     FOB_PutFBB( int ID, struct FOB_StatusFBB *flag, BOOL &MS, FOB_BirdFBB fbb[] );
int     FOB_GetFBB( int ID, BOOL &MS, struct FOB_BirdFBB fbb[] );
BOOL    FOB_SetFBB( int ID, BYTE birds );

//      FOB reset commands...
BOOL    FOB_ResetFOB( int ID, BOOL nowait );
BOOL    FOB_ResetFOB( int ID );
BOOL    FOB_ResetCOM( int ID );

/******************************************************************************/
/* FOB-INFO.CPP  Information structures and functions.                        */
/******************************************************************************/

extern  struct  STR_TextItem      FOB_HemisphereText[];
extern  int                       FOB_FrameSize[];

//      What data formats are returned in current session...
BOOL    FOB_DataType( int ID, int bird, BOOL list[] );
BOOL    FOB_DataType( int ID, BOOL list[] );
BOOL    FOB_DataType( BOOL list[] );

//      Data format for a specific port and bird...
BOOL    FOB_DataTypePosition( int ID, int bird );
BOOL    FOB_DataTypeAngles( int ID, int bird );
BOOL    FOB_DataTypeMatrix( int ID, int bird );
BOOL    FOB_DataTypeQuaternion( int ID, int bird );

//      Data format for a specific bird (on any port)...
BOOL    FOB_DataTypePosition( int bird );
BOOL    FOB_DataTypeAngles( int bird );
BOOL    FOB_DataTypeMatrix( int bird );
BOOL    FOB_DataTypeQuaternion( int bird );
BOOL    FOB_DataTypePOMX( int bird );
BOOL    FOB_DataTypeROMX( int bird );
BOOL    FOB_DataTypeRTMX( int bird );

//      Data format for all birds on any port...
BOOL    FOB_DataTypePosition( void );
BOOL    FOB_DataTypeAngles( void );
BOOL    FOB_DataTypeMatrix( void );
BOOL    FOB_DataTypeQuaternion( void );

void    FOB_ScaleCode( WORD scale, WORD &code );
WORD    FOB_ScaleCode( WORD code );

WORD    FOB_BirdRate2Code( float rate );
float   FOB_BirdCode2Rate( WORD code );

int     FOB_HemisphereCode( WORD code );
void    FOB_HemisphereCode( int hemisphere, WORD &code);

BOOL    FOB_ErrorFatal( BYTE code );             // Is this error code fatal?

/******************************************************************************/
/* FOB-DATA.CPP  Data conversions and stuff...                                */
/******************************************************************************/

#define FOB_FRAME_BIT   0x80                // Framing bit in FOB data.

#define FOB_LSB         0                   // Least Significant Byte index.
#define FOB_MSB         1                   // Most Significant Byte index.

BOOL    FOB_FrameByte( BYTE &data );        // Is framing bit set on this byte?
WORD    FOB_FrameWord( WORD data );         // Convert FOB word to regular format.

#define FOB_ANGLE_SCALE      180            // For converting from FOB short to angle (degrees).
#define FOB_MATRIX_SCALE     1.0            // For converting from FOB short to matrix value.
#define FOB_QUATERNION_SCALE 1.0            // For converting from FOB short to quaternion value.

//      Convert raw FOB data to various units...
float   FOB_Raw2Inch( short raw, WORD scale );
float   FOB_Inch2cm( float inch );
float   FOB_Raw2cm( short raw, WORD scale );
float   FOB_Raw2Degree( short raw, short scale );
float   FOB_Degree2Radian( float degree );
float   FOB_Raw2Radian( short raw, short scale );
float   FOB_Raw2Float( short raw, float scale );

//      Convert raw FOB frames to various SPMX matrices...
void    FOB_Raw2POMX( FOB_RawPosition *raw, WORD scale, matrix &pomx );
void    FOB_Raw2POMX( int ID, FOB_RawPosition *raw, matrix &pomx );
void    FOB_Raw2DegreeEUMX( FOB_RawAngles *raw, matrix &eamx );
void    FOB_Raw2RadianEUMX( FOB_RawAngles *raw, matrix &eamx );
void    FOB_Raw2EUMX( int ID, FOB_RawAngles *raw, matrix &eamx );
void    FOB_Raw2ROMX( FOB_RawMatrix *raw, matrix &romx );
void    FOB_Raw2QTMX( FOB_RawQuaternion *raw, matrix &qtmx );

//      Convert raw FOB data words to raw FOB frames...
int     FOB_Data2Angles( WORD data[], FOB_RawAngles *angles );
int     FOB_Data2Position( WORD data[], FOB_RawPosition *position );
int     FOB_Data2Matrix( WORD data[], FOB_RawMatrix *mtx );
int     FOB_Data2Quaternion( WORD data[], FOB_RawQuaternion *quaternion );
int     FOB_Data2Frame( int ID, int bird, WORD data[], FOB_RawFrame *frame );
int     FOB_Data2Frame( BYTE dataformat, WORD data[], FOB_RawReading *reading );

//      Convert a single FOB frame (raw data) for a specific bird to the appropriate application SPMX matrices...
void    FOB_PutSPMX( int ID, int bird, BYTE dataformat, FOB_RawReading *data, matrix *pomx, matrix *eamx, matrix *romx, matrix *qtmx, matrix *rtmx );
void    FOB_PutSPMX( int ID, int bird, BYTE dataformat, FOB_RawReading *data, matrix *pomx, matrix *eamx, matrix *romx, matrix *qtmx );

//      Convert a single FOB frame (raw data) for a specific bird to a particular SPMX matrix...
void    FOB_PutPOMX( int ID, int bird, FOB_RawReading *data, matrix &pomx );
void    FOB_PutROMX( int ID, int bird, FOB_RawReading *data, matrix &romx );
void    FOB_PutRTMX( int ID, int bird, FOB_RawReading *data, matrix &rtmx );

//      Convert array of FOB frames (raw data) for all birds into appropriate application SPMX matrices...
void    FOB_PutSPMX( int ID, FOB_RawReading data[], matrix *pomx, matrix *eamx, matrix *romx, matrix *qtmx, matrix *rtmx );
void    FOB_PutPOMX( int ID, FOB_RawReading data[], matrix pomx[] );
void    FOB_PutEUMX( int ID, FOB_RawReading data[], matrix eamx[] );
void    FOB_PutROMX( int ID, FOB_RawReading data[], matrix romx[] );
void    FOB_PutQTMX( int ID, FOB_RawReading data[], matrix qtmx[] );
void    FOB_PutRTMX( int ID, FOB_RawReading data[], matrix rtmx[] );

//      Convert various SPMX matrices to user-defined co-ordinate frame...
void    FOB_AxisPOMX( matrix &pomx );
void    FOB_AxisROMX( matrix &romx );
void    FOB_AxisRTMX( matrix &rtmx );

//      Track positions as birds move through various transmitter hemispheres...
void    FOB_TrackHemisphere( int ID, int bird, matrix &pomx );

/******************************************************************************/
/* FOB-TASK.CPP  Data stream loop task stuff...                               */
/******************************************************************************/

void    FOB_StreamTask( int ID );
void    FOB_LoopTask( void );

BOOL    FOB_StreamFresh( int ID );
BOOL    FOB_StreamFresh( void );

BOOL    FOB_PortError( int ID );
BOOL    FOB_PortError( void );

BOOL    FOB_LastData( double frameage[], BOOL &fresh, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[] );
BOOL    FOB_LastData( double frameage[], BOOL &fresh, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[] );
BOOL    FOB_LastData( BOOL &fresh, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[] );
BOOL    FOB_LastData( BOOL &fresh, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[] );

BOOL    FOB_LastData( double frameage[], matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[] );
BOOL    FOB_LastData( double frameage[], matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[] );
BOOL    FOB_LastData( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[] );
BOOL    FOB_LastData( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[] );

BOOL    FOB_Data( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[], BOOL &fresh );
BOOL    FOB_Data( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], BOOL &fresh );
BOOL    FOB_Data( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[] );
BOOL    FOB_Data( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[] );

/******************************************************************************/

struct  FOB_PortHandle                           // Information about a port handle...
{
    BOOL    used;                                // Handle item in use.
    BOOL    started;                             // Data collection started (Birds are flying).
    int     comH;                                // Com port handle.
    int     comX;                                // Com port number.
    long    baudrate;                            // Com port baud rate.
    int     birdsinflock;                        // Total number of birds in flock.
    int     birdsonport;                         // Number of birds on this port.
    WORD    scale;                               // Position scale (inches).
    BYTE    address[FOB_BIRD_MAX];               // FBB address for each bird on port.
    BYTE    dataformat[FOB_BIRD_MAX];            // Data format.
    BYTE    hemisphere[FOB_BIRD_MAX];            // Transmitter hemisphere.

    WORD    birdspeed;                           // Clock speed of bird's CPU.

    TIMER           *session;
    TIMER_Frequency *requestrate;
    TIMER_Frequency *looprate;
    TIMER_Frequency *framerate;
    TIMER_Interval  *frametime;
    TIMER           *framelast;
    DATAPROC        *framewait;

    long    frame;
    long    sync;
    long    overwrite;
 
    int     size;
    int     got;

    BOOL    error;
    BOOL    datasync;
    BOOL    framedata;

#define FOB_BUFF           512
    BYTE    buff[FOB_BUFF];

    FOB_RawReading reading[FOB_BIRD_MAX];

    int     data[FOB_BIRD_MAX];                  // Offset into frame for bird's data.
    int     addr[FOB_BIRD_MAX];                  // Offset into frame for bird's address.
    BYTE    tracking[FOB_BIRD_MAX];              // Current hemisphere for tracking.
    matrix  pomx[FOB_BIRD_MAX];                  // Last position for hemisphere tracking.

    int    last;
    int    stage;
#define FOB_STAGE_START      0
#define FOB_STAGE_WAITING    1
#define FOB_STAGE_DATA       2
#define FOB_STAGE_RECORD     3

};

/******************************************************************************/

#define FOB_DATAREADYCHAR      0x7F    // Character for data ready notification.

/******************************************************************************/

#define FOB_NULL_WORD               0xFFFF
#define FOB_DEFAULT_WORD            FOB_NULL_WORD

#define FOB_NULL_BYTE               0xFF
#define FOB_DEFAULT_BYTE            FOB_NULL_BYTE

/******************************************************************************/

int     FOB_messgf( const char *mask, ... );
int     FOB_errorf( const char *mask, ... );
int     FOB_debugf( const char *mask, ... );

/******************************************************************************/

void    FOB_CnfgTable( void );
void    FOB_CnfgDefaults( void );
void    FOB_CnfgRead( void );
BOOL    FOB_CnfgLoad( char *file );
BOOL    FOB_CnfgAxis( char *type, int &axis );

void    FOB_DCB( DCB *dcb );

void    FOB_Init( int ID );
int     FOB_Free( void );

void    FOB_Use( int ID );

BOOL    FOB_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    FOB_API_stop( void );
BOOL    FOB_API_check( void );

BOOL    FOB_Check( int ID );

char   *FOB_Error( BYTE code );

BOOL    FOB_DataMode( int ID, BYTE datamode, BOOL flag );

BOOL    FOB_GetFirstByte( int ID, BYTE *buff, BOOL wait, BOOL &framed );
BOOL    FOB_GetFirstByte( int ID, BYTE *buff, BOOL &framed );
BOOL    FOB_GetFirstByte( int ID, BYTE *buff );

BOOL    FOB_GetTheRest( int ID, BYTE *buff, int size, long wait );
BOOL    FOB_GetTheRest( int ID, BYTE *buff, int size );

BOOL    FOB_GetNextByte( int ID, BYTE *buff, BOOL &done );

BOOL    FOB_GetPoint( int ID, int bird, WORD buff[], int size );
BOOL    FOB_GetPoint( int ID, WORD buff[], int size );
BOOL    FOB_GetStream( int ID, WORD buff[], int size, BOOL &framed );

BOOL    FOB_GetNonGroup( int ID, FOB_RawFrame *data, BOOL &fresh );
BOOL    FOB_GetGroup( int ID, FOB_RawFrame *data, BOOL &fresh );

BOOL    FOB_GetFresh( int ID, FOB_RawFrame *data, BOOL &fresh );
BOOL    FOB_GetFrame( int ID, FOB_RawFrame *data );

BOOL    FOB_BirdStatusOK( int ID );
BOOL    FOB_FlockStatusOK( int ID );

BOOL    FOB_SetFormat( int ID, BYTE dataformat );

/******************************************************************************/

/* Application-level function calls...                                        */
/******************************************************************************/

//      Check parameters for opening FOB...
BOOL    FOB_OpenParameters( int birdsinflock, int birdsonport, BYTE &datamode, BYTE &groupmode, BOOL &multiport, BOOL &multibird );

//      Open the FOB device on a particular RS232 port...
int     FOB_Open( int comX, int birdsinflock, int birdsonport, BYTE addr, long baud, double freq, WORD scale, BYTE &datamode, BYTE &groupmode, BYTE dataformat[], BYTE hemisphere[] );
BOOL    FOB_Open( int comX[], int birds, BYTE addr, long baud, double freq, WORD scale, BYTE &datamode, BYTE &groupmode, BYTE dataformat[], BYTE hemisphere[] );
BOOL    FOB_Open( char *cnfg );

//      Close FOB device...
void    FOB_Close( int ID );
void    FOB_CloseAll( void );

//      Start / Stop a FOB data collection session...
BOOL    FOB_Start( int ID );
void    FOB_Stop( int ID );

//      Get latest position information for all birds in flock...
BOOL    FOB_GetPosn( int ID, matrix pomx[], BOOL &fresh );
BOOL    FOB_GetPosn( int ID, matrix pomx[] );

//      Get latest rotation angles for all birds in flock...
BOOL    FOB_GetAngles( int ID, matrix eamx[], BOOL &fresh );
BOOL    FOB_GetAngles( int ID, matrix eamx[] );

//      Get latest position and rotation angles for all birds in flock...
BOOL    FOB_GetPosnAngles( int ID, matrix pomx[], matrix eamx[], BOOL &fresh );
BOOL    FOB_GetPosnAngles( int ID, matrix pomx[], matrix eamx[] );

//      Get latest rotation matrix for all birds in flock...
BOOL    FOB_GetROMX( int ID, matrix romx[], BOOL &fresh );
BOOL    FOB_GetROMX( int ID, matrix romx[] );

//      Get latest position and rotation matrix for all birds in flock...
BOOL    FOB_GetPosnROMX( int ID, matrix pomx[], matrix romx[], BOOL &fresh );
BOOL    FOB_GetPosnROMX( int ID, matrix pomx[], matrix romx[] );

//      Get latest rotation / translation matrix for all birds in flock...
BOOL    FOB_GetRTMX( int ID, matrix pomx[], matrix romx[], matrix rtmx[], BOOL &fresh );
BOOL    FOB_GetRTMX( int ID, matrix pomx[], matrix romx[], matrix rtmx[] );
BOOL    FOB_GetRTMX( int ID, matrix rtmx[], BOOL &fresh );
BOOL    FOB_GetRTMX( int ID, matrix rtmx[] );

//      Get latest quaternion matrix for all birds in flock...
BOOL    FOB_GetQTMX( int ID, matrix qtmx[], BOOL &fresh );
BOOL    FOB_GetQTMX( int ID, matrix qtmx[] );

//      Get latest position and quaternion matrix for all birds in flock...
BOOL    FOB_GetPosnQTMX( int ID, matrix pomx[], matrix qtmx[], BOOL &fresh );
BOOL    FOB_GetPosnQTMX( int ID, matrix pomx[], matrix qtmx[] );

//      Get latest data (depending on data format) for all birds in flock...
BOOL    FOB_GetData( int ID, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[], BOOL &fresh );
BOOL    FOB_GetData( int ID, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], BOOL &fresh );
BOOL    FOB_GetData( int ID, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[] );
BOOL    FOB_GetData( int ID, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[] );

//      Return information about the flock...
int     FOB_BirdsInFlock( void );               // How many birds in flock?
int     FOB_PortsInFlock( void );               // How many ports in flock?
BOOL    FOB_MultiPortFlock( void );             // Multiple ports in flock?
int     FOB_BirdsOnPort( int ID );              // How many birds on this port?
BOOL    FOB_MultiBirdPort( int ID );            // More than one bird on this port?
BOOL    FOB_StandAlonePort( int ID );           // Single bird on this port?
int     FOB_BirdIndex( int ID, BYTE addr );     // Bird index (0..n) for address.
BOOL    FOB_Master( int ID );                   // Is the MASTER on this port?
int     FOB_Master( void );                     // Return port ID for MASTER.

//      Return information about the FOB...
BOOL    FOB_Started( int ID, BOOL verbose );    // Data collection started?
BOOL    FOB_Started( int ID );
BOOL    FOB_Started( void );
BOOL    FOB_Flying( int ID );                   // Are the birds flying?
BOOL    FOB_Streaming( int ID );                // Data streaming started?
BOOL    FOB_Streaming( void );
WORD    FOB_PosnScale( int ID );                // Position scale in use (inches).
BYTE    FOB_AngleUnit( void );                  // What is the unit for angles?
BYTE    FOB_DataFormat( int ID, int bird );     // What is data format?

/******************************************************************************/
/* FOB-STUB.CPP  Functions stubs that use default port ID...                  */
/******************************************************************************/

void    FOB_Close( void );
BOOL    FOB_Start( void );
void    FOB_Stop( void );
BOOL    FOB_GetPosn( matrix pomx[], BOOL &fresh );
BOOL    FOB_GetPosn( matrix pomx[] );
BOOL    FOB_GetAngles( matrix eamx[], BOOL &fresh );
BOOL    FOB_GetAngles( matrix eamx[] );
BOOL    FOB_GetPosnAngles( matrix pomx[], matrix eamx[], BOOL &fresh );
BOOL    FOB_GetPosnAngles( matrix pomx[], matrix eamx[] );
BOOL    FOB_GetROMX( matrix romx[], BOOL &fresh );
BOOL    FOB_GetROMX( matrix romx[] );
BOOL    FOB_GetPosnROMX( matrix pomx[], matrix romx[], BOOL &fresh );
BOOL    FOB_GetPosnROMX( matrix pomx[], matrix romx[] );
BOOL    FOB_GetRTMX( matrix pomx[], matrix romx[], matrix rtmx[], BOOL &fresh );
BOOL    FOB_GetRTMX( matrix pomx[], matrix romx[], matrix rtmx[] );
BOOL    FOB_GetRTMX( matrix rtmx[], BOOL &fresh );
BOOL    FOB_GetRTMX( matrix rtmx[] );
BOOL    FOB_GetQTMX( matrix qtmx[], BOOL &fresh );
BOOL    FOB_GetQTMX( matrix qtmx[] );
BOOL    FOB_GetPosnQTMX( matrix pomx[], matrix qtmx[], BOOL &fresh );
BOOL    FOB_GetPosnQTMX( matrix pomx[], matrix qtmx[] );
BOOL    FOB_GetData( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[], BOOL &fresh );
BOOL    FOB_GetData( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], BOOL &fresh );
BOOL    FOB_GetData( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[] );
BOOL    FOB_GetData( matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[] );
BOOL    FOB_Flying( void );
int     FOB_BirdsInFlock( void );
WORD    FOB_PosnScale( void );

/******************************************************************************/

#pragma pack()	   // Resume default packing of structures.

/******************************************************************************/

#endif


