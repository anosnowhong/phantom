/******************************************************************************/
/*                                                                            */ 
/* MODULE  : OPTO.h                                                           */ 
/*                                                                            */ 
/* PURPOSE : Interface functions for OptoTrak Motion Sensor System.           */ 
/*                                                                            */ 
/* DATE    : 08/Jun/2000                                                      */ 
/*                                                                            */ 
/* CHANGES :                                                                  */ 
/*                                                                            */ 
/* V2.0  JNI 08/Jun/2000 - Re-developed from "mylib.lib" module of like name. */ 
/*                                                                            */ 
/* V2.1  JNI 06/Dec/2000 - Put delay in OPTO_start(...) function. This fixed  */ 
/*                         problem with initial position readings in the new  */ 
/*                         PCI/ISA Northern Digital libraries.                */ 
/*                                                                            */ 
/* V2.4  JNI 18/Jan/2002 - Made some changes to increase consistency with     */ 
/*                         other related APIs.                                */ 
/*                                                                            */ 
/******************************************************************************/

#define OPTO_DEBUG() if( !OPTO_API_start(printf,printf,printf) ) { printf("Cannot start OPTO API.\n"); exit(0); }

/******************************************************************************/

typedef Position3d           ND3D;               // Northern Digital raw 3d positions.
typedef RealType             NDreal;             // Northern Digital real type.

/******************************************************************************/

#define OPTO_MAX_MARKER      50                  // Maximum number of markers.
#define OPTO_MAX_PORT        4                   // Maximum port number.
#define OPTO_MIN_PORT        1                   // Minimum port number.
#define OPTO_MAX_CT          99999.0             // Maximum Collection Time (CT).

#define OPTO_MARKER_FREQ     2500.0              // Marker frequency.

// User-defined co-ordinate frames specified with a R/T matrix...
#define OPTO_RTMX_NONE       ""                  // No RTMX file.
#define OPTO_RTMX_FILE       "OPTOTRAK.CAL"      // Default RTMX file name.

#define OPTO_CONFIG_FILE     "OPTOTRAK.CFG"      // Default configuration file.

#define OPTO_NIF_FILE        "system"            // Default Network Information File.

// Default values for OPTO_open()...
#define OPTO_DEFAULT_FREQ    100.0               // Default frame rate frequency (Hz).
#define OPTO_DEFAULT_CT      0.0                 // Default collection time (maximum).
#define OPTO_DEFAULT_FLAG    0                   // Default collection flags.
#define OPTO_DEFAULT_RTMX    OPTO_RTMX_FILE      // Default RTMX file.

// Return code values...
#define OPTO_RC_OK           0
#define OPTO_RC_USER         OPTO_USER_ERROR_CODE
#define OPTO_RC_SYSTEM       OPTO_SYSTEM_ERROR_CODE

#define OPTO_SEEN_ROW        1                   // Rows in visible marker ("seen") matrix.
#define OPTO_SEEN_MTX        1,1

#define OPTO_POSN_SCALE      10.0

#define OPTO_FRAME_ERROR     0xFFFF

#define OPTO_ERROR_STRING    MAX_ERROR_STRING_LENGTH

#define OPTO_START_WAIT  100.0                   // Wait time for starting session. (V2.1)

/******************************************************************************/

struct  OPTO_MarkerItem
{
    int port;
    int marker;
};

#define OPTO_PORT_NULL       0
#define OPTO_MARKER_NULL     0

/******************************************************************************/

extern  float   OPTO_Frequency;
extern  int     OPTO_PortMarker[OPTO_MAX_PORT];
extern  float   OPTO_CollectionTime;
extern  STRING  OPTO_RTFN;
extern  int     OPTO_Flags;
extern  int     OPTO_ReadType;

// OPTO_ReadType
#define OPTO_READ_BLOCKING 0      // Use blocking data read function
#define OPTO_READ_AUTO     1      // Automatically request latest data (non-blocking)
#define OPTO_READ_MANUAL   2      // Manually request latest data (non-blocking)

/******************************************************************************/

int     OPTO_messgf( const char *mask, ... );
int     OPTO_errorf( const char *mask, ... );
int     OPTO_debugf( const char *mask, ... );

void    OPTO_Fail( char *func, int rc );

void    OPTO_ThreadLock( void );
void    OPTO_ThreadUnlock( void );

BOOL    OPTO_SetRTMX( char *file );

void    OPTO_RT( matrix &posn );
void    OPTO_RT( matrix posn[], int markers );

BOOL    OPTO_Markers( BOOL activate );
#define OPTO_MARKERS_ON      TRUE
#define OPTO_MARKERS_OFF     FALSE

BOOL    OPTO_Port( int port );

int    *OPTO_Markers( int port, int marker, ... );
int    *OPTO_Markers( int port[], int marker[] );
int     OPTO_Markers( void );

int     OPTO_Index( int port, int marker );
void    OPTO_Index( int index, int &port, int &marker );

BOOL    OPTO_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    OPTO_API_stop( void );
BOOL    OPTO_API_check( void );
BOOL    OPTO_API_restart( void );

BOOL    OPTO_Open( char *cnfg );
BOOL    OPTO_Open( void );
void    OPTO_Close( void );

BOOL    OPTO_Strober( int m1, int m2, int m3, int m4 );

