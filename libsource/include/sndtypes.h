/*************************************************************************
*   NDTYPES.H                                                            *
*************************************************************************/
typedef unsigned char       boolean;

#ifndef AMIGA
//typedef char                BYTE;
typedef unsigned char       UBYTE;
#endif

#ifndef FALSE
#define FALSE   0
#define TRUE    1
#endif

#define ERR     (-1)

#ifndef PI
#define PI  (RealType)3.141592654
#endif
#define BAD_FLOAT (float)-3.697314E28
#define MAX_NEGATIVE (float)-3.0E28

#ifndef REAL64
typedef  float           RealType;
#else
typedef  double           RealType;
#endif


struct ComplexStruct
{
    double  re;
    double  im;
};

/*
 * Structures originally from eulerd.h.
 */
typedef struct RotationStruct
{
    RealType   roll;           /* rotation about the object's z-axis */
    RealType   pitch;          /* rotation about the object's y-axis */
    RealType   yaw;            /* rotation about the object's x-axis */
} rotation;

typedef struct Position3dStruct
{
    RealType   x;
    RealType   y;
    RealType   z;
} Position3d;

typedef struct TransformationStruct
{
    rotation   rotation;
    Position3d translation;
} transformation;

typedef RealType
    RotationMatrixType[ 3 ][ 3 ];

typedef struct RotationTransformationStruct
{
    RotationMatrixType      matrix;
    Position3d              translation;
} RotationTransformation;



/*
 * Structure defined in quatern.h.
 */
typedef struct QuatRotationStruct
{
    RealType   q0;
    RealType   qx;
    RealType   qy;
    RealType   qz;
} QuatRotation;

typedef struct QuatTransformationStruct
{
    QuatRotation     rotation;
    Position3d       translation;
} QuatTransformation;

#define VIEW_NAME_LENGTH        12
#define VIEW_NAME_SPACE         13
#define MARKER_NAME_LENGTH      12
#define MARKER_NAME_SPACE       13
struct data3DStruct
{
    unsigned        NumRigidViews;
    unsigned char   ObjectName[ 80];
    unsigned        RigidMarkers;
    unsigned char   *RigidViews;
    Position3d      *RigidPositions;
    unsigned char   (*ViewNames)[VIEW_NAME_SPACE];
    unsigned char   (*RigidMarkerNames)[MARKER_NAME_SPACE];
    unsigned        ImagMarkers;
    Position3d      *ImagPositions;
    unsigned char   (*ImagMarkerNames)[MARKER_NAME_SPACE];
    unsigned        nNormalMarkers;
    Position3d      *pNormals;
    float           fMaxSensorError;
    float           fMax3dError;
    int             nMarkerAngle;
    float           fMax3dRmsError;
    float           fMaxSensorRmsError;
    int             nMinimumMarkers;
    int             nFlags;
};


#ifdef __cplusplus
#define CPLUSPLUS( x ) x
#else
#define CPLUSPLUS( x )
#endif
#ifdef __cplusplus
#define CPLUSPLUS_START extern "C" {
#define CPLUSPLUS_END }
#else
#define CPLUSPLUS_START
#define CPLUSPLUS_END

#endif
