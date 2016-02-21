/******************************************************************************/
/*                                                                            */ 
/* MODULE  : VERSION.h                                                        */ 
/*                                                                            */ 
/* PURPOSE : Version registry API for library modules.                        */ 
/*                                                                            */ 
/* DATE    : 16/May/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 16/May/2000 - Initial version.                                   */ 
/*                                                                            */ 
/* V1.1  JNI 23/Nov/2004 - Initial version.                                   */ 
/*                                                                            */ 
/******************************************************************************/

#define VERSION_LIST    40             // Maximum number of modules in registry.

/******************************************************************************/

struct  VERSION_RegistryItem           // Version registry item structure...
{
    BOOL   used;                       // Entry in use flag.

    STRING name;                       // Module name.
    STRING text;                       // Text description of module.
    STRING date;                       // Module date.
    STRING vers;                       // Module version.

    BOOL   started;                    // Module started flag.

    PRINTF prnf;                       // Module print function.
};

/******************************************************************************/

#define VERSION_ITEM_ERROR   -1

int     VERSION_messgf( const char *mask, ... );
int     VERSION_errorf( const char *mask, ... );
int     VERSION_debugf( const char *mask, ... );

int     VERSION_printf( short item, const char *mask, ... );

BOOL    VERSION_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    VERSION_API_stop( void );

BOOL    VERSION_API_check( void );

short   VERSION_find( char *name );

void    VERSION_start( char *name, char *text, char *date, char *version, PRINTF prnf );

void    VERSION_stop( char *name );
void    VERSION_stop( short item );

void    VERSION_list( PRINTF prnf );
short   VERSION_used( void );
short   VERSION_started( void );

/******************************************************************************/

#define MODULE_start(P)      VERSION_start(MODULE_NAME,MODULE_TEXT,MODULE_DATE,MODULE_VERSION,(P))
#define MODULE_stop()        VERSION_stop(MODULE_NAME)

#define MODULE_MESSG        0
#define MODULE_ERROR        1
#define MODULE_DEBUG        2
#define MODULE_PRINT        3

#define MODULE_messgf()      MODULE_printf(MODULE_NAME,MODULE_MESSG)
#define MODULE_errorf()      MODULE_printf(MODULE_NAME,MODULE_ERROR)
#define MODULE_debugf()      MODULE_printf(MODULE_NAME,MODULE_DEBUG)

PRINTF  MODULE_printf( char *module, int type );

int     MODULE_PrintError( const char *mask, ... );
int     MODULE_PrintMessg( const char *mask, ... );
int     MODULE_PrintDebug( const char *mask, ... );

void    MODULE_PrintLoad( void );
BOOL    MODULE_PrintFind( int type, char *module );

/******************************************************************************/

