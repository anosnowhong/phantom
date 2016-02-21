/******************************************************************************/
/*                                                                            */
/* MODULE  : matrix.cpp                                                       */
/*                                                                            */
/* PURPOSE : Matrix class and related functions.                              */
/*                                                                            */
/* DATE    : 07/Jul/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V2.0  JNI 07/Jul/2000 - Development taken over and module re-worked.       */
/*                                                                            */
/* V2.1  JNI 27/Sep/2001 - Processing for empty (NULL) matrices. Changed      */
/*                         default constructor to create empry matrix.        */
/*                                                                            */
/* V2.2  JNI 25/Feb/2002 - Fixed bug in element compare function (used in     */
/*                         sorting); Added function to find median; new sort  */
/*                         functions to allow matrices (not just vectors) to  */
/*                         be sorted.                                         */
/*                                                                            */
/* V2.3  JNI 16/Mar/2004 - Fixed bug in matrix_sort(...) functions.           */
/*                                                                            */
/* V2.4  JNI 04/Apr/2005 - Added matrix name and print error maximum.         */
/*                                                                            */
/* V2.5  JNI 22/May/2009 - BOOL UndefinedDouble( double value );              */
/*                         Added function to catch undefined values in double */
/*                         variables.                                         */
/*                                                                            */
/* V2.6  JNI 27/Mar/2015 - Latency of matrix construction / destruction.      */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "MATRIX"
#define MODULE_TEXT     "Matrix API"
#define MODULE_DATE     "27/03/2015"
#define MODULE_VERSION  "2.6"
#define MODULE_LEVEL    1

/******************************************************************************/

#include <stdarg.h>
#include <motor.h>                               // Includes (almost) everything we need.
#include <nr.h>                                  // Numerical Recipes.

/******************************************************************************/

// #define MATRIX_DEBUG

/******************************************************************************/

BOOL    MATRIX_API_started=FALSE;                // API started flag.

int     MATRIX_current=0;                        // Current number of matrix objects.
int     MATRIX_maximum=0;                        // Maximum number.
int     MATRIX_total=0;                          // Accumulative total.

int     MATRIX_ErrorCount=0;                     // Number of matrix errors printed.

/******************************************************************************/

// V2.6 Matrix construction / destruction latencies.
BOOL    MATRIX_TimingFlag=FALSE;

TIMER_Interval   MATRIX_TimingConstruction("MatrixConstruction");
TIMER_Interval   MATRIX_TimingDestruction("MatrixDestruction");

void MATRIX_TimingOn( void )
{
    MATRIX_TimingFlag = TRUE;
}

void MATRIX_TimingOff( void )
{
    MATRIX_TimingFlag = FALSE;
}

void MATRIX_TimingResults( void )
{
STRING tname;
double tmean,tmin,tmax,tsd;
int tcount;

    MATRIX_TimingConstruction.Results(tname,tmean,tmin,tmax,tsd,tcount);
    printf("%s: mean=%.3lf, min=%.3lf, max=%.3lf, sd=%.3lf (usec), n=%d\n","MatrixConstruction",1000.0*tmean,1000.0*tmin,1000.0*tmax,1000.0*tsd,tcount);

    MATRIX_TimingDestruction.Results(tname,tmean,tmin,tmax,tsd,tcount);
    printf("%s: mean=%.3lf, min=%.3lf, max=%.3lf, sd=%.3lf (usec), n=%d\n","MatrixDestruction",1000.0*tmean,1000.0*tmin,1000.0*tmax,1000.0*tsd,tcount);
}

/******************************************************************************/
/* JNI 15/Jun/00 - Moved pseudo error functions (#define) to real functions.  */
/******************************************************************************/

void    MATRIX_MemErr( int line, char *file )
{
    MATRIX_errorf("MATRIX: Memory Error in line %i in file %s\n",line,file);
}

void    MATRIX_GenErr( char *text, matrix p, matrix q )
{
    MATRIX_errorf("MATRIX: Error in %s: Matrices are %i x %i and %i x %i\n",text,p.row,p.col,q.row,q.col);
}

void    MATRIX_GenErr( char *text, matrix p )
{
    MATRIX_errorf("MATRIX: Error in %s: Matrix is %i x %i\n",text,p.row,p.col);
}

void    MATRIX_GenErr( char *text )
{
    MATRIX_errorf("MATRIX: %s\n",text);
}

/******************************************************************************/

STRING MATRIX_SourceFile="";
int    MATRIX_SourceLine=0;

void MATRIX_SetSourceFile( char *file, int line )
{
    strncpy(MATRIX_SourceFile,file,STRLEN);
    MATRIX_SourceLine = line;
}

/******************************************************************************/

PRINTF  MATRIX_PRN_messgf=NULL;                  // General messages printf function.
PRINTF  MATRIX_PRN_errorf=NULL;                  // Error messages printf function.
PRINTF  MATRIX_PRN_debugf=NULL;                  // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     MATRIX_messgf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(MATRIX_PRN_messgf,buff));
}

/******************************************************************************/

int     MATRIX_errorf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];
int     rval=0;

    MATRIX_ErrorCount++;

    if( MATRIX_ErrorCount <= MATRIX_ERROR_MAX )
    {
        va_start(args,mask);
        vsprintf(buff,mask,args);
        va_end(args);

        rval = STR_printf(MATRIX_PRN_errorf,buff);
    }
    else
    if( MATRIX_ErrorCount == (MATRIX_ERROR_MAX+1) )
    {
        rval = STR_printf(MATRIX_PRN_errorf,STR_stringf("MATRIX: Too many errors (Max=%d).\n",MATRIX_ERROR_MAX));
    }

    return(rval);
}

/******************************************************************************/

