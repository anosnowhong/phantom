#ifndef NDPACK_H_INCLUDED
#define NDPACK_H_INCLUDED

/*
 * The following #define statements can be used to select the version
 * Pack/UnPack routines required for the appropriate host computer.
 * If your computer is listed, change the defined constant to a 1.
 * If your computer is not listed, you will have to add the appropriate
 * defines for it.
 */

#define IBMPC           1
#define APOLLO          0
#define AMIGA           0
#define MACINTOSH       0
#define BSD386          0

#if IBMPC
#define HOSTDEFINED
#define LITTLE_ENDIAN
#define IEEE_FLOATS
#endif

#if (BSD386)
#define HOSTDEFINED
#define PACK_UNPACK
#define LITTLE_ENDIAN
#define IEEE_FLOATS
#endif

#if (AMIGA | MACINTOSH)
#define HOSTDEFINED
#define PACK_UNPACK
#define BIG_ENDIAN
#define IEEE_FLOATS
#endif

#if APOLLO
#define BIG_ENDIAN
#define IEEE_FLOATS
#define PACK_UNPACK
#define HOSTDEFINED
#endif

#ifndef HOSTDEFINED
#error You must define the type of system you are using...
#endif

#if !IBMPC
#define far
#define FAR
#define near
#define NEAR
#define huge
#define HUGE
#endif

#ifdef  REAL64
#define PackRealType( p, q, s )     PackDouble( (p), (q), (s) )
#define UnPackRealType( p, q, s )   UnPackDouble( (p), (q), (s) )
#else
#define PackRealType( p, q, s )     PackFloat( (p), (q), (s) )
#define UnPackRealType( p, q, s )   UnPackFloat( (p), (q), (s) )
#endif

#define UnPackUChar( p, q, s )  (*(p) = *(q), 1 )
#define UnPackChar( p, q, s )   UnPackUChar( (unsigned char *)(p), (q), (s) )
#define UnPackUnsigned(p, q, s) (*(p) = (unsigned)(q)[1] << 8 | *(q), 2 )
#define UnPackInt( p, q, s )    (*(p) = (int)(((signed char *)(q))[1]) << 8 | \
                                        *(q), 2 )
#define UnPackULong( p, q, s )  (*(p) = (unsigned long)(q)[3] << 24 |       \
                                        (unsigned long)(q)[2] << 16 |       \
                                        (unsigned long)(q)[1] << 8 | *(q), 4 )
#define UnPackLong( p, q, s )   (*(p) = (long)(((signed char *)(q))[3]) << 24 |\
                                        (unsigned long)(q)[2] << 16 |       \
                                        (unsigned long)(q)[1] << 8 | *(q), 4 )
#define UnPackFloat( p, q, s )  UnPackULong( ((unsigned long *)(p)), (q), (s) )

#define PackUChar( p, q, s )    (*(q) = *(p), 1 )
#define PackChar( p, q, s )     PackUChar( (unsigned char *)(p), (q), (s) )
#define PackUnsigned( p, q, s ) ((q)[1] = (*(p) >> 8) & 0xff,               \
                                 (q)[0] = *(p) & 0xff, 2 )
#define PackInt( p, q, s )      PackUnsigned( (unsigned *)(p), (q), (s) )
#define PackULong( p, q, s )    ((q)[3] = (*(p) >> 24) & 0xff,              \
                                 (q)[2] = (*(p) >> 16) & 0xff,              \
                                 (q)[1] = (*(p) >> 8) & 0xff,               \
                                 (q)[0] = *(p) & 0xff, 4 )
#define PackLong( p, q, s )     PackULong( (unsigned long *)(p), (q), (s) )
#define PackFloat( p, q, s )    PackULong( ((unsigned long *)(p)), (q), (s) )

#ifdef PACK_UNPACK
#define PC_CHAR_SIZE        1
#define PC_INT_SIZE         2
#define PC_LONG_SIZE        4
#define PC_FLOAT_SIZE       4
#define PC_DOUBLE_SIZE      8
#else
#define PC_CHAR_SIZE        sizeof(char)
#define PC_INT_SIZE         sizeof(int)
#define PC_LONG_SIZE        sizeof(long)
#define PC_FLOAT_SIZE       sizeof(float)
#define PC_DOUBLE_SIZE      sizeof(double)
#endif


unsigned PackDouble( double *pDbl, unsigned char *pchBuff, unsigned uBufSiz );
unsigned UnPackDouble( double *pDbl, unsigned char *pchBuff, unsigned uBufSiz );

extern char
    szErrOverRun[];

void HandleOverrun( char *pszStr );

CPLUSPLUS_START
unsigned
    PackRotationMatrix( RotationMatrixType *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackRotationMatrix( RotationMatrixType *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackComplex( struct ComplexStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackComplex( struct ComplexStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackRotation( struct RotationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackRotation( struct RotationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackPosition3d( struct Position3dStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackPosition3d( struct Position3dStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackTransformation( struct TransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackTransformation( struct TransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackRotationTransformation( struct RotationTransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackRotationTransformation( struct RotationTransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackQuatRotation( struct QuatRotationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackQuatRotation( struct QuatRotationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackQuatTransformation( struct QuatTransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackQuatTransformation( struct QuatTransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackData3D( struct data3DStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackData3D( struct data3DStruct *p, unsigned char *pchBuff, unsigned uBuffSize );

CPLUSPLUS_END

#endif

