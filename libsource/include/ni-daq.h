/******************************************************************************/
/*                                                                            */
/* MODULE  : NI-DAQ.h                                                         */
/*                                                                            */
/* PURPOSE : National Instruments - Data AQuisition API                       */
/*                                                                            */
/* DATE    : 18/Sep/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 23/Aug/2001 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#ifndef NIDAQ_H
#define NIDAQ_H

/******************************************************************************/

#define NIDAQ_DEBUG() if( !NIDAQ_API_start(printf,printf,printf) ) { printf("Cannot start NIDAQ API.\n"); exit(0); }

/******************************************************************************/

#define NIDAQ_CHANNEL_MAX       256
#define NIDAQ_CHANNEL_ALL        -1
#define NIDAQ_CHANNEL_END        -2
#define NIDAQ_CHANNEL_NULL       -2

#define NIDAQ_RATE_FREQUENCY      0
#define NIDAQ_RATE_INTERVAL       1

#define NIDAQ_DEVICE_MAX          4

#define NIDAQ_HW_INVALID         -1
#define NIDAQ_HW_WHATEVER         0
#define NIDAQ_HW_DAQPAD6020EUSB  76
#define NIDAQ_HW_PCI6035E       316

#define NIDAQ_MONITOR_CURRENT     0
#define NIDAQ_MONITOR_ALL         1

#define NIDAQ_INPUTMODE_DIFF      0
#define NIDAQ_INPUTMODE_RSE       1
#define NIDAQ_INPUTMODE_NRSE      2

#define NIDAQ_POLARITY_BI         0
#define NIDAQ_POLARITY_UNI        1

#define NIDAQ_DIGITAL_INPUT       0
#define NIDAQ_DIGITAL_OUTPUT      1

/******************************************************************************/

struct  NIDAQ_HW_DeviceInformation
{
    STRING    description;
    int       device;
    BOOL      supported;
};

/******************************************************************************/

struct  NIDAQ_AI_ChannelInformation
{
    int channel;
    double gain;
    double offset;
};

/******************************************************************************/

struct  NIDAQ_AI_SessionInformation
{
    BOOL      running;
    BOOL      complete;
    int       frames;
    double    sr;
    double    duration;
    BOOL      buffered;
    void     *buffer;
    int       channels;
    struct    NIDAQ_AI_ChannelInformation channel[NIDAQ_CHANNEL_MAX];
};

/******************************************************************************/

extern  int  NIDAQ_Device;

extern  struct  STR_TextItem  NIDAQ_PolarityText[];
extern  struct  STR_TextItem  NIDAQ_InputModeText[];

/******************************************************************************/

int     NIDAQ_messgf( const char *mask, ... );
int     NIDAQ_errorf( const char *mask, ... );
int     NIDAQ_debugf( const char *mask, ... );

/******************************************************************************/

char   *NIDAQ_DeviceDescription( int code );

/******************************************************************************/

BOOL    NIDAQ_Error( i16 status, char *func );

BOOL    NIDAQ_HW_Supported( int code );
BOOL    NIDAQ_Devices( void );
int     NIDAQ_SetDevice( char *description );
int     NIDAQ_SetDevice( int code );
int     NIDAQ_SetDevice( void );
int     NIDAQ_DeviceCode( int device );
char   *NIDAQ_DeviceName( int device );

BOOL    NIDAQ_Installed( int &device );
BOOL    NIDAQ_Installed( void );

/******************************************************************************/

BOOL    NIDAQ_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    NIDAQ_API_stop( void );
BOOL    NIDAQ_API_check( void );

/******************************************************************************/

BOOL    NIDAQ_Check( int dev );

/******************************************************************************/
/* Analog Input Functions...                                                  */
/******************************************************************************/

void   *NIDAQ_AI_BufferAllocate( int channels, int frames );
void    NIDAQ_AI_BufferFree( void **ptr );

BOOL    NIDAQ_AI_Setup( int dev, int chan, int mode, int polar );
BOOL    NIDAQ_AI_Setup( int dev, int chan[], int mode[], int polar[] );
BOOL    NIDAQ_AI_Setup( int chan[], int mode[], int polar[] );

BOOL    NIDAQ_AI_SetupAll( int dev, int mode, int polar );
BOOL    NIDAQ_AI_SetupAll( int mode, int polar );

