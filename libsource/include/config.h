/******************************************************************************/
/*                                                                            */
/* MODULE  : CONFIG.h                                                         */
/*                                                                            */
/* PURPOSE : Configuration file I/O functions.                                */
/*                                                                            */
/* DATE    : 18/Sep/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 18/Sep/2000 - Initial Development of module.                     */
/*                                                                            */
/* V1.2  JNI 28/Feb/2002 - Allocate buffer space from a fixed pool.           */
/*                                                                            */
/* V1.3  JNI 15/Jun/2002 - Changed parsing to allow spaces in data.           */
/*                                                                            */
/* V1.4  JNI 26/May/2004 - Added matrix variable type.                        */
/*                                                                            */
/* V1.5  JNI 26/May/2004 - Added label variable type.                         */
/*                                                                            */
/******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

/******************************************************************************/

#define CONFIG_DEBUG() if( !CONFIG_API_start(printf,printf,printf) ) { printf("Cannot start CONFIG API.\n"); exit(0); }

/******************************************************************************/

#define VAR(N) #N,N

/******************************************************************************/

struct  CONFIG_variable                     // Configuration variable item...
{
    STRING name;                            // Variable name.
    STRING label;                           // Optional label.

    void   *vptr;                           // Pointer to variable.

    BYTE    type;                           // Variable type...
#define CONFIG_TYPE_NONE     0
#define CONFIG_TYPE_CHAR     1 
#define CONFIG_TYPE_INT      2
#define CONFIG_TYPE_LONG     3
#define CONFIG_TYPE_FLOAT    4
#define CONFIG_TYPE_DOUBLE   5
#define CONFIG_TYPE_STRING   6
#define CONFIG_TYPE_BOOL     7
#define CONFIG_TYPE_SHORT    8
#define CONFIG_TYPE_USHORT   9
#define CONFIG_TYPE_MATRIX  10
#define CONFIG_TYPE_DWORD   11

    int     flag;                           // Flags...
#define CONFIG_FLAG_NONE   0x00
#define CONFIG_FLAG_READ   0x01
#define CONFIG_FLAG_PARSE  0x02
#define CONFIG_FLAG_ARRAY  0x04
#define CONFIG_FLAG_HIDDEN 0x08
#define CONFIG_FLAG_LABEL  0x10

#define CONFIG_DIMENSIONS    2              // Maximum number of array dimensions.
    int     indx[CONFIG_DIMENSIONS+1];      // Array dimensions.
};

/******************************************************************************/

#define CONFIG_NONE          CONFIG_TYPE_NONE  ,CONFIG_FLAG_NONE
#define CONFIG_CHAR          CONFIG_TYPE_CHAR  ,CONFIG_FLAG_NONE
#define CONFIG_INT           CONFIG_TYPE_INT   ,CONFIG_FLAG_NONE
#define CONFIG_LONG          CONFIG_TYPE_LONG  ,CONFIG_FLAG_NONE
#define CONFIG_FLOAT         CONFIG_TYPE_FLOAT ,CONFIG_FLAG_NONE
#define CONFIG_DOUBLE        CONFIG_TYPE_DOUBLE,CONFIG_FLAG_NONE
#define CONFIG_STRING        CONFIG_TYPE_STRING,CONFIG_FLAG_NONE
#define CONFIG_BOOL          CONFIG_TYPE_BOOL  ,CONFIG_FLAG_NONE
#define CONFIG_SHORT         CONFIG_TYPE_SHORT ,CONFIG_FLAG_NONE
#define CONFIG_USHORT        CONFIG_TYPE_USHORT,CONFIG_FLAG_NONE
#define CONFIG_MATRIX        CONFIG_TYPE_MATRIX,CONFIG_FLAG_NONE

