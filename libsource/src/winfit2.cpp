/******************************************************************************/

#include <motor.h>

/******************************************************************************/

WINFIT2::WINFIT2( )
{
}

/******************************************************************************/

WINFIT2::~WINFIT2( )
{
    LogFree();
}

/******************************************************************************/

WINFIT2::WINFIT2( char *name, int window, BOOL logflag )
{
    Init(name,window,logflag);
}

/******************************************************************************/

WINFIT2::WINFIT2( int window )
{
    Init(NULL,window,FALSE);
}

/******************************************************************************/

void WINFIT2::Init( char *name, int window, BOOL logflag )
{
int i;

    for( i=0; (i < WINFIT2_LOG); i++ )
    {
        log[i] = NULL;
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
    if( window > WINFIT2_SIZE )
    {
        window = WINFIT2_SIZE;
    }

    Count = 0;
    Window = window;
    IndexHead = 0;

    if( logflag )
    {
        for( i=0; (i <= WINFIT2_LOG); i++ )
        {
            log[i] = new DATAPROC(STR_stringf("%s[%d]",name,i));
        }
    }
}

/******************************************************************************/

int WINFIT2::WindowSize( void )
{
    return(Window);
}

/******************************************************************************/

void WINFIT2::Reset( void )
{
    LogReset();

    Count = 0;
    IndexHead = 0;
}

/******************************************************************************/

void WINFIT2::Point( double data, double dt )
{
int index;

    index = (Count % Window);

    xlist[index] = data;
    dtlist[index] = dt;

    IndexHead = index;
   
    Count++;
}

/******************************************************************************/

BOOL WINFIT2::Primed( void )
{
BOOL flag=FALSE;

    if( Count >= Window )
    {
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

int WINFIT2::GetWindow( double *x, double *dt )
{
int i,index;

    if( !Primed() )
    {
        return(0);
    }

    index = IndexHead;

    for( i=0; (i < Window); i++ )
    {
        x[i] = xlist[index];
        dt[i] = dtlist[index];

        if( --index < 0 )
        {
            index = (Window-1);
        }
    }

    return(Window);
}

/******************************************************************************/

BOOL WINFIT2::QuadraticFit( double &pos, double &vel, double &acc )
{
static double XX[WINFIT2_SIZE],X[WINFIT2_SIZE],Y[WINFIT2_SIZE];
static double x,y,sx,sy,sx2,sxy,sx2y,sx3,sx4;
static double ex2,exy,ex2y,ex3,ex4,w;
static double a,b,c;
static int i,n;
static double xmean,ymean,xstd,ystd,tnow;
static BOOL norm_flag=FALSE;

    if( !Primed() ) 
    {
        return(FALSE);
    }

    n = GetWindow(Y,XX); // Y = position, XX = dt

    // X = continuous time across window.
    for( i=0; (i < n); i++ )
    { 
        if( i == 0 )
        {
            X[i] = 0.0;
        }
        else
        {
            X[i] = X[i-1] - XX[i-1];
        }
    }

    if( norm_flag )
    {
        // Calculate mean.
        xmean = 0.0;
        ymean = 0.0;

        for( i=0; (i < n); i++ )
        {
            xmean += X[i];
            ymean += Y[i];
        }

        xmean /= (double)n;
        ymean /= (double)n;

        // Calculate SD.
        xstd = 0.0;
        ystd = 0.0;

        for( i=0; (i < n); i++ )
        {
            xstd += (xmean - X[i]) * (xmean - X[i]);
            ystd += (ymean - Y[i]) * (ymean - Y[i]);
        }

        xstd = sqrt(xstd/(double)(n-1));
        ystd = sqrt(ystd/(double)(n-1));
        ystd += 1.0; // to avoid zero.

        // Normalize.
        for( i=0; (i < n); i++ )
        {
            X[i] = (X[i] - xmean) / xstd;
            Y[i] = (Y[i] - ymean) / ystd;
        }        
    }

    x = 0.0;
    sx = 0.0;
    sy = 0.0;
    sx2 = 0.0;
    sxy = 0.0;
    sx2y = 0.0;
    sx3 = 0.0;
    sx4  = 0.0;

    for( i=0; (i < n); i++ )
    { 
        // accumulate time-steps to get continuous time across window.
        /*if( i == 0 )
        {
            x = 0.0;
        }
        else
        {
            x -= X[i-1];
        }*/

        x = X[i]; // time
        y = Y[i]; // position

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

    //tnow = 0.0005;
    tnow = 0.0;

    if( norm_flag )
    {
        tnow = (tnow-xmean)/xstd;
        pos = (ystd * ((a*tnow*tnow) + (b*tnow) + c)) + ymean;        
        vel = (ystd * ((2.0*a*tnow) + b)) / xstd;        
        acc = 0;
    }
    else
    {
        pos = c;
        vel = b;
        acc = c;
    }

    pos = Y[0];

    //vel = (Y[0] - Y[1]) / XX[0];
    vel = (Y[0] - Y[2]) / (XX[0]+XX[1]);
    //vel = (Y[0] - Y[3]) / (XX[0]+XX[1]+XX[2]);

    acc = 0.0;

    return(TRUE);
}

/******************************************************************************/

BOOL WINFIT2::QuadraticFit( double &vel, double &acc )
{
double pos;
BOOL flag;

    flag = QuadraticFit(pos,vel,acc);

    return(flag);
}

/******************************************************************************/

BOOL WINFIT2::QuadraticFit( double &vel )
{
double pos,acc;
BOOL flag;

    flag = QuadraticFit(pos,vel,acc);

    return(flag);
}

/******************************************************************************/

BOOL WINFIT2::LogSave( void )
{
int i;
BOOL ok=TRUE;

    for( i=0; (i < WINFIT2_LOG); i++ )
    {
        if( log[i] != NULL )
        {
            printf("%s: Save(File=%s)\n",ObjectName,log[i]->File());
            if( !log[i]->Save() )
            {
                ok = FALSE;
            }
        }
    }

    return(ok);
}

/******************************************************************************/

void WINFIT2::LogFree( void )
{
int i;

    for( i=0; (i < WINFIT2_LOG); i++ )
    {
        if( log[i] != NULL )
        {
            delete log[i];
            log[i] = NULL;
        }
    }
}

/******************************************************************************/

void WINFIT2::LogReset( void )
{
int i;

    for( i=0; (i < WINFIT2_LOG); i++ )
    {
        if( log[i] != NULL )
        {
            log[i]->Reset();
        }
    }
}

/******************************************************************************/

void WINFIT2::LogPoint( double dt, double x_raw, double xfit, double dxfit, double ddxfit )
{
int i;
double value[WINFIT2_LOG];

    value[0] = dt;
    value[1] = x_raw;
    value[2] = xfit;
    value[3] = dxfit;
    value[4] = ddxfit;

    for( i=0; (i < WINFIT2_LOG); i++ )
    {
        if( log[i] != NULL )
        {
            log[i]->Data(value[i]);
        }
    }
}

/******************************************************************************/

