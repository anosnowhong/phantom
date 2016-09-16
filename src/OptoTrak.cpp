/******************************************************************************/

matrix OptoTrakPosition[OPTO_MAX_MARKER];
BOOL   OptoTrakVisible[OPTO_MAX_MARKER];
BOOL   OptoTrakStarted=FALSE;
int    OptoTrakMarkers=0;
BOOL   OptoTrakMarkersActive=FALSE;
BOOL   OptoTrakWaiting=FALSE;
int    OptoTrakFrame;
int    OptoTrakFrameLast=-1;
STRING OptoTrakConfig="OPTOTRAK.CFG";
double OptoTrakLatency;
double OptoTrakFrequency;
TIMER_Interval  OptoTrakLatencyTimer("OptoTrakLatency");
TIMER_Frequency OptoTrakFrequencyTimer("OptoTrakFrequency");

/******************************************************************************/

void OptoTrakMarkersOn( void )
{
BOOL ok;

    if( (OptoTrakMarkers == 0) || !OptoTrakStarted )
    {
        return;
    }

    if( !OptoTrakMarkersActive )
    {
        ok = OPTO_Markers(ON);
        OptoTrakMarkersActive = TRUE;
    }
}

/******************************************************************************/

void OptoTrakMarkersOff( void )
{
BOOL ok;

    if( (OptoTrakMarkers == 0) || !OptoTrakStarted )
    {
        return;
    }

    if( OptoTrakMarkersActive )
    {
        ok = OPTO_Markers(OFF);
        OptoTrakMarkersActive = FALSE;
    }
}

/******************************************************************************/

void OptoTrakInitialize( void )
{
int i;

    for( i=0; (i < OptoTrakMarkers); i++ )
    {
        OptoTrakPosition[i].dim(SPMX_PTMX_MTX);
        OptoTrakVisible[i] = FALSE;
    }
}

/******************************************************************************/

BOOL OptoTrakStart( void )
{
BOOL ok;

    if( OptoTrakMarkers == 0 )
    {
        return(TRUE);
    }

    ok = OPTO_Start(OptoTrakConfig);

    if( ok )
    {
        if( OptoTrakMarkers != OPTO_Markers() )
        {
            printf("OptoTrak markers do not match.\n");
            OPTO_Stop();
            OPTO_Close();
            ok = FALSE;
        }

        OptoTrakStarted = TRUE;
        OptoTrakWaiting = FALSE;

        ok = OPTO_Markers(OFF);
        OptoTrakMarkersActive = FALSE;

        OptoTrakLatencyTimer.Reset();
        OptoTrakFrequencyTimer.Reset();
    }

    return(ok);
}

/******************************************************************************/

void OptoTrakStop( void )
{
    if( OptoTrakMarkers == 0 )
    {
        return;
    }

    OptoTrakStarted = FALSE;
    OPTO_Stop();
    OPTO_Close();
}

/******************************************************************************/

void OptoTrakRead( void )
{
BOOL ok;

    if( (OptoTrakMarkers == 0) || !OptoTrakStarted )
    {
        return;
    }

    if( !OptoTrakWaiting )
    {
        if( TrialRunning )
        {
            OptoTrakMarkersOn();
        }
        else
        {
            OptoTrakMarkersOff();
        }

        ok = OPTO_RequestLatest();
        OptoTrakWaiting = TRUE;
    }

    if( OptoTrakWaiting )
    {
        OptoTrakLatencyTimer.Before();
        OptoTrakFrame = OPTO_GetPosn(OptoTrakPosition,OptoTrakVisible);
        OptoTrakLatency = OptoTrakLatencyTimer.After();

        if( OptoTrakFrame != 0 )
        {
            if( OptoTrakFrameLast != OptoTrakFrame )
            {
                OptoTrakFrameLast = OptoTrakFrame;
                OptoTrakFrequency = OptoTrakFrequencyTimer.Loop();
            }

            OptoTrakWaiting = FALSE;
        }
    }
}

/******************************************************************************/

void OptoTrakResults( void )
{
    if( OptoTrakMarkers == 0 )
    {
        return;
    }

    OptoTrakLatencyTimer.Results();
    OptoTrakFrequencyTimer.Results();
}

/******************************************************************************/