#define CONFIG_CHARS         CONFIG_TYPE_CHAR  ,CONFIG_FLAG_ARRAY
#define CONFIG_INTS          CONFIG_TYPE_INT   ,CONFIG_FLAG_ARRAY
#define CONFIG_LONGS         CONFIG_TYPE_LONG  ,CONFIG_FLAG_ARRAY
#define CONFIG_FLOATS        CONFIG_TYPE_FLOAT ,CONFIG_FLAG_ARRAY
#define CONFIG_DOUBLES       CONFIG_TYPE_DOUBLE,CONFIG_FLAG_ARRAY
#define CONFIG_STRINGS       CONFIG_TYPE_STRING,CONFIG_FLAG_ARRAY
#define CONFIG_BOOLS         CONFIG_TYPE_BOOL  ,CONFIG_FLAG_ARRAY
#define CONFIG_SHORTS        CONFIG_TYPE_SHORT ,CONFIG_FLAG_ARRAY
#define CONFIG_USHORTS       CONFIG_TYPE_USHORT,CONFIG_FLAG_ARRAY
#define CONFIG_MATRICES      CONFIG_TYPE_MATRIX,CONFIG_FLAG_ARRAY

/******************************************************************************/

//      End of Table marker.
#define CONFIG_ENDOFTABLE    { "","",NULL,CONFIG_TYPE_NONE }  

#define CONFIG_SEPARATOR     TAB                 // Variable name / data separator.
#define CONFIG_DELIMETER     ","

#define CONFIG_BUFF_POOL     10                  // Number of buffers. (V1.2)
#define CONFIG_BUFF_SIZE     1024                // Size of buffers. (V1.2)

#define CONFIG_NOTFOUND     -1                   // Item not found.
#define CONFIG_INVALID      -1                   // Invalid.

#define CONFIG_CNFG          128                 // Maximum item for internal array.

#define CONFIG_NULL_CHAR     0                   // NULL values for each type...
#define CONFIG_NULL_INT      0
#define CONFIG_NULL_LONG     0
#define CONFIG_NULL_FLOAT    0.0
#define CONFIG_NULL_DOUBLE   0.0
#define CONFIG_NULL_STRING   NULL
#define CONFIG_NULL_BOOL     FALSE
#define CONFIG_NULL_SHORT    0
#define CONFIG_NULL_USHORT   0

/******************************************************************************/

extern char  *CONFIG_typetext[];                 // List of variable type names.
extern int    CONFIG_typesize[];                 // List of variable type sizes.

/******************************************************************************/

int     CONFIG_messgf( const char *mask, ... );
int     CONFIG_errorf( const char *mask, ... );
int     CONFIG_debugf( const char *mask, ... );

/*****************************************************************************/

//      Allocate local buffer space. (V1.2)
char   *CONFIG_Buff( void );

BOOL    CONFIG_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    CONFIG_API_stop( void );
BOOL    CONFIG_API_check( void );

//      General file open / close functions...
FILE   *CONFIG_open( char *file, char *mode );
void    CONFIG_close( FILE *FP );

//      Returns TRUE if specified item is EndOfTable in configuration list...
BOOL    CONFIG_EoT( struct CONFIG_variable cnfg[], int item );
BOOL    CONFIG_EoT( int item );

//      Array processing stuff...
void    CONFIG_array( struct CONFIG_variable *cnfg );
void   *CONFIG_array( struct CONFIG_variable *cnfg, int indx );

//      Check items in configuration list (return count)...
int     CONFIG_check( struct CONFIG_variable cnfg[] );
BOOL    CONFIG_check( struct CONFIG_variable cnfg[], int &count );

//      Flag stuff...
void    CONFIG_flagON( struct CONFIG_variable cnfg[], int flag );
void    CONFIG_flagON( struct CONFIG_variable cnfg[], int item, int flag );
void    CONFIG_flagOFF( struct CONFIG_variable cnfg[], int flag );
void    CONFIG_flagOFF( struct CONFIG_variable cnfg[], int item, int flag );
BOOL    CONFIG_flag( struct CONFIG_variable cnfg[], int flag );
BOOL    CONFIG_flag( struct CONFIG_variable cnfg[], int item, int flag );
char   *CONFIG_flag( struct CONFIG_variable cnfg[], int item, int flag, char *on, char *off );

//      Flag stuff for default configuraiton list...
void    CONFIG_flagON( int flag );
void    CONFIG_flagON( int item, int flag );
void    CONFIG_flagOFF( int flag );
void    CONFIG_flagOFF( int item, int flag );
BOOL    CONFIG_flag( int flag );
BOOL    CONFIG_flag( int item, int flag );
char   *CONFIG_flag( int item, int flag, char *on, char *off );

