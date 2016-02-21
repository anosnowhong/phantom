/******************************************************************************/
/*                                                                            */
/* MODULE  : Graphics.h                                                       */
/*                                                                            */
/* PURPOSE : OpenGL / GLUT Graphics module.                                   */
/*                                                                            */
/* DATE    : 15/Feb/2001                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V2.0  JNI 16/Jul/2003 - Re-developed from old MyLib module.                */
/*                                                                            */
/* V2.1  JNI 10/Sep/2008 - Added display rotation to GRAPHICS.CFG file.       */
/*                                                                            */
/* V2.2  JNI 27/Apr/2010 - Added lighting parameters to GRAPHICS.CFG file.    */
/*                                                                            */
/* V2.3  JNI 09/Jan/2015 - Support for Oculus Rift HMD.                       */
/*                                                                            */
/******************************************************************************/

#include "graphics-old.h"

/******************************************************************************/

enum { X, Y, Z, W };                   // Elements of a vertex.

/******************************************************************************/

#define GL_ROTATE_X     1.0,0.0,0.0    // OpenGL rotation axis...
#define GL_ROTATE_Y     0.0,1.0,0.0
#define GL_ROTATE_Z     0.0,0.0,1.0

/******************************************************************************/

#define GRAPHICS_errorf printf
#define GRAPHICS_debugf printf
#define GRAPHICS_messgf printf

/******************************************************************************/

#define OPENGL_VECTOR   16
#define OPENGL_MATRIX   4,4

/******************************************************************************/

#define GRAPHICS_RANGE  2
#define GRAPHICS_MIN    0
#define GRAPHICS_MAX    1

#define GRAPHICS_X      0
#define GRAPHICS_Y      1
#define GRAPHICS_Z      2

#define GRAPHICS_2D     2
#define GRAPHICS_3D     3
#define GRAPHICS_RGB    3

/******************************************************************************/

#define GRAPHICS_WINDOW_INVALID -1

/******************************************************************************/

#define GRAPHICS_CONFIG           "GRAPHICS.CFG"
#define GRAPHICS_CALIBRATION      "GRAPHICS_%dD.CAL"

/******************************************************************************/

extern  STRING  GRAPHICS_Description;
extern  int     GRAPHICS_StereoMode;
extern  float   GRAPHICS_DisplaySize[GRAPHICS_3D][GRAPHICS_RANGE];
extern  float   GRAPHICS_FocalPlane;
extern  float   GRAPHICS_PupilToCentre;
extern  float   GRAPHICS_EyeCentre[GRAPHICS_3D];
extern  matrix  GRAPHICS_EyePOMX;
extern  STRING  GRAPHICS_CalibPath;
extern  STRING  GRAPHICS_CalibName;
extern  BOOL    GRAPHICS_SpaceBall;
extern  float   GRAPHICS_DisplayRotation[GRAPHICS_3D];
extern  int     GRAPHICS_SphereSegments;

/******************************************************************************/

extern  struct  STR_TextItem  GRAPHICS_DisplayText[];
extern  struct  STR_TextItem  GRAPHICS_StereoText[];

/******************************************************************************/

extern  HWND    GRAPHICS_WindowParent;
extern  HWND    GRAPHICS_WindowGLUT;

extern  int     GRAPHICS_Display;
extern  int     GRAPHICS_Dimensions;

extern  int     GRAPHICS_DisplayPixels[GRAPHICS_2D];
extern  int     GRAPHICS_DisplayHeight;
extern  int     GRAPHICS_DisplayFrequency;

extern  double  GRAPHICS_VerticalRetraceFrequency;
extern  double  GRAPHICS_VerticalRetracePeriod;
extern  long    GRAPHICS_SwapBuffersCount;
extern  double  GRAPHICS_SwapBuffersToVerticalRetraceTime;
extern  double  GRAPHICS_SwapBuffersLastOnsetTime;
extern  double  GRAPHICS_SwapBuffersLastOffsetTime;
extern  double  GRAPHICS_VerticalRetraceNextOnsetTime;
extern  double  GRAPHICS_VerticalRetraceNextOffsetTime;

/******************************************************************************/

