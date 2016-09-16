/******************************************************************************/
/*                                                                            */
/* MODULE  : KalmanFilter.cpp                                                 */
/*                                                                            */
/* PURPOSE : Kalman Filter class.                                             */
/*                                                                            */
/* DATE    : 24/Apr/2007                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 24/Apr/2007 - Initial development.                               */
/*                                                                            */
/* V2.0  JNI 24/Mar/2015 - Re-visit for 3BOT.                                 */
/*                                                                            */
/******************************************************************************/

#include <motor.h>

/******************************************************************************/

int KALMANFILTER_Index=0;

/******************************************************************************/

KALMANFILTER::KALMANFILTER( char *name )
{
    Init(name);
}

/******************************************************************************/

KALMANFILTER::KALMANFILTER( void )
{
    Init(NULL);
}

/******************************************************************************/

KALMANFILTER::~KALMANFILTER( )
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

    if( latency != NULL )
    {
        delete latency;
        latency = NULL;
    }

    DataFree();
}

/******************************************************************************/

void KALMANFILTER::Init( void )
{
int i;

    memset(ObjectName,0,STRLEN);
    OpenFlag = FALSE;
    FirstFlag = FALSE;

    timer = NULL;
    freq = NULL;
    latency = NULL;
    tlast = 0.0;

    for( i=0; (i < KALMANFILTER_DATA); i++ )
    {
        data[i] = NULL;
    }
}

/******************************************************************************/

void KALMANFILTER::Init( char *name )
{
    Init();

    if( name == NULL )
    {
        strncpy(ObjectName,STR_stringf("KALMAN%d",KALMANFILTER_Index),STRLEN);
    }
    else
    {
        strncpy(ObjectName,name,STRLEN);
    }

    timer = new TIMER(ObjectName);
    freq = new TIMER_Frequency(ObjectName);
    latency = new TIMER_Interval(ObjectName);

    /*H.dim(1,3);
    Q.dim(3,3);
    R.dim(1,1);
    x.dim(3,1);
    P.dim(3,3);
    A.dim(3,3);

    A.zeros();
    A(1,1) = 1.0;
    A(2,2) = 1.0;
    A(3,3) = 1.0;

    H(1,1) = 1.0;
    H(1,2) = 0.0;
    H(1,3) = 0.0;*/

    pos = 0.0;
    vel = 0.0;
    acc = 0.0;

    P11 = 0.0;
    P12 = 0.0;
    P13 = 0.0;
    P21 = 0.0;
    P22 = 0.0;
    P23 = 0.0;
    P31 = 0.0;
    P32 = 0.0;
    P33 = 0.0;

    KALMANFILTER_Index++;
}

/******************************************************************************/

double powint( double x, int exponent )
{
double y;
int i;

    for( y=x,i=2; (i <= exponent); i++ )
    {
        y *= y;
    }

    return(y);
}

/******************************************************************************/

void KALMANFILTER::OldCalculate( double xraw, double dtime, double &xfilt, double &dxfilt )
{
double LoopTaskIterationPeriod=0.001,ptime;
static matrix z;

    if( !OpenFlag )
    {
        return;
    }

    freq->Loop();
    //latency->Before();

    if( dtime == 0.0 )
    {
        dtime = dt;
    }

    A(1,2) = dtime;
    A(1,3) = powint(dtime,2) / 2.0;
    A(2,3) = dtime;

    Q(1,1) = powint(dtime,5) / 20.0;
    Q(1,2) = powint(dtime,4) / 8.0;
    Q(1,3) = powint(dtime,3) / 6.0;

    Q(2,1) = Q(1,2);
    Q(2,2) = powint(dtime,3) / 3.0;
    Q(2,3) = powint(dtime,2) / 2.0;

    Q(3,1) = Q(1,3);
    Q(3,2) = Q(2,3);
    Q(3,3) = dtime;

    Q *= (w*w);

    if( FirstFlag )
    {
        x(1,1) = xraw;
        x(2,1) = 0.0;
        x(3,1) = 0.0;
        P = Q;

        FirstFlag = FALSE;
    }

    latency->Before();
    xm = A*x;
    Pm = (A*P*T(A))+Q;
    //K = Pm*T(H)*inv((H*Pm*T(H))+R);
    z = ((H*Pm*T(H))+R);
    K = Pm*T(H)*(1.0/z(1,1));
    x = xm+K*(xraw-H*xm);
    P = (I(3)-K*H)*Pm;
    latency->After();

    ptime = 0.5 * (LoopTaskIterationPeriod);
    A(1,2) = ptime;
    A(1,3) = powint(ptime,2) / 2.0;
    A(2,3) = ptime;

    xm = A*x;

    xfilt = xm(1,1);
    dxfilt = xm(2,1);

    //x(2,1) = (xraw - x(1,1)) / dtime;
    //x(1,1) = xraw;

    //xfilt = x(1,1);
    //dxfilt = x(2,1);

    /*if( data[0] != NULL ) data[0]->Data(dtime);
    if( data[1] != NULL ) data[1]->Data(xraw);
    if( data[2] != NULL ) data[2]->Data(xfilt);
    if( data[3] != NULL ) data[3]->Data(dxfilt);*/

    //latency->After();
}