BOOL    CONFIG_readflag( struct CONFIG_variable cnfg[], int item );
BOOL    CONFIG_readflag( int item );
BOOL    CONFIG_readflag( struct CONFIG_variable cnfg[], char *name );
BOOL    CONFIG_readflag( char *name );

//      Reset default configuration list...
void    CONFIG_reset( void );

//      Set default flag value...
void    CONFIG_defaultflags( int flags );

//      Clear variable data in configuration list...
void    CONFIG_clear( struct CONFIG_variable cnfg[] );
void    CONFIG_clear( void );

void    CONFIG_setlabel( char *label );

//      Generic add variable to default configuration list...
int     CONFIG_set( int type, int flag, char *name, void *vptr, int indx0, int indx1 );
int     CONFIG_set( int type, int flag, char *name, void *vptr, int indx0 );
int     CONFIG_set( int type, int flag, char *name, void *vptr );

int     CONFIG_set( int type, char *name, void *vptr, int indx0, int indx1 );
int     CONFIG_set( int type, char *name, void *vptr, int indx0 );
int     CONFIG_set( int type, char *name, void *vptr );

//      Add a variable of specific type to default configuraiton list...
int     CONFIG_set( char *name, int *data, int indx0 );
int     CONFIG_set( char *name, int &data );
int     CONFIG_set( char *name, long *data, int indx0 );
int     CONFIG_set( char *name, long &data );
int     CONFIG_set( char *name, float *data, int indx0 );
int     CONFIG_set( char *name, float &data );
int     CONFIG_set( char *name, double *data, int indx0 );
int     CONFIG_set( char *name, double &data );
int     CONFIG_set( char *name, STRING *data, int indx0 );
int     CONFIG_set( char *name, STRING data );
int     CONFIG_set( char *name, char *data, int indx0 );
int     CONFIG_set( char *name, char &data );
int     CONFIG_setBOOL( char *name, BOOL *data, int indx0 );
int     CONFIG_setBOOL( char *name, BOOL &data );
int     CONFIG_set( char *name, short *data, int indx0 );
int     CONFIG_set( char *name, short &data );
int     CONFIG_set( char *name, USHORT *data, int indx0 );
int     CONFIG_set( char *name, USHORT &data );
int     CONFIG_set( char *name, matrix *data, int indx0 );
int     CONFIG_set( char *name, matrix &data );

int     CONFIG_label( char *name, int *data, int indx0 );
int     CONFIG_label( char *name, int &data );
int     CONFIG_label( char *name, long *data, int indx0 );
int     CONFIG_label( char *name, long &data );
int     CONFIG_label( char *name, float *data, int indx0 );
int     CONFIG_label( char *name, float &data );
int     CONFIG_label( char *name, double *data, int indx0 );
int     CONFIG_label( char *name, double &data );
int     CONFIG_label( char *name, STRING *data, int indx0 );
int     CONFIG_label( char *name, STRING data );
int     CONFIG_label( char *name, char *data, int indx0 );
int     CONFIG_label( char *name, char &data );
int     CONFIG_labelBOOL( char *name, BOOL *data, int indx0 );
int     CONFIG_labelBOOL( char *name, BOOL &data );
int     CONFIG_label( char *name, short *data, int indx0 );
int     CONFIG_label( char *name, short &data );
int     CONFIG_label( char *name, USHORT *data, int indx0 );
int     CONFIG_label( char *name, USHORT &data );
int     CONFIG_label( char *name, matrix *data, int indx0 );
int     CONFIG_label( char *name, matrix &data );

//      Add's a label variable...
int     CONFIG_label( char *name );

//      Maximum width of variable names in configuration list...
int     CONFIG_width( struct CONFIG_variable cnfg[] );

int     CONFIG_type( int type, void *vptr );

//      Return printable text string for a configuration variable...
char   *CONFIG_text( struct CONFIG_variable cnfg[], int item, BOOL type );
char   *CONFIG_text( struct CONFIG_variable cnfg[], int item );

//      Print a list of configuration variable names and their values...
void    CONFIG_list( struct CONFIG_variable cnfg[], int flag, BOOL set, PRINTF prnf );
void    CONFIG_list( struct CONFIG_variable cnfg[], PRINTF prnf );
void    CONFIG_list( int flag, BOOL set, PRINTF prnf );
void    CONFIG_list( PRINTF prnf );
void    CONFIG_list( void );

