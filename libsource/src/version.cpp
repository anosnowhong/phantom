/******************************************************************************/
/*                                                                            */ 
/* MODULE  : VERSION.cpp                                                      */ 
/*                                                                            */ 
/* PURPOSE : Version registry API for library modules.                        */ 
/*                                                                            */ 
/* DATE    : 16/May/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 16/May/2000 - Initial version.                                   */ 
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     "VERSION"
#define MODULE_TEXT     "Version Registry API"
#define MODULE_DATE     "28/06/2000"
#define MODULE_VERSION  "1.0"

/******************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

struct  VERSION_RegistryItem      VERSION_Item[VERSION_LIST];
BOOL    VERSION_API_started=FALSE;

/******************************************************************************/

PRINTF  VERSION_PRN_messgf=NULL;                 // General messages printf function.
PRINTF  VERSION_PRN_errorf=NULL;                 // Error messages printf function.
PRINTF  VERSION_PRN_debugf=NULL;                 // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int    VERSION_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(VERSION_PRN_messgf,buff));
}

/******************************************************************************/

int     VERSION_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(VERSION_PRN_errorf,buff));
}

/******************************************************************************/

int     VERSION_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(VERSION_PRN_debugf,buff));
}

/******************************************************************************/

int     VERSION_printf( short item, const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];
int     plen=0;

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    if( VERSION_Item[item].prnf != NULL )
    {
        plen = (*VERSION_Item[item].prnf)(buff);
    }
    else
    {

        plen = VERSION_messgf(buff);
    }

    return(plen);
}

/******************************************************************************/

BOOL     VERSION_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
short    item;
BOOL     ok=FALSE;

#ifdef VERSION_HACK
    return(TRUE);
