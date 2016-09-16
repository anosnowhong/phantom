/******************************************************************************/
/*                                                                            */ 
/* MODULE  : MEM.h                                                            */ 
/*                                                                            */ 
/* PURPOSE : Memory manangement API.                                          */ 
/*                                                                            */ 
/* DATE    : 05/Jul/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 05/Jul/2000 - Initial development.                               */ 
/*                                                                            */ 
/* V1.1  JNI 21/Dec/2000 - Some improvements.                                 */ 
/*                                                                            */ 
/* V1.2  JNI 08/Jan/2001 - Memory management and block signing is optional.   */ 
/*                                                                            */ 
/******************************************************************************/

#define MEM_NAME "MEM"                 // Module name.

/******************************************************************************/

#define MEM_DEBUG() if( !MEM_API_start(printf,printf,printf) ) { printf("Cannot start MEM API.\n"); exit(0); }
#define MEM_SIGN()  MEM_signing=TRUE

/******************************************************************************/

#define MEM_ITEM_MAX     20480         // Maximum number of (M)allocated blocks.
#define MEM_ITEM_NULL       -1         // NULL item handle value.

/*****************************************************************************/

typedef
struct  MALLOC_Header                  // Memory block header...
{
    int     item;                      // Item index in pointer array.

    size_t  size;                      // Size of block.
    int     line;                      // Line number in source file.

#define MEM_SRCE   40
    char    srce[MEM_SRCE];            // Source file name.

    BOOL    sign;                      // Block signed?
}
MALLOC;

#define MALLOC_HEADER   sizeof(MALLOC)
#define MALLOC_STRING   "ABC"
#define MALLOC_STRLEN   3

/*****************************************************************************/

extern  BOOL  MEM_signing;             // Sign memory blocks? (V1.2)

/*****************************************************************************/

BOOL    MEM_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    MEM_API_stop( void );
BOOL    MEM_API_check( void );

int     MEM_messgf( const char *mask, ... );
int     MEM_errorf( const char *mask, ... );
int     MEM_debugf( const char *mask, ... );

int     MEM_find( void *mptr );
int     MEM_used( void );
long    MEM_size( void );
void   *MEM_base( int item  );
void   *MEM_base( void *mptr );
MALLOC *MEM_head( int item );
MALLOC *MEM_head( void *mptr );
char   *MEM_sign( void *mptr );
char   *MEM_sign( int item );
size_t  MEM_size( size_t size );
void    MEM_list( PRINTF prnf );
BOOL    MEM_signed( int item );

/*****************************************************************************/
/* Replacement functions...                                                  */
/*****************************************************************************/

void   *MEM_malloc( size_t size, char *file, int line, BOOL sign );
void   *MEM_malloc( size_t size, char *file, int line );
void   *MEM_calloc( size_t item, size_t size, char *file, int line, BOOL sign );
void   *MEM_calloc( size_t item, size_t size, char *file, int line );
void    MEM_free( void **mptr, char *file, int line );
char   *MEM_strdup( char *str, char *file, int line );

/*****************************************************************************/
/* Define macro replacements for standard C memory allocation routines...    */
/*****************************************************************************/

#ifdef  MEM              // Memory management enabled? (V1.2)
#define calloc(N,S)      MEM_calloc((N),(S),__FILE__,__LINE__)
#define malloc(S)        MEM_malloc((S),__FILE__,__LINE__)
#define free(P)          MEM_free((void **)&P,__FILE__,__LINE__)
#define strdup(S)        MEM_strdup((S),__FILE__,__LINE__);
#endif

/*****************************************************************************/