int     MATRIX_debugf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(MATRIX_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    MATRIX_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok=FALSE;

    MATRIX_PRN_messgf = messgf;        // General API message print function.
    MATRIX_PRN_errorf = errorf;        // API error message print function.
    MATRIX_PRN_debugf = debugf;        // Debug information print function.

    if( MATRIX_API_started )           // Start the API once...
    {
        return(TRUE);
    }

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(MATRIX_API_stop);        // Install stop function.
        MATRIX_API_started = TRUE;          // Set started flag.
        MODULE_start(MATRIX_PRN_messgf);    // Register module.
    }
    else
    {
        MATRIX_errorf("MATRIX_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    MATRIX_API_stop( void )
{
    if( !MATRIX_API_started )          // API not started in the first place...
    {
         return;
    }

    // Print usage information...
    STR_printf((MATRIX_current == 0),MATRIX_debugf,MATRIX_messgf,"MATRIX_API_stop() Usage: %d total, %d peak, %d current.\n",
                MATRIX_total,
                MATRIX_maximum,
                MATRIX_current);

    MODULE_stop();                     // Register module stop.
    MATRIX_API_started = FALSE;
}

/******************************************************************************/

BOOL    MATRIX_API_check( void )
{
BOOL    ok=TRUE;

    if( !MATRIX_API_started )          // API not started...
    {                                  // Start module automatically...
        ok = MATRIX_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        MATRIX_debugf("MATRIX_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/*****************************************************************************/

void    matrix_constructed( void )
{
    MATRIX_current++;
    MATRIX_total++;

    if( MATRIX_current > MATRIX_maximum )
    {
        MATRIX_maximum = MATRIX_current;
    }
}

/*****************************************************************************/

void    matrix_destructed( void )
{
    if( MATRIX_current > 0 )
    {
        MATRIX_current--;
    }
}

/*****************************************************************************/
/* Matrix constructors - called to create a matrix.                          */
/*                                                                           */
/* These functions are called automatically by the object manager in C++ to  */
/* create instances of matrix objects, based on the parameters passed.       */
/*****************************************************************************/

void matrix::init( void )
{
    // Make sure the API is running...
    if( !MATRIX_API_check() )
    {
        return;
    }

    // Update usage statistics.
    matrix_constructed();

    memset(ObjectName,0,STRLEN);
    memset(SourceFile,0,STRLEN);

    mtx = NULL;
    row = 0;
    col = 0;
}

/*****************************************************************************/

void matrix::init( char *name, char *file, int line )
{
    // Make sure the API is running...
    if( !MATRIX_API_check() )
    {
        return;
    }

    // Basic initialization.
    init();

    if( !STR_null(name) )
    {
        strncpy(ObjectName,name,STRLEN);
    }

    if( !STR_null(file) )
    {
        strncpy(SourceFile,file,STRLEN);
        SourceLine = line;
    }
}

/*****************************************************************************/

void matrix::init( char *name, char *file, int line, int rows, int columns )
{
    // Make sure the API is running...
    if( !MATRIX_API_check() )
    {
        return;
    }

    // Basic initialization.
    init(name,file,line);

    // Check for zero-size (empty) matrices...
    if( (rows * columns) > 0 )
    {
        row = rows;
        col = columns;

        if( (mtx=dmatrix(row,col)) == NULL )
        {
            Errorf("Memory error.\n");

            row = 0;
            col = 0;

            return;
        }
    }

#ifdef  MATRIX_DEBUG
    Debugf("Matrix created");
#endif
}

/*****************************************************************************/

void matrix::init( char *name, char *file, int line, matrix &source )
{
    init(name,file,line,source.row,source.col);

    if( (row * col) > 0 )
    {
        matrix_memcpy(mtx,source.mtx,row,col);
    }
}

/*****************************************************************************/

matrix::matrix( char *name, char *file, int line, int rows, int columns )
{
    init(name,file,line,rows,columns);
}

/*****************************************************************************/

matrix::matrix( int rows, int columns )
{
    init(NULL,MATRIX_SourceFile,MATRIX_SourceLine,rows,columns);
}

/*****************************************************************************/

matrix::matrix( char *name, char *file, int line, int rows )
{
    init(name,file,line,rows,1);
}

/*****************************************************************************/

matrix::matrix( int rows )
{
    init(NULL,MATRIX_SourceFile,MATRIX_SourceLine,rows,1);
}

/*****************************************************************************/

matrix::matrix( char *name, char *file, int line )
{
    init(name,file,line,0,0);
}

/*****************************************************************************/

matrix::matrix( void )
{
    MATRIX_ConstructionBefore();
    init(NULL,MATRIX_SourceFile,MATRIX_SourceLine,0,0);
    MATRIX_ConstructionAfter();
}

/*****************************************************************************/

matrix::matrix( char *name, char *file, int line, matrix &source )
{
    MATRIX_ConstructionBefore();
    init(name,file,line,source);
    MATRIX_ConstructionAfter();
}

/*****************************************************************************/

matrix::matrix( matrix &source )
{
    MATRIX_ConstructionBefore();
    init(NULL,MATRIX_SourceFile,MATRIX_SourceLine,source);
    MATRIX_ConstructionAfter();
}

/*****************************************************************************/

matrix::matrix( int rows, int columns, double d, ... )
{
int     i,j;
va_list argp;

    MATRIX_ConstructionBefore();
    init(NULL,MATRIX_SourceFile,MATRIX_SourceLine,rows,columns);

    if( mtx != NULL )
    {
        mtx[1][1] = d;

        va_start(argp,d);

        for( j=2,i=1; (i <= row); i++ )
        {
            for( ; (j <= col); j++ )
            {
                mtx[i][j] = va_arg(argp,double);
            }
 
            j=1;
        }

        va_end(argp);
    }
    MATRIX_ConstructionAfter();
}

/*****************************************************************************/
/* Matrix deconstructor - called to destroy a matrix.                        */
/*****************************************************************************/

matrix::~matrix( void )
{
    MATRIX_DestructionBefore();

    if( !MATRIX_API_check() )                    // Make sure the API is running.
    {
        return;
    }

#ifdef  MATRIX_DEBUG
    Debugf("Matrix destroyed");
#endif

    if( mtx != NULL )
    {
        dump(mtx);
        mtx = NULL;
    }

    row = 0;
    col = 0;

    matrix_destructed();

    MATRIX_DestructionAfter();
}

/******************************************************************************/

char *matrix::Name( void )
{
static STRING name;

    memset(name,0,STRLEN);

    if( !STR_null(SourceFile) )
    {
        strncat(name,STR_stringf("%s(%d) ",SourceFile,SourceLine),STRLEN);
    }

    strncat(name,"matrix",STRLEN);

    if( !STR_null(ObjectName) )
    {
        strncat(name,STR_stringf(" %s",ObjectName),STRLEN);
    }

    strncat(name,STR_stringf("(%d,%d)",row,col),STRLEN);

    return(name);
}

/******************************************************************************/

int matrix::Messgf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(MATRIX_messgf("%s %s",Name(),buff));
}

/******************************************************************************/

int matrix::Errorf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(MATRIX_errorf("%s %s",Name(),buff));
}

/******************************************************************************/

int matrix::Debugf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(MATRIX_debugf("%s %s",Name(),buff));
}

/******************************************************************************/

int matrix::Printf( BOOL ok, const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ok,MATRIX_debugf,MATRIX_errorf,STR_stringf("%s %s",Name(),buff)));
}


/*****************************************************************************/

BOOL    matrix_dim_ok( int rows, int columns )
{
BOOL    ok;

    ok = (rows >= 1) && (columns >= 1);

    return(ok);
}

/*****************************************************************************/

void matrix::dim( int r, int c )
{
    if( !matrix_dim_ok(r,c) )
    {
        Errorf("dim(%d,%d) Invalid dimensions.\n",r,c);
        return;
    }

    if( (mtx != NULL) && (row == r) && (col == c) )
    {
        zeros();
        return;
    }

    if( mtx != NULL )
    {
        dump(mtx);
        mtx = NULL;
    }

    row = 0;
    col = 0;

    if( (r*c) != 0 )
    {
        if( (mtx=dmatrix(r,c)) == NULL )
        {
            Errorf("dim(%d,%d) Memory error.\n",r,c);
        }
        else
        {
            row = r;
            col = c;
        }
    }

#ifdef  MATRIX_DEBUG
    Debugf("dim(%d,%d).\n",r,c);
#endif
}

/*****************************************************************************/

void matrix::dim( int r )
{
    dim(r,1);
}

/*****************************************************************************/

void matrix::dim( matrix &mtx )
{
    dim(mtx.rows(),mtx.cols());
}

/*****************************************************************************/

void matrix::empty( void )
{
    if( mtx != NULL )
    {
        dump(mtx);
        mtx = NULL;
    }

    row = 0;
    col = 0;
}

/*****************************************************************************/
/* Matrix information - find out stuff about the matrix.                     */
/*****************************************************************************/

int     matrix::rows( void )
{
    return(row);
}

/*****************************************************************************/

int     matrix::cols( void )
{
    return(col);
}

//*****************************************************************************/

int     matrix::items( void )
{
    return(col * row);
}

/*****************************************************************************/

BOOL    matrix::isrowvector( void )
{
BOOL    flag;

    flag = (row == 1) && (col >= 1);

    return(flag);
}

/*****************************************************************************/

BOOL    matrix::iscolvector( void )
{
BOOL    flag;

    flag = (row >= 1) && (col == 1);

    return(flag);
}

/*****************************************************************************/

BOOL    matrix::isvector( void )
{
BOOL    flag;

    flag = isrowvector() || iscolvector();

    return(flag);
}

/*****************************************************************************/

BOOL    matrix::isscalar( void )
{
BOOL    flag;

    flag = (row == 1) && (col == 1);

    return(flag);
}

/*****************************************************************************/

BOOL    matrix::isempty( void )                                      /* V2.1 */
{
BOOL    flag;

    flag = ((row * col) == 0);

    return(flag);
}

/*****************************************************************************/

BOOL    matrix::isnotempty( void )                                      /* V2.1 */
{
BOOL    flag;

    flag = ((row * col) != 0);

    return(flag);
}

/*****************************************************************************/

