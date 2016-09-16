 /*****************************************************************************/
/* Matrix division.                                                          */
/*****************************************************************************/

void    matrix_divide( matrix &C, matrix &A, matrix &B )        // C = A / B;
{
int     i,j;

    matrix_dim(C,A.row,A.col);
  
    if( (A.col == B.col) && (A.row == B.row) )
    {
        for( i=1; (i <= C.row); i++ )
        {
            for( j=1; (j <= C.col); j++ )
            {
                C.mtx[i][j] = A.mtx[i][j] / B.mtx[i][j];
            }
        }
    }
    else
    if( B.isscalar() ) 
    {
        for( i=1; (i <= C.row); i++ )
        {
            for( j=1; (j <= C.col); j++ )
            {
                C.mtx[i][j] = A.mtx[i][j] / B.mtx[1][1];
            }
        }
    }
    else 
    {
        ERR2("C=A/B",A,B);
    }
}

/*****************************************************************************/

void    matrix_divide( matrix &C, matrix &A, double b )         // C = A / (double)b;
{
int     i,j;

    if( b == 0.0 )
    {
        ERR0("C=A/(double)b: Divide by zero scalar");
        return;
    }

    matrix_dim(C,A.row,A.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = A.mtx[i][j] / b;
        }
    }
}

/*****************************************************************************/

matrix  operator/( matrix &A, double b )                        // C = A / (double)b;
{
MTXRETN C;

    matrix_divide(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator/( matrix &A, float b )                         // C = A / (float)b;
{
MTXRETN C;

    matrix_divide(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

matrix  operator/( matrix &A, int b )                           // C = A / (int)b;
{
MTXRETN C;

    matrix_divide(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

//##ModelId=3EC4C36502B0
void    matrix::operator/=( double b )                          // A /= (double)b;
{
MTXRETN C;

    matrix_divide(C,*this,b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

void    matrix::operator/=( float b )                           // A /= (float)b;
{
MTXRETN C;

    matrix_divide(C,*this,(double)b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

void    matrix::operator/=( int b )                             // A /= (int)b;
{
MTXRETN C;

    matrix_divide(C,*this,(double)b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

matrix  operator/( matrix &A, matrix &B )                       // C = A / B;
{
MTXRETN C;

    matrix_divide(C,A,B);
    return(C);
}

/*****************************************************************************/

//##ModelId=3EC4C36502B2
void    matrix::operator/=( matrix &B )                         // A /= B;
{
MTXRETN C;

    matrix_divide(C,*this,B);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/
