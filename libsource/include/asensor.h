/******************************************************************************/
/*                                                                            */
/* MODULE  : ASENSOR.h                                                        */
/*                                                                            */
/* PURPOSE : Analogue Sensor API.                                             */
/*                                                                            */
/* DATE    : 23/Nov/2004                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 24/Nov/2004 - Initial development (for ATI DAQ F/T sensors).     */
/*                                                                            */
/* V2.0  JNI 26/Mar/2008 - Renamed & generalized to "ASENSOR" (from "DAQFT"). */
/*                                                                            */
/* V2.1  JNI 08/Dec/2008 - Added PhotoTransistor sensor type.                 */
/*                                                                            */
/******************************************************************************/

int     ASENSOR_messgf( const char *mask, ... );
int     ASENSOR_errorf( const char *mask, ... );
int     ASENSOR_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    ASENSOR_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    ASENSOR_API_stop( void );
BOOL    ASENSOR_API_check( void );

/******************************************************************************/

extern struct STR_TextItem ASENSOR_DeviceText[];
extern struct STR_TextItem ASENSOR_DAQ_Text[];

#define ASENSOR_CHANNELS    12

#define ASENSOR_BIAS_POINTS  100
#define ASENSOR_BIAS_TIMEOUT 1.0

/******************************************************************************/

class ASENSOR
{
private:
    STRING ObjectName;

    BOOL OpenFlag;

    int DeviceType;
#define ASENSOR_DEVICE_DAQFT             0
#define ASENSOR_DEVICE_ACCELEROMETER_XY  1
#define ASENSOR_DEVICE_PHOTOTRANSISTOR   2

    int DAQ_Type;
#define ASENSOR_DAQ_NIDAQ    0
#define ASENSOR_DAQ_SENSORAY 1

    int DAQ_Address;

    int DAQ_ChannelCount;
    int DAQ_Channels[ASENSOR_CHANNELS];

#define ASENSOR_CALIBRATION_FLAGS 3
#define ASENSOR_CALIBRATION_MTX   0
#define ASENSOR_CALIBRATION_GAIN  1
#define ASENSOR_CALIBRATION_BIAS  2
    BOOL CalibrationFlags[ASENSOR_CALIBRATION_FLAGS];

    matrix Calibration;
    matrix Gain;
    matrix SensorVolts;
    matrix SensorValues;

    WINFIT *Window;
    int WindowSize;

    int BiasPoints;
    int BiasCurrent;

    int BiasState;
#define ASENSOR_BIAS_CLEAR   0
#define ASENSOR_BIAS_PENDING 1
#define ASENSOR_BIAS_DONE    2

    DATAPROC *BiasData[ASENSOR_CHANNELS];
    matrix BiasVolts;
    STRING BiasFile;
    STRING BiasHistoryFile;

    DATAPROC *VoltData[ASENSOR_CHANNELS];
    int VoltPoints;

public:
    ASENSOR( );
    ASENSOR( char *name, int window, int bias, int vdata );
    ASENSOR( char *name, int window, int bias );
    ASENSOR( char *name );

   ~ASENSOR( );

    void Init( void );
    void Init( char *name, int window, int bias, int vdata );

    BOOL Open( BOOL reset );
    BOOL Open( void );
    BOOL Opened( void );

    void Close( void );

    void ConfigSetup( void );
    BOOL ConfigLoad( char *file );

    int Device( int &DAQ, int &addr, int &chancount, int chanlist[] );

    void Calculate( void );

    void VoltsDoubleToColumnVector( double v[], matrix &V );
    void MakeRowVector( matrix &M );
    void MakeColumnVector( matrix &M );

    void BiasReset( int type );
#define ASENSOR_BIAS_ANTEROGRADE 0
#define ASENSOR_BIAS_RETROGRADE  1

    void BiasReset( void );
    void BiasResetAnterograde( void );
    void BiasResetRetrograde( void );

    BOOL BiasWait( void );

    void BiasSample( matrix &volts );
    void BiasSample( double volts[] );

    void BiasSet( void );
    void BiasSet( matrix &volts );
    void BiasSet( double volts[] );

    BOOL BiasDone( void );
    BOOL BiasSave( void );

    void SampleVolts( matrix &volts );
    void SampleVolts( double volts[] );

    void ConvertValues( matrix &volts, matrix &values );
    void ConvertValues( double volts[], matrix &values );
    double ConvertValue( double volts );

    void CurrentValues( matrix &values );
    double CurrentValue( void );

    BOOL VoltDataInUse( void );
    BOOL VoltDataSave( void );
};

/******************************************************************************/

#define ASENSOR_OPEN_CONFIGURATION 0
#define ASENSOR_OPEN_CALIBRATION   1
#define ASENSOR_OPEN_GAIN          2
#define ASENSOR_OPEN_BIAS          3
#define ASENSOR_OPEN_VARIABLES     4

/******************************************************************************/

void ASENSOR_DAQFT_FT( ASENSOR *asensor, double volts[], matrix &f, matrix &t );
void ASENSOR_DAQFT_FT( ASENSOR *asensor, matrix &f, matrix &t );
void ASENSOR_DAQFT_Forces( ASENSOR *asensor, double volts[], matrix &f );
void ASENSOR_DAQFT_Forces( ASENSOR *asensor, matrix &f );

/******************************************************************************/