BOOL    matrix::issquare( void )
{
BOOL    flag;

    flag = (row == col);

    return(flag);
}

/*****************************************************************************/

BOOL    matrix::exist( int r, int c )
{
BOOL    flag;

    flag = (r >= 1) && (r <= row) && (c >= 1) && (c <= col);

    return(flag);
}

/*****************************************************************************/

BOOL    matrix::iszero( void )
{
BOOL    flag;
int     i,j;

    for( flag=TRUE,i=1; ((i <= row) && flag); i++ )
    {
        for( j=1; ((j <= col) && flag); j++ )
        {
            flag = (mtx[i][j] == 0.0);
        }
    }

    return(flag);
}

/*****************************************************************************/

char *matrix::dimtext( void )
{
char *text;

    text = STR_stringf("(%d,%d)",rows(),cols());
    return(text);
}

/*****************************************************************************/
/* Matrix type-cast element extractions.                                     */
/*****************************************************************************/

int  matrix::I( int r, int c )
{
static int value;

    value = (float)extract(r,c);
    return(value);
}

/*****************************************************************************/
   
long matrix::L( int r, int c )
{
static long value;

    value = (long)extract(r,c);
    return(value);
}

/*****************************************************************************/

BOOL matrix::B( int r, int c )
{
static BOOL value;

    value = (extract(r,c) != 0.0);
    return(value);
}

/*****************************************************************************/

float matrix::F( int r, int c )
{
static float value;

    value = (float)extract(r,c);
    return(value);
}

/*****************************************************************************/
/* Matrix operators - called to perform various mathematical operations.     */
/*****************************************************************************/

//##ModelId=3EC4C55103C7
void    matrix::operator=( matrix &B )                          // A = B;
{                                                               // Matrix assignment.
#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator=( matrix &B ) Matrix assignment.\n");
#endif

//  Special assignment for empty (NULL) matrix (V2.1)...
    if( B.isempty() )
    {
        if( mtx != NULL )
        {
            dump(mtx);
            mtx = NULL;

            row = 0;
            col = 0;
        }

        return;
    }

    if( (row != B.row) || (col != B.col) )       // Do we need to re-dimension target?
    {
        if( mtx != NULL )
        {
            dump(mtx);                           // Dump existing memory block.
        }

        mtx = NULL;
        row = 0;
        col = 0;

        if( (mtx=dmatrix(B.row,B.col)) == NULL ) // Allocate new memory block.
        {
            MATRIX_errorf("matrix::operator=( matrix &B ) Memory error in matrix[%d][%d]. \n",B.row,B.col);
            return;
        }

        // Set new dimensions. 
        row = B.row;                             
        col = B.col;
    }

    matrix_itmcpy(mtx,B.mtx,row,col);
}

/*****************************************************************************/

void    matrix::operator=( double b )                           // A = (double)b;
{                                                               // Matrix assignment (v2.1).
#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator=( double b ) Matrix assignment.\n");
#endif

//  Free existing matrix memory...
    if( mtx != NULL )
    {
        dump(mtx);
        mtx = NULL;
    }

    mtx = NULL;
    row = 0;
    col = 0;

//  Allocate new memory block...
    if( (mtx=dmatrix(1,1)) == NULL )
    {
        MATRIX_debugf("matrix::operator=( double b ) Memory error in matrix[%d][%d]. \n",1,1);
        return;
    }

//  Set new dimensions...
    row = 1;
    col = 1;

    mtx[1][1] = b;
}

/*****************************************************************************/

void    matrix::operator=( float b )                            // A = (float)b;
{                                                               // Matrix assignment (v2.1).
#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator=( float b ) Matrix assignment.\n");
#endif

//  Free existing matrix memory...
    if( mtx != NULL )
    {
        dump(mtx);
        mtx = NULL;
    }

    mtx = NULL;
    row = 0;
    col = 0;

//  Allocate new memory block...
    if( (mtx=dmatrix(1,1)) == NULL )        
    {
        MATRIX_debugf("matrix::operator=( float b ) Memory error in matrix[%d][%d]. \n",1,1);
        return;
    }

//  Set new dimensions...
    row = 1;
    col = 1;

    mtx[1][1] = (double)b;
}

/*****************************************************************************/

void    matrix::operator=( int b )                              // A = (int)b;
{                                                               // Matrix assignment (v2.1).
#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator=( int b ) Matrix assignment.\n");
#endif

//  Free existing matrix memory...
    if( mtx != NULL )
    {
        dump(mtx);
        mtx = NULL;
    }

    mtx = NULL;
    row = 0;
    col = 0;

//  Allocate new memory block...
    if( (mtx=dmatrix(1,1)) == NULL )        
    {
        MATRIX_debugf("matrix::operator=( float b ) Memory error in matrix[%d][%d]. \n",1,1);
        return;
    }

//  Set new dimensions...
    row = 1;
    col = 1;

    mtx[1][1] = (double)b;
}

/*****************************************************************************/

void matrix::duplicate( matrix &A )
{
int i,j;

    dim(A.rows(),A.cols());

    for( i=1; (i <= A.rows()); i++ )
    {
        for( j=1; (j <= A.cols()); j++ )
        {
            this->mtx[i][j] = A(i,j);
        }
    }
}

/*****************************************************************************/

void matrix::transpose( void )
{
int i,j;
matrix A;

    A.duplicate(*this);
    dim(A.cols(),A.rows());

    for( i=1; (i <= A.rows()); i++ )
    {
        for( j=1; (j <= A.cols()); j++ )
        {
            this->mtx[j][i] = A(i,j);
        }
    }
}

/*****************************************************************************/

void matrix_transpose( matrix &A, matrix &B )                   // A = T(B);
{                                                               // Transpose.
    A.duplicate(B);
    A.transpose();
}

/*****************************************************************************/

void matrix_dim( matrix &mtx, int r, int c )
{
    mtx.dim(r,c);
}    

/*****************************************************************************/

matrix  matrix_dim( int r, int c )
{
MTXRETN mtx;

    mtx.dim(r,c);
    return(mtx);
}

/*****************************************************************************/

matrix  matrix_dim( int r )
{
    return(matrix_dim(r,1));
}

/*****************************************************************************/

void    matrix_dim( matrix &dest, matrix &srce )
{
    dest.dim(srce);
}

/*****************************************************************************/

void    matrix_empty( matrix &mtx )
{
    if( mtx.mtx != NULL )
    {
        dump(mtx.mtx);
        mtx.mtx = NULL;
    }

    mtx.row = 0;
    mtx.col = 0;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix_empty( matrix &mtx ) Dimensioning matrix[%d][%d].\n",mtx.row,mtx.col);
#endif
}

/*****************************************************************************/

void    matrix_scalar( matrix &mtx, double scalar )
{
    matrix_dim(mtx,1,1);
    mtx(1,1) = scalar;
}

/*****************************************************************************/

void    matrix_scalar( matrix &mtx, float scalar )
{
    matrix_scalar(mtx,(double)scalar);
}

/*****************************************************************************/

void    matrix_scalar( matrix &mtx, int scalar )
{
    matrix_scalar(mtx,(double)scalar);
}

/*****************************************************************************/

matrix  scalar( double scalar )
{
MTXRETN C;

    matrix_scalar(C,scalar);
    return(C);

}

/*****************************************************************************/

matrix  scalar( float scalar )
{
MTXRETN C;

    matrix_scalar(C,scalar);
    return(C);
}

/*****************************************************************************/

matrix  scalar( int scalar )
{
MTXRETN C;

    matrix_scalar(C,scalar);
    return(C);
}

/*****************************************************************************/

double  scalar( matrix &mtx )
{
double  scalar=0.0;

    if( !mtx.isscalar() )
    {
        mtx.Errorf("scalar() Scalar (1x1) matrix expected.\n");
    }
    else
    {
        scalar = mtx(1,1);
    }

    return(scalar);
}

/*****************************************************************************/

