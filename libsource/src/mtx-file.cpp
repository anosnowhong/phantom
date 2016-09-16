/*****************************************************************************/
/* Matrix file I/O.                                                          */
/*****************************************************************************/

BOOL    matrix_read( FILE *FP, matrix &mtx, int row, int col )
{
BOOL    ok;
int     i,j;

    matrix_dim(mtx,row,col);

    for( ok=TRUE,i=1; (i <= mtx.row); i++ )
    {
        for( j=1; (j <= mtx.col); j++ )
        {
            ok = (fscanf(FP,"%lf",&mtx.mtx[i][j]) == 1);
        }
    }

    if( !ok )
    {
        MATRIX_errorf("BOOL matrix_read(FILE *FP,...) Cannot read data.\n");
    }

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_read( char *file, int &row, int &col )
{
FILE   *FP=NULL;
char   *buff=NULL;
BOOL    ok;
int     fld;
int     c,r,l,i;
BOOL    s;

    if( (FP=fopen(file,"r")) == NULL )
    {
        MATRIX_errorf("BOOL matrix_read(file=%s,...) Cannot open file.\n",file);
        return(FALSE);
    }

    if( (buff=(char *)malloc(MATRIX_READ_BUFF)) == NULL )
    {
        MATRIX_errorf("BOOL matrix_read(file=%s,...) Cannot allocate memory.\n",file);
        fclose(FP);
        return(FALSE);
    }

    ok = TRUE;
    c = 0;
    r = 1;
 
    for( fld=MATRIX_READ_COL; ((fld >= MATRIX_READ_ROW) && ok); )
    {
        memset(buff,0,MATRIX_READ_BUFF);

        if( fgets(buff,MATRIX_READ_BUFF,FP) == NULL )
        {
            fld--;
            continue;
        }

        switch( fld )
        {
            case MATRIX_READ_COL :
               l = strlen(buff);
               s = TRUE;

               for( i=0; (i < l); i++ )
               {
                   if( isdigit(buff[i]) && s )
                   {
                       c++;
                       s = FALSE;
                   }
                   else
                   if( isspace(buff[i]) )
                   {
                       s = TRUE;
                   }
               }

               fld--;
               break;

            case MATRIX_READ_ROW :
               r++;
               break;
        }
    }

    if( ok )
    {
        row = r;
        col = c;

#ifdef  MATRIX_DEBUGF
        MATRIX_debugf("BOOL martix_read(file=%s,row=%d,col=%d) OK.\n",file,row,col);
#endif
    }
    else
    {
        MATRIX_errorf("BOOL matrix_read(file=%s,...) Cannot parse file.\n",file);
    }

    if( FP != NULL )
    {
        fclose(FP);
        FP = NULL;
    }

    if( buff != NULL )
    {
        free(buff);
        buff = NULL;
    }

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_read( char *file, matrix &mtx )
{
FILE   *FP;
BOOL    ok=FALSE;
int     row,col;

//  Determine dimensions of matrix in file...
    if( !matrix_read(file,row,col) )
    {
        return(ok);
    }
 
//  Open file to read matrix...
    if( (FP=fopen(file,"r")) == NULL )
    {
        MATRIX_errorf("BOOL matrix_read(file=%s) Cannot open file.\n",file);
    }
    else
    {
        ok = matrix_read(FP,mtx,row,col);
        fclose(FP);
    }

#ifdef  MATRIX_DEBUGF
    MATRIX_debugf("BOOL matrix_read(file=%s) %s.\n",file,STR_OkFailed(ok));
#endif

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_read( char *file, matrix &mtx, int row, int col )
{
FILE   *FP;
BOOL    ok=FALSE;

    if( (FP=fopen(file,"r")) == NULL )
    {
        MATRIX_errorf("BOOL matrix_read(file=%s) Cannot open file.\n",file);
    }
    else
    {
        ok = matrix_read(FP,mtx,row,col);
        fclose(FP);
    }

#ifdef  MATRIX_DEBUGF
    MATRIX_debugf("BOOL matrix_read(file=%s) %s.\n",file,STR_OkFailed(ok));
#endif

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_write( FILE *FP, matrix &mtx, int first, int last, void (*func)( void ) )
{
BOOL    ok;
int     i,j,r,rows;

    if( first == -1 )
    {
        first = 1;
    }

    if( last == -1 )
    {
        last = mtx.row;
    }

    rows = (last-first+1);
    if( rows < 1 )
    {
        rows += mtx.row;
    }

    for( ok=TRUE,r=1,i=first; ((r <= rows) && ok); r++ )
    {
        for( j=1; ((j <= mtx.col) && ok); j++ )
        {
            ok = (fprintf(FP,"%.12lf\t",mtx.mtx[i][j]) > 0);
        }

        ok = (fprintf(FP,"\n") > 0);

        if( func != NULL )
        {
          (*func)();
        }

        if( ++i > mtx.row )
        {
            i = 1;
        }
    }

    if( !ok )
    {
        MATRIX_errorf("BOOL matrix_write(...) Error writing file.\n");
    }

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_write( char *file, char *mode, matrix &mtx, int first, int last, void (*func)( void ) )
{
FILE   *FP;
BOOL    ok=FALSE;

    if( (FP=fopen(file,mode)) == NULL )
    {
        MATRIX_errorf("BOOL matrix_write(file=%s,mode=%s,first=%d,last=%d) Cannot open file.\n",file,mode,first,last);
    }
    else
    {
        ok = matrix_write(FP,mtx,first,last,func);

        fclose(FP);
    }

#ifdef  MATRIX_DEBUGF
    MATRIX_debugf("BOOL matrix_write(file=%s,mode=%s,row=%d) %s.\n",file,mode,row,STR_OkFailed(ok));
#endif

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_write( char *file, matrix &mtx, int first, int last, void (*func)( void ) )
{
BOOL    ok=FALSE;

    ok = matrix_write(file,"w",mtx,first,last,func);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_write( char *file, matrix &mtx, int first, int last )
{
BOOL    ok=FALSE;

    ok = matrix_write(file,"w",mtx,first,last,NULL);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_write( char *file, char *mode, matrix &mtx, int row, void (*func)( void ) )
{
BOOL ok=FALSE;

    ok = matrix_write(file,mode,mtx,-1,row,func);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_write( char *file, matrix &mtx, int row, void (*func)( void ) )
{
BOOL    ok=FALSE;

    ok = matrix_write(file,"w",mtx,-1,row,func);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_write( char *file, matrix &mtx, int row )
{
BOOL ok;

    ok = matrix_write(file,mtx,-1,row,NULL);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_write( char *file, matrix &mtx, void (*func)( void ) )
{
BOOL    ok=FALSE;

    ok = matrix_write(file,"w",mtx,-1,-1,func);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_write( char *file, matrix &mtx )
{
BOOL    ok=FALSE;

    ok = matrix_write(file,"w",mtx,-1,-1,NULL);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_append( char *file, matrix &mtx, int row, void (*func)( void ) )
{
BOOL    ok=FALSE;

    ok = matrix_write(file,"a",mtx,-1,row,func);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_append( char *file, matrix &mtx, int row )
{
BOOL    ok=FALSE;

    ok = matrix_write(file,"a",mtx,-1,row,NULL);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_append( char *file, matrix &mtx )
{
BOOL    ok=FALSE;

    ok = matrix_write(file,"a",mtx,-1,-1,NULL);

    return(ok);
}

/*****************************************************************************/

BOOL    matrix_append( char *file, matrix &mtx, void (*func)( void ) )
{
BOOL    ok=FALSE;

    ok = matrix_write(file,"a",mtx,-1,-1,func);

    return(ok);
}

/*****************************************************************************/

void    empty( char *file )
{
FILE   *FP;

    if( (FP=fopen(file,"w")) != NULL )
    {
        fclose(FP);
    }
}

/*****************************************************************************/
