/******************************************************************************/

#include <motor.h>

/******************************************************************************/

WINFIT::WINFIT( )
{
}

/******************************************************************************/

WINFIT::~WINFIT( )
{
int i;

    matrix_empty(Points);

    for( i=0; (i <= DOF); i++ )
    {
        if( data[i] != NULL )
        {
            delete data[i];
            data[i] = NULL;
        }
    }

    LogFree();
}

/******************************************************************************/

WINFIT::WINFIT( char *name, int window, int dof )
{
    Init(name,window,dof);
}

/******************************************************************************/

WINFIT::WINFIT( int window, int dof )
{
    Init(NULL,window,dof);
}

/******************************************************************************/

void WINFIT::Init( char *name, int window, int dof )
{
int i,j;

    for( i=0; (i <= WINFIT_DOF); i++ )
    {
        data[i] = NULL;
    }

    for( i=0; (i < WINFIT_DOF); i++ )
    {
        for( j=0; (j < WINFIT_LOG); j++ )
        {
            log[i][j] = NULL;
        }
    }

    // Object name...
    memset(ObjectName,0,STRLEN);
    if( name != NULL )
    {
        strncpy(ObjectName,name,STRLEN);
    }

    // Check window size...
    if( window <= 0 )
    {
        window = 1;
    }
    else
    if( window > WINFIT_SIZE )
    {
        window = WINFIT_SIZE;
    }

    Count = 0;
    Window = window;
    DOF = dof;

    Points.dim(window,dof);
    dValue.dim(dof,1);

    for( i=0; (i <= DOF); i++ )
    {
        data[i] = new DATAPROC(STR_stringf("%s[%d]",name,i));
    }

    for( i=0; (i < DOF); i++ )
    {
        for( j=0; (j <= WINFIT_LOG); j++ )
        {
            log[i][j] = new DATAPROC(STR_stringf("%s[%d][%d]",name,i,j));
        }
    }
}

/******************************************************************************/

int WINFIT::WindowSize( void )
{
    return(Window);
}

/******************************************************************************/

int WINFIT::DOFs( void )
{
    return(DOF);
}

/******************************************************************************/

void WINFIT::Reset( void )
{
int i;

    Points.zeros();
    dValue.zeros();

    for( i=0; (i < Window); i++ )
    {
        dtlist[i] = 0.0;
    }

    for( i=0; (i <= DOF); i++ )
    {
        if( data[i] != NULL )
        {
            data[i]->Reset();
        }
    }

    Count = 0;
}

/******************************************************************************/

void WINFIT::Point( matrix data, double dt )
{
double twin;
int i,index;
//matrix p,d;

    // Current index within window...
    index = (Count % Window);

    /*if( data.isrowvector() )
    {
        data = T(data);
    }*/

    // Assign data to current point within window...
    matrix_array_put(Points,T(data),index+1);
    dtlist[index] = dt;

   /* dValue.zeros();

    // Check if window is full...
    if( Primed() )
    {
        // Add up time steps for current window...
        for( twin=0.0,i=0; (i < Window); i++ )
        {
            index = (Count-i) % Window;
            twin += dtlist[index];
        }

        // Make sure time window isn't zero...
        if( twin != 0.0 )
        {
            // Extract oldest point & differentiate with new point...
            index = (Count+1) % Window;
            p = T(Points(index+1));
            d = data - p;
            //dValue = d / (twin / 1000.0);
            dValue = d / twin;
        }
    }*/

    Count++;
}

/******************************************************************************/

void WINFIT::Point( matrix data )
{
double dt=0;

    Point(data,dt);
}

/******************************************************************************/

void WINFIT::Point( double data, double dt )
{
    Point(scalar(data),dt);
}

/******************************************************************************/

void WINFIT::Point( double data )
{
double dt=0;

    Point(scalar(data),dt);
}

/******************************************************************************/

