/******************************************************************************/
/*                                                                            */
/* MODULE  : MATDAT.h                                                         */
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

#ifndef MATDAT_H
#define MATDAT_H

/******************************************************************************/

#define MATDAT_STRLEN 40

struct MATDAT_Variable
{
    char Name[MATDAT_STRLEN];
    int Type;
    void *Data;
    int Column;

    int Items;
    int Rows;
    int Columns;
};

/******************************************************************************/

class MATDAT
{
private:
public:
    char ObjectName[MATDAT_STRLEN];

    int Rows;
    int Columns;
    int Variables;

    int Row;

    matrix *Mtx;

#define MATDAT_VARIABLES 128
    struct MATDAT_Variable Variable[MATDAT_VARIABLES];

    MATDAT( );
    MATDAT( char *name, matrix *mtx, int rows );
    MATDAT( char *name, matrix *mtx );
    MATDAT( char *name, int rows );
    MATDAT( char *name );

   ~MATDAT( );

    void Init( void );
    void Init( char *name, matrix *mtx, int rows );

    char *Name( void );
    matrix *Matrix( void );

    void SetRows( int rows );
    int GetRows( void );
    int GetRow( void );
    int GetColumns( void );

    BOOL RowValid( int row );

    BOOL Full( void );

    void Reset( void );

    int GetVariables( void );
    void GetVariable( int index, char *name, int &items, int &rows, int &cols );

    void AddVariable( char *name, int type, void *data, int items );
    void AddVariable( char *name, int type, void *data );
    void AddVariable( char *name, BYTE &data );
    void AddVariable( char *name, BYTE *data, int items );
    void AddVariable( char *name, int &data );
    void AddVariable( char *name, int *data, int items );
    void AddVariable( char *name, long &data );
    void AddVariable( char *name, long *data, int items );
    void AddVariable( char *name, DWORD &data );
    void AddVariable( char *name, DWORD *data, int items );
    void AddVariable( char *name, float &data );
    void AddVariable( char *name, float *data, int items );
    void AddVariable( char *name, double &data );
    void AddVariable( char *name, double *data, int items );
    void AddVariable( char *name, matrix &data );
    void AddVariable( char *name, matrix *data, int items );

    void RowSave( int row );
    void RowSave( void );

    void RowLoad( int row );

    void RowSwap( int row1, int row2 );

    int FindVariable( void *variable );
    BOOL GetVariable( void *variable, char *name, int &items, int &rows, int &cols );

    int GetData( void *variable, matrix data[] );
    int GetData( void *variable, matrix &data );

    int GetData( BYTE &variable, matrix data[] );
    int GetData( BYTE &variable, matrix &data );
    int GetData( int &variable, matrix data[] );
    int GetData( int &variable, matrix &data );
    int GetData( long &variable, matrix data[] );
    int GetData( long &variable, matrix &data );
    int GetData( DWORD &variable, matrix data[] );
    int GetData( DWORD &variable, matrix &data );
    int GetData( float &variable, matrix data[] );
    int GetData( float &variable, matrix &data );
    int GetData( double &variable, matrix data[] );
    int GetData( double &variable, matrix &data );
    int GetData( matrix &variable, matrix data[] );
    int GetData( matrix &variable, matrix &data );
};

/******************************************************************************/

char   &MATDAT_char( void *vptr, int index );
char   &MATDAT_char( void *vptr );
int    &MATDAT_int( void *vptr, int index );
int    &MATDAT_int( void *vptr );
long   &MATDAT_long( void *vptr, int index );
long   &MATDAT_long( void *vptr );
DWORD  &MATDAT_DWORD( void *vptr, int index );
DWORD  &MATDAT_DWORD( void *vptr );
float  &MATDAT_float( void *vptr, int index );
float  &MATDAT_float( void *vptr );
double &MATDAT_double( void *vptr, int index );
double &MATDAT_double( void *vptr );
matrix *MATDAT_matrix( void *vptr, int index );
matrix *MATDAT_matrix( void *vptr );

/******************************************************************************/

#define MATDAT_errorf  printf
#define MATDAT_debugf  printf
#define MATDAT_messgf  printf

/******************************************************************************/

#endif