/******************************************************************************/

void KALMANFILTER::Calculate( double xraw, double dtime, double &xfilt, double &dxfilt )
{
static double LoopTaskIterationPeriod=0.001,ptime;
static double t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25,t26,t27,t28,t29,t30,t31,t32,t33;

    if( !OpenFlag )
    {
        return;
    }

    freq->Loop();
    latency->Before();

    if( dtime == 0.0 )
    {
        dtime = dt;
    }

    if( FirstFlag )
    {
        pos = xraw;
        vel = 0.0;
        acc = 0.0;

        //P = Q;
        P11 = (w*w) * powint(dtime,5) / 20.0;
        P12 = (w*w) * powint(dtime,4) / 8.0;
        P13 = (w*w) * powint(dtime,3) / 6.0;

        P21 = P12;
        P22 = (w*w) * powint(dtime,3) / 3.0;
        P23 = (w*w) * powint(dtime,2) / 2.0;

        P31 = P13;
        P32 = P23;
        P33 = (w*w) * dtime;

        FirstFlag = FALSE;
    }

    // x.cpp
    t2 = dtime*vel;
    t3 = dtime*dtime;
    t4 = acc*t3*(1.0/2.0);
    t5 = t3*t3;
    t6 = P12*dtime;
    t7 = P23*dtime;
    t8 = P33*t3*(1.0/2.0);
    t9 = P13+t7+t8;
    t10 = t3*t9*(1.0/2.0);
    t11 = P13*t3*(1.0/2.0);
    t12 = w*w;
    t13 = dtime*t5*t12*(1.0/2.0E1);
    t14 = P22*dtime;
    t15 = P23*t3*(1.0/2.0);
    t16 = P12+t14+t15;
    t17 = dtime*t16;
    t18 = pos+t2+t4-xraw;
    t19 = v*v;
    t20 = P11+t6+t10+t11+t13+t17+t19;
    t21 = 1.0/t20;

    //A0[0][0] = pos+t2+t4-t18*t21*(P11+t6+t10+t11+t13+t17);
    //A0[1][0] = vel+acc*dtime-t18*t21*(P12+P13*dtime+t5*t12*(1.0/8.0)+t3*(P23+P33*dtime)*(1.0/2.0)+dtime*(P22+t7));
    //A0[2][0] = acc-t18*t21*(P13+t7+t8+dtime*t3*t12*(1.0/6.0));

    pos = pos+t2+t4-t18*t21*(P11+t6+t10+t11+t13+t17);
    vel = vel+acc*dtime-t18*t21*(P12+P13*dtime+t5*t12*(1.0/8.0)+t3*(P23+P33*dtime)*(1.0/2.0)+dtime*(P22+t7));
    acc = acc-t18*t21*(P13+t7+t8+dtime*t3*t12*(1.0/6.0));

    // P.cpp
    t2 = dtime*dtime;
    t3 = t2*t2;
    t4 = P12*dtime;
    t5 = P23*dtime;
    t6 = P33*t2*(1.0/2.0);
    t7 = P13+t5+t6;
    t8 = t2*t7*(1.0/2.0);
    t9 = P13*t2*(1.0/2.0);
    t10 = w*w;
    t11 = dtime*t3*t10*(1.0/2.0E1);
    t12 = P22*dtime;
    t13 = P23*t2*(1.0/2.0);
    t14 = P12+t12+t13;
    t15 = dtime*t14;
    t16 = P11+t4+t8+t9+t11+t15;
    t17 = v*v;
    t18 = P11+t4+t8+t9+t11+t15+t17;
    t19 = 1.0/t18;
    t20 = t16*t19;
    t21 = t20-1.0;
    t22 = P33*dtime;
    t23 = P23+t22;
    t24 = t3*t10*(1.0/8.0);
    t25 = dtime*t7;
    t26 = P12+t12+t13+t24+t25;
    t27 = dtime*t2*t10*(1.0/6.0);
    t28 = P13+t5+t6+t27;
    t29 = P13*dtime;
    t30 = P22+t5;
    t31 = dtime*t30;
    t32 = t2*t23*(1.0/2.0);
    t33 = P12+t24+t29+t31+t32;

    //A0[0][0] = -t16*t21;
    //A0[0][1] = -t21*t26;
    //A0[0][2] = -t21*t28;
    //A0[1][1] = P22+t5+dtime*t23+dtime*t2*t10*(1.0/3.0)-t19*t26*t33;
    //A0[1][2] = P23+t22+t2*t10*(1.0/2.0)-t19*t28*t33;
    //A0[2][2] = P33+dtime*t10-t19*(t28*t28);

    P11 = -t16*t21;
    P12 = -t21*t26;
    P13 = -t21*t28;
    P22 = P22+t5+dtime*t23+dtime*t2*t10*(1.0/3.0)-t19*t26*t33;
    P23 = P23+t22+t2*t10*(1.0/2.0)-t19*t28*t33;
    P33 = P33+dtime*t10-t19*(t28*t28);

    xfilt = pos;
    dxfilt = vel;

    if( data[0] != NULL ) data[0]->Data(dtime);
    if( data[1] != NULL ) data[1]->Data(xraw);
    if( data[2] != NULL ) data[2]->Data(xfilt);
    if( data[3] != NULL ) data[3]->Data(dxfilt);

    latency->After();
}

