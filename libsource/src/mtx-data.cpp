/*****************************************************************************/
/* MTX_DATA: Special matrix data functions...                                */
/*****************************************************************************/

void    matrix_data( int ROW, int COL, matrix &mtx, double d, ... )
{
int     i,j;
va_list argp;

    matrix_dim(mtx,ROW,COL);

    mtx(1,1) = d;

    va_start(argp,d);
 
    for( j=2,i=1; (i <= mtx.rows()); i++ )
    {
        for( ; (j <= mtx.cols()); j++ )
        {
            mtx(i,j) = va_arg(argp,double);
        }

        j=1;
    }

    va_end(argp);
}

/*****************************************************************************/

void    matrix_data( matrix &mtx, double d, ... )
{
int     i,j;
va_list argp;

    mtx(1,1) = d;

    va_start(argp,d);
 
    for( j=2,i=1; (i <= mtx.rows()); i++ )
    {
        for( ; (j <= mtx.cols()); j++ )
        {
            mtx(i,j) = va_arg(argp,double);
        }

        j=1;
    }

    va_end(argp);
}

/*****************************************************************************/

void    matrix_data( int ROW, int COL, matrix &mtx, char *data[] )
{
    matrix_dim(mtx,ROW,COL);
    matrix_data(mtx,data);
}

/*****************************************************************************/

void    matrix_data( matrix &mtx, char *data[] )
{
int     i,j,d;

    for( d=0,i=1; (i <= mtx.rows()); i++ )
    {
        for( j=1; (j <= mtx.cols()); j++ )
        {
            if( data[d] == NULL )
            {
                mtx(i,j) = 0.0;
            }
            else
            {
                mtx(i,j) = atof(data[d]);
                d++;
            }
        }
    }
}

/*****************************************************************************/

void    matrix_data( int ROW, int COL, matrix &mtx, char *buff )
{
    matrix_dim(mtx,ROW,COL);
    matrix_data(mtx,buff);
}

/*****************************************************************************/

void    matrix_data( matrix &mtx, char *buff )
{
char  **token;
int     count=0;

    token = STR_tokens(buff,count);

#ifdef  MATRIX_DEBUGF
int     i;

    MATRIX_debugf("void matrix_data( matrix &mtx, char *buff );\n");

    for( i=0; (i < count); i++ )
    {
        MATRIX_debugf("token[%02d] %s\n",i,token[i]);
    }
#endif

    matrix_data(mtx,token);
}

/*****************************************************************************/

char   *matrix_string( matrix &mtx, char *mask )
{
char   *buff; 
int     i,j;

    if( (buff=STR_Buff(STR_TEXT)) == NULL )
    {
        return(NULL);
    }

    for( i=1; (i <= mtx.rows()); i++ )
    {
        for( j=1; (j <= mtx.cols()); j++ )
        {
            if( strlen(buff) > 0 )
            {
                strncat(buff,",",STR_TEXT);
            }

            strncat(buff,STR_stringf(mask,mtx(i,j)),STR_TEXT);
        }
    }

    return(buff);
}

/*****************************************************************************/

char   *matrix_string( matrix &mtx )
{
    return(matrix_string(mtx,"%lf"));
}

/*****************************************************************************/

void    matrix_array_put( matrix &dest, matrix &srce, int item )
{
int     i,j,offset;

    if( dest.rows() == srce.rows() )
    {
        if( (dest.cols() % srce.cols()) != 0 )
        {
            MATRIX_errorf("matrix_array_put(...)\n");
            return;
        }

        offset = (item-1) * srce.cols();

        for( i=1; (i <= srce.rows()); i++ )
        {
            for( j=1; (j <= srce.cols()); j++ )
            {
                dest(i,offset+j) = srce(i,j);
            }
        }
    }
    else
    if( dest.cols() == srce.cols() )
    {
        if( (dest.rows() % srce.rows()) != 0 )
        {
            MATRIX_errorf("matrix_array_put(...)\n");
            return;
        }

        offset = (item-1) * srce.rows();

        for( i=1; (i <= srce.rows()); i++ )
        {
            for( j=1; (j <= srce.cols()); j++ )
            {
                dest(offset+i,j) = srce(i,j);
            }
        }
    }
}

