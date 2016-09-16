/******************************************************************************/
/*                                                                            */
/* MODULE  : MATRIX.h                                                         */
/*                                                                            */
/* PURPOSE : Matrix class & related functions - prototypes & definitions.     */
/*                                                                            */
/* DATE    : 22/Aug/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V2.0  JNI 22/Aug/2000 - Development taken over and module re-worked.       */
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
/******************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

/******************************************************************************/

#define M_PI                 3.14159265358979323846
#define M_PI_2               1.57079632679489661923
#define Pi                   3.14159265358979323846264

#define sqr(a)               ((a)*(a))
#define euc(x1,y1,x2,y2)     sqrt(sqr((x1)-(x2))+sqr((y1)-(y2)))
#define mina(a,b)            (a < b ? a : b)
#define maxa(a,b)            (a > b ? a : b)
#define Pi                   3.14159265358979323846264
#define mydisplay(s,d)       dis("<"#s">",s,d);

/*****************************************************************************/

typedef double  *VECDBL;
typedef float   *VECFLT;
typedef int     *VECINT;

typedef double **MTXDBL;
typedef float  **MTXFLT;
typedef int    **MTXINT;

/*****************************************************************************/

#define MTXRETN  matrix           // Local return matrix.
#define MTXWORK  matrix           // Local working matrix.

#define MATRIX_ERROR_MAX 50       // Maximum number of matrix errors to print.

/*****************************************************************************/

#define MTX2D(A,R,C) matrix A(#A,__FILE__,__LINE__,R,C)
#define MTX1D(A,R)   matrix A(#A,__FILE__,__LINE__,R)
#define MTX(A)       matrix A(#A,__FILE__,__LINE__)
#define MTXDUP(A,B)  matrix A(#A,__FILE__,__LINE__,B)

#define MATRIX MATRIX_SetSourceFile(__FILE__,__LINE__); matrix

/*****************************************************************************/

void MATRIX_SetSourceFile( char *file, int line );

/*****************************************************************************/

class   matrix
{
public:

/*****************************************************************************/
//  Data...

    MTXDBL   mtx;                                // Pointer to block of memory for elements.

    int      row;                                // Number of rows in matrix.
    int      col;                                // Number of columns in matrix.

    STRING   ObjectName;                         // Name of matrix.
    STRING   SourceFile;                         // Source file.
    int      SourceLine;                         // Line number.

/*****************************************************************************/
//  Deconstructor...

   ~matrix();                                    // Destroys a matrix.

/*****************************************************************************/
//  Constructors...

    matrix( char *name, char *file, int line, int rows, int columns );
    matrix( char *name, char *file, int line, int rows );
    matrix( char *name, char *file, int line, matrix &source );
    matrix( char *name, char *file, int line );

    matrix( void );                              // Creates a null matrix.
    matrix( int rows );                          // Creates a rows x 1 matrix.
    matrix( int rows, int columns );             // Creates a rows x columns matrix.
    matrix( matrix &source );                    // Creates a copy of matrix A.
    matrix( int rows, int columns, double d, ... );   // Creates a rows x columns matrix and fills it.

/*****************************************************************************/
//  Dimension matrix...

    void init( void );
    void init( char *name, char *file, int line );
    void init( char *name, char *file, int line, int rows, int columns );
    void init( char *name, char *file, int line, matrix &source );

/*****************************************************************************/
//  Dimension matrix...

    void dim( int r, int c );
    void dim( int r );
    void dim( matrix &mtx );

/*****************************************************************************/
// Object print functions...

    char *Name( void );

    int Errorf( const char *mask, ... );
    int Messgf( const char *mask, ... );
    int Debugf( const char *mask, ... );
    int Printf( BOOL ok, const char *mask, ... );

/*****************************************************************************/
//  Information about the matrix...

    int  rows( void );                           // How many rows in the matrix?
    int  cols( void );                           // How many columns in the matrix?
    int  items( void );                          // How many items in the matrix?

    BOOL isrowvector( void );                    // Is the matrix a row vector (rows = 1)?
    BOOL iscolvector( void );                    // Is the matrix a column vector (columns = 1)?
    BOOL isvector( void );                       // Is the matrix a vector?
    BOOL isscalar( void );                       // Is the matrix a scalar point (1x1)?
    BOOL issquare( void );                       // Is the matrix square (rows == columns)?
    BOOL exist( int r, int c );                  // Does element (r,c) exist in matrix?
    BOOL iszero( void );                         // Is the matrix full of zeros?
    BOOL isempty( void );                        // Is the matrix an empty (NULL) matrix? (V2.1)
    BOOL isnotempty( void );                     // Does the matrix contain something ("not empty")? (V2.1)


