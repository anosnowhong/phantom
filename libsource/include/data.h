/******************************************************************************/

#ifndef DATA_H
#define DATA_H

/******************************************************************************/

class   DATA
{
private:
    STRING  ObjectName;
    int     MaxItems;
#define DATA_ITEM_MAX     10000

    BYTE    Flags;
#define DATA_FLAG_NONE     0x00
#define DATA_FLAG_CIRCULAR 0x01

    double *data;
    int     count;
    int     index;

    BOOL    DoneMean;
    BOOL    DoneVariance;

    int     tail;

    BYTE    error;
#define DATA_ERROR_NONE      0
#define DATA_ERROR_MAX       1
#define DATA_ERROR_ITEM      2

    double  total;
    double  mean;
    double  maximum;
    double  minimum;
    double  variance;
    double  sd;

public:
    DATA( char *name, int items, BYTE flags );
    DATA( char *name, int items );
    DATA( char *name );
    DATA( void );

   ~DATA( void );

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

    double ValueD( int item );
    float  ValueF( int item );
    long   ValueL( int item );
    int    ValueI( int item );

    double GetD( int func );
    float  GetF( int func );
    long   GetL( int func );
    int    GetI( int func );
#define DATA_GET_FIRST       0
#define DATA_GET_LAST        1
#define DATA_GET_NEXT        2
#define DATA_GET_PREVIOUS    3

    BOOL Error( BYTE &code );
    BOOL Error( STRING text );

    void CalculateMean( void );
    void CalculateVariance( void );
    void Calculate( void );

    char *Name( void );
    int MaxData( void );
    int Count( void );
    int Tail( void );
    BOOL Full( void );
    double Total( void );
    double Mean( void );
    double Max( void );
    double Min( void );
    double Variance( void );
    double SD( void );

    void Results( PRINTF prnf );
    void Results( void );

    char *File( void );

    BOOL File( char *file, int func, int type );

#define DATA_FUNC_LOAD       0
#define DATA_FUNC_SAVE       1

#define DATA_TYPE_ASCII      0
#define DATA_TYPE_DOUBLE     1
#define DATA_TYPE_FLOAT      2
#define DATA_TYPE_LONG       3
#define DATA_TYPE_INT        4

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