void    matrix_assign_row( matrix &A, int r, matrix &B )
{
int     i;

    for( i=1; (i <= cols(A)); i++ )
    {
        A(r,i) = B(1,i);
    }
}

/*****************************************************************************/

void    matrix_assign_col( matrix &A, int c, matrix &B )
{
int     i;

    for( i=1; (i <= rows(A)); i++ )
    {
        A(i,c) = B(i,1);
    }
}

/*****************************************************************************/
/* Matrix statistics.                                                        */
/*****************************************************************************/

double   sum( matrix &A )
{
int      i,j;
double   sum=0.0;

    for( i=1; (i <= A.row); i++ )
    {
        for( j=1; (j <= A.col); j++ )
        {
            sum += A.mtx[i][j];
        }
    }

    return(sum);
}

/*****************************************************************************/

void    matrix_sum_row( matrix &sum, matrix &mtx )
{
int     i,j;

    matrix_dim(sum,mtx.row,1);               // Dimension sum matrix (also zero).

    for( i=1; (i <= mtx.row); i++ ) 
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            sum.mtx[i][1] += mtx.mtx[i][j]; 
        }
    }
}

/*****************************************************************************/

void    matrix_sum_col( matrix &sum, matrix &mtx )
{
int     i,j;

    matrix_dim(sum,1,mtx.col);              // Dimension sum matrix (also zeros).

    for( i=1; (i <= mtx.col); i++ ) 
    {
        for( j=1; (j <= mtx.row); j++ )
        {
            sum.mtx[1][i] += mtx.mtx[j][i]; 
        }
    }
}

/*****************************************************************************/

void    matrix_mean_col( matrix &mean, matrix &mtx )
{
MTXWORK sum;

    matrix_sum_col(sum,mtx);
    matrix_divide(mean,sum,(double)mtx.row);
}    

/*****************************************************************************/

void    matrix_mean_row( matrix &mean, matrix &mtx )
{
MTXWORK sum;

    matrix_sum_row(sum,mtx);
    matrix_divide(mean,sum,(double)mtx.col);
}    

/*****************************************************************************/

matrix  rowsum( matrix &mtx )
{
MTXRETN sum;

    matrix_sum_row(sum,mtx);
    return(sum);
}

/*****************************************************************************/

matrix  colsum( matrix &mtx )
{
MTXRETN sum;

    matrix_sum_col(sum,mtx);
    return(sum);
}

/*****************************************************************************/
 
double  prod( matrix &mtx )
{
int     i,j;
double  prod=1.0;
  
    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            prod *= mtx.mtx[i][j];
        }
    }
  
    return(prod);
}

/*****************************************************************************/

double  mmin( matrix &mtx )
{
int     i,j;
double  min;

    if( !mtx.isempty() )
    {
        min = mtx.mtx[1][1];
    }

    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            if( mtx.mtx[i][j] < min )
            {
                min = mtx.mtx[i][j];
            }
        }
    }

    return(min);
}

/*****************************************************************************/

double  mmax( matrix &mtx )
{
int     i,j;
double  max;

    if( !mtx.isempty() )
    {
        max = mtx.mtx[1][1];
    }

    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            if( mtx.mtx[i][j] > max )
            {
                max = mtx.mtx[i][j];
            }
        }
    }

    return(max);
}

/*****************************************************************************/

double  mean( matrix &mtx )
{
MTXWORK A,B;
double  mean;

//   mean = (colmean(rowmean(mtx))(1,1);
//   A = rowmean(mtx);
//   B = colmean(A);
//   mean = B(1,1);

     matrix_mean_row(A,mtx);
     matrix_mean_col(B,A);
     mean = B(1,1);

     return(mean);
}

/*****************************************************************************/

matrix  colmean( matrix &mtx )
{ 
MTXRETN mean;

    matrix_mean_col(mean,mtx);
    return(mean);
}

/*****************************************************************************/

matrix  rowmean( matrix &mtx )
{
MTXRETN mean;

    matrix_mean_row(mean,mtx);
    return(mean);
}

/*****************************************************************************/

double  var( matrix &mtx )
{
MTXWORK work;
double  var=0.0;

    if( mtx.isrowvector() )
    {
        matrix_var_row(work,mtx);
        var = work(1,1);
    }
    else
    if( mtx.iscolvector() )
    {
        matrix_var_col(work,mtx);
        var = work(1,1);
    }
    else
    {
        ERR1("double var( matrix &mtx )",mtx);
    }

    return(var);
}

/*****************************************************************************/

void    matrix_var_col( matrix &var, matrix &mtx )
{
MTXWORK A,B,C,D;

//  var = colmean(mtx ^ mtx) - (colmean(mtx) ^ colmean(mtx));
//  A = mtx ^ mtx;
//  B = colmean(mtx);
//  C = colmean(A);
//  D = B ^ B;
//  var = C - D;

    matrix_multipew(A,mtx,mtx);
    matrix_mean_col(B,mtx);
    matrix_mean_col(C,A);
    matrix_multipew(D,B,B);
    matrix_subtract(var,C,D);
}

/*****************************************************************************/

void    matrix_var_row( matrix &var, matrix &mtx )
{ 
MTXWORK A,B,C,D;

//  var = rowmean(mtx ^ mtx) - (rowmean(mtx) ^ rowmean(mtx));
//  A = mtx ^ mtx;
//  B = rowmean(mtx);
//  C = rowmean(A);
//  D = B ^ B;
//  var = C - D;

    matrix_multipew(A,mtx,mtx);
    matrix_mean_row(B,mtx);
    matrix_mean_row(C,A);
    matrix_multipew(D,B,B);
    matrix_subtract(var,C,D);
}

/*****************************************************************************/

void    matrix_var_cov( matrix &var, matrix &mtx1, matrix &mtx2 )
{
MTXWORK A,B1,B2,C,D;
BOOL    ok=FALSE;

//  var = colmean(mtx1 ^ mtx2) - (colmean(mtx1) ^ colmean(mtx2));
//  A = mtx1 ^ mtx2;
//  B1 = colmean(mtx1);
//  B2 = colmean(mtx2);
//  C = colmean(A);
//  D = B1 ^ B2;
//  var = C - D;

    if( (mtx1.col != mtx2.col) || (mtx1.row != mtx2.row) )
    {
        ERR2("void matrix_cov( matrix &var, matrix &mtx1, matrix &mtx2 )",mtx1,mtx2);
        return;
    }

    if( A.iscolvector() ) 
    {
        matrix_multipew(A,mtx1,mtx2);
        matrix_mean_col(B1,mtx1);
        matrix_mean_col(B2,mtx2);
        matrix_mean_col(C,A);
        matrix_multipew(D,B1,B2);
        matrix_subtract(var,C,D);
    }
    else
    if( A.isrowvector() )
    {
        matrix_multipew(A,mtx1,mtx2);
        matrix_mean_row(B1,mtx1);
        matrix_mean_row(B2,mtx2);
        matrix_mean_row(C,A);
        matrix_multipew(D,B1,B2);
        matrix_subtract(var,C,D);
    }
}

/*****************************************************************************/

matrix  colvar( matrix &mtx )
{ 
MTXRETN var;

    matrix_var_col(var,mtx);
    return(var);
}

/*****************************************************************************/

matrix  rowvar( matrix &mtx )
{ 
MTXRETN var;

    matrix_var_row(var,mtx);
    return(var);
}

/*****************************************************************************/

matrix  cov( matrix &A, matrix &B )
{
MTXRETN var;

    matrix_var_cov(var,A,B);
    return(var);
}

/*****************************************************************************/

double  matrix_median( matrix &srce )
{
matrix  sorted;
double  median=0.0;
int     m;

    if( srce.isscalar() )
    {
        return(srce(1,1));
    }

    matrix_sort(sorted,srce);

    if( srce.isrowvector() ) 
    {
        m = srce.col / 2;

        if( (srce.col%2) != 0 )
        {
            median = sorted(1,m+1);
        }
        else
        {
            median = (sorted(1,m) + sorted(1,m+1)) / 2.0;
        }
    }
    else
    if( srce.iscolvector() ) 
    {
        m = srce.row / 2;

        if( (srce.row%2) != 0 )
        {
            median = sorted(m+1,1);
        }
        else
        {
            median = (sorted(m,1) + sorted(m+1,1)) / 2.0;
        }
    }

    return(median);
}

