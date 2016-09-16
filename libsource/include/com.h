/******************************************************************************/
/*                                                                            */
/* MODULE  : COM.h                                                            */
/*                                                                            */
/* PURPOSE : RS232 Communications API.                                        */
/*                                                                            */
/* DATE    : 27/Dec/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 27/Dec/2000 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 10/Sep/2001 - Use integer handle rather than Win32 HANDLE.       */
/*                                                                            */
/******************************************************************************/

#define COM_DEBUG() if( !COM_API_start(printf,printf,printf) ) { printf("Cannot start COM API.\n"); exit(0); }

/******************************************************************************/

#define COM_MAX             16         // Maximum number of COM handles.
#define COM_INVALID         -1         // Invalid handle value.
#define COM_BUFF          4096         // I/O buffer size for RS232 communications.
#define COM_RdTO           500         // COM_Read(...) default timeout (mesc).
#define COM_RdIT      MAXDWORD         // Read Interval Timeout.
#define COM_RdTTM            0         // Read Total Timeout Multiplier.
#define COM_RdTTC            0         // Read Total Timeout Constant.

#define COM_BAUD_NULL        0L        // Null BAUD rate value.

/******************************************************************************/

struct  COM_HandleItem
{
    int       comX;                    // COM port number.
    HANDLE    comH;                    // Windows handle.
    TIMER    *timer;
};

/******************************************************************************/

int     COM_messgf( const char *mask, ... );
int     COM_errorf( const char *mask, ... );
int     COM_debugf( const char *mask, ... );

BOOL    COM_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    COM_API_stop( void );
BOOL    COM_API_check( void );

void    COM_Init( int handle );
int     COM_Find( HANDLE comH );
HANDLE  COM_Handle( int item );
char   *COM_Error( void );
int     COM_Open( int comX );
int     COM_Open( int comX, long baud );
int     COM_Open( int comX, long baud, void (*SetDCB)( DCB *dcb ) );
BOOL    COM_Close( HANDLE comH );
BOOL    COM_Close( int item );
void    COM_CloseAll( void );
BOOL    COM_Setup( int item, long baud, void (*SetDCB)( DCB *dcb) );
BOOL    COM_Write( int item, void *buff, int size );
BOOL    COM_Read( int item, void *buff, int size, int &got, BOOL &complete, double wait_msec );
BOOL    COM_Read( int item, void *buff, int size, BOOL &complete, double wait_msec );
BOOL    COM_Read( int item, void *buff, int size, double wait_msec );
BOOL    COM_Read( int item, void *buff, int size );
BOOL    COM_ModemStatus( int item, DWORD *stat );
BOOL    COM_ModemStatus( int item, BOOL *CTS, BOOL *DSR, BOOL *Ring, BOOL *RLSD );
BOOL    COM_Signal( int item, DWORD signal );
BOOL    COM_RTS( int item, BOOL flag );
BOOL    COM_DTR( int item, BOOL flag );
BOOL    COM_Reset( int item );

/******************************************************************************/