    char *dimtext( void );                       // Text version of dimensions.

/*****************************************************************************/
//  Extraction operators...

    double& operator()( int r, int c );          // Set [A(1,1)=f;] or Get [f=A(1,1);] single element.
    matrix operator()( int r, matrix &A );       // Extract specific elements in a row.
    matrix operator()( matrix &A, int c );       // Extract specific elements in a column.

    matrix operator[]( int c );                  // Extract a column from the matrix: A = B[c];
    matrix operator()( int r );                  // Extract a row from the matrix:    A = B(r);
    matrix operator()( matrix &A );              // Extracts a submatrix made of multiple rows or columns.

    matrix operator()( int r1, int c1,           // Sub-matrix extraction by boundary.
                       int r2, int c2 );



    //double& extract( int r, int c );

//  These have been removed from the source...Why?
//  friend void operator<<( matrix &q, float  &f );
//  friend void operator<<( float  &f, matrix &q );
//  friend void operator<<( matrix &q, double &f );
//  friend void operator<<( double &f, matrix &q );
//  friend void operator<<( matrix &q, int    &f );
//  friend void operator<<( int    &f, matrix &q );

//  int    matrix::operator int();
//  float  matrix::operator float();
//  double matrix::operator double();

    int    I( int r, int c );
    long   L( int r, int c );
    BOOL   B( int r, int c );
    float  F( int r, int c );

    friend void matrix_assign_row( matrix &A, int r, matrix &B );
    friend void matrix_assign_col( matrix &A, int c, matrix &B );

    //  Mathematical operators...

    void   operator=( matrix & );                     // A = B;
    void   operator=( double   );                     // A = (double)b;
    void   operator=( float    );                     // A = (float)b;
    void   operator=( int      );                     // A = (int)b;

  
      matrix operator-();                               // A = -B;

    // Transpose matrix (rotate it 90 degrees)...
    friend matrix operator~(matrix&);                 // A = ~B;

    // Element-wise division...
    friend matrix operator/( matrix &, matrix & );    // C = A / B;           /* Division by matrix, vector or point. */
    friend matrix operator/( matrix &, double   );    // C = A / (double)b;     
    friend matrix operator/( matrix &, float    );    // C = A / (float)b;     
    friend matrix operator/( matrix &, int      );    // C = A / (int)b;     
  
    void   operator/=( matrix & );                    // A /= B;
    void   operator/=( double   );                    // A /= (double)b;
    void   operator/=( float    );                    // A /= (float)b;
    void   operator/=( int      );                    // A /= (int)b;

    // Element-wise multiplication...
    friend matrix operator^( matrix &A, matrix &B );  // C = A ^ B;

    // Matrix multiplication...

    friend matrix operator*( matrix &A, matrix &B );  // C = A * B;
    friend matrix operator*( double  a, matrix &B );  // C = (double)a * B;
    friend matrix operator*( matrix &A, double  b );  // C = A * (double)b;
    friend matrix operator*( float   a, matrix &B );  // C = (float)a * B;
    friend matrix operator*( matrix &A, float   b );  // C = A * (float)b;
    friend matrix operator*( int     a, matrix &B );  // C = (int)a * B;
    friend matrix operator*( matrix &A, int     b );  // C = A * (int)b;

    void   operator*=( matrix & );                    // A *= B;
    void   operator*=( double   );                    // A *= (double)b;
    void   operator*=( float    );                    // A *= (float)b;
    void   operator*=( int      );                    // A *= (int)b;


    // Addition...

    friend matrix operator+( matrix &, matrix & );
    friend matrix operator+( matrix &, double   );
    friend matrix operator+( double  , matrix & );
    friend matrix operator+( matrix &, float    );
    friend matrix operator+( float   , matrix & );
    friend matrix operator+( matrix &, int      );
    friend matrix operator+( int     , matrix & );
    void   operator+=( matrix & );                    // A += B;
    void   operator+=( double   );                    // A += (double)b;
    void   operator+=( float    );                    // A += (float)b;
    void   operator+=( int      );                    // A += (int)b;

 
    // Subtraction...

    friend matrix operator-( matrix &, matrix & );
    friend matrix operator-( matrix &, double   );
    friend matrix operator-( double  , matrix & );
    friend matrix operator-( matrix &, float    );
    friend matrix operator-( float   , matrix & );
    friend matrix operator-( matrix &, int      );
    friend matrix operator-( int     , matrix & );

