/******************************************************************************/
/*                                                                            */
/* MODULE  : LIBERTY.cpp                                                      */
/*                                                                            */
/* PURPOSE : Polhemus Liberty motion tracking system.                         */
/*                                                                            */
/* DATE    : 06/Jul/2009                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 06/Jul/2009 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 08/Jan/2015 - 3BOT-inspired continued development and bug fixes. */
/*                                                                            */
/******************************************************************************/

#include <motor.h>

/******************************************************************************/

#define LIBERTY_SENSOR_MAX  16

/******************************************************************************/

void    LIBERTY_Error( char *str );

void    LIBERTY_ConfigSetup( void );
BOOL    LIBERTY_ConfigLoad( char *file );

BOOL    LIBERTY_Open( char *cnfg );
BOOL    LIBERTY_Open( ePiFrameRate FrameRateCode, matrix &HemisphereVector, BOOL SensorList[] );
void    LIBERTY_Close( void );

BOOL    LIBERTY_Started( void );

BOOL    LIBERTY_Start( void );
void    LIBERTY_Stop( void );

BOOL    LIBERTY_GetFrame( BOOL &ready, DWORD &frame, matrix posn[], matrix ornt[], int distlev[] );
BOOL    LIBERTY_GetFrame( BOOL &ready, DWORD &frame, matrix posn[], matrix ornt[] );
BOOL    LIBERTY_GetFrame( BOOL &ready, matrix posn[], matrix ornt[], int distlev[] );
BOOL    LIBERTY_GetFrame( BOOL &ready, matrix posn[], matrix ornt[] );

BOOL    LIBERTY_GetPosn( BOOL &ready, matrix posn[] );
BOOL    LIBERTY_GetPosn( matrix posn[] );

void    LIBERTY_TimingResults( void );

int     LIBERTY_Sensors( void );
BOOL   *LIBERTY_SensorFlagList( int s1, ... );

/******************************************************************************/

