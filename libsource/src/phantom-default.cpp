/******************************************************************************/
/* Functions for using default PHANTOM ID (PHANTOM_ID).                       */
/******************************************************************************/

BOOL    PHANTOM_Start( void (*func)( long EU[], long MU[] ) )
{
BOOL ok;

    ok = PHANTOM_Start(PHANTOM_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( void (*func)( matrix &p ) )
{
BOOL ok;

    ok = PHANTOM_Start(PHANTOM_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( void (*func)( matrix &p, matrix &f ) )
{
BOOL ok;

    ok = PHANTOM_Start(PHANTOM_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( void (*func)( matrix &p, matrix &v, matrix &f ) )
{
BOOL ok;

    ok = PHANTOM_Start(PHANTOM_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) )
{
BOOL ok;

    ok = PHANTOM_Start(PHANTOM_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( void )
{
    return(PHANTOM_Start(PHANTOM_ID));
}

/******************************************************************************/

void    PHANTOM_Stop( void )
{
    PHANTOM_Stop(PHANTOM_ID);
}

/******************************************************************************/

void    PHANTOM_Close( void )
{
    PHANTOM_Close(PHANTOM_ID);
}

/******************************************************************************/

void    PHANTOM_PanicNow( void )
{
    PHANTOM_PanicNow(PHANTOM_ID);
}

/******************************************************************************/

BOOL    PHANTOM_PanicOff2On( void )
{
    return(PHANTOM_PanicOff2On(PHANTOM_ID));
}

/******************************************************************************/

BOOL    PHANTOM_PanicOn2Off( void )
{
    return(PHANTOM_PanicOn2Off(PHANTOM_ID));
}

/******************************************************************************/

BOOL    PHANTOM_Cooling( void )
{
    return(PHANTOM_Cooling(PHANTOM_ID));
}

/******************************************************************************/

BOOL    PHANTOM_Safe( void )
{
    return(PHANTOM_Safe(PHANTOM_ID));
}

/******************************************************************************/

BOOL    PHANTOM_Panic( void )
{
    return(PHANTOM_Panic(PHANTOM_ID));
}

/******************************************************************************/

BOOL    PHANTOM_Started( void )
{
    return(PHANTOM_Started(PHANTOM_ID));
}

/******************************************************************************/

void    PHANTOM_EncoderReset( void )
{
    PHANTOM_EncoderReset(PHANTOM_ID);
}

/******************************************************************************/

long    PHANTOM_Encoder( int axis )
{
    return(PHANTOM_Encoder(PHANTOM_ID,axis));
}

/******************************************************************************/

void    PHANTOM_MotorReset( void )
{
    PHANTOM_MotorReset(PHANTOM_ID);
}

/******************************************************************************/

float   PHANTOM_MotorTorqueNm( int axis )
{
    return(PHANTOM_MotorTorqueNm(PHANTOM_ID,axis));
}

/******************************************************************************/

long    PHANTOM_MotorTorqueMU( int axis )
{
    return(PHANTOM_MotorTorqueMU(PHANTOM_ID,axis));
}

/******************************************************************************/

void    PHANTOM_MotorTorqueNm( float Nm[] )
{
    PHANTOM_MotorTorqueNm(PHANTOM_ID,Nm);
}

/******************************************************************************/

void    PHANTOM_MotorTorqueMU( long MU[] )
{
    PHANTOM_MotorTorqueMU(PHANTOM_ID,MU);
}

/******************************************************************************/

double  PHANTOM_TempMotor( int motor )
{
    return(PHANTOM_TempMotor(PHANTOM_ID,motor));
}

/******************************************************************************/

void    PHANTOM_TempMotor( double T[] )
{
    PHANTOM_TempMotor(PHANTOM_ID,T);
}

/******************************************************************************/

void    PHANTOM_AngleOffset( float a1, float a2, float a3 )
{
    PHANTOM_AngleOffset(PHANTOM_ID,a1,a2,a3);
}

/******************************************************************************/

void    PHANTOM_AnglesRaw( float &a1, float &a2, float &a3 )
{
    PHANTOM_AnglesRaw(PHANTOM_ID,a1,a2,a3);
}

/******************************************************************************/

void    PHANTOM_AnglesRaw( double &a1, double &a2, double &a3 )
{
    PHANTOM_AnglesRaw(PHANTOM_ID,a1,a2,a3);
}

/******************************************************************************/

void    PHANTOM_AnglesRaw( matrix &mtx )
{
    PHANTOM_AnglesRaw(PHANTOM_ID,mtx);
}

/******************************************************************************/

void    PHANTOM_Angles( float &a1, float &a2, float &a3 )
{
    PHANTOM_Angles(PHANTOM_ID,a1,a2,a3);
}

/******************************************************************************/

void    PHANTOM_Angles( double &a1, double &a2, double &a3 )
{
    PHANTOM_Angles(PHANTOM_ID,a1,a2,a3);
}

/******************************************************************************/

void    PHANTOM_Angles( matrix &mtx )
{
    PHANTOM_Angles(PHANTOM_ID,mtx);
}

/******************************************************************************/

void    PHANTOM_Angles( float &a1, float &a2, float &a3, BOOL offset )
{
    PHANTOM_Angles(PHANTOM_ID,a1,a2,a3,offset);
}

/******************************************************************************/

void    PHANTOM_Angles( double &a1, double &a2, double &a3, BOOL offset )
{
    PHANTOM_Angles(PHANTOM_ID,a1,a2,a3,offset);
}

/******************************************************************************/

void    PHANTOM_Angles( matrix &mtx, BOOL offset )
{
    PHANTOM_Angles(PHANTOM_ID,mtx,offset);
}

/******************************************************************************/

void    PHANTOM_Angles( float angle[], BOOL offset )
{
    PHANTOM_Angles(PHANTOM_ID,angle,offset);
}

/******************************************************************************/

void    PHANTOM_PosnRaw( float &x, float &y, float &z )
{
    PHANTOM_PosnRaw(PHANTOM_ID,x,y,z);
}

/******************************************************************************/

void    PHANTOM_PosnRaw( double &x, double &y, double &z )
{
    PHANTOM_PosnRaw(PHANTOM_ID,x,y,z);
}

/******************************************************************************/

void    PHANTOM_PosnRaw( matrix &mtx )
{
    PHANTOM_PosnRaw(PHANTOM_ID,mtx);
}

/******************************************************************************/

void    PHANTOM_Posn( float &x, float &y, float &z )
{
    PHANTOM_Posn(PHANTOM_ID,x,y,z);
}

/******************************************************************************/

void    PHANTOM_Posn( double &x, double &y, double &z )
{
    PHANTOM_Posn(PHANTOM_ID,x,y,z);
}

/******************************************************************************/

void    PHANTOM_Posn( matrix &mtx )
{
    PHANTOM_Posn(PHANTOM_ID,mtx);
}

/******************************************************************************/

void    PHANTOM_Posn( float &x, float &y, float &z, BOOL adjust )
{
    PHANTOM_Posn(PHANTOM_ID,x,y,z,adjust);
}

/******************************************************************************/

void    PHANTOM_Posn( double &x, double &y, double &z, BOOL adjust )
{
    PHANTOM_Posn(PHANTOM_ID,x,y,z,adjust);
}

/******************************************************************************/

void    PHANTOM_Posn( matrix &mtx, BOOL adjust )
{
    PHANTOM_Posn(PHANTOM_ID,mtx,adjust);
}

/******************************************************************************/

void    PHANTOM_Marker( float angle[], float &x, float &y, float &z, int marker )
{
    PHANTOM_Marker(PHANTOM_ID,angle,x,y,z,marker);
}

/******************************************************************************/

void    PHANTOM_Marker( float a1, float a2, float a3, float &x, float &y, float &z, int marker )
{
    PHANTOM_Marker(PHANTOM_ID,a1,a2,a3,x,y,z,marker);
}

/******************************************************************************/

void    PHANTOM_Marker( float &x, float &y, float &z, int marker )
{
    PHANTOM_Marker(PHANTOM_ID,x,y,z,marker);
}

/******************************************************************************/

void    PHANTOM_Marker( double &x, double &y, double &z, int marker )
{
    PHANTOM_Marker(PHANTOM_ID,x,y,z,marker);
}

/******************************************************************************/

void    PHANTOM_Marker( matrix &mtx, int marker )
{
    PHANTOM_Marker(PHANTOM_ID,mtx,marker);
}

/******************************************************************************/

void    PHANTOM_Encoder( long EU[] )
{
    PHANTOM_Encoder(PHANTOM_ID,EU);
}

/******************************************************************************/

double  PHANTOM_RampValue( void )
{
    return(PHANTOM_RampValue(PHANTOM_ID));
}

/******************************************************************************/

long    PHANTOM_RampMU( long MU )
{
    return(PHANTOM_RampMU(MU));
}

/******************************************************************************/

void    PHANTOM_RampZero( void )
{
    PHANTOM_RampZero(PHANTOM_ID);
}

/******************************************************************************/

ROBOT *PHANTOM_Robot( void )
{
ROBOT *robot=NULL;

    if( PHANTOM_Check(PHANTOM_ID) )
    {
        robot = PHANTOM_Item[PHANTOM_ID].Robot;
    }

    return(robot);
}

/******************************************************************************/

int PHANTOM_DOF( void )
{
int dof;

    dof = PHANTOM_DOF(PHANTOM_ID);

    return(dof);
}

/******************************************************************************/

BOOL PHANTOM_3D( void )
{
BOOL flag;

    flag = PHANTOM_3D(PHANTOM_ID);

    return(flag);
}

/******************************************************************************/

BOOL PHANTOM_2D( void )
{
BOOL flag;

    flag = PHANTOM_2D(PHANTOM_ID);

    return(flag);
}

/******************************************************************************/

