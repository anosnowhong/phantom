/******************************************************************************/
/*                                                                            */
/* MODULE  : AtExit.cpp                                                       */
/*                                                                            */
/* PURPOSE : At Exit API functions.                                           */
/*                                                                            */
/* DATE    : 15/Feb/2001                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 15/Feb/2001 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 18/Mar/2002 - Added Control-C handle function.                   */
/*                                                                            */
/*                       - Added level number so that exit functions are      */
/*                         called in a sequence from highest to lowest.       */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "ATEXIT"
#define MODULE_TEXT     "At Exit API"
#define MODULE_DATE     "18/03/2002"
#define MODULE_VERSION  "1.1"
#define MODULE_LEVEL    -1

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

#undef  atexit                    // Don't wrap the wrapper...

/******************************************************************************/

#define ATEXIT_LIST     100
struct  ATEXIT_Item     ATEXIT_List[ATEXIT_LIST];

/******************************************************************************/

BOOL    ATEXIT_API_started=FALSE;

/******************************************************************************/

PRINTF  ATEXIT_PRN_messgf=NULL;                  // General messages printf function.
PRINTF  ATEXIT_PRN_errorf=NULL;                  // Error messages printf function.
PRINTF  ATEXIT_PRN_debugf=NULL;                  // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     ATEXIT_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ATEXIT_PRN_messgf,buff));
}

/******************************************************************************/

int     ATEXIT_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ATEXIT_PRN_errorf,buff));
}

/******************************************************************************/

