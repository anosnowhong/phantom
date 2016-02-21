/*****************************************************************************/
/* Matrix extraction.                                                        */
/*****************************************************************************/

double& matrix::extract( int r, int c )
{
static double value=0.0;

    if( !exist(r,c) )
    {
        Errorf("Invalid element (%d,%d).\n",r,c);
        return(value);
    }

#ifdef  MATRIX_DEBUG
    Debugf("Extract element (%d,%d)=%lf.\n",r,c,mtx[r][c]);
#endif

    return(mtx[r][c]);
}

/*****************************************************************************/

double& matrix::operator()( int r, int c )       // Set [A(1,1)=1.0;] or Get [f=A(1,1);] a single matrix element.
{
    return(extract(r,c));
}  

/*****************************************************************************/

void    matrix_extract_col( matrix &dest, matrix &srce, int col ) // Extract specific column from matrix.
{
int     r,c,i;                                                           

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("extract_col( matrix &dest, matrix &srce, int col );\n");
#endif

    // Extract to a column matrix (vector).
    r = srce.row;
    c = 1;
    if( (dest.row != r) || (dest.col != c) )
    {
        dest.dim(r,c);
    }
    for( i=1; (i <= srce.row); i++ )
    {
        dest.mtx[i][1] = srce.mtx[i][col];
    }
}

/*****************************************************************************/

void    matrix_extract_row( matrix &dest, matrix &srce, int row ) // Extract specific row from matrix.
{
int     r,c,i;                                                           

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("extract_row( matrix &dest, matrix &srce, int row );\n");
#endif

    // Extract to a row matrix (vector).
    r = 1;
    c = srce.col;
    if( (dest.row != r) || (dest.col != c) )
    {
        dest.dim(r,c);
    }

    for( i=1; (i <= srce.col); i++ )
    {
        dest.mtx[1][i] = srce.mtx[row][i];
    }
}

/*****************************************************************************/

void    matrix_extract_sub( matrix &dest, matrix &srce, int r1, int c1, int r2, int c2 )
{                                                          // Sub-matrix extraction.
int     r,c,i,j;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("extract_sub( mattrix &dest, matrix &srce, int r1, int c1, int r2, int c2 );\n");
#endif

    if( !srce.exist(r1,c1) || !srce.exist(r2,c2) )
    {
        srce.Errorf("extract_sub (%d,%d;%d,%d) does not exist.\n",r1,c1,r2,c2);
        return;
    }

    r = (r2-r1)+1;
    c = (c2-c1)+1;
    if( (dest.row != r) || (dest.col != c) )
    {
        dest.dim(r,c);
    }

    for( i=1; (i <= dest.row); i++ ) 
    {
        for( j=1; (j <= dest.col); j++ )
        {
            dest.mtx[i][j] = srce.mtx[r1+(i-1)][c1+(j-1)];
        }
    }
}

/*****************************************************************************/

void    matrix_extract_sub( matrix &dest, matrix &srce, matrix &mask )
{
int     r,c,i,j,k;
  
    if( mask.isrowvector() && (mask.col == srce.col) )     // Select specific columns...
    {
        k = (int)sum(mask != 0.0);
        
        r = srce.row;
        c = k;
      
        if( (dest.row != r) || (dest.col != c) )
        {
            dest.dim(r,c);
        }
      
        for( i=1,k=0; (i <= mask.col); i++ ) 
        {
            if( mask.mtx[1][i] != 0.0 ) 
            {
                k++;

                for( j=1; (j <= dest.row); j++ )
                {
                    dest.mtx[j][k] = srce.mtx[j][i];
                }
            }
        } 
    } 
    else
    if( mask.iscolvector() && (mask.row == srce.row) )     // Select specific rows...
    {
        k = (int)sum(mask != 0.0);

        r = k;
        c = srce.col;
      
        if( (dest.row != r) || (dest.col != c) )
        {
            dest.dim(r,c);
        }
       
        for( i=1,k=0; (i <= mask.row); i++ ) 
        {
            if( mask.mtx[i][1] != 0.0 ) 
            {
                k++;

                for( j=1; (j <= dest.col); j++ )
                {
                    dest.mtx[k][j] = srce.mtx[i][j];
                }
            } 
        }
    }
    else
    {
        ERR1("extract_sub( matrix &dest, matrix &srce, matrix &mask )",mask);
    }
}

/*****************************************************************************/

//##ModelId=3EC4C3650242
matrix  matrix::operator[]( int COL )                      // Extract specific column from matrix.
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix matrix::operator[]( int COL ) A=B[column];\n");
#endif

    matrix_extract_col(C,*this,COL);
    return(C);
}

/*****************************************************************************/
 
//##ModelId=3EC4C365024B
matrix  matrix::operator()( int ROW )                      // Extract specific row from matrix.
{
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix matrix::operator()( int ROW ) A=B(row);\n");
#endif

    matrix_extract_row(C,*this,ROW);
    return(C);
}

