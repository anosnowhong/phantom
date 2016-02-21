/******************************************************************************/

#include <motor.h>

/******************************************************************************/

#define DATAPROC_errorf  printf
#define DATAPROC_debugf  printf
#define DATAPROC_messgf  printf

/******************************************************************************/

DATAPROC::DATAPROC( char *name, int items, BYTE flags )
{
    // Initialize object...
    if( Init(name,items,flags) )
    {
        Reset();
    }
}

/******************************************************************************/

DATAPROC::DATAPROC( char *name, int items )
{
    // Initialize object...
    if( Init(name,items,DATAPROC_FLAG_NONE) )
    {
        Reset();
    }
}

/******************************************************************************/

DATAPROC::DATAPROC( char *name )
{
    // Initialize object...
    if( Init(name,DATAPROC_ITEM_MAX,DATAPROC_FLAG_NONE) )
    {
        Reset();
    }
}

/******************************************************************************/

DATAPROC::DATAPROC( void )
{
    // Initialize object...
    if( Init(NULL,DATAPROC_ITEM_MAX,DATAPROC_FLAG_NONE) )
    {
        Reset();
    }
}

/******************************************************************************/

DATAPROC::~DATAPROC( void )
{
    // Free memory...
    if( data != NULL )
    {
        free(data);
        data = NULL;
    }
}

/******************************************************************************/

BOOL DATAPROC::Init( char *name, int items, BYTE flags )
{
    Init();

    if( (data=(double *)calloc(items,sizeof(double))) == NULL )
    {
        DATAPROC_errorf("Init(name=%s,items=%d) Cannot allocate memory.\n",name,items);
        return(FALSE);
    }

    if( name != NULL )
    {
        strncpy(ObjectName,name,STRLEN);
    }

    Flags = flags;
    MaxItems = items;

    Reset();

    return(TRUE);
}

/******************************************************************************/

void DATAPROC::Init( void )
{
    memset(ObjectName,0,STRLEN);
    MaxItems = 0;
    Flags = DATAPROC_FLAG_NONE;
    data = NULL;
}

/******************************************************************************/

void DATAPROC::Reset( void )
{
    count = 0;
    index = 0;
    tail = 0;
    DoneMean = TRUE;
    DoneVariance = TRUE;
    error = DATAPROC_ERROR_NONE;
    total = 0.0;
    mean = 0.0;
    maximum = 0.0;
    minimum = 0.0;
    signedmax = 0.0;
    sd = 0.0;
}

/******************************************************************************/

void DATAPROC::Data( double value )
{
int index;

    if( Full() )
    {
        error = DATAPROC_ERROR_MAX;
        return;
    }

    index = Index(count);

    data[index] = value;

    if( count < MaxItems )
    {
        count++;
    }
    else
    {
        tail = Wrap(++tail);
    }

    // Reset various calculation flags...
    DoneMean = FALSE;
    DoneVariance = FALSE;

    // If circular, return without further processing...
    if( (Flags & DATAPROC_FLAG_CIRCULAR) )
    {
        return;
    }

    // Calculate simple statistics (mean, etc.) on the fly...

    total += value;
    mean = total / (double)count;
 
    if( (value > maximum) || (count == 1) )
    {
        maximum = value;
    }

    if( (value < minimum) || (count == 1) )
    {
        minimum = value;
    }

    if( (fabs(value) > fabs(signedmax)) || (count == 1) )
    {
        signedmax = value;
    }

    range = maximum - minimum;

    // Set mean calculation flag.
    DoneMean = TRUE;
}

/******************************************************************************/

void DATAPROC::Data( float value )
{
    Data((double)value);
}

/******************************************************************************/

void DATAPROC::Data( long value )
{
    Data((double)value);
}

/******************************************************************************/

void DATAPROC::Data( int value )
{
    Data((double)value);
}

/******************************************************************************/

BOOL DATAPROC::Item( int item )
{
BOOL ok;

    ok = (item >= 0) && (item < count);

    if( !ok )
    {
        error = DATAPROC_ERROR_ITEM;
    }

    return(ok);
}

/******************************************************************************/

int DATAPROC::Wrap( int index )
{
    if( index >= MaxItems )
    {
        index -= MaxItems;
    }

    return(index);
}

/******************************************************************************/

int DATAPROC::Index( int item )
{
int index;

    index = Wrap(tail+item);

    return(index);
}

/******************************************************************************/