/*****************************************************************************/


/******************************************************************/ 
#ifndef DOS
matrix smooth(matrix& p,double sigma,int bins)
{
  double f,sum;
  double from,to,step;
  int i,j;
  matrix r(bins,2);

  from=mmin(p);
  to=mmax(p);
  step=(to-from)/(double)bins;

  for(i=1;i<=bins;i++)
    {
      f=from+(double)i*step;
      sum=0;
      
      for(j=1;j<=rows(p);j++) 
	sum+=exp(-0.5*sqr((p(j,1)-f)/sigma));
      
      r(i,1)=f;
      r(i,2)=sum/((double)rows(p)*sigma*sqrt(2.0*M_PI));
    }
  return r;
}

/******************************************************************/

matrix smooth(matrix& p,matrix& cov,int bins)
{
  int d=cols(p);
  matrix f(d);
  matrix invcov=inv(cov);
  double sum;

  double from[d+1],step[d+1];
  int count[d+1];

  int i,k=0;
  matrix r(pow(bins,d),d+1);
  
  float dsigma=fabs(1.0/(float)det(cov));
  
  for(i=1;i<=d;i++) 
    {
      from[i]=mmin(p[i]);
      step[i]=(max(p[i])-from[i])/(double)bins;
    }  

  for(i=1;i<=d;i++) count[i]=1;
  
  do 
    {
      for(i=1;i<=d;i++)   f(i,1)=from[i] + count[i]*step[i];
      
      sum=0;
      
      for(i=1;i<rows(p);i++) 
	sum+=exp(-0.5*sqr(~(~p(i)-f)*invcov*(~p(i)-f))(1,1));
      
      k++;
      for(i=1;i<=d;i++) r(k,i)=f(i,1);
      r(k,d+1)=sum; //((double)rows(p)*sqrt(2.0*M_PI))/dsigma;

      count[1]++;
      
      for(i=1;i<d;i++)  
	if(count[i]>bins) 
	  {
	    count[i]=1;
	    count[i+1]++;
	  }
    }
  while(count[d]<=bins);
  return r;
}

/******************************************************************/

matrix smooth(matrix& p,matrix &w,double sigma,int bins)
{
  double f,tsum;
  double from,to,step;
  int i,j;
  matrix r(bins,2);
  
  from=mmin(p);
  to=mmax(p);
  step=(to-from)/(double)bins;
  float g;
  
  for(i=1;i<=bins;i++)
    {
      f=from+(double)i*step;
      tsum=0;
      g=0;
      
      for(j=1;j<=rows(p);j++)
	{
	tsum+=w(j,1)*exp(-0.5*sqr((p(j,1)-f)/sigma));
	g+=exp(-0.5*sqr((p(j,1)-f)/sigma));
	}
      r(i,1)=f;
      r(i,2)=tsum/g;
    }
  return r;
}
#endif

/*****************************************************************************/

int     rows( matrix &mtx )
{
    return(mtx.rows());
}

/*****************************************************************************/

int     cols( matrix &mtx )
{
    return(mtx.cols());
}

/*****************************************************************************/
/* Matrix maths routines.                                                    */
/*****************************************************************************/

void    matrix_abs( matrix &result, matrix &mtx )
{
int     i,j;

    matrix_dim(result,mtx.row,mtx.col);

    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            result.mtx[i][j] = fabs(mtx.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

matrix  abs( matrix &mtx )
{
MTXRETN result;

    matrix_abs(result,mtx);
    return(result);
}

/*****************************************************************************/

void    matrix_mpow( matrix &result, matrix &mtx, int power )
{
int     i;

    if( mtx.issquare() )
    {
        result = mtx;

        for( i=1; (i < power); i++ )
        {
            result *= mtx;
        }
    }
    else
    {
        ERR1("void matrix_mpow( ) Square matrix expected",mtx);
    }
}

/*****************************************************************************/
 
matrix  mpow( matrix &mtx, int power )
{
MTXRETN result;

    matrix_pow(result,mtx,(double)power);
    return(result);
}

/*****************************************************************************/

void    matrix_pow( matrix &result, matrix &mtx, double power )
{
int     i,j;

    matrix_dim(result,mtx.row,mtx.col);

    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            result.mtx[i][j] = pow(mtx.mtx[i][j],power);
        }
    }
}

/*****************************************************************************/

matrix  pow( matrix &mtx, double power )
{
MTXRETN result;

    matrix_pow(result,mtx,power);
    return(result);
}

/*****************************************************************************/

void    matrix_sqrt( matrix &result, matrix &mtx )
{
int     i,j;

    matrix_dim(result,mtx.row,mtx.col);

    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            result.mtx[i][j] = sqrt(mtx.mtx[i][j]);
        }
    }
}
  
/*****************************************************************************/

matrix  sqrt( matrix &mtx )
{
MTXRETN result;

    matrix_sqrt(result,mtx);
    return(result);
}

/*****************************************************************************/

void    matrix_sgn( matrix &result, matrix &mtx )
{
int     i,j;

    matrix_dim(result,mtx.row,mtx.col);
  
    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            if( mtx.mtx[i][j] < 0.0 )
            {
                result.mtx[i][j] = -1.0;
            }
            else
            if( mtx.mtx[i][j] > 0.0 )
            {
                result.mtx[i][j] = 1.0;
            }
        }
    }
}

/*****************************************************************************/
 
matrix  sgn( matrix &mtx )
{
MTXRETN result;

    matrix_sgn(result,mtx);
    return(result);
}

/*****************************************************************************/

void    matrix_I( matrix &mtx, int n )
{
int     i;

    matrix_dim(mtx,n,n);

    for( i=1; (i <= n); i++ )
    {
        mtx.mtx[i][i] = 1.0;
    }
}

/*****************************************************************************/

void    matrix_I( matrix &mtx )
{
int     x,y;

    if( mtx.rows() != mtx.cols() )
    {
        return;
    }

    for( x=1; (x <= mtx.rows()); x++ )
    {
        for( y=1; (y <= mtx.cols()); y++ )
        {
            mtx.mtx[x][y] = (x == y) ? 1.0 : 0.0;
        }
    }
}

/*****************************************************************************/

matrix  I( int n )
{
MTXRETN result;

    matrix_I(result,n);
    return(result);
}

/*****************************************************************************/

void    matrix_rotate2D( matrix &mtx, double theta )
{

    mtx.dim(2,2);
    mtx.zeros();

    mtx(1,1) = mtx(2,2) = cos(theta);
    mtx(1,2) = sin(theta);
    mtx(2,1) =-mtx(1,2);
}

/*****************************************************************************/

matrix  rotate2D( double theta )
{
MTXRETN  C;

    matrix_rotate2D(C,theta);
    return(C);
}

/*****************************************************************************/

void matrix_linspace( matrix &mtx, double a, double b, int n )
{
int i;
double step,d;

    // Dimension matrix to be a row vector.
    mtx.dim(n,1);
    mtx.zeros();

    // Calculate step size for increments.
    step = (b-a) / (double)(n-1);

    // Loop over elements filling matrix with values.
    for( d=a,i=1; (i <= n); d+=step,i++ )
    {
        mtx(i,1) = d;
    }
}

/*****************************************************************************/

matrix linspace( double a, double b, int n )
{
MTXRETN C;

    matrix_linspace(C,a,b,n);
    return(C);
}

/*****************************************************************************/

void matrix_incspace( matrix &mtx, double a, double b, double step )
{
int n;

    // Calculate number of elements.
    n = 1 + (int)((b-a) / step);

    // Check that we get to final value.
    if( b != (a+((n-1)*step)) )
    {
        MATRIX_errorf("matrix_incspace(...) Invalid values.\n");
        return;
    }

    // Generate matrix of elements.
    matrix_linspace(mtx,a,b,n);
}

