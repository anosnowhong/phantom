/******************************************************************************/
/*                                                                            */
/* MODULE  : CMDARG.cpp                                                       */
/*                                                                            */
/* PURPOSE : Command-Line Argument processing functions.                      */
/*                                                                            */
/* DATE    : 02/Dec/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 02/Dec/2000 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "CMDARG"
#define MODULE_TEXT     "Command-Line Argument API"
#define MODULE_DATE     "02/12/2000"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL    1

/******************************************************************************/

#include <motor.h>                               // Includes (almost) everything we need.

/******************************************************************************/

char    CMDARG_code( char *argp, char **data )
{
char    code;

    code = toupper(argp[1]);
   *data = NULL;

    if( (argp[2] == ':') && (STR_strip(&argp[3]) > 0) )
    {
       *data = &argp[3];
    }

    return(code);
}

/******************************************************************************/

BOOL    CMDARG_data( char *dest, char *data, int max )
{
BOOL    ok=TRUE;
int     len;

    if( data == NULL )
    {
        ok = FALSE;
        return(ok);
    }

    len = strlen(data);

    if( (len == 0) || (len > max) )
    {
        ok = FALSE;
    }
    else
    {
        strncpy(dest,data,max);
    }

    return(ok);
}

/******************************************************************************/

BOOL    CMDARG_data( STRING dest, char *data )
{
BOOL    ok=TRUE;
int     len,max=STRLEN;

    if( data == NULL )
    {
        ok = FALSE;
        return(ok);
    }

    len = strlen(data);

    if( (len == 0) || (len > max) )
    {
        ok = FALSE;
    }
    else
    {
        strncpy(dest,data,max);
    }

    return(ok);
}

/******************************************************************************/

BOOL    CMDARG_data( char *dest[], char *data, int max, int n )
{
char  **list;
int     tokens,i;
BOOL    ok;

    list = STR_tokens(data,tokens);

    for( ok=TRUE,i=0; ((i < tokens) && (i < n) && ok); i++ )
    {
        ok = CMDARG_data(dest[i],list[i],max);
    }

    return(ok);
}

/******************************************************************************/

BOOL    CMDARG_data( STRING dest[], char *data, int n )
{
char  **list;
int     tokens,i;
BOOL    ok;

    list = STR_tokens(data,tokens);

    for( ok=TRUE,i=0; ((i < tokens) && (i < n) && ok); i++ )
    {
        ok = CMDARG_data(dest[i],list[i]);
    }

    return(ok);
}

/******************************************************************************/

BOOL    CMDARG_data( int &dest, char *data )
{
BOOL    ok=TRUE;

    if( data == NULL )
    {
        ok = FALSE;
    }
    else
    {
        if( data[0] == 'x' )
        {
            if( sscanf(&data[1],"%X",&dest) != 1 )
            {
                ok = FALSE;
            }
        }
        else
        {
            dest = atol(data);
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    CMDARG_data( int &dest, char *data, int min, int max )
{
BOOL    ok=TRUE;
int     temp;

    if( (ok=CMDARG_data(temp,data)) )
    {
        if( (temp < min) || (temp > max) )
        {
            ok = FALSE;
        }
        else
        {
            dest = temp;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( int dest[], char *data, int n )
{
char **list;
int tokens,i;
BOOL ok;

    list = STR_tokens(data,tokens);

    for( ok=TRUE,i=0; ((i < tokens) && (i < n) && ok); i++ )
    {
        ok = CMDARG_data(dest[i],list[i]);
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( int dest[], char *data, int min, int max, int n )
{
char **list;
int tokens,i;
BOOL ok;

    list = STR_tokens(data,tokens);

    for( ok=TRUE,i=0; ((i < tokens) && (i < n) && ok); i++ )
    {
        ok = CMDARG_data(dest[i],list[i],min,max);
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( short &dest, char *data )
{
BOOL ok;
int temp;

    if( (ok=CMDARG_data(temp,data)) )
    {
        dest = (short)temp;
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( short &dest, char *data, short min, short max )
{
BOOL ok;
int temp;

    if( (ok=CMDARG_data(temp,data,(int)min,(int)max)) )
    {
        dest = (short)temp;
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( USHORT &dest, char *data )
{
BOOL ok;
int temp;

    if( (ok=CMDARG_data(temp,data)) )
    {
        dest = (USHORT)temp;
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( USHORT &dest, char *data, USHORT min, USHORT max )
{
BOOL ok;
int temp;

    if( (ok=CMDARG_data(temp,data,(int)min,(int)max)) )
    {
        dest = (USHORT)temp;
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( double &dest, char *data )
{
BOOL ok=TRUE;

    if( data == NULL )
    {
        ok = FALSE;
    }
    else
    {
        dest = atof(data);
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( double &dest, char *data, double min, double max )
{
BOOL ok=TRUE;
double temp;

    if( (ok=CMDARG_data(temp,data)) )
    {
        if( (temp < min) || (temp > max) )
        {
            ok = FALSE;
        }
        else
        {
            dest = temp;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( double dest[], char *data, int n )
{
char **list;
int tokens,i;
BOOL ok;

    list = STR_tokens(data,tokens);

    for( ok=TRUE,i=0; ((i < tokens) && (i < n) && ok); i++ )
    {
        ok = CMDARG_data(dest[i],list[i]);
    }

    return(ok);
}
/******************************************************************************/

BOOL CMDARG_data( double dest[], char *data, double min, double max, int n )
{
char **list;
int tokens,i;
BOOL ok;

    list = STR_tokens(data,tokens);

    for( ok=TRUE,i=0; ((i < tokens) && (i < n) && ok); i++ )
    {
        ok = CMDARG_data(dest[i],list[i],min,max);
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( float &dest, char *data )
{
BOOL ok;
double temp;

    if( (ok=CMDARG_data(temp,data)) )
    {
        dest = (float)temp;
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( float &dest, char *data, float min, float max )
{
BOOL ok;
double temp;

    if( (ok=CMDARG_data(temp,data,(double)min,(double)max)) )
    {
        dest = (float)temp;
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( float dest[], char *data, int n )
{
char **list;
int tokens,i;
BOOL ok;

    list = STR_tokens(data,tokens);

    for( ok=TRUE,i=0; ((i < tokens) && (i < n) && ok); i++ )
    {
        ok = CMDARG_data(dest[i],list[i]);
    }

    return(ok);
}

/******************************************************************************/

BOOL CMDARG_data( float dest[], char *data, float min, float max, int n )
{
char **list;
int tokens,i;
BOOL ok;

    list = STR_tokens(data,tokens);

    for( ok=TRUE,i=0; ((i < tokens) && (i < n) && ok); i++ )
    {
        ok = CMDARG_data(dest[i],list[i],min,max);
    }

    return(ok);
}

/******************************************************************************/

