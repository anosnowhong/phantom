/******************************************************************************/
/*                                                                            */
/* MODULE  : SPMX.h                                                           */
/*                                                                            */
/* PURPOSE : SPatial MatriX API definitions and prototypes.                   */
/*                                                                            */
/* DATE    : 11/Nov/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 11/Nov/2000 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 16/May/2001 - Added linear size (vector) of matrix (SPMX_?_VEC). */
/*                                                                            */
/* V1.2  JNI 14/Jun/2001 - Added RTMX matrix functions (given X,Y,Z angle) &  */
/*                         functions to return Euler angles for RTMX.         */
/*                                                                            */
/* V1.3  JNI 27/Jun/2001 - Added (over-loaded) function to find RTMX without  */
/*                         translation (allow rotation only).                 */
/*                                                                            */
/******************************************************************************/

// Macros to convert from Degrees to Radians and vice versa...
#define D2R(D)     ((D) * (PI / 180.0))
#define R2D(R)     ((R) / (PI / 180.0))

/******************************************************************************/

// Usefull angles in radians...
extern  double  r15;
extern  double  r30;
extern  double  r45;
extern  double  r60;
extern  double  r90;
extern  double  r135;
extern  double  r180;
extern  double  r270;
extern  double  r360;

/******************************************************************************/

#define AXIS_XYZ   3
#define AXIS_XY    2

#define AXIS_X     0
#define AXIS_Y     1
#define AXIS_Z     2

/******************************************************************************/

#define SPMX_3D    3
#define SPMX_2D    2

/******************************************************************************/

// Map old DIM to new MTX (changed when SPMX_?_VEC was added)...
#define SPMX_POMX_DIM        SPMX_POMX_MTX
#define SPMX_PTMX_DIM        SPMX_PTMX_MTX
#define SPMX_EAMX_DIM        SPMX_EAMX_MTX
#define SPMX_PAMX_DIM        SPMX_PAMX_MTX
#define SPMX_ROMX_DIM        SPMX_ROMX_MTX
#define SPMX_RTMX_DIM        SPMX_RTMX_MTX
#define SPMX_QTMX_DIM        SPMX_QTMX_MTX
#define SPMX_EAMX_DIM        SPMX_EAMX_MTX

// POMX - Position Only MatriX...
#define SPMX_POMX_ROW        3
#define SPMX_POMX_COL        1
#define SPMX_POMX_MTX        SPMX_POMX_ROW,SPMX_POMX_COL
#define SPMX_POMX_VEC        SPMX_POMX_ROW*SPMX_POMX_COL
#define SPMX_POMX_X          1
#define SPMX_POMX_Y          2
#define SPMX_POMX_Z          3
#define SPMX_POMX_XC         SPMX_POMX_X,1
#define SPMX_POMX_YC         SPMX_POMX_Y,1
#define SPMX_POMX_ZC         SPMX_POMX_Z,1
#define SPMX_POMX_NULL       zeros(SPMX_POMX_MTX)

// PTMX - Position / Tranlsation MatriX...
#define SPMX_PTMX_ROW        4
#define SPMX_PTMX_COL        1
#define SPMX_PTMX_MTX        SPMX_PTMX_ROW,SPMX_PTMX_COL
#define SPMX_PTMX_VEC        SPMX_PTMX_ROW*SPMX_PTMX_COL
#define SPMX_PTMX_X          1
#define SPMX_PTMX_Y          2
#define SPMX_PTMX_Z          3
#define SPMX_PTMX_T          4
#define SPMX_PTMX_XC         SPMX_PTMX_X,1
#define SPMX_PTMX_YC         SPMX_PTMX_Y,1
#define SPMX_PTMX_ZC         SPMX_PTMX_Z,1
#define SPMX_PTMX_TC         SPMX_PTMX_T,1
#define SPMX_PTMX_NULL       SPMX_pomx2ptmx(SPMX_POMX_NULL)

