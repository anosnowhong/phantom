/******************************************************************************/

BOOL    MOUSE_Open( char *config );
BOOL    MOUSE_Open( void );
void    MOUSE_Close( void );

BOOL    MOUSE_Start( void );
void    MOUSE_Stop( void );
BOOL    MOUSE_Started( void );

BOOL    MOUSE_GetPosn( matrix &pomx );

void    MOUSE_UpdatePosition( int x, int y );
void    MOUSE_UpdateButton( int button, int state, int x, int y );

void    MOUSE_ButtonIncrement( int &index );
void    MOUSE_ButtonEvent( int button );
void    MOUSE_ButtonClear( void );
BOOL    MOUSE_ButtonGet( int &button );

BOOL    MOUSE_ButtonDepressed( int button );
BOOL    MOUSE_ButtonDepressed( void );

#define MOUSE_BUTTONS 3
#define MOUSE_EVENTS  128

/******************************************************************************/