// Screen dimensions...
#define SL      GRAPHICS_DisplaySize[GRAPHICS_X][GRAPHICS_MIN]
#define SR      GRAPHICS_DisplaySize[GRAPHICS_X][GRAPHICS_MAX]
#define ST      GRAPHICS_DisplaySize[GRAPHICS_Y][GRAPHICS_MAX]
#define SB      GRAPHICS_DisplaySize[GRAPHICS_Y][GRAPHICS_MIN]
#define SZ      GRAPHICS_FocalPlane 

#define ZNEAR   GRAPHICS_DisplaySize[GRAPHICS_Z][GRAPHICS_MIN]
#define ZFAR    GRAPHICS_DisplaySize[GRAPHICS_Z][GRAPHICS_MAX]

// Eye position...
#define EX      GRAPHICS_EyeCentre[GRAPHICS_X]
#define EY      GRAPHICS_EyeCentre[GRAPHICS_Y]
#define EZ      GRAPHICS_EyeCentre[GRAPHICS_Z]
#define ESEP    GRAPHICS_PupilToCentre

// Screen resolution (pixels)...
#define HPIX    GRAPHICS_DisplayPixels[GRAPHICS_X]
#define VPIX    GRAPHICS_DisplayPixels[GRAPHICS_Y]

// Workspace dimensions...
#define WL      GRAPHICS_CalibRange[GRAPHICS_X][GRAPHICS_MIN]
#define WR      GRAPHICS_CalibRange[GRAPHICS_X][GRAPHICS_MAX]
#define WT      GRAPHICS_CalibRange[GRAPHICS_Y][GRAPHICS_MAX]
#define WB      GRAPHICS_CalibRange[GRAPHICS_Y][GRAPHICS_MIN]
#define WN      GRAPHICS_CalibRange[GRAPHICS_Z][GRAPHICS_MIN]
#define WF      GRAPHICS_CalibRange[GRAPHICS_Z][GRAPHICS_MIN]

/******************************************************************************/

#define GRAPHICS_SHOWMOUSE     0x10    // Show mouse cursor in GLUT window.
#define GRAPHICS_DONTFOCUS     0x20    // Don't mess with window focus.
#define GRAPHICS_DISPLAY       0x0F    // Mask for display bits (see below)...

#define GRAPHICS_DISPLAY_DEFAULT 0x0F  // Default (user specified).
#define GRAPHICS_DISPLAY_MONO    0x00  // Regular 3D
#define GRAPHICS_DISPLAY_STEREO  0x01  // Stereoscopic 3D
#define GRAPHICS_DISPLAY_2D      0x02  // 2D

/******************************************************************************/

#define GRAPHICS_STEREO_NONE           0
#define GRAPHICS_STEREO_FRAMEALTERNATE 1
#define GRAPHICS_STEREO_DUALSCREEN     2
#define GRAPHICS_STEREO_OCULUS         3

/******************************************************************************/

#define EYE_MAX    3
#define EYE_LEFT   0
#define EYE_RIGHT  1
#define EYE_MONO   2

/******************************************************************************/

#define GRAPHICS_EyeLoop(E)  for( int E=GRAPHICS_EyeLeft(); (E <= GRAPHICS_EyeRight()); E++ )

extern  int  GRAPHICS_BufferEye[EYE_MAX];

int     GRAPHICS_EyeLeft( void );
int     GRAPHICS_EyeRight( void );

/******************************************************************************/

BOOL    GRAPHICS_StereoModeParse( int &code, char *text );
BOOL    GRAPHICS_StereoModeOculus( void );

BOOL    GRAPHICS_DisplayModeParse( int &mode, char *text );
#define GRAPHICS_DisplayMode GRAPHICS_DisplayModeParse
BOOL    GRAPHICS_DisplayMono( void );
BOOL    GRAPHICS_DisplayStereo( void );
BOOL    GRAPHICS_Display3D( void );
BOOL    GRAPHICS_Display2D( void );
int     GRAPHICS_Dimension( void );

/******************************************************************************/

void    GRAPHICS_ConfigVariables( void );

BOOL    GRAPHICS_Cnfg( char *file );

BOOL    GRAPHICS_Init( char *cnfg );
BOOL    GRAPHICS_Init( void );

/******************************************************************************/

extern  float   GRAPHICS_CalibRange[GRAPHICS_3D][GRAPHICS_RANGE];
extern  float   GRAPHICS_CalibCentre[GRAPHICS_3D];
extern  float   GRAPHICS_CalibPupilToCentre;
extern  float   GRAPHICS_CalibValidationError;
extern  float   GRAPHICS_CalibMatrix[OPENGL_VECTOR];
extern  matrix  GRAPHICS_CalibCentrePOMX;