// EAMX - Euler Angles MatriX...
#define SPMX_EAMX_ROW        3
#define SPMX_EAMX_COL        1
#define SPMX_EAMX_MTX        SPMX_EAMX_ROW,SPMX_EAMX_COL
#define SPMX_EAMX_VEC        SPMX_EAMX_ROW*SPMX_EAMX_COL
#define SPMX_EAMX_X          1
#define SPMX_EAMX_Y          2
#define SPMX_EAMX_Z          3
#define SPMX_EAMX_XC         SPMX_EAMX_X,1
#define SPMX_EAMX_YC         SPMX_EAMX_Y,1
#define SPMX_EAMX_ZC         SPMX_EAMX_Z,1

// PEMX - Position / Euler angles MatriX...
#define SPMX_PEMX_ROW        6
#define SPMX_PEMX_COL        1
#define SPMX_PEMX_MTX        SPMX_PEMX_ROW,SPMX_PEMX_COL
#define SPMX_PEMX_VEC        SPMX_PEMX_ROW*SPMX_PEMX_COL
#define SPMX_PEMX_PX         1
#define SPMX_PEMX_PY         2
#define SPMX_PEMX_PZ         3
#define SPMX_PEMX_EX         4
#define SPMX_PEMX_EY         5
#define SPMX_PEMX_EZ         6
#define SPMX_PEMX_PXC        SPMX_PEMX_PX,1
#define SPMX_PEMX_PYC        SPMX_PEMX_PY,1
#define SPMX_PEMX_PZC        SPMX_PEMX_PZ,1
#define SPMX_PEMX_EXC        SPMX_PEMX_EX,1
#define SPMX_PEMX_EYC        SPMX_PEMX_EY,1
#define SPMX_PEMX_EZC        SPMX_PEMX_EZ,1

// ROMX - Rotation Only MatriX...
#define SPMX_ROMX_ROW        3
#define SPMX_ROMX_COL        3
#define SPMX_ROMX_MTX        SPMX_ROMX_ROW,SPMX_ROMX_COL
#define SPMX_ROMX_VEC        SPMX_ROMX_ROW*SPMX_ROMX_COL
#define SPMX_ROMX_NULL       I(SPMX_ROMX_ROW)

// RTMX - Rotation / Translation MatriX...
#define SPMX_RTMX_ROW        4
#define SPMX_RTMX_COL        4
#define SPMX_RTMX_MTX        SPMX_RTMX_ROW,SPMX_RTMX_COL
#define SPMX_RTMX_VEC        SPMX_RTMX_ROW*SPMX_RTMX_COL
#define SPMX_RTMX_T          1,4
#define SPMX_RTMX_NULL       I(SPMX_RTMX_ROW)
#define SPMX_RTMX_X          1
#define SPMX_RTMX_Y          2
#define SPMX_RTMX_Z          3
#define SPMX_RTMX_XC         SPMX_RTMX_X,4
#define SPMX_RTMX_YC         SPMX_RTMX_Y,4
#define SPMX_RTMX_ZC         SPMX_RTMX_Z,4

// QTMX - Quaternion MatriX...
#define SPMX_QTMX_ROW        4
#define SPMX_QTMX_COL        1
#define SPMX_QTMX_MTX        SPMX_QTMX_ROW,SPMX_QTMX_COL
#define SPMX_QTMX_VEC        SPMX_QTMX_ROW*SPMX_QTMX_COL

// EAMX - Euler Angles MatriX...
#define SPMX_EAMX_ROW        3
#define SPMX_EAMX_COL        1
#define SPMX_EAMX_MTX        SPMX_EAMX_ROW,SPMX_EAMX_COL
#define SPMX_EAMX_VEC        SPMX_EAMX_ROW*SPMX_EAMX_COL
#define SPMX_EAMX_X          1
#define SPMX_EAMX_Y          2
#define SPMX_EAMX_Z          3
#define SPMX_EAMX_XC         SPMX_EAMX_X,1
#define SPMX_EAMX_YC         SPMX_EAMX_Y,1
#define SPMX_EAMX_ZC         SPMX_EAMX_Z,1

