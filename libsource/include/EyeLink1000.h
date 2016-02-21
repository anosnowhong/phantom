/******************************************************************************/

#define EYELINK_errorf printf

/******************************************************************************/

void EYELINK_Close( void );
BOOL EYELINK_Open( void );
void EYELINK_Results( void );
BOOL EYELINK_Start( void );
void EYELINK_Stop( void );
void EYELINK_FrameExtract( FSAMPLE *frame, double &TimeStamp, double EyeXY[], double &PupilSize );
BOOL EYELINK_FrameNext( double &TimeStamp, double EyeXY[], double &PupilSize, BOOL &ready );
BOOL EYELINK_FrameNext( double &TimeStamp, double EyeXY[], BOOL &ready );

/******************************************************************************/