/******************************************************************************/

void    GRAPHICS_CalibCnfg( void );
BOOL    GRAPHICS_CalibCnfg( int func, char *path );

BOOL    GRAPHICS_CalibSave( char *path );
BOOL    GRAPHICS_CalibLoad( char *path );

char   *GRAPHICS_CalibFile( char *name, int dimensions );
char   *GRAPHICS_CalibFile( int dimensions );
char   *GRAPHICS_CalibFile( char *name );
char   *GRAPHICS_CalibFile( void );

BOOL    GRAPHICS_CalibUse( char *name, int dimensions );
BOOL    GRAPHICS_CalibUse( int dimensions );
BOOL    GRAPHICS_CalibUse( char *name );
BOOL    GRAPHICS_CalibUse( void );

/******************************************************************************/

//      Frustum matrix (linear RTMX) for LEFT, RIGHT and MONO...
extern  double  GRAPHICS_Frustum[EYE_MAX][SPMX_RTMX_VEC];

extern  double  GRAPHICS_FrustumLeft[EYE_MAX];
extern  double  GRAPHICS_FrustumRight[EYE_MAX];
extern  double  GRAPHICS_FrustumTop[EYE_MAX];
extern  double  GRAPHICS_FrustumBottom[EYE_MAX];

/******************************************************************************/

void    GRAPHICS_FocusWindow( HWND window );
void    GRAPHICS_FocusParent( void );
void    GRAPHICS_FocusGLUT( void );

BOOL    GRAPHICS_Initialized( void );
BOOL    GRAPHICS_Started( void );

BOOL    GRAPHICS_Start( char *cnfg, int mode, int wid, int hgt, char *name, int &ID );
BOOL    GRAPHICS_Start( char *cnfg, int mode, int wid, int hgt, char *name );
BOOL    GRAPHICS_Start( char *cnfg, int mode, char *name, int &ID );
BOOL    GRAPHICS_Start( char *cnfg, int mode, char *name );
BOOL    GRAPHICS_Start( char *cnfg, int mode );

BOOL    GRAPHICS_Start( int mode, int wid, int hgt, char *name, int &ID );
BOOL    GRAPHICS_Start( int mode, int wid, int hgt, char *name );
BOOL    GRAPHICS_Start( int mode, char *name, int &ID );
BOOL    GRAPHICS_Start( int mode, char *name );
BOOL    GRAPHICS_Start( int mode );

void    GRAPHICS_Stop( void );

void    GRAPHICS_Reshape( int w, int h );

void    GRAPHICS_BufferSet( int eye );

// Calibrate graphics screen for the appropriate eye using an option external calibration matrix...
void    GRAPHICS_View( BOOL calib, BOOL centre, float x, float y, float z, int eye );
void    GRAPHICS_View( BOOL calib, BOOL centre, float x, float y, int eye );
void    GRAPHICS_View( BOOL calib, BOOL centre, matrix &xyz, int eye );
void    GRAPHICS_View( BOOL calib, BOOL centre, int eye );

void    GRAPHICS_View( BOOL calib, float x, float y, float z, int eye );
void    GRAPHICS_View( BOOL calib, float x, float y, int eye );
void    GRAPHICS_View( BOOL calib, matrix &xyz, int eye );
void    GRAPHICS_View( BOOL calib, int eye );

void    GRAPHICS_View( BOOL calib, float x, float y, float z );
void    GRAPHICS_View( BOOL calib, float x, float y );
void    GRAPHICS_View( BOOL calib, matrix &xyz );
void    GRAPHICS_View( BOOL calib );

// Calibrate graphics screen without using an external calibration matrix...
void    GRAPHICS_ViewNoCalib( float x, float y, float z, int eye );
void    GRAPHICS_ViewNoCalib( float x, float y, int eye );
void    GRAPHICS_ViewNoCalib( matrix &xyz, int eye );
void    GRAPHICS_ViewNoCalib( int eye );

void    GRAPHICS_ViewNoCalib( float x, float y, float z );
void    GRAPHICS_ViewNoCalib( float x, float y );
void    GRAPHICS_ViewNoCalib( matrix &xyz );
void    GRAPHICS_ViewNoCalib( void );