/******************************************************************************/

void    matrix_array_get( matrix &dest, matrix &srce, int item )
{
int     i,j,offset;

    if( dest.rows() == srce.rows() )
    {
        if( (srce.cols() % dest.cols()) != 0 )
        {
            MATRIX_errorf("matrix_array_get(...)\n");
            return;
        }

        offset = (item-1) * dest.cols();

        for( i=1; (i <= dest.rows()); i++ )
        {
            for( j=1; (j <= dest.cols()); j++ )
            {
                dest(i,j) = srce(i,offset+j);
            }
        }
    }
    else
    if( dest.cols() == srce.cols() )
    {
        if( (srce.rows() % dest.rows()) != 0 )
        {
            MATRIX_errorf("matrix_array_get(...)\n");
            return;
        }

        offset = (item-1) * dest.rows();

        for( i=1; (i <= dest.rows()); i++ )
        {
            for( j=1; (j <= dest.cols()); j++ )
            {
                dest(i,j) = srce(offset+i,j);
            }
        }
    }
}

/******************************************************************************/

int matrix_array_get( matrix dest[], matrix &srce, int max )
{
int i,r,c,items=0;

    r = dest[0].rows();
    c = dest[0].cols();

    if( r == srce.rows() )
    {
        if( (srce.cols() % c) != 0 )
        {
            MATRIX_errorf("matrix_array_get(...)\n");
            return(0);
        }

        items = srce.cols() / c;
    }
    else
    if( c == srce.cols() )
    {
        if( (srce.rows() % r) != 0 )
        {
            MATRIX_errorf("matrix_array_get(...)\n");
            return(0);
        }

        items = srce.rows() / r;
    }
    else
    {
        MATRIX_errorf("matrix_array_get(...)\n");
        return(0);
    }

    for( i=0; ((i < items) && (i < max)); i++ )
    {
        matrix_dim(dest[i],r,c);
        matrix_array_get(dest[i],srce,i+1);
    }

    return(i);
}

/******************************************************************************/

int matrix_array_put( matrix &dest, matrix srce[], int max )
{
int i,r,c,items;

    r = srce[0].rows();
    c = srce[0].cols();

    if( r == dest.rows() )
    {
        if( (dest.cols() % c) != 0 )
        {
            MATRIX_errorf("matrix_array_put(...)\n");
            return(0);
        }

        items = dest.cols() / c;
    }
    else
    if( c == dest.cols() )
    {
        if( (dest.rows() % r) != 0 )
        {
            MATRIX_errorf("matrix_array_put(...)\n");
            return(0);
        }

        items = dest.rows() / r;
    }
    else
    {
        MATRIX_errorf("matrix_array_put(...)\n");
        return(0);
    }

    for( i=0; ((i < items) && (i < max)); i++ )
    {
        matrix_dim(dest[i],r,c);
        matrix_array_put(dest,srce[i],i+1);
    }

    return(i);
}

/******************************************************************************/

void    matrix_data( matrix &srce, VECDBL dest )
{
int     i,j,k;

    for( k=1,i=1; (i <= srce.rows()); i++ )
    {
        for( j=1; (j <= srce.cols()); j++,k++ )
        {
            dest[k] = srce(i,j);
        }
    }
}

/******************************************************************************/

void    matrix_data( VECDBL srce, matrix &dest )
{
int     i,j,k;

    for( k=1,i=1; (i <= dest.rows()); i++ )
    {
        for( j=1; (j <= dest.cols()); j++,k++ )
        {
            dest(i,j) = srce[k];
        }
    }
}
/******************************************************************************/

