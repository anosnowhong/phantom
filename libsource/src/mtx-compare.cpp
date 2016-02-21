/*****************************************************************************/
/* Matrix comparisons.                                                       */
/*****************************************************************************/

void    matrix_equal( matrix &C, matrix &A, matrix &B )
{
int     i,j;

//  Matrices A & B must have same dimensions...
    if( (A.row != B.row) || (A.col != B.col) )
    {
        ERR2("matrix_equal(...)",A,B);
        return;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_equal( matrix &C, matrix &A, matrix &B ) C=(A==B);\n");
#endif

//  Special processing for empty matrices (V2.1)...
    if( A.isempty() && B.isempty() )
    {
        matrix_empty(C);
        return;
    }

    matrix_dim(C,A.row,A.col);
  
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] == B.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_equal( matrix &C, matrix &A, double b )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_equal( matrix &C, matrix &A, double b ) C=(A==(double)b);\n");
#endif
  
    matrix_dim(C,A.row,A.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] == b);
        }
    } 
}

/*****************************************************************************/

void    matrix_or( matrix &C, matrix &A, matrix &B )
{
int     i,j;

//  Matrices A & B must have same dimensions...
    if( (A.row != B.row) || (A.col != B.col) )
    {
        ERR2("matrix_or(...)",A,B);
        return;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_or( matrix &C, matrix &A, matrix &B ) C=(A||B);\n");
#endif

//  Special processing for empty matrices (V2.1)...
    if( A.isempty() && B.isempty() )
    {
        matrix_empty(C);
        return;
    }

    matrix_dim(C,A.row,A.col);
  
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)((BOOL)A.mtx[i][j] || (BOOL)B.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_or( matrix &C, matrix &A, double b )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_or( matrix &C, matrix &A, double b ) C=(A||(double)b);\n");
#endif

    matrix_dim(C,A.row,A.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)((BOOL)A.mtx[i][j] || (BOOL)b);
        }
    } 
}

/*****************************************************************************/

void    matrix_and( matrix &C, matrix &A, matrix &B )
{
int     i,j;

//  Matrices A & B must have same dimensions...
    if( (A.row != B.row) || (A.col != B.col) )
    {
        ERR2("matrix_and(...)",A,B);
        return;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_and( matrix &C, matrix &A, matrix &B ) C=(A&&B);\n");
#endif

//  Special processing for empty matrices (V2.1)...
    if( A.isempty() && B.isempty() )
    {
        matrix_empty(C);
        return;
    }

    matrix_dim(C,A.row,A.col);
  
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)((BOOL)A.mtx[i][j] && (BOOL)B.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_and( matrix &C, matrix &A, double b )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_and( matrix &C, matrix &A, double b ) C=(A&&(double)b);\n");
#endif

    matrix_dim(C,A.row,A.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)((BOOL)A.mtx[i][j] && (BOOL)b);
        }
    } 
}

/*****************************************************************************/

void    matrix_notequal( matrix &C, matrix &A, matrix &B )
{
int     i,j;

//  Matrices A & B must have same dimensions...
    if( (A.row != B.row) || (A.col != B.col) )
    {
        ERR2("matrix_notequal(...)",A,B);
        return;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_notequal( matrix &C, matrix &A, matrix &B ) C=(A!=B);\n");
#endif

//  Special processing for empty matrices (V2.1)...
    if( A.isempty() && B.isempty() )
    {
        matrix_empty(C);
        return;
    }

    matrix_dim(C,A.row,A.col);
  
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] != B.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_notequal( matrix &C, matrix &A, double b )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_notequal( matrix &C, matrix &A, double b ) C=(A!=(double)b);\n");
#endif

    matrix_dim(C,A.row,A.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] != b);
        }
    } 
}

/*****************************************************************************/

