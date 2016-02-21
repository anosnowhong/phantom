/******************************************************************************/

#define GLOVE_DATA_SINGLE              'G'
#define GLOVE_DATA_STREAM              'S'

#define GLOVE_BAUDRATE                 'B'
#define GLOVE_CALIBRATE                'C'
#define GLOVE_SENSORMASK               'M'
#define GLOVE_SENSORSAMPLE             'N'
#define GLOVE_FLAGS                    'P'
#define GLOVE_SAMPLEPERIOD             'T'

#define GLOVE_REINITIALIZE            0x09
#define GLOVE_RESTART                 0x12
#define GLOVE_BREAK                   0x03

#define GLOVE_QUERY                    '?'

#define GLOVE_CONNECTED                'G'
#define GLOVE_INFORMATION              'I'
#define GLOVE_HARDWAREMASK             'K'
#define GLOVE_HAND                     'R'
#define GLOVE_SENSORS                  'S'
#define GLOVE_VERSION                  'V'
#define GLOVE_DISPLAYCODES             '?'

#define GLOVE_FLAG_TIMESTAMP           'D'
#define GLOVE_FLAG_FILTER              'F'
#define GLOVE_FLAG_SWITCHLIGHT         'J'
#define GLOVE_FLAG_LIGHT               'L'
#define GLOVE_FLAG_QUANTITIZED         'Q'
#define GLOVE_FLAG_GLOVESTATUS         'U'
#define GLOVE_FLAG_SWITCHSTATUS        'W'
#define GLOVE_FLAG_EXTERNALSYNC        'Y'

#define GLOVE_ERROR                    'e'

#define GLOVE_ERROR_COMMAND            '?'
#define GLOVE_ERROR_PARAMETERS         'n'
#define GLOVE_ERROR_SYNCFAST           'y'
#define GLOVE_ERROR_NOGLOVE            'g'
#define GLOVE_ERROR_SAMPLEFAST         's'

/******************************************************************************/

#define GLOVE_THUMB_TMJ           0
#define GLOVE_THUMB_MPJ           1
#define GLOVE_THUMB_IJ            2
#define GLOVE_THUMB_ABD           3
#define GLOVE_INDEX_MPJ           4
#define GLOVE_INDEX_PIJ           5
#define GLOVE_INDEX_DIJ           6
#define GLOVE_INDEX_ABD          -1
#define GLOVE_MIDDLE_MPJ          7
#define GLOVE_MIDDLE_PIJ          8
#define GLOVE_MIDDLE_DIJ          9
#define GLOVE_MIDDLE_INDEX_ABD   10
#define GLOVE_RING_MPJ           11
#define GLOVE_RING_PIJ           12
#define GLOVE_RING_DIJ           13
#define GLOVE_RING_MIDDLE_ABD    14
#define GLOVE_PINKIE_MPJ         15
#define GLOVE_PINKIE_PIJ         16
#define GLOVE_PINKIE_DIJ         17
#define GLOVE_PINKIE_RING_ABD    18
#define GLOVE_PALM_ARCH          19
#define GLOVE_WRIST_PITCH        20
#define GLOVE_WRIST_YAW          21

char *GLOVE_SensorName( int sensor );
int GLOVE_SensorCode( char *name );

/******************************************************************************/

#define GLOVE_SIZE_TIMESTAMP      5
#define GLOVE_SIZE_GLOVESTATUS    1

/******************************************************************************/

#define GLOVE_MSECPERTICK             0.00868048      // Internal timer tick (msec).

/******************************************************************************/

#define GLOVE_CONNECTED_INITIALIZED   1
#define GLOVE_CONNECTED_GLOVE         2

#define GLOVE_HAND_LEFT               0
#define GLOVE_HAND_RIGHT              1
#define GLOVE_HAND_INVALID           -1

#define GLOVE_SET  0
#define GLOVE_GET  1

/******************************************************************************/

#define GLOVE_PORT_MAX        2
#define GLOVE_PORT_INVALID   -1

#define GLOVE_SENSOR         23
#define GLOVE_SENSOR_MIN      0
#define GLOVE_SENSOR_MAX     22
#define GLOVE_SENSOR_INVALID -1

#define GLOVE_FLAG_MAX        7

/******************************************************************************/

struct  GLOVE_Handle
{
    BOOL      used;

    BOOL      started;
    BOOL      streaming;

    int       comX;
    int       comH;
    int       baudrate;

    int       sensors;

    int       hand;

    int       selected;

    int       size;

    int       stage;
#define GLOVE_STAGE_START    0
#define GLOVE_STAGE_WAITING  1
#define GLOVE_STAGE_COMMAND  2
#define GLOVE_STAGE_DATA     3
#define GLOVE_STAGE_NUL      4
#define GLOVE_STAGE_SYNC     5

    int       got;

#define GLOVE_BUFF      256
    BYTE      buff[GLOVE_BUFF];

    BOOL      query;
    BYTE      command;

