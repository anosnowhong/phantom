/******************************************************************************/

#define FILE_DEBUG() if( !FILE_API_start(printf,printf,printf) ) { printf("Cannot start FILE API.\n"); exit(0); }

/******************************************************************************/

#define FILE_LOAD  0
#define FILE_SAVE  1

char *FILE_Func( int func );

/******************************************************************************/

#define FILE_CALIBRATION_SEARCH   ".;..\\calib\\general"
#define FILE_CALIBRATION_PATH     "..\\calib\\general"

/******************************************************************************/

int     FILE_messgf( const char *mask, ... );
int     FILE_errorf( const char *mask, ... );
int     FILE_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    FILE_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    FILE_API_stop( void );
BOOL    FILE_API_check( void );

/******************************************************************************/

#define FILE_SIZE_INVALID 0xFFFFFFFF

DWORD   FILE_Size( char *file );
BOOL    FILE_Exist( char *file );
BOOL    FILE_Load( char *file, BYTE **buff, int &size );

/******************************************************************************/

char   *FILE_SearchPaths( char *path, char *subpath, char *name );
BOOL    FILE_SearchPaths( char *path, char *subpath, char *name, char *file, int size );
BOOL    FILE_SearchPaths( char *path, char *subpath, char *name, char *file );

char   *FILE_SearchPath( char *path, char *name );
BOOL    FILE_SearchPath( char *path, char *name, char *file );

char   *FILE_MachinePath( char *machine );
char   *FILE_MachinePath( void );

char   *FILE_MachinePath( char *path, char *name );
BOOL    FILE_MachinePath( char *path, char *name, char *file );

char   *FILE_Calibration( char *name );
BOOL    FILE_Calibration( char *name, char *file );
char   *FILE_CalibrationSilent( char *name );

char   *FILE_Path( char *file );

double  FILE_DiskSpace( void );
double  FILE_DiskSpace( char DriveLetter );

void    FILE_DiskSpace( double bytes, double *KB, double *MB, double *GB, double *TB );
double  FILE_DiskSpaceKB( double bytes );
double  FILE_DiskSpaceMB( double bytes );
double  FILE_DiskSpaceGB( double bytes );
double  FILE_DiskSpaceTB( double bytes );

void    FILE_Silent( BOOL flag );

/******************************************************************************/
