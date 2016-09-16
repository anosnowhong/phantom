/************************************************************************/

#define BEEPER_LOOPTASK_FREQUENCY      50.0
#define BEEPER_LOOPTASK_PRIORITY       LOOPTASK_PRIORITY_LOW

/************************************************************************/

void BEEPER_ToneExit( void );
void BEEPER_Tone( int frequency, BOOL flag );

void BEEPER_ToneOn( double frequency );
void BEEPER_ToneOff( void );

BOOL BEEPER_Playing( void );

void BEEPER_Task( void );
void BEEPER_Start( double frequency, int priority );
void BEEPER_Start( double frequency );
void BEEPER_Start( void );
void BEEPER_Stop( void );

struct BEEPER_Note
{
    double frequency;
    double duration;
};

#define BEEPER_NOTES 64

#define BEEPER_PAUSE 0

/************************************************************************/

void BEEPER_Beep( double frequency, double duration );
void BEEPER_BeepWait( double frequency, double duration );
void BEEPER_Wait( void );

void BEEPER_SequenceNote( double frequency, double duration );
void BEEPER_SequencePause( double duration );
void BEEPER_SequenceClear( void );
void BEEPER_SequencePlay( void );

/************************************************************************/

