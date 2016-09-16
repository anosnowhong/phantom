/******************************************************************************/
/*                                                                            */ 
/* MODULE  : MEM.cpp                                                          */ 
/*                                                                            */ 
/* PURPOSE : Memory manangement API.                                          */ 
/*                                                                            */ 
/* DATE    : 05/Jul/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 05/Jul/2000 - Initial development.                               */ 
/*                                                                            */ 
/* V1.1  JNI 21/Dec/2000 - Some improvements, most notable being memory block */ 
/*                         signing so that block over-run corruptions are     */ 
/*                         trapped when the pointer is freed.                 */ 
/*                                                                            */ 
/* V1.2  JNI 08/Jan/2001 - Memory management and block signing is optional.   */ 
/*                         Turn memory management on by defining MEM (with    */
/*                         compiler option /DMEM). Turn block signing on by   */ 
/*                         setting MEM_signing to TRUE.                       */ 
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     MEM_NAME
#define MODULE_TEXT     "Memory Manager API"
#define MODULE_DATE     "08/01/2001"
#define MODULE_VERSION  "1.2"
#define MODULE_LEVEL    0

/*****************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/
// Don't wrap them. We are the wrapper...
#undef  malloc
#undef  calloc
#undef  free
#undef  strdup
/*****************************************************************************/

BOOL    MEM_API_started=FALSE;                   // API started flag.
void  **MEM_item=NULL;                           // Array of memory pointers.
BOOL    MEM_signing=FALSE;                       // Sign memory blocks? (V1.2)

/*****************************************************************************/

PRINTF  MEM_PRN_messgf=NULL;                     // General messages printf function.
PRINTF  MEM_PRN_errorf=NULL;                     // Error messages printf function.
PRINTF  MEM_PRN_debugf=NULL;                     // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     MEM_messgf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(MEM_PRN_messgf,buff));
}


/******************************************************************************/

int     MEM_errorf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(MEM_PRN_errorf,buff));
}

/******************************************************************************/

int     MEM_debugf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(MEM_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    MEM_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok=TRUE;
int     item;

    if( MEM_API_started )              // Start the API once...
    {
        return(TRUE);
    }

    MEM_PRN_messgf = messgf;           // General API message print function.
    MEM_PRN_errorf = errorf;           // API error message print function.
    MEM_PRN_debugf = debugf;           // Debug information print function.

//  Allocated space for list of memory pointers.
    if( (MEM_item=(void **)calloc(MEM_ITEM_MAX,sizeof(void *))) == NULL )
    {
        MEM_PRN_errorf("MEM_API_start(...) Cannot allocate memory.\n");
        ok = FALSE;
    }

    if( !ok )
    {
        MEM_errorf("MEM_API_start() Failed.\n");
        return(FALSE);
    }

//  Initialise memory point list to NULL...
    for( item=0; (item < MEM_ITEM_MAX); item++ )
    {
        MEM_item[item] = NULL;
    }

    ATEXIT_API(MEM_API_stop);          // Install stop function.
    MEM_API_started = TRUE;            // Set started flag.

    MODULE_start(MEM_PRN_messgf);      // Register module.

    return(ok);
}

/******************************************************************************/

void    MEM_API_stop( void )
{
int     item,stray;
MALLOC *head;

    if( !MEM_API_started )             // API not started in the first place...
    {
         return;
    }

    for( stray=0,item=0; (item < MEM_ITEM_MAX); item++ )
    {
        if( MEM_item[item] == NULL )
        {
            continue;
        }

        head = MEM_head(MEM_item[item]);
        MEM_errorf("%s(%d): Stray pointer (%u bytes).\n",head->srce,head->line,head->size);
        MEM_free(&MEM_item[item],__FILE__,__LINE__);
        stray++;
    }

    STR_printf((stray > 0),MEM_errorf,MEM_messgf,"MEM_API_stop() %d stray pointer(s) released.\n",stray);

    MODULE_stop();                     // Register module stop.
    MEM_API_started = FALSE;
}

