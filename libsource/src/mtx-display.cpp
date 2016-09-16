/*****************************************************************************/
/* Matrix display.                                                           *
/*****************************************************************************/

void    dis( matrix &mtx )
{ 
    dis(NULL,mtx);
}

/*****************************************************************************/

void    dis( char *s, matrix &mtx )
{
    dis(NULL,mtx,2);
}

/*****************************************************************************/

void    dis( char *s, matrix &mtx, int sig )
{
int     i,j;

    if( s == NULL )
    {
        fprintf(stderr,"Matrix is %i by %i\n",mtx.row,mtx.col);
    }
    else
    {
        fprintf(stderr,"Matrix %s is %i by %i\n",s,mtx.row,mtx.col);
    }

    for( i=1; (i <= mtx.row); i++ ) 
    {
        for( j=1; (j <= mtx.col); j++ ) 
	{
            printf("% .*lf\t",sig,mtx.mtx[i][j]);
	}
        printf("\n");
    }

    printf("\n");
}

/*****************************************************************************/

void    dis( char *s, double val, int sig )
{
    fprintf(stderr,"%s is % .*lf\n",s,sig,val);
}

/*****************************************************************************/