    void   operator-=( matrix & );                    // A -= B;
    void   operator-=( double   );                    // A -= (double)b;
    void   operator-=( float    );                    // A -= (float)b;
    void   operator-=( int      );                    // A -= (int)b;

   // Logical comparisons...

    friend matrix operator==( matrix &, matrix & );
    friend matrix operator==( matrix &, double   );
    friend matrix operator==( double  , matrix & );
    friend matrix operator==( matrix &, float    );
    friend matrix operator==( float   , matrix & );
  
    friend matrix operator||( matrix &, matrix & );
    friend matrix operator||( matrix &, double   );
    friend matrix operator||( double  , matrix & );
  
    friend matrix operator&&( matrix &, matrix & );
    friend matrix operator&&( matrix &, double   );
    friend matrix operator&&( double  , matrix & );
  
    friend matrix operator!=( matrix &, matrix & );
    friend matrix operator!=( matrix &, double   );
    friend matrix operator!=( double  , matrix & );

    friend matrix operator>=( matrix &, matrix & );
    friend matrix operator>=( matrix &, double   );
    friend matrix operator>=( double  , matrix & );
 
    friend matrix operator>( matrix &, matrix & );
    friend matrix operator>( matrix &, double   );
    friend matrix operator>( double  , matrix & );

    friend matrix operator<=( matrix &, matrix & );
    friend matrix operator<=( matrix &, double   );
    friend matrix operator<=( double  , matrix & );

    friend matrix operator<( matrix &, matrix & );
    friend matrix operator<( matrix &, double   );
    friend matrix operator<( double  , matrix & );

   // Concatenation columns...

    friend matrix operator|( matrix &, matrix & );    // C = A | B;
    friend matrix operator|( matrix &, double   );    // C = A | (double)b;
    friend matrix operator|( matrix &, float    );    // C = A | (float)b;
    friend matrix operator|( matrix &, int      );    // C = A | (int)b;
    friend matrix operator|( double, matrix & );      // C = (double)b | A;
    friend matrix operator|( float,  matrix & );      // C = (float)b | A;
    friend matrix operator|( int,    matrix & );      // C = (int)b | A;

    void   operator|=( matrix & );                    // A |= B;
    void   operator|=( double   );                    // A |= (double)b;
    void   operator|=( float    );                    // A |= (float)b;
    void   operator|=( int      );                    // A |= (int)b;

    // Concatenation rows...

    friend matrix operator%( matrix &, matrix & );    // C = A % B;
    friend matrix operator%( matrix &, double   );    // C = A % (double)b;
    friend matrix operator%( matrix &, float    );    // C = A % (float)b;
    friend matrix operator%( matrix &, int      );    // C = A % (int)b;
    friend matrix operator%( double, matrix & );      // C = (double)b % A;
    friend matrix operator%( float,  matrix & );      // C = (float)b % A;
    friend matrix operator%( int,    matrix & );      // C = (int)b % A;

    void   operator%=( matrix & );                    // A %= B;
    void   operator%=( double   );                    // A %= (double)b;
    void   operator%=( float    );                    // A %= (float)b;
    void   operator%=( int      );                    // A %= (int)b;

 // Other functions...
    void fill( double value );
    void zeros( void );
    void ones( void );

    // Clamp matrix between minimum and maximum values...
    void clamp( matrix &minimum, matrix &maximum );
    void clamp( double minimum, double maximum );
    void clamp( matrix &absolute );
    void clamp( double absoute );

    // Clamp normal of vector...
    void clampnorm( double minimum, double maximum );
 //mathematical operators
  double norm( void );                         // Normal of a vector.

};

/*****************************************************************************/

#define MEMERR          MATRIX_MemErr(__LINE__,__FILE__)
#define ERR2(F,P,Q)     MATRIX_GenErr((F),(P),(Q))
#define ERR1(F,P)       MATRIX_GenErr((F),(P))
#define ERR0(F)         MATRIX_GenErr((F))

/*****************************************************************************/

void    MATRIX_MemErr( int line, char *file );
void    MATRIX_GenErr( char *text, matrix p, matrix q );
void    MATRIX_GenErr( char *text, matrix p );
void    MATRIX_GenErr( char *text );

/*****************************************************************************/

int     MATRIX_messgf( const char *mask, ... );
int     MATRIX_errorf( const char *mask, ... );
int     MATRIX_debugf( const char *mask, ... );

/*****************************************************************************/
/* Matrix API start / stop functions.                                        */
/*****************************************************************************/

