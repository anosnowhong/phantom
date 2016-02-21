/******************************************************************************/

#define BUTTON_MAX 8

#define BUTTON_LAST_RESET 0xFF

/******************************************************************************/

void BUTTON_Open( int port, BYTE mask );
void BUTTON_Open( BYTE mask );
void BUTTON_Open( void );

void BUTTON_Reset( void );
void BUTTON_LoopTask( void );

BOOL BUTTON_Button( int &button );

void BUTTON_PutEvent( int button );
BOOL BUTTON_GetEvent( BYTE buttons[] );

BOOL BUTTON_Key( BYTE &code, BYTE xlat[] );
BOOL BUTTON_Key( BYTE &code );

BOOL BUTTON_KeyLeftRight( BYTE &code );
BOOL BUTTON_KeyNoYes( BYTE &code );
BOOL BUTTON_KeyQWER( BYTE &code );
BOOL BUTTON_KeyUIOP( BYTE &code );

BOOL BUTTON_ESC( void );

BOOL BUTTON_TaskKey( BYTE &code, BYTE xlat[] );
BOOL BUTTON_TaskKey( BYTE &code );
BOOL BUTTON_TaskGetEvent( BYTE buttons[] );
BOOL BUTTON_TaskButton( int &button );

/******************************************************************************/

