/******************************************************************************/

#define MODULE_NAME     "STR"
#define MODULE_TEXT     "String API"
#define MODULE_DATE     "18/03/2001"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL    1

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

PRINTF  STR_PRN_messgf=NULL;                     // General messages printf function.
PRINTF  STR_PRN_errorf=printf;                   // Error messages printf function.
PRINTF  STR_PRN_debugf=printf;                   // Debug information printf function.

/******************************************************************************/
//* API print functions for different message types...                         */
/******************************************************************************/

int     STR_messgf( const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(STR_PRN_messgf,buff));
}


/******************************************************************************/

int     STR_errorf( const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(STR_PRN_errorf,buff));
}

/******************************************************************************/

int     STR_debugf( const char *mask, ... )
{
va_list args;
char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(STR_PRN_debugf,buff));
}

/******************************************************************************/

char *stristr( char *string, char *strCharSet )
{
char *s1,*s2,*s;

    s1 = STR_BuffCopy(string);
    s2 = STR_BuffCopy(strCharSet);

    s = strstr(_strupr(s1),_strupr(s2));

    return(s);
}

/******************************************************************************/

char    STR_BuffArea[STR_BUFF_SIZE];   // String buffer area.
int     STR_BuffPosn = 0;              // Current position in string buffer.

/******************************************************************************/

void    STR_BuffUnit( void )
{
/*  if( STR_BuffArea != NULL )
    {
        free(STR_BuffArea);
        STR_BuffArea = NULL;
    } */
}

/******************************************************************************/

BOOL    STR_BuffInit( void )
{
BOOL    ok=TRUE;

/*  if( STR_BuffArea != NULL )
    {
        return(ok);
    }

    if( (STR_BuffArea=(char *)malloc(STR_BUFF_SIZE)) != NULL )
    {
        STR_debugf("STR_BuffInit() %d bytes allocated.\n",STR_BUFF_SIZE);
        ATEXIT_API(STR_BuffUnit);
    }
    else
    {
        STR_errorf("STR_Buffer(...) Cannot allocate string buffer.\n");
        ok = FALSE;
    } */

    return(ok);
}

/******************************************************************************/

char   *STR_Buff( int size )
{
int     posn;

//  Make sure buffer is initialized...
    if( !STR_BuffInit() )
    {
        return(NULL);
    }

//  Check size is not larger than buffer...
    if( size > STR_BUFF_SIZE )
    {
        STR_errorf("STR_Buff(size=%d) Cannot allocate buffer.\n");
        return(NULL);
    }

//  Allow for NULL termination...
    size++;

//  Wrap back to start if we're at the end of the buffer...
    if( (STR_BuffPosn+size) >= STR_BUFF_SIZE )
    {
        STR_BuffPosn = 0;
    }

//  Work out new position...
    posn = STR_BuffPosn;
    STR_BuffPosn += size;

//  Clear the buffer...
    memset(&STR_BuffArea[posn],0x00,size);

    return(&STR_BuffArea[posn]);
}

/******************************************************************************/

char   *STR_BuffCopy( char *str )
{
char   *buf;
int     len;

    len = strlen(str);

    if( (buf=STR_Buff(len)) == NULL )            // String buffer allocation...
    {
        return(NULL);
    }

    strcpy(buf,str);

    return(buf);
}

/******************************************************************************/
/* Text translation functions associates tables of strings with integers...   */
/******************************************************************************/

struct  STR_TextItem    STR_TextYesNo[]      = { { TRUE ,"Yes"  },{ FALSE,"No"     },{ STR_TEXT_ENDOFTABLE } };
struct  STR_TextItem    STR_TextTrueFalse[]  = { { TRUE ,"True" },{ FALSE,"False"  },{ STR_TEXT_ENDOFTABLE } };
struct  STR_TextItem    STR_TextOnOff[]      = { { TRUE ,"On"   },{ FALSE,"Off"    },{ STR_TEXT_ENDOFTABLE } };
struct  STR_TextItem    STR_TextOkFailed[]   = { { TRUE ,"OK"   },{ FALSE,"Failed" },{ STR_TEXT_ENDOFTABLE } };
struct  STR_TextItem    STR_TextLockUnlock[] = { { TRUE ,"Lock" },{ FALSE,"Unlock" },{ STR_TEXT_ENDOFTABLE } };

