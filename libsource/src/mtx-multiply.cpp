/*****************************************************************************/
/* Matrix multiplication.                                                    */
/*****************************************************************************/

void    matrix_multiply( matrix &C, matrix &A, double b )       // C = A * (double)b;
{
int     i,j;

    matrix_dim(C,A.row,A.col);
  
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = A.mtx[i][j] * b;
        }
    }
}
/*****************************************************************************/

void    matrix_multiply( matrix &C, matrix &A, matrix &B )      // C = A * B;
{
int     i,j,k;
  
    matrix_dim(C,A.row,B.col);

    if( A.col != B.row )
    {
        ERR2("C=A*B",A,B);
        return;
    }
  
    for( i=1; (i <= A.row); i++ )
    {
        for( j=1; (j <= B.col); j++ )
        {
            for( k=1; (k <= B.row); k++ ) 
            {
                C.mtx[i][j] += A.mtx[i][k] * B.mtx[k][j];
            }
        }
    }
}

/*****************************************************************************/

void    matrix_multipew( matrix &C, matrix &A, matrix &B )      // C = A ^ B;
{                                                               // Element-wise multiply....
int     i,j;
  
    if( (A.col == B.col) && (A.row == B.row) )
    {
        matrix_dim(C,A.row,A.col);

        for( i=1; (i <= C.row); i++ )
        {
            for( j=1; (j <= C.col); j++ )
            {
                C.mtx[i][j] = A.mtx[i][j] * B.mtx[i][j];
            }
        }
    }
    else
    {
        ERR2("C=A^B",A,B);
    }
}

/*****************************************************************************/

matrix  operator*( matrix &A, matrix &B )                       // C = A * B;
{
MTXRETN C;

    matrix_multiply(C,A,B);
    return(C);
}

/*****************************************************************************/

//##ModelId=3EC4C3650292
void    matrix::operator*=( matrix &B )                         // A *= B;
{
MTXRETN C;

    matrix_multiply(C,*this,B);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

//##ModelId=3EC4C36502A6
void    matrix::operator*=( double b )                          // A *= (double)b;
{
MTXRETN C;

    matrix_multiply(C,*this,b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

//##ModelId=3EC4C365029D
void    matrix::operator*=( float b )                           // A *= (float)b;
{
MTXRETN C;

    matrix_multiply(C,*this,(double)b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

void    matrix::operator*=( int b )                             // A *= (int)b;
{
MTXRETN C;

    matrix_multiply(C,*this,(double)b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

matrix  operator^( matrix &A, matrix &B )                       // C = A ^ B;
{                                                               // Element-wise multiply...
MTXRETN C;

    matrix_multipew(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator*( double a, matrix &B )                        // C = (double)a * B;
{
MTXRETN C;

    matrix_multiply(C,B,a);
    return(C);
}

/*****************************************************************************/

matrix  operator*( matrix &A, double  b )                       // C = A * (double)b;
{
MTXRETN C;

    matrix_multiply(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator*( float a, matrix &B )                         // C = (float)a * B;
{
MTXRETN C;

    matrix_multiply(C,B,(double)a);
    return(C);
}

/*****************************************************************************/

matrix  operator*( matrix &A, float b )                         // C = A * (float)b;
{
MTXRETN C;

    matrix_multiply(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

matrix  operator*( int a, matrix &B )                           // C = (int)a * B;
{
MTXRETN C;

    matrix_multiply(C,B,(double)a);
    return(C);
}

/*****************************************************************************/

matrix  operator*( matrix &A, int b )                           // C = A * (int)b;
{
MTXRETN C;

    matrix_multiply(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