/******************************************************************************/

void KALMANFILTER::Calculate( double xraw, double &xfilt, double &dxfilt )
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

    Calculate(xraw,dtime,xfilt,dxfilt);
}

/******************************************************************************/

BOOL KALMANFILTER::DataSave( void )
{
int i;
BOOL ok;

    if( !OpenFlag )
    {
        return(FALSE);
    }

    printf("%s: Save()\n",ObjectName);
    for( ok=TRUE,i=0; (i < KALMANFILTER_DATA); i++ )
    {
    	if( data[i] != NULL )
    	{
            printf("%s: Save(File=%s)\n",ObjectName,data[i]->File());
            if( !data[i]->Save() )
            {
                ok = FALSE;
            }
        }
    }

    return(ok);
}

/******************************************************************************/

void KALMANFILTER::DataFree( void )
{
int i;

    for( i=0; (i < KALMANFILTER_DATA); i++ )
    {
    	if( data[i] != NULL )
    	{
            delete data[i];
            data[i] = NULL;
        }
    }
}

/******************************************************************************/

BOOL KALMANFILTER::Opened( void )
{
    return(OpenFlag);
}

/******************************************************************************/

void KALMANFILTER::Open( double kf_w, double kf_v, double kf_dt )
{
    Open(kf_w,kf_v,kf_dt,0);
}

/******************************************************************************/

void KALMANFILTER::Open( double kf_w, double kf_v, double kf_dt, int ditems )
{
int i;

    printf("%s: w=%.3lf v=%.3lf dt=%.3lf ditems=%d\n",ObjectName,kf_w,kf_v,kf_dt,ditems);

    // Don't open the object if parameters are null...
    if( (kf_w * kf_v * kf_dt) == 0.0 )
    {
        return;
    }

    if( ditems > 0 )
    {
        for( i=0; (i < KALMANFILTER_DATA); i++ )
        {
            data[i] = new DATAPROC(STR_stringf("%s-%d",ObjectName,i),ditems);
            printf("DATAPROC(name=%s,items=%d)\n",STR_stringf("%s-%d",ObjectName,i),ditems);
        }
    }

    w = kf_w;
    v = kf_v;
    dt = kf_dt;

    //R(1,1) = v*v;

    FirstFlag = TRUE;
    OpenFlag = TRUE;
}

/******************************************************************************/

void KALMANFILTER::Close( void )
{
BOOL ok;

    printf("%s: Close()\n",ObjectName);

    if( freq != NULL )
    {
        freq->Results();
    }

    if( latency != NULL )
    {
        latency->Results();
    }

    ok = DataSave();
    DataFree();

    OpenFlag = FALSE;
}

/******************************************************************************/

