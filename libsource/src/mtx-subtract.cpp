#/*****************************************************************************/
/* Matrix subtraction.                                                       */
/*****************************************************************************/

//##ModelId=3EC4C36502C5
void    matrix::operator-=( matrix &A )                         // A -= B;
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A-=B;\n");
#endif

    if( (col == A.col) && (row == A.row) )
    {
        for( i=1; (i <= row); i++ )
        {
            for( j=1; (j <= col); j++ )
            {
                mtx[i][j] -= A.mtx[i][j];
            }
        }
    }
    else
    {
        ERR2("A-=B",*this,A);
    }
}

/*****************************************************************************/

//##ModelId=3EC4C36502CF
void    matrix::operator-=( double a )                          // A -= (double)b;
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A-=(double)b;\n");
#endif

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            mtx[i][j] -= a;
        }
    } 
}

/*****************************************************************************/

void    matrix::operator-=( float a )                            // A -= (float)b;
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A-=(float)b;\n");
#endif

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            mtx[i][j] -= (double)a;
        }
    } 
}

/*****************************************************************************/

void    matrix::operator-=( int a )                              // A -= (int)b;
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A-=(int)b;\n");
#endif

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            mtx[i][j] -= (double)a;
        }
    } 
}

/*****************************************************************************/

//##ModelId=3EC4C36502D8
matrix  matrix::operator-()                                    // B = -A;
{
MTXRETN C;
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A=-B;\n");
#endif

    matrix_negate(C,*this);
    return(C);
}

/*****************************************************************************/

matrix  operator-( matrix &A, matrix &B )                       // C = A - B;
{
MTXRETN C;

    matrix_subtract(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator-( matrix &A, double b )                        // C = A - (double)b;
{
MTXRETN C;

    matrix_subtract(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator-( double a, matrix &B )                        // C = (double)a - B;
{
MTXRETN C;

    matrix_subtract(C,a,B);
    return(C);
}

/*****************************************************************************/

matrix  operator-( matrix &A, float b )                         // C = A - (float)b;
{
MTXRETN C;

    matrix_subtract(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

matrix  operator-( float a, matrix &B )                         // C = (float)a - B;
{
MTXRETN C;

    matrix_subtract(C,(double)a,B);
    return(C);
}

/*****************************************************************************/

matrix  operator-( matrix &A, int b )                           // C = A - (int)b;
{
MTXRETN C;

    matrix_subtract(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

matrix  operator-( int a, matrix &B )                           // C = (int)a - B;
{
MTXRETN C;

    matrix_subtract(C,(double)a,B);
    return(C);
}

/*****************************************************************************/

void    matrix_subtract( matrix &C, matrix &A, matrix &B )      // C = A - B;
{
int     i,j;

    matrix_dim(C,A.row,A.col);
 
    if( (A.col == B.col) && (A.row == B.row) )
    {
        for( i=1; (i <= C.row); i++ )
        {
            for( j=1; (j <= C.col); j++ )
            {
                C.mtx[i][j] = A.mtx[i][j] - B.mtx[i][j];
            }
        }
    }
    else
    {
        ERR2("C=A-B",A,B);
    }
}

/*****************************************************************************/

void    matrix_subtract( matrix &C, matrix &A, double b )       // C = A - (double)b;
{
int     i,j;

    matrix_dim(C,A.row,A.col);
 
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = A.mtx[i][j] - b;
        }
    }
}

/*****************************************************************************/

void    matrix_subtract( matrix &C, double a, matrix &B )       // C = (double)a - B;
{
int     i,j;

    matrix_dim(C,B.row,B.col);
 
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = a - B.mtx[i][j];
        }
    }
}

/*****************************************************************************/

void    matrix_negate( matrix &B, matrix &A )                   // B = -A;
{
int     i,j;

    matrix_dim(B,A.row,A.col);
 
    for( i=1; (i <= B.row); i++ )
    {
        for( j=1; (j <= B.col); j++ )
        {
            B.mtx[i][j] = -A.mtx[i][j];
        }
    }
}

/*****************************************************************************/