void    matrix_data( matrix &srce, VECFLT dest )
{
int     i,j,k;

    for( k=1,i=1; (i <= srce.rows()); i++ )
    {
        for( j=1; (j <= srce.cols()); j++,k++ )
        {
            dest[k] = (float)srce(i,j);
        }
    }
}

/******************************************************************************/

void    matrix_data( VECFLT srce, matrix &dest )
{
int     i,j,k;

    for( k=1,i=1; (i <= dest.rows()); i++ )
    {
        for( j=1; (j <= dest.cols()); j++,k++ )
        {
            dest(i,j) = (double)srce[k];
        }
    }
}
/******************************************************************************/

void    matrix_data( matrix &srce, VECINT dest )
{
int     i,j,k;

    for( k=1,i=1; (i <= srce.rows()); i++ )
    {
        for( j=1; (j <= srce.cols()); j++,k++ )
        {
            dest[k] = (int)srce(i,j);
        }
    }
}

/******************************************************************************/

void    matrix_data( VECINT srce, matrix &dest )
{
int     i,j,k;

    for( k=1,i=1; (i <= dest.rows()); i++ )
    {
        for( j=1; (j <= dest.cols()); j++,k++ )
        {
            dest(i,j) = (double)srce[k];
        }
    }
}

/******************************************************************************/

void    matrix_data( matrix &srce, MTXDBL dest )
{
VECDBL  vdest=dest[0];

    matrix_data(srce,vdest);
}

/******************************************************************************/

void    matrix_data( MTXDBL srce, matrix &dest )
{
VECDBL  vsrce=srce[0];

    matrix_data(vsrce,dest);
}

/******************************************************************************/

void    matrix_data( matrix &srce, MTXFLT dest )
{
VECFLT  vdest=dest[0];

    matrix_data(srce,vdest);
}

/******************************************************************************/

void    matrix_data( MTXFLT srce, matrix &dest )
{
VECFLT  vsrce=srce[0];

    matrix_data(vsrce,dest);
}

/******************************************************************************/

void    matrix_data( matrix &srce, MTXINT dest )
{
VECINT  vdest=dest[0];

    matrix_data(srce,vdest);
}

/******************************************************************************/

void    matrix_data( MTXINT srce, matrix &dest )
{
VECINT  vsrce=srce[0];

    matrix_data(vsrce,dest);
}

/******************************************************************************/

