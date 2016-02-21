/******************************************************************************/
/* ROBOT: Event log (ROBOT-Log.cpp)                                           */
/******************************************************************************/

TIMER  ROBOT_LogTimer[ROBOT_MAX];
BOOL   ROBOT_LogOpenFlag[ROBOT_MAX];
long   ROBOT_LogFrameCount[ROBOT_MAX];
long   ROBOT_LogFrameMax[ROBOT_MAX];
STRING ROBOT_LogFileName[ROBOT_MAX];
matrix ROBOT_LogData[ROBOT_MAX];

/******************************************************************************/

BOOL ROBOT_LogOpen( int ID, long frames )
{
    if( !ROBOT_Check(ID) )
    {
        return(FALSE);
    }

    if( ROBOT_LogOpenFlag[ID] )
    {
        return(TRUE);
    }

    ROBOT_LogTimer[ID].Reset();

    if( frames == 0 )
    {
        ROBOT_LogFrameMax[ID] = ROBOT_LOG_FRAMES;
    }
    else
    {
        ROBOT_LogFrameMax[ID] = frames;
    }

    ROBOT_LogFrameCount[ID] = 0;
    strncpy(ROBOT_LogFileName[ID],STR_stringf("%s.LOG",ROBOT_KeyWord[ID]),STRLEN);

    ROBOT_LogData[ID].dim(ROBOT_LogFrameMax[ID],ROBOT_LOG_COLUMNS);

    ROBOT_LogOpenFlag[ID] = TRUE;

    ROBOT_messgf("%s: Open Event Log (file=%s, frames=%ld).\n",ROBOT_KeyWord[ID],ROBOT_LogFileName[ID],ROBOT_LogFrameMax[ID]);

    return(TRUE);
}

/******************************************************************************/

BOOL ROBOT_LogOpen( int ID )
{
BOOL ok;
long frames=0;

    ok = ROBOT_LogOpen(ID,frames);

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_LogClose( int ID )
{
BOOL ok=TRUE;

    if( !ROBOT_LogOpenFlag[ID] )
    {
        return(TRUE);
    }

    ROBOT_LogOpenFlag[ID] = FALSE;

    if( ROBOT_LogFrameCount[ID] > 0 )
    {
        ROBOT_messgf("%s: Writing Event Log (file=%s, frames=%ld).\n",ROBOT_KeyWord[ID],ROBOT_LogFileName[ID],ROBOT_LogFrameCount[ID]);
        ok = matrix_write(ROBOT_LogFileName[ID],ROBOT_LogData[ID],ROBOT_LogFrameCount[ID]);
    }

    return(ok);
}

/******************************************************************************/

void ROBOT_LogEvent( int ID, int event, int interval )
{
int row,col;
//static long lastcount=-1L;
//static double lasttime=0.0,timestep=0.0;

    if( !ROBOT_Check(ID) || !ROBOT_LogOpenFlag[ID] )
    {
        return;
    }

    if( ROBOT_LogFrameCount[ID] >= ROBOT_LogFrameMax[ID] )
    {
        return;
    }

    /*if( lastcount != ROBOT_LoopTaskCount[ID] )
    {
        lastcount = ROBOT_LoopTaskCount[ID];
        lasttime += 0.200;
        timestep = 0.0;
    }
    else
    {
        timestep += 0.002;
    }*/

    row = ++ROBOT_LogFrameCount[ID];

    col = 0;
    ROBOT_LogData[ID](row,++col) = (double)ROBOT_LoopTaskCount[ID];
    ROBOT_LogData[ID](row,++col) = ROBOT_LogTimer[ID].Elapsed(); // lasttime + timestep
    ROBOT_LogData[ID](row,++col) = (double)event;
    ROBOT_LogData[ID](row,++col) = (double)interval;
}

/******************************************************************************/

void ROBOT_LogEvent( int ID, int event )
{
    ROBOT_LogEvent(ID,event,ROBOT_LOG_EVENT);
}

/******************************************************************************/

void ROBOT_LogBefore( int ID, int event )
{
    ROBOT_LogEvent(ID,event,ROBOT_LOG_BEFORE);
}

/******************************************************************************/

void ROBOT_LogAfter( int ID, int event )
{
    ROBOT_LogEvent(ID,event,ROBOT_LOG_AFTER);
}

/******************************************************************************/

