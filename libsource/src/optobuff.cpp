/******************************************************************************/
/*                                                                            */ 
/* MODULE  : OPTObuff.cpp                                                     */ 
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
/* V2.5  JNI 09/Jul/2002 - Some changes to buffered collection. Specifically  */ 
/*                         to deal with the problem where the OptoTrak API    */ 
/*                         takes "forever" to finish spooling data.           */ 
/*                                                                            */ 
/******************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

int      OPTO_BufferStage=OPTO_BUFFER_NONE;      // Buffering stage.
int      OPTO_BufferType;                        // Buffering type (file or memory).
ULONG    OPTO_BufferFrame;                       // Number of frames collected.
OPTOMEM *OPTO_Buffer;                            // Pointer to memory-buffered block.
OPTOMEM  OPTO_Buff;                              // Default memory block information.

/******************************************************************************/

char *OPTO_BufferText[] =
{
    "File",
    "Memory",
    NULL,
};

/******************************************************************************/

char    *OPTO_FileDefault( char *file )          // V2.3
{
    if( file == OPTO_FILE_NULL )
    {
        file = OPTO_FILE_DEFAULT;
    }

    return(file);
}
/******************************************************************************/

OPTOMEM *OPTO_MemoryDefault( OPTOMEM *buff )     // V2.3
{
    if( buff == OPTO_BUFF_NULL )
    {
        buff = OPTO_BUFF_DEFAULT;
    }

    return(buff);
}

/******************************************************************************/