BOOL    MATRIX_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    MATRIX_API_stop( void );
BOOL    MATRIX_API_check( void );

/*****************************************************************************/

void    matrix_constructed( void );
void    matrix_destructed( void );

/*****************************************************************************/
/* VALLOC (Vector ALLOCate) - Allocate memory for vectors and matrices.      */
/*****************************************************************************/

typedef
struct  VALLOC_Header                  // VALLOCated memory block header.
{
    int    type;                       // Type of vector...
#define VALLOC_NULL     0              // Empty (NULL) vector (V2.1)
#define VALLOC_VECDBL   1              // Doubles
#define VALLOC_VECFLT   2              // Floats
#define VALLOC_VECINT   3              // Integers

    size_t elements;                   // Number of elements in vector.
    size_t block;                      // Size of memory block.
}
VALLOC;

#define VALLOC_HEADER   sizeof(VALLOC)

#define valloc(I,S)     VALLOC_make((I),(S),__FILE__,__LINE__)
#define vfree           VALLOC_free
#define vbase           VALLOC_base
#define vitem           VALLOC_elements
#define vsize           VALLOC_block
#define vcopy           VALLOC_copy

void   *VALLOC_make( int type, size_t elements, char *file, int line );
void    VALLOC_free( void *vptr );
void   *VALLOC_base( void *vptr );
VALLOC *VALLOC_head( void *vptr );
int     VALLOC_type( void *vptr );
size_t  VALLOC_elements( void *vptr );
size_t  VALLOC_block( void *vptr );
void    VALLOC_copy( void *dest, void *srce );

/*****************************************************************************/

VECDBL  VECDBL_make( int elements, char *srce, int line ); 
VECFLT  VECFLT_make( int elements, char *srce, int line ); 
VECINT  VECINT_make( int elements, char *srce, int line ); 

MTXDBL  MTXDBL_make( int row, int col, char *srce, int line ); 
MTXFLT  MTXFLT_make( int row, int col, char *srce, int line ); 
MTXINT  MTXINT_make( int row, int col, char *srce, int line ); 

#define dvector(E)      VECDBL_make((E),__FILE__,__LINE__)
#define fvector(E)      VECFLT_make((E),__FILE__,__LINE__)
#define ivector(E)      VECINT_make((E),__FILE__,__LINE__)

#define dmatrix(R,C)    MTXDBL_make((R),(C),__FILE__,__LINE__)
#define fmatrix(R,C)    MTXFLT_make((R),(C),__FILE__,__LINE__)
#define imatrix(R,C)    MTXINT_make((R),(C),__FILE__,__LINE__)

/*****************************************************************************/

void    zero( VECDBL ptr );                      // Zero vector.
void    zero( VECFLT ptr );
void    zero( VECINT ptr );

void    zero( MTXDBL ptr );                      // Zero matrix.
void    zero( MTXFLT ptr );
void    zero( MTXINT ptr );

/*****************************************************************************/

void    dump( VECDBL ptr );                      // Dump vector memory.
void    dump( VECFLT ptr );
void    dump( VECINT ptr );

void    dump( MTXDBL ptr );                      // Dump matrix memory.
void    dump( MTXFLT ptr );
void    dump( MTXINT ptr );

/*****************************************************************************/

void    matrix_fill( matrix &mtx, double val );
void    matrix_zero( matrix &mtx );

/*****************************************************************************/

void    matrix_data( int ROW, int COL, matrix &mtx, double d, ... );
void    matrix_data( matrix &mtx, double d, ... );
void    matrix_data( int ROW, int COL, matrix &mtx, char *data[] );
void    matrix_data( matrix &mtx, char *data[] );
void    matrix_data( int ROW, int COL, matrix &mtx, char *data );
void    matrix_data( matrix &mtx, char *data );

char   *matrix_string( matrix &mtx, char *mask );
char   *matrix_string( matrix &mtx );

void    matrix_array_put( matrix &dest, matrix &srce, int item );
void    matrix_array_get( matrix &dest, matrix &srce, int item );

int     matrix_array_get( matrix dest[], matrix &srce, int max );
int     matrix_array_put( matrix &dest, matrix srce[], int max );

void    matrix_data( matrix &srce, MTXDBL  dest );
void    matrix_data( MTXDBL  srce, matrix &dest );

void    matrix_data( matrix &srce, MTXFLT  dest );
void    matrix_data( MTXFLT  srce, matrix &dest );

void    matrix_data( matrix &srce, MTXINT  dest );
void    matrix_data( MTXINT  srce, matrix &dest );

