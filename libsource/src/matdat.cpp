/******************************************************************************/
/*                                                                            */
/* MODULE  : MATDAT.cpp                                                       */
/*                                                                            */
/* PURPOSE : Class to save experimental data to matrices.                     */
/*                                                                            */
/* DATE    : 21/Sep/2006                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 21/Sep/2006 - Initial Development of module.                     */
/*                                                                            */
/******************************************************************************/

#include <motor.h>

/******************************************************************************/

MATDAT::MATDAT( )
{
    Init();
}

/******************************************************************************/

MATDAT::MATDAT( char *name, matrix *mtx, int rows )
{
    Init(name,mtx,rows);
}

/******************************************************************************/

MATDAT::MATDAT( char *name, int rows )
{
matrix *mtx=NULL;

    Init(name,mtx,rows);
}

/******************************************************************************/

MATDAT::MATDAT( char *name, matrix *mtx )
{
int rows=0;

    Init(name,mtx,rows);
}

/******************************************************************************/

MATDAT::MATDAT( char *name )
{
matrix *mtx=NULL;
int rows=0;

    Init(name,mtx,rows);
}

/******************************************************************************/

MATDAT::~MATDAT( )
{
}

/******************************************************************************/

void MATDAT::Init( void )
{
int v;

    memset(ObjectName,0,MATDAT_STRLEN);
    Rows = 0;
    Columns = 0;
    Variables = 0;
    Row = 0;
    Mtx = NULL;

    for( v=0; (v < MATDAT_VARIABLES); v++ )
    {
        memset(Variable[v].Name,0,MATDAT_STRLEN);
        Variable[v].Type = CONFIG_TYPE_NONE;
        Variable[v].Data = NULL;
        Variable[v].Column = 0;
        Variable[v].Rows = 0;
        Variable[v].Columns = 0;
        Variable[v].Items = 0;
    }
}

/******************************************************************************/

void MATDAT::Init( char *name, matrix *mtx, int rows )
{
    Init();

    strncpy(ObjectName,name,MATDAT_STRLEN);

    if( mtx != NULL )
    {
        Mtx = mtx;
    }
    else
    {
        Mtx = new matrix();
    }

    if( rows != 0 )
    {
        SetRows(rows);
    }
}

/******************************************************************************/

char *MATDAT::Name( void )
{
    return(ObjectName);
}

/******************************************************************************/

matrix *MATDAT::Matrix( void )
{
    return(Mtx);
}

/******************************************************************************/

void MATDAT::SetRows( int rows )
{
    Rows = rows;
    Mtx->dim(Rows,Columns);
}

/******************************************************************************/

int MATDAT::GetRows( void )
{
    return(Rows);
}

/******************************************************************************/

int MATDAT::GetRow( void )
{
    return(Row);
}

/******************************************************************************/

int MATDAT::GetColumns( void )
{
    return(Columns);
}

/******************************************************************************/

BOOL MATDAT::RowValid( int row )
{
BOOL flag;

    flag = ((row >= 1) && (row <= Rows));

    return(flag);
}

/******************************************************************************/

BOOL MATDAT::Full( void )
{
BOOL flag;

    flag = (Row == Rows);

    return(flag);
}

/******************************************************************************/