/*****************************************************************************/

void matrix_incspace( matrix &mtx, int a, int b, int step )
{
    matrix_incspace(mtx,(double)a,(double)b,(double)step);
}

/*****************************************************************************/

void matrix_incspace( matrix &mtx, int a, int b )
{
    matrix_incspace(mtx,a,b,1);
}

/*****************************************************************************/

matrix incspace( double a, double b, double step )
{
MTXRETN C;

    matrix_incspace(C,a,b,step);
    return(C);
}

/*****************************************************************************/

matrix incspace( int a, int b, int step )
{
MTXRETN C;

    matrix_incspace(C,a,b,step);
    return(C);
}

/*****************************************************************************/

matrix incspace( int a, int b )
{
MTXRETN C;

    matrix_incspace(C,a,b);
    return(C);
}

/*****************************************************************************/

void    matrix_diag( matrix &mtx, matrix &p )
{
int     i,j;

    if( p.iscolvector() )
    {
        matrix_dim(mtx,p.row,p.row);

        for( i=1; (i <= p.row); i++ )
        {
            mtx.mtx[i][i] = p.mtx[i][1];
        }
    }
    else
    if( p.isrowvector() )
    {
        matrix_dim(mtx,p.col,p.col);

        for( i=1; (i <= p.col); i++ )
        {
            mtx.mtx[i][i] = p.mtx[1][i];
        }
    }
    else
    if( p.issquare() )
    {
        matrix_dim(mtx,p.row,1);

        for( i=1; (i <= p.row); i++ )
        {
            mtx.mtx[i][1] = p.mtx[i][i];
        }
    }
    else
    {
        ERR1("void matrix_diag(...)",p);
    }
}


/*****************************************************************************/

matrix  diag( matrix &p )
{
MTXRETN mtx;

    matrix_diag(mtx,p);
    return(mtx);
}


/*****************************************************************************/
/* Matrix specific math.                                                     */
/*****************************************************************************/

double  det( matrix &p )
{
MTXFLT  a;
VECINT  ix;
float   d;
int     j,i,N;
  
    N = p.row;
    ix = ivector(N);
    a = fmatrix(N,N);
  
    for( i=1; (i <= N); i++ ) 
    {
        for( j=1; (j <= N); j++ )
        {
            a[i][j] = (float)p.mtx[i][j];
        }
    }
  
    if( singular(a,N) )
    {
        return(0.0);
    }

    ludcmp(a,N,ix,&d);

    for( j=1; (j <= N); j++ )
    {
        d *= a[j][j];
    }

    dump(a);
    dump(ix);
  
    return(d);
}

/*****************************************************************************/

void    matrix_inverse( matrix &mtx, matrix &p )
{
MTXFLT  a;
VECFLT  column;
VECINT  indx;
float   d;
int     j,i,N;

    matrix_dim(mtx,p.row,p.col);
 
/* if(fabs(det(p))<1e-6)
{
printf("matrix_inverse() singular matrix returning identity\n");
mtx=I(p.row);
return ;
}*/

    if( !p.issquare() )
    {
        ERR1("void matrix_inverse(...)",p);
        return;
    }
  
    N = p.row;
  
    column = fvector(N);
    indx = ivector(N);
    a = fmatrix(N,N);  
  
    for( i=1; (i <= N); i++ ) 
    {
         for( j=1; (j <= N); j++ )
         {
             a[i][j] = (float)p.mtx[i][j];
         }
    }
  
    ludcmp(a,N,indx,&d);
  
    for( j=1; (j <= N); j++ )
    {
         for( i=1; (i <= N); i++ )
         {
             column[i] = (i == j) ? 1.0 : 0.0;
         }

         lubksb(a,N,indx,column);

         for( i=1; (i <= N); i++ )
         {
             mtx.mtx[i][j] = (double)column[i];
         }
    }
  
    dump(a); 
    dump(column);
    dump(indx);
}

/*****************************************************************************/

matrix  inv( matrix &p )
{
MTXRETN mtx;

    matrix_inverse(mtx,p);
    return(mtx);
}

/*****************************************************************************/

void    matrix_eigenvec( matrix &mtx, matrix &p )
{
MTXWORK T1,T2,T3;
MTXFLT  a,v;
VECFLT  d;
int     j,i,nrot,N;

    matrix_dim(mtx,p.row,p.col);

    if( !p.issquare() )
    {
        ERR1("void matrix_eigenvec(...) Wrong dimensions",p);
        return;
    }

    // T3 = abs(p - T(p));
    matrix_transpose(T1,p);
    matrix_subtract(T2,p,T1);
    matrix_abs(T3,T2);

    if( sum(T3) != 0 )
    {
        ERR1("void matrix_eigenvec(...) Not symmetric",p);
        return;
    }

    N = p.row;
  
    d = fvector(N);
    a = fmatrix(N,N);  
    v = fmatrix(N,N);  
  
    for( i=1; (i <= N); i++ ) 
    {
        for( j=1; (j <= N); j++ )
        {
             a[i][j] = (float)p.mtx[i][j];
        }
    }
  
    jacobi(a,N,d,v,&nrot);
    eigsrt(d,v,N);

    for( i=1; (i <= N); i++ ) 
    {
        for( j=1; (j <= N); j++ )
        {
             mtx.mtx[i][j] = (double)v[i][j];
        }
    }
  
    dump(a);
    dump(v);
    dump(d);
}

/*****************************************************************************/

matrix  eigenvec( matrix &p )
{
MTXRETN mtx;

    matrix_eigenvec(mtx,p);
    return(mtx);
}

/*****************************************************************************/
 
void    matrix_eigenval( matrix &mtx, matrix &p )
{
MTXWORK T1,T2,T3;
MTXFLT  a,v;
VECFLT  d;
int     j,i,nrot,N;

    matrix_dim(mtx,p.row,1);

    if( !p.issquare() )
    {
        ERR1("void matrix_eigenval(...) Wrong dimensions",p);
        return;
    }

    matrix_transpose(T1,p);
    matrix_subtract(T2,p,T1);
    matrix_abs(T3,T2);

    if( sum(T3) != 0 )
    {
        ERR1("void matrix_eigenval(...) Not diagonal",p);
        return;
    }

    N = p.row;
  
    d = fvector(N);
    a = fmatrix(N,N);  
    v = fmatrix(N,N);  

    for( i=1; (i <= N); i++ ) 
    {
        for( j=1; (j <= N); j++ )
        {
             a[i][j] = (float)p.mtx[i][j];
        }
    }

    jacobi(a,N,d,v,&nrot);
    eigsrt(d,v,N);
  
    for( i=1; (i <= N); i++ )
    {
        mtx(i,1) = (double)d[i];
    }
  
    dump(a);
    dump(v);
    dump(d);
}

/*****************************************************************************/

matrix  eigenval( matrix &p )
{
MTXRETN mtx;

    matrix_eigenval(mtx,p);
    return(mtx);
}

/*****************************************************************************/

void    svd( matrix &p, matrix &U, matrix &S, matrix &V )
{
MTXFLT  a,v;
VECFLT  w;
int     i,j;
int     M=p.row;
int     N=p.col;

    matrix_dim(U,M,N);
    matrix_dim(V,N,N);
    matrix_dim(S,N,1);
  
    a = fmatrix(M,N);  

    for( i=1; (i <= M); i++ ) 
    {
        for( j=1; (j <= N); j++ )  
        {
            a[i][j] = (float)p(i,j);
        }
    }
  
    w = fvector(N);
    v = fmatrix(N,N);

    svdcmp(a,M,N,w,v);

    for( i=1; (i <= M); i++ ) 
    {
        for( j=1; (j <= N); j++ )  
        {
            U(i,j) = (double)a[i][j];
        }
    }

    for( i=1; (i <= N); i++ )
    {
        for( j=1; (j <= N); j++ )  
        {
            V(i,j) = (double)v[i][j];
        }
    }
  
    for( i=1; (i <= N); i++ )
    {
        S(i,1) = (double)w[i];
    }
    
    S = diag(S);

    dump(v);
    dump(w);
    dump(a);
} 