/******************************************************************************/

BOOL    MEM_API_check( void )
{
BOOL    ok=TRUE;

    if( !MEM_API_started )             // Start module automatically...
    {
        ok = MEM_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        MEM_debugf("MEM_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

int     MEM_find( void *mptr )
{
int     item,find;

//  Make sure the API is running...
    if( !MEM_API_check() )
    {
        return(MEM_ITEM_NULL);
    }

//  Find pointer in list of current pointers...
    for( find=MEM_ITEM_NULL,item=0; (item < MEM_ITEM_MAX); item++ )
    {
        if( MEM_item[item] == mptr )
        {
            find = item;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

size_t  MEM_size( size_t size )
{
    return(MALLOC_HEADER+size+MALLOC_STRLEN+1);
}

/******************************************************************************/

int     MEM_used( void )
{
int     item,used;

//  Make sure the API is running...
    if( !MEM_API_check() )
    {
        return(0);
    }

//  Count number of used pointers...
    for( used=0,item=0; (item < MEM_ITEM_MAX); item++ )
    {
        if( MEM_item != NULL )
        {
            used++;
        }
    }

    return(used);
}

/******************************************************************************/

long    MEM_size( void )
{
int     item;
long    size;

//  Make sure the API is running...
    if( !MEM_API_check() )
    {
        return(0L);
    }

//  Add up total size of blocks...
    for( size=0L,item=0; (item < MEM_ITEM_MAX); item++ )
    {
        if( MEM_item[item] != NULL )
        {
            size += (long)MEM_head(MEM_item[item])->size;
        }
    }

    return(size);
}

/*****************************************************************************/

void   *MEM_base( void *mptr )
{
void   *base=NULL;
char   *bptr;

    if( mptr != NULL )
    {
        bptr = (char *)mptr;
        base = (void *)&bptr[-MALLOC_HEADER];
    }

    return(base);
}

/*****************************************************************************/

void   *MEM_base( int item )
{
void   *base=NULL;

    if( item != MEM_ITEM_NULL )
    {
        base = MEM_base(MEM_item[item]);
    }

    return(base);
}

/*****************************************************************************/

MALLOC *MEM_head( void *mptr )
{
MALLOC *head;

    head = (MALLOC *)MEM_base(mptr);

    return(head);
}

/*****************************************************************************/

MALLOC *MEM_head( int item )
{
MALLOC *head;

    head = (MALLOC *)MEM_base(item);

    return(head);
}

/*****************************************************************************/

char   *MEM_sign( void *mptr )
{
MALLOC *head;
char   *sptr;
char   *sign=NULL;

    if( mptr != NULL )
    {
        head = MEM_head(mptr);
        sptr = (char *)mptr;
        sign = &sptr[head->size];
    }

    return(sign);
}

/*****************************************************************************/

char   *MEM_sign( int item )
{
char   *sign=NULL;

    if( item != MEM_ITEM_NULL )
    {
        sign = MEM_sign(MEM_item[item]);
    }

    return(sign);
}    

/*****************************************************************************/

BOOL    MEM_signed( int item )
{
char   *sign;
int     slen;
BOOL    ok=FALSE;

    if( (sign=MEM_sign(item)) != NULL )
    {
        slen = MALLOC_STRLEN;
        ok = (memcmp(MALLOC_STRING,sign,slen) == 0);
        STR_printf(ok,MEM_debugf,MEM_errorf,"MEM_signed() %s [%-*.*s]\n",STR_OkFailed(ok),slen,slen,sign);
    }

    return(ok);
}

/*****************************************************************************/

void    MEM_list( PRINTF prnf )
{
int     item;
MALLOC *head;

    for( item=0; (item < MEM_ITEM_MAX); item++ )
    {
        if( MEM_item[item] == NULL )
        {
            continue;
        }

        head = MEM_head(item);
      (*prnf)("[%05d] %s(%d) %u bytes.\n",item,head->srce,head->line,head->size);
    }
}

/******************************************************************************/

void   *MEM_malloc( size_t size, char *file, int line, BOOL sign )
{
char   *mptr,*sptr;
int     item;
MALLOC *head;

//  Allocate the memory block...
    if( !MEM_API_check() )
    {
        return(NULL);
    }

//  Check for NULL size...
    if( size == 0 )
    {
        MEM_errorf("%s(%d): MEM_malloc(%u bytes) Null size.\n",file,line,size);
        return(NULL);
    }

//  Find a free handle...
    if( (item=MEM_find(NULL)) == MEM_ITEM_NULL )
    {
        MEM_errorf("%s(%d): MEM_malloc(%u bytes) No free handles.\n",file,line,size);
        return(NULL);
    }

//  Allocate the memory block...
    if( (mptr=(char *)malloc(MEM_size(size))) == NULL )
    {
        MEM_errorf("%s(%d): MEM_malloc(%u bytes) Cannot allocate memory.\n",file,line,size);
        return(NULL);
    }

//  Set header information...
    head = (MALLOC *)&mptr[0];              // Header prefix...
    head->item = item;                      // Item index in pointer list.
    head->size = size;                      // Size of user portion of block.
    head->sign = sign;                      // Sign block?
    head->line = line;                      // Line number in source file.
    strncpy(head->srce,file,MEM_SRCE);      // File name of source.

//  User part of block...
    MEM_item[item] = &mptr[MALLOC_HEADER];

//  Sign the end of the block...
    if( head->sign )
    {
        sptr = MEM_sign(item);
        strcpy(sptr,MALLOC_STRING);
        sptr[MALLOC_STRLEN] = NUL;
    }

    MEM_debugf("%s(%d): MEM_malloc(%u bytes) SIGNED=%s.\n",file,line,size,STR_YesNo(sign));
    MEM_debugf("[%06d] mptr=%p head=%p user=%p\n",item,mptr,head,MEM_item[item]);

    return(MEM_item[item]);
}

/******************************************************************************/

void   *MEM_malloc( size_t size, char *file, int line )
{
    return(MEM_malloc(size,file,line,MEM_signing));
}

/******************************************************************************/

void   *MEM_calloc( size_t item, size_t size, char *file, int line, BOOL sign )
{
    return(MEM_malloc(item*size,file,line,sign));
}

/******************************************************************************/

void   *MEM_calloc( size_t item, size_t size, char *file, int line )
{
    return(MEM_malloc(item*size,file,line,MEM_signing));
}

/******************************************************************************/

void    MEM_free( void **mptr, char *file, int line )
{
MALLOC *head;
void   *base;

//  Make sure the API is running...
    if( !MEM_API_check() )
    {
        return;
    }

//  Attempt to free NULL pointer?
    if( *mptr == NULL )
    {
        MEM_errorf("%s(%d): MEM_Free(...) NULL pointer.\n",file,line);
        return;
    }

//  Header and base pointers for block...
    head = MEM_head(*mptr);
    base = MEM_base(head);

//  Check if the block is still signed...
    if( head->sign )
    {
        if( !MEM_signed(head->item) )
        {
            MEM_errorf("%s(%d): MEM_free() Memory block corrupted.\n",head->srce,head->line);
        }
    }

    MEM_debugf("%s(%d): MEM_free() %u bytes from %s(%d).\n",file,line,head->size,head->srce,head->line);
    MEM_debugf("user=%p head=%p base=%p\n",mptr,head,base);

//  Set pointers to NULL...
    MEM_item[head->item] = NULL;
   *mptr = NULL;

//  And, finally, free the memory block...
    free(base);
}

/*****************************************************************************/

char   *MEM_strdup( char *str, char *file, int line )
{
char   *ptr;
int     len;

    len = strlen(str);

    if( (ptr=(char *)MEM_malloc(len+1,file,line,FALSE)) != NULL )
    {
        strcpy(ptr,str);
        ptr[len] = NUL;
    }
    
    return(ptr);
}

/*****************************************************************************/

