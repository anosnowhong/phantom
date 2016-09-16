/******************************************************************************/
/*                                                                            */
/* MODULE  : TRACKING.h                                                       */
/*                                                                            */
/* PURPOSE : Position TRACKING API for general access to tracking hardware.   */
/*                                                                            */
/* DATE    : 30/Jan/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 30/Jan/2002 - Initial development of module.                     */
/*                                                                            */
/* V1.2  JNI 06/Jul/2009 - Added support for Polhemus Liberty.                */
/*                                                                            */
/******************************************************************************/

#ifndef TRACKING
#define TRACKING

/******************************************************************************/

#define TRACKING_DEBUG() if( !TRACKING_API_start(printf,printf,printf) ) { printf("Cannot start TRACKING API.\n"); exit(0); }

/******************************************************************************/

#define TRACKING_INVALID    -1
#define TRACKING_NONE        0
#define TRACKING_OPTOTRAK    1
#define TRACKING_FOB         2
#define TRACKING_ROBOT       3
#define TRACKING_MOUSE       4
#define TRACKING_LIBERTY     5
#define TRACKING_MAX         6

/******************************************************************************/

#define TRACKING_SELECT      3
#define TRACKING_DEVICE      0
#define TRACKING_CONFIG      1
#define TRACKING_MARKER      2

/******************************************************************************/

int     TRACKING_messgf( const char *mask, ... );
int     TRACKING_errorf( const char *mask, ... );
int     TRACKING_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    TRACKING_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    TRACKING_API_stop( void );
BOOL    TRACKING_API_check( void );

/******************************************************************************/

int     TRACKING_DeviceCode( char *name );

BOOL    TRACKING_Check( void );

BOOL    TRACKING_Open( STRING select[] );
BOOL    TRACKING_Open( char *device, char *config, char *marker );
BOOL    TRACKING_Open( char *device, char *config );
void    TRACKING_Close( void );

BOOL    TRACKING_Type( int device );
BOOL    TRACKING_Started( void );

BOOL    TRACKING_Start( void );
void    TRACKING_Stop( void );

BOOL    TRACKING_GetPosn( int marker, matrix &pomx );
BOOL    TRACKING_GetPosn( matrix &pomx );

/******************************************************************************/

extern  STRING  TRACKING_Config[];
extern  struct  STR_TextItem    TRACKING_DeviceText[];

#define TRACKING_CONFIG_OPTOTRAK  TRACKING_Config[TRACKING_OPTOTRAK]
#define TRACKING_CONFIG_FOB       TRACKING_Config[TRACKING_FOB]
#define TRACKING_CONFIG_ROBOT     TRACKING_Config[TRACKING_ROBOT]
#define TRACKING_CONFIG_MOUSE     TRACKING_Config[TRACKING_MOUSE]
#define TRACKING_CONFIG_LIBERTY   TRACKING_Config[TRACKING_LIBERTY]

void TRACKING_ConfigVariables( void );

/******************************************************************************/

#endif