BOOL DATAPROC::Value( int item, double &D )
{
BOOL ok;
int index;

    if( (ok=Item(item)) )
    {
        index = Index(item);
        D = data[index];
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Value( int item, float &F )
{
BOOL ok;
double value;

    if( (ok=Value(item,value)) )
    {
        F = (float)value;
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Value( int item, long &L )
{
BOOL ok;
double value;

    if( (ok=Value(item,value)) )
    {
        L = (long)value;
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Value( int item, int &I )
{
BOOL ok;
double value;

    if( (ok=Value(item,value)) )
    {
        I = (int)value;
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Get( int func, double &D )
{
BOOL ok;
int next=0;

    switch( func )
    {
        case DATAPROC_GET_FIRST :
           next = 0;
           break;

        case DATAPROC_GET_LAST :
           next = count-1;
           break;

        case DATAPROC_GET_NEXT :
           next = index+1;
           break;

        case DATAPROC_GET_PREVIOUS :
           next = index-1;
           break;
    }

    if( (ok=Value(next,D)) )
    {
        index = next;
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Get( int func, float &F )
{
BOOL ok;
double value;

    if( (ok=Get(func,value)) )
    {
        F = (float)value;
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Get( int func, long &L )
{
BOOL ok;
double value;

    if( (ok=Get(func,value)) )
    {
        L = (long)value;
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Get( int func, int &I )
{
BOOL ok;
double value;

    if( (ok=Get(func,value)) )
    {
        I = (int)value;
    }

    return(ok);
}

/******************************************************************************/

void DATAPROC::GetVector( matrix &V )
{
int rows,item;

    rows = Count();

    if( rows == 0 )
    {
        return;
    }

    V.dim(rows,1);

    for( item=0; (item < rows); item++ )
    {
        V(item+1,1) = ValueD(item);
    }
}

/******************************************************************************/

double DATAPROC::ValueD( int item )
{
double value=0.0;
BOOL ok;

    ok = Value(item,value);

    return(value);
}

/******************************************************************************/

float DATAPROC::ValueF( int item )
{
float value;

    value = (float)ValueD(item);

    return(value);
}

/******************************************************************************/

long DATAPROC::ValueL( int item )
{
long value;

    value = (long)ValueD(item);

    return(value);
}

/******************************************************************************/

int DATAPROC::ValueI( int item )
{
int value;

    value = (int)ValueD(item);

    return(value);
}

/******************************************************************************/

double DATAPROC::GetD( int func )
{
double value=0.0;
BOOL ok;

    ok = Get(func,value);

    return(value);
}

/******************************************************************************/

float DATAPROC::GetF( int func )
{
float value;

    value = (float)GetD(func);

    return(value);
}

/******************************************************************************/

long DATAPROC::GetL( int func )
{
long value;

    value = (long)GetD(func);

    return(value);
}

/******************************************************************************/

int DATAPROC::GetI( int func )
{
int value;

    value = (int)GetD(func);

    return(value);
}

/******************************************************************************/

BOOL DATAPROC::Error( BYTE &code )
{
    code = error;
    error = DATAPROC_ERROR_NONE;

    return(code != DATAPROC_ERROR_NONE);
}

/******************************************************************************/

BOOL DATAPROC::Error( STRING text )
{
BYTE code;
BOOL flag;

    flag = Error(code);
    strncpy(text,STR_stringf("DATAPROC Error %d.",code),STRLEN);

    return(flag);
}

/******************************************************************************/

void DATAPROC::Calculate( void )
{
    CalculateMean();
    CalculateVariance();
}

/******************************************************************************/

void DATAPROC::CalculateMean( void )
{
int  item;
double value;

    if( DoneMean )
    {
        return;
    }

    total = 0.0;
    mean = 0.0;
    maximum = 0.0;
    minimum = 0.0;
    signedmax = 0.0;

    for( item=0; (item < count); item++ )
    {
        value = ValueD(item);
        total += value;

        if( (value > maximum) || (count == 1) )
        {
            maximum = value;
        }

        if( (value < minimum) || (count == 1) )
        {
            minimum = value;
        }

        if( (fabs(value) > fabs(signedmax)) || (count == 1) )
        {
            signedmax = value;
        }
    }

    if( count > 0 )
    {
        mean = total / (double)count;
    }

    range = maximum - minimum;

    DoneMean = TRUE;
}

/******************************************************************************/

void DATAPROC::CalculateVariance( void )
{
int  item;
double value,d,vtot;

    CalculateMean();

    if( DoneVariance )
    {
        return;
    }

    for( vtot=0.0,item=0; (item < count); item++ )
    {
        d = ValueD(item) - mean;
        vtot += (d * d);
    }

    variance = 0.0;
    sd = 0.0;

    if( count > 1 )
    {
        variance = vtot / (double)(count-1);
        sd = sqrt(variance);
    }

    DoneVariance = TRUE;
}


/******************************************************************************/

void DATAPROC::CalculateMedian( void )
{
matrix D,S;
int items,i,middle;

    items = Count();

    if( items == 1 )
    {
        median = ValueD(0);
        return;
    }

    D.dim(1,items);

    for( i=0; (i < items); i++ )
    {
        D(1,i+1) = ValueD(i);
    }

    matrix_sort(S,D);
 
    middle = items / 2;

    if( (items % 2) == 1 )
    {
        median = S(1,middle+1);
    }
    else
    {
        median = (S(1,middle)+S(1,middle+1)) / 2.0;
    }
}

/******************************************************************************/

char *DATAPROC::Name( void )
{
    return(ObjectName);
}

/******************************************************************************/

int DATAPROC::MaxData( void )
{
    return(MaxItems);
}

/******************************************************************************/

int DATAPROC::Count( void )
{
    return(count);
}

/******************************************************************************/

int DATAPROC::Tail( void )
{
    return(tail);
}

/******************************************************************************/

int DATAPROC::Full( void )
{
BOOL full=TRUE;

    if( Flags & DATAPROC_FLAG_CIRCULAR )
    {
        full = FALSE;
    }
    else
    if( count < MaxItems )
    {
        full = FALSE;
    }

    return(full);
}

/******************************************************************************/

double DATAPROC::Sum( void )
{
    CalculateMean();

    return(total);
}

/******************************************************************************/

double DATAPROC::Mean( void )
{
    CalculateMean();

    return(mean);
}

/******************************************************************************/

double DATAPROC::Median( void )
{
    CalculateMedian();

    return(median);
}

/******************************************************************************/

double DATAPROC::Max( void )
{
    CalculateMean();

    return(maximum);
}

/******************************************************************************/

double DATAPROC::Min( void )
{
    CalculateMean();

    return(minimum);
}

/******************************************************************************/

double DATAPROC::SignedMax( void )
{
    CalculateMean();

    return(signedmax);
}

/******************************************************************************/

double DATAPROC::Range( void )
{
    CalculateMean();
 
    return(range);
}

/******************************************************************************/

double DATAPROC::Variance( void )
{
    CalculateVariance();

    return(variance);
}

/******************************************************************************/

double DATAPROC::SD( void )
{
    CalculateVariance();

    return(sd);
}

/******************************************************************************/

char *DATAPROC::File( void )
{
static STRING file;

    strncpy(file,Name(),STRLEN);

    if( STR_pack(file) == 0 )
    {
        DATAPROC_errorf("File() Empty file name.\n");
        strcpy(file,"DATAPROC");
    }

    strncat(file,".DAT",STRLEN);

    return(file);
}

/******************************************************************************/

#define DATAPROC_STEP_OPEN  0
#define DATAPROC_STEP_IO    1
#define DATAPROC_STEP_CLOSE 2

char *DATAPROC_StepText[] = { "Open","I/O","Close" };
char *DATAPROC_FuncText[] = { "Load","Save" };
char *DATAPROC_TypeText[] = { "ASCII","double","float","long","int" };

/******************************************************************************/

BOOL DATAPROC::File( char *file, int func, int type )
{
FILE *FP=NULL;
BOOL ok;
int step;

    if( (func == DATAPROC_FUNC_SAVE) && (Count() == 0) )
    {
        return(TRUE);
    }

    for( ok=TRUE,step=DATAPROC_STEP_OPEN; ((step <= DATAPROC_STEP_CLOSE) && ok); step++ )
    {
        switch( step )
        {
            case DATAPROC_STEP_OPEN :
               ok = ((FP=fopen(file,"w+b")) != NULL);
               break;

            case DATAPROC_STEP_IO :
               switch( func )
               {
                   case DATAPROC_FUNC_LOAD :
                      ok = FileLoad(FP,type);
                      break;

                   case DATAPROC_FUNC_SAVE :
                      ok = FileSave(FP,type);
                      break;
               }
               break;

            case DATAPROC_STEP_CLOSE :
               ok = (fclose(FP) == 0);
               break;
        }

        if( !ok )
        {
            DATAPROC_errorf("%s(%s) %s failed.\n",DATAPROC_FuncText[func],file,DATAPROC_StepText[step]);
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::FileLoad( FILE *FP, int type )
{
BOOL ok=TRUE;

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::FileSave( FILE *FP, int type )
{
BOOL ok;
int item;
double d;
float f;
long l;
int i;

    for( ok=TRUE,item=0; (ok); item++ )
    {
        if( !Get((item == 0) ? DATAPROC_GET_FIRST : DATAPROC_GET_NEXT,d) )
        {
            break;
        }

        switch( type )
        {
            case DATAPROC_TYPE_ASCII :
               ok = (fprintf(FP,"%.20lf\n",d) > 0);
               break;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Load( char *file, int type )
{
BOOL ok;

    ok = File(file,DATAPROC_FUNC_LOAD,type);

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Load( char *file )
{
BOOL ok;

    ok = Load(file,DATAPROC_TYPE_ASCII);

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Load( void )
{
BOOL ok;

    ok = Load(File(),DATAPROC_TYPE_ASCII);

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Save( char *file, int type )
{
BOOL ok;

    ok = File(file,DATAPROC_FUNC_SAVE,type);

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Save( char *file )
{
BOOL ok;

    ok = Save(file,DATAPROC_TYPE_ASCII);

    return(ok);
}

/******************************************************************************/

BOOL DATAPROC::Save( void )
{
BOOL ok;

    ok = Save(File(),DATAPROC_TYPE_ASCII);

    return(ok);
}

/******************************************************************************/

void    DATAPROC::Results( PRINTF prnf )
{
    prnf("%s Mean=%.3lf SD=%.3lf Min=%.3lf Max=%.3lf (n=%d).\n",Name(),Mean(),SD(),Min(),Max(),Count());
}

/******************************************************************************/

void    DATAPROC::Results( void )
{
    Results(printf);
}

/******************************************************************************/

