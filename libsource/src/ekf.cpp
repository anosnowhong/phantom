/******************************************************************************/
/*                                                                            */
/* MODULE  : EKF.h                                                            */
/*                                                                            */
/* PURPOSE : Extended Kalman Filter (3rd order) class.                        */
/*                                                                            */
/* DATE    : 30/Jan/2009                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 30/Jan/2009 - Initial development.                               */
/*                                                                            */
/******************************************************************************/

#include <motor.h>

/******************************************************************************/

int EKF_Index=0;
char *EKF_DataTypeText[] = { "dtime","xraw","xfilt","dxfilt","ddxfilt" };

/******************************************************************************/

EKF::EKF( char *name )
{
    Init(name);
}

/******************************************************************************/

EKF::EKF( void )
{
    Init(NULL);
}

/******************************************************************************/

EKF::~EKF( )
{
    if( timer != NULL )
    {
        delete timer;
        timer = NULL;
    }

    if( freq != NULL )
    {
        delete freq;
        freq = NULL;
    }

    DataFree();
}

/******************************************************************************/

void EKF::Init( void )
{
int i,j;

    memset(ObjectName,0,STRLEN);
    OpenFlag = FALSE;
    FirstFlag = FALSE;
    ArmType = EKF_ARM_UNKNOWN;

    timer = NULL;
    freq = NULL;
    tlast = 0.0;

    for( i=0; (i < EKF_DATA); i++ )
    {
        for( j=0; (j < EKF_DOF); j++ )
        {
            data[i][j] = NULL;
        }
    }
}

/******************************************************************************/

void EKF::Init( char *name )
{
    Init();

    if( name == NULL )
    {
        strncpy(ObjectName,STR_stringf("KALMAN%d",EKF_Index),STRLEN);
    }
    else
    {
        strncpy(ObjectName,name,STRLEN);
    }

    timer = new TIMER(ObjectName);
    freq = new TIMER_Frequency(ObjectName);

    EKF_Index++;
}

/******************************************************************************/

void EKF::Init2D( void )
{
    H.dim(2,6);
    Q.dim(6,6);
    R.dim(2,2);
    x.dim(6,1);
    P.dim(6,6);
    A.dim(6,6);
    J.dim(2,2);
    S.dim(2,2);
    a.dim(2,1);
    z.dim(2,1);
    px.dim(2,1);

    H.zeros();

    Q.zeros();

    R.zeros();
    R(1,1) = v*v;
    R(2,2) = R(1,1);

    A = I(6);

    FirstFlag = TRUE;
}

/******************************************************************************/

void EKF::Init3D( void )
{
    FirstFlag = TRUE;
}

/******************************************************************************/

double EKF::MatrixDeterminant2x2( matrix &A )
{
double d=0.0;

    if( (A.rows() == 2) && A.issquare() )
    {
        d = ((A(1,1)*A(2,2))-(A(1,2)*A(2,1)));
    }

    return(d);
}

/******************************************************************************/

void EKF::MatrixInverse2x2( matrix &A, matrix &invA )
{
double d;

    if( (A.rows() == 2) && A.issquare() )
    {
        invA.dim(2,2);
        d = MatrixDeterminant2x2(A);

        if( d == 0.0 )
        {
            return;
        }

        d = 1.0/d;
        invA(1,1) = A(2,2)*d;
        invA(1,2) = -A(1,2)*d;
        invA(2,1) = -A(2,1)*d;
        invA(2,2) = A(1,1)*d;
    }
}

/******************************************************************************/

void EKF::Calculate( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[], int &line )
{
    if( !OpenFlag )
    {
        return;
    }

    freq->Loop();
    
    switch( dof )
    {
        case 2 :
           Calculate2D(xraw,dtime,xfilt,dxfilt,ddxfilt,line);
           break;

        case 3 :
           Calculate3D(xraw,dtime,xfilt,dxfilt,ddxfilt,line);
           break;
    }
}

/******************************************************************************/

