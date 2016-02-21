/******************************************************************************/
/*                                                                            */
/* MODULE  : CONFIG.cpp                                                       */
/*                                                                            */
/* PURPOSE : Configuration file I/O functions.                                */
/*                                                                            */
/* DATE    : 18/Sep/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 18/Sep/2000 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 23/Jan/2002 - Read a list of default environment variables.      */
/*                                                                            */
/* V1.2  JNI 28/Feb/2002 - Allocate buffer space from a fixed pool.           */
/*                                                                            */
/* V1.3  JNI 15/Jun/2002 - Changed parsing to allow spaces in data.           */
/*                                                                            */
/* V1.4  JNI 16/Jul/2003 - Comments in configuration files with '%' or '#'.   */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "CONFIG"
#define MODULE_TEXT     "Configuration File API"
#define MODULE_DATE     "16/07/2003"
#define MODULE_VERSION  "1.4"
#define MODULE_LEVEL    1

/******************************************************************************/

#include <motor.h>                               // Includes (almost) everything we need.

/******************************************************************************/

char   *CONFIG_typetext[] = { "none","char","int","long","float","double","string","BOOL","short","USHORT","matrix","label",NULL };
int     CONFIG_typesize[] = { 0,sizeof(char),sizeof(int),sizeof(long),sizeof(float),sizeof(double),sizeof(STRING),sizeof(BOOL),sizeof(short),sizeof(USHORT),0,0,-1 };

/******************************************************************************/

BOOL    CONFIG_API_started = FALSE;

/******************************************************************************/

PRINTF  CONFIG_PRN_messgf=NULL;                  // General messages printf function.
PRINTF  CONFIG_PRN_errorf=NULL;                  // Error messages printf function.
PRINTF  CONFIG_PRN_debugf=NULL;                  // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     CONFIG_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(CONFIG_PRN_messgf,buff));
}

/******************************************************************************/

int     CONFIG_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(CONFIG_PRN_errorf,buff));
}

/******************************************************************************/

int     CONFIG_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(CONFIG_PRN_debugf,buff));
}

/******************************************************************************/

int                          CONFIG_item=0;
struct  CONFIG_variable      CONFIG_cnfg[CONFIG_CNFG+1];
STRING                       CONFIG_labels="";
int                          CONFIG_flags=CONFIG_FLAG_NONE;

/******************************************************************************/

char    CONFIG_BuffArea[CONFIG_BUFF_POOL][CONFIG_BUFF_SIZE+1];
int     CONFIG_BuffIndx=0;

char   *CONFIG_Buff( void )
{
int     indx;

    indx = CONFIG_BuffIndx;
    if( ++CONFIG_BuffIndx >= CONFIG_BUFF_POOL )
    {
        CONFIG_BuffIndx = 0;
    }

    memset(CONFIG_BuffArea[indx],0x00,CONFIG_BUFF_SIZE+1);

    return(CONFIG_BuffArea[indx]);
}

/******************************************************************************/

FILE   *CONFIG_open( char *file, char *mode )
{
FILE   *FP=NULL;

    if( STR_null(file) )
    {
        CONFIG_errorf("CONFIG_open(...) NULL file name.\n");
    }
    else
    if( (FP=fopen(file,mode)) == NULL )
    {
        CONFIG_errorf("CONFIG_open(file=%s,mode=%s) Cannot open file.\n",file,mode);
    }
    else
    {
        CONFIG_debugf("CONFIG_open(file=%s,mode=%s) File open.\n",file,mode);
    }

    return(FP);
}

/******************************************************************************/

void    CONFIG_close( FILE *FP )
{
    if( FP == NULL )
    {
        return;
    }

    if( fclose(FP) != 0 )
    {
        CONFIG_errorf("CONFIG_close(...) Cannot close file.\n");
    }
    else
    {
        CONFIG_debugf("CONFIG_close(...) File closed.\n");
    }
}

/******************************************************************************/

BOOL CONFIG_EoT( struct CONFIG_variable cnfg[], int item )
{
BOOL EoT;

    EoT = (cnfg[item].type == CONFIG_TYPE_NONE);

    return(EoT);
}

/******************************************************************************/

BOOL CONFIG_EoT( int item )
{
BOOL EoT;

    EoT = CONFIG_EoT(CONFIG_cnfg,item);

    return(EoT);
}

/******************************************************************************/

void    CONFIG_array( struct CONFIG_variable *cnfg )
{
int     n;

    for( cnfg->indx[CONFIG_DIMENSIONS]=1,n=0; (n < CONFIG_DIMENSIONS); n++ )
    {
        if( cnfg->indx[n] <= 1 )
        {
            break;
        }

        cnfg->indx[CONFIG_DIMENSIONS] *= cnfg->indx[n];
    }

    if( cnfg->indx[CONFIG_DIMENSIONS] > 1 )
    {
        cnfg->flag |= CONFIG_FLAG_ARRAY;
    }
}

/******************************************************************************/

void   *CONFIG_array( struct CONFIG_variable *cnfg, int indx )
{
int     size;
BYTE   *bptr;
void   *vptr=NULL;

    if( cnfg->flag & CONFIG_FLAG_ARRAY )
    {
        if( (indx >= 0) && (indx < cnfg->indx[CONFIG_DIMENSIONS]) )
        {
            size = CONFIG_typesize[cnfg->type];
            bptr = (BYTE *)cnfg->vptr;
            vptr = &bptr[indx * size];
        }
        else
        {
            CONFIG_errorf("CONFIG_array() %s[%d] Element out of range (%d).\n",cnfg->name,indx,cnfg->indx[CONFIG_DIMENSIONS]);
        }
    }
    else
    {
        CONFIG_errorf("CONFIG_array() %s[%d] Not an array.\n",cnfg->name,indx);
    }

    return(vptr);
}

/******************************************************************************/

int     CONFIG_check( struct CONFIG_variable cnfg[] )
{
BOOL    ok;
int     item,n;

    for( ok=TRUE,item=0; (ok && !CONFIG_EoT(cnfg,item)); item++ )
    {
        if( cnfg[item].flag & CONFIG_FLAG_PARSE )
        {
            continue;
        }

        CONFIG_array(&cnfg[item]);

        cnfg[item].flag |= CONFIG_FLAG_PARSE;
    }

    return(ok ? item : CONFIG_INVALID);
}

/******************************************************************************/

BOOL    CONFIG_check( struct CONFIG_variable cnfg[], int &count )
{
BOOL    ok=TRUE;

    if( (count=CONFIG_check(cnfg)) == CONFIG_INVALID )
    {
        ok = FALSE;
    }
    else
    if( count == 0 )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok=TRUE;

//  Start the API once...
    if( CONFIG_API_started )
    {
        return(TRUE);
    }

    CONFIG_PRN_messgf = messgf;             // General API message print function.
    CONFIG_PRN_errorf = errorf;             // API error message print function.
    CONFIG_PRN_debugf = debugf;             // Debug information print function.

    ATEXIT_API(CONFIG_API_stop);            // Install stop function.
    CONFIG_API_started = TRUE;              // Set started flag.
    MODULE_start(CONFIG_PRN_messgf);        // Register module.
    CONFIG_reset();                         // Must do reset here.

    // Load environment variables... (V1.1)
    ok = CONFIG_EnvironmentLoad();  

    return(TRUE);
}