void    matrix_data( matrix &srce, VECDBL  dest );
void    matrix_data( VECDBL  srce, matrix &dest );

void    matrix_data( matrix &srce, VECFLT  dest );
void    matrix_data( VECFLT  srce, matrix &dest );

void    matrix_data( matrix &srce, VECINT  dest );
void    matrix_data( VECINT  srce, matrix &dest );

#define MATRIX_LINEAR_MAX       128         // Maximum number of elements in linear array.
#define MATRIX_LINEAR_ROW         0         // Row changes more frequently (default).
#define MATRIX_LINEAR_COL         1         // Column changes more frequently.
#define MATRIX_LINEAR_DEFAULT     MATRIX_LINEAR_ROW

//      Produce (and return) various linear arrays for matrix...
double *matrix_double( matrix &mtx, int type );
float  *matrix_float( matrix &mtx, int type );
int    *matrix_int( matrix &mtx, int type );

double *matrix_double( matrix &mtx );
float  *matrix_float( matrix &mtx );
int    *matrix_int( matrix &mtx );

//      Produce various linear arrays for matrix...
void    matrix_double( matrix &mtx, double data[], int type );
void    matrix_float( matrix &mtx, float data[], int type );
void    matrix_int( matrix &mtx, int data[], int type );

void    matrix_double( matrix &mtx, double data[] );
void    matrix_float( matrix &mtx, float data[] );
void    matrix_int( matrix &mtx, int data[] );

//      Produce matrix from various linear arrays...
void    matrix_double( double data[], matrix &mtx, int type );
void    matrix_float( float data[], matrix &mtx, int type );
void    matrix_int( int data[], matrix &mtx, int type );

void    matrix_double( double data[], matrix &mtx );
void    matrix_float( float data[], matrix &mtx );
void    matrix_int( int data[], matrix &mtx );

matrix *M( matrix mtx[], int n );

/*****************************************************************************/

void    matrix_memcpy( matrix &dest, matrix &srce );
void    matrix_itmcpy( matrix &dest, matrix &srce );

void    matrix_memcpy( MTXDBL dest, MTXDBL srce, int row, int col );
void    matrix_itmcpy( MTXDBL dest, MTXDBL srce, int row, int col );

void    matrix_copy( matrix &dest, matrix &srce );

/*****************************************************************************/

BOOL    matrix_dim_ok( int ROW, int COL );

void    matrix_dim( matrix &mtx, int ROW, int COL );
matrix  matrix_dim( int ROW, int COL );
matrix  matrix_dim( int ROW );
void    matrix_dim( matrix &dest, matrix &srce );

void    matrix_empty( matrix &mtx );        // Make the matrix empty (V2.1).

/*****************************************************************************/

#define MATRIX_EXTRACT_MAX      100         // Maximum colum/row for multiple extraction.
#define MATRIX_EXTRACT_END        0         // End of extraction column/row marker.

//      Extract a specific column or row...
void    matrix_extract_col( matrix &dest, matrix &srce, int col );
void    matrix_extract_row( matrix &dest, matrix &srce, int row );

//      Extract a sub-matrix...
void    matrix_extract_sub( matrix &dest, matrix &srce, int r1, int c1, int r2, int c2 );
void    matrix_extract_sub( matrix &dest, matrix &srce, matrix &mask );

matrix  submatrix( matrix &srce, matrix &mask );

//      Extract multiple columns or rows to a sub-matrix...
void    matrix_extract_cols( matrix &dest, matrix &srce, int cols[] );
void    matrix_extract_cols( matrix &dest, matrix &srce, int col, ... );
void    matrix_extract_rows( matrix &dest, matrix &srce, int rows[] );
void    matrix_extract_rows( matrix &dest, matrix &srce, int row, ... );

/*****************************************************************************/

void    matrix_add( matrix &C, matrix &A, matrix &B );          // C = A + B;
void    matrix_add( matrix &C, matrix &A, double  b );          // C = A + b;
void    matrix_add( matrix &C, double  a, matrix &B );          // C = a + B;

void    matrix_subtract( matrix &C, matrix &A, matrix &B );     // C = A - B;
void    matrix_subtract( matrix &C, matrix &A, double  b );     // C = A - b;
void    matrix_subtract( matrix &C, double  a, matrix &B );     // C = a - B;

void    matrix_negate( matrix &B, matrix &A );                  // B = -A;

void    matrix_multiply( matrix &C, matrix &A, double  b );     // C = A * (double)b;
void    matrix_multiply( matrix &C, matrix &A, matrix &B );     // C = A * B;