int     ATEXIT_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ATEXIT_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    ATEXIT_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok=TRUE;
int     ID;

    ATEXIT_PRN_messgf = messgf;             // General API message print function.
    ATEXIT_PRN_errorf = errorf;             // API error message print function.
    ATEXIT_PRN_debugf = debugf;             // Debug information print function.

    if( ATEXIT_API_started )                // Start the API once...
    {
        return(TRUE);
    }

    ATEXIT_Init();                          // Initialize ATEXIT function list...

    // Set Control-C handle function... (V1.1)
    if( !SetConsoleCtrlHandler(ATEXIT_ExitHandler,TRUE) )
    {
        ATEXIT_errorf("ATEXIT_API_start(...) SetConsoleCtrlHandler(...) Failed.\n");
        ok = FALSE;
    }

    if( ok )
    {
        atexit(ATEXIT_API_stop);            // Install stop function.
        ATEXIT_API_started = TRUE;          // Set started flag.

        MODULE_start(ATEXIT_PRN_messgf);    // Register module.
    }
    else
    {
        ATEXIT_errorf("ATEXIT_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    ATEXIT_API_stop( void )
{
int     ID;

    if( !ATEXIT_API_started )          // API not started in the first place...
    {
         return;
    }

    ATEXIT_Exit();                     // Perform list of exit functions...

    ATEXIT_API_started = FALSE;        // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    ATEXIT_API_check( void )
{
BOOL    ok=TRUE;

    if( !ATEXIT_API_started )          // API not started...
    {                                  // Start module automatically...
        ok = ATEXIT_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        STR_printf(ok,ATEXIT_debugf,ATEXIT_errorf,"ATEXIT_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

void    ATEXIT_Init( void )
{
int     item;

    for( item=0; (item < ATEXIT_LIST); item++ )
    {
        memset(&ATEXIT_List[item],NUL,sizeof(struct ATEXIT_Item));
        ATEXIT_List[item].func = NULL;
    }
}

/******************************************************************************/

void    ATEXIT_Exit( void )
{
int     item,level;
BOOL    done;

    for( level=ATEXIT_LEVEL_HIGHEST,done=FALSE; (!done); level-- )
    {
        for( done=TRUE,item=0; (item < ATEXIT_LIST); item++ )
        {
            if( ATEXIT_List[item].func == NULL )
            {
                continue;
            }

            if( (ATEXIT_List[item].level != level) && (level >= ATEXIT_LEVEL_LOWEST) )
            {
                done = FALSE;
                continue;
            }

            ATEXIT_messgf("ATEXIT_Exit() %s[%02d] %1d(%1d) %s[%d]\n",ATEXIT_List[item].name,item,ATEXIT_List[item].level,ATEXIT_MaxLevel(),ATEXIT_List[item].source,ATEXIT_List[item].line);
// printf("ATEXIT_Exit() %s[%02d] %1d(%1d) %s[%d]\n",ATEXIT_List[item].name,item,ATEXIT_List[item].level,ATEXIT_MaxLevel(),ATEXIT_List[item].source,ATEXIT_List[item].line);
// getch();
            ATEXIT_List[item].func();
            ATEXIT_List[item].func = NULL;
        }
    }
}

/******************************************************************************/

int     ATEXIT_MaxLevel( void )
{
int     max,item;

    for( max=0,item=0; (item < ATEXIT_LIST); item++ )
    {
        if( ATEXIT_List[item].func == NULL )
        {
            continue;
        }

        if( ATEXIT_List[item].level > max )
        {
            max = ATEXIT_List[item].level;
        }
    }

    return(max);
}

/******************************************************************************/

int     ATEXIT_Find( void (*func)( void ) )
{
int     find,item;

    // Check if API started...
    if( !ATEXIT_API_check() )
    {
        return(ATEXIT_INVALID_HANDLE);
    }

    for( find=ATEXIT_INVALID_HANDLE,item=0; (item < ATEXIT_LIST); item++ )
    {
        if( ATEXIT_List[item].func == func )
        {
            find = item;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

int     ATEXIT_Free( void )
{
int     item;

    item = ATEXIT_Find(NULL);

    return(item);
}

/******************************************************************************/

BOOL    ATEXIT_Func( void (*func)( void ), char *name, char *source, int line )
{
BOOL    ok;

    ok = ATEXIT_Func(func,name,source,line,ATEXIT_LEVEL_HIGHEST);

    return(ok);
}

/******************************************************************************/

BOOL    ATEXIT_Func( void (*func)( void ), char *name, char *source, int line, int level )
{
int     item;

    // Check if API started...
    if( !ATEXIT_API_check() )
    {
        return(FALSE);
    }

    // Make sure we're not installing multiple instances of the same function...
    if( (item=ATEXIT_Find(func)) != ATEXIT_INVALID_HANDLE )
    {
        ATEXIT_debugf("ATEXIT_Func() [%02d] %s (already installed)\n",item,name);
        return(TRUE);
    }

    // Find a free handle...
    if( (item=ATEXIT_Free()) == ATEXIT_INVALID_HANDLE )
    {
        ATEXIT_errorf("ATEXIT: No free handles.\n");
        return(FALSE);
    }

    // Set parameters...
    ATEXIT_List[item].func = func;
    strncpy(ATEXIT_List[item].name,name,STRLEN);
    strncpy(ATEXIT_List[item].source,source,STRLEN);
    ATEXIT_List[item].line = line;
    ATEXIT_List[item].level = level;

    ATEXIT_debugf("ATEXIT_Func() [%02d] %s\n",item,name);

    return(TRUE);
}

/******************************************************************************/

BOOL WINAPI ATEXIT_ExitHandler( DWORD CtrlType )
{
BOOL    Exit=FALSE;
char   *Type="[???]";

    switch( CtrlType )
    {
        case CTRL_C_EVENT :
            Type = "Control-C";
            Exit = TRUE;
            break;

        case CTRL_BREAK_EVENT :
            Type = "Break";
            Exit = TRUE;
            break;

        case CTRL_CLOSE_EVENT :
            Type = "Close";
            Exit = TRUE;
            break;

        case CTRL_LOGOFF_EVENT :
            Type = "Logoff";
            Exit = TRUE;
            break;

        case CTRL_SHUTDOWN_EVENT :
            Type = "Shutdown";
            Exit = TRUE;
            break;
    }

    ATEXIT_messgf("ATEXIT_ExitHanlder() %s detected (Exit=%s).\n",Type,STR_YesNo(Exit));

    if( Exit )
    {
        ATEXIT_Exit();
        ATEXIT_messgf("ATEXIT_ExitHanlder() Done.\n");
    }

    return(FALSE);
}

/******************************************************************************/

BOOL ExitProgramWait = FALSE;

void ExitProgram( int code )
{
    if( ExitProgramWait )
    {
        printf("Press any key to exit program...\n");
        while( !KB_anykey() );
    }

    exit(code);
}

void ExitProgram( void )
{
    ExitProgram(0);
}

/******************************************************************************/