void MATDAT::Reset( void )
{
    Row = 0;
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, int type, void *data, int items )
{
int rows=1,columns=1,elements;
matrix *m;

    // Check if already have maximum number of variables.
    if( Variables == MATDAT_VARIABLES )
    {
        MATDAT_errorf("%s: AddVariable(%s,...) Too many variables.\n","MATDAT",name);
        return;
    }

    // Special processing for matrices to get rows and columns.
    if( type == CONFIG_TYPE_MATRIX )
    {
        m = (matrix *)data;
        rows = m->rows();
        columns = m->cols();
    }

    elements = items * rows * columns;
    if( (Columns+elements) > MATRIX_LINEAR_MAX )
    {
        MATDAT_errorf("%s: AddVariable(%s,...) Too many columns.\n","MATDAT",name);
        return;
    }

    // Add details of variable to list.
    strncpy(Variable[Variables].Name,name,MATDAT_STRLEN);
    Variable[Variables].Type = type;
    Variable[Variables].Data = data;
    Variable[Variables].Column = 1+Columns;
    Variable[Variables].Rows = rows;
    Variable[Variables].Columns = columns;
    Variable[Variables].Items = items;

    elements = items * rows * columns;
    Columns += elements;

    Variables++;    
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, int type, void *data )
{
    AddVariable(name,type,data,1);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, BYTE &data )
{
    AddVariable(name,CONFIG_TYPE_CHAR,&data);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, BYTE *data, int items )
{
    AddVariable(name,CONFIG_TYPE_CHAR,data,items);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, int &data )
{
    AddVariable(name,CONFIG_TYPE_INT,&data);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, int *data, int items )
{
    AddVariable(name,CONFIG_TYPE_INT,data,items);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, long &data )
{
    AddVariable(name,CONFIG_TYPE_LONG,&data);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, long *data, int items )
{
    AddVariable(name,CONFIG_TYPE_LONG,data,items);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, DWORD &data )
{
    AddVariable(name,CONFIG_TYPE_DWORD,&data);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, DWORD *data, int items )
{
    AddVariable(name,CONFIG_TYPE_DWORD,data,items);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, float &data )
{
    AddVariable(name,CONFIG_TYPE_FLOAT,&data);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, float *data, int items )
{
    AddVariable(name,CONFIG_TYPE_FLOAT,data,items);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, double &data )
{
    AddVariable(name,CONFIG_TYPE_DOUBLE,&data);
}
/******************************************************************************/

void MATDAT::AddVariable( char *name, double *data, int items )
{
    AddVariable(name,CONFIG_TYPE_DOUBLE,data,items);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, matrix &data )
{
    AddVariable(name,CONFIG_TYPE_MATRIX,&data);
}

/******************************************************************************/

void MATDAT::AddVariable( char *name, matrix *data, int items )
{
    AddVariable(name,CONFIG_TYPE_MATRIX,data,items);
}

/******************************************************************************/

int MATDAT::GetVariables( void )
{
    return(Variables);
}

/******************************************************************************/

void MATDAT::GetVariable( int index, char *name, int &items, int &rows, int &cols )
{
    if( (index < 0) || (index >= Variables) )
    {
        return;
    }

    if( name != NULL )
    {
        strncpy(name,Variable[index].Name,MATDAT_STRLEN);
    }

    items = Variable[index].Items;
    rows = Variable[index].Rows;
    cols = Variable[index].Columns;
}

/******************************************************************************/

int MATDAT::FindVariable( void *variable )
{
int i,index;

    for( index=-1,i=0; (i < Variables); i++ )
    {
        if( variable == Variable[i].Data )
        {
            index = i;
        }
    }

    return(index);
}

/******************************************************************************/