void    matrix_double( matrix &mtx, double data[], int type )
{
int     r,c,d;

    if( type == MATRIX_BY_COL )         // Fill column by column.
    {
        for( d=0,c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
        {
            for( r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
            {
                data[d++] = mtx(r,c);
            }
        }
    }
    else
    if( type == MATRIX_BY_ROW )         // Fill row by row.
    {
        for( d=0,r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
        {
            for( c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
            {
                data[d++] = mtx(r,c);
            }
        }
    }
}

/******************************************************************************/

void    matrix_double( matrix &mtx, double data[] )
{
    matrix_double(mtx,data,MATRIX_BY_DEFAULT);
}

/******************************************************************************/

double *matrix_double( matrix &mtx, int type )
{
static  double  data[MATRIX_LINEAR_MAX];

    matrix_double(mtx,data,type);
    return(data);
}

/******************************************************************************/

double *matrix_double( matrix &mtx )
{
    return(matrix_double(mtx,MATRIX_BY_DEFAULT));
}

/******************************************************************************/

void    matrix_double( double data[], matrix &mtx, int type )
{
int     r,c,d;

    if( type == MATRIX_BY_COL )         // Fill column by column.
    {
        for( d=0,c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
        {
            for( r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
            {
                mtx(r,c) = data[d++];
            }
        }
    }
    else
    if( type == MATRIX_BY_ROW )         // Fill row by row.
    {
        for( d=0,r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
        {
            for( c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
            {
                mtx(r,c) = data[d++];
            }
        }
    }
}

/******************************************************************************/

void    matrix_double( double data[], matrix &mtx )
{
    matrix_double(data,mtx,MATRIX_BY_DEFAULT);
}

/******************************************************************************/

void    matrix_float( matrix &mtx, float data[], int type )
{
int     r,c,d;

    if( type == MATRIX_BY_COL )         // Row changes more frequently...
    {
        for( d=0,c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
        {
            for( r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
            {
                data[d++] = mtx.F(r,c);
            }
        }
    }
    else
    if( type == MATRIX_BY_ROW )         // Column changes more frequently...
    {
        for( d=0,r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
        {
            for( c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
            {
                data[d++] = mtx.F(r,c);
            }
        }
    }
}

/******************************************************************************/

void    matrix_float( matrix &mtx, float data[] )
{
    matrix_float(mtx,data,MATRIX_BY_DEFAULT);
}

/******************************************************************************/

float  *matrix_float( matrix &mtx, int type )
{
static  float  data[MATRIX_LINEAR_MAX];

    matrix_float(mtx,data,type);
    return(data);
}

/******************************************************************************/

float  *matrix_float( matrix &mtx )
{
    return(matrix_float(mtx,MATRIX_BY_DEFAULT));
}

/******************************************************************************/

void    matrix_float( float data[], matrix &mtx, int type )
{
int     r,c,d;

    if( type == MATRIX_BY_COL )         // Row changes more frequently...
    {
        for( d=0,c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
        {
            for( r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
            {
                mtx(r,c) = (double)data[d++];
            }
        }
    }
    else
    if( type == MATRIX_BY_ROW )         // Column changes more frequently...
    {
        for( d=0,r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
        {
            for( c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
            {
                mtx(r,c) = (double)data[d++];
            }
        }
    }
}

/******************************************************************************/

void    matrix_float( float data[], matrix &mtx )
{
    matrix_float(data,mtx,MATRIX_BY_DEFAULT);
}

/******************************************************************************/

void    matrix_int( matrix &mtx, int data[], int type )
{
int     r,c,d;

    if( type == MATRIX_BY_COL )         // Row changes more frequently...
    {
        for( d=0,c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
        {
            for( r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
            {
                data[d++] = mtx.I(r,c);
            }
        }
    }
    else
    if( type == MATRIX_BY_ROW )         // Column changes more frequently...
    {
        for( d=0,r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
        {
            for( c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
            {
                data[d++] = mtx.I(r,c);
            }
        }
    }
}

/******************************************************************************/

void    matrix_int( matrix &mtx, int data[] )
{
    matrix_int(mtx,data,MATRIX_BY_DEFAULT);
}

/******************************************************************************/

int    *matrix_int( matrix &mtx, int type )
{
static  int  data[MATRIX_LINEAR_MAX];

    matrix_int(mtx,data,type);
    return(data);
}

/******************************************************************************/

int    *matrix_int( matrix &mtx )
{
    return(matrix_int(mtx,MATRIX_BY_DEFAULT));
}

/******************************************************************************/

void    matrix_int( int data[], matrix &mtx, int type )
{    
int     r,c,d;

    if( type == MATRIX_BY_COL )         // Row changes more frequently...
    {
        for( d=0,c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
        {
            for( r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
            {
                mtx(r,c) = (double)data[d++];
            }
        }
    }
    else
    if( type == MATRIX_BY_ROW )         // Column changes more frequently...
    {
        for( d=0,r=1; ((r <= mtx.rows()) && (d < MATRIX_LINEAR_MAX)); r++ )
        {
            for( c=1; ((c <= mtx.cols()) && (d < MATRIX_LINEAR_MAX)); c++ )
            {
                mtx(r,c) = (double)data[d++];
            }
        }
    }
}

/******************************************************************************/

void    matrix_int( int data[], matrix &mtx )
{
    matrix_int(data,mtx,MATRIX_BY_DEFAULT);
}

/******************************************************************************/

matrix *M( matrix mtx[], int n )
{
    if( mtx == NULL )
    {
        return(NULL);
    }

    return(&mtx[n]);
}

/******************************************************************************/