void    matrix_multipew( matrix &C, matrix &A, matrix &B );     // C = A ^ B; (Element-wise multply)

void    matrix_divide( matrix &C, matrix &A, matrix &B );       // C = A / B;
void    matrix_divide( matrix &C, matrix &A, double  b );       // C = A / b;

/*****************************************************************************/

void    matrix_equal( matrix &C, matrix &A, matrix &B );        // C = (A == B);
void    matrix_equal( matrix &C, matrix &A, double  b );        // C = (A == b);

void    matrix_or( matrix &C, matrix &A, matrix &B );           // C = (A || B);
void    matrix_or( matrix &C, matrix &A, double  b );           // C = (A || b);

void    matrix_and( matrix &C, matrix &A, matrix &B );          // C = (A && B);
void    matrix_and( matrix &C, matrix &A, double  b );          // C = (A && b);

void    matrix_notequal( matrix &C, matrix &A, matrix &B );     // C = (A != B);
void    matrix_notequal( matrix &C, matrix &A, double  b );     // C = (A != b);

void    matrix_greaterequal( matrix &C, matrix &A, matrix &B ); // C = (A >= B);
void    matrix_greaterequal( matrix &C, matrix &A, double  b ); // C = (A >= b);
void    matrix_greaterequal( matrix &C, double  a, matrix &B ); // C = (a >= B);

void    matrix_greater( matrix &C, matrix &A, matrix &B );      // C = (A > B);
void    matrix_greater( matrix &C, matrix &A, double  b );      // C = (A > b);
void    matrix_greater( matrix &C, double  a, matrix &B );      // C = (a > B);

void    matrix_lessequal( matrix &C, matrix &A, matrix &B );    // C = (A <= B);
void    matrix_lessequal( matrix &C, matrix &A, double  b );    // C = (A <= b);
void    matrix_lessequal( matrix &C, double  a, matrix &B );    // C = (a <= B);

void    matrix_less( matrix &C, matrix &A, matrix &B );         // C = (A < B);
void    matrix_less( matrix &C, matrix &A, double  b );         // C = (A < b);
void    matrix_less( matrix &C, double  a, matrix &B );         // C = (a < B);

/*****************************************************************************/

/* DW & JI are here*/

// Apply the function to each element in the matrix...

void    matrix_apply( matrix &result, matrix &mtx, double (*func)(double x) );
void    matrix_apply( matrix &result, matrix &mtx_x, matrix &mtx_y, double (*func)(double x, double y) );
void    matrix_apply( matrix &result, matrix &mtx, float (*func)(float x) );
void    matrix_apply( matrix &result, matrix &mtx_x, matrix &mtx_y, float (*func)(float x, float y) );
void    matrix_apply( matrix &result, matrix &mtx, double (*func)(double x, double y), double y );
void    matrix_apply( matrix &result, matrix &mtx, float (*func)(float x, float y), float y );

matrix  apply( matrix &X, float  (*func)(float  x, float  y),float  y);
matrix  apply( matrix &X, double (*func)(double x, double y),double y );
matrix  apply( matrix &X, double (*func)(double x) );
matrix  apply( matrix &X, float  (*func)(float  x) );
matrix  apply( matrix &X, matrix &Y, double (*func)(double x, double y) );
matrix  apply( matrix &X ,matrix &Y, float  (*func)(float  x, float  y) );

/*****************************************************************************/

void    matrix_concat_col( matrix &C, matrix &A, matrix &B );
void    matrix_concat_col( matrix &C, matrix &A, double  b );
void    matrix_concat_row( matrix &C, matrix &A, matrix &B );
void    matrix_concat_row( matrix &C, matrix &A, double  b );

/*****************************************************************************/

#define display(s,d)    dis("<"#s">",s,d); 
#define disp(s)         dis("<"#s">",s,2); 

void    dis( matrix &mtx );
void    dis( char *s, matrix &mtx );
void    dis( char *s, matrix &mtx, int sig );
void    dis( char *s, double val, int sig );

/*****************************************************************************/

matrix  scalar( double val );
matrix  scalar( float val );
matrix  scalar( int val );

double  scalar( matrix &mtx );

void    matrix_scalar( matrix &mtx, double val );
void    matrix_scalar( matrix &mtx, float val );
void    matrix_scalar( matrix &mtx, int val );

double  sum( matrix &A );

void    matrix_sum_row( matrix &sum, matrix &mtx );
matrix  rowsum( matrix &mtx );