// Calibrate graphics screen using an external calibration matrix...
void    GRAPHICS_ViewCalib( float x, float y, float z, int eye );
void    GRAPHICS_ViewCalib( float x, float y, int eye );
void    GRAPHICS_ViewCalib( matrix &xyz, int eye );
void    GRAPHICS_ViewCalib( int eye );

void    GRAPHICS_ViewCalib( float x, float y, float z );
void    GRAPHICS_ViewCalib( float x, float y );
void    GRAPHICS_ViewCalib( matrix &xyz );
void    GRAPHICS_ViewCalib( void );

// As above; translate to the centre of the calibrated space...
void    GRAPHICS_ViewNoCalibCentre( float x, float y, float z, int eye );
void    GRAPHICS_ViewNoCalibCentre( float x, float y, int eye );
void    GRAPHICS_ViewNoCalibCentre( matrix &xyz, int eye );
void    GRAPHICS_ViewNoCalibCentre( int eye );

void    GRAPHICS_ViewNoCalibCentre( float x, float y, float z );
void    GRAPHICS_ViewNoCalibCentre( float x, float y );
void    GRAPHICS_ViewNoCalibCentre( matrix &xyz );
void    GRAPHICS_ViewNoCalibCentre( void );

void    GRAPHICS_ViewCalibCentre( float x, float y, float z, int eye );
void    GRAPHICS_ViewCalibCentre( float x, float y, int eye );
void    GRAPHICS_ViewCalibCentre( matrix &xyz, int eye );
void    GRAPHICS_ViewCalibCentre( int eye );

void    GRAPHICS_ViewCalibCentre( float x, float y, float z );
void    GRAPHICS_ViewCalibCentre( float x, float y );
void    GRAPHICS_ViewCalibCentre( matrix &xyz );
void    GRAPHICS_ViewCalibCentre( void );

void    GRAPHICS_FrustumSet( int eye, BOOL orthographic, float scale, float eyex );

void    GRAPHICS_FrustumInit( BOOL orthographic );
void    GRAPHICS_FrustumInit( void );

// Get screen dimensions...
BOOL    GRAPHICS_Screen( int p[], double c[] );
BOOL    GRAPHICS_Screen( int &px, int &py, double &cx, double &cy );
BOOL    GRAPHICS_Screen( double &L, double &R, double &T, double &B, double &W, double &H );
BOOL    GRAPHICS_Screen( double &L, double &R, double &T, double &B );
BOOL    GRAPHICS_Screen( double &W, double &H );

BOOL    GRAPHICS_WorkSpace( double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax );
BOOL    GRAPHICS_WorkSpace( double &xmin, double &xmax, double &ymin, double &ymax );
BOOL    GRAPHICS_WorkSpace( double &xlen, double &ylen );

// Translate from eye centre to work-space centre...
void    GRAPHICS_TranslateCentre( matrix &position );

void    GRAPHICS_Translate( matrix &position );
void    GRAPHICS_Rotate( matrix &rotation );

/******************************************************************************/

// Color table { red,green,blue,alpha,pname } values
#define GRAPHICS_COLOR_LIGHTBLUE   0.30,0.30,0.70,1.00,GL_AMBIENT_AND_DIFFUSE
#define GRAPHICS_COLOR_DARKBLUE    0.15,0.15,0.35,1.00,GL_AMBIENT_AND_DIFFUSE
#define GRAPHICS_COLOR_BLACK       0.00,0.00,0.00,1.00,GL_AMBIENT
#define GRAPHICS_COLOR_LIGHTBLACK  0.10,0.10,0.10,1.00,GL_AMBIENT
#define GRAPHICS_COLOR_RED         1.00,0.00,0.00,1.00,GL_AMBIENT_AND_DIFFUSE
#define GRAPHICS_COLOR_YELLOW      1.00,0.90,0.00,1.00,GL_AMBIENT_AND_DIFFUSE
#define GRAPHICS_COLOR_BLUE        0.00,0.00,1.00,1.00,GL_AMBIENT_AND_DIFFUSE
#define GRAPHICS_COLOR_GREEN       0.00,0.80,0.00,1.00,GL_AMBIENT_AND_DIFFUSE
#define GRAPHICS_COLOR_WHITE       1.00,1.00,1.00,1.00,GL_AMBIENT_AND_DIFFUSE
#define GRAPHICS_COLOR_GREY        0.50,0.50,0.50,1.00,GL_AMBIENT
#define GRAPHICS_COLOR_DARKGREY    0.25,0.25,0.25,1.00,GL_AMBIENT
#define GRAPHICS_COLOR_LIGHTGREY   0.75,0.75,0.75,1.00,GL_AMBIENT
#define GRAPHICS_COLOR_TURQUOISE   0.30,1.00,1.00,1.00,GL_AMBIENT
#define GRAPHICS_COLOR_PURPLE      1.00,0.00,1.00,1.00,GL_AMBIENT
#define GRAPHICS_COLOR_UNDEFINED  -1.0,-1.0,-1.0,-1.0,-1.0

