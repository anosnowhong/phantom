/******************************************************************************/

BOOL   WAVE_Volume( double &volume, BOOL set );
double WAVE_GetVolume( void );
void   WAVE_SetVolume( double volume );

/******************************************************************************/

class WAVEPLAY
{
private:
    STRING ObjectName;
    STRING FileName;
    STRING FilePath;
    BOOL LoadFlag;
    char *Buffer;
    DWORD Size;
    double Seconds;

public:
    WAVEPLAY( char *name, char *file );
    WAVEPLAY( char *file );
    WAVEPLAY( void );
   ~WAVEPLAY( void );

    BOOL Init( char *name, char *file );
    BOOL Init( char *file );
    BOOL Init( void );

    void Free( void );

    BOOL Load( char *file );
    BOOL Load( void );

    BOOL Loaded( void );

    BOOL Play( BOOL Wait, BOOL Loop );
    BOOL Play( void );
    BOOL PlayWait( void );

    void Stop( void );
};

/******************************************************************************/

#define WAVEPLAY_errorf printf
#define WAVEPLAY_debugf NULL
#define WAVEPLAY_messgf NULL

/******************************************************************************/

struct WAVELIST
{
    STRING ObjectName;
    STRING FileName;
    WAVEPLAY *Object;
};

/******************************************************************************/

BOOL WAVELIST_Load( WAVELIST *list );
BOOL WAVELIST_Open( WAVELIST list[] );
void WAVELIST_Close( WAVELIST list[] );
WAVEPLAY *WAVELIST_Object( WAVELIST list[], char *name );
void WAVELIST_Play( WAVELIST list[], char *name, BOOL wait );
void WAVELIST_Play( WAVELIST list[], char *name );
void WAVELIST_PlayWait( WAVELIST list[], char *name );

/******************************************************************************/

