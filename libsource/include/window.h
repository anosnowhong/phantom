/******************************************************************************/

#define WINDOW_SIZE 1000
#define WINDOW_DOF  6

/******************************************************************************/

 class WINDOW
{
private:
    STRING ObjectName;

    int Count;
    int Window;

    int DOF;

    matrix Points;
    double dtlist[WINDOW_SIZE];

    matrix dValue;

    DATA *data[WINDOW_DOF+1];

public:

    WINDOW( );
    WINDOW( char *name, int window, int dof );
    WINDOW( int window, int dof );

   ~WINDOW( );

    void Init( char *name, int window, int dof );

    BOOL Primed( void );
    int DOFs( void );
    int WindowSize( void );

    DATA *Data( int dof );

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
    double Variance( int dof );
    double SD( int dof );

    void MatrixDim( matrix &mtx );
    void MatrixSet( matrix &mtx, int dof, double value );

    void Mean( matrix &mtx );
    void Max( matrix &mtx );
    void Min( matrix &mtx );
    void Variance( matrix &mtx );
    void SD( matrix &mtx );

    BOOL QuadraticFit( int dof, double &pos, double &vel, double &acc );
    BOOL QuadraticFit( double &pos, double &vel, double &acc );
    BOOL QuadraticFit( double &vel, double &acc );
    BOOL QuadraticFit( double &vel );
    BOOL QuadraticFit( matrix &pos, matrix &vel, matrix &acc );
    BOOL QuadraticFit( matrix &vel, matrix &acc );
    BOOL QuadraticFit( matrix &vel );

};

/******************************************************************************/

