/******************************************************************************/
/*                                                                            */ 
/* MODULE  : OPTO.cpp                                                         */ 
/*                                                                            */ 
/* PURPOSE : Main interface functions for OptoTrak Position Sensor System.    */ 
/*                                                                            */ 
/* DATE    : 08/Jun/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V2.0  JNI 08/Jun/2000 - Re-developed from "mylib.lib" module of like name. */ 
/*                                                                            */ 
/* V2.1  JNI 06/Dec/2000 - Put delay in OPTO_Start(...) function. This fixed  */ 
/*                         problem with initial position readings in the new  */ 
/*                         PCI/ISA Northern Digital libraries.                */ 
/*                                                                            */ 
/* V2.2  JNI 14/Feb/2001 - Testing led to improvements & bug fixes in file /  */ 
/*                         memory buffering & data conversion modules.       */ 
/*                                                                            */ 
/* V2.3  JNI 25/Apr/2001 - Changed to use default file names and buffers.     */ 
/*                                                                            */ 
/* V2.4  JNI 18/Jan/2002 - Made some changes to increase consistency with     */ 
/*                         other related APIs. Added Open & Close functions.  */ 
/*                         Added configuration file options.                  */ 
/*                                                                            */ 
/* V2.5  JNI 09/Jul/2002 - Some changes to buffered collection.               */ 
/*                                                                            */ 
/* V2.6  JNI 09/Mar/2010 - Non-Blocking read implemented to solve problem     */ 
/*                         of long latencies associated with blocking read.   */ 
/*                                                                            */ 
/*       JNI 16/Mar/1020 - Add critical sections to make multi-thread safe.   */ 
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     "OPTO"
#define MODULE_TEXT     "OptoTrak API"
#define MODULE_DATE     "09/07/2002"
#define MODULE_VERSION  "2.5"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

// #define OPTO_DEBUGF

/******************************************************************************/

BOOL    OPTO_API_started = FALSE;                // API started flag.
BOOL    OPTO_MarkersOn = FALSE;                  // Markers active flag.
BOOL    OPTO_Opened = FALSE;                     // OptoTrak open flag. (V2.4)
BOOL    OPTO_StartFlag = FALSE;                  // Collection started flag.
BOOL    OPTO_ReadWaiting = FALSE;                // Non-Blocking read waiting flag. (V2.6)
float   OPTO_CT;                                 // Collection time.
float   OPTO_FF;                                 // Frame frequency.
float   OPTO_MF;                                 // Marker frequency.
char    OPTO_ErrorString[OPTO_ERROR_STRING+1];   // OptoTrak API error string.
ND3D    OPTO_nd3d[OPTO_MAX_MARKER];              // Raw OptoTrak x,y,z positions.

struct  OPTO_MarkerItem      OPTO_MarkList[OPTO_MAX_MARKER];
int                          OPTO_PortList[OPTO_MAX_PORT];

// Critical section used to make code multi-thread safe. (V2.6)
CRITICAL_SECTION OPTO_CriticalSection;           

/******************************************************************************/

float   OPTO_Frequency=OPTO_DEFAULT_FREQ;
int     OPTO_PortMarker[OPTO_MAX_PORT]= { 0,0,0,0 };
float   OPTO_CollectionTime=OPTO_DEFAULT_CT;
STRING  OPTO_RTFN=OPTO_DEFAULT_RTMX;
int     OPTO_Flags=OPTO_DEFAULT_FLAG;
int     OPTO_ReadType=OPTO_READ_BLOCKING; // Blocking versus Non-blocking read (V2.6)

/******************************************************************************/

void OPTO_ThreadLock( void )
{
BOOL ok;

return;

    do
    {
        ok = TryEnterCriticalSection(&OPTO_CriticalSection);
    }
    while( !ok );
}

/******************************************************************************/

void OPTO_ThreadUnlock( void )
{
return;
    LeaveCriticalSection(&OPTO_CriticalSection);
}

/******************************************************************************/

