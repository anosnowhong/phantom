/*****************************************************************************/
/* Matrix apply various functions to each element.                           */
/*****************************************************************************/

void    matrix_apply( matrix &result, matrix &mtx, double (*func)(double x) )
{
int     i,j;

    matrix_dim(result,mtx.row,mtx.col);
  
    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            result.mtx[i][j] = (*func)(mtx.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_apply( matrix &result, matrix &mtx, float (*func)(float x) )
{
int     i,j;

    matrix_dim(result,mtx.row,mtx.col);
  
    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            result.mtx[i][j] = (double)(*func)((float)mtx.mtx[i][j]);
        }
    }
}

/*****************************************************************************/

void    matrix_apply( matrix &result, matrix &mtx_x, matrix &mtx_y, double (*func)(double x, double y) )
{
int     i,j;

    if( (mtx_x.row == mtx_y.row) && (mtx_x.col == mtx_y.col) )
    {
        matrix_dim(result,mtx_x.row,mtx_x.col);
  
        for( i=1; (i <= mtx_x.row); i++ )
        {
            for( j=1; (j <= mtx_x.col); j++ )
            {
                result.mtx[i][j] = (*func)(mtx_x.mtx[i][j],mtx_y.mtx[i][j]);
            }
        }
    }
    else
    {
        ERR2("void matrix_apply(...)",mtx_x,mtx_y);
    }
}

/*****************************************************************************/

void    matrix_apply( matrix &result, matrix &mtx_x, matrix &mtx_y, float (*func)(float x, float y) )
{
int     i,j;

    if( (mtx_x.row == mtx_y.row) && (mtx_x.col == mtx_y.col) )
    {
        matrix_dim(result,mtx_x.row,mtx_x.col);
  
        for( i=1; (i <= mtx_x.row); i++ )
        {
            for( j=1; (j <= mtx_x.col); j++ )
            {
                result.mtx[i][j] = (double)(*func)((float)mtx_x.mtx[i][j],(float)mtx_y.mtx[i][j]);
            }
        }
    }
    else
    {
        ERR2("void matrix_apply(...)",mtx_x,mtx_y);
    }
}

/*****************************************************************************/

void    matrix_apply( matrix &result, matrix &mtx, double (*func)(double x, double y), double y )
{
int     i,j;

    matrix_dim(result,mtx.row,mtx.col);  
  
    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            result.mtx[i][j] = (*func)(mtx.mtx[i][j],y);
        }
    }
}

/*****************************************************************************/

void    matrix_apply( matrix &result, matrix &mtx, float (*func)(float x, float y), float y )
{
int     i,j;

    matrix_dim(result,mtx.row,mtx.col);  
  
    for( i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            result.mtx[i][j] = (*func)(mtx.mtx[i][j],y);
        }
    }
}

/*****************************************************************************/

matrix  apply( matrix &mtx, double (*func)(double x) )
{
MTXRETN result;

    matrix_apply(result,mtx,func);
    return(result);
}

/*****************************************************************************/

matrix  apply( matrix &mtx_x, matrix &mtx_y, double (*func)(double x, double y) )
{
MTXRETN result;

    matrix_apply(result,mtx_x,mtx_y,func);
    return(result);
}

/*****************************************************************************/

matrix  apply( matrix &mtx, float (*func)(float x) )
{
MTXRETN result;

    matrix_apply(result,mtx,func);
    return(result);
}

/*****************************************************************************/

matrix  apply( matrix &mtx_x, matrix &mtx_y, float (*func)(float x, float y) )
{
MTXRETN result;

    matrix_apply(result,mtx_x,mtx_y,func);
    return(result);
}

/*****************************************************************************/

matrix  apply( matrix &mtx, double (*func)(double x, double y), double y )
{
MTXRETN result;

    matrix_apply(result,mtx,func,y);
    return(result);
}

/*****************************************************************************/

matrix  apply( matrix &mtx, float (*func)(float x, float y), float y )
{
MTXRETN result;

    matrix_apply(result,mtx,func,y);
    return(result);
}

/*****************************************************************************/
