/******************************************************************************/

#define SOCKET_errorf printf
#define SOCKET_debugf printf
#define SOCKET_messgf printf

#define SOCKET_PACKET         256
#define SOCKET_ENCAPSULATION  6
#define SOCKET_DATA           (SOCKET_PACKET-SOCKET_ENCAPSULATION)

#define SOCKET_PORT_DEFAULT   5113

#define SOCKET_SIZE_TIMERS    10
#define SOCKET_SIZE_STEP      5

/******************************************************************************/

BOOL SOCKET_Start( void );
void SOCKET_Stop( void );

BOOL SOCKET_OpenServer( char *IP, int port );
BOOL SOCKET_OpenServer( char *IP );

BOOL SOCKET_OpenClient( int port );
BOOL SOCKET_OpenClient( void );

BOOL SOCKET_ClientWait( void );
void SOCKET_ClientClose( void );

void SOCKET_Close( void );
void SOCKET_SocketClose( SOCKET *handle );

int  SOCKET_Status( void );

BOOL SOCKET_Send( BYTE *data, int size );
BOOL SOCKET_Recv( BOOL &ready, BYTE *data, int &size );

/******************************************************************************/

