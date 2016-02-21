/******************************************************************************/
/*                                                                            */
/* MODULE  : AtExit.h                                                         */
/*                                                                            */
/* PURPOSE : At Exit API functions.                                           */
/*                                                                            */
/* DATE    : 15/Feb/2001                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 15/Feb/2001 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define ATEXIT_DEBUG() if( !ATEXIT_API_start(printf,printf,printf) ) { printf("Cannot start ATEXIT API.\n"); exit(0); }

/******************************************************************************/

struct  ATEXIT_Item               // ATEXIT item.
{
    void (*func)( void );         // Function to perform.

    STRING name;                  // Name.
    STRING source;                // Source file name.
    int    line;                  // Source file line number.
    int    level;                 // Level.
};

/******************************************************************************/

#define ATEXIT_INVALID_HANDLE    -1

#define ATEXIT_LEVEL_HIGHEST      4
#define ATEXIT_LEVEL_LOWEST       0
#define ATEXIT_LEVEL_LAST        -1

/******************************************************************************/

int     ATEXIT_messgf( const char *mask, ... );
int     ATEXIT_errorf( const char *mask, ... );
int     ATEXIT_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    ATEXIT_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    ATEXIT_API_stop( void );
BOOL    ATEXIT_API_check( void );

void    ATEXIT_Init( void );
void    ATEXIT_Exit( void );
int     ATEXIT_Free( void );
BOOL    ATEXIT_Func( void (*func)( void ), char *name, char *srce, int line );
BOOL    ATEXIT_Func( void (*func)( void ), char *name, char *srce, int line, int level );
int     ATEXIT_MaxLevel( void );

BOOL WINAPI ATEXIT_ExitHandler( DWORD CtrlType );

/******************************************************************************/

#define atexit(F)       ATEXIT_Func(F,#F,__FILE__,__LINE__)

#define ATEXIT_API(F)   ATEXIT_Func(F,MODULE_NAME,__FILE__,__LINE__,MODULE_LEVEL)
#define ATEXIT_Appl(F)  ATEXIT_Func(F,MODULE_NAME,__FILE__,__LINE__,ATEXIT_LEVEL_HIGHEST)

/******************************************************************************/

extern BOOL ExitProgramWait;
void ExitProgram( int code );
void ExitProgram( void );

/******************************************************************************/