// Color table row numbers.
#define NOCOLOR    -1
#define RED         0
#define YELLOW      1
#define BLUE        2
#define WHITE       3
#define GREEN       4
#define TURQUOISE   5
#define PURPLE      6
#define BLACK       7
#define LIGHTBLUE   8
#define DARKBLUE    9
#define GREY       10
#define DARKGREY   11
#define LIGHTBLACK 12
#define LIGHTGREY  13

#define GRAPHICS_COLOR_ROWS       32
#define GRAPHICS_COLOR_COLUMNS    5
#define GRAPHICS_COLOR_VALUES     GRAPHICS_COLOR_COLUMNS
#define GRAPHICS_COLOR_RGB        3
#define GRAPHICS_COLOR_R          0
#define GRAPHICS_COLOR_G          1
#define GRAPHICS_COLOR_B          2
#define GRAPHICS_COLOR_ALPHA      3
#define GRAPHICS_COLOR_PNAME      4

extern float GRAPHICS_ColorTable[GRAPHICS_COLOR_ROWS][GRAPHICS_COLOR_COLUMNS];

/******************************************************************************/

// Get integer color-code for string color name...
BOOL GRAPHICS_ColorCode( int &code, char *text );
int  GRAPHICS_ColorCode( char *text );

BOOL GRAPHICS_ColorValid( float entry[] );
BOOL GRAPHICS_ColorValid( int code );
void GRAPHICS_ColorEntry( float entry[], float r, float b, float g, float alpha, int pname );
void GRAPHICS_ColorUndefine( float entry[] );
int  GRAPHICS_ColorDefine( float entry[] );
int  GRAPHICS_ColorDefine( float r, float b, float g, float alpha, int pname );
int  GRAPHICS_ColorDefine( float r, float b, float g, int pname );

// Set material parameters for particular colors...
void GRAPHICS_ColorSet( int code, float alpha );
void GRAPHICS_ColorSet( int code );
void GRAPHICS_ColorSet( char *text, float alpha );
void GRAPHICS_ColorSet( char *text );

int  GRAPHICS_ColorGet( void );

char *GRAPHICS_ColorName( int code );

/******************************************************************************/

// Set background ("clear") color...
void GRAPHICS_ClearColor( int color );
void GRAPHICS_ClearColor( void );
void GRAPHICS_ClearColorPush( void );
void GRAPHICS_ClearColorPop( void );

// Regular clear functions...
void GRAPHICS_Clear( GLbitfield mask );
void GRAPHICS_Clear( void );

// Stereo depth buffer clear functions...
void GRAPHICS_ClearStereo( void );
void GRAPHICS_ClearMono( void );

// Clear FRONT and BACK buffers...
void GRAPHICS_ClearBuffers( void );

/******************************************************************************/

void GRAPHICS_SwapBuffers( void );

/******************************************************************************/

#define GRAPHICS_CAPABILITY_INDEX   45
#define GRAPHICS_CAPABILITY_STACK   16
#define GRAPHICS_CAPABILITY_INVALID -1

void GRAPHICS_Capability( int capability, BOOL flag );
int  GRAPHICS_CapabilityIndex( int capability );
void GRAPHICS_CapabilityPush( int capability, BOOL flag );
void GRAPHICS_CapabilityPop( int capability );
void GRAPHICS_CapabilityEnable( int capability );
void GRAPHICS_CapabilityDisable( int capability );

/******************************************************************************/

