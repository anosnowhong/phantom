/******************************************************************************/

class PERMUTELIST
{
public:
    matrix List;
    int Min;
    int Max;
    int Count;
    int Index;
    int Loops;
    BOOL PermuteFlag;

    PERMUTELIST( int min, int max, BOOL permuted );
    PERMUTELIST( int min, int max );
    PERMUTELIST( );
   ~PERMUTELIST( );

    void Init( int min, int max, BOOL permuted );
    void Init( int min, int max );
    void Init( void );

    void Reset( void );

    BOOL NextValue( int &i );

    int GetNext( int &loop );
    int GetNext( void );

    int LoopCount( void );
};

/******************************************************************************/

