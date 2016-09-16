/******************************************************************************/

BOOL    FOB_Streaming( int ID )
{
BOOL    ok=TRUE;

    if( !FOB_Started(ID,FALSE) )
    {
        ok = FALSE;
    }
    else
    if( FOB_datamode != FOB_DATAMODE_STREAM )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Streaming( void )
{
BOOL    ok=TRUE;

    if( !FOB_Started(FOB_PORT_ALL,FALSE) )
    {
        ok = FALSE;
    }
    else
    if( FOB_datamode != FOB_DATAMODE_STREAM )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

int     FOB_StreamBird[FOB_BIRD_MAX] = { -1,-1,-1,-1,-1,-1,-1,-1 };

void    FOB_StreamTask( int ID )
{
static  BOOL  current=FALSE;
static  BYTE  buff[FOB_BUFF];
int     size,got,want,now,b,word,bird;
BOOL    ok,done;
WORD   *wptr;
BYTE   *bptr;

    // Nothing to do if not streaming or data collection not started...
    if( !FOB_Streaming(ID) )
    {
        return;
    }

    // Stop function re-entry. Are we currently in the function?
    if( current )
    {
        return;
    }

    // Currently executing function...
    current = TRUE; 

    // Timer used during development to time execution...
    FOB_Timer.Reset();

    // Count number of loops for each port...
    FOB_Port[ID].looprate->Loop();

    // Save last stage for each port...
    FOB_Port[ID].last = FOB_Port[ID].stage;

/*  // Check for com's timeout if appropriate...
    if( FOB_StageComs[FOB_Port[ID].stage.stage] )
    {
        elapsed = TIMER_elapsed(FOB_Port[ID].stage.coms_timer);

        if( elapsed > (2.0 * FOB_framemsec) )
        {
            FOB_errorf("FOB_StreamTask() FOB[%d] COM%d Timeout (%.2lf msec).\n",ID,FOB_Port[ID].comX,elapsed);
            TIMER_reset(FOB_Port[ID].stage.coms_timer);
        }
    } */

    // Stage specific processing...
    switch( FOB_Port[ID].stage )
    {
        case FOB_STAGE_START :
           FOB_Port[ID].stage++;
           break;

        case FOB_STAGE_WAITING :
           FOB_Port[ID].frametime->Before();

           if( !(ok=COM_Read(FOB_Port[ID].comH,&FOB_Port[ID].buff[0],1,done,0L)) )
           {
               FOB_Port[ID].error = TRUE;
               break;
           }

           if( !done )
           {
               break;
           }
           // Make sure first byte is a frame byte...
           if( FOB_FrameByte(FOB_Port[ID].buff[0]) )
           {
               FOB_Port[ID].got = 1;
               FOB_Port[ID].stage++;

               break;
           }

           // Sync error...
           FOB_Port[ID].sync++;
           break;

        case FOB_STAGE_DATA :
           // What we want is what we need less what we have got...
           want = FOB_Port[ID].size - FOB_Port[ID].got;
           now = 0;

           if( !(ok=COM_Read(FOB_Port[ID].comH,buff,want,now,done,0L)) )
           {
               FOB_Port[ID].error = TRUE;
               break;
           }

           for( b=0; (b < now); b++ )
           {
               FOB_Port[ID].buff[FOB_Port[ID].got++] = buff[b];
           }

           // Have we got a complete data record...
           if( FOB_Port[ID].got == FOB_Port[ID].size )
           {
if( TIMER_EveryHz(4.0) )
{
for( int i=0; (i < FOB_Port[ID].got); i++ ) 
{
//printf("%02X ",FOB_Port[ID].buff[i]);
}
//printf("\n");
}
               FOB_Port[ID].stage++;
           }
           break;

        case FOB_STAGE_RECORD :
           // There is something wrong with this when streaming multiple birds on a single port...
           bptr = (BYTE *)FOB_Port[ID].buff;

           for( b=0; (b < FOB_BirdsOnPort(ID)); b++ )
           {
               wptr = (WORD *)&FOB_Port[ID].buff[FOB_Port[ID].data[b]];

               bird = b;

               // If multiple birds on port, data can come in any order...
               if( FOB_MultiBirdPort(ID) && FOB_groupmode )
               {            
                   if( (bird=FOB_BirdIndex(ID,bptr[FOB_Port[ID].addr[b]])) == FOB_BIRD_NULL )
                   {
                       FOB_Port[ID].sync++;
                       continue;
                   }
               }

               FOB_StreamBird[b] = bird;  // This is for debugging...
               word = FOB_Data2Frame(FOB_Port[ID].dataformat[bird],wptr,&FOB_Port[ID].reading[bird]);
           }

           FOB_Port[ID].frame++;

           FOB_Port[ID].frametime->After();
           FOB_Port[ID].framerate->Loop();
           FOB_Port[ID].framelast->Reset();

           if( FOB_Port[ID].framedata )
           {
               FOB_Port[ID].overwrite++;
           }
           else
           {
               FOB_Port[ID].framedata = TRUE;
           }

           FOB_Port[ID].stage = FOB_STAGE_WAITING;
           break;
    }

    // Clear in function flag...
    current = FALSE;
}

/******************************************************************************/

void    FOB_LoopTask( void )
{
int     ID;

    for( ID=0; (ID < FOB_PortsInFlock()); ID++ )
    {
        FOB_StreamTask(ID);
    }
}

/******************************************************************************/

BOOL    FOB_StreamFresh( int ID )
{
BOOL    fresh=FALSE;

    // Multiple ports, so re-enter function for each port...
    if( ID == FOB_PORT_ALL )
    {
        for( fresh=TRUE,ID=0; (ID < FOB_PortsInFlock()); ID++ )
        {
            if( !FOB_StreamFresh(ID) )
            {
                fresh = FALSE;
            }
        }
    }
    else
    if( FOB_Streaming(ID) )
    {
        fresh = FOB_Port[ID].framedata;
    }

    return(fresh);
}

/******************************************************************************/

BOOL    FOB_StreamFresh( void )
{
BOOL    fresh;

    fresh = FOB_StreamFresh(FOB_PORT_ALL);

    return(fresh);
}

/******************************************************************************/

BOOL    FOB_PortError( int ID )
{
BOOL    error=FALSE;

    // Multiple ports, so re-enter function for each port...
    if( ID == FOB_PORT_ALL )
    {
        for( error=FALSE,ID=0; (ID < FOB_PortsInFlock()); ID++ )
        {
            if( FOB_Port[ID].error )
            {
                error = TRUE;
            }
        }
    }
    else
    {
        error = FOB_Port[ID].error;
    }

    return(error);
}

/******************************************************************************/

BOOL    FOB_PortError( void )
{
BOOL    error;

    error = FOB_PortError(FOB_PORT_ALL);

    return(error);
}

/******************************************************************************/

BOOL    FOB_LastData( double frameage[], BOOL &fresh, matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], matrix rtmx[] )
{
int     ID,flockbird,portbird;
BOOL    error;

    // An error on one or more ports so return...
    if( FOB_PortError() )
    {
        return(FALSE);
    }

    // Set fresh flag...
    fresh = FOB_StreamFresh();

    // Get data frame for each bird in flock...
    for( flockbird=0,ID=0; (ID < FOB_PortsInFlock()); ID++ )
    {
        // Request rate...
        FOB_Port[ID].requestrate->Loop();

        // Is a new frame wating...
        if( FOB_Port[ID].framedata )
        {
            frameage[ID] = FOB_Port[ID].framelast->Elapsed();
            FOB_Port[ID].framewait->Data(frameage[ID]);
            FOB_Port[ID].framedata = FALSE;
        }
        else
        {
            frameage[ID] = 0.0;
        }

        // Process data frame...
        for( portbird=0; (portbird < FOB_BirdsOnPort(ID)); portbird++,flockbird++ )
        {
            FOB_PutSPMX(ID,flockbird,FOB_Port[ID].dataformat[portbird],&FOB_Port[ID].reading[portbird],M(pomx,flockbird),M(aomx,flockbird),M(romx,flockbird),M(qtmx,flockbird),M(rtmx,flockbird));
        }
    }

    return(TRUE);
}

/******************************************************************************/

BOOL    FOB_LastData( double frameage[], matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], matrix rtmx[] )
{
BOOL    ok,fresh;

    ok = FOB_LastData(frameage,fresh,pomx,aomx,romx,qtmx,rtmx);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_LastData( double frameage[], BOOL &fresh, matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[] )
{
BOOL    ok;

    ok = FOB_LastData(frameage,fresh,pomx,aomx,romx,qtmx,NULL);

    return(ok);
}
/******************************************************************************/

BOOL    FOB_LastData( double frameage[], matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[] )
{
BOOL    ok,fresh;

    ok = FOB_LastData(frameage,fresh,pomx,aomx,romx,qtmx,NULL);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_LastData( BOOL &fresh, matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], matrix rtmx[] )
{
BOOL    ok;
double  frameage[FOB_BIRD_MAX];

    ok = FOB_LastData(frameage,fresh,pomx,aomx,romx,qtmx,rtmx);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_LastData( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], matrix rtmx[] )
{
BOOL    ok,fresh;
double  frameage[FOB_BIRD_MAX];

    ok = FOB_LastData(frameage,fresh,pomx,aomx,romx,qtmx,rtmx);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_LastData( BOOL &fresh, matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[] )
{
BOOL    ok;
double  frameage[FOB_BIRD_MAX];

    ok = FOB_LastData(frameage,fresh,pomx,aomx,romx,qtmx,NULL);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_LastData( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[] )
{
BOOL    ok,fresh;
double  frameage[FOB_BIRD_MAX];

    ok = FOB_LastData(frameage,fresh,pomx,aomx,romx,qtmx,NULL);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Data( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], matrix rtmx[], BOOL &fresh )
{
BOOL    ok=TRUE;

    fresh = FALSE;

    switch( FOB_datamode )
    {
        case FOB_DATAMODE_STREAM :
           if( (fresh=FOB_StreamFresh()) )
           {
               ok = FOB_LastData(pomx,aomx,romx,qtmx,rtmx);
           }
           break;

        case FOB_DATAMODE_POINT :
           if( (fresh=FOB_Timer.Expired(FOB_framemsec)) )
           {
               ok = FOB_GetData(pomx,aomx,romx,qtmx,rtmx);
           } 
           break;

        case FOB_DATAMODE_DRC :
        default :
           ok = FALSE;
           break;
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Data( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], BOOL &fresh )
{
BOOL    ok;

    ok = FOB_Data(pomx,aomx,romx,qtmx,NULL,fresh);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Data( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], matrix rtmx[] )
{
BOOL    ok,fresh;

    ok = FOB_Data(pomx,aomx,romx,qtmx,rtmx,fresh);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Data( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[] )
{
BOOL    ok,fresh;

    ok = FOB_Data(pomx,aomx,romx,qtmx,NULL,fresh);

    return(ok);
}

/******************************************************************************/

