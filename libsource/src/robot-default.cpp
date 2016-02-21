/******************************************************************************/
/* Functions for using default ROBOT ID (ROBOT_ID).                           */
/******************************************************************************/

BOOL ROBOT_Start( void (*func)( long EU[], long MU[] ) )
{
BOOL ok;

    ok = ROBOT_Start(ROBOT_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_Start( void (*func)( matrix &p ) )
{
BOOL ok;

    ok = ROBOT_Start(ROBOT_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_Start( void (*func)( matrix &p, matrix &f ) )
{
BOOL ok;

    ok = ROBOT_Start(ROBOT_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_Start( void (*func)( matrix &p, matrix &v, matrix &f ) )
{
BOOL ok;

    ok = ROBOT_Start(ROBOT_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_Start( void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) )
{
BOOL ok;

    ok = ROBOT_Start(ROBOT_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( void (*func)( void ) )
{
BOOL ok;

    ok = ROBOT_Start(ROBOT_ID,func);

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_Start( void )
{
    return(ROBOT_Start(ROBOT_ID));
}

/******************************************************************************/

void ROBOT_Stop( void )
{
    ROBOT_Stop(ROBOT_ID);
}

/******************************************************************************/

void ROBOT_Close( void )
{
    ROBOT_Close(ROBOT_ID);
}

/******************************************************************************/

void ROBOT_PanicNow( void )
{
    ROBOT_PanicNow(ROBOT_ID);
}

/******************************************************************************/

BOOL ROBOT_PanicOff2On( void )
{
    return(ROBOT_PanicOff2On(ROBOT_ID));
}

/******************************************************************************/

BOOL ROBOT_PanicOn2Off( void )
{
    return(ROBOT_PanicOn2Off(ROBOT_ID));
}

/******************************************************************************/

BOOL ROBOT_Cooling( void )
{
    return(ROBOT_Cooling(ROBOT_ID));
}

/******************************************************************************/

BOOL ROBOT_Safe( void )
{
    return(ROBOT_Safe(ROBOT_ID));
}

/******************************************************************************/

void ROBOT_UnSafe( void )
{
    ROBOT_UnSafe(ROBOT_ID);
}

/******************************************************************************/

BOOL ROBOT_Panic( void )
{
    return(ROBOT_Panic(ROBOT_ID));
}

/******************************************************************************/

BOOL ROBOT_Started( void )
{
    return(ROBOT_Started(ROBOT_ID));
}

/******************************************************************************/

BOOL ROBOT_EncoderReset( void )
{
BOOL ok;

    ok = ROBOT_EncoderReset(ROBOT_ID);

    return(ok);
}

/******************************************************************************/

void ROBOT_Encoder( int axis, double &xraw, double &x, double &dx, double &ddx )
{
    ROBOT_Encoder(ROBOT_ID,axis,xraw,x,dx,ddx);
}

/******************************************************************************/

void ROBOT_Encoder( double xraw[], double x[], double dx[], double ddx[] )
{
    ROBOT_Encoder(ROBOT_ID,xraw,x,dx,ddx);
}

/******************************************************************************/

void ROBOT_Encoder( double xraw[] )
{
    ROBOT_Encoder(ROBOT_ID,xraw);
}

/******************************************************************************/

void ROBOT_MotorReset( void )
{
    ROBOT_MotorReset(ROBOT_ID);
}

/******************************************************************************/

long ROBOT_MotorGetMU( int axis )
{
    return(ROBOT_MotorGetMU(ROBOT_ID,axis));
}

/******************************************************************************/

void ROBOT_MotorGetMU( long MU[] )
{
    ROBOT_MotorGetMU(ROBOT_ID,MU);
}

/******************************************************************************/

double ROBOT_MotorGetNm( int axis )
{
    return(ROBOT_MotorGetNm(ROBOT_ID,axis));
}

/******************************************************************************/

void ROBOT_MotorGetNm( double Nm[] )
{
    ROBOT_MotorGetNm(ROBOT_ID,Nm);
}

/******************************************************************************/

void ROBOT_MotorGetNm( matrix &Nm )
{
    ROBOT_MotorGetNm(ROBOT_ID,Nm);
}

/******************************************************************************/

double ROBOT_TempMotor( int motor )
{
    return(ROBOT_TempMotor(ROBOT_ID,motor));
}

/******************************************************************************/

void ROBOT_TempMotor( double T[] )
{
    ROBOT_TempMotor(ROBOT_ID,T);
}

/******************************************************************************/

void ROBOT_AnglesRaw( matrix &AP )
{
    ROBOT_AnglesRaw(ROBOT_ID,AP);
}

/******************************************************************************/

void ROBOT_AnglesRaw( matrix &AP, matrix &AV, matrix &AA )
{
    ROBOT_AnglesRaw(ROBOT_ID,AP,AV,AA);
}

/******************************************************************************/

void ROBOT_AnglesRaw( double angpos[], double angvel[], double angacc[] )
{
    ROBOT_AnglesRaw(ROBOT_ID,angpos,angvel,angacc);
}

/******************************************************************************/

void ROBOT_AnglesRaw( double angpos[] )
{
    ROBOT_AnglesRaw(ROBOT_ID,angpos);
}

/******************************************************************************/

void ROBOT_Angles( matrix &AP, matrix &AV, matrix &AA )
{
    ROBOT_Angles(ROBOT_ID,AP,AV,AA);
}

/******************************************************************************/

void ROBOT_Angles( matrix &AP )
{
    ROBOT_Angles(ROBOT_ID,AP);
}

/******************************************************************************/

void ROBOT_Angles( double angpos[], double angvel[], double angacc[] )
{
    ROBOT_Angles(ROBOT_ID,angpos,angvel,angacc);
}

/******************************************************************************/

void ROBOT_Angles( double angpos[] )
{
    ROBOT_Angles(ROBOT_ID,angpos);
}

/******************************************************************************/

void ROBOT_Angles( matrix &AP, matrix &AV, matrix &AA, BOOL offset )
{
    ROBOT_Angles(ROBOT_ID,AP,AV,AA,offset);
}

/******************************************************************************/

void ROBOT_Angles( double angpos[], double angvel[], double angacc[], BOOL offset )
{
    ROBOT_Angles(ROBOT_ID,angpos,angvel,angacc,offset);
}

/******************************************************************************/

void ROBOT_Position( matrix &P )
{
    ROBOT_Position(ROBOT_ID,P);
}

/******************************************************************************/

void ROBOT_Position( matrix &P, matrix &V, matrix &A )
{
    ROBOT_Position(ROBOT_ID,P,V,A);
}

/******************************************************************************/

void    ROBOT_Position( double p_xyz[] )
{
    ROBOT_Position(ROBOT_ID,p_xyz);
}

/******************************************************************************/

void    ROBOT_Position( double p_xyz[], double v_xyz[], double a_xyz[] )
{
    ROBOT_Position(ROBOT_ID,p_xyz,v_xyz,a_xyz);
}

/******************************************************************************/

void ROBOT_PositionMarker( int marker, matrix &P )
{
    ROBOT_PositionMarker(ROBOT_ID,marker,P);
}

/******************************************************************************/

void    ROBOT_PositionMarker( int marker, double xyz[] )
{
    ROBOT_PositionMarker(ROBOT_ID,marker,xyz);
}

/******************************************************************************/

double ROBOT_RampValue( void )
{
    return(ROBOT_RampValue(ROBOT_ID));
}

/******************************************************************************/

long ROBOT_RampMU( long MU )
{
    return(ROBOT_RampMU(ROBOT_ID,MU));
}

/******************************************************************************/

void ROBOT_RampZero( void )
{
    ROBOT_RampZero(ROBOT_ID);
}

/******************************************************************************/

ROBOT *ROBOT_Robot( void )
{
ROBOT *robot=NULL;

    if( ROBOT_Check(ROBOT_ID) )
    {
        robot = ROBOT_Item[ROBOT_ID].Robot;
    }

    return(robot);
}

/******************************************************************************/

int ROBOT_TDOF( void )
{
int dof;

    dof = ROBOT_TDOF(ROBOT_ID);

    return(dof);
}

/******************************************************************************/

int ROBOT_RDOF( void )
{
int dof;

    dof = ROBOT_RDOF(ROBOT_ID);

    return(dof);
}

/******************************************************************************/

int ROBOT_DOF( void )
{
int dof;

    dof = ROBOT_DOF(ROBOT_ID);

    return(dof);
}

/******************************************************************************/

BOOL ROBOT_3D( void )
{
BOOL flag;

    flag = ROBOT_3D(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_2D( void )
{
BOOL flag;

    flag = ROBOT_2D(ROBOT_ID);

    return(flag);
}


/******************************************************************************/

BOOL ROBOT_Activated( void )
{
BOOL flag=FALSE;

    flag = ROBOT_Activated(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_JustActivated( void )
{
BOOL flag=FALSE;

    flag = ROBOT_JustActivated(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_JustDeactivated( void )
{
BOOL flag=FALSE;

    flag = ROBOT_JustDeactivated(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

double ROBOT_LoopTaskGetFrequency( void )
{
double freq;

    freq = ROBOT_LoopTaskGetFrequency(ROBOT_ID);

    return(freq);
}

/******************************************************************************/

double ROBOT_LoopTaskGetPeriod( void )
{
double period;

    period = ROBOT_LoopTaskGetPeriod(ROBOT_ID);

    return(period);
}

/******************************************************************************/

UINT ROBOT_OptoTrakFrameLast( void )
{
UINT frame;

    frame = ROBOT_OptoTrakFrameLast(ROBOT_ID);

    return(frame);
}

/******************************************************************************/

double ROBOT_LoopTaskLatencyGet( void )
{
double value;

    value = ROBOT_LoopTaskLatencyGet(ROBOT_ID);

    return(value);
}

/******************************************************************************/