void EKF::Calculate( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[] )
{
int line;

    Calculate(xraw,dtime,xfilt,dxfilt,ddxfilt,line);
}

/******************************************************************************/

// #define EKF_DEBUG

void EKF::Calculate2D( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[], int &line )
{
static int i,j;
static double d,xm,ym,f,dw,dm,l1,l2;

    line = 0;

    if( !OpenFlag )
    {
        return;
    }

    freq->Loop();
    
    A(1,2) = dtime;
    A(1,3) = pow(dtime,2.0) / 2.0;
    A(2,3) = dtime;
    A(4,5) = A(1,2);
    A(4,6) = A(1,3);
    A(5,6) = A(2,3);

    Q(1,1) = pow(dtime,5.0) / 20.0;
    Q(1,2) = pow(dtime,4.0) / 8.0;
    Q(1,3) = pow(dtime,3.0) / 6.0;
    Q(2,1) = pow(dtime,4.0) / 8.0;
    Q(2,2) = pow(dtime,3.0) / 3.0;
    Q(2,3) = pow(dtime,2.0) / 2.0;
    Q(3,1) = pow(dtime,3.0) / 6.0;
    Q(3,2) = pow(dtime,2.0) / 2.0;
    Q(3,3) = dtime;

    for( i=1; (i <= 3); i++ )
    {
        for( j=1; (j <= 3); j++ )
        {
            Q(3+i,3+j) = Q(i,j);
        }
    }

    Q *= (w*w);

    // Limit of reachable workspace.
    dw = link[0] + link[1];

    // Estimate position based on raw angles.
    xm = (-link[0]*sin(xraw[0])) + (link[1]*sin(xraw[1]));
    ym = (-link[0]*cos(xraw[0])) + (-link[1]*cos(xraw[1]));

    // Bound estimate of position to within reachable workspace.
    dm = sqrt((xm*xm)+(ym*ym));

    if( dm > dw )
    {
        xm *= dw/dm;
        ym *= dw/dm;
    }

    if( FirstFlag )
    {
        // Initial estimate of position based on raw angles.
        px(1,1) = xm;
        px(2,1) = ym;

        for( i=1,j=1; (j <= dof); j++ )
        {
            x(i++,1) = px(j,1);
            x(i++,1) = 0.0;
            x(i++,1) = 0.0;
        }

        P = Q;

        FirstFlag = FALSE;

        // Set arm type based on elbow angle.
        if( R2D(xraw[1]) > 200.0 )
        {
            ArmType = EKF_ARM_LEFT;
            printf("EKF-%s: ArmType=LEFT\n",ObjectName);

        }
        else
        {
            ArmType = EKF_ARM_RIGHT;
            printf("EKF-%s: ArmType=RIGHT\n",ObjectName);
        }
    }

    // New cartesian estimate.
    x = A*x;

    if( UndefinedDouble(x) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: x (line=%d).\n",line);
#endif
        return;
    }

    switch( ArmType )
    {
        case EKF_ARM_RIGHT :
           l1 = link[0];
           l2 = link[1];
           break;

        case EKF_ARM_LEFT :
           l1 = link[0];
           l2 = -link[1];
           break;
    }

    // Calculate angles associated with this estimate.
    xm = x(1,1);
    ym = x(4,1);

    // Bound estimate of position to within reachable workspace.
    dm = sqrt((xm*xm)+(ym*ym));

    if( dm > dw )
    {
        xm *= dw/dm;
        ym *= dw/dm;
    }

    f = ((xm*xm)+(ym*ym)-(l1*l1)-(l2*l2)) / (2.0*l1*l2);

    if( abs(f) > 1.0 )
    {
        f = 1.0;
    }
    else
    if( abs(f) < -1.0 )
    {
        f = -1.0;
    }

    a(2,1) = acos(f);

    if( UndefinedDouble(a(2,1)) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: a(2,1) (line=%d).\n",line);
        printf("acos(%.5lf)\n",f);
#endif
        return;
    }

    a(1,1) = atan2(-ym,-xm) - atan2(l2*sin(a(2,1)),l1+(l2*cos(a(2,1))));

    if( UndefinedDouble(a(1,1)) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: a(1,1) (line=%d).\n",line);
        printf("atan2(%.5lf,%.5lf)\n",-ym,-xm);
        printf("atan2(%.5lf,%.5lf)\n",link[1]*sin(a(2,1)),link[0]+(link[1]*cos(a(2,1))));
#endif
        return;
    }

    switch( ArmType )
    {
        case EKF_ARM_RIGHT :
           a(1,1) = (PI/2.0)-a(1,1);
           a(2,1) = a(2,1)-a(1,1);
           if( a(1,1) > D2R(150.0) )
           {
               a(1,1) -= (2*PI);
           }
           break;

        case EKF_ARM_LEFT :
           a(1,1) = (PI/2.0)-a(1,1);
           a(2,1) = (a(2,1)-a(1,1))+PI;
           break;
    }

    J(1,1) = -link[0]*cos(a(1,1));
    J(1,2) = link[1]*cos(a(2,1));
    J(2,1) = link[0]*sin(a(1,1));
    J(2,2) = link[1]*sin(a(2,1));

    if( UndefinedDouble(J) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: J (line=%d).\n",line);
#endif
        return;
    }

    //invJ = inv(J);
    if( MatrixDeterminant2x2(J) == 0.0 )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: det(J) == 0 (line=%d).\n",line);
#endif
        return;
    }

    MatrixInverse2x2(J,invJ);

    if( UndefinedDouble(invJ) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: invJ (line=%d).\n",line);
#endif
        return;
    }

    H(1,1) = invJ(1,1);
    H(1,4) = invJ(1,2);
    H(2,1) = invJ(2,1);
    H(2,4) = invJ(2,2);

    Pm = (A*P*T(A))+Q;

    if( UndefinedDouble(Pm) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: Pm (line=%d).\n",line);
#endif
        return;
    }

    //S = (H*Pm*T(H))+(V*R*T(V));
    S = (H*Pm*T(H))+R;

    if( UndefinedDouble(S) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: S (line=%d).\n",line);
#endif
        return;
    }

    //invS = inv(S);
    if( MatrixDeterminant2x2(S) == 0.0 )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: det(S) == 0 (line=%d).\n",__LINE__);