struct  STR_TextItem   *STR_TextBool[] = { STR_TextYesNo,STR_TextTrueFalse,STR_TextOnOff,STR_TextOkFailed,STR_TextLockUnlock,NULL };

/******************************************************************************/

BOOL    STR_TextEoT( struct STR_TextItem xlat[], int item )
{
BOOL    EoT;

    EoT = (xlat[item].code == STR_TEXT_EOT);

    return(EoT);
}

/******************************************************************************/

char   *STR_TextCode( struct STR_TextItem xlat[], int code )
{
char   *buff;
int     item;

    // String buffer allocation...
    if( (buff=STR_Buff(STRLEN)) == NULL )         
    {
        return(NULL);
    }

    for( item=0; (!STR_TextEoT(xlat,item) && (xlat[item].code != code)); item++ );

    if( STR_TextEoT(xlat,item) )
    {
        strncpy(buff,STR_stringf("UNKNOWN [%d]",code),STRLEN);
    }
    else
    {
        strncpy(buff,xlat[item].text,STRLEN);
    }

    return(buff);
}

/******************************************************************************/

int     STR_TextCode( struct STR_TextItem xlat[], char *text )
{
int     item,code=STR_NOTFOUND;

    if( (item=STR_TextIndex(xlat,text)) != STR_NOTFOUND )
    {
        code = xlat[item].code;
    }

    return(code);
}

/******************************************************************************/

BOOL STR_TextCode( struct STR_TextItem xlat[], int &code, char *text )
{
BOOL ok=FALSE;
int i;

    if( STR_null(text) )
    {
        return(TRUE);
    }

    if( (i=STR_TextCode(xlat,text)) != STR_NOTFOUND )
    {
        ok = TRUE;
        code = i;
    }

    return(ok);
}

/******************************************************************************/

int     STR_TextIndex( struct STR_TextItem xlat[], int code )
{
int     item,find;

    for( find=STR_NOTFOUND,item=0; !STR_TextEoT(xlat,item); item++ )
    {
        if( xlat[item].code == code )
        {
            find = item;
            break;
        }
    }

    return(find);
}


/******************************************************************************/

