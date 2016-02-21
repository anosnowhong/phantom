/******************************************************************************/

#define WINFIT2_SIZE 100

/******************************************************************************/

 class WINFIT2
{
private:
    STRING ObjectName;

    int Count;
    int Window;
    int IndexHead;

    double xlist[WINFIT2_SIZE];
    double dtlist[WINFIT2_SIZE];

#define WINFIT2_LOG 5
    DATAPROC *log[WINFIT2_LOG];

public:

    WINFIT2( );
    WINFIT2( char *name, int window, BOOL logflag );
    WINFIT2( char *name, int window );
    WINFIT2( int window );

   ~WINFIT2( );

    void Init( char *name, int window, BOOL logflag );

    BOOL Primed( void );
    int WindowSize( void );

    void Reset( void );

    void Point( double data, double dt );

    int GetWindow( double *x, double *dt );

    void Calculate( void );

    BOOL QuadraticFit( double &pos, double &vel, double &acc );
    BOOL QuadraticFit( double &vel, double &acc );
    BOOL QuadraticFit( double &vel );

    void LogPoint( double dt, double x_raw, double xfit, double dxfit, double ddxfit );
    BOOL LogSave( void );
    void LogFree( void );
    void LogReset( void );
};

/******************************************************************************/