#endif
        return;
    }

    MatrixInverse2x2(S,invS);

    if( UndefinedDouble(invS) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: f (line=%d).\n",line);
#endif
        return;
    }

    K = Pm*T(H)*invS;

    if( UndefinedDouble(K) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: K (line=%d).\n",line);
#endif
        return;
    }

    z(1,1) = xraw[0];
    z(2,1) = xraw[1];

    x = x+(K*(z-a));

    if( UndefinedDouble(x) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: x (line=%d).\n",line);
#endif
        return;
    }

    // Bound estimate of position to within reachable workspace.
    xm = x(1,1);
    ym = x(4,1);
    dm = sqrt((xm*xm)+(ym*ym));

    if( dm > dw )
    {
        x(1,1) *= dw/dm;
        x(4,1) *= dw/dm;
    }

    P = (I(6)-(K*H))*Pm;

    if( UndefinedDouble(P) )
    {
        line = __LINE__;
#ifdef EKF_DEBUG
        printf("EKF UNDEFINED: P (line=%d).\n",line);
#endif
        return;
    }

    for( i=1,j=0; (j < dof); j++ )
    {
        xfilt[j] = x(i++,1);
        dxfilt[j] = x(i++,1);
        ddxfilt[j] = x(i++,1);
    }

    for( i=0; (i < EKF_DATA); i++ )
    {
        for( j=0; (j < dof); j++ )
        {
            if( data[i][j] == NULL )
            {
                continue; 
            }

            switch( i )
            {
                case 0 :
                   d = dtime;
                   break;

                case 1 :
                   d = xraw[j];
                   break;

                case 2 :
                   d = xfilt[j];
                   break;

                case 3 :
                   d = dxfilt[j];
                   break;

                case 4 :
                   d = ddxfilt[j];
                   break;
            }

            data[i][j]->Data(d);
        }
    }
}