/*****************************************************************************/

#include "mtx-memory.cpp"              // Matrix (and vector) memory allocation.
#include "mtx-extract.cpp"             // Matrix extraction.
#include "mtx-compare.cpp"             // Matrix logical comparisons.
#include "mtx-multiply.cpp"            // Matrix multiplication.
#include "mtx-divide.cpp"              // Matrix division.
#include "mtx-add.cpp"                 // Matrix addition.
#include "mtx-subtract.cpp"            // Matrix subtraction.
#include "mtx-functions.cpp"           // Matrix apply functions.
#include "mtx-maths.cpp"               // General maths functions.
#include "mtx-concat.cpp"              // Matrix concatenation (append columns or rows).
#include "mtx-file.cpp"                // Matrix file I/O functions.
#include "mtx-data.cpp"                // Matrix special data I/O functions.
#include "mtx-display.cpp"             // Matrix display functions.
#include "mtx-sort.cpp"                // Matrix sort functions.

/*****************************************************************************/
/* Matrix initialization.                                                    */
/*****************************************************************************/

void    matrix::fill( double value )
{
int     i,j;

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            mtx[i][j] = value;
        }
    }
}

/*****************************************************************************/

void    matrix::zeros( void )
{
    fill(0.0);
}

/*****************************************************************************/

void    matrix::ones( void )
{
    fill(1.0);
}

/*****************************************************************************/

matrix fill( int row, int col, double value )
{
MTXRETN mtx;

    matrix_dim(mtx,row,col);
    mtx.fill(value);

    return(mtx);
}

/*****************************************************************************/

matrix fill( matrix &mtx, double value )
{
MTXRETN A;

    A = fill(mtx.rows(),mtx.cols(),value);

    return(A);
}

/*****************************************************************************/

matrix zeros( int row, int col )
{
MTXRETN mtx;

    mtx.dim(row,col);
    mtx.zeros();

    return(mtx);
}

/*****************************************************************************/

matrix zeros( matrix &mtx )
{
MTXRETN A;

    A = zeros(mtx.rows(),mtx.cols());

    return(A);
}

/*****************************************************************************/

matrix ones( int row, int col )
{
MTXRETN mtx;

    mtx.dim(row,col);
    mtx.ones();

    return(mtx);
}

/*****************************************************************************/

matrix ones( matrix &mtx )
{
MTXRETN A;

    A = ones(mtx.rows(),mtx.cols());

    return(A);
}

/*****************************************************************************/

void matrix_rand( matrix &mtx, double min, double max )
{
int i,j;

    for( i=1; (i <= mtx.rows()); i++ )
    {
        for( j=1; (j <= mtx.cols()); j++ ) 	
        {
            mtx(i,j) = drand(min,max);
        }
    }
}

/*****************************************************************************/

void matrix_rand( matrix &mtx )
{
    matrix_rand(mtx,0.0,1.0);
}

/*****************************************************************************/

matrix rand( int row, int col, double min, double max )
{
MTXRETN C;

    C.dim(row,col);
    matrix_rand(C,min,max);
    return(C);
}

/*****************************************************************************/

matrix rand( int row, int col )
{
MTXRETN C;

    C.dim(row,col);
    matrix_rand(C,0.0,1.0);
    return(C);
}

/*****************************************************************************/

void matrix_randn( matrix &mtx, double m, double sd )
{
int i,j;

    for( i=1; (i <= mtx.rows()); i++ )
    {
        for( j=1; (j <= mtx.cols()); j++ )
        {
            mtx(i,j) = gauss(m,sd);
        }
    }
}

/*****************************************************************************/

void matrix_randn( matrix &mtx )
{
    matrix_randn(mtx,0.0,1.0);
}

/*****************************************************************************/

matrix randn( int row, int col, double m, double sd )
{
MTXRETN C;

    C.dim(row,col);
    matrix_randn(C,m,sd);
    return(C);
}

/*****************************************************************************/

matrix randn( int row, int col )
{
MTXRETN C;

    C.dim(row,col);
    matrix_randn(C,0.0,1.0);
    return(C);
}

/*****************************************************************************/

BOOL    singular( MTXFLT a, int n )
{
int     i,j;
float   max,f;
BOOL    flag;
  
    for( flag=FALSE,i=1; ((i <= n) && !flag); i++ ) 
    {
        for( max=0.0,j=1; (j <= n); j++ )
        {
            if( (f=fabs(a[i][j])) > max )
            {
                max = f;
            }
        }

        flag = (max == 0.0);
    }

    return(flag);
}

/*****************************************************************************/

double  matrix::norm( void )
{
MTXWORK T1,T2;
double  d=0.0;

    if( isvector() )
    {
        d = sqrt(sum(*this ^ *this));
    }
    else
    {
        ERR1("double mtx.norm( void ) Vector expected.",*this);
    }

    return(d);
}

/*****************************************************************************/

double  norm( matrix &mtx )
{
double n;

    n = mtx.norm();

    return(n);
}

/*****************************************************************************/

matrix  rownorm( matrix &mtx )
{
MTXRETN A;

    A = sqrt(rowsum(mtx ^ mtx));

    return(A);
}

/*****************************************************************************/

matrix  colnorm( matrix &mtx )
{
MTXRETN A;

    A = sqrt(colsum(mtx ^ mtx));

    return(A);
}

/*****************************************************************************/

void    matrix_sqrtm( matrix &mtx, matrix &x )
{
matrix eval,evec;

  eval = eigenval(x);

  if( mmin(eval) < 0.0 )
  {
    ERR0("matrix_sqrtm() Negative eigenvalues.\n");
    return;
  }

  evec = eigenvec(x);

  mtx = evec * diag(apply(eval,sqrtf)) * inv(evec);
}

/*****************************************************************************/

matrix  sqrtm( matrix &x )
{
MTXRETN mtx;

    matrix_sqrtm(mtx,x);
    return(mtx);
}

/*****************************************************************************/

void    matrix_mgauss( matrix &mtx, matrix &m, matrix &c )
{
MTXWORK T1,T2,T3;

    matrix_dim(T1,m.row,1);
    matrix_randn(T1);
    matrix_sqrtm(T2,c);
    matrix_multiply(T3,T2,T1);
    matrix_add(mtx,m,T3);
}

/*****************************************************************************/

matrix  mgauss( matrix &m, matrix &c )
{
MTXRETN mtx;

    matrix_mgauss(mtx,m,c);
    return(mtx);
}

/*****************************************************************************/

void    matrix_mselect( matrix &mtx, int n, int N )
{
int     k=0,r=0;
double  p;

    matrix_dim(mtx,n,1);

    for( k=1,r=0; (r < n); k++ )
    {
        p = (double)(n-r) / (double)(N-k);
        
        if( randomflip(p) )
        {
            mtx(++r,1) = k;
        }
    }
}

/*****************************************************************************/

matrix  mselect( int n, int N )
{
MTXRETN C;

    matrix_mselect(C,n,N);
    return(C);
}

/*****************************************************************************/

void    matrix_permute( matrix &B, matrix &A )
{
MTXWORK C,D;
int     i,j;

    matrix_memcpy(B,A);

    for( i=A.row; (i >= 1); i-- )
    {
        j = irand(1,i);

        matrix_extract_row(C,B,j);
        matrix_extract_row(D,B,i);

        matrix_assign_row(B,j,D);
        matrix_assign_row(B,i,C);
    }
}

/*****************************************************************************/

matrix  permute( matrix &A )
{
MTXRETN C;

    matrix_permute(C,A);
    return(C);
}

/*****************************************************************************/

matrix  T( matrix &A )
{
MTXRETN C;

    matrix_transpose(C,A);
    return(C);
}

/*****************************************************************************/