#endif

    if( VERSION_API_started )          // Start the API once...
    {
        return(TRUE);
    }

    VERSION_PRN_messgf = messgf;       // General API message print function.
    VERSION_PRN_errorf = errorf;       // API error message print function.
    VERSION_PRN_debugf = debugf;       // Debug information print function.

    for( item=0; (item < VERSION_LIST); item++ )
    {
        memset(&VERSION_Item[item],0,sizeof(struct VERSION_RegistryItem));
        VERSION_Item[item].used = FALSE;
    }

    ok = TRUE;

    if( ok )
    {
//      ATEXIT_API(VERSION_API_stop);       // Install stop function.
        VERSION_API_started = TRUE;         // Set started flag.

        MODULE_start(VERSION_PRN_messgf);   // Register module.
    }
    else
    {
        VERSION_errorf("VERSION_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    VERSION_API_stop( void )
{
short   item;

#ifdef VERSION_HACK
    return;
#endif

    if( !VERSION_API_started )         // API not started in the first place...
    {
        return;
    }

    MODULE_stop();
    VERSION_API_started = FALSE;
}

/******************************************************************************/

BOOL    VERSION_API_check( void )
{
BOOL    ok=TRUE;

#ifdef VERSION_HACK
    return(TRUE);
#endif

    if( !VERSION_API_started )         // API not started...
    {                                  // Start module automatically...
        ok = VERSION_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        VERSION_debugf("VERSION_API_check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/******************************************************************************/

short   VERSION_find( char *name )
{
short   item,find=VERSION_ITEM_ERROR;

    for( item=0; (item < VERSION_LIST); item++ )
    {
        if( name != NULL )        // Find specific module name...
        {
            if( strcmp(VERSION_Item[item].name,name) == 0 )
            {
                find = item;
                break;
             }
        }
        else                      // Find a free slot...
        {
            if( !VERSION_Item[item].used )
            {
                find = item;
                break;
            }
        }
    }

    return(find);
}

/******************************************************************************/

void    VERSION_start( char *name, char *text, char *date, char *vers, PRINTF prnf )
{
short   item;

#ifdef VERSION_HACK
    return;
#endif

    if( !VERSION_API_check() )              // Make sure API is started...
    {
        return;
    }

    if( (item=VERSION_find(name)) == VERSION_ITEM_ERROR )
    {                                  // Module not already in list...
        if( (item=VERSION_find(NULL)) != VERSION_ITEM_ERROR )
        {                              // Add module to registry list...
            VERSION_Item[item].used = TRUE;
            VERSION_Item[item].prnf = prnf;

            strncpy(VERSION_Item[item].name,name,STRLEN);
            strncpy(VERSION_Item[item].text,text,STRLEN);
            strncpy(VERSION_Item[item].date,date,STRLEN);
            strncpy(VERSION_Item[item].vers,vers,STRLEN);
        }
    }

    if( item == VERSION_ITEM_ERROR )         // List full or something not right...
    {
        return;
    }

//  Print module start message...
    VERSION_printf(item,"%s V%s Started.\n",VERSION_Item[item].text,VERSION_Item[item].vers);

    VERSION_Item[item].started = TRUE;      // Module is running... 
}

/******************************************************************************/

void    VERSION_stop( short item )
{
#ifdef VERSION_HACK
    return;
#endif

    if( !VERSION_API_check() )              // Make sure API is started...
    {
        return;
    }

    if( !VERSION_Item[item].used )          // Module registry item not used...
    {
        return;
    }

    if( !VERSION_Item[item].started )       // Module already stopped...
    {
        return;
    }

//  Print module stop messsage...
    VERSION_printf(item,"%s V%s Stopped.\n",VERSION_Item[item].text,VERSION_Item[item].vers);

    VERSION_Item[item].started = FALSE;     // Module is not running... 
}

/******************************************************************************/

void    VERSION_stop( char *name )
{
short   item;

#ifdef VERSION_HACK
    return;
#endif

    if( !VERSION_API_check() )              // Make sure API is started...
    {
        return;
    }

    if( (item=VERSION_find(name)) >= VERSION_LIST )
    {                                       // Module not in list...
        return;
    }

    VERSION_stop(item);                     // Stop module... 
}

/******************************************************************************/

void    VERSION_list( PRINTF prnf )
{
short   item;

    if( !VERSION_API_check() )              // Make sure API is started...
    {
        return;
    }

    for( item=0; (item < VERSION_LIST); item++ )
    {
        if( !VERSION_Item[item].used )
        {
            continue;
        }

      (*prnf)("%s Vs %s %s %s\n",
              VERSION_Item[item].name,
              VERSION_Item[item].vers,
              VERSION_Item[item].date,
              VERSION_Item[item].text,
              VERSION_Item[item].started ? "Started" : "Stopped");
    }
}

/******************************************************************************/

short   VERSION_used( void )
{
short   item,used;

    if( !VERSION_API_check() )              // Make sure API is started...
    {
        return(VERSION_ITEM_ERROR);
    }

    for( used=0,item=0; (item < VERSION_LIST); item++ )
    {
        if( VERSION_Item[item].used )
        {
            used++;
        }
    }

    used++;

    return(used);
}

/******************************************************************************/

short   VERSION_started( void )
{
short   item,started;

    if( !VERSION_API_check() )              // Make sure API is started...
    {
        return(VERSION_ITEM_ERROR);
    }

    for( started=0,item=0; (item < VERSION_LIST); item++ )
    {
        if( VERSION_Item[item].used && VERSION_Item[item].started )
        {
            started++;
        }
    }

    return(started);
}

/******************************************************************************/

STRING  MODULE_PrintName[MODULE_PRINT] = { "ModuleMessg","ModuleError","ModuleDebug" };
STRING  MODULE_PrintList[MODULE_PRINT] = { "","","" };
BOOL    MODULE_PrintLoaded=FALSE;

/******************************************************************************/

void MODULE_PrintLoad( void )
{
int type;
DWORD size;

    for( type=MODULE_MESSG; (type <= MODULE_DEBUG); type++ )
    {
        size = GetEnvironmentVariable(MODULE_PrintName[type],MODULE_PrintList[type],STRLEN);
    }
}

/******************************************************************************/

BOOL MODULE_PrintFind( int type, char *module )
{
BOOL flag=FALSE;

    if( !MODULE_PrintLoaded )
    {
        MODULE_PrintLoad();
        MODULE_PrintLoaded = TRUE;
    }

    if( strstr(MODULE_PrintList[type],module) != NULL )
    {
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

int     MODULE_PrintError( const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];
int     plen=0;
PRINTF  pfunc=printf;

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    plen = (*pfunc)(buff);
    
    return(plen);
}

/******************************************************************************/

int     MODULE_PrintMessg( const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];
int     plen=0;
PRINTF  pfunc=printf;

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    plen = (*pfunc)(buff);
    
    return(plen);
}
/******************************************************************************/

int     MODULE_PrintDebug( const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];
int     plen=0;
PRINTF  pfunc=printf;

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    plen = (*pfunc)(buff);
    
    return(plen);
}

/******************************************************************************/

PRINTF MODULE_printf( char *module, int type )
{
PRINTF pfunc=NULL;
BOOL flag;

    flag = MODULE_PrintFind(type,module);

    switch( type )
    {
        case MODULE_MESSG :
           if( flag )
           {
               pfunc = MODULE_PrintMessg;
           }
           break;

        case MODULE_ERROR :
           pfunc = MODULE_PrintError;
           break;

        case MODULE_DEBUG :
           if( flag )
           {
               pfunc = MODULE_PrintDebug;
           }
           break;
    }

    return(pfunc);
}

/******************************************************************************/

BOOL MOTOR_Connect( int argc, char *argv )
{
BOOL ok=TRUE;

    return(ok);
}

/******************************************************************************/

void MOTOR_Disconnect( void )
{
}

/******************************************************************************/

