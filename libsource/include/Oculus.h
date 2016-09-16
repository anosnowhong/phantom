/******************************************************************************/
/*                                                                            */
/* MODULE  : Oculus.h                                                         */
/*                                                                            */
/* PURPOSE : Oculus Rift HMD module.                                          */
/*                                                                            */
/* DATE    : 09/Jan/2015                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 09/Jan/2015 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 10/Sep/2015 - Re-visit to solve rotation / workspace issue.      */
/*                                                                            */
/******************************************************************************/

#ifndef OCULUS_H
#define OCULUS_H
#define OCULUS_DEBUG() if( !OCULUS_API_start(printf,printf,printf) ) { printf("Cannot start OCULUS API.\n"); exit(0); }

typedef OVR::Vector3f Vector3f;
typedef OVR::Matrix4f Matrix4f;
typedef OVR::Vector4f Vector4f;
/******************************************************************************/

extern  PRINTF  OCULUS_PRN_messgf;      // General messages printf function.
extern  PRINTF  OCULUS_PRN_errorf;      // Error messages printf function.
extern  PRINTF  OCULUS_PRN_debugf;      // Debug information printf function.

#define OCULUS_MinX OCULUS_DisplaySize[GRAPHICS_X][GRAPHICS_MIN]
#define OCULUS_MaxX OCULUS_DisplaySize[GRAPHICS_X][GRAPHICS_MAX]
#define OCULUS_MinY OCULUS_DisplaySize[GRAPHICS_Y][GRAPHICS_MIN]
#define OCULUS_MaxY OCULUS_DisplaySize[GRAPHICS_Y][GRAPHICS_MAX]
#define OCULUS_MinZ OCULUS_DisplaySize[GRAPHICS_Z][GRAPHICS_MIN]
#define OCULUS_MaxZ OCULUS_DisplaySize[GRAPHICS_Z][GRAPHICS_MAX]

#define OCULUS_CONFIG "OCULUS.CFG"

/******************************************************************************/
#include <Extras/OVR_Math.h>
#include <Kernel/OVR_Types.h>

int  OCULUS_messgf( const char *mask, ... );
int  OCULUS_errorf( const char *mask, ... );
int  OCULUS_debugf( const char *mask, ... );

BOOL OCULUS_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void OCULUS_API_stop( void );

BOOL OCULUS_Config( char *file );

BOOL OCULUS_Check( void );
BOOL OCULUS_Open( void );
void OCULUS_Close( void );
BOOL OCULUS_Opened( void );
BOOL OCULUS_Start( void );
void OCULUS_Stop( void );
void OCULUS_RenderTarget( int width, int height );

void OCULUS_GraphicsDisplay( void (*draw)( void ) );
void OCULUS_GraphicsDisplay( void );

void OCULUS_QuaternionToRotationMatrix( float *quat, float *mat );
void OCULUS_QuaternionToRotationMatrix( double *quat, double *mat );
UINT OCULUS_NextPower2( UINT x );

void OCULUS_GridBuild( float xmin, float xmax, float ymin, float ymax, float zpos, int grid, int ID );
void OCULUS_GridDraw( void );
void OCULUS_GridBuild( void );

void OCULUS_DisplayPosition( int x, int y );
void OCULUS_DisplayOnHMD( void );
void OCULUS_DisplayOnDeskTop( void );
void OCULUS_DisplayToggle( void );

BOOL OCULUS_GetPose(ovrPosef& pose);
BOOL OCULUS_GetViewProjection(Matrix4f& M, float &scale);

void OCULUS_TimingResults( void );

BOOL OCULUS_GlutKeyboard( BYTE key, int x, int y );

#endif

/******************************************************************************/

