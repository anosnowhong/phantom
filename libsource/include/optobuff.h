/******************************************************************************/
/*                                                                            */ 
/* MODULE  : OPTObuff.h                                                       */ 
/*                                                                            */ 
/* PURPOSE : OptoTrak data buffering routines.                                */ 
/*                                                                            */ 
/* DATE    : 21/Jun/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V2.0  JNI 21/Jun/2000 - Developed with new MOTOR.LIB modules.              */ 
/*                                                                            */ 
/* V2.2  JNI 14/Feb/2001 - Testing led to improvements & bug fixes.           */ 
/*                                                                            */ 
/* V2.3  JNI 25/Apr/2001 - Changed to use default file names and buffers.     */ 
/*                                                                            */ 
/******************************************************************************/

#define OPTO_BUFFER_NONE     0              // Buffering stages...
#define OPTO_BUFFER_SETUP    1
#define OPTO_BUFFER_RUNNING  2
#define OPTO_BUFFER_DONE     3

/******************************************************************************/

typedef struct OPTO_MemoryBuffer            // Memory for buffered collection.
{
    float freq;
    float ct;
    int   frames;
    int   collected;
    int   markers;
    void *data;
}
OPTOMEM;

/******************************************************************************/

extern  int      OPTO_BufferType;                // Buffering type (file or memory).
extern  char    *OPTO_BufferText[];
extern  ULONG    OPTO_BufferFrame;               // Number of frames collected.
extern  OPTOMEM *OPTO_Buffer;                    // Pointer to memory-buffered block.

/******************************************************************************/

char    *OPTO_FileDefault( char *file );         // Return default file name. (V2.3)
#define  OPTO_FILE_DEFAULT   "OPTOTRAK.DAT"      // Default file name.
#define  OPTO_FILE_NULL      NULL                // NULL file name.

OPTOMEM *OPTO_MemoryDefault( OPTOMEM *buff );    // Return default memory buffer. (V2.3)
#define  OPTO_BUFF_DEFAULT  &OPTO_Buff;          // Default memory block information.
#define  OPTO_BUFF_NULL      NULL                // NULL memory block information.

/******************************************************************************/

BOOL    OPTO_BufferSetup( float freq, int m1, int m2, int m3, int m4, float ct, int flag, int type, void *buff );
#define OPTO_BUFFER_FILE     0              // Buffer to file.
#define OPTO_BUFFER_MEMORY   1              // Buffer to memory area.

BOOL    OPTO_BufferSetup( int type, char *cnfg );

BOOL    OPTO_BufferFile( float freq, int m1, int m2, int m3, int m4, float ct, int flag, char *file );
BOOL    OPTO_BufferFile( float freq, int m1, int m2, int m3, int m4, float ct, int flag );
BOOL    OPTO_BufferFile( float freq, int m[], float ct, int flag, char *file );
BOOL    OPTO_BufferFile( float freq, int m[], float ct, int flag );
BOOL    OPTO_BufferFile( int m[], float ct, char *file );
BOOL    OPTO_BufferFile( int m[], float ct );
BOOL    OPTO_BufferFile( char *cnfg );
BOOL    OPTO_BufferFile( void );

BOOL    OPTO_BufferMemory( float freq, int m1, int m2, int m3, int m4, float ct, int flag, OPTOMEM *buff );
BOOL    OPTO_BufferMemory( float freq, int m1, int m2, int m3, int m4, float ct, int flag );
BOOL    OPTO_BufferMemory( float freq, int m[], float ct, int flag, OPTOMEM *buff );
BOOL    OPTO_BufferMemory( float freq, int m[], float ct, int flag );
BOOL    OPTO_BufferMemory( int m[], float ct, OPTOMEM *buff );
BOOL    OPTO_BufferMemory( int m[], float ct );
BOOL    OPTO_BufferMemory( char *cnfg );
BOOL    OPTO_BufferMemory( void );

BOOL    OPTO_BufferStart( void );
BOOL    OPTO_BufferStop( BOOL terminate );

BOOL    OPTO_BufferComplete( void );
BOOL    OPTO_BufferStopNow( void );

BOOL    OPTO_BufferUpdate( BOOL *RTD, BOOL *SC, BOOL *SS, ULONG *FB );
BOOL    OPTO_BufferUpdate( BOOL *SC, ULONG *FB );
BOOL    OPTO_BufferUpdate( BOOL *SC );
BOOL    OPTO_BufferUpdate( void );

#define OPTO_BUFFER_WAIT     20.0      // Default time-out for buffer wait (seconds).

BOOL    OPTO_BufferWait( double seconds );
BOOL    OPTO_BufferWait( void );

void    OPTO_BufferFree( OPTOMEM *buff );

int     OPTO_BufferFrames( float freq, float ct );
int     OPTO_BufferTotalFloats( float freq, float ct, int markers );
int     OPTO_BufferFloatsPerFrame( int markers );
int     OPTO_BufferTotalBytes( float freq, float ct, int markers );
void    OPTO_BufferDimensions( float freq, float ct, int markers, int &row, int &col );

/******************************************************************************/