// RAMX - Rotation and Angle MatriX...
#define SPMX_RAMX_ROW        4
#define SPMX_RAMX_COL        1
#define SPMX_RAMX_MTX        SPMX_RAMX_ROW,SPMX_RAMX_COL
#define SPMX_RAMX_VEC        SPMX_RAMX_ROW*SPMX_RAMX_COL
#define SPMX_RAMX_A          4
#define SPMX_RAMX_X          1
#define SPMX_RAMX_Y          2
#define SPMX_RAMX_Z          3
#define SPMX_RAMX_AC         SPMX_RAMX_A,1
#define SPMX_RAMX_XC         SPMX_RAMX_X,1
#define SPMX_RAMX_YC         SPMX_RAMX_Y,1
#define SPMX_RAMX_ZC         SPMX_RAMX_Z,1

/******************************************************************************/

#define SPMX_DimPOMX(M) matrix_dim(M,SPMX_POMX_MTX);
#define SPMX_DimPTMX(M) matrix_dim(M,SPMX_PTMX_MTX);
#define SPMX_DimEAMX(M) matrix_dim(M,SPMX_EAMX_MTX);
#define SPMX_DimPAMX(M) matrix_dim(M,SPMX_PAMX_MTX);
#define SPMX_DimROMX(M) matrix_dim(M,SPMX_ROMX_MTX);
#define SPMX_DimRTMX(M) matrix_dim(M,SPMX_RTMX_MTX);
#define SPMX_DimQTMX(M) matrix_dim(M,SPMX_QTMX_MTX);
#define SPMX_DimRAMX(M) matrix_dim(M,SPMX_RAMX_MTX);

/******************************************************************************/

void    SPMX_mtx2xyz( matrix &mtx, double &x, double &y, double &z );
void    SPMX_mtx2xyz( matrix &mtx, double xyz[] );
void    SPMX_mtx2xyz( matrix &mtx, float &x, float &y, float &z );

void    SPMX_xyz2mtx( float x, float y, float z, matrix &mtx );
void    SPMX_xyz2mtx( double x, double y, double z, matrix &mtx );
void    SPMX_xyz2mtx( double xyz[], matrix &mtx );
matrix  SPMX_xyz2mtx( float x, float y, float z );
matrix  SPMX_xyz2mtx( double x, double y, double z );
matrix  SPMX_xyz2mtx( double xyz[] );

void    SPMX_xyz2pomx( float x, float y, float z, matrix &pomx );
matrix  SPMX_xyz2pomx( float x, float y, float z );
void    SPMX_pomx2xyz( matrix &pomx, float &x, float &y, float &z );
void    SPMX_xyz2pomx( float xyz[], matrix &pomx );
void    SPMX_pomx2xyz( matrix &pomx, float xyz[] );
void    SPMX_xyz2ptmx( float x, float y, float z, matrix &ptmx );
void    SPMX_ptmx2xyz( matrix &ptmx, float &x, float &y, float &z );

void    SPMX_xyz2eamx( float x, float y, float z, matrix &eamx );
void    SPMX_xyz2eamx( float xyz[], matrix &eamx );
void    SPMX_eamx2xyz( matrix &eamx, float &x, float &y, float &z );

void    SPMX_pomx2ptmx( matrix &pomx, matrix &ptmx );
void    SPMX_ptmx2pomx( matrix &ptmx, matrix &pomx );
void    SPMX_poea2pemx( matrix &pomx, matrix &eamx, matrix &pemx );
void    SPMX_pemx2poea( matrix &pemx, matrix &pomx, matrix &eamx );
void    SPMX_xyz2romx( double x, double y, double z, matrix &romx );
void    SPMX_romx2xyz( matrix &romx, double &x, double &y, double &z );
void    SPMX_eamx2romx( matrix &eamx, matrix &romx );
void    SPMX_romx2eamx( matrix &romx, matrix &eamx );
void    SPMX_pamx2rtmx( matrix &pamx, matrix &rtmx );
void    SPMX_rtmx2pamx( matrix &rtmx, matrix &pamx );
void    SPMX_poea2rtmx( matrix &pomx, matrix &eamx, matrix &rtmx );
void    SPMX_rtmx2poea( matrix &rtmx, matrix &pomx, matrix &eamx );
void    SPMX_poro2rtmx( matrix &pomx, matrix &romx, matrix &rtmx );
void    SPMX_pomx2rtmx( matrix &pomx, matrix &rtmx );
void    SPMX_ptmx2rtmx( matrix &ptmx, matrix &rtmx );
void    SPMX_romx2rtmx( matrix &romx, matrix &rtmx );
void    SPMX_rtmx2poro( matrix &rtmx, matrix &pomx, matrix &romx );
void    SPMX_rtmx2pomx( matrix &rtmx, matrix &pomx );
void    SPMX_rtmx2ptmx( matrix &rtmx, matrix &ptmx );
void    SPMX_rtmx2romx( matrix &rtmx, matrix &romx );