void    matrix_greaterequal( matrix &C, matrix &A, matrix &B )
{
int     i,j;

//  Matrices A & B must have same dimensions...
    if( (A.row != B.row) || (A.col != B.col) )
    {
        ERR2("matrix_greaterequal(...)",A,B);
        return;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_greaterequal( matrix &C, matrix &A, matrix &B ) C=(A>=B);\n");
#endif

//  Special processing for empty matrices (V2.1)...
    if( A.isempty() && B.isempty() )
    {
        matrix_empty(C);
        return;
    }

    matrix_dim(C,A.row,A.col);
  
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] >= B.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_greaterequal( matrix &C, matrix &A, double b )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_greaterequal( matrix &C, matrix &A, double b ) C=(A>=(double)b);\n");
#endif

    matrix_dim(C,A.row,A.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] >= b);
        }
    } 
}
/*****************************************************************************/

void    matrix_greaterequal( matrix &C, double a, matrix &B )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_greaterequal( matrix &C, double a, double &B ) C=((double)a>=B);\n");
#endif

    matrix_dim(C,B.row,B.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(a >= B.mtx[i][j]);
        }
    } 
}

/*****************************************************************************/

void    matrix_greater( matrix &C, matrix &A, matrix &B )
{
int     i,j;

//  Matrices A & B must have same dimensions...
    if( (A.row != B.row) || (A.col != B.col) )
    {
        ERR2("matrix_greater(...)",A,B);
        return;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_greater( matrix &C, matrix &A, matrix &B ) C=(A>B);\n");
#endif

//  Special processing for empty matrices (V2.1)...
    if( A.isempty() && B.isempty() )
    {
        matrix_empty(C);
        return;
    }

    matrix_dim(C,A.row,A.col);
  
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] > B.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_greater( matrix &C, matrix &A, double b )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_greater( matrix &C, matrix &A, double b ) C=(A>(double)b);\n");
#endif

    matrix_dim(C,A.row,A.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] > b);
        }
    } 
}
/*****************************************************************************/

void    matrix_greater( matrix &C, double a, matrix &B )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_greater( matrix &C, double a, double &B ) C=((double)a>B);\n");
#endif

    matrix_dim(C,B.row,B.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(a > B.mtx[i][j]);
        }
    } 
}

/*****************************************************************************/

void    matrix_lessequal( matrix &C, matrix &A, matrix &B )
{
int     i,j;

//  Matrices A & B must have same dimensions...
    if( (A.row != B.row) || (A.col != B.col) )
    {
        ERR2("matrix_lessequal(...)",A,B);
        return;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_lessequal( matrix &C, matrix &A, matrix &B ) C=(A<=B);\n");
#endif

//  Special processing for empty matrices (V2.1)...
    if( A.isempty() && B.isempty() )
    {
        matrix_empty(C);
        return;
    }

    matrix_dim(C,A.row,A.col);
  
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] <= B.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_lessequal( matrix &C, matrix &A, double b )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_lessequal( matrix &C, matrix &A, double b ) C=(A<=(double)b);\n");
#endif

    matrix_dim(C,A.row,A.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] <= b);
        }
    } 
}

/*****************************************************************************/

void    matrix_lessequal( matrix &C, double a, matrix &B )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_lessequal( matrix &C, double a, double &B ) C=((double)a<=B);\n");
#endif

    matrix_dim(C,B.row,B.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(a <= B.mtx[i][j]);
        }
    } 
}

/*****************************************************************************/

void    matrix_less( matrix &C, matrix &A, matrix &B )
{
int     i,j;

//  Matrices A & B must have same dimensions...
    if( (A.row != B.row) || (A.col != B.col) )
    {
        ERR2("matrix_less(...)",A,B);
        return;
    }

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_less( matrix &C, matrix &A, matrix &B ) C=(A<B);\n");
#endif

//  Special processing for empty matrices (V2.1)...
    if( A.isempty() && B.isempty() )
    {
        matrix_empty(C);
        return;
    }

    matrix_dim(C,A.row,A.col);
  
    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] < B.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_less( matrix &C, matrix &A, double b )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_less( matrix &C, matrix &A, double b ) C=(A<(double)b);\n");
#endif

    matrix_dim(C,A.row,A.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(A.mtx[i][j] < b);
        }
    } 
}

/*****************************************************************************/

void    matrix_less( matrix &C, double a, matrix &B )
{
int     i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("void matrix_less( matrix &C, double a, double &B ) C=((double)a<B);\n");
#endif

    matrix_dim(C,B.row,B.col);

    for( i=1; (i <= C.row); i++ )
    {
        for( j=1; (j <= C.col); j++ )
        {
            C.mtx[i][j] = (double)(a < B.mtx[i][j]);
        }
    } 
}