//      Find the variable name in the configuration list...
int     CONFIG_lookup( struct CONFIG_variable cnfg[], char *name );
int     CONFIG_lookup( char *name );

//      Get and Put variable values from and to configuration variables...
BOOL    CONFIG_getvar( struct CONFIG_variable cnfg[], char *name, char *data );
BOOL    CONFIG_putvar( FILE *FP, struct CONFIG_variable *cnfg );

//      Return value for variable from its data buffer...
char    CONFIG_char  ( void *vptr );
int     CONFIG_int   ( void *vptr );
long    CONFIG_long  ( void *vptr );
float   CONFIG_float ( void *vptr );
double  CONFIG_double( void *vptr );
char   *CONFIG_string( void *vptr );
BOOL    CONFIG_bool  ( void *vptr );
short   CONFIG_short ( void *vptr );
USHORT  CONFIG_ushort( void *vptr );
matrix *CONFIG_matrix( void *vptr );

//      Return value for a variable from its name and configuration list...
char    CONFIG_char  ( struct CONFIG_variable cnfg[], char *name );
int     CONFIG_int   ( struct CONFIG_variable cnfg[], char *name );
long    CONFIG_long  ( struct CONFIG_variable cnfg[], char *name );
float   CONFIG_float ( struct CONFIG_variable cnfg[], char *name );
double  CONFIG_double( struct CONFIG_variable cnfg[], char *name );
char   *CONFIG_string( struct CONFIG_variable cnfg[], char *name );
BOOL    CONFIG_bool  ( struct CONFIG_variable cnfg[], char *name );
short   CONFIG_short ( struct CONFIG_variable cnfg[], char *name );
USHORT  CONFIG_ushort( struct CONFIG_variable cnfg[], char *name );
matrix *CONFIG_matrix( struct CONFIG_variable cnfg[], char *name );

//      Return value for a variable from its name using default configuration list...
char    CONFIG_char  ( char *name );
int     CONFIG_int   ( char *name );
long    CONFIG_long  ( char *name );
float   CONFIG_float ( char *name );
double  CONFIG_double( char *name );
char   *CONFIG_string( char *name );
BOOL    CONFIG_bool  ( char *name );
short   CONFIG_short ( char *name );
USHORT  CONFIG_ushort( char *name );
matrix *CONFIG_matrix( char *name );

//      Convert from string buffer to particular variable types...
char    CONFIG_buff2char  ( char *buff );
int     CONFIG_buff2int   ( char *buff );
long    CONFIG_buff2long  ( char *buff );
float   CONFIG_buff2float ( char *buff );
double  CONFIG_buff2double( char *buff );
char   *CONFIG_buff2str   ( char *buff );
BOOL    CONFIG_buff2bool  ( char *buff );
short   CONFIG_buff2short ( char *buff );
USHORT  CONFIG_buff2ushort( char *buff );

//      Convert from string buffer to variable in configuration list...
BOOL    CONFIG_buff2var( int type, void *vptr, char *buff );
BOOL    CONFIG_buff2cnfg( struct CONFIG_variable cnfg[], int item, char *buff );
BOOL    CONFIG_buff2array( struct CONFIG_variable cnfg[], int item, char *buff );

//      Return a string for a particular variable type...
char   *CONFIG_var2str( int type, void *vptr );

//      Return a string for a particular configuration variable...
char   *CONFIG_cnfg2str( struct CONFIG_variable *cnfg );

//      Return array dimension string ([x][y])...
char   *CONFIG_dimensions( int indx[] );
char   *CONFIG_dimensions( struct CONFIG_variable *cnfg );
char   *CONFIG_name( struct CONFIG_variable *cnfg );

//      Parse array dimensions from variable name (not used)...
BOOL    CONFIG_dimensions( struct CONFIG_variable cnfg[], int item );
BOOL    CONFIG_dimensions( char *name, int &nD, int indx[] );

//      Parse and process configuration information...
BOOL    CONFIG_parse( char *buff, char *name, char *data );
BOOL    CONFIG_process( struct CONFIG_variable cnfg[], char *buff );

