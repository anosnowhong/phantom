/******************************************************************************/
/* SEQUENCER - Execute a prescribed sequence of TRIGGER pulses...             */
/******************************************************************************/
#include <motor.h>
#include <sequencer.h>

int SEQUENCER_Port=1;
int SEQUENCER_Channel=0;
double SEQUENCER_Frequency;
double SEQUENCER_Period;
STRING SEQUENCER_Sequence;
int SEQUENCER_Pulses=0;
int SEQUENCER_Repeats;
int SEQUENCER_Cycles;
double SEQUENCER_PulseWidth=1.0;

int SEQUENCER_Length;
int SEQUENCER_Index=0;
int SEQUENCER_Repeat=0;
int SEQUENCER_Cycle=0;
double SEQUENCER_Time=0.0;
double SEQUENCER_TimeRepeats=0.0;
int SEQUENCER_PulsesRepeats=0;
double SEQUENCER_TimeCycles=0.0;
int SEQUENCER_PulsesCycles=0;
double SEQUENCER_Step=0.0;

TIMER SEQUENCER_Timer("Sequence");
TIMER SEQUENCER_TimerCycle("Sequence Cycle");
int SEQUENCER_PulseCount=0;

BOOL SEQUENCER_LoadFlag=FALSE;
BOOL SEQUENCER_RunningFlag=FALSE;
BOOL SEQUENCER_Finish=FALSE;

/******************************************************************************/

BOOL SEQUENCER_Open( int port, int channel, double pulsewidth )
{
    SEQUENCER_Port = port;
    SEQUENCER_Channel = channel;
    SEQUENCER_PulseWidth = pulsewidth;

    return(TRUE);
}

/******************************************************************************/

void SEQUENCER_Close( void )
{
    SEQUENCER_Stop();
    SEQUENCER_Unload();
}

/******************************************************************************/

BOOL SEQUENCER_Load( double frequency, char *sequence, int repeats )
{
BOOL ok;

    ok = SEQUENCER_Load(frequency,sequence,repeats,0,0.0);

    return(ok);
}

/******************************************************************************/

BOOL SEQUENCER_Load( double frequency, char *sequence, int repeats, int cycles, double period )
{
int i;

    if( SEQUENCER_Running() )
    {
        return(TRUE);
    }

    SEQUENCER_Unload();

    SEQUENCER_Frequency = frequency;
    SEQUENCER_Repeats = repeats;
    SEQUENCER_Cycles = cycles;
    SEQUENCER_Period = period;
    strncpy(SEQUENCER_Sequence,sequence,STRLEN);

    SEQUENCER_Length = strlen(SEQUENCER_Sequence);
    SEQUENCER_Step = 1.0 / SEQUENCER_Frequency;
    SEQUENCER_Time = SEQUENCER_Step * (double)SEQUENCER_Length;
    SEQUENCER_TimeRepeats = SEQUENCER_Time * (double)SEQUENCER_Repeats;
    SEQUENCER_TimeCycles = SEQUENCER_Period * (double)SEQUENCER_Cycles;

    SEQUENCER_Pulses = 0;

    for( i=0; (i < SEQUENCER_Length); i++ )
    {
        if( SEQUENCER_Pulse(SEQUENCER_Sequence,i) )
        {
            SEQUENCER_Pulses++;
        }
    }

    SEQUENCER_PulsesRepeats = SEQUENCER_Pulses * SEQUENCER_Repeats;
    SEQUENCER_PulsesCycles = SEQUENCER_PulsesRepeats * SEQUENCER_Cycles;

    if( !LOOPTASK_start(SEQUENCER_Task,SEQUENCER_Frequency) )
    {
        printf("SEQUENCE: Cannot start LoopTask.\n");
        return(FALSE);
    }

    SEQUENCER_LoadFlag = TRUE;

    return(TRUE);
}

/******************************************************************************/

void SEQUENCER_Unload( void )
{
    if( SEQUENCER_Running() )
    {
        return;
    }

    LOOPTASK_stop(SEQUENCER_Task);

    SEQUENCER_LoadFlag = FALSE;
}

/******************************************************************************/

