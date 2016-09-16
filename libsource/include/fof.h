/******************************************************************************/
/*                                                                            */
/* MODULE  : FOF.h                                                            */
/*                                                                            */
/* PURPOSE : Flock of FOBs API functions.                                     */
/*                                                                            */
/* DATE    : 16/Feb/2001                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 16/Feb/2001 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define FOF_DEBUG() if( !FOF_API_start(printf,printf,printf) ) { printf("Cannot start FOF API.\n"); exit(0); }

/******************************************************************************/

#define FOF_FOB_INVALID     -1              // Invalid FOB value.

/******************************************************************************/

struct  FOF_FOB_Item                        // Details for each FOB in FOF...
{
    int       ID;                           // FOB API handle.
    int       comX;                         // COM port number.
    int       birds;                        // Number of birds.
    BOOL      started;                      // Flying flag.
    int       first;                        // First FOF bird number on this FOB.
    int       last;                         // Last FOF bird number on this FOB.
    BOOL      fresh;

    matrix    POMX;                         // Temporary matrices for FOB data...
    matrix    AOMX;
    matrix    ROMX;
    matrix    RTMX;
    matrix    QTMX;
};

/******************************************************************************/

int     FOF_messgf( const char *mask, ... );
int     FOF_errorf( const char *mask, ... );
int     FOF_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    FOF_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    FOF_API_stop( void );
BOOL    FOF_API_check( void );

/******************************************************************************/

void    FOF_Init( void );
void    FOF_Init( int fob );

void    FOF_BirdStuff( void );
int     FOF_Birds( void );
int     FOF_BirdIndex( int fob, int bird );
void    FOF_BirdIndex( int index, int &fob, int &bird );

void    FOF_ID( int fofBird, int &fobID, int &fobBird );

BOOL    FOF_Open( int comX[], int birds[], BYTE addr, long baud, float freq, WORD scale, BYTE datamode, BYTE dataformat[], BYTE hemisphere[], char *rtmx );
BOOL    FOF_Open( int birdX[], BYTE addr, long baud, float freq, WORD scale, BYTE datamode, BYTE dataformat, BYTE hemisphere, char *rtmx );
BOOL    FOF_Open( int birdX[], BYTE datamode, BYTE dataformat );
BOOL    FOF_Open( char *cnfg );

void    FOF_Close( void );

BOOL    FOF_Start( BYTE dataformat[] );
BOOL    FOF_Start( void );

void    FOF_Stop( void );

BOOL    FOF_GetPosn( matrix &pomx, matrix &frmx );
BOOL    FOF_GetPosn( matrix &pomx );

BOOL    FOF_GetAngles( matrix &aomx, matrix &frmx );
BOOL    FOF_GetAngles( matrix &aomx );

BOOL    FOF_GetPosnAngles( matrix &pomx, matrix &aomx, matrix &frmx );
BOOL    FOF_GetPosnAngles( matrix &pomx, matrix &aomx );

BOOL    FOF_GetROMX( matrix &romx, matrix &frmx );
BOOL    FOF_GetROMX( matrix &romx );

BOOL    FOF_GetPosnROMX( matrix &pomx, matrix &romx, matrix &frmx );
BOOL    FOF_GetPosnROMX( matrix &pomx, matrix &romx );

BOOL    FOF_GetRTMX( matrix &rtmx, matrix &frmx );
BOOL    FOF_GetRTMX( matrix &rtmx );

BOOL    FOF_GetQTMX( matrix &qtmx, matrix &frmx );
BOOL    FOF_GetQTMX( matrix &qtmx );

BOOL    FOF_GetPosnQTMX( matrix &pomx, matrix &qtmx, matrix &frmx );
BOOL    FOF_GetPosnQTMX( matrix &pomx, matrix &qtmx );

BOOL    FOF_GetWhatever( matrix *pomx, matrix *aomx, matrix *romx, matrix *qtmx, matrix *rtmx, matrix &frmx );
BOOL    FOF_GetWhatever( matrix *pomx, matrix *aomx, matrix *romx, matrix *qtmx, matrix *rtmx );

BOOL    FOF_NewPosn( matrix &pomx );
BOOL    FOF_NewROMX( matrix &romx );
BOOL    FOF_NewRTMX( matrix &rtmx );

BOOL    FOF_Flying( void );

void    FOF_DimPOMX( matrix &pomx );
void    FOF_DimAOMX( matrix &aomx );
void    FOF_DimROMX( matrix &romx );
void    FOF_DimRTMX( matrix &rtmx );
void    FOF_DimQTMX( matrix &qtmx );
void    FOF_DimFRMX( matrix &frmx );

/******************************************************************************/

#define FOF_FRMX_ROW    5
#define FOF_FRMX_COL    1
#define FOF_FRMX_FOB    1
#define FOF_FRMX_BIRD   2
#define FOF_FRMX_FRAME  3
#define FOF_FRMX_FRESH  4
#define FOF_FRMX_TIME   5

/******************************************************************************/

