/******************************************************************************/
/*                                                                            */
/* MODULE  : matrix.cpp                                                       */
/*                                                                            */
/* SOURCE  : mtx-memory.cpp                                                   */
/*                                                                            */
/* PURPOSE : Matrix memory allocation and management.                         */
/*                                                                            */
/* DATE    : 07/Jul/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V2.0  JNI 07/Jul/2000 - Development started and MATRIX module re-worked.   */
/*                                                                            */
/* V2.1  JNI 27/Sep/2001 - Processing for empty (NULL) matrices. Changed      */
/*                         default constructor to create empry matrix.        */
/*                                                                            */
/******************************************************************************/

size_t  VALLOC_TypeSize[] = { 0,sizeof(double),sizeof(float),sizeof(int),0 };
char   *VALLOC_TypeText[] = { "NULL","double","float","int",NULL };

/*****************************************************************************/

void   *VALLOC_make( int type, size_t elements, char *file, int line )
{                                                // Allocate some memory for a vector or matrix.
size_t  size,block;
char   *mptr;
VALLOC *head;

    // Make sure the API is running...
    if( !MATRIX_API_check() )
    {
        return(NULL);
    }

    // Check number of elements (V2.1)...
    if( elements == 0 )
    {
        return(NULL);
    }

    // Element size...
    size = VALLOC_TypeSize[type];

    // Size of memory block...
    block = VALLOC_HEADER + (elements * size);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VALLOC_make(type=%s[%d],elements=%d,file=%s,line=%d) = %d bytes.\n",
                   VALLOC_TypeText[type],VALLOC_TypeSize[type],elements,file,line,block);
#endif

    // Allocate the memory block...
#ifdef  MEM
    if( (mptr=(char *)MEM_malloc(block+10,file,line)) == NULL )
#else
    if( (mptr=(char *)malloc(block+10)) == NULL )
#endif
    {
        MEMERR;
        return(NULL);
    }

    // Set header information...
    head = (VALLOC *)mptr;                  // Header prefix.
    head->type = type;                      // VALLOC type.
    head->elements = elements;              // Number of elements.
    head->block = block;                    // Size of block.

    // Return user part of block...
    return(&mptr[VALLOC_HEADER]); 
}

/*****************************************************************************/

void   *VALLOC_base( void *vptr )
{
char   *base=NULL,*mptr;

    if( vptr != NULL )
    {
        mptr = (char *)vptr;
        base = &mptr[-VALLOC_HEADER];
    }

    return(base);
}

/*****************************************************************************/

VALLOC *VALLOC_head( void *vptr )
{
VALLOC *head=NULL;

    if( vptr != NULL )
    {
        head = (VALLOC *)VALLOC_base(vptr);
    }

    return(head);
}

/*****************************************************************************/

size_t  VALLOC_elements( void *vptr )
{
VALLOC *head;
int     elements=0;

    if( vptr != NULL )
    {
        head = VALLOC_head(vptr);
        elements = head->elements;
    }

    return(elements);
}

/*****************************************************************************/

int     VALLOC_type( void *vptr )
{
VALLOC *head;
int     type=VALLOC_NULL;

    if( vptr != NULL )
    {
        head = VALLOC_head(vptr);
        type = head->type;
    }

    return(type);
}

/*****************************************************************************/

size_t  VALLOC_block( void *vptr )
{
VALLOC *head;
size_t  block=0;

    if( vptr != NULL )
    {
        head = VALLOC_head(vptr);
        block = head->block;
    }

    return(block);
}

/*****************************************************************************/

void    VALLOC_copy( void *dest, void *srce )
{
BOOL    ok=FALSE;

    if( srce == NULL )
    {
        MATRIX_debugf("VALLOC_copy(...) NULL source.\n");
        return;
    }
    else
    if( VALLOC_type(dest) != VALLOC_type(srce) )
    {
        MATRIX_errorf("VALLOC_copy(...) Different types.\n");
    }
    else
    if( VALLOC_elements(dest) != VALLOC_elements(srce) )
    {
        MATRIX_errorf("VALLOC_copy(...) Different element count.\n");
    }
    else
    {
        memcpy(dest,srce,VALLOC_block(dest));
        ok = TRUE;
    }
}

/*****************************************************************************/

