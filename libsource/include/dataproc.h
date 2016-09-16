/******************************************************************************/

#ifndef DATAPROC_H
#define DATAPROC_H

/******************************************************************************/

class   DATAPROC
{
private:
    STRING  ObjectName;
    int     MaxItems;
#define DATAPROC_ITEM_MAX     10000

    BYTE    Flags;
#define DATAPROC_FLAG_NONE     0x00
#define DATAPROC_FLAG_CIRCULAR 0x01

    double *data;
    int     count;
    int     index;

    BOOL    DoneMean;
    BOOL    DoneVariance;

    int     tail;

    BYTE    error;
#define DATAPROC_ERROR_NONE      0
#define DATAPROC_ERROR_MAX       1
#define DATAPROC_ERROR_ITEM      2

    double  total;
    double  mean;
    double  maximum;
    double  minimum;
    double  signedmax;
    double  range;
    double  variance;
    double  sd;
    double  median;

public:
    DATAPROC( char *name, int items, BYTE flags );
    DATAPROC( char *name, int items );
    DATAPROC( char *name );
    DATAPROC( void );

   ~DATAPROC( void );

    void Init( void );
    BOOL Init( char *name, int items, BYTE flags );
    void Reset( void );

    int Wrap( int index );
    int Index( int item );

    void Data( double value );
    void Data( float value );
    void Data( long value );
    void Data( int value );

    BOOL Item( int item );

    BOOL Value( int item, double &D );
    BOOL Value( int item, float &F );
    BOOL Value( int item, long &L );
    BOOL Value( int item, int &I );

    BOOL Get( int func, double &D );
    BOOL Get( int func, float &F );
    BOOL Get( int func, long &L );
    BOOL Get( int func, int &I );

    void GetVector( matrix &V );

    double ValueD( int item );
    float  ValueF( int item );
    long   ValueL( int item );
    int    ValueI( int item );

    double GetD( int func );
    float  GetF( int func );
    long   GetL( int func );
    int    GetI( int func );
#define DATAPROC_GET_FIRST       0
#define DATAPROC_GET_LAST        1
#define DATAPROC_GET_NEXT        2
#define DATAPROC_GET_PREVIOUS    3

    BOOL Error( BYTE &code );
    BOOL Error( STRING text );

    void CalculateMean( void );
    void CalculateVariance( void );
    void CalculateMedian( void );
    void Calculate( void );

    char *Name( void );
    int MaxData( void );
    int Count( void );
    int Tail( void );
    BOOL Full( void );
    double Sum( void );
    double Mean( void );
    double Max( void );
    double Min( void );
    double SignedMax( void );
    double Range( void );
    double Variance( void );
    double SD( void );
    double Median( void );

    void Results( PRINTF prnf );
    void Results( void );

    char *File( void );

    BOOL File( char *file, int func, int type );

#define DATAPROC_FUNC_LOAD       0
#define DATAPROC_FUNC_SAVE       1

#define DATAPROC_TYPE_ASCII      0
#define DATAPROC_TYPE_DOUBLE     1
#define DATAPROC_TYPE_FLOAT      2
#define DATAPROC_TYPE_LONG       3
#define DATAPROC_TYPE_INT        4

    BOOL FileLoad( FILE *FP, int type );
    BOOL FileSave( FILE *FP, int type );

    BOOL Load( char *file, int type );
    BOOL Load( char *file );
    BOOL Load( void );

    BOOL Save( char *file, int type );
    BOOL Save( char *file );
    BOOL Save( void );
};

/******************************************************************************/

#endif

