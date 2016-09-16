/******************************************************************************/

#include <motor.h>
#include <EyeLink1000.h>

/******************************************************************************/

TIMER_Frequency EYELINK_SampleFrequency("EYELINK_SampleFrequency");
TIMER_Frequency EYELINK_EventFrequency("EYELINK_EventFrequency");

TIMER_Interval  EYELINK_CheckRecordingLatency("EYELINK_CheckRecordingLatency");
TIMER_Interval  EYELINK_GetNextDataLatency("EYELINK_GetNextDataLatency");
TIMER_Interval  EYELINK_GetFloatDataLatency("EYELINK_GetFloatDataLatency");
TIMER_Interval  EYELINK_DataStart("EYELINK_DataStart");
TIMER_Interval  EYELINK_DataStartWaitReady("EYELINK_DataStartWaitReady");
TIMER_Interval  EYELINK_DataStop("EYELINK_DataStop");
TIMER_Interval  EYELINK_DataStopWaitReady("EYELINK_DataStopWaitReady");
TIMER_Interval  EYELINK_OpenLatency("EYELINK_OpenLatency");
TIMER_Interval  EYELINK_CloseLatency("EYELINK_CloseLatency");
TIMER_Interval  EYELINK_StartLatency("EYELINK_StartLatency");
TIMER_Interval  EYELINK_StopLatency("EYELINK_StopLatency");

BOOL    EYELINK_OpenFlag=FALSE;
BOOL    EYELINK_StartFlag=FALSE;
INT16   EYELINK_EyeUsed=-1;
FSAMPLE EYELINK_SampleData;
FEVENT  EYELINK_EventData;
int     EYELINK_FrameCount=0;

/******************************************************************************/

void EYELINK_Close( void )
{
UINT16 rc;
BOOL ok;

    EYELINK_CloseLatency.Before();

    ok = ((rc=eyelink_close(1)) == 0);
    EYELINK_errorf("EYELINK_Close: eyelink_close() %s.\n",STR_OkFailed(ok));

    EYELINK_CloseLatency.After();

    EYELINK_OpenFlag = FALSE;
}

/******************************************************************************/

BOOL EYELINK_Open( void )
{
UINT16 rc;
BOOL ok;
int i;

    if( EYELINK_OpenFlag )
    {
        return(TRUE);
    }

    EYELINK_OpenLatency.Before();

    for( ok=TRUE,i=0; ((i < 3) && ok); i++ ) // Loop over steps for opening EyeLink1000
    {
        switch( i )
        {
            case 0 : // Initialize EyeLink library.
               ok = ((rc=open_eyelink_connection(-1)) == 0);
               EYELINK_errorf("EYELINK_Open: open_eyelink_connection() %s (rc=%d).\n",STR_OkFailed(ok),rc);
               break;

            case 1 : // Open connection to EyeLink hardware.
               ok = ((rc=eyelink_open()) == 0);
               EYELINK_errorf("EYELINK_Open: eyelink_open() %s (rc=%d).\n",STR_OkFailed(ok),rc);
               break;

            case 2 : // Check that connection is open.
               ok = (eyelink_is_connected() != 0);
               EYELINK_errorf("EYELINK_Open: eyelink_is_connected() %s.\n",STR_YesNo(ok));
        }
    }

    EYELINK_OpenLatency.After();

    if( ok )
    {
        EYELINK_OpenFlag = TRUE;
    }
    else
    {
        EYELINK_Close();
    }

    return(ok);
}

/******************************************************************************/

void EYELINK_Results( void )
{
    EYELINK_OpenLatency.Results();
    EYELINK_CloseLatency.Results();
    EYELINK_StartLatency.Results();
    EYELINK_StopLatency.Results();
    EYELINK_SampleFrequency.Results();

    EYELINK_CheckRecordingLatency.Results();
    EYELINK_GetNextDataLatency.Results();
    EYELINK_GetFloatDataLatency.Results();
    EYELINK_EventFrequency.Results();
    EYELINK_DataStart.Results();
    EYELINK_DataStartWaitReady.Results();
    EYELINK_DataStop.Results();
    EYELINK_DataStopWaitReady.Results();
}

/******************************************************************************/

