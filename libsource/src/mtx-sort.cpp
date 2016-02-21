/******************************************************************************/
/* MTX-SORT.CPP                                                        (V2.2) */
/******************************************************************************/


/******************************************************************************/

int     matrix_sort_cmp( const void *e1, const void *e2 )
{
int     cmp=0;
struct  matrix_sort_tag  *d1,*d2;

    d1 = (struct matrix_sort_tag *)e1;
    d2 = (struct matrix_sort_tag *)e2;

    if( d1->value > d2->value )
    {
        cmp = +1;
    }
    else
    if( d1->value < d2->value )
    {
        cmp = -1;
    }

    return(cmp);
}

/******************************************************************************/

void    matrix_sortrows( matrix &dest, matrix &srce, int column )
{
struct  matrix_sort_tag  *data;
int     i,j;

    // Check that sort column exists matrix...
    if( !srce.exist(1,column) )
    {
        MATRIX_errorf("matrix_sortrows(...) Invalid column specified.\n");
        return;
    }

    // Allocate memory for sort list...
    if( (data=(struct matrix_sort_tag *)calloc(sizeof(struct matrix_sort_tag),srce.row)) == NULL )
    {
        MATRIX_errorf("matrix_sortrows(...) Cannot allocate memory.\n");
        return;
    }

    // Put column and index values in sort list...
    for( i=1; (i <= srce.row); i++ )
    {
        data[i-1].value = srce(i,column);
        data[i-1].index = i;
    }

    // Sort list...
    // void qsort( void *base, size_t num, size_t width, int (__cdecl *compare )(const void *elem1, const void *elem2 ) );
    qsort((void *)data,srce.row,sizeof(struct matrix_sort_tag),matrix_sort_cmp);

    // Create destination matrix to hold sorted version of source matrix...
    matrix_dim(dest,srce);

    // Copy rows from source to destination using sort list as index...
    for( j=1; (j <= srce.col); j++ )
    {
        for( i=1; (i <= srce.row); i++ )
        {
            dest(i,j) = srce(data[i-1].index,j);
        }
    }

    // Release sort-list memory...
    free(data);
}

/******************************************************************************/

void    matrix_sortcols( matrix &dest, matrix &srce, int row )
{
struct  matrix_sort_tag  *data;
int     i,j;

    // Check that sort row exists matrix...
    if( !srce.exist(row,1) )
    {
        MATRIX_errorf("matrix_sortcols(...) Invalid row specified.\n");
        return;
    }

    // Allocate memory for sort list...
    if( (data=(struct matrix_sort_tag *)calloc(sizeof(struct matrix_sort_tag),srce.col)) == NULL )
    {
        MATRIX_errorf("matrix_sortcols(...) Cannot allocate memory.\n");
        return;
    }

    // Put row and index values in sort list...
    for( i=1; (i <= srce.col); i++ )
    {
        data[i-1].value = srce(row,i);
        data[i-1].index = i;
    }

    // Sort list...
    qsort((void *)data,srce.col,sizeof(struct matrix_sort_tag),matrix_sort_cmp);

    // Create destination matrix to hold sorted version of source matrix...
    matrix_dim(dest,srce);

    // Copy columns from source to destination using sort list as index...
    for( j=1; (j <= srce.row); j++ )
    {
        for( i=1; (i <= srce.col); i++ )
        {
            dest(j,i) = srce(j,data[i-1].index);
        }
    }

    // Release sort-list memory...
    free(data);
}

/******************************************************************************/

void    matrix_sortrows( matrix &dest, matrix &srce )
{
    matrix_sortrows(dest,srce,1);
}

/******************************************************************************/

void    matrix_sortcols( matrix &dest, matrix &srce )
{
    matrix_sortcols(dest,srce,1);
}

/******************************************************************************/

matrix  sortrows( matrix &A, int column )
{
MTXRETN mtx;

    matrix_sortrows(mtx,A,column);
    return(mtx);
}

/******************************************************************************/

matrix  sortrows( matrix &A )
{
MTXRETN mtx;

    matrix_sortrows(mtx,A);
    return(mtx);
}

/******************************************************************************/

matrix  sortcols( matrix &A, int row )
{
MTXRETN mtx;

    matrix_sortcols(mtx,A,row);
    return(mtx);
}

/******************************************************************************/

matrix  sortcols( matrix &A )
{
MTXRETN mtx;

    matrix_sortcols(mtx,A);
    return(mtx);
}

/******************************************************************************/

int     matrix_cmp( const void *e1, const void *e2 )
{
int     cmp=0;
double  d1,d2;

    d1 = *((double *)e1);
    d2 = *((double *)e2);

    if( d1 > d2 )
    {
        cmp = +1;
    }
    else
    if( d1 < d2 )
    {
        cmp = -1;
    }

    return(cmp);
}

/*****************************************************************************/

void    matrix_sort( matrix &dest, matrix &srce )
{
    if( srce.isrowvector() )
    {
        matrix_sortcols(dest,srce);
    }
    else
    if( srce.iscolvector() )
    {
        matrix_sortrows(dest,srce);
    }
}

/*****************************************************************************/

matrix  sort( matrix &A )
{
MTXRETN mtx;

    matrix_sort(mtx,A);
    return(mtx);
}

/*****************************************************************************/