BOOL OPTO_Cnfg( void )
{
BOOL ok;

    ok = OPTO_Cnfg(NULL);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Cnfg( char *cnfg )
{
STRING  file=OPTO_CONFIG_FILE;
char   *path;

    if( !STR_null(cnfg) )
    {
        strncpy(file,cnfg,STRLEN);
    }

    if( (path=FILE_Calibration(file)) == NULL )
    {
        OPTO_errorf("OPTO_Cnfg(file=%s) File not found.\n",file);
        return(FALSE);
    }

    CONFIG_reset();

    CONFIG_set("Frequency",OPTO_Frequency);
    CONFIG_set("Markers",OPTO_PortMarker,OPTO_MAX_PORT);
    CONFIG_set("CollectionTime",OPTO_CollectionTime);
    CONFIG_set("RTMX",OPTO_RTFN);
    CONFIG_set("Flags",OPTO_Flags);
    CONFIG_set("ReadType",OPTO_ReadType);

    if( !CONFIG_read(path) )      // Load configuration file...
    {
        OPTO_errorf("OPTO_Cnfg(%s) Cannot read file.\n",path);
        return(FALSE);
    }

    OPTO_messgf("OPTO_Cnfg(%s) Loaded.\n",path);
    CONFIG_list(OPTO_messgf);

    return(TRUE);
}

/******************************************************************************/
//      OptoTrak (camara) centred co-ordinates are usually translated to user-defined
//      co-ordinate system. The Rotation / Translation MatriX (RTMX, below) does this.

matrix  OPTO_RTMX;      // Rotation / Translation matrix for co-ordinate system.

/******************************************************************************/

PRINTF  OPTO_PRN_messgf=NULL;                    // General messages printf function.
PRINTF  OPTO_PRN_errorf=NULL;                    // Error messages printf function.
PRINTF  OPTO_PRN_debugf=NULL;                    // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     OPTO_messgf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(OPTO_PRN_messgf,buff));
}

/******************************************************************************/

int     OPTO_errorf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(OPTO_PRN_errorf,buff));
}

/******************************************************************************/

int     OPTO_debugf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(OPTO_PRN_debugf,buff));
}

/******************************************************************************/

void    OPTO_Fail( char *func, int rc )
{
int     ESrc;

    if( (ESrc=OptotrakGetErrorString(OPTO_ErrorString,OPTO_ERROR_STRING)) != OPTO_RC_OK )
    {
        OPTO_errorf("OptotrakGetErrorString(...) rc=0x%04X Failed.\n",ESrc);
        strncpy(OPTO_ErrorString,"Unknown",OPTO_ERROR_STRING);
    }

    OPTO_errorf("%s(...) rc=0x%04X ERROR: %s\n",rc,OPTO_ErrorString);
}

/******************************************************************************/

BOOL    OPTO_SetRTMX( char *file )
{
char   *rtmx;
BOOL    ok;

    // Clear RTMX...
    OPTO_RTMX.empty();

    // No RTMX file required...
    if( STR_null(file) )
    {
        OPTO_debugf("OPTO_SetRTMX(NULL).\n");
        return(TRUE);
    }

    // Find RTMX (calibration) file...
    if( (rtmx=FILE_Calibration(file)) == NULL )
    {
        OPTO_errorf("OPTO_SetRTMX(%s) Cannot find file.\n",file);
        return(FALSE);
    }

    // Load file into R/T matrix...
    if( (ok=matrix_read(rtmx,OPTO_RTMX,SPMX_RTMX_DIM)) )
    {
        OPTO_debugf("OPTO_SetRTMX(%s) Loaded.\n",rtmx);
    }
    else
    {
        OPTO_errorf("OPTO_SetRTMX(%s) Cannot load.\n",rtmx);
    }

    return(ok);
}

/******************************************************************************/

void    OPTO_RT( matrix &posn )
{
static  matrix  temp;

    // If no RTMX martix loaded, there is nothing to do...
    if( OPTO_RTMX.isempty() )
    {
        return;
    }

    // If RTMX martix is full of zeros, there is nothing to do...
    if( OPTO_RTMX.iszero() )
    {
        return;
    }

    // Copy position matrix adding 4th row if required...
    if( posn.rows() == SPMX_POMX_ROW )
    {
        SPMX_pomx2ptmx(posn,temp);
    }
    else
    if( posn.rows() == SPMX_PTMX_ROW )
    {
        matrix_copy(temp,posn);
    }
    else
    {
        OPTO_errorf("OPTO_RT(...) Invalid number of rows.\n");
        return;
    }

    matrix_multiply(posn,OPTO_RTMX,temp);
}

/******************************************************************************/

void    OPTO_RT( matrix posn[], int markers )
{
int     m;

    // If no RTMX martix loaded there is nothing to do...
    if( OPTO_RTMX.isempty() )
    {
        return;
    }

    // Loop through each marker individually...
    for( m=0; (m < markers); m++ )
    {
        OPTO_RT(posn[m]);
    }
}

/******************************************************************************/

