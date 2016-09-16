/******************************************************************************/
/*                                                                            */ 
/* MODULE  : RIGID.h                                                          */ 
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
/******************************************************************************/

#ifndef RIGID_H
#define RIGID_H

/******************************************************************************/

#define RIGID_DEBUG() if( !RIGID_API_start(printf,printf,printf) ) { printf("Cannot start RIGID API.\n"); exit(0); }

/******************************************************************************/

#define RIGID_MIN_SEEN            3              // Minimum markers that must be seen.
#define RIGID_MARKERS             8              // Maximum number of markers.
#define RIGID_RTMX_TOLERANCE    0.4              // Error tolerance for R/T matrix.

/******************************************************************************/

typedef struct RIGID_information                 // Rigid Body information structure.
{
    STRING    name;                              // Name of Rigid Body.

    int       m1;                                // First marker.
    int       m2;                                // Last marker.
    int       mc;                                // Marker count.

    matrix    rbmx;                              // Rigid body centred position MatriX.
    matrix    mdmx;                              // Marker Distance MatriX.
}
RIGID;

/******************************************************************************/

int     RIGID_messgf( const char *mask, ... );
int     RIGID_errorf( const char *mask, ... );
int     RIGID_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    RIGID_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    RIGID_API_stop( void );
BOOL    RIGID_API_check( void );

/******************************************************************************/

void    RIGID_Init( RIGID &rigid );

BOOL    RIGID_Make( RIGID &rigid, int m1, int m2, char *name );

void    RIGID_Cnfg( RIGID &rigid );

BOOL    RIGID_FileLoad( RIGID &rigid, char *name );
BOOL    RIGID_FileSave( RIGID &rigid, char *name );
BOOL    RIGID_FileSave( RIGID &rigid );

BOOL    RIGID_GetBody( RIGID &rigid, int m1, int m2, char *name );
BOOL    RIGID_GetBody( RIGID &rigid );

BOOL    RIGID_RTMX( RIGID &rigid, matrix &rtmx );

/******************************************************************************/

#define RIGID_Define(RB,M1,M2)    RIGID_Make(RB,M1,M2,#RB)
#define RIGID_Create(RB,M1,M2)    RIGID_GetBody(RB,M1,M2,#RB)
#define RIGID_Load(RB)            RIGID_FileLoad(RB,#RB)
#define RIGID_Save(RB)            RIGID_FileSave(RB)

/******************************************************************************/

#endif