    BOOL      error;

    BOOL      framedata;

    TIMER           *session;
    TIMER_Frequency *framerate;
    TIMER_Interval  *frametime;
    TIMER           *framelast;
    DATAPROC        *framewait;

    BYTE      data[GLOVE_SENSOR_MAX];
    double    timestamp;
    BYTE      glovestatus;
#define GLOVE_GLOVESTATUS_CONNECTED    0x01
#define GLOVE_GLOVESTATUS_SWITCH       0x02
#define GLOVE_GLOVESTATUS_LIGHT        0x03

    long      frame;
    long      sync;
    long      overwrite;

    BOOL      flag[GLOVE_FLAG_MAX];

    STRING    calibfile;
    matrix    calibdata;

    double    frameHz;
};

/******************************************************************************/

extern  struct  GLOVE_Handle  GLOVE_Port[GLOVE_PORT_MAX];

/******************************************************************************/

int     GLOVE_messgf( const char *mask, ... );
int     GLOVE_errorf( const char *mask, ... );
int     GLOVE_debugf( const char *mask, ... );

void    GLOVE_DCB( DCB *dcb );
void    GLOVE_Init( int ID );
int     GLOVE_Free( void );
void    GLOVE_Use( int ID );

BOOL    GLOVE_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    GLOVE_API_stop( void );
BOOL    GLOVE_API_check( void );

BOOL    GLOVE_Check( int ID );

BOOL    GLOVE_OpenParameters( void );

BOOL    GLOVE_Status( int ID );
BOOL    GLOVE_Config( int ID );

BOOL    GLOVE_CnfgLoad( char *file );

int     GLOVE_Open( char *cnfg );
void    GLOVE_Close( int ID );
void    GLOVE_CloseAll( void );

BOOL    GLOVE_Start( int ID, BOOL streaming );
BOOL    GLOVE_Start( int ID );
void    GLOVE_Stop( int ID );

BOOL    GLOVE_Response( int ID, BOOL framed, BYTE *recv, int &size );
BOOL    GLOVE_Response( int ID, BYTE *recv, int &size );

void    GLOVE_BuffInit( int item );

void    GLOVE_BuffSet( int item );
int     GLOVE_BuffGet( int item );

void    GLOVE_BuffPutByte( int item, BYTE data );
BYTE    GLOVE_BuffGetByte( int item );

void    GLOVE_BuffPutWord( int item, WORD data );
WORD    GLOVE_BuffGetWord( int item );

void    GLOVE_BuffGetData( int item, void *data, int size );
void    GLOVE_BuffPutData( int item, void *data, int size );

int     GLOVE_BuffSize( int item );

void    GLOVE_BuffCommand( BYTE code );
void    GLOVE_BuffQuery( BYTE code );

BOOL    GLOVE_BuffWrite( int comH );
BOOL    GLOVE_BuffSend( int ID  );

BOOL    GLOVE_BuffSend( int ID, BOOL framed, void *buff, int size );
BOOL    GLOVE_BuffSend( int ID, void *buff, int size );

BOOL    GLOVE_Command( int ID, int action, BYTE command, BYTE *data, int size );

BOOL    GLOVE_SamplePeriod( int ID, int action, WORD &w1, WORD &w2 );
BOOL    GLOVE_SampleRate( int ID, int action, double &rateHz );
BOOL    GLOVE_GetSampleRate( int ID, double &rateHz );
BOOL    GLOVE_SetSampleRate( int ID, double rateHz );

BOOL    GLOVE_GloveStatus( int ID, int action, BYTE &flag );
BOOL    GLOVE_GetGloveStatus( int ID, BYTE &flag );
BOOL    GLOVE_SetGloveStatus( int ID, BYTE flag );

BOOL    GLOVE_QueryConnected( int ID, BOOL &initialized, BOOL &connected );
BOOL    GLOVE_QuerySensors( int ID, int &sensors );
BOOL    GLOVE_QueryHand( int ID, int &hand );

BOOL   &GLOVE_Flag( int ID, int code );

BOOL    GLOVE_Flag( int ID, int action, BYTE code, BOOL &flag );

BOOL    GLOVE_FlagLight( int ID, int action, BOOL &flag );
BOOL    GLOVE_FlagSwitchLight( int ID, int action, BOOL &flag );
BOOL    GLOVE_FlagSwitchStatus( int ID, int action, BOOL &flag );
BOOL    GLOVE_FlagGloveStatus( int ID, int action, BOOL &flag );

BOOL    GLOVE_SetLight( int ID, BOOL flag );
BOOL    GLOVE_GetLight( int ID, BOOL &flag );

BOOL    GLOVE_SetSwitchLight( int ID, BOOL flag );
BOOL    GLOVE_GetSwitchLight( int ID, BOOL &flag );

BOOL    GLOVE_SetSwitchStatus( int ID, BOOL flag );
BOOL    GLOVE_GetSwitchStatus( int ID, BOOL &flag );

