/******************************************************************************/
/*                                                                            */
/* MODULE  : CMDARG.h                                                         */
/*                                                                            */
/* PURPOSE : Command-Line Argument processing functions.                      */
/*                                                                            */
/* DATE    : 02/Dec/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 02/Dec/2000 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

char    CMDARG_code( char *argp, char **data );

BOOL    CMDARG_data( char *dest, char *data, int max );
BOOL    CMDARG_data( char *dest[], char *data, int max, int n );

BOOL    CMDARG_data( STRING dest, char *data );
BOOL    CMDARG_data( STRING dest[], char *data, int n );

BOOL    CMDARG_data( int &dest, char *data );
BOOL    CMDARG_data( int &dest, char *data, int min, int max );
BOOL    CMDARG_data( int dest[], char *data, int n );
BOOL    CMDARG_data( int dest[], char *data, int min, int max, int n );

BOOL    CMDARG_data( short &dest, char *data );
BOOL    CMDARG_data( short &dest, char *data, short min, short max );

BOOL    CMDARG_data( USHORT &dest, char *data );
BOOL    CMDARG_data( USHORT &dest, char *data, USHORT min, USHORT max );

BOOL    CMDARG_data( double &dest, char *data );
BOOL    CMDARG_data( double &dest, char *data, double min, double max );
BOOL    CMDARG_data( double dest[], char *data, int n );
BOOL    CMDARG_data( double dest[], char *data, double min, double max, int n );

BOOL    CMDARG_data( float &dest, char *data );
BOOL    CMDARG_data( float &dest, char *data, float min, float max );
BOOL    CMDARG_data( float dest[], char *data, int n );
BOOL    CMDARG_data( float dest[], char *data, float min, float max, int n );

/******************************************************************************/