void     VALLOC_free( void *vptr )
{
void    *base;

    if( !MATRIX_API_check() )                    // Make sure the API is running.
    {
        return;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VALLOC_free(%p) %d bytes released.\n",vptr,VALLOC_block(vptr));
#endif

    base = VALLOC_base(vptr);
    free(base);
}

/*****************************************************************************/

VECDBL  VECDBL_make( int elements, char *srce, int line )
{
VECDBL  ptr;

    if( !MATRIX_API_check() )               // Make sure the API is running.
    {
        return(NULL);
    }

    if( line == 0 )                         // Empty vector (2.1).     
    {
        return(NULL);
    }

    if( (ptr=(VECDBL)VALLOC_make(VALLOC_VECDBL,elements+1,srce,line)) == NULL )
    {
        MEMERR;
        return(NULL);
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VECDBL(%d) Allocated.\n",elements);
#endif

    zero(ptr);

    return(ptr);
}

/*****************************************************************************/

VECFLT  VECFLT_make( int elements, char *srce, int line )
{
VECFLT  ptr;

    if( !MATRIX_API_check() )               // Make sure the API is running.
    {
        return(NULL);
    }

    if( line == 0 )                         // Empty vector (2.1).     
    {
        return(NULL);
    }

    if( (ptr=(VECFLT)VALLOC_make(VALLOC_VECFLT,elements+1,srce,line)) == NULL )
    {
        MEMERR;
        return(NULL);
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VECFLT(%d) Allocated.\n",elements);
#endif

    zero(ptr);
    
    return(ptr);
}

/*****************************************************************************/

VECINT  VECINT_make( int elements, char *srce, int line )
{
VECINT  ptr;

    if( !MATRIX_API_check() )               // Make sure the API is running.
    {
        return(NULL);
    }

    if( line == 0 )                         // Empty matrix (2.1).     
    {
        return(NULL);
    }

    if( (ptr=(VECINT)VALLOC_make(VALLOC_VECINT,elements+1,srce,line)) == NULL )
    {
        MEMERR;
        return(NULL);
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VECINT(%d) Allocated.\n",elements);
#endif

    zero(ptr);
    
    return(ptr);
}

/*****************************************************************************/

MTXDBL  MTXDBL_make( int row, int col, char *srce, int line )
{
MTXDBL  ptr;
int     r;

    if( !MATRIX_API_check() )               // Make sure the API is running.
    {
        return(NULL);
    }

    if( (row*col) == 0 )                    // Empty matrix (2.1).     
    {
        return(NULL);
    }

//  First allocate space for row pointers...
    if( (ptr=(MTXDBL)calloc(row+1,sizeof(VECDBL))) == NULL )
    {
        MEMERR;
        return(NULL);
    }

//  Then allocate space for elements...
    if( (ptr[0]=VECDBL_make((row+1)*(col+1),srce,line)) == NULL )
    {
        MEMERR;
        return(NULL);
    }

//  Now assign each row pointer to it's starting column element...
    for( r=1; (r <= row); r++ )
    {
        ptr[r] = &ptr[r-1][col+1];
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("MTXDBL(%d,%d) Allocated.\n",row,col);
#endif

//    zero(ptr);

    return(ptr);
}

/*****************************************************************************/

MTXFLT  MTXFLT_make( int row, int col, char *srce, int line )
{
MTXFLT  ptr;
int     r;

    if( !MATRIX_API_check() )               // Make sure the API is running.
    {
        return(NULL);
    }

    if( (row*col) == 0 )                    // Empty matrix (2.1).     
    {
        return(NULL);
    }

//  First allocate space for row pointers...
    if( (ptr=(MTXFLT)calloc(row+1,sizeof(VECFLT))) == NULL )
    {
        MEMERR;
        return(NULL);
    }

//  Then allocate space for elements...
    if( (ptr[0]=VECFLT_make((row+1)*(col+1),srce,line)) == NULL )
    {
        MEMERR;
        return(NULL);
    }

//  Now assign each row pointer to it's starting column element...
    for( r=1; (r <= row); r++ )
    {
        ptr[r] = &ptr[r-1][col+1];
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("MTXFLT(%d,%d) Allocated.\n",row,col);
#endif

    zero(ptr);

    return(ptr);
}

/*****************************************************************************/

MTXINT  MTXINT_make( int row, int col, char *srce, int line )
{
MTXINT  ptr;
int     r;

    if( !MATRIX_API_check() )               // Make sure the API is running.
    {
        return(NULL);
    }

    if( (row*col) == 0 )                    // Empty matrix (2.1).     
    {
        return(NULL);
    }

//  First allocate space for row pointers...
    if( (ptr=(MTXINT)calloc(row+1,sizeof(VECINT))) == NULL )
    {
        MEMERR;
        return(NULL);
    }

//  Then allocate space for elements...
    if( (ptr[0]=VECINT_make((row+1)*(col+1),srce,line)) == NULL )
    {
        MEMERR;
        return(NULL);
    }

//  Now assign each row pointer to it's starting column element...
    for( r=1; (r <= row); r++ )
    {
        ptr[r] = &ptr[r-1][col+1];
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("MTXINT(%d,%d) Allocated.\n",row,col);
#endif

    zero(ptr);

    return(ptr);
}

/*****************************************************************************/
/* Zero the contents of various types of matrix & vectors arrays.            */
/*****************************************************************************/

void    zero( VECDBL ptr )
{
int     item,elements;

    if( ptr == NULL )
    {
        return;
    }

    elements = vitem(ptr);

    for( item=0; (item < elements); item++ )
    {
        ptr[item] = 0.0;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VECDLB(%d) Zero.\n",elements);
#endif
}

/*****************************************************************************/

void    zero( VECFLT ptr )
{
int     item,elements;

    if( ptr == NULL )
    {
        return;
    }

    elements = vitem(ptr);

    for( item=0; (item < elements); item++ )
    {
        ptr[item] = 0.0;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VECFLT(%d) Zero.\n",elements);
#endif
}

/*****************************************************************************/

void    zero( VECINT ptr )
{
int     item,elements;

    if( ptr == NULL )
    {
        return;
    }

    elements = vitem(ptr);

    for( item=0; (item < elements); item++ )
    {
        ptr[item] = 0.0;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VECINT(%d) Zero.\n",elements);
#endif
}

/*****************************************************************************/

void    zero( MTXDBL ptr )
{
    zero(ptr[0]);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("MTXDBL Zero.\n");
#endif
}

/*****************************************************************************/

void    zero( MTXFLT ptr )
{
    zero(ptr[0]);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("MTXFLT Zero.\n");
#endif
}

/*****************************************************************************/

void    zero( MTXINT ptr )
{
    zero(ptr[0]);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("MTXINT Zero.\n");
#endif
}

/*****************************************************************************/
/* Free memory allocated to various types of matrix & vectors arrays.        */
/*****************************************************************************/

void     dump( VECDBL ptr )
{
    if( ptr == NULL )
    {
        return;
    }

    vfree(ptr);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VECDBL Dump.\n");
#endif
}

/*****************************************************************************/

void     dump( VECFLT ptr )
{
    if( ptr == NULL )
    {
        return;
    }

    vfree(ptr);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VECFLT Dump.\n");
#endif
}

/*****************************************************************************/

void     dump( VECINT ptr )
{
    if( ptr == NULL )
    {
        return;
    }

    vfree(ptr);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("VECINT Dump.\n");
#endif
}

/*****************************************************************************/

void    dump( MTXDBL ptr )
{
    if( ptr == NULL )
    {
        return;
    }

    dump(ptr[0]);
    free(ptr);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("MTXDBL() Dump.\n");
#endif
}

/*****************************************************************************/

void    dump( MTXFLT ptr )
{
    if( ptr == NULL )
    {
        return;
    }

    dump(ptr[0]);
    free(ptr);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("MTXFLT() Dump.\n");
#endif
}

/*****************************************************************************/

void    dump( MTXINT ptr )
{
    if( ptr == NULL )
    {
        return;
    }

    dump(ptr[0]);
    free(ptr);

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("MTXINT() Dump.\n");
#endif
}

/*****************************************************************************/

void    matrix_memcpy( matrix &dest, matrix &srce )
{
    if( srce.isempty() )          // Copying an empty matrix (2.1)...
    {
        matrix_empty(dest);
        return;
    }

    matrix_dim(dest,srce.row,srce.col);
    matrix_memcpy(dest.mtx,srce.mtx,dest.row,dest.col);
}

/*****************************************************************************/

void    matrix_itmcpy( matrix &dest, matrix &srce )
{
    if( srce.isempty() )          // Copying an empty matrix (2.1)...
    {
        matrix_empty(dest);
        return;
    }

    matrix_dim(dest,srce.row,srce.col);
    matrix_itmcpy(dest.mtx,srce.mtx,dest.row,dest.col);
}

/*****************************************************************************/

void    matrix_memcpy( MTXDBL dest, MTXDBL srce, int row, int col )
{
    vcopy(dest[0],srce[0]);
}

/*****************************************************************************/

void    matrix_itmcpy( MTXDBL dest, MTXDBL srce, int row, int col )
{
int     x,y;

    for( x=1; (x <= row); x++ ) 
    {
        for( y=1; (y <= col); y++ )
        {
            dest[x][y] = srce[x][y]; 
        }
    }
}

/*****************************************************************************/

void    matrix_copy( matrix &dest, matrix &srce )
{
    matrix_memcpy(dest,srce);
}

/*****************************************************************************/

void    matrix_fill( matrix &mtx, double val )
{
int     i,j;

    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            mtx.mtx[i][j] = val;
        }
    }
}

/*****************************************************************************/

void    matrix_zero( matrix &mtx )
{
    matrix_fill(mtx,0.0);
}

/*****************************************************************************/