int     STR_TextIndex( struct STR_TextItem xlat[], char *text )
{
int     item,find;

    for( find=STR_NOTFOUND,item=0; !STR_TextEoT(xlat,item); item++ )
    {
        if( _stricmp(xlat[item].text,text) == 0 )
        {
            find = item;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

int     STR_TextWidth( struct STR_TextItem xlat[] )
{
int     item,width,w;

    for( width=0,item=0; !STR_TextEoT(xlat,item); item++ )
    {
        if( (w=strlen(xlat[item].text)) > width )
        {
            width = w;
        }
    }

    return(width);
}

/******************************************************************************/

void    STR_TextPrint( struct STR_TextItem xlat[], PRINTF prnf )
{
int     item,width;

    width = STR_TextWidth(xlat);

    for( item=0; !STR_TextEoT(xlat,item); item++ )
    {
      (*prnf)("%02d: %-*.*s %02d\n",
              item,   
              width,width,xlat[item].text,
              xlat[item].code);
    }
}

/******************************************************************************/

char   *STR_TextList( struct STR_TextItem xlat[], char *separator, char *terminator )
{
char   *buff;
int     item;

    if( (buff=STR_Buff(STR_TEXT)) == NULL )      // String buffer allocation...
    {
        return(NULL);
    }

    for( item=0; !STR_TextEoT(xlat,item); item++ )
    {
        if( (strlen(buff) > 0) && (separator != NULL) )
        {
            strncat(buff,separator,STR_TEXT);
        }

        strncat(buff,xlat[item].text,STR_TEXT);
    }

    if( (strlen(buff) > 0) && (terminator != NULL) )
    {
        strncat(buff,terminator,STR_TEXT);
    }

    return(buff);
}

/******************************************************************************/

char   *STR_TextList( struct STR_TextItem xlat[] )
{
    return(STR_TextList(xlat,",",NULL));
}

/******************************************************************************/

char   *STR_TextFlag( struct STR_TextItem xlat[], int flag )
{
char   *buff;
BOOL    find;
int     item;

    if( (buff=STR_Buff(STR_TEXT)) == NULL )      // String buffer allocation...
    {
        return(NULL);
    }

    for( item=0; !STR_TextEoT(xlat,item); item++ )
    {
        if( (xlat[item].code & flag) == 0 )
        {
            continue;
        }

        if( strlen(buff) )
        {
            strncat(buff," ",STR_TEXT);
        }

        strncat(buff,xlat[item].text,STR_TEXT);
    }

    return(buff);
}

/******************************************************************************/

char    *STR_Bool( BOOL flag, int type )
{
    return(STR_TextCode(STR_TextBool[type],(int)flag));
}

/******************************************************************************/

char    *STR_YesNo( BOOL flag )
{
    return(STR_Bool(flag,STR_BOOL_YES_NO));
}

/******************************************************************************/

char    *STR_TrueFalse( BOOL flag )
{
    return(STR_Bool(flag,STR_BOOL_TRUE_FALSE));
}

/******************************************************************************/

char    *STR_OnOff( BOOL flag )
{
    return(STR_Bool(flag,STR_BOOL_ON_OFF));
}

/******************************************************************************/

char    *STR_OkFailed( BOOL flag )
{
    return(STR_Bool(flag,STR_BOOL_OK_FAILED));
}

/******************************************************************************/

char    *STR_LockUnlock( BOOL flag )
{
    return(STR_Bool(flag,STR_BOOL_LOCK_UNLOCK));
}

/******************************************************************************/

BOOL     STR_Bool( char *text )
{
BOOL     ok;
int      type,code;

    for( ok=FALSE,type=0; (STR_TextBool[type] != NULL); type++ )
    {
        if( (code=STR_TextCode(STR_TextBool[type],text)) != STR_TEXT_EOT )
        {
            ok = (BOOL)code;
            break;
        }
    }

    return(ok);
}
/******************************************************************************/

BOOL    STR_FlagEoT( struct STR_FlagItem list[], int item )
{
BOOL    EoT;

    EoT = (list[item].code == STR_FLAG_EOT);

    return(EoT);
}

/******************************************************************************/

int     STR_FlagFind( struct STR_FlagItem list[], char code )
{
int     item,find;

    for( find=STR_FLAG_INVALID,item=0; !STR_FlagEoT(list,item); item++ )
    {
        if( list[item].code == code )
        {
            find = item;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

int     STR_FlagFind( struct STR_FlagItem list[], BOOL flag )
{
int     item,find;

    for( find=STR_FLAG_INVALID,item=0; !STR_FlagEoT(list,item); item++ )
    {
        if( list[item].flag == flag )
        {
            find = item;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

void    STR_FlagData( struct STR_FlagItem list[], int item, BOOL flag )
{
    list[item].flag = flag;
}

/******************************************************************************/

void    STR_FlagData( struct STR_FlagItem list[], BOOL flag )
{
int     item;

    for( item=0; !STR_FlagEoT(list,item); STR_FlagData(list,item++,flag) );
}

/******************************************************************************/

BOOL    STR_FlagCode( struct STR_FlagItem list[], char code, int flag )
{
BOOL    ok=FALSE;
int     item;

    if( (item=STR_FlagFind(list,code)) != STR_FLAG_INVALID )
    {
        if( flag && STR_FLAG_EXCLUSIVE )
        {
            STR_FlagData(list,(flag & STR_FLAG_BOOL) == STR_FLAG_FALSE);
        }

        STR_FlagData(list,item,(flag & STR_FLAG_BOOL) == STR_FLAG_TRUE);

        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    STR_FlagList( struct STR_FlagItem list[], char *code, BOOL flag )
{
BOOL    ok;
int     len,item;

    for( ok=TRUE,len=strlen(code),item=0; ((item < len) && ok); item++ )
    {
        ok = STR_FlagCode(list,code[item],flag);
    }

    return(ok);
}

/******************************************************************************/

BOOL    STR_Flag( struct STR_FlagItem list[], int item )
{
    return(list[item].flag);
}

/******************************************************************************/

BOOL    STR_Flag( struct STR_FlagItem list[], char code )
{
BOOL    flag=FALSE;
int     item;

    if( (item=STR_FlagFind(list,code)) != STR_FLAG_INVALID )
    {
        flag = STR_Flag(list,item);
    }

    return(flag);
}

/******************************************************************************/
/* General purpose string functions...                                        */
/******************************************************************************/

int     STR_length( char *str, int len )
{
    if( str == NULL )
    {
        return(0);
    }

    if( len == STR_NULLTERM )
    {
        len = strlen(str);
    }

    return(len);
}

/******************************************************************************/

int     STR_length( char *str )
{
    return(STR_length(str,STR_NULLTERM));
}

/******************************************************************************/

int     STR_trimlength( char *str, int len )
{
int     l;

    len = STR_length(str,len);

    for( l=len-1; ((l >= 0) && ((str[l] == SPACE) || (str[l] == NUL))); l-- );

    return(l+1);
}

/******************************************************************************/

int     STR_trimlength( char *str )
{
    return(STR_trimlength(str,STR_NULLTERM));
}

/******************************************************************************/

char   *STR_stringf( char *mask, ... )
{
char   *buff;
va_list arg_ptr;

    if( (buff=STR_Buff(STR_TEXT)) == NULL )      // String buffer allocation...
    {
        return(NULL);
    }

    va_start(arg_ptr,mask);
    vsprintf(buff,mask,arg_ptr);
    va_end(arg_ptr);

    return(buff);
}

/*****************************************************************************/

int     STR_trim( char *str, int len )
{
int     l;

    len = STR_length(str,len);

    for( l=len-1; ((l >= 0) && ((str[l] == SPACE) || (str[l] == NUL))); l-- )
    {
        str[l] = NUL;
    }

    return(l+1);
}

/*****************************************************************************/

int     STR_trim( char *str )
{
    return(STR_trim(str,STR_NULLTERM));
}

/*****************************************************************************/

int     STR_strip( char *str, int len )
{
int     beg,end;
char   *buf;

    if( (buf=STR_Buff(len=STR_length(str,len))) == NULL )
    {
        return(0);
    }

    for( beg=0;     ((beg <  len) && (str[beg] == SPACE)); beg++ );
    for( end=len-1; ((end >= 0  ) && (str[end] == SPACE)); end-- );

    memcpy(buf,&str[beg],(end-beg)+1);
    memcpy(str,buf,len);

    return((end-beg)+1);
}

/*****************************************************************************/

int     STR_strip( char *str )
{
    return(STR_strip(str,STR_NULLTERM));
}

/*****************************************************************************/

char   *STR_pack( char *str, int len )
{
int     s,d;
char   *buf;

    if( (buf=STR_Buff(len=STR_length(str,len))) == NULL )
    {
        return(0);
    }

    for( d=0,s=0; (s < len); s++ )
    {
        if( str[s] != SPACE )
        {
            buf[d++] = str[s];
        }
    }

    return(buf);
}

/*****************************************************************************/

char   *STR_pack( char *str )
{
    return(STR_pack(str,STR_NULLTERM));
}

/*****************************************************************************/

char  **STR_varg( short argc, ... )
{
static  char   *array[STR_VARG+1];
va_list arg_ptr;
int     i;

    if( argc > STR_VARG )
    {
        return(NULL);
    }

    va_start(arg_ptr,argc);

    for( i=0; (i < argc); i++ )
    {
        array[i] = va_arg(arg_ptr,char *);
    }

    va_end(arg_ptr);

    array[i] = NULL;

    return(array);
}

/*****************************************************************************/

BOOL    STR_memnull( char *str, int len )
{
BOOL    null;
int     i;

    if( str == NULL )
    {
        return(TRUE);
    }

    for( null=TRUE,i=0; (null && (i < len)); null=(str[i++] == NUL) );

    return(null);
}

/*****************************************************************************/

BOOL    STR_null( char *str, int len )
{
    if( str == NULL )
    {
        return(TRUE);
    }

    len = STR_trim(str,len);
    return(STR_memnull(str,len));
}

/*****************************************************************************/

BOOL    STR_null( char *str )
{
    return(STR_null(str,STR_NULLTERM));
}

/*****************************************************************************/

int     STR_trimCRLF( char *str, int len )
{
    len = STR_length(str,len);

    while( len > 0 )
    {
        len--;

        if( (str[len] == CR) || (str[len] == LF) )
        {
            str[len] = NUL;
        }
        else
        {
            break;
        }
    }

    return(len);
}

/*****************************************************************************/

int     STR_trimCRLF( char *str )
{
    return(STR_trimCRLF(str,STR_NULLTERM));
}

/*****************************************************************************/

char   *STR_left( char *str, int len )
{
char   *buf;

    buf = STR_Buff(len+1);
    memcpy(buf,str,len);
    return(buf);
}
/*****************************************************************************/

char   *STR_right( char *str, int len )
{
char   *buf;
int     l;

    l = strlen(str);
    if( len > l ) len = l;
    buf = STR_Buff(len+1);
    memcpy(buf,&str[l-len],len);
    return(buf);
}

/*****************************************************************************/
/* String parsing functions...                                               */
/*****************************************************************************/

char  **STR_tokens( char *buff, char *delimeters, int &tokens )
{
static  char  *token[STR_TOKENS+1];
char   *bcopy;
int     count=0;

    bcopy = STR_BuffCopy(buff);
    token[count] = strtok(bcopy,delimeters);

    while( token[count] != NULL )
    {
        if( ++count == STR_TOKENS )
        {
            token[count] = NULL;
            continue;
        }
    
        token[count] = strtok(NULL,delimeters);
    }

    tokens = count;
    return(token);
}

/******************************************************************************/

char  **STR_tokens( char *buff, int &tokens )
{
    return(STR_tokens(buff,STR_DELIMETERS,tokens));
}

/******************************************************************************/

int     STR_printf( BOOL ok, PRINTF TRUEprnf, PRINTF FALSEprnf, const char *mask, ... )
{
va_list args;
char   *buff;
int     plen=0;
PRINTF  prnf;

    if( (prnf = ok ? TRUEprnf : FALSEprnf) == NULL )
    {
        return(0);
    }

    if( (buff=STR_Buff(PRNTBUFF)) == NULL )      // String buffer allocation...
    {
        return(0);
    }

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    plen = (*prnf)(buff);

    return(plen);
}

/******************************************************************************/

int     STR_printf( PRINTF prnf, const char *mask, ... )
{
va_list args;
char   *buff;
int     plen=0;

    if( prnf == NULL )
    {
        return(0);
    }

    if( (buff=STR_Buff(PRNTBUFF)) == NULL )      // String buffer allocation...
    {
        return(0);
    }

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    plen = (*prnf)(buff);

    return(plen);
}

/******************************************************************************/

char *STR_hex( void *data, int size )
{
BYTE *bptr;
int b,len,value;
char *buff;

    // String buffer allocation...
    if( (buff=STR_Buff(len=(1+(3*size)))) == NULL )      
    {
        return(NULL);
    }

    for( bptr=(BYTE *)data,b=0; (b < size); b++ )
    {
        value = (int)bptr[b];
        strncat(buff,STR_stringf("%02X ",value),len);
    }

    return(buff);
}

/******************************************************************************/

int STR_count( STRING str[], int max )
{
int count;

    for( count=0; (count < max); count++ )
    {
        if( STR_null(str[count]) )
        {
            break;
        }
    }

    return(count);
}

/******************************************************************************/