//      Find RTMX to get from one set of points to the other (translation optional)...
BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, BOOL translation );
BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, matrix &xyz3, double &err, BOOL translation );
BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, double &err, BOOL translation );

//      Find RTMX to get from one set of points to the other (allow translation)...
BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2 );
BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, matrix &xyz3, double &err );
BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, double &err );

void    SPMX_doRT( matrix &rtmx, matrix &pomx );
void    SPMX_doRT( matrix &rtmx, matrix &xyz1, matrix &xyz2 );
void    SPMX_doRT( matrix &rtmx, double &x, double &y, double &z );
void    SPMX_doRT( matrix &rtmx, double xyz[] );
void    SPMX_doRT( matrix &rtmx, float &x, float &y, float &z );

void    SPMX_undoRT( matrix &rtmx, matrix &pomx );
void    SPMX_undoRT( matrix &rtmx, matrix &xyz1, matrix &xyz2 );
void    SPMX_undoRT( matrix &rtmx, double &x, double &y, double &z );
void    SPMX_undoRT( matrix &rtmx, double xyz[] );
void    SPMX_undoRT( matrix &rtmx, float &x, float &y, float &z );

void    SPMX_doR( matrix &romx, matrix &pomx );
void    SPMX_doR( matrix &romx, matrix &xyz1, matrix &xyz2 );
void    SPMX_doR( matrix &romx, double &x, double &y, double &z );
void    SPMX_doR( matrix &romx, double xyz[] );
void    SPMX_doR( matrix &romx, float &x, float &y, float &z );

void    SPMX_romxX( double angle, matrix &R );
void    SPMX_romxY( double angle, matrix &R );
void    SPMX_romxZ( double angle, matrix &R );

matrix  SPMX_romxX( double angle );
matrix  SPMX_romxY( double angle );
matrix  SPMX_romxZ( double angle );

matrix  SPMX_rtmxX( double angle );
matrix  SPMX_rtmxY( double angle );
matrix  SPMX_rtmxZ( double angle );

matrix  SPMX_rtmx2pomx( matrix &rtmx );
matrix  SPMX_rtmx2ptmx( matrix &rtmx );
matrix  SPMX_rtmx2romx( matrix &rtmx );
matrix  SPMX_ptmx2pomx( matrix &ptmx );
matrix  SPMX_pomx2ptmx( matrix &pomx );
matrix  SPMX_romx2rtmx( matrix &romx );
matrix  SPMX_pomx2rtmx( matrix &pomx );
matrix  SPMX_ptmx2rtmx( matrix &ptmx );
matrix  SPMX_xyz2rtmx( double x, double y, double z );

void    SPMX_qtmx2romx( matrix &qtmx, matrix &romx );
void    SPMX_qtmx2rtmx( matrix &qtmx, matrix &rtmx );
void    SPMX_romx2qtmx( matrix &romx, matrix &qtmx );
void    SPMX_rtmx2qtmx( matrix &rtmx, matrix &qtmx );

void    SPMX_qtmx2ramx( matrix &qtmx, matrix &ramx );
void    SPMX_ramx2qtmx( matrix &ramx, matrix &qtmx );

void    SPMX_ramx2romx( matrix &ramx, matrix &romx );
void    SPMX_ramx2rtmx( matrix &ramx, matrix &rtmx );
void    SPMX_romx2ramx( matrix &romx, matrix &ramx );
void    SPMX_rtmx2ramx( matrix &rtmx, matrix &ramx );

/******************************************************************************/

void    SPMX_romx2exyz( matrix &romx, matrix &eamx );
matrix  SPMX_romx2exyz( matrix &romx );

void    SPMX_romx2exzy( matrix &romx, matrix &eamx );
matrix  SPMX_romx2exzy( matrix &romx );