void    matrix_perpend( matrix &mtx, matrix &A, matrix &B )
{
MTXWORK C;
double  n;

    matrix_crossprod(C,A,B);

    n = norm(C);

    if( n == 0.0 )
    {
        ERR2("void matrix_perpend( matrix &C, matrix &A, matrix &B ) Divide by zero.",A,B);
    }
    else
    {
        matrix_divide(mtx,C,n);
    }
}

/*****************************************************************************/

matrix  perpend( matrix &A, matrix &B )
{
MTXRETN C;

    matrix_perpend(C,A,B);
    return(C);
}

/*****************************************************************************/

void    matrix_tangent( matrix &mtx, matrix &A, matrix &B )
{
MTXWORK C;
double  n;

    matrix_subtract(C,A,B);
    n = norm(C);

    if( n == 0.0 )
    {
        ERR2("void matrix_tangent( matrix &C, matrix &A, matrix &B ) Divide by zero.",A,B);
    }
    else
    {
        matrix_divide(mtx,C,n);
    }
}

/*****************************************************************************/

matrix  tangent( matrix &A, matrix &B )
{
MTXRETN C;

    matrix_tangent(C,A,B);
    return(C);
}

/*****************************************************************************/

void    matrix_crossprod( matrix &C, matrix &A, matrix &B )
{
    matrix_dim(C,3,1);

    C(1,1) = -(A(3,1) * B(2,1)) + (A(2,1) * B(3,1));
    C(2,1) =  (A(3,1) * B(1,1)) - (A(1,1) * B(3,1));
    C(3,1) = -(A(2,1) * B(1,1)) + (A(1,1) * B(2,1));
}

/*****************************************************************************/

matrix  crossprod( matrix &p, matrix &q )
{
MTXRETN C;

    matrix_crossprod(C,p,q);
    return(C);
}

/*****************************************************************************/

double  dotprod( matrix &A, matrix &B )
{
MTXWORK T;
double  dp;

//  dp = sum(A^B);

    matrix_multipew(T,A,B);
    dp = sum(T);

    return(dp);
}

/*****************************************************************************/

void    matrix_plane( matrix &mtx, matrix &p, matrix &q, matrix &r, matrix &x )
{
//      Returns the coefficient of the plane Ax + By + Cz + D=0
//      in a column form for the plane which passes through the
//      three points given, x is outside the wall.
  
MTXWORK T1,T2,T3;
float   p1=p(1,1),p2=p(2,1),p3=p(3,1);
float   q1=q(1,1),q2=q(2,1),q3=q(3,1);
float   r1=r(1,1),r2=r(2,1),r3=r(3,1);

    matrix_dim(T1,4,1);
  
    T1(1,1) = -(p3*q2) + p2*q3 + p3*r2 - q3*r2 - p2*r3 + q2*r3;
    T1(2,1) = p3*q1 - p1*q3 - p3*r1 + q3*r1 + p1*r3 - q1*r3;
    T1(3,1) = -(p2*q1) + p1*q2 + p2*r1 - q2*r1 - p1*r2 + q1*r2;
    T1(4,1) = p3*q2*r1 - p2*q3*r1 - p3*q1*r2 + p1*q3*r2 + p2*q1*r3 - p1*q2*r3;

//  T3 = T1 / norm(T1(1,1,3,1));
//  T2 = T1(1,1,3,1);
//  T3 = T1 / norm(T2);

    matrix_extract_sub(T2,T1,1,1,3,1);
    matrix_divide(T3,T1,norm(T2));

    if( point2plane(x,T3) < 0.0 )
    {
        matrix_negate(mtx,T3);
    }
    else
    {
        matrix_memcpy(mtx,T3);
    }
}

/*****************************************************************************/

void    matrix_plane( matrix &mtx, matrix &p, matrix &q, matrix &r )
{
MTXWORK x(3,1);

    matrix_plane(mtx,p,q,r,x);
}

/*****************************************************************************/

matrix  plane( matrix &p, matrix &q, matrix &r, matrix &x )
{
MTXRETN C;

    matrix_plane(C,p,q,r,x);
    return(C);

}

/*****************************************************************************/

matrix  plane( matrix &p, matrix &q, matrix &r )
{
MTXRETN C;

    matrix_plane(C,p,q,r);
    return(C);
}

/*****************************************************************************/

double  point2plane( matrix &p, matrix &c )
{
MTXWORK P,C,T;
double  p2p;

//  p2p = sum(p(1,1,3,1) ^ c(1,1,3,1)) + c(4,1);
//  P = p(1,1,3,1);
//  C = c(1,1,3,1);
//  T = P ^ C;                    // Element-wise multiplication.
//  p2p = sum(T) + c(4,1);
    
    matrix_extract_sub(P,p,1,1,3,1);
    matrix_extract_sub(C,c,1,1,3,1);
    matrix_multipew(T,P,C);       // Element-wise multiplication.

    p2p = sum(T) + c(4,1);

    return(p2p);
}

/*****************************************************************************/

void    matrix_interleave( matrix &C, matrix &A, matrix &B )
{
MTXWORK R;
int     i,j;

    matrix_dim(C,A.row+B.row,A.col);

    if( (A.col == B.col) && (A.row == B.row) )
    {
        for( i=1,j=1; (i < A.row); i++ )
        {
            matrix_extract_row(R,A,i);
            matrix_assign_row(C,j++,R);

            matrix_extract_row(R,B,i);
            matrix_assign_row(C,j++,R);
        }
    }
    else
    {
        ERR2("void matrix_interleave( matrix &C, matrix &A, matrix &B )",A,B);
    }
}

/*****************************************************************************/

matrix  interleave( matrix &A, matrix &B )
{
MTXRETN C;

    matrix_interleave(C,A,B);
    return(C);
}

/******************************************************************************/

void matrix::clamp( matrix &minimum, matrix &maximum )
{
int i,j;

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            range(&mtx[i][j],minimum(i,j),maximum(i,j));
        }
    }
}

/******************************************************************************/

void matrix::clamp( double minimum, double maximum )
{
int i,j;

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            range(&mtx[i][j],minimum,maximum);
        }
    }
}


/******************************************************************************/

void matrix::clamp( matrix &absolute )
{
int i,j;

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            range(&mtx[i][j],absolute(i,j));
        }
    }
}

/******************************************************************************/

void matrix::clamp( double absolute )
{
int i,j;

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            range(&mtx[i][j],absolute);
        }
    }
}

/******************************************************************************/

void matrix::clampnorm( double minimum, double maximum )
{
double n;

    // This only works for vectors...
    if( !isvector() )
    {
        return;
    }

    n = norm();

    if( n > maximum )
    {
        *this *= (maximum / n);
    }
    else
    if( n < minimum )
    {
        *this *= (minimum / n);
    }
}

/******************************************************************************/

void matrix::clampnorm( double maximum )
{
    clampnorm(0.0,maximum);
}

/******************************************************************************/

void matrix_flipud( matrix &mtx, matrix &A )
{
int r1,r2,c;

    mtx.dim(A);

    for( r1=1,r2=mtx.rows(); (r1 <= mtx.rows()); r1++,r2-- )
    {
        for( c=mtx.cols(); (c <= mtx.cols()); c++ )
        {
            mtx(r1,c) = A(r2,c);
        }
    }
}

/******************************************************************************/

matrix flipud( matrix &A )
{
MTXRETN mtx;

    matrix_flipud(mtx,A);
    return(mtx);
}

/******************************************************************************/

void matrix_fliplr( matrix &mtx, matrix &A )
{
int r,c1,c2;

    mtx.dim(A);

    for( r=1; (r <= mtx.rows()); r++ )
    {
        for( c1=1,c2=mtx.cols(); (c1 <= mtx.cols()); c1++,c2-- )
        {
            mtx(r,c1) = A(r,c2);
        }
    }
}

/******************************************************************************/

matrix fliplr( matrix &A )
{
MTXRETN mtx;

    matrix_fliplr(mtx,A);
    return(mtx);
}

/******************************************************************************/