/*****************************************************************************/

//##ModelId=3EC4C3650255
matrix  matrix::operator()( int r1, int c1, int r2, int c2 )
{                                                // Sub-matrix extraction.
MTXRETN    C;

#ifdef  MATRIX_DEBUG
    MATRIX_debugf("matrix matrix::operator()( int r1, int c1, int r2, int c2 ) A=B(r1,c1,r2,c2);\n");
#endif

    matrix_extract_sub(C,*this,r1,c1,r2,c2);
    return(C);
}

/*****************************************************************************/

//##ModelId=3EC4C3650273
matrix  matrix::operator()( matrix &A )
{
MTXRETN    C;

    matrix_extract_sub(C,*this,A);
    return(C);
}

/*****************************************************************************/

//##ModelId=3EC4C365022D
matrix  matrix::operator()( int ROW, matrix &A )
{
MTXRETN    C;
int     i,j,k;
  
    if( A.isrowvector() && (A.col == col) ) // Select specific columns...
    {
        k = (int)sum(A != 0.0);
        C.dim(1,k);
      
        for( i=1,k=0; (i <= A.col); i++ ) 
        {
            if( A.mtx[1][i] != 0.0 ) 
            {
                k++;
                C.mtx[1][k] = mtx[ROW][i];
            }
        }
    } 
    else 
    {
        ERR1("matrix::operator()( int ROW, matrix &A )\n",A);
    }
  
   return(C);
}

/*****************************************************************************/

//##ModelId=3EC4C3650237
matrix  matrix::operator()( matrix &A, int COL )
{
MTXRETN    C;
int     i,j,k;
  
    if( A.iscolvector() && (A.row == row) ) // Select specific rows...
    {
        k = (int)sum(A != 0.0);
        C.dim(k,1);
      
        for( i=1,k=0; (i <= A.row); i++ ) 
        {
            if( A.mtx[i][1] != 0.0 ) 
            {
                k++;
                C.mtx[k][1] = mtx[i][COL];
            }
        } 
    } 
    else 
    {
        ERR1("matrix::operator()( matrix &A, int COL )\n",A);
    }
  
   return(C);
}

/*****************************************************************************/

void    matrix_extract_cols( matrix &dest, matrix &srce, int col, ... )
{
int     n,i,j;
int     cols[MATRIX_EXTRACT_MAX+1];
va_list argp;

    n=0;
    cols[n++] = col;

    va_start(argp,col);

    while( (i=va_arg(argp,int)) != MATRIX_EXTRACT_END )
    {
        if( n >= MATRIX_EXTRACT_MAX )
        {
            dest.Errorf("extract_cols(...) Too many columns.\n");
            break;
        }

        cols[n++] = i;
    }

    va_end(argp);

    cols[n++] = MATRIX_EXTRACT_END;

    matrix_extract_cols(dest,srce,cols);
}

/*****************************************************************************/

void    matrix_extract_cols( matrix &dest, matrix &srce, int cols[] )
{
int     i,j,n;

    for( n=0; ((n < MATRIX_EXTRACT_MAX) && (cols[n] != MATRIX_EXTRACT_END)); n++ );

    if( n == 0 )
    {
        return;
    }

    dest.dim(srce.row,n);
  
    for( i=1; (i <= dest.row); i++ ) 
    {
        for( j=1; (j <= dest.col); j++ )
        {
            dest.mtx[i][j] = srce.mtx[i][cols[j-1]];
        }
    }
}

/*****************************************************************************/

void    matrix_extract_rows( matrix &dest, matrix &srce, int row, ... )
{
int     n,i,j;
int     rows[MATRIX_EXTRACT_MAX]; 
va_list argp;

    n=0;
    rows[n++] = row;

    va_start(argp,row);

    while( (i=va_arg(argp,int)) != MATRIX_EXTRACT_END )
    {
        if( n >= MATRIX_EXTRACT_MAX )
        {
            dest.Errorf("extract_rows(...) Too many rows.\n");
            break;
        }

        rows[n++] = i;
    }
  
    va_end(argp);

    rows[n++] = MATRIX_EXTRACT_END;

    matrix_extract_rows(dest,srce,rows);
}

/*****************************************************************************/

void    matrix_extract_rows( matrix &dest, matrix &srce, int rows[] )
{
int     i,j,n;

    for( n=0; ((n < MATRIX_EXTRACT_MAX) && (rows[n] != MATRIX_EXTRACT_END)); n++ );

    if( n == 0 )
    {
        return;
    }

    dest.dim(n,srce.col);
  
    for( i=1; (i <= dest.row); i++ ) 
    {
        for( j=1; (j <= dest.col); j++ )
        {
            dest.mtx[i][j] = srce.mtx[rows[i-1]][j];
        }
    }
}

/*****************************************************************************/

matrix  submatrix( matrix &srce, matrix &mask )
{
MTXRETN C;

    matrix_extract_sub(C,srce,mask);
    return(C);
}

/*****************************************************************************/