#define GRAPHICS_FLAG_NONE         0x00
#define GRAPHICS_FLAG_LIGHTING     0x01
#define GRAPHICS_FLAG_TRANSPARENCY 0x02
#define GRAPHICS_FLAG_ANTIALIASING 0x04

// Do the standard OpenGL set-up stuff...
void GRAPHICS_OpenGL( BYTE flag, int clearcolor );
void GRAPHICS_OpenGL( BYTE flag );
void GRAPHICS_OpenGL( void );

void GRAPHICS_StandardOpenGL( BYTE flag, int clearcolor );

// Do the standard GLUT initalization...
void GRAPHICS_GlutInit( int glut, int wid, int hgt );

/******************************************************************************/

BOOL GRAPHICS_DisplayConfigureDefault( void );
BOOL GRAPHICS_DisplayConfigure( BOOL flag );

/******************************************************************************/

#define DISPLAY_MODES 512

struct  DISPLAY_ModeItem
{
    int ColorBits;
    int Width;
    int Height;
    int Frequency;
};

/******************************************************************************/

#define DISPLAY_errorf GRAPHICS_errorf
#define DISPLAY_debugf GRAPHICS_debugf
#define DISPLAY_messgf GRAPHICS_messgf

/******************************************************************************/

BOOL DISPLAY_ModeGet( int item, DEVMODE *mode );
BOOL DISPLAY_ModeGet( DEVMODE *mode );

BOOL DISPLAY_ModeList( void );

BOOL DISPLAY_ModeSupported( int width, int height, int frequency );

BOOL DISPLAY_ModeGet( int &width, int &height, int &frequency );
BOOL DISPLAY_ModeGet( int &width, int &height );
BOOL DISPLAY_ModeSet( int width, int height, int frequency );

BOOL DISPLAY_Frequency( int &frequency );

BOOL DISPLAY_ModePush( int width, int height, int frequency );
BOOL DISPLAY_ModePush( int width, int height );
void DISPLAY_ModePop( void );

/******************************************************************************/

void GRAPHICS_Sphere( matrix *posn, float radius, int color, float alpha );
void GRAPHICS_Sphere( matrix *posn, float radius, int color );
void GRAPHICS_Sphere( matrix *posn, float radius );

void GRAPHICS_WireSphere( matrix *posn, float radius, int color, float alpha, int segments );
void GRAPHICS_WireSphere( matrix *posn, float radius, int color, float alpha );
void GRAPHICS_WireSphere( matrix *posn, float radius, int color );
void GRAPHICS_WireSphere( matrix *posn, float radius );

void GRAPHICS_Cuboid( matrix *posn, float xlength, float ywidth, float zheight, int color, float alpha );
void GRAPHICS_Cuboid( matrix *posn, float xlength, float ywidth, float zheight, int color );
void GRAPHICS_Cuboid( matrix *posn, float xlength, float ywidth, float zheight );

void GRAPHICS_Cube( matrix *posn, float length, int color, float alpha );
void GRAPHICS_Cube( matrix *posn, float length, int color );
void GRAPHICS_Cube( matrix *posn, float length );

void GRAPHICS_Circle( matrix *posn, float radius, float width, int color, float alpha );

void GRAPHICS_Circle( matrix *posn, float radius, int color, float alpha );
void GRAPHICS_Circle( matrix *posn, float radius, int color );
void GRAPHICS_Circle( matrix *posn, float radius );

void GRAPHICS_Ring( matrix *posn, float radius, float width, int color, float alpha );
void GRAPHICS_Ring( matrix *posn, float radius, float width, int color );
void GRAPHICS_Ring( matrix *posn, float radius, float width );

void GRAPHICS_Sector( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha );

void GRAPHICS_Sector( matrix *posn, float radius, float arcrad, float rotrad, int color, float alpha );
void GRAPHICS_Sector( matrix *posn, float radius, float arcrad, float rotrad, int color );
void GRAPHICS_Sector( matrix *posn, float radius, float arcrad, float rotrad );

void GRAPHICS_Arc( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha );
void GRAPHICS_Arc( matrix *posn, float radius, float arcrad, float rotrad, float width, int color );
void GRAPHICS_Arc( matrix *posn, float radius, float arcrad, float rotrad, float width );

void GRAPHICS_ArcAt( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha );
void GRAPHICS_ArcAt( matrix *posn, float radius, float arcrad, float rotrad, float width, int color );
void GRAPHICS_ArcAt( matrix *posn, float radius, float arcrad, float rotrad, float width );