BOOL    NIDAQ_AI_Start( int dev, int chan[], double gain[], int frames, double sr, void *ptr );

BOOL    NIDAQ_AI_Start( int dev, int chan[], double gain[], double duration, double sr );
BOOL    NIDAQ_AI_Start( int dev, int chan, double gain, double duration, double sr );
BOOL    NIDAQ_AI_Start( int chan[], double gain[], double duration, double sr );
BOOL    NIDAQ_AI_Start( int chan, double gain, double duration, double sr );

BOOL    NIDAQ_AI_Start( int dev, int chan[], double duration, double sr );
BOOL    NIDAQ_AI_Start( int dev, int chan, double duration, double sr );
BOOL    NIDAQ_AI_Start( int chan[], double duration, double sr );
BOOL    NIDAQ_AI_Start( int chan, double duration, double sr );

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan[], double gain[], double duration, double sr, int dio, double post_trigger );
BOOL    NIDAQ_AI_TriggerStart( int chan[], double gain[], double duration, double sr, int dio, double post_trigger );
BOOL    NIDAQ_AI_TriggerStart( int dev, int chan, double gain, double duration, double sr, int dio, double post_trigger );
BOOL    NIDAQ_AI_TriggerStart( int chan, double gain, double duration, double sr, int dio, double post_trigger );

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan[], double gain[], double duration, double sr, double post_trigger );
BOOL    NIDAQ_AI_TriggerStart( int chan[], double gain[], double duration, double sr, double post_trigger );
BOOL    NIDAQ_AI_TriggerStart( int dev, int chan, double gain, double duration, double sr, double post_trigger );
BOOL    NIDAQ_AI_TriggerStart( int chan, double gain, double duration, double sr, double post_trigger );

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan[], double gain[], double duration, double sr, int dio );
BOOL    NIDAQ_AI_TriggerStart( int chan[], double gain[], double duration, double sr, int dio );
BOOL    NIDAQ_AI_TriggerStart( int dev, int chan, double gain, double duration, double sr, int dio );
BOOL    NIDAQ_AI_TriggerStart( int chan, double gain, double duration, double sr, int dio );

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan[], double gain[], double duration, double sr );
BOOL    NIDAQ_AI_TriggerStart( int chan[], double gain[], double duration, double sr );
BOOL    NIDAQ_AI_TriggerStart( int dev, int chan, double gain, double duration, double sr );
BOOL    NIDAQ_AI_TriggerStart( int chan, double gain, double duration, double sr );

BOOL    NIDAQ_AI_TriggerSetup( int dev, int dio );

void    NIDAQ_AI_Trigger( int dev );
void    NIDAQ_AI_Trigger( void );

BOOL    NIDAQ_AI_Progress( int dev, int &frames, BOOL &complete );
BOOL    NIDAQ_AI_Progress( int &frames, BOOL &complete );

BOOL    NIDAQ_AI_IsComplete( int dev, int &frames );
BOOL    NIDAQ_AI_IsComplete( int &frames );

BOOL    NIDAQ_AI_WaitComplete( int dev, int &frames, double msec );
BOOL    NIDAQ_AI_WaitComplete( int dev, int &frames );
BOOL    NIDAQ_AI_WaitComplete( int &frames, double msec );
BOOL    NIDAQ_AI_WaitComplete( int &frames );

BOOL    NIDAQ_AI_Monitor( int dev, double voltages[], int &frames, BOOL &ready, BOOL &complete );
BOOL    NIDAQ_AI_Monitor( int dev, double voltages[], BOOL &ready, BOOL &complete );
BOOL    NIDAQ_AI_Monitor( int dev, double voltages[] );
BOOL    NIDAQ_AI_Monitor( double voltages[], int &frames, BOOL &ready, BOOL &complete );
BOOL    NIDAQ_AI_Monitor( double voltages[], BOOL &ready, BOOL &complete );
BOOL    NIDAQ_AI_Monitor( double voltages[] );

BOOL    NIDAQ_AI_Abort( int dev, BOOL release );
BOOL    NIDAQ_AI_Abort( int dev );
BOOL    NIDAQ_AI_Abort( void );

