/******************************************************************************/

#pragma pack(push)
#pragma pack(1)

/******************************************************************************/

struct SOCKETATIFT_Header
{
    BYTE code;
#define SOCKETATIFT_CODE_START   0
#define SOCKETATIFT_CODE_STOP    1
#define SOCKETATIFT_CODE_BIAS    2
#define SOCKETATIFT_CODE_DATA    3
};

/******************************************************************************/

struct SOCKETATIFT_Packet
{
    struct SOCKETATIFT_Header header;

#define SOCKETATIFT_DATA 256
    BYTE data[SOCKETATIFT_DATA];
};

/******************************************************************************/

struct SOCKETATIFT_Start
{
    SOCKETATIFT_Header header;
    int SSN;
    float freq;
    BYTE flag;
#define SOCKETATIFT_FLAG_BIAS      0x01
#define SOCKETATIFT_FLAG_FORCES    0x02
#define SOCKETATIFT_FLAG_TORQUES   0x04
};

/******************************************************************************/

struct SOCKETATIFT_Stop
{
    struct SOCKETATIFT_Header header;
};

/******************************************************************************/

struct SOCKETATIFT_Bias
{
    struct SOCKETATIFT_Header header;
};

/******************************************************************************/

struct SOCKETATIFT_Data
{
    struct SOCKETATIFT_Header header;

    BYTE flag;

    int frame;

    float values[ATIFT_DATA_FORCE+ATIFT_DATA_TORQUE];
};

/******************************************************************************/

#define SOCKETATIFT_SIZE_HEADER sizeof(struct SOCKETATIFT_Header)
#define SOCKETATIFT_SIZE_START  sizeof(struct SOCKETATIFT_Start)
#define SOCKETATIFT_SIZE_STOP   sizeof(struct SOCKETATIFT_Stop)
#define SOCKETATIFT_SIZE_BIAS   sizeof(struct SOCKETATIFT_Bias)
#define SOCKETATIFT_SIZE_DATA   sizeof(struct SOCKETATIFT_Header)+size(BYTE)+size(int)

#pragma pack(pop)

/******************************************************************************/
 
BOOL SOCKETATIFT_Open( char *IP );
void SOCKETATIFT_Close( void );

BOOL SOCKETATIFT_Receive( BOOL &ready, double wait, BYTE *data, int &size );

BOOL SOCKETATIFT_Start( int SSN, float freq, BYTE flag );
BOOL SOCKETATIFT_Start( int SSN, float freq );

BOOL SOCKETATIFT_Stop( void );

BOOL SOCKETATIFT_Bias( void );

BOOL SOCKETATIFT_Process( BOOL &ready, int &frame, double F[], double T[] );
BOOL SOCKETATIFT_Process( BOOL &ready, int &frame, double F[] );

/******************************************************************************/

