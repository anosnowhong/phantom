/******************************************************************************/

void range( int *value, int minmax[] )
{
    *value = range(*value,minmax);
}

/******************************************************************************/

void range( int *value, int minimum, int maximum )
{
    *value = range(*value,minimum,maximum);
}

/******************************************************************************/

void range( int *value, int absolute )
{
    *value = range(*value,absolute);
}

/******************************************************************************/

void range( double *value, double minmax[] )
{
    *value = range(*value,minmax);
}

/******************************************************************************/

void range( double *value, double minimum, double maximum )
{
    *value = range(*value,minimum,maximum);
}

/******************************************************************************/

void range( double *value, double absolute )
{
    *value = range(*value,absolute);
}

/******************************************************************************/

int range( int value, int minmax[] )
{
    value = range(value,minmax[0],minmax[1]);

    return(value);
}

/******************************************************************************/

int range( int value, int minimum, int maximum )
{
    if( value < minimum )
    {
        value = minimum;
    }
    else
    if( value > maximum )
    {
        value = maximum;
    }

    return(value);
}

/******************************************************************************/

double range( double value, double minmax[] )
{
    value = range(value,minmax[0],minmax[1]);

    return(value);
}

/******************************************************************************/

double range( double value, double minimum, double maximum )
{
    if( value < minimum )
    {
        value = minimum;
    }
    else
    if( value > maximum )
    {
        value = maximum;
    }

    return(value);
}

/******************************************************************************/

int range( int value, int absolute )
{
int minimum,maximum;

    minimum = -abs(absolute);
    maximum = +abs(absolute);

    return(range(value,minimum,maximum));
}

/******************************************************************************/

double range( double value, double absolute )
{
double minimum,maximum;

    minimum = -fabs(absolute);
    maximum = +fabs(absolute);

    return(range(value,minimum,maximum));
}

/******************************************************************/

void randomize( UINT seed )
{
static BOOL done=FALSE;

    // Seed only once...
    if( done )
    {
        return;
    }

    srand(seed);
    (void)rand();

    done = TRUE;
}

/******************************************************************/

void randomize( void )
{
timeb t;
double seconds;
UINT seed;

    ftime(&t);
    seconds = (double)t.time + (double)t.millitm / 1000.0;
    seed = (UINT)floor(seconds * 10.0);
  
    randomize(seed);
}

/******************************************************************/

double drand( double a, double b )
{
double d;

    d = (double)rand() / RAND_MAX;
    d = (b-a) * d;
    d += a;

    return(d);
}

/******************************************************************/

int irand( int a, int b )
{
double d;
int i;

    d = drand((double)a,0.99999+(double)b);
    d = floor(d);
    i = (int)d;

    return(i);
}

/******************************************************************/

BOOL randomflip( double bias )
{
double d;
BOOL flip;

    d = drand(0.0,1.0);
    flip = (d < bias);

    return(flip);
}

/******************************************************************/

double  gauss( double mean, double sd )
{
  double x1,y1,x2,y2,d,e; 
  x1=drand(0.0,1.0);
  x2=drand(0.0,1.0);
  d=drand(0.0,1.0);
  
  y1=(cos(2*M_PI*x2))*sqrt(-2*log(x1));
  y2=(sin(2*M_PI*x2))*sqrt(-2*log(x1));
  
  e=y1;
  if(d<0.5) e=y2;
  return ((e*sd)+mean);
}

/******************************************************************/ 

double  reciprocal( double x )
{
  return 1.0/x;
}

/******************************************************************/ 

double  logistic( double x )
{
  return 1.0/(1+exp(-x));
}

/******************************************************************/ 

double  sgn( double x )
{
  if(x<0) return -1.0;
  else if (x>0) return 1.0;
  else return 0.0;
}

/******************************************************************/

double  deriv_logistic( double x )
{
  double p;
  p=logistic(x);
  return p*(1-p);
}

/************************************************************/

