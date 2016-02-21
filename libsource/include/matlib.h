/******************************************************************************/

#include "mfunc.h"

/******************************************************************************/

int      MATLIB_messgf( const char *mask, ... );
int      MATLIB_errorf( const char *mask, ... );
int      MATLIB_debugf( const char *mask, ... );

/******************************************************************************/

BOOL     MATLIB_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void     MATLIB_API_stop( void );
BOOL     MATLIB_API_check( void );

/******************************************************************************/

void     MATLIB_dimensions( matrix &mtx, int &rows, int &cols );
void     MATLIB_dimensions( mxArray *matlab, int &rows, int &cols );

/******************************************************************************/

mxArray *MATLIB_convert( double scalar );
mxArray *MATLIB_convert( float scalar );
mxArray *MATLIB_convert( int scalar );
mxArray *MATLIB_convert( matrix &mtx );

void     MATLIB_convert( mxArray *matlab, double &scalar );
void     MATLIB_convert( mxArray *matlab, float &scalar );
void     MATLIB_convert( mxArray *matlab, int &scalar );
void     MATLIB_convert( mxArray *matlab, matrix &mtx );

/******************************************************************************/

void     MATLIB_Pivot( matrix &X, matrix &err, matrix &A );
void     MATLIB_Minqdef( matrix &x, matrix &y, int &ier, matrix &c, matrix &G, matrix &A, matrix &b, matrix &eq, int prt, matrix &xx );

/******************************************************************************/