void GRAPHICS_ArcPolygon( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha );
void GRAPHICS_ArcPolygon( matrix *posn, float radius, float arcrad, float rotrad, float width, int color );
void GRAPHICS_ArcPolygon( matrix *posn, float radius, float arcrad, float rotrad, float width );

void GRAPHICS_ArcPolygonAt( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha );
void GRAPHICS_ArcPolygonAt( matrix *posn, float radius, float arcrad, float rotrad, float width, int color );
void GRAPHICS_ArcPolygonAt( matrix *posn, float radius, float arcrad, float rotrad, float width );

void GRAPHICS_Rectangle( matrix *posn, float xwidth, float yheight, int color, float alpha );
void GRAPHICS_Rectangle( matrix *posn, float xwidth, float yheight, int color );
void GRAPHICS_Rectangle( matrix *posn, float xwidth, float yheight );

void GRAPHICS_Square( matrix *posn, float length, int color, float alpha );
void GRAPHICS_Square( matrix *posn, float length, int color );
void GRAPHICS_Square( matrix *posn, float length );

void GRAPHICS_Triangle( matrix *posn, double Zangle, double sides, double base, int color, float alpha );
void GRAPHICS_Triangle( matrix *posn, double Zangle, double sides, double base, int color );
void GRAPHICS_Triangle( matrix *posn, double Zangle, double sides, double base );

void GRAPHICS_Line( matrix &s, matrix &e, double width, int color );
void GRAPHICS_Line( matrix &s, matrix &e, double width );

void GRAPHICS_FixationCross( matrix &posn, float length, float width, int color );

void GRAPHICS_MassOnAStick( matrix &HandlePosition, double HandleAngle, int ObjectColor, double HandleRadius, double RodLength, double RodWidth, int HeadSize );

/******************************************************************************/

void GRAPHICS_WindowDestroy( void );

/******************************************************************************/

double GRAPHICS_VerticalRetraceOnsetTimeSinceLast( void );
double GRAPHICS_VerticalRetraceOffsetTimeSinceLast( void );

double GRAPHICS_VerticalRetraceOnsetTimeUntilNext( void );
double GRAPHICS_VerticalRetraceOffsetTimeUntilNext( void );

void   GRAPHICS_VerticalRetraceCatchOnset( void );
void   GRAPHICS_VerticalRetraceCatchOffset( void );
void   GRAPHICS_VerticalRetraceCatch( double wait_msec );
void   GRAPHICS_VerticalRetraceCatch( void );

void   GRAPHICS_VerticalRetraceResults( void );

double GRAPHICS_DisplayDelayTarget( matrix &posn );

/******************************************************************************/

void GRAPHICS_FrameData( void );
void GRAPHICS_FrameData( MATDAT *data );

/******************************************************************************/

#define GRAPHICS_DISPLAY_INITIALIZE  0
#define GRAPHICS_DISPLAY_PRE_VR      1
#define GRAPHICS_DISPLAY_WAIT_VR     2
#define GRAPHICS_DISPLAY_POST_VR     3
 
void GRAPHICS_DisplayStateNext( int state );
void GRAPHICS_DisplayStateProcess( double SecondsPreVR, double SecondsCatchVR, void (*DisplayPreVR)( void ), void (*DisplayPostVR)( void ) );
void GRAPHICS_DisplayStateProcess( double SecondsPreVR, double SecondsCatchVR, void (*DisplayPreVR)( void ) );

/******************************************************************************/

void GRAPHICS_SetWindow( void );

BOOL GRAPHICS_GraphicsStart( int mode, BYTE flag, int clearcolor );
BOOL GRAPHICS_GraphicsStart( int mode, int clearcolor );
BOOL GRAPHICS_GraphicsStart( int mode );
BOOL GRAPHICS_GraphicsStart( void );

BOOL GRAPHICS_OculusStart( char *name );
void GRAPHICS_OculusStop( void );

void GRAPHICS_OculusOpenGL( int clearcolor  );

BOOL GRAPHICS_Lighting( void );
void GRAPHICS_LightingDisable( void );
void GRAPHICS_LightingEnable( void );

void GRAPHICS_GraphicsDisplay( void (*draw)( void ) );
void GRAPHICS_GraphicsDisplay( void );

