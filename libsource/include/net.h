/******************************************************************************/

#define SOCKET_errorf printf
#define SOCKET_debugf printf
#define SOCKET_messgf printf

#define SOCKET_PACKET         256
#define SOCKET_ENCAPSULATION  6
#define SOCKET_DATA           (SOCKET_PACKET-SOCKET_ENCAPSULATION)

/******************************************************************************/

BOOL SOCKET_Open( char *IP, int port );
BOOL SOCKET_Open( int port );

void SOCKET_Close( void );

int SOCKET_Status( void );

BOOL SOCKET_Send( BYTE *data, int size );

BOOL SOCKET_Recv( BOOL &ready, BOOL &process, BYTE *data, int &size );
BOOL SOCKET_Recv( BOOL &ready, BYTE *data, int &size );

/******************************************************************************/