void    matrix_sum_col( matrix &sum, matrix &mtx );
matrix  colsum( matrix &mtx );

double  prod( matrix &mtx );

double  mmin( matrix &mtx );
double  mmax( matrix &mtx );

void    matrix_mean_col( matrix &mean, matrix &mtx );
matrix  colmean( matrix &mtx );

void    matrix_mean_row( matrix &mean, matrix &mtx );
matrix  rowmean( matrix &mtx );

double  mean( matrix &mtx );
double  var( matrix &mtx );

void    matrix_var_col( matrix &var, matrix &mtx );
matrix  colvar( matrix &mtx );

void    matrix_var_row( matrix &var, matrix &mtx );
matrix  rowvar( matrix &mtx );

void    matrix_var_cov( matrix &var, matrix &mtx1, matrix &mtx2 );
matrix  cov( matrix &A, matrix &B );

double  matrix_median( matrix &srce );

int     rows( matrix &mtx );
int     cols( matrix &mtx );

void    matrix_abs( matrix &result, matrix &mtx );
matrix  abs( matrix &mtx );

void    matrix_pow( matrix &result, matrix &mtx, double power );
matrix  pow( matrix &mtx, double power );
matrix  pow( matrix &mtx, int power );

void    matrix_sqrt( matrix &result, matrix &mtx );
matrix  sqrt( matrix &mtx );

void    matrix_sgn( matrix &result, matrix &mtx );
matrix  sgn( matrix &mtx );

void    matrix_I( matrix &mtx, int n );
void    matrix_I( matrix &mtx );
matrix  I( int n );

void    matrix_repeat( matrix &mtx, double val, int row );
matrix  repeat( double val, int row );
matrix  repeat( int val, int row );

void    matrix_rotate2D( matrix &mtx, double theta );
matrix  rotate2D( double theta );

void    matrix_series( matrix &mtx, double a, double b );
matrix  series( double a, double b );
matrix  series( int a, int b );

void    matrix_series( matrix &mtx, double a, double b, double c );
matrix  series( double a, double b, double c );
matrix  series( int a, int b ,int c );

void    matrix_series( matrix &mtx, double a, double b , int n );
matrix  series( double a, double b, int n );

matrix  copy( matrix &A );

void    matrix_diagonal( matrix &mtx, matrix &p );
matrix  diagonal( matrix &p );

void    matrix_offdiagonal( matrix &mtx, matrix &p );
matrix  offdiagonal( matrix &p );

double  det( matrix &p );

void    matrix_inverse( matrix &mtx, matrix &p );
matrix  inv( matrix &p );

void    matrix_eigenvec( matrix &mtx, matrix &p );
matrix  eigenvec( matrix &p );

void    matrix_eigenval( matrix &mtx, matrix &p );
matrix  eigenval( matrix &p );

void    svd( matrix &p, matrix &U, matrix &S, matrix &V );

void    zero( matrix &mtx );

matrix  fill( int row, int col, double value );
matrix  zeros( int row, int col );
matrix  ones( int row, int col );

// Changed set(...) to fill(...) because it's a keyword

#ifndef _SET_
#define set fill
#endif

void    fill( matrix &mtx, double val );
void    fill( matrix &mtx, int val );

void    rand( matrix &mtx, double min, double max );

void    gauss( matrix &mtx, double m, double sd );

BOOL    singular( MTXFLT a, int n );

double  norm( matrix &mtx );

void    matrix_sqrtm( matrix mtx, matrix &x );        // Guts commented out???
matrix  sqrtm( matrix &x );

void    matrix_mgauss( matrix &mtx, matrix &m, matrix &c );
matrix  mgauss( matrix &m, matrix &c );

void    matrix_mselect( matrix &mtx, int n, int N );
matrix  mselect( int n, int N );

void    matrix_permute( matrix &B, matrix &A );
matrix  permute( matrix &A );

void    matrix_transpose( matrix &dest, matrix &srce );
matrix  T( matrix &A );                               // Also matrix operator (~): A = ~B;

void    matrix_perpend( matrix &C, matrix &A, matrix &B );
matrix  perpend( matrix &A, matrix &B );              // Vector product normalixed of A and B...

void    matrix_tangent( matrix &C, matrix &A, matrix &B );
matrix  tangent( matrix &A, matrix &B );

void    matrix_crossprod( matrix &C, matrix &A, matrix &B );
matrix  crossprod( matrix &A, matrix &B );

double  dotprod( matrix &A, matrix &B );