/******************************************************************************/

void EKF::Calculate2D( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[] )
{
int line;

    Calculate2D(xraw,dtime,xfilt,dxfilt,ddxfilt,line);
}

/******************************************************************************/

void EKF::Calculate3D( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[], int &line )
{
}

/******************************************************************************/

void EKF::Calculate3D( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[] )
{
int line;

    Calculate3D(xraw,dtime,xfilt,dxfilt,ddxfilt,line);
}

/******************************************************************************/

void EKF::Calculate( double xraw[], double xfilt[], double dxfilt[], double ddxfilt[], int &line )
{
double t,dtime;

    if( !OpenFlag )
    {
        return;
    }

    t = timer->ElapsedSeconds();

    if( FirstFlag )
    {
        dtime = dt;
    }
    else
    {
        dtime = t - tlast;
    }

    tlast = t;

    Calculate(xraw,dtime,xfilt,dxfilt,ddxfilt,line);
}

/******************************************************************************/

void EKF::Calculate( double xraw[], double xfilt[], double dxfilt[], double ddxfilt[] )
{
int line;

    Calculate(xraw,xfilt,dxfilt,ddxfilt,line);
}

/******************************************************************************/

BOOL EKF::DataSave( )
{
int i,j;
BOOL ok;

    if( !OpenFlag )
    {
        return(FALSE);
    }

    for( ok=TRUE,i=0; (i < EKF_DATA); i++ )
    {
        for( j=0; (j < dof); j++ )
        {
            if( data[i][j] != NULL )
            {
                if( !data[i][j]->Save() )
                {
                    ok = FALSE;
                }
            }
        }
    }

    return(ok);
}

/******************************************************************************/

void EKF::DataFree( )
{
int i,j;

    for( i=0; (i < EKF_DATA); i++ )
    {
        for( j=0; (j < EKF_DOF); j++ )
        {
            if( data[i][j] != NULL )
            {
                delete data[i][j];
                data[i][j] = NULL;
            }
        }
    }
}

/******************************************************************************/

BOOL EKF::Opened( void )
{
    return(OpenFlag);
}

/******************************************************************************/

void EKF::Open( int kf_dof, double kf_w, double kf_v, double kf_dt, double kf_link[] )
{
    Open(kf_dof,kf_w,kf_v,kf_dt,kf_link,0);
}

/******************************************************************************/

void EKF::Open( int kf_dof, double kf_w, double kf_v, double kf_dt, double kf_link[], int ditems )
{
int i,j;

    // Don't open the object if parameters are null...
    if( (kf_w * kf_v * kf_dt) == 0.0 )
    {
        return;
    }
    else
    if( (kf_dof < 2) || (kf_dof > 2) )
    {
        return;
    }

    printf("EKF-%s: dof=%d, w=%.3lf v=%.3lf dt=%.3lf link=%.3f,%.3f(m)\n",ObjectName,kf_dof,kf_w,kf_v,kf_dt,kf_link[0],kf_link[1]);

    if( ditems > 0 )
    {
        for( i=0; (i < EKF_DATA); i++ )
        {
            for( j=0; (j < kf_dof); j++ )
            {
                if( (i > 1) || (j == 1) )
                {
                    data[i][j] = new DATAPROC(STR_stringf("%s-%s[%d]",ObjectName,EKF_DataTypeText[i],j),ditems);
                }
            }
        }
    }

    w = kf_w;
    v = kf_v;
    dt = kf_dt;
    dof = kf_dof;

    for( i=0; (i < EKF_LINK); i++ )
    {
        link[i] = kf_link[i];
    }

    switch( dof )
    {
        case 2 :
           Init2D();
           break;

        case 3 :
           Init3D();
           break;
    }

    OpenFlag = TRUE;
}

/******************************************************************************/

void EKF::Close( void )
{
BOOL ok;

    OpenFlag = FALSE;

    ok = DataSave();
    DataFree();
}

/******************************************************************************/