BOOL    OPTO_Markers( BOOL activate )
{
int     rc;
BOOL    ok=TRUE;

    if( !OPTO_API_check() )                 // Has the API been started?
    {
        return(FALSE);
    }

    OPTO_ThreadLock();                     // Multi-thread safe. (V2.6).

    if( activate && !OPTO_MarkersOn )      // Turn them ON...
    {
        if( (rc=OptotrakActivateMarkers()) != OPTO_RC_OK )
        {
            OPTO_Fail("OptotrakActivateMarkers",rc);
            ok = FALSE;
        }
        else
        {
            OPTO_MarkersOn = TRUE;
            OPTO_debugf("Markers turned ON.\n");
        }
    }
    else
    if( !activate && OPTO_MarkersOn )      // Turn them OFF...
    {
        if( (rc=OptotrakDeActivateMarkers()) != OPTO_RC_OK )
        {
            OPTO_Fail("OptotrakDeActivateMarkers",rc);
            ok = FALSE;
        }
        else
        {
            OPTO_MarkersOn = FALSE;
            OPTO_debugf("Markers turned OFF.\n");
        }
    }

    OPTO_ThreadUnlock();                   // Multi-thread safe. (V2.6).

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Port( int port )
{
BOOL    ok;

    ok = (port >= 1) && (port <= OPTO_MAX_PORT);

    return(ok);
}

/******************************************************************************/

int    *OPTO_Markers( int port, int marker, ... )
{
static  int  portlist[OPTO_MAX_MARKER];
static  int  marklist[OPTO_MAX_MARKER];
int     item,p;
BOOL    ok;
va_list argp;

    for( item=0; (item < OPTO_MAX_MARKER); item++ )
    {
        portlist[item] = OPTO_PORT_NULL;
        marklist[item] = OPTO_MARKER_NULL;
    }

    item = 0;
    portlist[item] = port;
    marklist[item] = marker;
    item++;

    va_start(argp,marker);
 
    do
    {
        p = va_arg(argp,int);
        
        if( (ok=OPTO_Port(p)) )
        {
            portlist[item] = p;
            marklist[item] = va_arg(argp,int);
            item++;
        }
    }
    while( ok );

    va_end(argp);

    return(OPTO_Markers(portlist,marklist));
}

/******************************************************************************/

int    *OPTO_Markers( int port[], int marker[] )
{
int     p,m;

    for( p=0; (p < OPTO_MAX_PORT); p++ )
    {
        OPTO_PortList[p] = OPTO_PORT_NULL;
    }

    for( m=0; (m < OPTO_MAX_MARKER); m++ )
    {
        OPTO_MarkList[m].port = OPTO_PORT_NULL;
        OPTO_MarkList[m].marker = OPTO_MARKER_NULL;
    }

    for( m=0; (port[m] != OPTO_PORT_NULL); m++ )
    {
        p = port[m];

        if( !OPTO_Port(p) )
        {
            break;
        }

        OPTO_MarkList[m].port = p;
        OPTO_MarkList[m].marker = marker[m];

        p--;

        if( marker[m] > OPTO_PortList[p] )
        {
            OPTO_PortList[p] = marker[m];
        }
    }

#ifdef OPTO_DEBUGF
    OPTO_debugf("OPTO_Markers(...);\n");

    OPTO_debugf("PORT");
    for( p=0; (p < OPTO_MAX_PORT); p++ )
    {
        OPTO_debugf(" %1d:%d",p,OPTO_PortList[p]);
    }
    OPTO_debugf("\n");

    for( m=0; ((m < OPTO_MAX_MARKER) && (OPTO_MarkList[m].port != OPTO_PORT_NULL)); m++ )
    {
        OPTO_debugf("M[%02d] %1d:%02d\n",m,OPTO_MarkList[m].port,OPTO_MarkList[m].marker);
    }
#endif

    return(OPTO_PortList);
}

/******************************************************************************/

int     OPTO_Markers( void )
{
int     markers,p;

    for( markers=0,p=0; (p < OPTO_MAX_PORT); markers+=OPTO_PortList[p++] );

#ifdef OPTO_DEBUGF
    OPTO_debugf("%d marker(s) specified.\n",markers);
#endif

    return(markers);
}

/******************************************************************************/

int     OPTO_Index( int port, int marker )
{
int     index,p;

    port--;

    for( index=marker,p=0; (p < port); index+=OPTO_PortList[p++] );

    return(index);
}

/******************************************************************************/

void    OPTO_Index( int index, int &port, int &marker )
{
int     p,m;

    for( p=1; (p <= OPTO_MAX_PORT); p++ )
    {
        for( m=1; (m <= OPTO_PortList[p]); m++ )
        {
            if( OPTO_Index(p,m) == index )
            {
                port = p;
                marker = m;
                break;
            }
        }
    }
}

/******************************************************************************/

BOOL    OPTO_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok=TRUE;

    // Start API only once...
    if( OPTO_API_started )
    {
        return(TRUE);
    }

    OPTO_PRN_messgf = messgf;          // General API message print function.
    OPTO_PRN_errorf = errorf;          // API error message print function.
    OPTO_PRN_debugf = debugf;          // Debug information print function.

    OPTO_MarkersOn = FALSE;

    InitializeCriticalSection(&OPTO_CriticalSection);

    if( ok )
    {
        ATEXIT_API(OPTO_API_stop);     // Install stop function.
        OPTO_API_started = TRUE;       // Set started flag.
        MODULE_start(OPTO_PRN_messgf); // Register module.
    }
    else
    {
        OPTO_errorf("OPTO_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    OPTO_API_stop( void )
{
int     rc;

    // Was API started in the first place...
    if( !OPTO_API_started )
    {
         return;
    }

    // Stop and Close OptoTrak system...
    OPTO_Stop();
    OPTO_Close();

    DeleteCriticalSection(&OPTO_CriticalSection);

    // Register module stop.
    MODULE_stop();                     
    OPTO_API_started = FALSE;
}

/******************************************************************************/

BOOL    OPTO_API_check( void )
{
BOOL    ok=TRUE;

    if( !OPTO_API_started )            // API not started...
    {                                  // Start module automatically...
        ok = OPTO_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        OPTO_debugf("OPTO_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_API_restart( void )
{
BOOL    ok;

    if( OPTO_API_started )
    {
        OPTO_API_stop();
        ok = OPTO_API_start(OPTO_PRN_messgf,OPTO_PRN_errorf,OPTO_PRN_debugf);
    }
    else
    {
        ok = OPTO_API_check();
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Open( char *cnfg )
{
BOOL    ok;
int     rc;

    // Make sure API running...
    if( !OPTO_API_check() )
    {
        return(FALSE);
    }

    // Load configuration file if required...
    if( cnfg != NULL )
    {
        ok = OPTO_Cnfg(cnfg);
        STR_printf(ok,OPTO_debugf,OPTO_errorf,"OPTO_Open(%s) %s.\n",cnfg,STR_OkFailed(ok));

        if( !ok )
        {
            return(FALSE);
        }
    }

    // Make sure OptoTrak not already opened...
    if( OPTO_Opened )
    {
        return(TRUE);
    }

    // Start OptoTrak system...
    if( (rc=TransputerInitializeSystem(0)) == OPTO_RC_OK )
    {
        OPTO_debugf("TransputerInitializeSystem(...) OK.\n");
        OPTO_Opened = TRUE;
        ok = TRUE;
    }
    else
    {
        OPTO_Fail("TransputerInitializeSystem",rc);
        ok = FALSE;
    }

    STR_printf(ok,OPTO_debugf,OPTO_errorf,"OPTO_Open(...) %s.\n",STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Open( void )
{
BOOL    ok;

    ok = OPTO_Open(NULL);
 
    return(ok);
}

/******************************************************************************/

void    OPTO_Close( void )
{
int     rc;

    // Make sure API running...
    if( !OPTO_API_check() )
    {
        return;
    }

    // Stop any active collection-session...
    OPTO_Stop();

    // Close OptoTrak only if it is opened...
    if( !OPTO_Opened )
    {
        return;
    }

    // Make sure markers are turned off.
    OPTO_Markers(OPTO_MARKERS_OFF);

    // Shutdown OptoTrak "Transputer" system...
    if( (rc=TransputerShutdownSystem()) == OPTO_RC_OK )    // Stop OptoTrak system...
    {
        OPTO_debugf("TransputerShutdownSystem(...) OK.\n");
    }
    else
    {
        OPTO_Fail("TransputerShutdownSystem",rc);
    }

    OPTO_Opened = FALSE;
}

/******************************************************************************/

BOOL    OPTO_Strober( int m1, int m2, int m3, int m4 )
{
BOOL    ok=TRUE;
int     p,markers,rc;

    // Save marker maximum for each strober port...
    OPTO_PortList[0] = m1;
    OPTO_PortList[1] = m2;
    OPTO_PortList[2] = m3;
    OPTO_PortList[3] = m4;

    // Count total markers...
    markers = OPTO_Markers();

    if( markers == 0 )
    {
        OPTO_errorf("OPTO_Strober(...) Marker count zero.\n");
        ok = FALSE;
    }
    else
    if( markers > OPTO_MAX_MARKER )
    {
        OPTO_errorf("OPTO_Strober(...) Too many markers (maximum %d).\n",OPTO_MAX_MARKER);
        ok = FALSE;
    }
    else
    if( (rc=OptotrakSetStroberPortTable(m1,m2,m3,m4)) == OPTO_RC_OK )
    {
        OPTO_debugf("OptotrakSetStroberPortTable(m1=%d,m2=%d,m3=%d,m4=%d) OK.\n",m1,m2,m3,m4);
    }
    else
    {
        OPTO_Fail("OptotrakSetStroberPortTable",rc);
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

#define OPTO_START_CAMERA    0         // Load camera parameters.
#define OPTO_START_RTMX      1         // Load Rotation / Translation MatriX file.
#define OPTO_START_STROBER   2         // Configure strober port table.
#define OPTO_START_COLLECT   3         // Configure collection parameters.
#define OPTO_START_MARKERS   4         // Turn markers on.
#define OPTO_START_DELAY     5         // Wait a minute...Put this in for PCI/ISA ND libraries (V2.1).

char   *OPTO_Step[] = { "CAMERA","RTMX","STROBER","COLLECT","MARKERS","DELAY",NULL };

BOOL    OPTO_Start( float freq, int m1, int m2, int m3, int m4, float ct, char *rtmx, int flag )
{
int     step;                          // The steps to start an OptoTrak collection are...
BOOL    ok;
int     rc,markers;

    // Make sure API is running...
    if( !OPTO_API_check() )
    {
        return(FALSE);
    }

    // Make sure OptoTrak is open...
    if( !OPTO_Opened )
    {
        if( !OPTO_Open() )
        {
            return(FALSE);
        }
    }

    // Make sure a collection-session is not already running...
    if( OPTO_StartFlag )
    {
        OPTO_errorf("OPTO_Start(...) Already started.\n");
        return(FALSE);
    }

    // Loop through various steps in starting an OptoTrak session...
    for( step=OPTO_START_CAMERA,ok=TRUE; ((step <= OPTO_START_DELAY) && ok); step++ )
    {
        switch( step )
        {
           case OPTO_START_CAMERA :
              if( (rc=OptotrakLoadCameraParameters(NULL)) == OPTO_RC_OK )
              {
                  OPTO_debugf("OptotrakLoadCameraParameters(...) OK.\n");
              }
              else
              {
                  OPTO_Fail("OptotrakLoadCameraParameters",rc);
                  ok = FALSE;
              }
              break;

           case OPTO_START_RTMX :
              ok = OPTO_SetRTMX(rtmx);
              break;

           case OPTO_START_STROBER :
              if( (ok=OPTO_Strober(m1,m2,m3,m4)) )
              {
                  markers = OPTO_Markers();
                  OPTO_debugf("OPTO_Start(...) %d marker(s) specified.\n",markers);
              }
              break;

           case OPTO_START_COLLECT :
              OPTO_CT = (ct == OPTO_DEFAULT_CT) ? OPTO_MAX_CT : ct;
              OPTO_FF = freq;               // Frame frequency.
              OPTO_MF = OPTO_MARKER_FREQ;   // Marker frequency.

              if( (OPTO_FF*(float)markers) >= OPTO_MF )
              {
                  OPTO_errorf("OPTO_Start(...) Markers (%d) & Frequency (%.0f Hz) exceed maximum (%.0f).\n",
                              markers,OPTO_FF,OPTO_MF);
                  ok = FALSE;
                  break;
              }

              if( (rc=OptotrakSetupCollection(markers,          // Total number of markers.
                                              OPTO_FF,          // Frame frequency.
                                              OPTO_MF,          // Marker frequency.
                                              30,               // Noise threshold.
                                              160,              // Minimum gain.
                                              TRUE,             // Stream data (TRUE/FALSE).
                                              0.35,             // Duty cycle.
                                              7.0,              // Strobe voltage.
                                              OPTO_CT,          // Collection time.
                                              0.0,              // Pre-trigger time.
                                              flag              // Flags.
                                              )) == OPTO_RC_OK )
              {
                  OPTO_debugf("OptotrakSetupCollection(markers=%d,freq=%.1f,time=%.1f) OK.\n",markers,OPTO_FF,OPTO_CT);
              }
              else
              {
                  OPTO_Fail("OptotrakSetupCollection",rc);
                  ok = FALSE;
              }
              break;

           case OPTO_START_MARKERS :
              ok = OPTO_Markers(OPTO_MARKERS_ON);
              break;

           case OPTO_START_DELAY :
//              TIMER_delay(OPTO_START_WAIT);
              break;
        }

        STR_printf(ok,OPTO_debugf,OPTO_errorf,"OPTO_Start(...) %s %s.\n",OPTO_Step[step],STR_OkFailed(ok));
    }

    if( ok )
    {
        OPTO_debugf("OPTO_Start(...) OK.\n");
        OPTO_StartFlag = TRUE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Start( float freq, int m[], float ct, char *rtmx, int flag )
{
BOOL    ok;

    ok = OPTO_Start(freq,m[0],m[1],m[2],m[3],ct,rtmx,flag);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Start( float freq, int m[], char *rtmx )
{
BOOL    ok;

    ok = OPTO_Start(freq,m,OPTO_CollectionTime,rtmx,OPTO_Flags);

    return(ok);
}


/******************************************************************************/

BOOL    OPTO_Start( int m[], char *rtmx )
{
BOOL    ok;

    ok = OPTO_Start(OPTO_Frequency,m,OPTO_CollectionTime,rtmx,OPTO_Flags);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Start( int m[] )
{
BOOL    ok;

    ok = OPTO_Start(OPTO_Frequency,m,OPTO_CollectionTime,OPTO_RTFN,OPTO_Flags);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Start( char *cnfg )
{
BOOL    ok=FALSE;

    if( OPTO_Cnfg(cnfg) )
    {
        ok = OPTO_Start();
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Start( void )
{
BOOL    ok;

    ok = OPTO_Start(OPTO_Frequency, 
                    OPTO_PortMarker[0],
                    OPTO_PortMarker[1],
                    OPTO_PortMarker[2],
                    OPTO_PortMarker[3],
                    OPTO_CollectionTime,
                    OPTO_RTFN,
                    OPTO_Flags);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Started( void )
{
BOOL    flag=FALSE;

    if( OPTO_API_check() )
    {
        flag = OPTO_StartFlag;
    }

    return(flag);
}

/******************************************************************************/

void    OPTO_Stop( void )
{
//  Mare sure API is running...
    if( !OPTO_API_check() )
    {
        return;
    }

//  Turn markers off whatever else...
    OPTO_Markers(OPTO_MARKERS_OFF);

//  Stop session if one is current...
    if( OPTO_StartFlag )
    {
        OPTO_debugf("OPTO_Stop() OK.\n");
        OPTO_StartFlag = FALSE;
    }
}

/******************************************************************************/

int OPTO_Dim( matrix *posn, matrix *seen )
{
int markers;

    // Mare sure API is running...
    if( !OPTO_API_check() )
    {
        return(0);
    }

    markers = OPTO_Markers();

    if( posn != NULL )
    {
        posn->dim(SPMX_PTMX_ROW,markers);
    }

    if( seen != NULL )
    {
        seen->dim(OPTO_SEEN_ROW,markers);
    }

    return(markers);
}

/******************************************************************************/

void    OPTO_ND3d2Posn( ND3D *nd3d, int col, matrix &posn, matrix &seen )
{
    if( nd3d->x <= (MAX_NEGATIVE/2.0) )     // Cannot see this marker...
    {
        posn(SPMX_POMX_X,col) = 999.0;
        posn(SPMX_POMX_Y,col) = 999.0;
        posn(SPMX_POMX_Z,col) = 999.0;

        seen(OPTO_SEEN_ROW,col) = (double)FALSE;
    }
    else                                   // Marker is visible...
    {
        posn(SPMX_POMX_X,col) = (double)(nd3d->x / OPTO_POSN_SCALE);
        posn(SPMX_POMX_Y,col) = (double)(nd3d->y / OPTO_POSN_SCALE);
        posn(SPMX_POMX_Z,col) = (double)(nd3d->z / OPTO_POSN_SCALE);

        seen(OPTO_SEEN_ROW,col) = (double)TRUE;
    }

    // Set the "T" row to 1.0 if it exists...
    if( posn.rows() == SPMX_PTMX_ROW )  
    {
        posn(SPMX_PTMX_T,col) = 1.0;
    }
}

/******************************************************************************/

void    OPTO_ND3d2Posn( ND3D nd3d[], matrix &posn, matrix &seen, int markers, BOOL rtmx )
{
static  matrix  p,s;
int     m;

    if( posn.isempty() )
    {
        posn.dim(SPMX_PTMX_ROW,markers);
    }

    if( seen.isempty() )
    {
        seen.dim(OPTO_SEEN_ROW,markers);
    }

    p.dim(posn.rows(),1);
    s.dim(OPTO_SEEN_MTX);

    // Loop for each marker and put ND 3D positions into our matrix...
    for( m=0; (m < markers); m++ )
    {
        OPTO_ND3d2Posn(&nd3d[m],1,p,s);

        // Do Rotation / Translation?
        if( rtmx && s.B(OPTO_SEEN_MTX) )
        {
            OPTO_RT(p);
        }

        matrix_array_put(posn,p,m+1);
        matrix_array_put(seen,s,m+1);
    }
}

/******************************************************************************/

void    OPTO_ND3d2Posn( ND3D nd3d[], matrix posn[], BOOL seen[], int markers, BOOL rtmx )
{
int     m;
matrix  s(OPTO_SEEN_MTX);

    // Loop for each marker and put ND 3D positions into our matrix...
    for( m=0; (m < markers); m++ )
    {
        if( posn[m].isempty() )
        {
            posn[m].dim(SPMX_PTMX_MTX);
        }

        OPTO_ND3d2Posn(&nd3d[m],1,posn[m],s);

        seen[m] = s.B(OPTO_SEEN_MTX);

        // Do Rotation / Translation?
        if( rtmx && seen[m] )
        {
            OPTO_RT(posn[m]);
        }
    }
}

/******************************************************************************/

UINT OPTO_ReadBlocking( ND3D nd3d[], int markers )
{
UINT    frame,elements,flags;
int     rc;

    // Make sure the API has been started...
    if( !OPTO_API_check() )
    {
        return(OPTO_FRAME_ERROR);
    }

    OPTO_ThreadLock();                     // Multi-thread safe. (V2.6).

    // Get an OptoTrak frame...
    if( (rc=DataGetLatest3D(&frame,&elements,&flags,nd3d)) != OPTO_RC_OK )
    {
        OPTO_Fail("DataGetLatest3D",rc);
        frame = OPTO_FRAME_ERROR;
    }

    OPTO_ThreadUnlock();                   // Multi-thread safe. (V2.6).

    // Return the frame number.
    return(frame);                     
}

/******************************************************************************/

BOOL OPTO_RequestLatest( void )
{
int rc;

    // Make sure the API has been started...
    if( !OPTO_API_check() )
    {
        return(FALSE);
    }

    if( OPTO_ReadType == OPTO_READ_BLOCKING )
    {
        return(TRUE);
    }

    OPTO_ThreadLock();                     // Multi-thread safe. (V2.6).

    if( OPTO_ReadWaiting )
    {
        OPTO_ThreadUnlock();               // Multi-thread safe. (V2.6).
        return(TRUE);
    }

    if( (rc=RequestLatest3D()) != OPTO_RC_OK )
    {
        OPTO_Fail("RequestLatest3D",rc);
        OPTO_ThreadUnlock();               // Multi-thread safe. (V2.6).
        return(FALSE);
    }

    OPTO_ReadWaiting = TRUE;

    OPTO_ThreadUnlock();                   // Multi-thread safe. (V2.6).

    return(TRUE);
}

/******************************************************************************/

UINT OPTO_ReadNonBlocking( ND3D nd3d[], int markers, BOOL automatic )
{
UINT frame,elements,flags;
BOOL ok;
int rc;

    // Make sure the API has been started...
    if( !OPTO_API_check() )
    {
        return(OPTO_FRAME_ERROR);
    }

    OPTO_ThreadLock();                     // Multi-thread safe. (V2.6).

    // Are we currently waiting on the latest OptoTrak frame?
    if( OPTO_ReadWaiting )
    {
        // Is OptoTrak frame ready?
        if( !DataIsReady() )
        {
            OPTO_ThreadUnlock();           // Multi-thread safe. (V2.6).
            return(0);
        }

        // Get an OptoTrak frame...
        if( (rc=DataReceiveLatest3D(&frame,&elements,&flags,nd3d)) != OPTO_RC_OK )
        {
            OPTO_Fail("ReceiveLatestData3D",rc);
            OPTO_ThreadUnlock();           // Multi-thread safe. (V2.6).
            return(0);
        }

        OPTO_ReadWaiting = FALSE;

        OPTO_ThreadUnlock();               // Multi-thread safe. (V2.6).

        // Return the retrieved frame number.
        return(frame);
    }

    OPTO_ThreadUnlock();                   // Multi-thread safe. (V2.6).

    if( automatic )
    {
        ok = OPTO_RequestLatest();
    }

    return(0);                     
}

/******************************************************************************/

UINT OPTO_Posn( ND3D nd3d[], int markers )
{
UINT frame=0;

    switch( OPTO_ReadType )
    {
        case OPTO_READ_BLOCKING : // Use blocking data read function
           frame = OPTO_ReadBlocking(nd3d,markers);
           break;

        case OPTO_READ_AUTO :     // Automatically request latest data (non-blocking)
           frame = OPTO_ReadNonBlocking(nd3d,markers,TRUE);
           break;

        case OPTO_READ_MANUAL :   // Manually request latest data (non-blocking)
           frame = OPTO_ReadNonBlocking(nd3d,markers,FALSE);
           break;
    }

    return(frame);
}

/******************************************************************************/

UINT OPTO_Posn( matrix &posn, matrix &seen, int markers, BOOL rtmx )
{
UINT frame;

    // Get an OptoTrak frame...
    frame = OPTO_Posn(OPTO_nd3d,markers);

    // Non-zero frame number indicates a frame was retrieved.
    if( frame != 0 )
    {
        // Convert Northern Digital's 3D positions to our position matrix...
        OPTO_ND3d2Posn(OPTO_nd3d,posn,seen,markers,rtmx);
    }

    // Return the frame number.
    return(frame);
}

/******************************************************************************/

UINT OPTO_Posn( matrix posn[], BOOL seen[], int markers, BOOL rtmx )
{
UINT frame;

    // Get an OptoTrak frame...
    frame = OPTO_Posn(OPTO_nd3d,markers);

    // Non-zero frame number indicates a frame was retrieved.
    if( frame != 0 )
    {
        // Convert Northern Digital's 3D positions to our position matrix...
        OPTO_ND3d2Posn(OPTO_nd3d,posn,seen,markers,rtmx);
    }

    // Return the frame number.
    return(frame);
}

/******************************************************************************/

UINT    OPTO_Posn( matrix &posn, matrix &seen, int m1, int m2, BOOL rtmx )
{
UINT    frame;
int     p_row,s_row;
matrix  p_mtx,s_mtx;

    if( posn.isempty() )
    {
        p_row = SPMX_PTMX_ROW;
        posn.dim(p_row,m2);
    }
    else
    {
        p_row = posn.rows();
    }

    s_row = OPTO_SEEN_ROW;

    if( seen.isempty() )
    {
        seen.dim(s_row,m2);
    }

    // Make arrays big enough for all markers...
    p_mtx.dim(p_row,m2);
    s_mtx.dim(s_row,m2);

    // Get position of all markers...
    frame = OPTO_Posn(p_mtx,s_mtx,m2,rtmx);

    // Pull out columns for specified marker range (m1..m2)...
    matrix_extract_sub(posn,p_mtx,1,m1,p_row,m2);
    matrix_extract_sub(seen,s_mtx,1,m1,s_row,m2);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_Posn( matrix posn[], BOOL seen[], int m1, int m2, BOOL rtmx )
{
static  matrix  p(SPMX_PTMX_ROW,OPTO_MAX_MARKER);
static  matrix  s(OPTO_SEEN_ROW,OPTO_MAX_MARKER);
UINT    frame;
int     m;

    // Get position of all markers...
    frame = OPTO_Posn(p,s,m1,m2,rtmx);

    for( m=0; (m <= (m2-m1)); m++ )
    {
        posn[m] = p[m+1];
        seen[m] = s.B(OPTO_SEEN_ROW,m+1);
    }

    return(frame);
}

/******************************************************************************/

UINT    OPTO_Posn( matrix &posn, matrix &seen, BOOL rtmx )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,OPTO_Markers(),rtmx);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_Posn( matrix posn[], BOOL seen[], BOOL rtmx )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,OPTO_Markers(),rtmx);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix &posn, matrix &seen )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,OPTO_RTMX.isnotempty());

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix posn[], BOOL seen[] )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,OPTO_RTMX.isnotempty());

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix &posn, matrix &seen, int markers )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,markers,OPTO_RTMX.isnotempty());

    return(frame);
}


/******************************************************************************/

UINT    OPTO_GetPosn( matrix posn[], BOOL seen[], int markers )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,markers,OPTO_RTMX.isnotempty());

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix &posn, matrix &seen, int m1, int m2 )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,m1,m2,OPTO_RTMX.isnotempty());

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix posn[], BOOL seen[], int m1, int m2 )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,m1,m2,OPTO_RTMX.isnotempty());

    return(frame);
}

/******************************************************************************/

UINT    OPTO_UsrPosn( matrix &posn, matrix &seen )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,TRUE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_UsrPosn( matrix posn[], BOOL seen[] )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,TRUE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_UsrPosn( matrix &posn, matrix &seen, int markers )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,markers,TRUE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_UsrPosn( matrix posn[], BOOL seen[], int markers )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,markers,TRUE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_UsrPosn( matrix &posn, matrix &seen, int m1, int m2 )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,m1,m2,TRUE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_UsrPosn( matrix posn[], BOOL seen[], int m1, int m2 )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,m1,m2,TRUE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_RawPosn( matrix &posn, matrix &seen )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,FALSE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_RawPosn( matrix posn[], BOOL seen[] )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,FALSE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_RawPosn( matrix &posn, matrix &seen, int markers )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,markers,FALSE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_RawPosn( matrix posn[], BOOL seen[], int markers )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,markers,FALSE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_RawPosn( matrix &posn, matrix &seen, int m1, int m2 )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,m1,m2,FALSE);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_RawPosn( matrix posn[], BOOL seen[], int m1, int m2 )
{
UINT    frame;

    frame = OPTO_Posn(posn,seen,m1,m2,FALSE);

    return(frame);
}

/******************************************************************************/

BOOL    OPTO_Seen( matrix &seen, int m1, int m2 )
{
int     m;
BOOL    ok;

    for( ok=TRUE,m=m1; ((m <= m2) && ok); m++ )
    {                                  // Can we see all the markers?
        ok = seen.B(1,m);
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Seen( matrix &seen, int markers )
{
BOOL    ok;

    ok = OPTO_Seen(seen,1,markers);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Seen( matrix &seen )
{
BOOL    ok;

    ok = OPTO_Seen(seen,1,OPTO_Markers());

    return(ok);
}

/******************************************************************************/

float   OPTO_Freq( void )
{
    return(OPTO_FF);              // Frame Rate Frequency.
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix &posn )
{
UINT    frame;
matrix  seen;

    frame = OPTO_GetPosn(posn,seen);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix &posn, int markers )
{
UINT    frame;
matrix  seen;

    frame = OPTO_GetPosn(posn,seen,markers);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix &posn, int m1, int m2 )
{
UINT    frame;
matrix  seen;

    frame = OPTO_GetPosn(posn,seen,m1,m2);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix posn[] )
{
UINT    frame;
BOOL	seen[OPTO_MAX_MARKER];

    frame = OPTO_GetPosn(posn,seen);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix posn[], int markers )
{
UINT    frame;
BOOL	seen[OPTO_MAX_MARKER];

    frame = OPTO_GetPosn(posn,seen,markers);

    return(frame);
}

/******************************************************************************/

UINT    OPTO_GetPosn( matrix posn[], int m1, int m2 )
{
UINT    frame;
BOOL	seen[OPTO_MAX_MARKER];

    frame = OPTO_GetPosn(posn,seen,m1,m2);

    return(frame);
}

/******************************************************************************/