BOOL    OPTO_Cnfg( void );
BOOL    OPTO_Cnfg( char *cnfg );

BOOL    OPTO_Start( float freq, int m1, int m2, int m3, int m4, float ct, char *rtmx, int flag );
BOOL    OPTO_Start( float freq, int m[], float ct, char *rtmx, int flag );
BOOL    OPTO_Start( float freq, int m[], char *rtmx );
BOOL    OPTO_Start( int m[], char *rtmx );
BOOL    OPTO_Start( int m[] );
BOOL    OPTO_Start( char *cnfg );
BOOL    OPTO_Start( void );

BOOL    OPTO_Started( void );

void    OPTO_Stop( void );

int     OPTO_Dim( matrix *posn, matrix *seen );

void    OPTO_ND3d2Posn( ND3D nd3d[], matrix &posn, matrix &seen, int markers, BOOL rtmx );
void    OPTO_ND3d2Posn( ND3D nd3d[], matrix posn[], BOOL seen[], int markers, BOOL rtmx );

UINT    OPTO_ReadBlocking( ND3D nd3d[], int markers );
UINT    OPTO_ReadNonBlocking( ND3D nd3d[], int markers, BOOL automatic );
BOOL    OPTO_RequestLatest( void );

UINT    OPTO_Posn( ND3D nd3d[], int markers );

//      Get OptoTrak marker positions (internal API functions)...
UINT    OPTO_Posn( matrix &posn, matrix &seen, BOOL rtmx );
UINT    OPTO_Posn( matrix &posn, matrix &seen, int markers, BOOL rtmx );
UINT    OPTO_Posn( matrix &posn, matrix &seen, int m1, int m2, BOOL rtmx );
UINT    OPTO_Posn( matrix posn[], BOOL seen[], BOOL rtmx );
UINT    OPTO_Posn( matrix posn[], BOOL seen[], int markers, BOOL rtmx );
UINT    OPTO_Posn( matrix posn[], BOOL seen[], int m1, int m2, BOOL rtmx );

//      Get OptoTrak marker positions, using user-defined co-ordinate frame if required...
UINT    OPTO_GetPosn( matrix &posn, matrix &seen );
UINT    OPTO_GetPosn( matrix &posn, matrix &seen, int markers );
UINT    OPTO_GetPosn( matrix &posn, matrix &seen, int m1, int m2 );
UINT    OPTO_GetPosn( matrix posn[], BOOL seen[] );
UINT    OPTO_GetPosn( matrix posn[], BOOL seen[], int markers );
UINT    OPTO_GetPosn( matrix posn[], BOOL seen[], int m1, int m2 );

UINT    OPTO_GetPosn( matrix &posn );
UINT    OPTO_GetPosn( matrix &posn, int markers );
UINT    OPTO_GetPosn( matrix &posn, int m1, int m2 );
UINT    OPTO_GetPosn( matrix posn[] );
UINT    OPTO_GetPosn( matrix posn[], int markers );
UINT    OPTO_GetPosn( matrix posn[], int m1, int m2 );

//      Get OptoTrak marker positions, using user-defined co-ordinate frame...
UINT    OPTO_UsrPosn( matrix &posn, matrix &seen );
UINT    OPTO_UsrPosn( matrix &posn, matrix &seen, int markers );
UINT    OPTO_UsrPosn( matrix &posn, matrix &seen, int m1, int m2 );
UINT    OPTO_UsrPosn( matrix posn[], BOOL seen[] );
UINT    OPTO_UsrPosn( matrix posn[], BOOL seen[], int markers );
UINT    OPTO_UsrPosn( matrix posn[], BOOL seen[], int m1, int m2 );

//      Included for compatibility...
#define OPTO_RTPosn  OPTO_UsrPosn

//      Get (raw) OptoTrak marker positions...
UINT    OPTO_RawPosn( matrix &posn, matrix &seen );
UINT    OPTO_RawPosn( matrix &posn, matrix &seen, int markers );
UINT    OPTO_RawPosn( matrix &posn, matrix &seen, int m1, int m2 );
UINT    OPTO_RawPosn( matrix posn[], BOOL seen[] );
UINT    OPTO_RawPosn( matrix posn[], BOOL seen[], int markers );
UINT    OPTO_RawPosn( matrix posn[], BOOL seen[], int m1, int m2 );

BOOL    OPTO_Seen( matrix &seen );
BOOL    OPTO_Seen( matrix &seen, int markers );
BOOL    OPTO_Seen( matrix &seen, int m1, int m2 );

float   OPTO_Freq( void );

/******************************************************************************/

#define OPTO_fail       OPTO_Fail   
#define OPTO_markers    OPTO_Markers
#define OPTO_start      OPTO_Start
#define OPTO_stop       OPTO_Stop
#define OPTO_getposn    OPTO_GetPosn
#define OPTO_rotposn    OPTO_RTPosn
#define OPTO_RotPosn    OPTO_RTPosn
#define OPTO_rawposn    OPTO_RawPosn
#define OPTO_seen       OPTO_Seen
#define OPTO_freq       OPTO_Freq

/******************************************************************************/

#include <OPTObuff.h>                  // OptoTrak buffering routines.
#include <OPTOdata.h>                  // OptoTrak data conversion routines.

/******************************************************************************/