BOOL    OPTO_BufferSetup( float freq, int m1, int m2, int m3, int m4, float ct, int flag, int type, void *buff )
{
BOOL    ok=FALSE;
char   *file;
int     rc;

    if( !OPTO_API_check() )            // Has the API been started?
    {
        return(FALSE);
    }
 
    if( OPTO_BufferStage != OPTO_BUFFER_NONE )
    {
        OPTO_errorf("OPTO_BufferSetup(...) Buffering already in progress.\n");
        return(FALSE);
    }
 
    if( !OPTO_Start(freq,m1,m2,m3,m4,ct,OPTO_RTMX_NONE,flag) )
    {
        OPTO_errorf("OPTO_BufferSetup(...) Cannot start.\n");
        return(FALSE);
    }
 
    switch( type )
    {
        case OPTO_BUFFER_FILE :
            if( buff == NULL )
            {
                OPTO_errorf("OPTO_BufferSetup(...) NULL file name.\n");
                ok = FALSE;
                break;
            }

            file = (char *)buff;

            if( (rc=DataBufferInitializeFile(OPTOTRAK,file)) == OPTO_RC_OK )
            {
 
                OPTO_debugf("DataBufferInitializeFile(file=%s) OK.\n",file);
                ok = TRUE;
            }
            else
            {
                OPTO_Fail("DataBufferInitializeFile",rc);
            }
            break;
 
        case OPTO_BUFFER_MEMORY :
            if( buff == NULL )
            {
                OPTO_errorf("OPTO_BufferSetup(...) NULL memory buffer.\n");
                ok = FALSE;
                break;
            }

            if( (rc=DataBufferInitializeMem(OPTOTRAK,buff)) == OPTO_RC_OK )
            {
                OPTO_debugf("DataBufferInitializeMem(...) OK.\n");
                ok = TRUE;
            }
            else
            {
                OPTO_Fail("DataBufferInitializeMem",rc);
            }
            break;
    }

    STR_printf(ok,OPTO_debugf,OPTO_errorf,"OPTO_BufferSetup(...) %s.\n",STR_OkFailed(ok));

    if( ok )
    {
        OPTO_BufferStage = OPTO_BUFFER_SETUP;
        OPTO_BufferType = type;
        OPTO_BufferFrame = 0;
    }
 
    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferFile( float freq, int m1, int m2, int m3, int m4, float ct, int flag, char *file )
{
BOOL    ok;

    file = OPTO_FileDefault(file);     // Set default file name. (V2.3)
 
    ok = OPTO_BufferSetup(freq,m1,m2,m3,m4,ct,flag,OPTO_BUFFER_FILE,file);

    return(ok);
}


/******************************************************************************/

BOOL    OPTO_BufferFile( float freq, int m1, int m2, int m3, int m4, float ct, int flag )
{
BOOL    ok;
 
    ok = OPTO_BufferFile(freq,m1,m2,m3,m4,ct,flag,OPTO_FILE_NULL);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferFile( float freq, int m[], float ct, int flag, char *file )
{
BOOL    ok;
 
    ok = OPTO_BufferFile(freq,m[0],m[1],m[2],m[3],ct,flag,file);

    return(ok);
}


/******************************************************************************/

BOOL    OPTO_BufferFile( float freq, int m[], float ct, int flag )
{
BOOL    ok;
 
    ok = OPTO_BufferFile(freq,m[0],m[1],m[2],m[3],ct,flag);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferFile( int m[], float ct, char *file )
{
BOOL    ok;
 
    ok = OPTO_BufferFile(OPTO_DEFAULT_FREQ,m[0],m[1],m[2],m[3],ct,OPTO_DEFAULT_FLAG,file);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferFile( int m[], float ct )
{
BOOL    ok;
 
    ok = OPTO_BufferFile(OPTO_DEFAULT_FREQ,m[0],m[1],m[2],m[3],ct,OPTO_DEFAULT_FLAG);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferFile( char *cnfg )
{
BOOL    ok=FALSE;

    if( OPTO_Cnfg(cnfg) )
    {
        ok = OPTO_BufferFile();
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferFile( void )
{
BOOL    ok;

    ok = OPTO_BufferFile(OPTO_Frequency, 
                         OPTO_PortMarker[0],
                         OPTO_PortMarker[1],
                         OPTO_PortMarker[2],
                         OPTO_PortMarker[3],
                         OPTO_CollectionTime,
                         OPTO_Flags);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferMemory( char *cnfg )
{
BOOL    ok=FALSE;

    if( OPTO_Cnfg(cnfg) )
    {
        ok = OPTO_BufferMemory();
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferMemory( void )
{
BOOL    ok;

    ok = OPTO_BufferMemory(OPTO_Frequency, 
                           OPTO_PortMarker[0],
                           OPTO_PortMarker[1],
                           OPTO_PortMarker[2],
                           OPTO_PortMarker[3],
                           OPTO_CollectionTime,
                           OPTO_Flags);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferMemory( float freq, int m1, int m2, int m3, int m4, float ct, int flag, OPTOMEM *buff )
{
BOOL    ok;
int     markers,size;

    buff = OPTO_MemoryDefault(buff);   // Set default memory buffer. (V2.3)
 
    markers = m1 + m2 + m3 + m4;

    // Initialize memory-buffered information structure...
    memset(buff,0,sizeof(OPTOMEM));
    buff->freq = freq;
    buff->ct = ct;
    buff->frames = OPTO_BufferFrames(freq,ct);
    buff->collected = 0;
    buff->markers = markers;

    // Allocate enough memory (determined by frequency, collection time and markers)...
    if( (buff->data=malloc(size=OPTO_BufferTotalBytes(freq,ct,markers))) == NULL )
    {
        OPTO_errorf("OPTO_BufferMemory() Cannot allocate %u bytes.\n",size);
        ok = FALSE;
    }
    else
    {
        OPTO_debugf("OPTO_BufferMemory() %u bytes allocated.\n",size);

        // Setup memory-buffered collection...
        if( (ok=OPTO_BufferSetup(freq,m1,m2,m3,m4,ct,flag,OPTO_BUFFER_MEMORY,buff->data)) )
        {
            OPTO_Buffer = buff;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferMemory( float freq, int m1, int m2, int m3, int m4, float ct, int flag )
{
BOOL    ok;

    ok = OPTO_BufferMemory(freq,m1,m2,m3,m4,ct,flag,OPTO_BUFF_NULL);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferMemory( float freq, int m[], float ct, int flag, OPTOMEM *buff )
{
BOOL    ok;

    ok = OPTO_BufferMemory(freq,m[0],m[1],m[2],m[3],ct,flag,buff);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferMemory( float freq, int m[], float ct, int flag )
{
BOOL    ok;

    ok = OPTO_BufferMemory(freq,m[0],m[1],m[2],m[3],ct,flag,OPTO_BUFF_NULL);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferMemory( int m[], float ct, OPTOMEM *buff )
{
BOOL    ok;

    ok = OPTO_BufferMemory(OPTO_DEFAULT_FREQ,m[0],m[1],m[2],m[3],ct,OPTO_DEFAULT_FLAG,buff);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferMemory( int m[], float ct )
{
BOOL    ok;

    ok = OPTO_BufferMemory(OPTO_DEFAULT_FREQ,m[0],m[1],m[2],m[3],ct,OPTO_DEFAULT_FLAG,OPTO_BUFF_NULL);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferStart( void )
{
BOOL    ok=FALSE;
int     rc;

    if( !OPTO_API_check() )            // Has the API been started?
    {
        return(FALSE);
    }

    if( OPTO_BufferStage != OPTO_BUFFER_SETUP )
    {
        OPTO_errorf("OPTO_BufferStart() Buffering not set up.\n");
        return(FALSE);
    }

    if( (rc=DataBufferStart()) == OPTO_RC_OK )
    {
        OPTO_debugf("DataBufferStart()  OK.\n");
        ok = TRUE;
    }
    else
    {
        OPTO_Fail("DataBufferStart",rc);
    }

    if( ok )
    {
        OPTO_debugf("OPTO_BufferSart() OK.\n");
        OPTO_BufferStage = OPTO_BUFFER_RUNNING;
    }
    else
    {
        OPTO_errorf("OPTO_BufferStart() Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferStop( BOOL terminate )
{
BOOL    ok=TRUE;
int     rc;

    // Has the API been started...
    if( !OPTO_API_check() )
    {
        return(FALSE);
    }

    // Buffered collection must be running or completed...
    if( OPTO_BufferStage < OPTO_BUFFER_RUNNING )
    {
        OPTO_errorf("OPTO_BufferStop() Not running.\n");
        return(FALSE);
    }

    // If buffered collection running, stop it...
    if( OPTO_BufferStage == OPTO_BUFFER_RUNNING )
    {
        if( terminate ) // Stop buffered collection now...
        {
            // Stop data buffering now...
            if( (rc=DataBufferStop()) == OPTO_RC_OK )
            {
                OPTO_debugf("DataBufferStop() OK.\n");
            }
            else
            {
                OPTO_Fail("DataBufferStop",rc);
                ok = FALSE;
            }
        }

        OPTO_debugf("OPTO_BufferStop() Waiting for spooling to complete.\n");
        // Wait for data remaining in buffer to be transfered...
        if( !OPTO_BufferWait() )
        {
            OPTO_errorf("OPTO_BufferStop(terminate=%s) Buffer wait failed.\n",STR_YesNo(terminate));
            ok = FALSE;
        }
    }

    STR_printf(ok,OPTO_debugf,OPTO_errorf,"OPTO_BufferStop(terminate=%s) %s.\n",STR_YesNo(terminate),STR_OkFailed(ok));
    OPTO_BufferStage = OPTO_BUFFER_NONE;
    OPTO_Stop();

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferComplete( void )
{
BOOL    ok;

//  Stop buffering, terminate flag = FALSE...
    ok = OPTO_BufferStop(FALSE);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferStopNow( void )
{
BOOL    ok;

//  Stop buffering, terminate flag = TRUE...
    ok = OPTO_BufferStop(TRUE);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferUpdate( BOOL *RTD, BOOL *SC, BOOL *SS, ULONG *FB )
{
UINT    RealTimeData=FALSE,SpoolComplete=FALSE,SpoolStatus=FALSE;
ULONG   FramesBuffered=0L;
BOOL    ok=TRUE;
int     rc;

    if( !OPTO_API_check() )            // Has the API been started?
    {
        return(FALSE);
    }

    if( SC == NULL )
    {
        OPTO_errorf("OPTO_BufferUpdate() Invalid NULL parameter.\n");
        return(FALSE);
    }

    if( OPTO_BufferStage == OPTO_BUFFER_DONE )        // Buffering done...
    {
        OPTO_debugf("OPTO_BufferUpdate() Complete.\n");
        SpoolComplete = TRUE;
    }
    else
    if( OPTO_BufferStage != OPTO_BUFFER_RUNNING )     // Not running...
    {
        OPTO_errorf("OPTO_BufferUpdate() Not running.\n");
        ok = FALSE;
    }
    else                                              // Running, so spool data...
    if( (rc=DataBufferWriteData(&RealTimeData,        // Real time data is ready.
                                &SpoolComplete,       // Spool is complete.
                                &SpoolStatus,         // Spool status.
                                &FramesBuffered       // Number of frames buffered.
                                )) != OPTO_RC_OK )
    {
        OPTO_Fail("DataBufferWriteData",rc);
        ok = FALSE;
    }
    else
    if( SpoolStatus )   // Check Spool Status value (V2.5)...
    {
        OPTO_errorf("DataBufferWriteData(...) Spool Status Error.\n");
        ok = FALSE;
    }

    if( !ok )
    {
        return(FALSE);
    }

    OPTO_BufferFrame = FramesBuffered;

    if( OPTO_BufferType == OPTO_BUFFER_MEMORY )
    {
        OPTO_Buffer->collected = (int)FramesBuffered;
    }

    if( SpoolComplete )
    {
        OPTO_BufferStage = OPTO_BUFFER_DONE;
    }

    if( RTD != NULL )
    {
       *RTD = (BOOL)RealTimeData;
    }

    if( SC != NULL )
    {
       *SC = (BOOL)SpoolComplete;
    }

    if( SS != NULL )
    {
       *SS = (BOOL)SpoolStatus;
    }

    if( FB != NULL )
    {
       *FB = FramesBuffered;
    }

    return(TRUE);
}

/******************************************************************************/

BOOL    OPTO_BufferUpdate( BOOL *SC, ULONG *FB )
{
BOOL    ok;

    ok = OPTO_BufferUpdate(NULL,SC,NULL,FB);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferUpdate( BOOL *SC )
{
BOOL    ok;

    ok = OPTO_BufferUpdate(NULL,SC,NULL,NULL);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferUpdate( void )
{
BOOL    ok,SC;

    ok = OPTO_BufferUpdate(NULL,&SC,NULL,NULL);

    return(ok && !SC);
}

/******************************************************************************/

BOOL    OPTO_BufferWait( double seconds )                            // V2.5
{
TIMER   timer("OPTO WAIT");
BOOL    ok=TRUE;

    timer.Reset();

    while( OPTO_BufferUpdate() && ok )
    {
        if( timer.ExpiredSeconds(seconds) )
        {
            OPTO_errorf("OPTO_BufferWait(%.2lf seconds) Time out.\n",seconds);
            ok = FALSE;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_BufferWait( void )                                      // V2.5
{
BOOL    ok;

    ok = OPTO_BufferWait(OPTO_BUFFER_WAIT);

    return(ok);
}

/******************************************************************************/

void    OPTO_BufferFree( OPTOMEM *buff )
{
    if( buff->data != NULL )
    {
        free(buff->data);
        buff->data = NULL;
    }
}

/******************************************************************************/

int     OPTO_BufferFrames( float freq, float ct )
{
int     total;

    // Number of frames = frame frequency * collection time.
    total = (int)(freq * ct);

    return(total);
}

/******************************************************************************/

int     OPTO_BufferFloatsPerFrame( int markers )
{
int     total;

    // Floats per frame = markers * floats per marker (XYZ = 3).
    total = markers * 3;

    return(total);
}

/******************************************************************************/

int     OPTO_BufferTotalFloats( float freq, float ct, int markers )
{
int     total;

    // Number of floats = number of frames * floats per frame.
    total = OPTO_BufferFrames(freq,ct) * OPTO_BufferFloatsPerFrame(markers);

    return(total);
}

/******************************************************************************/

int     OPTO_BufferTotalBytes( float freq, float ct, int markers )
{
int     total;

    // Size of memory required = number of floats * size of ND float.
    total = OPTO_BufferTotalFloats(freq,ct,markers) * sizeof(RealType);

    return(total);
}

/******************************************************************************/

void    OPTO_BufferDimensions( float freq, float ct, int markers, int &row, int &col )
{
    // Each frame of data is stored on a separate row.
    row = OPTO_BufferFrames(freq,ct);

    //  Floats for frame stored in columns.
    col = OPTO_BufferFloatsPerFrame(markers);
}

/******************************************************************************/

BOOL (*OPTO_BufferSetupFunc[])( char *cnfg ) =
{
    OPTO_BufferFile,
    OPTO_BufferMemory,
};

/******************************************************************************/

BOOL OPTO_BufferSetup( int type, char *cnfg )
{
BOOL ok;

    ok = (*OPTO_BufferSetupFunc[type])(cnfg);

    return(ok);
}

/******************************************************************************/

