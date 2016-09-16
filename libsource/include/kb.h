/******************************************************************************/
/*                                                                            */
/* MODULE  : kb.h                                                             */
/*                                                                            */
/* PURPOSE : Keyboard API.                                                    */
/*                                                                            */
/* DATE    : 29/Dec/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 29/Dec/2000 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 27/Jan/2002 - Added function to deliver keystrokes in command    */
/*                         shell window to the GLUT keyboard handler.         */
/*                                                                            */
/* V1.2  JNI 18/Jul/2002 - Processing for special (cursor & function) keys.   */
/*                                                                            */
/******************************************************************************/

#define KB_NOFLAGS 0x0000
#define KB_NOWAIT  0x0000
#define KB_WAIT    0x0001
#define KB_UPPER   0x0002

/******************************************************************************/

#define KB_IDLE_FUNC   10              // Maximum number of idle functions.
#define KB_IDLE_NULL   -1              // Idle function null handle.

void    KB_check( void );
void    KB_init( void );
int     KB_find( void (*func)( void ) );
void    KB_idle( void );

/******************************************************************************/
/* Application-level functions...                                             */
/******************************************************************************/

void    KB_idle(  int func, void (*fptr )( void ) );
#define KB_IDLE_START   0
#define KB_IDLE_STOP    1

BOOL    KB_hit( void );
BYTE    KB_get( int flag );
BOOL    KB_key( BYTE &code, int flag );
BOOL    KB_key( BYTE &code );
BOOL    KB_anykey( void );

BYTE    KB_wait( int flag );
BYTE    KB_wait( void );

BOOL    KB_ESC( int flag );
BOOL    KB_ESC( void );

/******************************************************************************/

#define KB_EXT_UP     0x48        // ASCII values returned for special keys...
#define KB_EXT_DOWN   0x50
#define KB_EXT_LEFT   0x4B
#define KB_EXT_RIGHT  0x4D
#define KB_EXT_HOME   0x47
#define KB_EXT_END    0x4F
#define KB_EXT_INSERT 0x52
#define KB_EXT_DELETE 0x53
#define KB_EXT_PGUP   0x49
#define KB_EXT_PGDN   0x51
#define KB_EXT_F11    0x44
#define KB_EXT_F12    0x44

#define KB_NUL_F1     0x3B        // ASCII values returned for special keys...
#define KB_NUL_F2     0x3C
#define KB_NUL_F3     0x3D
#define KB_NUL_F4     0x3E
#define KB_NUL_F5     0x3F
#define KB_NUL_F6     0x40
#define KB_NUL_F7     0x41
#define KB_NUL_F8     0x42
#define KB_NUL_F9     0x43
#define KB_NUL_F10    0x44

#define KB_EXT 0xE0               // ASCII values indicating special key follows...
#define KB_NUL 0x00

#define KB_SPECIAL 0x0E           // Special key value offset...

#define KB_UP      0+KB_SPECIAL   // Special keys (use these in your application)...
#define KB_DOWN    1+KB_SPECIAL
#define KB_LEFT    2+KB_SPECIAL
#define KB_RIGHT   3+KB_SPECIAL
#define KB_HOME    4+KB_SPECIAL
#define KB_END     5+KB_SPECIAL
#define KB_INSERT  6+KB_SPECIAL
#define KB_DELETE  7+KB_SPECIAL
#define KB_PGUP    8+KB_SPECIAL
#define KB_PGDN    9+KB_SPECIAL
#define KB_F1     10+KB_SPECIAL
#define KB_F2     11+KB_SPECIAL
#define KB_F3     12+KB_SPECIAL
#define KB_F4     13+KB_SPECIAL
#define KB_F5     14+KB_SPECIAL
#define KB_F6     15+KB_SPECIAL
#define KB_F7     16+KB_SPECIAL
#define KB_F8     17+KB_SPECIAL
#define KB_F9     18+KB_SPECIAL
#define KB_F10    19+KB_SPECIAL
#define KB_F11    20+KB_SPECIAL
#define KB_F12    21+KB_SPECIAL

BYTE    KB_xlat( BYTE xlat[][2], BYTE key );
BYTE    KB_special( BYTE code );

/******************************************************************************/

typedef void (*GLUTKEYBOARDFUNC)( unsigned char key, int x, int y );
typedef void (*GLUTSPECIALFUNC)( int key, int x, int y );

GLUTKEYBOARDFUNC KB_GLUT_KeyboardFuncInstall( GLUTKEYBOARDFUNC func );
GLUTSPECIALFUNC  KB_GLUT_SpecialFuncInstall( GLUTSPECIALFUNC func );

void    KB_GLUT_Events( void (*events)( void ) );
void    KB_GLUT_Events( void );

/******************************************************************************/