void    matrix_plane( matrix &mtx, matrix &p, matrix &q, matrix &r, matrix &x );
void    matrix_plane( matrix &mtx, matrix &p, matrix &q, matrix &r );
matrix  plane( matrix &p, matrix &q, matrix &r, matrix &x );
matrix  plane( matrix &p, matrix &q, matrix &r );
double  point2plane( matrix &p, matrix &c );

void    matrix_interleave( matrix &C, matrix &A, matrix &B );
matrix  interleave( matrix &A, matrix &B );

/*****************************************************************************/
// Matrix sort functions (mtx-sort.cpp)...

struct  matrix_sort_tag
{
    double value;
    int    index;
};

int     matrix_sort_cmp( const void *e1, const void *e2 );
int     matrix_cmp( const void *e1, const void *e2 );

//      Sort the rows of a matrix based on the values in the specified column. (V2.2)
void    matrix_sort_rows( matrix &dest, matrix &srce, int column );
//      As above using values in first column.  (V2.2)
void    matrix_sort_rows( matrix &dest, matrix &srce );

//      Sort the columns of a matrix based on the values in the specified row. (V2.2)
void    matrix_sort_columns( matrix &dest, matrix &srce, int row );
//      As above using values in first row. (V2.2)
void    matrix_sort_columns( matrix &dest, matrix &srce );

//      Old vector sort function.
void    matrix_sort_vector( matrix &dest, matrix &srce );

//      Sort a vector using new matrix sort functions.
void    matrix_sort( matrix &dest, matrix &srce );

matrix  sort( matrix &p );

/*****************************************************************************/
// General maths functions (mtx_math.cpp)...

void    range( int *value, int minmax[] );
void    range( int *value, int minimum, int maximum );
void    range( double *value, double minmax[] );
void    range( double *value, double minimum, double maximum );
void    range( int *value, int aboslute );
void    range( double *value, double absolute );

int     range( int value, int minmax[] );
int     range( int value, int minimum, int maximum );
double  range( double value, double minmax[] );
double  range( double value, double minimum, double maximum );
int     range( int value, int aboslute );
double  range( double value, double absolute );

void    randomize( void );
double  drand( double a, double b );
int     irand( int a, int b );
BOOL    randomflip( double bias );

double  gauss( double mean, double sd );
double  reciprocal( double x );
double  logistic( double x );
double  sgn( double x );
double  logistic_map( double x );
double  deriv_logistic( double x );
double  myerf2( double m );
double  myerf1( double m );

/*****************************************************************************/

#define colextract           matrix_extract_col
#define rowextract           matrix_extract_row

#define rowassign            matrix_assign_row
#define colassign            matrix_assign_col

#define dosread              matrix_get

#define mat                  scalar

/*****************************************************************************/
// Matrix File I/O...

#define MATRIX_READ_BUFF     10240
#define MATRIX_READ_ROW      0
#define MATRIX_READ_COL      1

BOOL    matrix_read( FILE *FP, matrix &mtx, int row, int col );
BOOL    matrix_read( char *file, int &row, int &col );
BOOL    matrix_read( char *file, matrix &mtx );
BOOL    matrix_read( char *file, matrix &mtx, int row, int col );

BOOL    matrix_write( FILE *FP, matrix &mtx, int row, void (*func)( void ) );
BOOL    matrix_write( char *file, char *mode, matrix &mtx, int first, int last, void (*func)( void ) );

BOOL    matrix_write( char *file, matrix &mtx, int first, int last, void (*func)( void ) );
BOOL    matrix_write( char *file, matrix &mtx, int first, int last );

BOOL    matrix_write( char *file, char *mode, matrix &mtx, int row, void (*func)( void ) );
BOOL    matrix_write( char *file, matrix &mtx, int row, void (*func)( void ) );
BOOL    matrix_write( char *file, matrix &mtx, int row );

BOOL    matrix_write( char *file, matrix &mtx, void (*func)( void ) );
BOOL    matrix_write( char *file, matrix &mtx );

BOOL    matrix_append( char *file, matrix &mtx, int row, void (*func)( void ) );
BOOL    matrix_append( char *file, matrix &mtx, int row );
BOOL    matrix_append( char *file, matrix &mtx, void (*func)( void ) );
BOOL    matrix_append( char *file, matrix &mtx );

matrix  matrix_get( char *file );
matrix  matrix_get( char *file, int row, int col );

void    append_file( matrix &mtx, char *file );       // Append's matrix to an existing text file.
void    write( matrix &mtx, char *file );             // Writes matrix to a text file.
void    empty( char *file );                          // Clears a file.

/*****************************************************************************/

#endif