BOOL EYELINK_Start( void )
{
UINT16 rc;
BOOL ok;
int i;

    if( !EYELINK_OpenFlag )
    {
        return(FALSE);
    }

    if( EYELINK_StartFlag )
    {
        return(TRUE);
    }

    EYELINK_StartLatency.Before();

    for( ok=TRUE,i=0; ((i < 3) && ok); i++ ) // Loop over steps for starting EyeLink1000
    {
        switch( i )
        {
            case 0 : // Start EyeLink data session.
               EYELINK_DataStart.Before();
               ok = ((rc=eyelink_data_start(RECORD_LINK_SAMPLES,0)) == 0);
               EYELINK_DataStart.After();
               EYELINK_errorf("EYELINK_Start: eyelink_data_start() %s (rc=%d).\n",STR_OkFailed(ok),rc);
               break;

            case 1 : // Wait for EyeLink to be ready.
               EYELINK_DataStartWaitReady.Before();
               ok = ((rc=eyelink_wait_for_mode_ready(1000)) != 0);
               EYELINK_DataStartWaitReady.After();
               EYELINK_errorf("EYELINK_Start: eyelink_wait_for_mode_ready() %s (rc=%d).\n",STR_OkFailed(ok),rc);
               break;

            case 2 : // What eye is being used?
               if( (EYELINK_EyeUsed=eyelink_eye_available()) == BINOCULAR )
               {
                   EYELINK_EyeUsed = LEFT_EYE;
               }
               EYELINK_errorf("EYELINK_Start: eyelink_eye_available()=%d.\n",EYELINK_EyeUsed);
               break;
        }
    }

    EYELINK_StartLatency.After();

    EYELINK_SampleFrequency.Reset();
    EYELINK_EventFrequency.Reset();

    EYELINK_StartFlag = ok;

    return(ok);
}

/******************************************************************************/

void EYELINK_Stop( void )
{
UINT16 rc;
BOOL ok;
int i;

    if( !(EYELINK_OpenFlag & EYELINK_StartFlag) )
    {
        return;
    }

    EYELINK_StopLatency.Before();

    for( ok=TRUE,i=0; ((i < 2) && ok); i++ ) // Loop over steps for stopping EyeLink1000
    {
        switch( i )
        {
            case 0 : // Stop EyeLink data session.
               EYELINK_DataStop.Before();
               ok = ((rc=eyelink_data_stop()) == 0);
               EYELINK_DataStop.After();
               EYELINK_errorf("EYELINK_Stop: eyelink_data_stop() %s (rc=%d).\n",STR_OkFailed(ok),rc);
               break;

            case 1 : // Wait for EyeLink to be ready.
               EYELINK_DataStopWaitReady.Before();
               ok = ((rc=eyelink_wait_for_mode_ready(1000)) == 0);
               EYELINK_DataStopWaitReady.After();
               EYELINK_errorf("EYELINK_Stop: eyelink_wait_for_mode_ready() %s (rc=%d).\n",STR_OkFailed(ok),rc);
               break;
        }
    }

    EYELINK_StopLatency.After();

    EYELINK_StartFlag = FALSE;
}

/******************************************************************************/

void EYELINK_FrameExtract( FSAMPLE *frame, double &TimeStamp, double EyeXY[], double &PupilSize )
{
    TimeStamp = (double)frame->time;
    EyeXY[0] = (double)frame->gx[EYELINK_EyeUsed];
    EyeXY[1] = (double)frame->gy[EYELINK_EyeUsed];
    PupilSize = (double)frame->pa[EYELINK_EyeUsed];
}

/******************************************************************************/

BOOL EYELINK_FrameNext( double &TimeStamp, double EyeXY[], double &PupilSize, BOOL &ready )
{
UINT16 rc;
BOOL ok;

    ready = FALSE;

    if( !(EYELINK_OpenFlag & EYELINK_StartFlag) )
    {
        return(FALSE);
    }

    EYELINK_CheckRecordingLatency.Before();
    ok = (check_recording() == TRIAL_OK);
    EYELINK_CheckRecordingLatency.After();

    if( !ok )
    {
        return(FALSE);
    }
 
    EYELINK_GetNextDataLatency.Before();
    rc = eyelink_get_next_data(NULL);
    EYELINK_GetNextDataLatency.After();

    switch( rc )
    {
        case 0 :           // No data
            break;

        case SAMPLE_TYPE : // Sample
            EYELINK_SampleFrequency.Loop();
            EYELINK_GetFloatDataLatency.Before();
            ok = ((rc=eyelink_get_float_data(&EYELINK_SampleData)) != 0);
            EYELINK_GetFloatDataLatency.After();
            EYELINK_FrameExtract(&EYELINK_SampleData,TimeStamp,EyeXY,PupilSize);
            ready = TRUE;
            break;

        case STARTBLINK :  // Events
        case ENDBLINK :
        case STARTSACC :
        case ENDSACC :
        case STARTFIX :
        case ENDFIX :
        case FIXUPDATE :
            EYELINK_EventFrequency.Loop();
            EYELINK_GetFloatDataLatency.Before();
            ok = ((rc=eyelink_get_float_data(&EYELINK_EventData)) != 0);
            EYELINK_GetFloatDataLatency.After();
            break;

        default :
            ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL EYELINK_FrameNext( double &TimeStamp, double EyeXY[], BOOL &ready )
{
BOOL ok;
double PupilSize;

    ok = EYELINK_FrameNext(TimeStamp,EyeXY,PupilSize,ready);

    return(ok);
}

/******************************************************************************/