void    SPMX_romx2eyxz( matrix &romx, matrix &eamx );
matrix  SPMX_romx2eyxz( matrix &romx );

void    SPMX_romx2eyzx( matrix &romx, matrix &eamx );
matrix  SPMX_romx2eyzx( matrix &romx );

void    SPMX_romx2ezxy( matrix &romx, matrix &eamx );
matrix  SPMX_romx2ezxy( matrix &romx );

void    SPMX_romx2ezyx( matrix &romx, matrix &eamx );
matrix  SPMX_romx2ezyx( matrix &romx );

/******************************************************************************/

void    SPMX_exyz2romx( matrix &eamx, matrix &romx );
matrix  SPMX_exyz2romx( matrix &eamx );

void    SPMX_exzy2romx( matrix &eamx, matrix &romx );
matrix  SPMX_exzy2romx( matrix &eamx );

void    SPMX_eyxz2romx( matrix &eamx, matrix &romx );
matrix  SPMX_eyxz2romx( matrix &eamx );

void    SPMX_eyzx2romx( matrix &eamx, matrix &romx );
matrix  SPMX_eyzx2romx( matrix &eamx );

void    SPMX_ezxy2romx( matrix &eamx, matrix &romx );
matrix  SPMX_ezxy2romx( matrix &eamx );

void    SPMX_ezyx2romx( matrix &eamx, matrix &romx );
matrix  SPMX_ezyx2romx( matrix &eamx );

/******************************************************************************/

#define SPMX_EAMX_MAX   6
#define SPMX_EAMS_XYZ   0
#define SPMX_EAMS_XZY   1
#define SPMX_EAMS_YXZ   2
#define SPMX_EAMS_YZX   3
#define SPMX_EAMS_ZXY   4
#define SPMX_EAMS_ZYX   5

extern  struct  STR_TextItem  SPMX_eamx[];

int     SPMX_eamxtype( char *text );

/******************************************************************************/

void    SPMX_romx2eamx( matrix &romx, matrix &eamx, int type );
void    SPMX_eamx2romx( matrix &eamx, matrix &romx, int type );

/******************************************************************************/

#define SPMX_AXIS_X  1
#define SPMX_AXIS_Y  2
#define SPMX_AXIS_Z  3

/******************************************************************************/

#define SPMX_AXIS_NONE  -1

/******************************************************************************/

//  The follow code-fragment is generated by a program....
#define SPMX_AXIS_XYZ    0
#define SPMX_AXIS_YnXZ   1
#define SPMX_AXIS_nZYX   2
#define SPMX_AXIS_XZnY   3
#define SPMX_AXIS_nXnYZ  4
#define SPMX_AXIS_nXYnZ  5
#define SPMX_AXIS_XnYnZ  6
#define SPMX_AXIS_nYXZ   7
#define SPMX_AXIS_ZYnX   8
#define SPMX_AXIS_XnZY   9
#define SPMX_AXIS_YZX    10
#define SPMX_AXIS_nZnXY  11
#define SPMX_AXIS_ZnXnY  12
#define SPMX_AXIS_nZXnY  13
#define SPMX_AXIS_ZnYX   14
#define SPMX_AXIS_nYnXnZ 15
#define SPMX_AXIS_nXnZnY 16
#define SPMX_AXIS_YXnZ   17
#define SPMX_AXIS_nXZY   18
#define SPMX_AXIS_nZnYnX 19
#define SPMX_AXIS_nYnZX  20
#define SPMX_AXIS_YnZnX  21
#define SPMX_AXIS_nYZnX  22
#define SPMX_AXIS_ZXY    23
//  End code-fragment...

#define SPMX_AXIS_MAX    24

/******************************************************************************/

int     SPMX_axistype( char *text );

void    SPMX_axisinit( matrix *romx );
void    SPMX_axisinit( void );

void    SPMX_axispomx( int axis, matrix &pomx );
void    SPMX_axisromx( int axis, matrix &romx );
void    SPMX_axisrtmx( int axis, matrix &rtmx );

/******************************************************************************/

GLdouble *OpenGL_rtmx2double( matrix &rtmx, BOOL translate );

/******************************************************************************/

double deg0to360( double angle );

/******************************************************************************/

