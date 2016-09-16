/************************************************************************/

#include <motor.h>

/************************************************************************/

BOOL    BEEPER_Started=FALSE;
BOOL    BEEPER_BeepNow=FALSE;
double  BEEPER_Time;
double  BEEPER_Mark;
double  BEEPER_Frequency;
double  BEEPER_Duration;

struct  BEEPER_Note  BEEPER_Notes[BEEPER_NOTES];
int     BEEPER_NoteCount=0;
int     BEEPER_NoteCurrent=0;

/************************************************************************/

void BEEPER_ToneExit( void )
{
    BEEPER_Tone(0,OFF);
}

/************************************************************************/

void BEEPER_Tone( int frequency, BOOL flag )
{
static BOOL init=TRUE;
static BOOL play=FALSE;
static BYTE control;
WORD freq;

    if( init )
    {
        init = FALSE;
        atexit(BEEPER_ToneExit);
    }

    if( !flag )
    {
        if( play )
        {
            // Turn speaker off...
            ISA_byte(0x61,control & ~0x03);
            play = FALSE;
        }

        return;
    }       

    // Prepare timer to accept frequency...
    ISA_byte(0x43,0xB6);
      
    // Write frequency to timer...
    freq = (WORD)(1193180 / frequency);
    ISA_byte(0x42,(BYTE)freq);
    ISA_byte(0x42,(BYTE)(freq >> 8));

    if( !play )
    { 
        // Save speaker control byte...
        control = ISA_byte(0x61);
      
        // Turn on the speaker (with bits 0 and 1)...
        ISA_byte(0x61,control | 0x03);
        play = TRUE;
    }
}

/************************************************************************/

void BEEPER_ToneOn( double frequency )
{
    BEEPER_Frequency = frequency;

    if( BEEPER_Frequency != BEEPER_PAUSE )
    {
        BEEPER_Tone((int)BEEPER_Frequency,ON);
    }

    BEEPER_BeepNow = TRUE;
}

/************************************************************************/

void BEEPER_ToneOff( void )
{
    if( BEEPER_Frequency != BEEPER_PAUSE )
    {
        BEEPER_Tone(0,OFF);
    }

    BEEPER_BeepNow = FALSE;
}

/************************************************************************/

BOOL BEEPER_Playing( void )
{
    BEEPER_Start();

    return(BEEPER_BeepNow);
}

/************************************************************************/

void BEEPER_Wait( void )
{
    while( BEEPER_Playing() );
}

/************************************************************************/

void BEEPER_Task( void )
{
    BEEPER_Time = LOOPTASK_msec();

    if( !BEEPER_BeepNow )
    {
        return;
    }

    if( (BEEPER_Time-BEEPER_Mark) >= BEEPER_Duration )
    {
        BEEPER_ToneOff();

        if( BEEPER_NoteCurrent < BEEPER_NoteCount )
        {
            BEEPER_Beep(BEEPER_Notes[BEEPER_NoteCurrent].frequency,BEEPER_Notes[BEEPER_NoteCurrent].duration);
            BEEPER_NoteCurrent++;
        }
    }
}

/************************************************************************/

void BEEPER_Start( double frequency, int priority )
{
    // Check if BEEPER already started...
    if( BEEPER_Started )
    {
        return;
    }

    if( !ISA_API_check() )
    {
        printf("BEEPER: Cannot start ISA API.\n");
    }

    // Start BEEPER Task...
    if( !LOOPTASK_running(BEEPER_Task) )
    {
        if( !LOOPTASK_start("BEEPER",BEEPER_Task,frequency,priority) )
        {
            printf("BEEPER: Cannot start LoopTask.\n");
        }
    }

    BEEPER_Started = TRUE;
    BEEPER_BeepNow = FALSE;

    BEEPER_SequenceClear();
}

/************************************************************************/

void BEEPER_Start( double frequency )
{
    // Start BEEPER with specified frequency and default priority...
    BEEPER_Start(frequency,LOOPTASK_PRIORITY_DEFAULT);
}

/************************************************************************/

void BEEPER_Start( void )
{
    // Start BEEPER with default frequency and priority...
    BEEPER_Start(BEEPER_LOOPTASK_FREQUENCY,BEEPER_LOOPTASK_PRIORITY);
}

/************************************************************************/

void BEEPER_Stop( void )
{
    if( BEEPER_Started )
    {
        LOOPTASK_stop(BEEPER_Task);
        BEEPER_Started = FALSE;
    }
}

/************************************************************************/

void BEEPER_Beep( double frequency, double duration )
{
    // Make sure BEEPER is started...
    BEEPER_Start();

    // Set various parameters so that it can be turned off...
    BEEPER_Mark = BEEPER_Time;
    BEEPER_Frequency = frequency;
    BEEPER_Duration = duration;

    // Immediately turn on the requested frequency...
    BEEPER_ToneOn(frequency);
}

/************************************************************************/

void BEEPER_BeepWait( double frequency, double duration )
{
    BEEPER_Beep(frequency,duration);
    BEEPER_Wait();
}

/************************************************************************/

void BEEPER_SequenceClear( void )
{
    BEEPER_NoteCount = 0;
    BEEPER_NoteCurrent = 0;
}

/************************************************************************/

void BEEPER_SequenceNote( double frequency, double duration )
{
    BEEPER_Start();

    if( BEEPER_NoteCount < BEEPER_NOTES )
    {
        BEEPER_Notes[BEEPER_NoteCount].frequency = frequency;
        BEEPER_Notes[BEEPER_NoteCount].duration = duration;
        BEEPER_NoteCount++;
    }
}

/************************************************************************/

void BEEPER_SequencePause( double duration )
{
    BEEPER_SequenceNote(BEEPER_PAUSE,duration);
}

/************************************************************************/

void BEEPER_SequencePlay( void )
{
    if( BEEPER_NoteCount == 0 )
    {
        return;
    }

    BEEPER_NoteCurrent = 0;
    BEEPER_Beep(BEEPER_Notes[BEEPER_NoteCurrent].frequency,BEEPER_Notes[BEEPER_NoteCurrent].duration);
    BEEPER_NoteCurrent++;
}

/************************************************************************/

