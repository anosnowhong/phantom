/*****************************************************************************/
/* matrix addition.                                                          */
/*****************************************************************************/

//##ModelId=3EC4C36502BB
void    matrix::operator+=( matrix &A )                         // A += B;
{
int     i,j;

#ifdef  MATRIX_DEBUG
    Debugf("operator A+=B\n");
#endif

    if( (col == A.col) && (row == A.row) )
    {
        for( i=1; (i <= row); i++ )
        {
            for( j=1; (j <= col); j++ )
            {
                mtx[i][j] += A.mtx[i][j];
            }
        }
    }
    else
    {
        ERR2("A+=B",*this,A);
    }
}

/*****************************************************************************/

void    matrix::operator+=( double a )                          // A += (double)b;
{
int     i,j;

#ifdef  MATRIX_DEBUG
    Debugf("operator A+=(double)b\n");
#endif

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            mtx[i][j] += a;
        }
    } 
}

/*****************************************************************************/

void    matrix::operator+=( float a )                           // A += (float)b;
{
int     i,j;

#ifdef  MATRIX_DEBUG
    Debugf("operator A+=(float)b\n");
#endif

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            mtx[i][j] += (double)a;
        }
    } 
}

/*****************************************************************************/

void    matrix::operator+=( int a )                             // A += (int)b;
{
int     i,j;

#ifdef  MATRIX_DEBUG
    Debugf("operator A+=(int)b\n");
#endif

    for( i=1; (i <= row); i++ )
    {
        for( j=1; (j <= col); j++ )
        {
            mtx[i][j] += (double)a;
        }
    } 
}

/*****************************************************************************/

matrix  operator+( matrix &A, matrix &B )                       // C = A + B;
{
MTXRETN C;

    matrix_add(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator+( matrix &A, double b )                        // C = A + (double)b;
{
MTXRETN C;

    matrix_add(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator+( double a, matrix &B )                        // C = (double)a + B;
{
MTXRETN C;

    matrix_add(C,B,a);
    return(C);
}

/*****************************************************************************/

matrix  operator+( matrix &A, float b )                         // C = A + (float)b;
{
MTXRETN C;

    matrix_add(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

matrix  operator+( float a, matrix &B )                         // C = (float)a + B;
{
MTXRETN C;

    matrix_add(C,B,(double)a);
    return(C);
}

/*****************************************************************************/

matrix  operator+( matrix &A, int b )                           // C = A + (int)b;
{
MTXRETN C;

    matrix_add(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

matrix  operator+( int a, matrix &B )                           // C = (int)a + B;
{
MTXRETN C;

    matrix_add(C,B,(double)a);
    return(C);
}

/*****************************************************************************/

void    matrix_add( matrix &C, matrix &A, matrix &B )           // C = A + B;
{
int     i,j;

    matrix_dim(C,A.row,A.col);

    if( (A.col == B.col) && (A.row == B.row) )
    {
        for( i=1; (i <= C.row); i++ )
        {
            for( j=1; (j <= C.col); j++ )
            {
                C.mtx[i][j] = A.mtx[i][j] + B.mtx[i][j];
            }
        }
    }
    else
    {
        C.Errorf("C=A+B %s %s\n",A.dimtext(),B.dimtext());
    }
}

/*****************************************************************************/

void    matrix_add( matrix &C, matrix &A, double b )            // C = A + (double)b;
{
int     i,j;

    matrix_dim(C,A.row,A.col);
 
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = A.mtx[i][j] + b;
        }
    }
}

/*****************************************************************************/
