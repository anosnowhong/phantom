/******************************************************************************/

#define WINFIT_SIZE 1000
#define WINFIT_DOF  6

/******************************************************************************/

 class WINFIT
{
private:
    STRING ObjectName;

    int Count;
    int Window;

    int DOF;

    matrix Points;
    double dtlist[WINFIT_SIZE];

    matrix dValue;

    DATAPROC *data[WINFIT_DOF+1];

#define WINFIT_LOG 5
    DATAPROC *log[WINFIT_DOF][WINFIT_LOG];

public:

    WINFIT( );
    WINFIT( char *name, int window, int dof );
    WINFIT( int window, int dof );

   ~WINFIT( );

    void Init( char *name, int window, int dof );

    BOOL Primed( void );
    int DOFs( void );
    int WindowSize( void );

    DATAPROC *Data( int dof );

    void Reset( void );

    void Point( matrix data, double dt );
    void Point( double data, double dt );
    void Point( matrix data );
    void Point( double data );

    double GetValue( int dof, int index );
    BOOL dXdt( matrix &data );
    BOOL GetWindow( int dof, matrix &data );

    void Calculate( int dof );

    double Mean( int dof );
    double Max( int dof );
    double Min( int dof );
    double SignedMax( int dof );
    double Variance( int dof );
    double SD( int dof );

    void MatrixDim( matrix &mtx );
    void MatrixSet( matrix &mtx, int dof, double value );

    void Mean( matrix &mtx );
    void Max( matrix &mtx );
    void Min( matrix &mtx );
    void SignedMax( matrix &mtx );
    void Variance( matrix &mtx );
    void SD( matrix &mtx );

    BOOL QuadraticFitOld( int dof, double &pos, double &vel, double &acc );
    BOOL QuadraticFit( int dof, double &pos, double &vel, double &acc );
    BOOL QuadraticFit( double &pos, double &vel, double &acc );
    BOOL QuadraticFit( double &vel, double &acc );
    BOOL QuadraticFit( double &vel );
    BOOL QuadraticFit( matrix &pos, matrix &vel, matrix &acc );
    BOOL QuadraticFit( matrix &vel, matrix &acc );
    BOOL QuadraticFit( matrix &vel );

    void LogPoint( int dof, double dt, double x_raw, double xfit, double dxfit, double ddxfit );
    BOOL LogSave( void );
    void LogFree( void );
};

/******************************************************************************/