BOOL    NIDAQ_AI_StopNow( int dev );
BOOL    NIDAQ_AI_StopNow( void );

BOOL    NIDAQ_AI_Voltage( int dev, int channel, i16 ADC, double &voltage );

BOOL    NIDAQ_AI_Voltages( int dev, matrix &voltages, BOOL release );
BOOL    NIDAQ_AI_Voltages( matrix &voltages, BOOL release );
BOOL    NIDAQ_AI_Voltages( int dev, matrix &voltages );
BOOL    NIDAQ_AI_Voltages( matrix &voltages );

BOOL    NIDAQ_AI_NB_Start( int dev, int chan[], double gain[], double sr );
BOOL    NIDAQ_AI_NB_Start( int dev, int chan[], double sr );
BOOL    NIDAQ_AI_NB_Start( int chan[], double gain[], double sr );
BOOL    NIDAQ_AI_NB_Start( int chan[], double sr );

BOOL    NIDAQ_AI_NB_Read( int dev, double voltages[] );
BOOL    NIDAQ_AI_NB_Read( double voltages[] );
BOOL    NIDAQ_AI_NB_Read( int dev, int values[] );
BOOL    NIDAQ_AI_NB_Read( int values[] );

BOOL    NIDAQ_AI_NB_Read( int dev, int chan, double gain, double &voltage );
BOOL    NIDAQ_AI_NB_Read( int chan, double gain, double &voltage );
BOOL    NIDAQ_AI_NB_Read( int dev, int chan, double gain, int &value );
BOOL    NIDAQ_AI_NB_Read( int chan, double gain, int &value );

/******************************************************************************/
/* Digital I/O Functions...                                                   */
/******************************************************************************/

BOOL    NIDAQ_DIG_Setup( int dev, int chan, int mode );
BOOL    NIDAQ_DIG_Setup( int chan, int mode );
BOOL    NIDAQ_DIG_Setup( int dev, int chan[], int mode[] );
BOOL    NIDAQ_DIG_Setup( int chan[], int mode[] );
BOOL    NIDAQ_DIG_Setup( int dev, int chan[], int mode );
BOOL    NIDAQ_DIG_Setup( int chan[], int mode );

BOOL    NIDAQ_DIG_ForOutput( int dev, int chan[] );
BOOL    NIDAQ_DIG_ForOutput( int dev, int chan );
BOOL    NIDAQ_DIG_ForOutput( int chan[] );
BOOL    NIDAQ_DIG_ForOutput( int chan );

BOOL    NIDAQ_DIG_Output( int dev, int chan, BOOL value );
BOOL    NIDAQ_DIG_Output( int chan, BOOL value );
BOOL    NIDAQ_DIG_Output( int dev, int chan[], BOOL value[] );
BOOL    NIDAQ_DIG_Output( int chan[], BOOL value[] );

BOOL    NIDAQ_DIG_SetOutput( int dev, int chan[], BOOL value[] );
BOOL    NIDAQ_DIG_SetOutput( int dev, int chan, BOOL value );
BOOL    NIDAQ_DIG_SetOutput( int chan[], BOOL value[] );
BOOL    NIDAQ_DIG_SetOutput( int chan, BOOL value );

BOOL    NIDAQ_DIG_ForInput( int dev, int chan[] );
BOOL    NIDAQ_DIG_ForInput( int dev, int chan );
BOOL    NIDAQ_DIG_ForInput( int chan[] );
BOOL    NIDAQ_DIG_ForInput( int chan );

BOOL    NIDAQ_DIG_Input( int dev, int chan, BOOL &value );
BOOL    NIDAQ_DIG_Input( int chan, BOOL &value );
BOOL    NIDAQ_DIG_Input( int dev, int chan[], BOOL value[] );
BOOL    NIDAQ_DIG_Input( int chan[], BOOL value[] );

BOOL    NIDAQ_DIG_GetInput( int dev, int chan, BOOL &value );
BOOL    NIDAQ_DIG_GetInput( int chan, BOOL &value );
BOOL    NIDAQ_DIG_GetInput( int dev, int chan[], BOOL value[] );
BOOL    NIDAQ_DIG_GetInput( int chan[], BOOL value[] );

/******************************************************************************/

#endif

/******************************************************************************/

