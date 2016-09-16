#ifndef STR_H
#define STR_H

/******************************************************************************/

int     STR_messgf( const char *mask, ... );
int     STR_errorf( const char *mask, ... );
int     STR_debugf( const char *mask, ... );

/******************************************************************************/

char *stristr( char *string, char *strCharSet );

/******************************************************************************/

#define STR_TEXT           512          // Size of general text buffer.
#define STR_VARG            32          // Maximum number of variable arguments.
#define STR_NOTFOUND        -1          // Not Found value.
#define STR_BUFF_SIZE    65536          // String buffer size.

/******************************************************************************/

BOOL    STR_BuffInit( void );
void    STR_BuffUnit( void );

char   *STR_Buff( int size );
char   *STR_BuffCopy( char *str );

/******************************************************************************/

struct  STR_TextItem
{
    int   code;
    char *text;
};

#define STR_TEXT_EOT         -9999               // End Of Table value.
#define STR_TEXT_ENDOFTABLE  STR_TEXT_EOT,NULL   // End Of Table record.

/******************************************************************************/

BOOL    STR_TextEoT( struct STR_TextItem xlat[], int item );
char   *STR_TextCode( struct STR_TextItem xlat[], int code );
char   *STR_TextFlag( struct STR_TextItem xlat[], int flag );
int     STR_TextIndex( struct STR_TextItem xlat[], int code );
int     STR_TextIndex( struct STR_TextItem xlat[], char *text );
int     STR_TextCode( struct STR_TextItem xlat[], char *text );
BOOL    STR_TextCode( struct STR_TextItem xlat[], int &code, char *text );
int     STR_TextWidth( struct STR_TextItem xlat[] );
void    STR_TextPrint( struct STR_TextItem xlat[], PRINTF prnf );
char   *STR_TextList( struct STR_TextItem xlat[], char *separator, char *terminator );
char   *STR_TextList( struct STR_TextItem xlat[] );

/******************************************************************************/

char   *STR_Bool( BOOL flag, int type );
#define STR_BOOL_YES_NO      0
#define STR_BOOL_TRUE_FALSE  1
#define STR_BOOL_ON_OFF      2
#define STR_BOOL_OK_FAILED   3
#define STR_BOOL_LOCK_UNLOCK 4

char   *STR_YesNo( BOOL flag );
char   *STR_TrueFalse( BOOL flag );
char   *STR_OnOff( BOOL flag );
char   *STR_OkFailed( BOOL flag );
char   *STR_LockUnlock( BOOL flag );

BOOL    STR_Bool( char *text );

/******************************************************************************/

struct  STR_FlagItem
{
    char      code;
    char     *name;
    BOOL      flag;
};

#define STR_FLAG_EOT                   0
#define STR_FLAG_ENDOFTABLE            STR_FLAG_EOT,"",NULL
#define STR_FLAG_INVALID              -1

#define STR_FLAG_TRUE                  0x01
#define STR_FLAG_FALSE                 0x00
#define STR_FLAG_BOOL                  0x01

#define STR_FLAG_INCLUSIVE             0x00
#define STR_FLAG_EXCLUSIVE             0x02
#define STR_FLAG_MODE                  0x02

#define STR_FLAG_INCLUSIVE_TRUE        STR_FLAG_INCLUSIVE | STR_FLAG_TRUE
#define STR_FLAG_INCLUSIVE_FALSE       STR_FLAG_INCLUSIVE | STR_FLAG_FALSE
#define STR_FLAG_EXCLUSIVE_TRUE        STR_FLAG_EXCLUSIVE | STR_FLAG_TRUE
#define STR_FLAG_EXCLUSIVE_FALSE       STR_FLAG_EXCLUSIVE | STR_FLAG_FALSE

/******************************************************************************/

int     STR_FlagFind( struct STR_FlagItem list[], char code );
int     STR_FlagFind( struct STR_FlagItem list[], BOOL flag );
void    STR_FlagData( struct STR_FlagItem list[], int item, BOOL flag );
void    STR_FlagData( struct STR_FlagItem list[], BOOL flag );
BOOL    STR_FlagCode( struct STR_FlagItem list[], char code, int flag );
BOOL    STR_FlagList( struct STR_FlagItem list[], char *code, BOOL flag );
BOOL    STR_Flag( struct STR_FlagItem list[], int item );
BOOL    STR_Flag( struct STR_FlagItem list[], char code );

/******************************************************************************/

#define STR_NULLTERM    -1                  // Flag for null terminated string.

int     STR_length( char *str, int len );
int     STR_trimlength( char *str, int len );
char   *STR_stringf( char *mask, ... );
int     STR_trim( char *str, int len );
int     STR_strip( char *str, int len );
char   *STR_pack( char *str, int len );
char  **STR_varg( short argc, ... );
BOOL    STR_memnull( char *str, int len );
BOOL    STR_null( char *str, int len );
int     STR_trimCRLF( char *str, int len );

int     STR_length( char *str);
int     STR_trimlength( char *str);
int     STR_trim( char *str );
int     STR_strip( char *str );
char   *STR_pack( char *str );
BOOL    STR_null( char *str );
int     STR_trimCRLF( char *str );

char   *STR_left( char *str, int len );
char   *STR_right( char *str, int len );

int     STR_count( STRING str[], int max );

/******************************************************************************/

#define STR_TOKENS      128            // Maximum number of tokens in a string.
#define STR_DELIMETERS  ", \t\n"       // Default delimeter list.
#define STR_DELIMETER   ","            // Default delimeter.

char  **STR_tokens( char *buff, char *delimeters, int &tokens );
char  **STR_tokens( char *buff, int &tokens );

/******************************************************************************/

int     STR_printf( BOOL ok, PRINTF TRUEprnf, PRINTF FALSEprnf, const char *mask, ... );
int     STR_printf( PRINTF prnf, const char *mask, ... );

/******************************************************************************/

char   *STR_hex( void *data, int size );

/******************************************************************************/


#endif