BOOL SEQUENCER_Loaded( void )
{
    return(SEQUENCER_LoadFlag);
}


/******************************************************************************/

BOOL SEQUENCER_Running( void )
{
    return(SEQUENCER_RunningFlag);
}

/******************************************************************************/

void SEQUENCER_Details( PRINTF prnf )
{
    (*prnf)("Sequence Details...\n");
    (*prnf)("%d bins at %.1lf Hz (%.3lf seconds).\n",SEQUENCER_Length,SEQUENCER_Frequency,SEQUENCER_Time); 
    (*prnf)("%d repeats at %.3lf seconds (%d pulses in %.3lf seconds).\n",SEQUENCER_Repeats,SEQUENCER_Time,SEQUENCER_PulsesRepeats,SEQUENCER_TimeRepeats);

    if( SEQUENCER_Cycles != 0 )
    {
        (*prnf)("%d cycles of %.3lf second period (%d pulses in %.3lf seconds).\n",SEQUENCER_Cycles,SEQUENCER_Period,SEQUENCER_PulsesCycles,SEQUENCER_TimeCycles);
    }
}

/******************************************************************************/

void SEQUENCER_Start( void )
{
    if( !SEQUENCER_Loaded() )
    {
        return;
    }

    if( SEQUENCER_Running() )
    {
        return;
    }

    SEQUENCER_Index = 0;
    SEQUENCER_Repeat = 0;
    SEQUENCER_Cycle = 0;
    SEQUENCER_PulseCount = 0;
    SEQUENCER_Finish = FALSE;

    SEQUENCER_Timer.Reset();
    SEQUENCER_TimerCycle.Reset();

    SEQUENCER_RunningFlag = TRUE;
}

/******************************************************************************/

void SEQUENCER_Stop( void )
{
    if( !SEQUENCER_Running() )
    {
        return;
    }

    SEQUENCER_Finish = TRUE;
    SEQUENCER_RunningFlag = FALSE;
}

/******************************************************************************/

BOOL SEQUENCER_Pulse( char *sequence, int index )
{
BOOL flag;

    flag = (sequence[index] == '1');

    return(flag);
}

/******************************************************************************/

void SEQUENCER_Task( void )
{
    if( !SEQUENCER_Running() )
    {
        return;
    }

    // Check for end of sequence...
    if( SEQUENCER_Index == SEQUENCER_Length )
    {
        // Check for repeats of sequences...
        if( SEQUENCER_Repeat == SEQUENCER_Repeats )
        {
            // Check for cycles...
            if( (SEQUENCER_Cycles == 0) || (SEQUENCER_Cycle == SEQUENCER_Cycles) )
            {
                SEQUENCER_Stop();
                return;
            }

            // Check if cycle period has expired...
            if( !SEQUENCER_TimerCycle.ExpiredSeconds(SEQUENCER_Period) )
            {
                return;
            }

            SEQUENCER_Repeat = 0;
            SEQUENCER_TimerCycle.Reset();
        }

        SEQUENCER_Index = 0;
    }

    if( SEQUENCER_Pulse(SEQUENCER_Sequence,SEQUENCER_Index) )
    {
        TRIGGER_pulse(SEQUENCER_Port,SEQUENCER_Channel,SEQUENCER_PulseWidth);
        SEQUENCER_PulseCount++;
    }

    // Increment various counters...
    if( ++SEQUENCER_Index == SEQUENCER_Length )
    {
        if( ++SEQUENCER_Repeat == SEQUENCER_Repeats )
        {
            SEQUENCER_Cycle++;
        }
    }
}

/******************************************************************************/

BOOL SEQUENCER_Finished( int &pulsecount, double &elapsedseconds )
{
BOOL flag=FALSE;

    if( SEQUENCER_Finish )
    {
        SEQUENCER_Finish = FALSE;

        pulsecount = SEQUENCER_PulseCount;
        elapsedseconds = SEQUENCER_Timer.ElapsedSeconds();

        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

BOOL SEQUENCER_Finished( void )
{
BOOL flag=FALSE;
int pulsecount;
double elapsedseconds;

    flag = SEQUENCER_Finished(pulsecount,elapsedseconds);

    return(flag);
}

/******************************************************************************/