void GRAPHICS_GlutKeyboard( BYTE key, int x, int y );
void GRAPHICS_GlutDisplay( void );
void GRAPHICS_GlutIdle( void );

void GRAPHICS_MainLoop( void (*KeyboardFunction)( BYTE key, int x, int y ), void (*DrawFunction)( void ), void (*IdleFunction)( void ) );

void GRAPHICS_TimingResults( void );

void GRAPHICS_SwapBufferBefore( void );
void GRAPHICS_SwapBufferAfter( void );
void GRAPHICS_AdaptiveDisplayUpdate( double SwapBufferLatency );
BOOL GRAPHICS_AdaptiveDisplayReady( void );
void GRAPHICS_AdaptiveDisplayResults( BOOL saveflag );
void GRAPHICS_AdaptiveDisplayResults( void );

/******************************************************************************/

#define GRAPHICS_GetError(S) GRAPHICS_glGetError(S,__FILE__,__LINE__)
void    GRAPHICS_glGetError( char *text, char *file, int line );

/******************************************************************************/

BOOL GRAPHICS_MonitorConfig( char *config );
BOOL GRAPHICS_MonitorStarted( void );
void GRAPHICS_MonitorTimingResults( void );
void GRAPHICS_MonitorSwapBuffers( void );
void GRAPHICS_MonitorDraw( void );
void GRAPHICS_MonitorDisplay( void );
void GRAPHICS_MonitorReshape( int wid, int hgt );
BOOL GRAPHICS_MonitorStart( int xpos, int ypos, int wid, int views, double freq );
BOOL GRAPHICS_MonitorStart( int xpos, int ypos, int wid, double freq );
BOOL GRAPHICS_MonitorStart( char *config );
BOOL GRAPHICS_MonitorStart( void );
void GRAPHICS_MonitorIdle( void );
void GRAPHICS_MonitorKeyboard( BYTE key, int x, int y );
void GRAPHICS_MonitorWindowDestroy( void );
void GRAPHICS_MonitorSetWindow( void );
void GRAPHICS_MonitorSize( int wid, int hgt );
void GRAPHICS_MonitorView( void );

/******************************************************************************/

struct GRAPHICS_TextItem
{
    BOOL     inuse;
    void    *font;
    matrix   position;
    matrix   rotation;
    int      color;
    float    size;
    float    width;
    STRING   text;
};

#define GRAPHICS_TEXT_LIST    10
#define GRAPHICS_TEXT_INVALID -1

void GRAPHICS_TextInit( void );
int  GRAPHICS_TextFind( char *text );
BOOL GRAPHICS_TextValid( int item );
BOOL GRAPHICS_TextInUse( void );

void GRAPHICS_TextClear( int item );
void GRAPHICS_TextClear( char *text );
void GRAPHICS_TextClear( void );

void GRAPHICS_TextPosition( matrix *position );
void GRAPHICS_TextRotation( matrix *rotation );
void GRAPHICS_TextColor( int color );
void GRAPHICS_TextSize( float size );
void GRAPHICS_TextWidth( float width );

void GRAPHICS_TextDefault( matrix *position, matrix *rotation, int color, float size, float width );
void GRAPHICS_TextDefault( matrix *position, int color, float size, float width );
void GRAPHICS_TextDefault( matrix *position, matrix *rotation, int color, float size );
void GRAPHICS_TextDefault( matrix *position, int color, float size );
void GRAPHICS_TextDefault( matrix *position, matrix *rotation, float size );
void GRAPHICS_TextDefault( matrix *position, float size );
void GRAPHICS_TextDefault( matrix *position, matrix *rotation );
void GRAPHICS_TextDefault( matrix *position );

void GRAPHICS_TextSet( int item, matrix *position, matrix *rotation, int color, float size, float width, char *text );
void GRAPHICS_TextSet( int item, matrix *position, int color, float size, char *text );
void GRAPHICS_TextSet( int item, matrix *position, int color, char *text );
void GRAPHICS_TextSet( int item, int color, char *text );
void GRAPHICS_TextSet( int item, char *text );
void GRAPHICS_TextSet( char *text );

void GRAPHICS_TextDraw( int item );
void GRAPHICS_TextDraw( TIMER_Interval *latency );
void GRAPHICS_TextDraw( void );

/******************************************************************************/