BOOL MATDAT::GetVariable( void *variable, char *name, int &items, int &rows, int &cols )
{
int index;
BOOL ok=FALSE;

    if( (index=FindVariable(variable)) != -1 )
    {
        GetVariable(index,name,items,rows,cols);
        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

int MATDAT::GetData( void *variable, matrix data[] )
{
int index,items,cols,rows,elements;
int i,r,c1,c2;

    if( (index=FindVariable(variable)) == -1 )
    {
        return(-1);
    }

    GetVariable(index,NULL,items,rows,cols);
    elements = rows * cols;
    c1 = Variable[index].Column;
    c2 = Variable[index].Column + elements - 1;
    r = (Row == 0) ? Rows : Row;

    for( i=0; (i < items); i++ )
    {
        data[i].dim(r,elements);
        matrix_extract_sub(data[i],*Mtx,1,c1,r,c2);
        c1 += elements;
        c2 += elements;
    }

    return(items);
}

/******************************************************************************/

int MATDAT::GetData( void *variable, matrix &data )
{
int index,items,cols,rows,elements;
int r,c1,c2;

    if( (index=FindVariable(variable)) == -1 )
    {
        return(-1);
    }

    GetVariable(index,NULL,items,rows,cols);
    elements = items * rows * cols;
    c1 = Variable[index].Column;
    c2 = Variable[index].Column + elements - 1;
    r = (Row == 0) ? Rows : Row;

    data.dim(r,elements);
    matrix_extract_sub(data,*Mtx,1,c1,r,c2);

    return(items);
}

/******************************************************************************/

int MATDAT::GetData( BYTE &variable, matrix data[] )   { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( BYTE &variable, matrix &data )    { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( int &variable, matrix data[] )    { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( int &variable, matrix &data )     { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( long &variable, matrix data[] )   { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( long &variable, matrix &data )    { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( DWORD &variable, matrix data[] )  { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( DWORD &variable, matrix &data )   { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( float &variable, matrix data[] )  { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( float &variable, matrix &data )   { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( double &variable, matrix data[] ) { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( double &variable, matrix &data )  { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( matrix &variable, matrix data[] ) { return(GetData((void *)&variable,data)); }
int MATDAT::GetData( matrix &variable, matrix &data )  { return(GetData((void *)&variable,data)); }

/******************************************************************************/

void MATDAT::RowSave( int row )
{
int v,i,j,c;
matrix *m;
double d[MATRIX_LINEAR_MAX];

    if( !RowValid(row) )
    {
        return;
    }

    for( v=0; (v < Variables); v++ )
    {
        switch( Variable[v].Type )
        {
            case CONFIG_TYPE_CHAR :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   (*Mtx)(row,Variable[v].Column+i) = (double)MATDAT_char(Variable[v].Data,i);
               }
               break;

            case CONFIG_TYPE_INT :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   (*Mtx)(row,Variable[v].Column+i) = (double)MATDAT_int(Variable[v].Data,i);
               }
               break;

            case CONFIG_TYPE_LONG :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   (*Mtx)(row,Variable[v].Column+i) = (double)MATDAT_long(Variable[v].Data,i);
               }
               break;

            case CONFIG_TYPE_DWORD :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   (*Mtx)(row,Variable[v].Column+i) = (double)MATDAT_DWORD(Variable[v].Data,i);
               }
               break;

            case CONFIG_TYPE_FLOAT :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   (*Mtx)(row,Variable[v].Column+i) = (double)MATDAT_float(Variable[v].Data,i);
               }
               break;

            case CONFIG_TYPE_DOUBLE :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   (*Mtx)(row,Variable[v].Column+i) = MATDAT_double(Variable[v].Data,i);
               }
               break;

            case CONFIG_TYPE_MATRIX :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   m = MATDAT_matrix(Variable[v].Data,i);
                   matrix_double(*m,d);
                   for( j=0; (j < m->items()); j++ )
                   {
                       c = Variable[v].Column + (i * Variable[v].Rows * Variable[v].Columns) + j;
                       (*Mtx)(row,c) = d[j];
                   }
               }
               break;
        }
    }

    if( row > Row )
    {
        Row = row;
    }
}

/******************************************************************************/

void MATDAT::RowSave( void )
{
    if( !Full() )
    {
        RowSave(++Row);
    }
}

/******************************************************************************/

void MATDAT::RowLoad( int row )
{
int v,i,c;
matrix *m;
double d[MATRIX_LINEAR_MAX];

    if( !RowValid(row) )
    {
        return;
    }

    for( v=0; (v < Variables); v++ )
    {
        switch( Variable[v].Type )
        {
            case CONFIG_TYPE_CHAR :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   MATDAT_char(Variable[v].Data,i) = (BYTE)((*Mtx)(row,Variable[v].Column+i));
               }
               break;

            case CONFIG_TYPE_INT :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   MATDAT_int(Variable[v].Data,i) = (int)((*Mtx)(row,Variable[v].Column+i));
               }
               break;

            case CONFIG_TYPE_LONG :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   MATDAT_long(Variable[v].Data,i) = (long)((*Mtx)(row,Variable[v].Column+i));
               }
               break;

            case CONFIG_TYPE_DWORD :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   MATDAT_DWORD(Variable[v].Data,i) = (DWORD)((*Mtx)(row,Variable[v].Column+i));
               }
               break;

            case CONFIG_TYPE_FLOAT :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   MATDAT_float(Variable[v].Data,i) = (float)((*Mtx)(row,Variable[v].Column+i));
               }
               break;

            case CONFIG_TYPE_DOUBLE :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   MATDAT_double(Variable[v].Data,i) = (double)((*Mtx)(row,Variable[v].Column+i));
               }
               break;

            case CONFIG_TYPE_MATRIX :
               for( i=0; (i < Variable[v].Items); i++ )
               {
                   matrix_double((*Mtx)(row),d);
                   m = MATDAT_matrix(Variable[v].Data,i);
                   c = Variable[v].Column + (i * Variable[v].Rows * Variable[v].Columns);
                   matrix_double(&d[c-1],*m);
               }
               break;
        }
    }
}

/******************************************************************************/

void MATDAT::RowSwap( int row1, int row2 )
{
matrix M;
int i;

    M.dim(1,Columns);

    for( i=1; (i <= Columns); i++ )
    {
        M(1,i) = (*Mtx)(row1,i);
    }

    for( i=1; (i <= Columns); i++ )
    {
        (*Mtx)(row1,i) = (*Mtx)(row2,i);
    }

    for( i=1; (i <= Columns); i++ )
    {
        (*Mtx)(row2,i) = M(1,i);
    }
}

/******************************************************************************/

char &MATDAT_char( void *vptr, int index )
{
char *data;

    data = (char *)vptr;
    return(data[index]);
}

/******************************************************************************/

char &MATDAT_char( void *vptr )
{
    return(MATDAT_char(vptr,0));
}

/******************************************************************************/

int &MATDAT_int( void *vptr, int index )
{
int *data;

    data = (int *)vptr;
    return(data[index]);
}

/******************************************************************************/

int &MATDAT_int( void *vptr )
{
    return(MATDAT_int(vptr,0));
}

/******************************************************************************/

long &MATDAT_long( void *vptr, int index )
{
long *data;

    data = (long *)vptr;
    return(data[index]);
}

/******************************************************************************/

long &MATDAT_long( void *vptr )
{
    return(MATDAT_long(vptr,0));
}

/******************************************************************************/

DWORD &MATDAT_DWORD( void *vptr, int index )
{
DWORD *data;

    data = (DWORD *)vptr;
    return(data[index]);
}

/******************************************************************************/

DWORD &MATDAT_DWORD( void *vptr )
{
    return(MATDAT_DWORD(vptr,0));
}

/******************************************************************************/

float &MATDAT_float( void *vptr, int index )
{
float *data;

    data = (float *)vptr;
    return(data[index]);
}

/******************************************************************************/

float &MATDAT_float( void *vptr )
{
    return(MATDAT_float(vptr,0));
}

/******************************************************************************/

double &MATDAT_double( void *vptr, int index )
{
double *data;

    data = (double *)vptr;
    return(data[index]);
}

/******************************************************************************/

double &MATDAT_double( void *vptr )
{
    return(MATDAT_double(vptr,0));
}

/******************************************************************************/

matrix *MATDAT_matrix( void *vptr, int index )
{
matrix *data;

    data = (matrix *)vptr;
    return(&data[index]);
}

/******************************************************************************/

matrix *MATDAT_matrix( void *vptr )
{
    return(MATDAT_matrix(vptr,0));
}

/******************************************************************************/