BOOL    GLOVE_SetGloveStatus( int ID, BOOL flag );
BOOL    GLOVE_GetGloveStatus( int ID, BOOL &flag );

int     GLOVE_RecordSize( int ID );

void    GLOVE_TimeStamp( int ID, BYTE *buff, int &b );
void    GLOVE_GloveStatus( int ID, BYTE *buff, int &b );

double  GLOVE_Raw2TimeStamp( BYTE *buff );
void    GLOVE_Raw2Data( int ID, BYTE *buff );

void    GLOVE_GetData( int ID, BYTE data[], double &timestamp, BYTE &glovestatus );

BOOL    GLOVE_GetRecord( int ID, BYTE data[], double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_GetRecord( int ID, BYTE data[], double &timestamp );
BOOL    GLOVE_GetRecord( int ID, BYTE data[] );

BOOL    GLOVE_GetRecord( int ID, matrix &angles, double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_GetRecord( int ID, matrix &angles, double &timestamp );
BOOL    GLOVE_GetRecord( int ID, matrix &angles );

BOOL    GLOVE_CnfgFlag( int ID, STRING text[GLOVE_FLAG_MAX], BOOL flag );

BOOL    GLOVE_Started( int ID );
BOOL    GLOVE_Streaming( int ID );
int     GLOVE_Sensors( int ID );
double  GLOVE_FrameRate( int ID );
BOOL    GLOVE_Stream( int ID, BOOL flag );

void    GLOVE_LoopTask( int ID );
void    GLOVE_LoopTask( void );

BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, BYTE data[], double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, BYTE data[], double &timestamp );
BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, BYTE data[] );

BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, BYTE data[], double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, BYTE data[], double &timestamp );
BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, BYTE data[] );

BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, matrix &angles, double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, matrix &angles, double &timestamp );
BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, matrix &angles );

BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, matrix &angles, double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, matrix &angles, double &timestamp );
BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, matrix &angles );

BOOL    GLOVE_CalibLoad( char *file, matrix &data );
BOOL    GLOVE_CalibLoad( int ID );
BOOL    GLOVE_CalibLoad( void );

void    GLOVE_Raw2Angles( BYTE data[], matrix &calib, matrix &angles );
void    GLOVE_MatrixDimension( int ID, matrix &mtx );

/******************************************************************************/

void    GLOVE_Close( void );

BOOL    GLOVE_Start( void );
void    GLOVE_Stop( void );

BOOL    GLOVE_Started( void );
BOOL    GLOVE_Streaming( void );
int     GLOVE_Sensors( void );
double  GLOVE_FrameRate( void );
void    GLOVE_MatrixDimension( matrix &mtx );

BOOL    GLOVE_GetRecord( BYTE data[], double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_GetRecord( BYTE data[], double &timestamp );
BOOL    GLOVE_GetRecord( BYTE data[] );

BOOL    GLOVE_GetRecord( matrix &angles, double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_GetRecord( matrix &angles, double &timestamp );
BOOL    GLOVE_GetRecord( matrix &angles );

BOOL    GLOVE_LastRecord( double &frameage, BOOL &fresh, BYTE data[], double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_LastRecord( double &frameage, BOOL &fresh, BYTE data[], double &timestamp );
BOOL    GLOVE_LastRecord( double &frameage, BOOL &fresh, BYTE data[] );

BOOL    GLOVE_LastRecord( BOOL &fresh, BYTE data[], double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_LastRecord( BOOL &fresh, BYTE data[], double &timestamp );
BOOL    GLOVE_LastRecord( BOOL &fresh, BYTE data[] );

BOOL    GLOVE_LastRecord( double &frameage, BOOL &fresh, matrix &angles, double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_LastRecord( double &frameage, BOOL &fresh, matrix &angles, double &timestamp );
BOOL    GLOVE_LastRecord( double &frameage, BOOL &fresh, matrix &angles );

BOOL    GLOVE_LastRecord( BOOL &fresh, matrix &angles, double &timestamp, BYTE &glovestatus );
BOOL    GLOVE_LastRecord( BOOL &fresh, matrix &angles, double &timestamp );
BOOL    GLOVE_LastRecord( BOOL &fresh, matrix &angles );

BOOL    GLOVE_LastRecord( BYTE data[] );

void    GLOVE_MatrixDimension( matrix &mtx );

BOOL   &GLOVE_Flag( int code );

BOOL    GLOVE_SetLight( BOOL flag );
BOOL    GLOVE_GetLight( BOOL &flag );

BOOL    GLOVE_SetSwitchLight( BOOL flag );
BOOL    GLOVE_GetSwitchLight( BOOL &flag );

BOOL    GLOVE_SetSwitchStatus( BOOL flag );
BOOL    GLOVE_GetSwitchStatus( BOOL &flag );

BOOL    GLOVE_SetGloveStatus( BOOL flag );
BOOL    GLOVE_GetGloveStatus( BOOL &flag );

/******************************************************************************/

