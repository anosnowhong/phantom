/******************************************************************************/
/*                                                                            */
/* MODULE  : EYET.h                                                           */
/*                                                                            */
/* PURPOSE : EyeLink-1000 high-level API.                                     */
/*                                                                            */
/* DATE    : 18/Jun/2011                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 18/Jun/2011 - Initial development.                               */
/*                                                                            */
/******************************************************************************/

#define EYET_errorf printf
#define EYET_messgf printf
#define EYET_debugf printf

void EYET_ConfigSetup( void );
BOOL EYET_ConfigLoad( char *file );
BOOL EYET_Open( char *cnfg, void (*grphtext)( char *text ), void (*beepgo)( void ) );
BOOL EYET_Open( char *cnfg, void (*grphtext)( char *text ) );
void EYET_Close( void );
double EYET_GetSampleRate( void );
void EYET_GraphicsText( char *text );
void EYET_BeepGo( void );
void EYET_KeyEvent( unsigned char key );
void EYET_KeyReset( void );
BOOL EYET_KeyPress( void );
BOOL EYET_KeyPress( unsigned char &key );
BOOL EYET_CalibrateStart( BOOL testflag );
void EYET_CalibrateDone( void );
BOOL EYET_TestStart( void );
void EYET_Abort( void );
BOOL EYET_StateIdle( void );
void EYET_StateNext( int state );
void EYET_StateProcess( void );
BOOL EYET_NoData( double EyeXY[] );
BOOL EYET_FrameNext( double &TimeStamp, double EyeXY[], double &PupilSize, BOOL &NoDataFlag, double &FixateSeconds, BOOL &ReadyFlag );
BOOL EYET_FrameNext( double &TimeStamp, double EyeXY[], BOOL &NoDataFlag, double &FixateSeconds, BOOL &ReadyFlag );
BOOL EYET_FrameNext( double &TimeStamp, double EyeXY[], double &PupilSize, BOOL &ReadyFlag );
BOOL EYET_FrameNext( double &TimeStamp, double EyeXY[], BOOL &ReadyFlag );
void EYET_LoopTask( void );
void EYET_GraphicsDisplay( void );

/******************************************************************************/
/* EYECOM - EyeLink1000 RS232 streaming functions.                           */
/******************************************************************************/

#define EYECOM_errorf printf
#define EYECOM_messgf printf
#define EYECOM_debugf printf

BOOL EYECOM_FrameNext( double &TimeState, double EyeXY[], BOOL &ready );
void EYECOM_PortDCB( DCB *dcb );
BOOL EYECOM_Open( int port );
void EYECOM_Close( void );
void EYECOM_ProcessByte( BOOL dataflag, BYTE databyte );
void EYECOM_LoopTask( void );

/******************************************************************************/