double  roundto( double x, int dp )
{
int i;
double p,y;

    for( p=1.0,i=0; (i < dp); i++ )
    {
        p *= 10.0;
    }
   
    y = floor((x * p) + 0.5);
    y = y / p;

    return(y);
}

/************************************************************/

double RandomNormal( void )
{
double d,mu=0.0,sd=1.0;

    d = RandomGaussian(mu,sd);
    
    return(d);
}

/************************************************************/

void RandomGaussian( double mu, double sd, int points, int n, matrix &mtx )
{
static double x[RANDOM_GAUSSIAN_POINTS+1];
static double y[RANDOM_GAUSSIAN_POINTS+1];
double r,d,xstep;
int i,j;

    mtx.dim(n,1);

    if( sd == 0.0 )
    {
        mtx.fill(mu);
        return;
    }

    if( (points == 0) || (points > RANDOM_GAUSSIAN_POINTS) )
    {
        points = RANDOM_GAUSSIAN_POINTS;
    }

    x[0] = -1000.0;
    xstep = (2.0*fabs(x[0]))/(double)points;

    for( i=1; (i <= points); i++ )
    {
    	x[i] = x[i-1] + xstep;
    }

    for( i=0; (i <= points); i++ )
    {
    	x[i] *= sd;
    	//y[i] = (1.0 / (sd * sqrt(2.0*PI))) * exp(-((x[i]-mu)*(x[i]-mu)) / (2.0*(sd*sd)));
    	y[i] = (1.0 / (sd * sqrt(2.0*PI))) * exp(-(x[i]*x[i]) / (2.0*(sd*sd)));
    	if( i > 0 )
    	{
            y[i] += y[i-1];
        }
    }

    for( i=0; (i <= points); i++ )
    {
        y[i] /= y[points];
    }

    for( j=1; (j <= n); j++ )
    {
        do
        {
            r = RandomUniform();
        }
        while( (r == 0.0) || (r == 1.0) );

        for( i=0; (i <= points); i++ )
        {
            if( y[i] > r )
            {
                break;
            }
        }

        if( i > points )
        {
            i = points;
        }

        mtx(j,1) = mu + x[i];
    }
}

/************************************************************/

void RandomGaussian( double mu, double sd, int n, matrix &mtx )
{
    RandomGaussian(mu,sd,0,n,mtx);
}

/************************************************************/

double RandomGaussian( double mu, double sd, int points )
{
matrix mtx;

    RandomGaussian(mu,sd,points,1,mtx);
    return(mtx(1,1));
}

/************************************************************/

double RandomGaussian( double mu, double sd )
{
matrix mtx;

    RandomGaussian(mu,sd,1,mtx);
    return(mtx(1,1));
}

/************************************************************/

double RandomUniform( double a, double b )
{
double d;

    d = RandomUniform();
    d = (b-a) * d;
    d += a;

    return(d);
}

/************************************************************/

double RandomUniform( void )
{
double d;

    d = (double)rand() / RAND_MAX;

    return(d);
}

/************************************************************/

int RandomInteger( int a, int b )
{
double d;
int i;

    d = RandomUniform();
    i = floor((0.99999+b-a) * d);
    i += a;

    return(i);
}

/************************************************************/

BOOL RandomCoinToss( double bias )
{
double d;
BOOL heads;

    d = RandomUniform();
    heads = (d < bias);

    return(heads);
}

/************************************************************/

BOOL UndefinedDouble( double value )
{
BOOL flag=TRUE;

    if( (fabs(value) >= 10.0) || (fabs(value) < 10.0) )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL UndefinedDouble( matrix &mtx )
{
BOOL flag;
int i,j;

    for( flag=FALSE,i=1; ((i <= mtx.rows()) && !flag); i++ )
    {
        for( j=1; ((j <= mtx.cols()) && !flag); j++ )
        {
            flag = UndefinedDouble(mtx(i,j));
        }
    }

    return(flag);
}

/******************************************************************************/