/******************************************************************************/

void    CONFIG_API_stop( void )
{
    if( !CONFIG_API_started )          // API not started in the first place...
    {
         return;
    }

    CONFIG_API_started = FALSE;        // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    CONFIG_API_check( void )
{
BOOL    ok=TRUE;

    if( !CONFIG_API_started )          // API not started...
    {                                  // Start module automatically...
        ok = CONFIG_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        CONFIG_debugf("CONFIG_API_check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/******************************************************************************/

void    CONFIG_flagON( struct CONFIG_variable cnfg[], int flag  )
{
int     item;

    for( item=0; !CONFIG_EoT(cnfg,item); item++ )
    {
        CONFIG_flagON(cnfg,item,flag);
    }
}

/******************************************************************************/

void    CONFIG_flagON( struct CONFIG_variable cnfg[], int item, int flag )
{
    cnfg[item].flag |= flag;
}

/******************************************************************************/

void    CONFIG_flagON( int flag )
{
    CONFIG_flagON(CONFIG_cnfg,flag);
}

/******************************************************************************/

void    CONFIG_flagON( int item, int flag )
{
    CONFIG_flagON(CONFIG_cnfg,item,flag);
}

/******************************************************************************/

void    CONFIG_flagOFF( struct CONFIG_variable cnfg[], int flag )
{
int     item;

    for( item=0; !CONFIG_EoT(cnfg,item); item++ )
    {
        CONFIG_flagOFF(cnfg,item,flag);
    }
}

/******************************************************************************/

void    CONFIG_flagOFF( struct CONFIG_variable cnfg[], int item, int flag )
{
    cnfg[item].flag &= ~flag;
}

/******************************************************************************/

void    CONFIG_flagOFF( int flag )
{
    CONFIG_flagOFF(CONFIG_cnfg,flag);
}

/******************************************************************************/

void    CONFIG_flagOFF( int item, int flag )
{
    CONFIG_flagOFF(CONFIG_cnfg,item,flag);
}

/******************************************************************************/

BOOL    CONFIG_flag( struct CONFIG_variable cnfg[], int flag )
{
int     item;
BOOL    set;

    for( set=TRUE,item=0; (!CONFIG_EoT(cnfg,item) && set); item++ )
    {
        set = CONFIG_flag(cnfg,item,flag);
    }

    return(set);
}

/******************************************************************************/

BOOL    CONFIG_flag( struct CONFIG_variable cnfg[], int item, int flag )
{
    return((cnfg[item].flag & flag) != 0);
}

/******************************************************************************/

BOOL    CONFIG_flag( int flag )
{
    return(CONFIG_flag(CONFIG_cnfg,flag));
}

/******************************************************************************/

BOOL    CONFIG_flag( int item, int flag )
{
    return(CONFIG_flag(CONFIG_cnfg,item,flag));
}

/******************************************************************************/

char   *CONFIG_flag( struct CONFIG_variable cnfg[], int item, int flag, char *on, char *off )
{
char   *buff;

    buff = CONFIG_flag(cnfg,item,flag) ? on : off;

    return(buff);
}

/******************************************************************************/

char   *CONFIG_flag( int item, int flag, char *on, char *off )
{
    return(CONFIG_flag(CONFIG_cnfg,item,flag,on,off));
}

/******************************************************************************/

BOOL    CONFIG_readflag( struct CONFIG_variable cnfg[], int item )
{
BOOL    flag=FALSE;

    flag = CONFIG_flag(cnfg,item,CONFIG_FLAG_READ);

    return(flag);
}

/******************************************************************************/

BOOL    CONFIG_readflag( int item )
{
BOOL    flag=FALSE;

    flag = CONFIG_readflag(CONFIG_cnfg,item);

    return(flag);
}

/******************************************************************************/

BOOL    CONFIG_readflag( struct CONFIG_variable cnfg[], char *name )
{
int     item;
BOOL    flag=FALSE;

    if( (item=CONFIG_lookup(cnfg,name)) != CONFIG_NOTFOUND )
    {
        flag = CONFIG_flag(cnfg,item,CONFIG_FLAG_READ);
    }

    return(flag);
}

/******************************************************************************/

BOOL    CONFIG_readflag( char *name )
{
BOOL    flag;

    flag = CONFIG_readflag(CONFIG_cnfg,name);

    return(flag);
}

/******************************************************************************/

void    CONFIG_reset( void )
{
int     item;

    if( !CONFIG_API_check() )               // Check if API started okay...
    {
        return;
    }

    for( item=0; (item <= CONFIG_CNFG); item++ )
    {
        memset(&CONFIG_cnfg[item],0,sizeof(struct CONFIG_variable));
        CONFIG_cnfg[item].type = CONFIG_TYPE_NONE;
    }

    CONFIG_item = 0;

    CONFIG_setlabel(NULL);

    CONFIG_debugf("CONFIG_reset()\n");
}

/******************************************************************************/

void    CONFIG_defaultflags( int flag )
{
    CONFIG_flags = flag;
}

/******************************************************************************/

void CONFIG_setlabel( char *label )
{
    if( label != NULL )
    {
        strncpy(CONFIG_labels,label,STRLEN);
    }
    else
    {
        memset(CONFIG_labels,0,STRLEN);
    }
}

/******************************************************************************/

int     CONFIG_set( int type, int flag, char *name, void *vptr, int indx0, int indx1 )
{
int     item=CONFIG_INVALID;

    if( !CONFIG_API_check() )               // Check if API started okay...
    {
        return(item);
    }

    if( (item=CONFIG_lookup(CONFIG_cnfg,name)) != CONFIG_NOTFOUND )
    {
        CONFIG_debugf("CONFIG_set(...) %s Already set.\n",name);
        return(item);
    }

    if( CONFIG_item >= CONFIG_CNFG )
    {
        CONFIG_errorf("CONFIG_set(...) %s Too many variables (max %d).\n",name,CONFIG_CNFG);
        return(item);
    }

    // Get item number for this variable...
    item = CONFIG_item++;

    // Variable may be an array...
    if( indx0 > 1 )
    {
        flag |= CONFIG_FLAG_ARRAY;
    }

    if( flag & CONFIG_FLAG_LABEL )
    {
        CONFIG_setlabel(name);
    }

    // Set variable details...
    strncpy(CONFIG_cnfg[item].name,name,STRLEN);
    strncpy(CONFIG_cnfg[item].label,CONFIG_labels,STRLEN);
    CONFIG_cnfg[item].vptr = vptr;   
    CONFIG_cnfg[item].type = type;
    CONFIG_cnfg[item].flag = flag;

    // Set array indexes...
    CONFIG_cnfg[item].indx[0] = indx0;
    CONFIG_cnfg[item].indx[1] = indx1;

    CONFIG_debugf("CONFIG_set(...) %02d %-6.6s %s\n",item,CONFIG_typetext[type],CONFIG_name(&CONFIG_cnfg[item]));

    return(item);
}

/******************************************************************************/

int     CONFIG_set( int type, int flag, char *name, void *vptr, int indx0 )
{
int     item;

    item = CONFIG_set(type,flag,name,vptr,indx0,0);

    return(item);
}

/******************************************************************************/

int     CONFIG_set( int type, int flag, char *name, void *vptr )
{
int     item;

    item = CONFIG_set(type,flag,name,vptr,0,0);

    return(item);
}

/******************************************************************************/

int     CONFIG_set( int type, char *name, void *vptr, int indx0, int indx1 )
{
int     item;

    // Create configuration variable using default flags...
    item = CONFIG_set(type,CONFIG_flags,name,vptr,indx0,indx1);

    return(item);
}
/******************************************************************************/

int     CONFIG_set( int type, char *name, void *vptr, int indx0 )
{
int     item;

    item = CONFIG_set(type,name,vptr,indx0,0);

    return(item);
}

/******************************************************************************/

int     CONFIG_set( int type, char *name, void *vptr )
{
int     item;

    item = CONFIG_set(type,name,vptr,0,0);

    return(item);
}

/******************************************************************************/

int     CONFIG_set( char *name, int *data, int indx0 )        { return(CONFIG_set(CONFIG_TYPE_INT,name,data,indx0)); }
int     CONFIG_set( char *name, int &data )                   { return(CONFIG_set(CONFIG_TYPE_INT,name,&data)); }
int     CONFIG_label( char *name, int *data, int indx0 )      { return(CONFIG_set(CONFIG_TYPE_INT,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_label( char *name, int &data )                 { return(CONFIG_set(CONFIG_TYPE_INT,CONFIG_FLAG_LABEL,name,&data)); }

int     CONFIG_set( char *name, long *data, int indx0 )       { return(CONFIG_set(CONFIG_TYPE_LONG,name,data,indx0)); }
int     CONFIG_set( char *name, long &data )                  { return(CONFIG_set(CONFIG_TYPE_LONG,name,&data)); }
int     CONFIG_label( char *name, long *data, int indx0 )     { return(CONFIG_set(CONFIG_TYPE_LONG,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_label( char *name, long &data )                { return(CONFIG_set(CONFIG_TYPE_LONG,CONFIG_FLAG_LABEL,name,&data)); }

int     CONFIG_set( char *name, float *data, int indx0 )      { return(CONFIG_set(CONFIG_TYPE_FLOAT,name,data,indx0)); }
int     CONFIG_set( char *name, float &data )                 { return(CONFIG_set(CONFIG_TYPE_FLOAT,name,&data)); }
int     CONFIG_label( char *name, float *data, int indx0 )    { return(CONFIG_set(CONFIG_TYPE_FLOAT,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_label( char *name, float &data )               { return(CONFIG_set(CONFIG_TYPE_FLOAT,CONFIG_FLAG_LABEL,name,&data)); }

int     CONFIG_set( char *name, double *data, int indx0 )     { return(CONFIG_set(CONFIG_TYPE_DOUBLE,name,data,indx0)); }
int     CONFIG_set( char *name, double &data )                { return(CONFIG_set(CONFIG_TYPE_DOUBLE,name,&data)); }
int     CONFIG_label( char *name, double *data, int indx0 )   { return(CONFIG_set(CONFIG_TYPE_DOUBLE,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_label( char *name, double &data )              { return(CONFIG_set(CONFIG_TYPE_DOUBLE,CONFIG_FLAG_LABEL,name,&data)); }

int     CONFIG_set( char *name, STRING *data, int indx0 )     { return(CONFIG_set(CONFIG_TYPE_STRING,name,data,indx0)); }
int     CONFIG_set( char *name, STRING data )                 { return(CONFIG_set(CONFIG_TYPE_STRING,name,data)); }
int     CONFIG_label( char *name, STRING *data, int indx0 )   { return(CONFIG_set(CONFIG_TYPE_STRING,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_label( char *name, STRING data )               { return(CONFIG_set(CONFIG_TYPE_STRING,CONFIG_FLAG_LABEL,name,data)); }

int     CONFIG_set( char *name, char *data, int indx0 )       { return(CONFIG_set(CONFIG_TYPE_CHAR,name,data,indx0)); }
int     CONFIG_set( char *name, char &data )                  { return(CONFIG_set(CONFIG_TYPE_CHAR,name,&data)); }
int     CONFIG_label( char *name, char *data, int indx0 )     { return(CONFIG_set(CONFIG_TYPE_CHAR,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_label( char *name, char &data )                { return(CONFIG_set(CONFIG_TYPE_CHAR,CONFIG_FLAG_LABEL,name,&data)); }

int     CONFIG_setBOOL( char *name, BOOL *data, int indx0 )   { return(CONFIG_set(CONFIG_TYPE_BOOL,name,data,indx0)); }
int     CONFIG_setBOOL( char *name, BOOL &data )              { return(CONFIG_set(CONFIG_TYPE_BOOL,name,&data)); }
int     CONFIG_labelBOOL( char *name, BOOL *data, int indx0 ) { return(CONFIG_set(CONFIG_TYPE_BOOL,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_labelBOOL( char *name, BOOL &data )            { return(CONFIG_set(CONFIG_TYPE_BOOL,CONFIG_FLAG_LABEL,name,&data)); }

int     CONFIG_set( char *name, short *data, int indx0 )      { return(CONFIG_set(CONFIG_TYPE_SHORT,name,data,indx0)); }
int     CONFIG_set( char *name, short &data )                 { return(CONFIG_set(CONFIG_TYPE_SHORT,name,&data)); }
int     CONFIG_label( char *name, short *data, int indx0 )    { return(CONFIG_set(CONFIG_TYPE_SHORT,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_label( char *name, short &data )               { return(CONFIG_set(CONFIG_TYPE_SHORT,CONFIG_FLAG_LABEL,name,&data)); }

int     CONFIG_set( char *name, USHORT *data, int indx0 )     { return(CONFIG_set(CONFIG_TYPE_USHORT,name,data,indx0)); }
int     CONFIG_set( char *name, USHORT &data )                { return(CONFIG_set(CONFIG_TYPE_USHORT,name,&data)); }
int     CONFIG_label( char *name, USHORT *data, int indx0 )   { return(CONFIG_set(CONFIG_TYPE_USHORT,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_label( char *name, USHORT &data )              { return(CONFIG_set(CONFIG_TYPE_USHORT,CONFIG_FLAG_LABEL,name,&data)); }

int     CONFIG_set( char *name, matrix *data, int indx0 )     { return(CONFIG_set(CONFIG_TYPE_MATRIX,name,data,indx0)); }
int     CONFIG_set( char *name, matrix &data )                { return(CONFIG_set(CONFIG_TYPE_MATRIX,name,&data)); }
int     CONFIG_label( char *name, matrix *data, int indx0 )   { return(CONFIG_set(CONFIG_TYPE_MATRIX,CONFIG_FLAG_LABEL,name,data,indx0)); }
int     CONFIG_label( char *name, matrix &data )              { return(CONFIG_set(CONFIG_TYPE_MATRIX,CONFIG_FLAG_LABEL,name,&data)); }

int     CONFIG_label( char *name )                            { return(CONFIG_set(CONFIG_TYPE_STRING,CONFIG_FLAG_LABEL,name,NULL)); }

/******************************************************************************/

void    CONFIG_clear( struct CONFIG_variable cnfg[] )
{
int     item;

    for( item=0; !CONFIG_EoT(cnfg,item); item++ )
    {
        memset(cnfg[item].vptr,0x00,CONFIG_typesize[cnfg[item].type]);
    }
}

/******************************************************************************/

void    CONFIG_clear( void )
{
    CONFIG_clear(CONFIG_cnfg);
}

/******************************************************************************/

int     CONFIG_width( struct CONFIG_variable cnfg[] )
{
int     item,width,w;

    for( width=0,item=0; !CONFIG_EoT(cnfg,item); item++ )
    {
        if( (w=STR_trimlength(cnfg[item].name,STRLEN)) > width )
        {
            width = w;
        }
    }

    return(width);
}

/******************************************************************************/

char   *CONFIG_text( struct CONFIG_variable cnfg[], int item, BOOL type )
{
char   *buff;
int     width;

    // Allocate a string buffer... (V1.2)
    buff = STR_Buff(CONFIG_BUFF_SIZE);

    width = CONFIG_width(cnfg);

    if( type )
    {
        strncat(buff,STR_stringf("%-6.6s ",CONFIG_typetext[cnfg[item].type]),CONFIG_BUFF_SIZE);
    }

    strncat(buff,STR_stringf("%-*.*s %s\n",width,width,cnfg[item].name,CONFIG_cnfg2str(&cnfg[item])),CONFIG_BUFF_SIZE);

    return(buff);
}

/******************************************************************************/

char   *CONFIG_text( struct CONFIG_variable cnfg[], int item )
{
    return(CONFIG_text(cnfg,item,FALSE));
}

/******************************************************************************/

void    CONFIG_list( struct CONFIG_variable cnfg[], int flag, BOOL set, PRINTF prnf )
{
int     item,items;

    if( (items=CONFIG_check(cnfg)) == CONFIG_INVALID )
    {
        return;
    }

    for( item=0; (item < items); item++ )
    {
        if( ((cnfg[item].flag & flag) == set) || !flag )
        {
          (*prnf)(CONFIG_text(cnfg,item));
        }
    }
}

/******************************************************************************/

void    CONFIG_list( int flag, BOOL set, PRINTF prnf )
{
    CONFIG_list(CONFIG_cnfg,flag,set,prnf);
}

/******************************************************************************/

void    CONFIG_list( struct CONFIG_variable cnfg[], PRINTF prnf )
{
    CONFIG_list(cnfg,0x00,FALSE,prnf);
}


/******************************************************************************/

void    CONFIG_list( PRINTF prnf )
{
    CONFIG_list(CONFIG_cnfg,prnf);
}

/******************************************************************************/

void    CONFIG_list( void )
{
    CONFIG_list(CONFIG_cnfg,printf);
}

/******************************************************************************/

int     CONFIG_lookup( struct CONFIG_variable cnfg[], char *name )
{
int     find,item,items;

    if( (items=CONFIG_check(cnfg)) == CONFIG_INVALID )
    {
        return(CONFIG_NOTFOUND);
    }

    for( find=CONFIG_NOTFOUND,item=0; (item < items); item++ )
    {
        if( strcmp(cnfg[item].label,name) == 0 )
        {
            if( cnfg[item].flag & CONFIG_FLAG_LABEL )
            {
                CONFIG_setlabel(cnfg[item].label);
            }
        }

        if( strcmp(cnfg[item].name,name) == 0 )
        {
            if( strcmp(cnfg[item].label,CONFIG_labels) == 0 )
            {
                find = item;
                break;
            }
        }
    }

    return(find);
}

/******************************************************************************/

char    CONFIG_buff2char( char *buff )
{
char    data;

    data = (char)CONFIG_buff2long(buff);
    return(data);
}

/******************************************************************************/

int     CONFIG_buff2int( char *buff )
{
int     data;

    data = (int)CONFIG_buff2long(buff);
    return(data);
}

/******************************************************************************/

long    CONFIG_buff2long( char *buff )
{
long    data;
int     i;

    for( i=0; ((buff[i] != 'x') && (i < 3)); i++ );

    if( i < 2 )
    {
        i++;

        if( sscanf(&buff[i],"%lX",&data) != 1 )
        {
            CONFIG_errorf("CONFIG_buff2long(%s) Invalid Hex value.\n",buff);
            data = CONFIG_NULL_INT;
        }
    }
    else
    {
        data = atol(buff);
    }

    return(data);
}

/******************************************************************************/

float   CONFIG_buff2float( char *buff )
{
float   data;

    data = (float)CONFIG_buff2double(buff);
    return(data);
}

/******************************************************************************/

double  CONFIG_buff2double( char *buff )
{
double  data;

    data = atof(buff);
    return(data);
}

/******************************************************************************/

char   *CONFIG_buff2str( char *buff )
{
char   *str;

    // Allocate a string buffer... (V1.2)
    str = STR_Buff(CONFIG_BUFF_SIZE+1);
    strncpy(str,buff,CONFIG_BUFF_SIZE);
    return(str);
}

/******************************************************************************/

BOOL    CONFIG_buff2bool( char *buff )
{
BOOL    data;

    data = (_stricmp(buff,"TRUE") == 0) || (_stricmp(buff,"YES") == 0) || (_stricmp(buff,"ON") == 0);
    return(data);
}

/******************************************************************************/

short   CONFIG_buff2short( char *buff )
{
short   data;

    data = (short)CONFIG_buff2long(buff);
    return(data);
}

/******************************************************************************/

USHORT  CONFIG_buff2ushort( char *buff )
{
USHORT  data;

    data = (USHORT)CONFIG_buff2long(buff);
    return(data);
}

/******************************************************************************/

BOOL    CONFIG_buff2var( int type, void *vptr, char *buff )
{
BOOL    ok=TRUE;
char    c;
int     i;
long    l;
float   f;
double  d;
char   *s;
BOOL    b;
short   sh;
USHORT  us;
matrix *M;

    switch( CONFIG_type(type,vptr) )
    {
       case CONFIG_TYPE_CHAR :
           c = CONFIG_buff2char(buff);
           memcpy(vptr,&c,sizeof(char));
           break;

        case CONFIG_TYPE_INT :
           i = CONFIG_buff2int(buff);
           memcpy(vptr,&i,sizeof(int));
           break;

        case CONFIG_TYPE_LONG :
           l = CONFIG_buff2long(buff);
           memcpy(vptr,&l,sizeof(long));
           break;

        case CONFIG_TYPE_FLOAT :
           f = CONFIG_buff2float(buff);
           memcpy(vptr,&f,sizeof(float));
           break;

        case CONFIG_TYPE_DOUBLE :
           d = CONFIG_buff2double(buff);
           memcpy(vptr,&d,sizeof(double));
           break;

        case CONFIG_TYPE_STRING :
           s = CONFIG_buff2str(buff);
           strncpy((char *)vptr,s,STRLEN);
           break;

        case CONFIG_TYPE_BOOL :
           b = CONFIG_buff2bool(buff);
           memcpy(vptr,&b,sizeof(BOOL));
           break;

        case CONFIG_TYPE_SHORT :
           sh = CONFIG_buff2short(buff);
           memcpy(vptr,&sh,sizeof(short));
           break;

        case CONFIG_TYPE_USHORT :
           us = CONFIG_buff2ushort(buff);
           memcpy(vptr,&us,sizeof(USHORT));
           break;

        case CONFIG_TYPE_MATRIX :
           s = CONFIG_buff2str(buff);
           M = (matrix *)vptr;
           CONFIG_str2mtx(s,M);
           break;

        case CONFIG_TYPE_NONE :
           break;

        default :
           ok = FALSE;
           break;
    }

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_buff2array( struct CONFIG_variable cnfg[], int item, char *buff )
{
BOOL    ok;
char  **tokens;
int     n,D;
void   *vptr;

    tokens = STR_tokens(buff,CONFIG_DELIMETER,D);

    for( ok=TRUE,n=0; ((n < D) && (n < cnfg[item].indx[0]) && ok); n++ )
    {
        if( (vptr=CONFIG_array(&cnfg[item],n)) == NULL )
        {
            CONFIG_errorf("CONFIG_buff2array() %s[%d] Invalid array index.\n",cnfg[item].name,n);
            ok = FALSE;
            continue;
        }

        ok = CONFIG_buff2var(cnfg[item].type,vptr,tokens[n]);
    }

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_buff2cnfg( struct CONFIG_variable cnfg[], int item, char *buff )
{
BOOL    ok;

    if( cnfg[item].flag & CONFIG_FLAG_ARRAY )
    {
        ok = CONFIG_buff2array(cnfg,item,buff);
    }
    else
    {
        ok = CONFIG_buff2var(cnfg[item].type,cnfg[item].vptr,buff);
    }

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_getvar( struct CONFIG_variable cnfg[], char *name, char *data )
{
int     item;
BOOL    ok=FALSE;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        CONFIG_errorf("CONFIG_getvar() Variable not found [%s].\n",name);
    }
    else
    {
        if( CONFIG_buff2cnfg(cnfg,item,data) )
        {
            CONFIG_flagON(cnfg,item,CONFIG_FLAG_READ);
            ok = TRUE;
        }
    }

    STR_printf(ok,CONFIG_debugf,CONFIG_errorf,"CONFIG_getvar(name=%s,data=%s) %s.\n",name,data,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_putvar( FILE *FP, struct CONFIG_variable *cnfg )
{
BOOL    ok;
char   *var;

    var = STR_stringf("%s%c%s",cnfg->name,CONFIG_SEPARATOR,CONFIG_cnfg2str(cnfg));

    CONFIG_debugf("[%s]\n",var);

    ok = (fprintf(FP,"%s\n",var) > 0 );

    return(ok);
}

/******************************************************************************/

char    CONFIG_char( void *vptr )
{
    return(*(char *)vptr);
}

/******************************************************************************/

int     CONFIG_int( void *vptr )
{
    return(*(int *)vptr);
}

/******************************************************************************/

long    CONFIG_long( void *vptr )
{
    return(*(long *)vptr);
}

/******************************************************************************/

float   CONFIG_float( void *vptr )
{
    return(*(float *)vptr);
}

/******************************************************************************/

double  CONFIG_double( void *vptr )
{
    return(*(double *)vptr);
}

/******************************************************************************/

char   *CONFIG_string( void *vptr )
{
    return((char *)vptr);
}

/******************************************************************************/

BOOL    CONFIG_bool( void *vptr )
{
    return(*(BOOL *)vptr);
}

/******************************************************************************/

short   CONFIG_short( void *vptr )
{
    return(*(short *)vptr);
}

/******************************************************************************/

USHORT  CONFIG_ushort( void *vptr )
{
    return(*(USHORT *)vptr);
}

/******************************************************************************/

matrix *CONFIG_matrix( void *vptr )
{
    return((matrix *)vptr);
}

/******************************************************************************/

char    CONFIG_char( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_CHAR);
    }

    return(CONFIG_char(cnfg[item].vptr));
}

/******************************************************************************/

char    CONFIG_char( char *name )
{
    return(CONFIG_char(CONFIG_cnfg,name));
}

/******************************************************************************/

int     CONFIG_int( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_INT);
    }

    return(CONFIG_int(cnfg[item].vptr));
}

/******************************************************************************/

int     CONFIG_int( char *name )
{
    return(CONFIG_int(CONFIG_cnfg,name));
}

/******************************************************************************/

long    CONFIG_long( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_LONG);
    }

    return(CONFIG_long(cnfg[item].vptr));
}

/******************************************************************************/

long    CONFIG_long( char *name )
{
    return(CONFIG_long(CONFIG_cnfg,name));
}

/******************************************************************************/

float   CONFIG_float( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_FLOAT);
    }

    return(CONFIG_float(cnfg[item].vptr));
}

/******************************************************************************/

float   CONFIG_float( char *name )
{
    return(CONFIG_float(CONFIG_cnfg,name));
}

/******************************************************************************/

double  CONFIG_double( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_DOUBLE);
    }

    return(CONFIG_double(cnfg[item].vptr));
}

/******************************************************************************/

double  CONFIG_double( char *name )
{
    return(CONFIG_double(CONFIG_cnfg,name));
}

/******************************************************************************/

char   *CONFIG_string( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_STRING);
    }

    return(CONFIG_string(cnfg[item].vptr));
}

/******************************************************************************/

char   *CONFIG_string( char *name )
{
    return(CONFIG_string(CONFIG_cnfg,name));
}

/******************************************************************************/

BOOL    CONFIG_bool( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_BOOL);
    }

    return(CONFIG_bool(cnfg[item].vptr));
}

/******************************************************************************/

BOOL    CONFIG_bool( char *name )
{
    return(CONFIG_bool(CONFIG_cnfg,name));
}

/******************************************************************************/

short   CONFIG_short( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_SHORT);
    }

    return(CONFIG_short(cnfg[item].vptr));
}

/******************************************************************************/

short   CONFIG_short( char *name )
{
    return(CONFIG_short(CONFIG_cnfg,name));
}

/******************************************************************************/

USHORT  CONFIG_ushort( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_USHORT);
    }

    return(CONFIG_ushort(cnfg[item].vptr));
}

/******************************************************************************/

USHORT  CONFIG_ushort( char *name )
{
    return(CONFIG_ushort(CONFIG_cnfg,name));
}

/******************************************************************************/

matrix *CONFIG_matrix( struct CONFIG_variable cnfg[], char *name )
{
int     item;

    if( (item=CONFIG_lookup(cnfg,name)) == CONFIG_NOTFOUND )
    {
        return(CONFIG_NULL_STRING);
    }

    return(CONFIG_matrix(cnfg[item].vptr));
}

/******************************************************************************/

matrix *CONFIG_matrix( char *name )
{
    return(CONFIG_matrix(CONFIG_cnfg,name));
}

/******************************************************************************/

int CONFIG_type( int type, void *vptr )
{
    if( vptr != NULL )
    {
        return(type);
    }

    return(CONFIG_TYPE_NONE);
}

/******************************************************************************/

char   *CONFIG_var2str( int type, void *vptr )
{
char   *buff;

    // Allocate a string buffer... (V1.2)
    buff = STR_Buff(STRLEN);

    // Do variable-type specific stuff...
    switch( CONFIG_type(type,vptr) )
    {
        case CONFIG_TYPE_CHAR :
           sprintf(buff,"%ld",(long)CONFIG_char(vptr));           
           break;

        case CONFIG_TYPE_INT :
           sprintf(buff,"%ld",(long)CONFIG_int(vptr));
           break;

        case CONFIG_TYPE_LONG :
           sprintf(buff,"%ld",CONFIG_long(vptr));
           break;

        case CONFIG_TYPE_FLOAT :
           sprintf(buff,"%.10lg",(double)CONFIG_float(vptr));
           break;

        case CONFIG_TYPE_DOUBLE :
           sprintf(buff,"%.10lg",CONFIG_double(vptr));
           break;

        case CONFIG_TYPE_STRING :
           strncpy(buff,CONFIG_string(vptr),STRLEN);
           break;

        case CONFIG_TYPE_BOOL :
           sprintf(buff,"%s",STR_TrueFalse(CONFIG_bool(vptr)));
           break;

        case CONFIG_TYPE_SHORT :
           sprintf(buff,"%ld",(long)CONFIG_short(vptr));
           break;

        case CONFIG_TYPE_USHORT :
           sprintf(buff,"%lu",(ULONG)CONFIG_ushort(vptr));
           break;

        case CONFIG_TYPE_MATRIX :
           CONFIG_mtx2str(buff,CONFIG_matrix(vptr));
           break;

        case CONFIG_TYPE_NONE :
           break;
    }

    return(buff);
}

/******************************************************************************/

char   *CONFIG_cnfg2str( struct CONFIG_variable *cnfg )
{
char   *buff;
void   *vptr;
int     n;

    // Simple if not an array variable...
    if( !(cnfg->flag & CONFIG_FLAG_ARRAY) )
    {
        return(CONFIG_var2str(cnfg->type,cnfg->vptr));
    }

    // Do array variable stuff...

    // Allocate a string buffer... (V1.2)
    buff = STR_Buff(CONFIG_BUFF_SIZE);

    // Loop for each index in array...
    for( n=0; (n < cnfg->indx[0]); n++ )
    {
        if( (vptr=CONFIG_array(cnfg,n)) == NULL )
        {
            CONFIG_errorf("CONFIG_cnfg2str() %s[%d] Invalid array index.\n",cnfg->name,n);
            break;
        }

        if( n > 0 )
        {
            strncat(buff,CONFIG_DELIMETER,CONFIG_BUFF_SIZE);
        }

        strncat(buff,CONFIG_var2str(cnfg->type,vptr),CONFIG_BUFF_SIZE);
    }

    return(buff);
}

/******************************************************************************/

void    CONFIG_mtx2str( char *str, matrix *mtx )
{
    strncpy(str,matrix_string(*mtx,"%.10lg"),STRLEN);
}

/**************************************************************************/

void    CONFIG_str2mtx( char *str, matrix *mtx )
{
    matrix_data(*mtx,str);
}

/******************************************************************************/

char   *CONFIG_dimensions( int indx[] )
{
char   *buff;
int     n;

    // Allocate a string buffer... (V1.2)
    buff = STR_Buff(STRLEN);

    for( n=0; (n < CONFIG_DIMENSIONS); n++ )
    {
        if( indx[n] <= 1 )
        {
            break;
        }

        strncat(buff,STR_stringf("[%d]",indx[n]),STRLEN);
    }

    return(buff);
}

/******************************************************************************/

char   *CONFIG_dimensions( CONFIG_variable *cnfg )
{
    return(CONFIG_dimensions(cnfg->indx));
}

/******************************************************************************/

char *CONFIG_name( CONFIG_variable *cnfg )
{
char *name;

    if( STR_null(cnfg->label) )
    {
        name = STR_stringf("%s%s",cnfg->name,CONFIG_dimensions(cnfg));
    }
    else
    {
        name = STR_stringf("%s:%s%s",cnfg->label,cnfg->name,CONFIG_dimensions(cnfg));
    }

    return(name);
}

/******************************************************************************/

BOOL    CONFIG_dimensions( char *name, int &nD, int indx[] )
{
char  **tokens;
int     item,items,n;

//  Zero array dimensions...
    for( n=0; (n < CONFIG_DIMENSIONS); indx[n++]=0 );

//  Extract list of tokens from variable name...
    tokens = STR_tokens(name,"[]",items);

//  Check number of dimensions...
    if( (items-1) > CONFIG_DIMENSIONS )
    {
        CONFIG_errorf("CONFIG_dimensions(name=%s) Too many dimensions.\n",name);
        return(FALSE);
    }

//  First token in list is variable name...
    item = 0;
    strcpy(name,tokens[item++]);

//  The rest (if they exist) are array dimensions...
    for( nD=0; ((item < items) && (nD < CONFIG_DIMENSIONS)); item++,nD++ )
    {
        indx[nD] = atoi(tokens[item]);
    }

    return(TRUE);
}

/******************************************************************************/

BOOL    CONFIG_dimensions( struct CONFIG_variable cnfg[], int item )
{
BOOL    ok;
int     nD;

    // Have array dimensions been assigned already...
    if( cnfg[item].flag & CONFIG_FLAG_ARRAY )
    {
        return(TRUE);
    }

    // Parse array dimensions from variable name...
    if( (ok=CONFIG_dimensions(cnfg[item].name,nD,cnfg[item].indx)) )
    {
        if( nD > 0 )
        {
            cnfg[item].flag |= CONFIG_FLAG_ARRAY;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_parse( char *buff, char *name, char *data )
{
BOOL    ok=TRUE;
int     b,l;

    l = strlen(buff);

    // First SPACE or TAB separates name from data...
    for( b=0; (b < l); b++ )
    {
        if( (buff[b] == SPACE) || (buff[b] == TAB) )
        {
            break;
        }
    }

    memcpy(name,buff,b);

    // Whatever's left over is data...
    for( ; (b < l); b++ )
    {
        if( (buff[b] != SPACE) && (buff[b] != TAB) )
        {
            break;
        }
    }

    memcpy(data,&buff[b],l-b);

//  ok = (sscanf(buff,"%s %s\n",name,data) == 2);

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_process( struct CONFIG_variable cnfg[], char *buff )
{
BOOL    ok;
char   *name,*data;

    // Allocate local buffers... (V1.2)
    name = CONFIG_Buff();
    data = CONFIG_Buff();

    if( !CONFIG_parse(buff,name,data) )
    {
        return(FALSE);
    }

    CONFIG_debugf("%s [%s]\n",name,data);

    if( !CONFIG_getvar(cnfg,name,data) )
    {
        return(FALSE);
    }

    return(TRUE);
}

/******************************************************************************/

BOOL    CONFIG_read( FILE *FP, char *buff, int size, BOOL &EofF )
{
BOOL    ok=TRUE;

    memset(buff,NUL,size);

    if( fgets(buff,size,FP) != NULL )
    {
        STR_trimCRLF(buff);
    }
    else
    if( !(EofF=feof(FP)) )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_read( struct CONFIG_variable cnfg[], char *file )
{
BOOL    ok=TRUE,EofF=FALSE;
FILE   *FP=NULL;
int     line=0,count;
char   *buff;

    // Check if API started...
    if( !CONFIG_API_check() )
    {
        return(FALSE);
    }

    // Make sure file isn't NULL...
    if( file == NULL )
    {
        return(FALSE);
    }

    // Check CONFIGuration variable list is okay...
    if( !CONFIG_check(cnfg,count) )
    {
        CONFIG_errorf("CONFIG_read(file=%s) Configuration list invalid.\n",file);
        return(FALSE);
    }

    CONFIG_setlabel(NULL);

    CONFIG_flagOFF(cnfg,CONFIG_FLAG_READ);

    ok = ((FP=CONFIG_open(file,"r")) != NULL);

    while( ok && !EofF )
    {
        // Allocate a local buffer... (V1.2)
        buff = CONFIG_Buff();

        // Read configuration file, one line at a time...
        if( !(ok=CONFIG_read(FP,buff,CONFIG_BUFF_SIZE,EofF)) )
        {
            CONFIG_errorf("CONFIG_read(...) %s Cannot read file.\n",file);
            continue;
        }

        line++;

        // Empty line, so skip it...
        if( STR_null(buff,CONFIG_BUFF_SIZE) )
        {
            continue;
        }

        CONFIG_debugf("%s[%02d] %s\n",file,line,buff);

        // Comment line, so skip it...
        if( (buff[0] == '%') || (buff[0] == '#') )
        {
            continue;
        }

        // Process configuration line...
        if( !(ok=CONFIG_process(cnfg,buff)) )
        {
            CONFIG_errorf("CONFIG_read(...) %s[%d] Cannot parse line.\n",file,line);
            continue;
        }
    }

    CONFIG_close(FP);

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_write( struct CONFIG_variable cnfg[], char *file, int flag )
{
BOOL    ok=TRUE;
FILE   *FP=NULL;
int     count,item;

    // Check if API started...
    if( !CONFIG_API_check() )
    {
        return(FALSE);
    }

    // Make sure file isn't NULL...
    if( file == NULL )
    {
        return(FALSE);
    }

    // Check CONFIGuration variable list is okay...
    if( !CONFIG_check(cnfg,count) )
    {
        CONFIG_errorf("CONFIG_write(file=%s) Configuration list invalid.\n",file);
        return(FALSE);
    }

    ok = ((FP=CONFIG_open(file,"w")) != NULL);

    for( item=0; ((item < count) && ok); item++ )
    {
        if( (flag != CONFIG_FLAG_NONE) && ((cnfg[item].flag & flag) == 0) )
        {
            continue;
        }

        if( !CONFIG_putvar(FP,&cnfg[item]) )
        {
            CONFIG_errorf("CONFIG_write(...) %s Cannot write to file.\n",file);
            ok = FALSE;
        }
    }

    CONFIG_close(FP);

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_write( struct CONFIG_variable cnfg[], char *file )
{
    return(CONFIG_write(cnfg,file,CONFIG_FLAG_NONE));
}

/******************************************************************************/

BOOL    CONFIG_read( char *file )
{
    return(CONFIG_read(CONFIG_cnfg,file));
}

/******************************************************************************/

BOOL    CONFIG_write( char *file, int flag )
{
    return(CONFIG_write(CONFIG_cnfg,file,flag));
}

/******************************************************************************/

BOOL    CONFIG_write( char *file )
{
    return(CONFIG_write(CONFIG_cnfg,file));
}

/******************************************************************************/

BOOL    CONFIG_environment( struct CONFIG_variable cnfg[] )
{
BOOL    ok;
STRING  buff;
DWORD   size;
int     item;

    // Check if API started running...
    if( !CONFIG_API_check() )
    {
        return(FALSE);
    }

    // Loop through list to process each variable ...
    for( ok=TRUE,item=0; !CONFIG_EoT(cnfg,item); item++ )
    {
        if( (size=GetEnvironmentVariable(cnfg[item].name,buff,STRLEN)) == 0L )
        {
            CONFIG_messgf("CONFIG_environment(...) %s Variable not found.\n",cnfg[item].name);
            continue;
        }

        CONFIG_debugf("%02d: %s=%s\n",item,cnfg[item].name,buff);

        ok = CONFIG_buff2cnfg(cnfg,item,buff);
        STR_printf(ok,CONFIG_debugf,CONFIG_errorf,"CONFIG_environment(...) %s=%s (%s).\n",cnfg[item].name,buff,STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_environment( void )
{
BOOL ok;

    ok = CONFIG_environment(CONFIG_cnfg);

    return(ok);
}

/******************************************************************************/

BOOL    CONFIG_list2cnfg( struct CONFIG_variable cnfg[], char *text )
{
BOOL    ok=TRUE;
char  **token;
int     count,item;

    token = STR_tokens(text,count);

    for( ok=TRUE,item=0; (!CONFIG_EoT(cnfg,item) && (item < count) && ok); item++ )
    {
        ok = CONFIG_buff2cnfg(cnfg,item,token[item]);
    }

    return(ok);
}

/******************************************************************************/


char   *CONFIG_cnfg2list( struct CONFIG_variable cnfg[] )
{
int     item;
char   *buff;

    // Allocate a local buffer... (V1.2)
    buff = CONFIG_Buff();

    for( item=0; !CONFIG_EoT(cnfg,item); item++ )
    {
        if( item > 0 )
        {
            strncat(buff,STR_DELIMETER,CONFIG_BUFF_SIZE);
        }

        strncat(buff,CONFIG_cnfg2str(&cnfg[item]),CONFIG_BUFF_SIZE);
    }

    return(buff);
}

/******************************************************************************/

void    TABLE_vptr( struct CONFIG_variable cnfg[], int func )
{
static  void  *vptr[CONFIG_CNFG];
int     item;

    for( item=0; !CONFIG_EoT(cnfg,item); item++ )
    {
        switch( func )
        {
            case TABLE_VPTR_SAVE :
               vptr[item] = cnfg[item].vptr;
               break;

            case TABLE_VPTR_RESTORE :
               cnfg[item].vptr = vptr[item];
               break;
         }
    }
}

/******************************************************************************/

void    TABLE_vptr( struct CONFIG_variable cnfg[], int type, int size, int line )
{
int     item,offset=0;
char   *bptr;

    for( item=0; !CONFIG_EoT(cnfg,item); item++ )
    {
        bptr = (char *)cnfg[item].vptr;

        switch( type )
        {
            case TABLE_STRUCTURE :
               offset = line * size;
               break;

            case TABLE_ARRAY :
               offset = line * CONFIG_typesize[cnfg[item].type];
               break;
        }

        cnfg[item].vptr = &bptr[offset];
    }
}

/******************************************************************************/

int     TABLE_read( char *file, struct CONFIG_variable cnfg[], int type, int size, int max )
{
BOOL    ok=TRUE,EofF=FALSE;
FILE   *FP=NULL;
int     line=0;
char   *buff;

    if( !CONFIG_API_check() )               // Check if API started okay...
    {
        return(FALSE);
    }

    TABLE_vptr(cnfg,TABLE_VPTR_SAVE);

    ok = ((FP=CONFIG_open(file,"r")) != NULL);

    while( ok && !EofF )
    {
        // Allocate a local buffer... (V1.2)
        buff = CONFIG_Buff();

        if( fscanf(FP,"%s\n",buff) == EOF )
        {
            EofF = TRUE;
            continue;
        }

        if( line >= max )
        {
            CONFIG_errorf("TABLE_read(...) %s[%d] Too many lines (max %d).\n",file,line,max);
            ok = FALSE;
            continue;
        }

        TABLE_vptr(cnfg,type,size,line);

        line++;
        CONFIG_debugf("%s[%02d] %s\n",file,line,buff);

        if( !CONFIG_list2cnfg(cnfg,buff) )
        {
            CONFIG_errorf("TABLE_read(...) %s[%d] Cannot process line.\n",file,line);
            ok = FALSE;
        }
        else
        {
            CONFIG_debugf("%s[XX] %s\n",file,CONFIG_cnfg2list(cnfg));
        }

        TABLE_vptr(cnfg,TABLE_VPTR_RESTORE);
    } 

    CONFIG_close(FP);

    return(ok ? line : TABLE_ERROR);
}

/******************************************************************************/

int TABLE_read( char *file, struct CONFIG_variable cnfg[], int size, int max )
{
int line;

    line = TABLE_read(file,cnfg,TABLE_STRUCTURE,size,max);

    return(line);
}

/******************************************************************************/

int TABLE_read( char *file, struct CONFIG_variable cnfg[], int max )
{
int line;

    line = TABLE_read(file,cnfg,TABLE_ARRAY,0,max);

    return(line);
}

/******************************************************************************/

int TABLE_read( char *file, int type, int size, int max )
{
int line;

    line = TABLE_read(file,CONFIG_cnfg,type,size,max);

    return(line);
}

/******************************************************************************/

int TABLE_read( char *file, int size, int max )
{
int line;

    line = TABLE_read(file,TABLE_STRUCTURE,size,max);

    return(line);
}

/******************************************************************************/

int TABLE_read( char *file, int max )
{
int line;

    line = TABLE_read(file,TABLE_ARRAY,0,max);

    return(line);
}

/******************************************************************************/

BOOL TABLE_write( char *file, struct CONFIG_variable cnfg[], int type, int size, int lines )
{
BOOL ok=TRUE;
FILE *FP=NULL;
int line;

    if( !CONFIG_API_check() )               // Check if API started okay...
    {
        return(FALSE);
    }

    TABLE_vptr(cnfg,TABLE_VPTR_SAVE);

    ok = ((FP=CONFIG_open(file,"w")) != NULL);

    for( line=0; ((line < lines) && ok); line++ )
    {
        TABLE_vptr(cnfg,type,size,line);

        if( fprintf(FP,"%s\n",CONFIG_cnfg2list(cnfg)) <= 0 )
        {
            CONFIG_errorf("TABLE_write(...) %s Cannot write to file.\n",file);
            ok = FALSE;
        }

        TABLE_vptr(cnfg,TABLE_VPTR_RESTORE);
    }

    CONFIG_close(FP);

    return(ok);
}

/******************************************************************************/

BOOL TABLE_write( char *file, struct CONFIG_variable cnfg[], int size, int lines )
{
BOOL ok; 

    ok = TABLE_write(file,cnfg,TABLE_STRUCTURE,size,lines);

    return(ok);
}

/******************************************************************************/

BOOL TABLE_write( char *file, struct CONFIG_variable cnfg[], int lines )
{
BOOL ok;

    ok = TABLE_write(file,cnfg,TABLE_ARRAY,0,lines);

    return(ok);
}

/******************************************************************************/

BOOL TABLE_write( char *file, int type, int size, int lines )
{
BOOL ok;

    ok = TABLE_write(file,CONFIG_cnfg,type,size,lines);

    return(ok);
}

/******************************************************************************/

BOOL TABLE_write( char *file, int size, int lines )
{
BOOL ok; 

    ok = TABLE_write(file,CONFIG_cnfg,TABLE_STRUCTURE,size,lines);

    return(ok);
}

/******************************************************************************/

BOOL TABLE_write( char *file, int lines )
{
BOOL ok;

    ok = TABLE_write(file,CONFIG_cnfg,TABLE_ARRAY,0,lines);

    return(ok);
}

/******************************************************************************/

void TABLE_list( struct CONFIG_variable cnfg[], int type, int size, int lines, PRINTF prnf )
{
int line;

    TABLE_vptr(cnfg,TABLE_VPTR_SAVE);

    for( line=0; (line < lines); line++ )
    {
        TABLE_vptr(cnfg,type,size,line);
      (*prnf)("%s\n",CONFIG_cnfg2list(cnfg));
        TABLE_vptr(cnfg,TABLE_VPTR_RESTORE);
    }
}

/******************************************************************************/

void TABLE_list( struct CONFIG_variable cnfg[], int size, int lines, PRINTF prnf )
{
    TABLE_list(cnfg,TABLE_STRUCTURE,size,lines,prnf);
}

/******************************************************************************/

void TABLE_list( struct CONFIG_variable cnfg[], int lines, PRINTF prnf )
{
    TABLE_list(cnfg,TABLE_ARRAY,0,lines,prnf);
}

/******************************************************************************/

void TABLE_list( int type, int size, int lines, PRINTF prnf )
{
    TABLE_list(CONFIG_cnfg,type,size,lines,prnf);
}

/******************************************************************************/

void TABLE_list( int size, int lines, PRINTF prnf )
{
    TABLE_list(CONFIG_cnfg,TABLE_STRUCTURE,size,lines,prnf);
}

/******************************************************************************/

void TABLE_list( int lines, PRINTF prnf )
{
    TABLE_list(CONFIG_cnfg,TABLE_ARRAY,0,lines,prnf);
}

/******************************************************************************/
/* Load specific environment variables (configuration style).          (V1.1) */
/******************************************************************************/

STRING  OperatingSystem="";
STRING  UserName="";
STRING  UserDomain="";
STRING  ComputerName="";
STRING  CalibrationPath="";

/******************************************************************************/

BOOL    CONFIG_EnvironmentLoad( void )
{
BOOL    ok;

    CONFIG_reset();

    CONFIG_set("OS",OperatingSystem);
    CONFIG_set("USERNAME",UserName);
    CONFIG_set("USERDOMAIN",UserDomain);
    CONFIG_set("COMPUTERNAME",ComputerName);
    CONFIG_set("CALIBRATIONPATH",CalibrationPath);

    ok = CONFIG_environment();
    STR_printf(ok,CONFIG_debugf,CONFIG_messgf,"CONFIG_LoadEnvironment() %s.\n",STR_OkFailed(ok));
    CONFIG_list(CONFIG_debugf);

    return(ok);
}

/******************************************************************************/

char   *CONFIG_OperatingSystem( void )
{
    if( !CONFIG_API_check() )
    {
        return(NULL);
    }

    return(OperatingSystem);
}

/******************************************************************************/

char   *CONFIG_UserName( void )
{
    if( !CONFIG_API_check() )
    {
        return(NULL);
    }

    return(UserName);
}

/******************************************************************************/

char   *CONFIG_UserDomain( void )
{
    if( !CONFIG_API_check() )
    {
        return(NULL);
    }

    return(UserDomain);
}

/******************************************************************************/

char   *CONFIG_ComputerName( void )
{
    if( !CONFIG_API_check() )
    {
        return(NULL);
    }

    return(ComputerName);
}

/******************************************************************************/

char   *CONFIG_CalibrationPath( void )
{
    if( !CONFIG_API_check() )
    {
        return(NULL);
    }

    return(CalibrationPath);
}

/******************************************************************************/

