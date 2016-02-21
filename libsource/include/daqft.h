/******************************************************************************/
/*                                                                            */
/* MODULE  : DAQFT.h                                                          */
/*                                                                            */
/* PURPOSE : Analog (DAQ) F/T Sensor API.                                     */
/*                                                                            */
/* DATE    : 23/Nov/2004                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 24/Nov/2004 - Initial development.                               */
/*                                                                            */
/******************************************************************************/

#ifndef DAQFT_H
#define DAQFT_H
int     DAQFT_messgf( const char *mask, ... );
int     DAQFT_errorf( const char *mask, ... );
int     DAQFT_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    DAQFT_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    DAQFT_API_stop( void );
BOOL    DAQFT_API_check( void );

/******************************************************************************/

#define DAQFT_GAUGES  6
#define DAQFT_FORCES  3
#define DAQFT_TORQUES 3
#define DAQFT_FT      6

#define DAQFT_WINDOW  1

#define DAQFT_BIAS_POINTS  100
#define DAQFT_BIAS_TIMEOUT 1.0

/******************************************************************************/

class DAQFT
{
private:
    STRING ObjectName;

    BOOL OpenFlag;

    int DeviceType;
#define DAQFT_DEVICE_NIDAQ    0
#define DAQFT_DEVICE_SENSORAY 1

    int DeviceAddress;

    int Channels[DAQFT_GAUGES];

    matrix Calibration;
    matrix Scale;
    matrix Gauges;
    matrix FTdata;

    WINFIT *Window;
    int WindowPoints;

    int BiasPoints;
    int BiasIndex;
    BOOL BiasFlag;
    DATAPROC *BiasData[DAQFT_GAUGES];
    matrix Bias;
    STRING BiasFile;

    DATAPROC *GaugeData[DAQFT_GAUGES];
    int GaugePoints;

public:
    DAQFT( );
    DAQFT( char *name, int window, int bias, int gdata );
    DAQFT( char *name, int window, int bias );
    DAQFT( char *name );

   ~DAQFT( );

    void Init( void );
    void Init( char *name, int window, int bias, int gdata );

    BOOL Open( BOOL reset );
    BOOL Open( void );
    BOOL Opened( void );

    void Close( void );

    void ConfigSetup( void );
    BOOL ConfigLoad( char *file );

    int Device( int &addr, int chan[] );

    void Calculate( void );

    void BiasReset( void );

    BOOL BiasWait( void );

    void BiasPoint( matrix &g );
    void BiasPoint( double g[] );

    void BiasSet( matrix &g );
    void BiasSet( double g[] );

    BOOL BiasDone( void );
    BOOL BiasSave( void );

    void RawGauges( matrix &g );
    void RawGauges( double g[] );

    void Forces( matrix &f );
    void Torques( matrix &t );
    void FT( matrix &f, matrix &t );
    void FT( matrix &ft );

    void Convert( double g[], matrix *f, matrix *t );
    void Convert( matrix &g, matrix *f, matrix *t );

    DATAPROC **Data( void );
};

#define DAQFT_OPEN_CONFIGURATION 0
#define DAQFT_OPEN_CALIBRATION   1
#define DAQFT_OPEN_SCALE         2
#define DAQFT_OPEN_BIAS          3
#define DAQFT_OPEN_VARIABLES     4

#endif
/******************************************************************************/

