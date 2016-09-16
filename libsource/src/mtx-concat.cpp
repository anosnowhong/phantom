/*****************************************************************************/
/* Matrix Concatenation.                                                     */
/*****************************************************************************/

void    matrix_concat_col( matrix &C, matrix &A, matrix &B )
{
int     i,j,k;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_concat_col( matrix &C, matrix &A, matrix &B )\n");
#endif

    if( A.isempty() )        // Empty matrix so copy (V2.1)...
    {
        matrix_memcpy(C,B);
    }
    else                     // Not empty, so actually concatenate...
    if( A.row == B.row )
    {
        matrix_dim(C,A.row,A.col+B.col);

        for( k=1,i=1; (i <= A.col); i++,k++ )
        {
            for( j=1; (j <= A.row); j++ )
            {
                C.mtx[j][k] = A.mtx[j][i];
            }
        }
 
        for( i=1; (i <= B.col); i++,k++ )
        {
            for( j=1; (j <= B.row); j++ )
            {
                C.mtx[j][k] = B.mtx[j][i];
            }
        }
    }
    else
    {
        ERR2("void matrix_concat_col( matrix &C, matrix &A, matrix &B )",A,B);
    }
}

/*****************************************************************************/

void    matrix_concat_col( matrix &C, matrix &A, double b )
{
int     i,j,k;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_concat_col( matrix &C, matrix &A, double b )\n");
#endif

    if( A.isempty() )        // Empty matrix so copy (V2.1)...
    {
        matrix_dim(C,1,1);      
        C.mtx[1][1] = b;	
    }
    else
    {
      matrix_dim(C,A.row,A.col+1);
      
      for( k=1,i=1; (i <= A.col); i++,k++ )
	{
	  for( j=1; (j <= A.row); j++ )
	    {
	      C.mtx[j][k] = A.mtx[j][i];
	    }
	}
      
      for( j=1; (j <= A.row); j++ )
	{
	  C.mtx[j][k] = b;
	}
    }
}

/*****************************************************************************/

void    matrix_concat_row( matrix &C, matrix &A, matrix &B )
{
int     i,j,k;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_concat_row( matrix &C, matrix &A, matrix &B )\n");
#endif
  
    if( A.isempty() )        // Empty matrix so copy (V2.1)...
    {
	 matrix_memcpy(C,B);
    }
    else  // A is not empty, so actually concatenate
    if( A.col == B.col )
    {
        matrix_dim(C,A.row+B.row,A.col);

        for( k=1,i=1; (i <= A.row); i++,k++ )
        {
            for( j=1; (j <= A.col); j++ )
            {
                C.mtx[k][j] = A.mtx[i][j];
            }
        }
 
        for( i=1; (i <= B.row); i++,k++ )
        {
            for( j=1; (j <= B.col); j++ )
            {
                C.mtx[k][j] = B.mtx[i][j];
            }
        }
    }
    else
    {
        ERR2("void matrix_concat_row( matrix &C, matrix &A, matrix &B )",A,B);
    }
}

/*****************************************************************************/

void    matrix_concat_row( matrix &C, matrix &A, double b )
{
int     i,j,k;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_concat_row( matrix &C, matrix &A, double b )\n");
#endif

    if( A.isempty() )        // Empty matrix so copy (V2.1)...
    {
	matrix_dim(C,1,1);      
	C.mtx[1][1] = b;	
    }
    else 
      {
	matrix_dim(C,A.row+1,A.col);
	
	for( k=1,i=1; (i <= A.row); i++,k++ )
	  {
	    for( j=1; (j <= A.col); j++ )
	      {
		C.mtx[k][j] = A.mtx[i][j];
	      }
	  }
	
	for( j=1; (j <= A.col); j++ )
	  {
	    C.mtx[k][j] = b;
	  }
      }
}

/*****************************************************************************/

matrix  operator|( matrix &A, matrix &B )
{
MTXRETN C;

    matrix_concat_col(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator|( matrix &A, double b )
{
MTXRETN C;

    matrix_concat_col(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator|( matrix &A, float b )
{
MTXRETN C;

    matrix_concat_col(C,A,(double)b);
    return(C);
}


/*****************************************************************************/

matrix  operator|( matrix &A, int b )
{
MTXRETN C;

    matrix_concat_col(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

matrix  operator|( double b, matrix &A )
{
MTXRETN C;
matrix B;

    B.dim(A.rows(),1);
    B = b;

    matrix_concat_col(C,B,A);
    return(C);
}

/*****************************************************************************/

matrix  operator|(  float b, matrix &A )
{
MTXRETN C;
matrix B;

    B.dim(A.rows(),1);
    B = b;

    matrix_concat_col(C,B,A);
    return(C);
}

/*****************************************************************************/

matrix  operator|( int b, matrix &A )
{
MTXRETN C;
matrix B;

    B.dim(A.rows(),1);
    B = b;

    matrix_concat_col(C,B,A);
    return(C);
}

/*****************************************************************************/

void    matrix::operator|=( matrix &B )
{
MTXRETN C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A|=B (concatenate columns)\n");
#endif

    matrix_concat_col(C,*this,B);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

void    matrix::operator|=( double b )
{
MTXRETN C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A|=(double)b (concatenate columns)\n");
#endif

    matrix_concat_col(C,*this,b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

void    matrix::operator|=( float b )
{
MTXRETN C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A|=(float)b (concatenate columns)\n");
#endif

    matrix_concat_col(C,*this,(double)b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

void    matrix::operator|=( int b )
{
MTXRETN C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A|=(int)b (concatenate columns)\n");
#endif

    matrix_concat_col(C,*this,(double)b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

matrix  operator%( matrix &A, matrix &B )
{
MTXRETN C;

    matrix_concat_row(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator%( matrix &A, double b )
{
MTXRETN C;

    matrix_concat_row(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator%( matrix &A, float b )
{
MTXRETN C;

    matrix_concat_row(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

matrix  operator%( matrix &A, int b )
{
MTXRETN C;

    matrix_concat_row(C,A,(double)b);
    return(C);
}

/*****************************************************************************/

matrix  operator%( double b, matrix &A )
{
MTXRETN C;
matrix B;

    B.dim(1,A.cols());
    B = b;

    matrix_concat_row(C,B,A);
    return(C);
}

/*****************************************************************************/

matrix  operator%( float b, matrix &A )
{
MTXRETN C;
matrix B;

    B.dim(1,A.cols());
    B = b;

    matrix_concat_row(C,B,A);
    return(C);
}

/*****************************************************************************/

matrix  operator%( int b, matrix &A )
{
MTXRETN C;
matrix B;

    B.dim(1,A.cols());
    B = b;

    matrix_concat_row(C,B,A);
    return(C);
}

/*****************************************************************************/

void    matrix::operator%=( matrix &B )
{
MTXRETN C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A%%=B (concatenate rows)\n");
#endif

    matrix_concat_row(C,*this,B);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

void    matrix::operator%=( double b )
{
MTXRETN C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A%%=(double)b (concatenate rows)\n");
#endif

    matrix_concat_row(C,*this,b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

void    matrix::operator%=( float b )
{
MTXRETN C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A%%=(float)b (concatenate rows)\n");
#endif

    matrix_concat_row(C,*this,(double)b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

void    matrix::operator%=( int b )
{
MTXRETN C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix::operator A%%=(int)b (concatenate rows)\n");
#endif

    matrix_concat_row(C,*this,(double)b);
    matrix_memcpy(*this,C);
}

/*****************************************************************************/

