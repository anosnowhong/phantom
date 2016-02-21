/******************************************************************************/
/*                                                                            */ 
/* MODULE  : OPTORIGID.h                                                      */ 
/*                                                                            */ 
/* PURPOSE : Rigid Body functions for use with OptoTrak Motion Sensor System. */ 
/*                                                                            */ 
/* DATE    : 21/Jun/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V2.0  JNI 21/Jun/2000 - Re-developed from "mylib.lib" OPTO module.         */ 
/*                                                                            */ 
/* V2.1  JNI 18/Jan/2002 - Someone actually wanted to use it so had to get it */ 
/*                         working (only 2 years after initial coding).       */ 
/*                                                                            */ 
/* V2.3  JNI 11/Aug/2006 - Renamed from RIGID to OPTORIGID.                   */ 
/*                                                                            */ 
/******************************************************************************/

#ifndef OPTORIGID_H
#define OPTORIGID_H

/******************************************************************************/

#define OPTORIGID_DEBUG() if( !OPTORIGID_API_start(printf,printf,printf) ) { printf("Cannot start OPTORIGID API.\n"); exit(0); }

/******************************************************************************/

#define OPTORIGID_MARKERS            16              // Maximum number of markers.

/******************************************************************************/

typedef struct OPTORIGID_information                 // Rigid Body information structure.
{
    STRING    name;                              // Name of Rigid Body.

    int       m1;                                // First marker.
    int       m2;                                // Last marker.
    int       mc;                                // Marker count.

    matrix    rbmx;                              // Rigid body centred position MatriX.
    matrix    mdmx;                              // Marker Distance MatriX.
}
OPTORIGID;

/******************************************************************************/

int     OPTORIGID_messgf( const char *mask, ... );
int     OPTORIGID_errorf( const char *mask, ... );
int     OPTORIGID_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    OPTORIGID_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    OPTORIGID_API_stop( void );
BOOL    OPTORIGID_API_check( void );

/******************************************************************************/

void    OPTORIGID_Init( OPTORIGID &rigid );

BOOL    OPTORIGID_Make( OPTORIGID &rigid, int m1, int m2, char *name );

void    OPTORIGID_Cnfg( OPTORIGID &rigid );

BOOL    OPTORIGID_FileLoad( OPTORIGID &rigid, char *name );
BOOL    OPTORIGID_FileSave( OPTORIGID &rigid, char *name );
BOOL    OPTORIGID_FileSave( OPTORIGID &rigid );

BOOL    OPTORIGID_GetBody( OPTORIGID &rigid, int m1, int m2, char *name );
BOOL    OPTORIGID_GetBody( OPTORIGID &rigid );

BOOL    OPTORIGID_RTMX( OPTORIGID &rigid, matrix &rtmx );
BOOL    OPTORIGID_RTMX( OPTORIGID &rigid, matrix &rtmx, matrix &posn, matrix &seen );
BOOL    OPTORIGID_RTMX( OPTORIGID &rigid, matrix &rtmx, matrix posn[], BOOL seen[] );

/******************************************************************************/

#define OPTORIGID_Define(RB,M1,M2)    OPTORIGID_Make(RB,M1,M2,#RB)
#define OPTORIGID_Create(RB,M1,M2)    OPTORIGID_GetBody(RB,M1,M2,#RB)
#define OPTORIGID_Load(RB)            OPTORIGID_FileLoad(RB,#RB)
#define OPTORIGID_Save(RB)            OPTORIGID_FileSave(RB)

/******************************************************************************/

BOOL OPTSIM_Load( char *file );
UINT OPTSIM_GetPosn( matrix &P, matrix &S );

#endif