BOOL WINFIT::Primed( void )
{
BOOL flag=FALSE;

    if( Count > Window )
    {
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

BOOL WINFIT::dXdt( matrix &data )
{
    data.dim(DOF,1);

    if( !Primed() )
    {
        return(FALSE);
    }

    data = dValue;

    return(TRUE);
}

/******************************************************************************/

double WINFIT::GetValue( int dof, int index )
{
double value;

    if( dof == 0 )
    {
        value = dtlist[index];
    }
    else
    {
        value = Points(index+1,dof);
    }

    return(value);
}

/******************************************************************************/

BOOL WINFIT::GetWindow( int dof, matrix &data )
{
int i,index;

    data.dim(Window,1);

    if( !Primed() )
    {
        return(FALSE);
    }

    for( i=1; (i <= Window); i++ )
    {
        index = (Count-i) % Window;
        data(i,1) = GetValue(dof,index);
    }

    return(TRUE);
}

/******************************************************************************/

void WINFIT::Calculate( int dof )
{
int i,index;

    data[dof]->Reset();

    if( !Primed() )
    {
        return;
    }

    for( i=1; (i <= Window); i++ )
    {
        index = (Count-i) % Window;
        data[dof]->Data(GetValue(dof,index));
    }

    data[dof]->Calculate();
}

/******************************************************************************/

double WINFIT::Mean( int dof )
{
double value=0.0;

    Calculate(dof);
    value = data[dof]->Mean();

    return(value);
}

/******************************************************************************/

double WINFIT::Max( int dof )
{
double value=0.0;

    Calculate(dof);
    value = data[dof]->Max();

    return(value);
}

/******************************************************************************/

double WINFIT::Min( int dof )
{
double value=0.0;

    Calculate(dof);
    value = data[dof]->Min();

    return(value);
}

/******************************************************************************/

double WINFIT::SignedMax( int dof )
{
double value=0.0;

    Calculate(dof);
    value = data[dof]->SignedMax();

    return(value);
}

/******************************************************************************/

double WINFIT::Variance( int dof )
{
double value=0.0;

    Calculate(dof);
    value = data[dof]->Variance();

    return(value);
}

/******************************************************************************/

double WINFIT::SD( int dof )
{
double value=0.0;

    Calculate(dof);
    value = data[dof]->SD();

    return(value);
}

/******************************************************************************/

void WINFIT::MatrixDim( matrix &mtx )
{
    if( mtx.exist(DOF,1) )
    {
        return;
    }
    else
    if( mtx.exist(1,DOF) )
    {
        return;
    }

    mtx.dim(1,DOF);
}

/******************************************************************************/

void WINFIT::MatrixSet( matrix &mtx, int dof, double value )
{
    if( mtx.exist(dof,1) )
    {
        mtx(dof,1) = value;
    }
    else
    if( mtx.exist(1,dof) )
    {
        mtx(1,dof) = value;
    }
}

/******************************************************************************/

void WINFIT::Mean( matrix &mtx )
{
int dof;

    MatrixDim(mtx);

    for( dof=1; (dof <= DOF); dof++ )
    {
        MatrixSet(mtx,dof,Mean(dof));
    }
}

/******************************************************************************/

void WINFIT::Max( matrix &mtx )
{
int dof;

    MatrixDim(mtx);

    for( dof=1; (dof <= DOF); dof++ )
    {
        MatrixSet(mtx,dof,Max(dof));
    }
}

/******************************************************************************/

void WINFIT::Min( matrix &mtx )
{
int dof;

    MatrixDim(mtx);

    for( dof=1; (dof <= DOF); dof++ )
    {
        MatrixSet(mtx,dof,Min(dof));
    }
}

/******************************************************************************/

void WINFIT::SignedMax( matrix &mtx )
{
int dof;

    MatrixDim(mtx);

    for( dof=1; (dof <= DOF); dof++ )
    {
        MatrixSet(mtx,dof,SignedMax(dof));
    }
}

/******************************************************************************/

void WINFIT::Variance( matrix &mtx )
{
int dof;

    MatrixDim(mtx);

    for( dof=1; (dof <= DOF); dof++ )
    {
        MatrixSet(mtx,dof,Variance(dof));
    }
}

/******************************************************************************/

void WINFIT::SD( matrix &mtx )
{
int dof;

    MatrixDim(mtx);

    for( dof=1; (dof <= DOF); dof++ )
    {
        MatrixSet(mtx,dof,SD(dof));
    }
}

/******************************************************************************/

DATAPROC *WINFIT::Data( int dof )
{
    return(data[dof]);
}

/******************************************************************************/

BOOL WINFIT::QuadraticFitOld( int dof, double &pos, double &vel, double &acc )
{
matrix Y,X,P,T,XT;
int i;
double ymean;

    if( !Primed() ) 
    {
        return(FALSE);
    }

    GetWindow(dof,Y);
    GetWindow(0,T);

    matrix_dim(X,WindowSize(),3);

    ymean = mean(Y);
    Y = Y - ymean;

    for( i=1; (i <= X.rows()); i++ )
    {
        if( i == 1 )
        {
            X(i,2) = 0.0;
        }
        else
        {
            X(i,2) = X(i-1,2) - T(i-1,1);
        }

        X(i,1) = 1.0;
        X(i,3) = X(i,2) * X(i,2);
    }

    matrix_transpose(XT,X);
    P = inv(XT * X) * XT * Y;

    // Position...
    pos = P(1,1) + ymean;

    // Velocity...
    //vel = P(2,1) * 1000.0;
    vel = P(2,1);

    // Acceleration...
    //acc = P(3,1) * 2.0 * 1000.0 * 1000.0;
    acc = P(3,1) * 2.0;

    return(TRUE);
}

/******************************************************************************/

TIMER_Interval WINFIT_L("",10000);
BOOL WINFIT::QuadraticFit( int dof, double &pos, double &vel, double &acc )
{
static matrix X,Y;
static double x,y,sx,sy,sx2,sxy,sx2y,sx3,sx4;
static double ex2,exy,ex2y,ex3,ex4,w;
static double a,b,c;
static int i,n;

//WINFIT_L.Before();
    if( !Primed() ) 
    {
        return(FALSE);
    }

    n = WindowSize();

    GetWindow(0,X);    // time-step
    GetWindow(dof,Y);  // positions

//WINFIT_L.Before();
    x = 0.0;
    sx = 0.0;
    sy = 0.0;
    sx2 = 0.0;
    sxy = 0.0;
    sx2y = 0.0;
    sx3 = 0.0;
    sx4  = 0.0;

    for( i=1; (i <= n); i++ )
    { 
        // accumulate time-steps to get continuous time across window.
        if( i == 1 )
        {
            x = 0.0;
        }
        else
        {
            x -= X(i-1,1);
        }

        y = Y(i,1); // position

        sx += x;
        sy += y;
        sx2 += x*x;
        sxy += x*y;
        sx2y += x*x*y;
        sx3 += x*x*x;
        sx4 += x*x*x*x;
    }

    ex2 = sx2-sx*sx/n;
    exy = sxy-sx*sy/n;
    ex2y = sx2y-sx2*sy/n;
    ex3 = sx3-sx*sx2/n;
    ex4 = sx4-sx2*sx2/n;

    w = (ex2*ex4-ex3*ex3);
 
    a = (ex2y*ex2-exy*ex3)/w; // 2 x accelaion
    b = (exy*ex4-ex2y*ex3)/w; // velocity
    c = sy/n-b*sx/n-a*sx2/n;  // position

    pos = c;
    vel = b;
    acc = c;

//WINFIT_L.After();

/*if( WINFIT_L.Count() >= 1000 )
{
printf("tmean=%.3lf±%.3lfusec\n",WINFIT_L.IntervalMean()*1000.0,WINFIT_L.IntervalSD()*1000.0);
WINFIT_L.Reset();
}*/

    return(TRUE);
}

/******************************************************************************/

BOOL WINFIT::QuadraticFit( double &pos, double &vel, double &acc )
{
BOOL flag;

    flag = QuadraticFit(1,pos,vel,acc);

    return(flag);
}

/******************************************************************************/

BOOL WINFIT::QuadraticFit( double &vel, double &acc )
{
double pos;
BOOL flag;

    flag = QuadraticFit(1,pos,vel,acc);

    return(flag);
}

/******************************************************************************/

BOOL WINFIT::QuadraticFit( double &vel )
{
double pos,acc;
BOOL flag;

    flag = QuadraticFit(1,pos,vel,acc);

    return(flag);
}

/******************************************************************************/

BOOL WINFIT::QuadraticFit( matrix &pos, matrix &vel, matrix &acc )
{
int dof;
double p,v,a;
BOOL flag;

    MatrixDim(pos);
    MatrixDim(vel);
    MatrixDim(acc);

    for( flag=TRUE,dof=1; ((dof <= DOFs()) && flag); dof++ )
    {
        flag = QuadraticFit(dof,p,v,a);

        MatrixSet(pos,dof,p);
        MatrixSet(vel,dof,v);
        MatrixSet(acc,dof,a);
    }

    return(flag);
}

/******************************************************************************/

BOOL WINFIT::QuadraticFit( matrix &vel, matrix &acc )
{
matrix pos;
BOOL flag;

    flag = QuadraticFit(pos,vel,acc);

    return(flag);
}

/******************************************************************************/

BOOL WINFIT::QuadraticFit( matrix &vel )
{
matrix pos,acc;
BOOL flag;

    flag = QuadraticFit(pos,vel,acc);

    return(flag);
}

/******************************************************************************/

BOOL WINFIT::LogSave( void )
{
int i,j;
BOOL ok;

    for( ok=TRUE,i=0; (i < DOF); i++ )
    {
        for( j=0; (j < WINFIT_LOG); j++ )
        {
            if( log[i][j] != NULL )
            {
                printf("%s: Save(File=%s)\n",ObjectName,log[i][j]->File());
                if( !log[i][j]->Save() )
                {
                    ok = FALSE;
                }
            }
        }
    }

    return(ok);
}

/******************************************************************************/

void WINFIT::LogFree( void )
{
int i,j;

    for( i=0; (i < DOF); i++ )
    {
        for( j=0; (j < WINFIT_LOG); j++ )
        {
            if( log[i][j] != NULL )
            {
                delete log[i][j];
                log[i][j] = NULL;
            }
        }
    }
}

/******************************************************************************/

void WINFIT::LogPoint( int dof, double dt, double x_raw, double xfit, double dxfit, double ddxfit )
{
int i,j;
double value[WINFIT_LOG];

    i = dof;

    value[0] = dt;
    value[1] = x_raw;
    value[2] = xfit;
    value[3] = dxfit;
    value[4] = ddxfit;

    for( j=0; (j < WINFIT_LOG); j++ )
    {
        if( log[i][j] != NULL )
        {
            log[i][j]->Data(value[j]);
        }
    }
}

/******************************************************************************/