/*****************************************************************************/

matrix  operator==( matrix &A, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator==( matrix &A, matrix &B ) C=(A==B);\n");
#endif

    matrix_equal(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator==( matrix &A, double b )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator==( maitrx &A, double b ) C=(A==(double)b);\n");
#endif

    matrix_equal(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator==( double a, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator==( double a, matrix &B) C=((double)a==B);\n");
#endif

    matrix_equal(C,B,a);
    return(C);
}

/*****************************************************************************/

matrix  operator||( matrix &A, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator||( matrix &A, matrix &B ) C=(A||B);\n");
#endif

    matrix_or(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator||( matrix &A, double b )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator||( matrix &A, double b ) C=(A||(double)b);\n");
#endif

    matrix_or(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator||( double a, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator||( double a, matrix &B ) C=((double)a||B);\n");
#endif

    matrix_or(C,B,a);
    return(C);
}

/*****************************************************************************/

matrix  operator&&( matrix &A, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator&&( matrix &A, matrix &B ) C=(A&&B);\n");
#endif

    matrix_and(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator&&( matrix &A, double b )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator&&( matrix &A, double b ) C=(A&&(double)b);\n");
#endif

    matrix_and(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator&&( double a, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator&&( double a, matrix &B ) C=((double)a&&B);\n");
#endif

    matrix_and(C,B,a);
    return(C);
}

/*****************************************************************************/

matrix  operator!=( matrix &A, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator!=( matrix &A, matrix &B ) C=(A!=B);\n");
#endif

    matrix_notequal(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator!=( matrix &A, double b )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator!=( matrix &A, double b ) C=(A!=(double)b);\n");
#endif

    matrix_notequal(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator!=( double a, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator!=( double a, matrix &B ) C=((double)a!=B);\n");
#endif

    matrix_notequal(C,B,a);
    return(C);
}

/*****************************************************************************/

matrix  operator>=( matrix &A, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator>=( matrix &A, matrix &B ) C=(A>=B);\n");
#endif

    matrix_greaterequal(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator>=( matrix &A, double b )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator>=( matrix &A, double b ) C=(A>=(double)b);\n");
#endif

    matrix_greaterequal(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator>=( double a, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator>=( double a, matrix &B ) C=((double)a>=B);\n");
#endif

    matrix_greaterequal(C,a,B);
    return(C);
}

/*****************************************************************************/

matrix  operator>( matrix &A, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator>( matrix &A, matrix &B ) C=(A>B);\n");
#endif

    matrix_greater(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator>( matrix &A, double b )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator>( matrix &A, double b ) C=(A>(double)b);\n");
#endif

    matrix_greater(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator>( double a, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator>( double a, matrix &B ) C=((double)a>B);\n");
#endif

    matrix_greater(C,a,B);
    return(C);
}

/*****************************************************************************/

matrix  operator<=( matrix &A, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator<=( matrix &A, matrix &B ) C=(A<=B);\n");
#endif

    matrix_lessequal(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator<=( matrix &A, double b )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator<=( matrix &A, double b ) C=(A<=(double)b);\n");
#endif

    matrix_lessequal(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator<=( double a, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator<=( double a, matrix &B ) C=((double)a<=B);\n");
#endif

    matrix_lessequal(C,a,B);
    return(C);
}

/*****************************************************************************/

matrix  operator<( matrix &A, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator<( matrix &A, matrix &B ) C=(A<B);\n");
#endif

    matrix_lessequal(C,A,B);
    return(C);
}

/*****************************************************************************/

matrix  operator<( matrix &A, double b )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator<( matrix &A, double b ) C=(A<(double)b);\n");
#endif

    matrix_less(C,A,b);
    return(C);
}

/*****************************************************************************/

matrix  operator<( double a, matrix &B )
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix operator<( double a, matrix &B ) C=((double)a<B);\n");
#endif

    matrix_less(C,a,B);
    return(C);
}

/*****************************************************************************/