//      Read a single line from the file...
BOOL    CONFIG_read( FILE *FP, char *buff, int size, BOOL &EofF );

//      Read configuration variables from file...
BOOL    CONFIG_read( struct CONFIG_variable cnfg[], char *file );
BOOL    CONFIG_read( char *file );

//      Write configuration variables to file...
BOOL    CONFIG_write( struct CONFIG_variable cnfg[], char *file, int flag );
BOOL    CONFIG_write( struct CONFIG_variable cnfg[], char *file );
BOOL    CONFIG_write( char *file, int flag );
BOOL    CONFIG_write( char *file );

//      Read configuration variables from environmental variables...
BOOL    CONFIG_environment( struct CONFIG_variable cnfg[] );
BOOL    CONFIG_environment( void );

//      Convert string list (sequentially) to configuration variables and back again...
BOOL    CONFIG_list2cnfg( struct CONFIG_variable cnfg[], char *text );
char   *CONFIG_cnfg2list( struct CONFIG_variable cnfg[] );

//      Convert matrix to string and back again...
void    CONFIG_mtx2str( char *str, matrix *mtx );
void    CONFIG_str2mtx( char *str, matrix *mtx );

/******************************************************************************/

BOOL    CONFIG_EnvironmentLoad( void );

char   *CONFIG_OperatingSystem( void );
char   *CONFIG_UserName( void );
char   *CONFIG_UserDomain( void );
char   *CONFIG_ComputerName( void );
char   *CONFIG_CalibrationPath( void );

/******************************************************************************/
/* Tables are configuration lists stored on multiple lines...
/******************************************************************************/

#define TABLE_STRUCTURE      0         // Table is an a single array of structures...
#define TABLE_ARRAY          1         // Table is multiple arrays of individual variables...
#define TABLE_ERROR         -1         // Table error value.

void    TABLE_vptr( struct CONFIG_variable cnfg[], int type, int size, int line );

#define TABLE_VPTR_SAVE      0
#define TABLE_VPTR_RESTORE   1
void    TABLE_vptr( struct CONFIG_variable cnfg[], int func );

//      Read table from file...
int     TABLE_read( char *file, struct CONFIG_variable cnfg[], int type, int size, int max );
int     TABLE_read( char *file, struct CONFIG_variable cnfg[], int size, int max );
int     TABLE_read( char *file, struct CONFIG_variable cnfg[], int max );

int     TABLE_read( char *file, int type, int size, int max );
int     TABLE_read( char *file, int size, int max );
int     TABLE_read( char *file, int max );

//      Write table to file...
BOOL    TABLE_write( char *file, struct CONFIG_variable cnfg[], int type, int size, int lines );
BOOL    TABLE_write( char *file, struct CONFIG_variable cnfg[], int size, int lines );
BOOL    TABLE_write( char *file, struct CONFIG_variable cnfg[], int lines );

BOOL    TABLE_write( char *file, int type, int size, int lines );
BOOL    TABLE_write( char *file, int size, int lines );
BOOL    TABLE_write( char *file, int lines );

//      Print the table...
void    TABLE_list( struct CONFIG_variable cnfg[], int type, int size, int lines, PRINTF prnf );
void    TABLE_list( struct CONFIG_variable cnfg[], int size, int lines, PRINTF prnf );
void    TABLE_list( struct CONFIG_variable cnfg[], int lines, PRINTF prnf );

void    TABLE_list( int type, int size, int lines, PRINTF prnf );
void    TABLE_list( int size, int lines, PRINTF prnf );
void    TABLE_list( int lines, PRINTF prnf );

/******************************************************************************/
/* The following definitions provide a level of compatability for the old     */
/* MYLIB BATCH method of reading parameters from a file.                      */
/******************************************************************************/

#define CONFIG_setvar(name)       CONFIG_set(#name,name)
#define CONFIG_setarray(name,N)   CONFIG_set(#name,name,N)
#define Set(name)                 CONFIG_set(#name,name)
#define Array(name,N)             CONFIG_set(#name,name,N)
#define assign(name,vptr,A,B,C)   CONFIG_set(#name,vptr)
#define read_batch(f)             CONFIG_read(f)
#define write_batch(f)            CONFIG_write(f)
#define BATCH                     void CONFIG_setup( void )

/******************************************************************************/

#endif

