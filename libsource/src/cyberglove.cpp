/******************************************************************************/
/*                                                                            */
/* MODULE  : CyberGlove.cpp                                                   */
/*                                                                            */
/* PURPOSE : CyberGlove API.                                                  */
/*                                                                            */
/* DATE    : 03/Apr/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 04/Nov/2000 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "GLOVE"
#define MODULE_TEXT     "CyberGlove API"
#define MODULE_DATE     "03/04/2002"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

BOOL    GLOVE_API_started=FALSE;

/******************************************************************************/

TIMER  *GLOVE_Timer=NULL;
double  GLOVE_Timeout=1000.0;

struct  GLOVE_Handle    GLOVE_Port[GLOVE_PORT_MAX];
int     GLOVE_ID=GLOVE_PORT_INVALID;

BYTE    GLOVE_Recv[GLOVE_BUFF];

/******************************************************************************/

struct  STR_TextItem  GLOVE_SensorText[] =
{
    { GLOVE_THUMB_TMJ       ,"ThumbTMJ" },
    { GLOVE_THUMB_MPJ       ,"ThumbMPJ" },
    { GLOVE_THUMB_IJ        ,"ThumbIJ" },
    { GLOVE_THUMB_ABD       ,"ThumbAbduct" },
    { GLOVE_INDEX_MPJ       ,"IndexMPJ" },
    { GLOVE_INDEX_PIJ       ,"IndexPIJ" },
    { GLOVE_INDEX_DIJ       ,"IndexDIJ" },
    { GLOVE_INDEX_ABD       ,"IndexAbduct" },
    { GLOVE_MIDDLE_MPJ      ,"MiddleMPJ" },
    { GLOVE_MIDDLE_PIJ      ,"MiddlePIJ" },
    { GLOVE_MIDDLE_DIJ      ,"MiddleDIJ" },
    { GLOVE_MIDDLE_INDEX_ABD,"MiddleIndexAbduct" },
    { GLOVE_RING_MPJ        ,"RingMPJ" },
    { GLOVE_RING_PIJ        ,"RingPIJ" },
    { GLOVE_RING_DIJ        ,"RingDIJ" },
    { GLOVE_RING_MIDDLE_ABD ,"RingMiddleAbduct" },
    { GLOVE_PINKIE_MPJ      ,"PinkieMPJ" },
    { GLOVE_PINKIE_PIJ      ,"PinkiePIJ" },
    { GLOVE_PINKIE_DIJ      ,"PinkieDIJ" },
    { GLOVE_PINKIE_RING_ABD ,"PinkieRingAbduct" },
    { GLOVE_PALM_ARCH       ,"PalmArch" },
    { GLOVE_WRIST_PITCH     ,"WristPitch" },
    { GLOVE_WRIST_YAW       ,"WristYaw" },
    { STR_TEXT_EOT },
};

/******************************************************************************/

char *GLOVE_SensorName( int sensor )
{
char *name;

    name = STR_TextCode(GLOVE_SensorText,sensor);

    return(name);
}

/******************************************************************************/

int GLOVE_SensorCode( char *name )
{
int sensor;

    sensor = STR_TextCode(GLOVE_SensorText,name);

    return(sensor);
}

/******************************************************************************/

PRINTF  GLOVE_PRN_messgf=NULL;              // General messages printf function.
PRINTF  GLOVE_PRN_errorf=NULL;              // Error messages printf function.
PRINTF  GLOVE_PRN_debugf=NULL;              // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     GLOVE_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(GLOVE_PRN_messgf,buff));
}

/******************************************************************************/

int     GLOVE_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(GLOVE_PRN_errorf,buff));
}

/******************************************************************************/

int     GLOVE_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(GLOVE_PRN_debugf,buff));
}

/******************************************************************************/

struct  STR_TextItem  GLOVE_CommandText[] =
{   
    { GLOVE_BAUDRATE     ,"BaudRate"     },
    { GLOVE_CALIBRATE    ,"Calibrate"    },
    { GLOVE_SENSORMASK   ,"SensorMask"   },
    { GLOVE_SENSORSAMPLE ,"SensorSample" },
    { GLOVE_FLAGS        ,"Flags"        },
    { GLOVE_SAMPLEPERIOD ,"SamplePeriod" },
    { GLOVE_REINITIALIZE ,"Re-Initialize"},
    { GLOVE_RESTART      ,"Re-Start"     },
    { GLOVE_BREAK        ,"Break"        },
    { GLOVE_CONNECTED    ,"Connected"    },
    { GLOVE_INFORMATION  ,"Information"  },
    { GLOVE_SENSORMASK   ,"SensorMask"   },
    { GLOVE_HAND         ,"Hand"         },
    { GLOVE_SENSORS      ,"Sensors"      },
    { GLOVE_VERSION      ,"Version"      },
    { GLOVE_DISPLAYCODES ,"DisplayCodes" },
    { STR_TEXT_ENDOFTABLE },
};

struct  STR_TextItem  GLOVE_HandText[] =
{   
    { GLOVE_HAND_INVALID,"INVALID" },
    { GLOVE_HAND_RIGHT  ,"RIGHT"   },
    { GLOVE_HAND_LEFT   ,"LEFT"    },
    { STR_TEXT_ENDOFTABLE },
};

struct  STR_TextItem  GLOVE_ActionText[] =
{   
    { GLOVE_SET,"Set" },
    { GLOVE_GET,"Get" },
    { STR_TEXT_ENDOFTABLE },
};

struct  STR_TextItem  GLOVE_FlagText[] =
{   
    { GLOVE_FLAG_TIMESTAMP   ,"TimeStamp"    },
    { GLOVE_FLAG_FILTER      ,"Filter"       },
    { GLOVE_FLAG_SWITCHLIGHT ,"SwitchLight"  },
    { GLOVE_FLAG_LIGHT       ,"Light"        },
    { GLOVE_FLAG_QUANTITIZED ,"Quantitized"  },
    { GLOVE_FLAG_GLOVESTATUS ,"GloveStatus"  },
    { GLOVE_FLAG_SWITCHSTATUS,"SwitchStatus" },
    { GLOVE_FLAG_EXTERNALSYNC,"ExternalSync" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

BOOL    GLOVE_CnfgFlag( int ID, STRING text[], BOOL flag )
{
BOOL    ok;
int     i,item;
BYTE    code;

    for( ok=TRUE,i=0; ((i < GLOVE_FLAG_MAX) && ok); i++ )
    {
        if( STR_null(text[i]) )
        {
            break;
        }

        if( (item=STR_TextCode(GLOVE_FlagText,text[i])) == STR_NOTFOUND )
        {
            GLOVE_errorf("GLOVE_CnfgFlag(ID=%d,...) %s Flag not found.\n",ID,text[i]);
            ok = FALSE;
            break;
        }

        code = (BYTE)item;

        ok = GLOVE_Flag(ID,GLOVE_SET,code,flag);
    }

    return(ok);
}

/******************************************************************************/

void    GLOVE_DCB( DCB *dcb )
{
    dcb->ByteSize = 8;
    dcb->Parity = NOPARITY;
    dcb->StopBits = ONESTOPBIT;
    dcb->fAbortOnError = TRUE;
    dcb->fDtrControl = DTR_CONTROL_DISABLE;
    dcb->fDsrSensitivity = FALSE;
    dcb->fOutxDsrFlow = FALSE;
    dcb->fOutxCtsFlow = FALSE;
    dcb->fRtsControl = RTS_CONTROL_DISABLE;
}

/******************************************************************************/

BOOL   &GLOVE_Flag( int ID, int code )
{
static  BOOL  flag;
int     index;

    flag = FALSE;

    if( (index=STR_TextIndex(GLOVE_FlagText,code)) == STR_NOTFOUND )
    {
        return(flag);
    }

    return(GLOVE_Port[ID].flag[index]);
}

/******************************************************************************/

void    GLOVE_Init( int ID )
{
int     flag;

    GLOVE_Port[ID].used = FALSE;
    GLOVE_Port[ID].started = FALSE;
    GLOVE_Port[ID].comH = COM_INVALID;

    GLOVE_Port[ID].session = NULL;
    GLOVE_Port[ID].framerate = NULL;
    GLOVE_Port[ID].frametime = NULL;
    GLOVE_Port[ID].framelast = NULL;
    GLOVE_Port[ID].framewait = NULL;

    memset(GLOVE_Port[ID].calibfile,0,STRLEN);
    matrix_empty(GLOVE_Port[ID].calibdata);

    for( flag=0; (flag < GLOVE_FLAG_MAX); flag++ )
    {
        GLOVE_Port[ID].flag[flag] = FALSE;
    }
}

/******************************************************************************/

int     GLOVE_Free( void )
{
int     ID,find;

    // Check if API started okay...
    if( !GLOVE_API_check() )                  
    {
        return(GLOVE_PORT_INVALID);
    }

    // Find free GLOVE port handle...
    for( find=GLOVE_PORT_INVALID,ID=0; (ID < GLOVE_PORT_MAX); ID++ )
    {
        if( !GLOVE_Port[ID].used )
        {
            find = ID;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

void    GLOVE_Use( int ID )
{
    GLOVE_ID = ID;
}

/******************************************************************************/

BOOL    GLOVE_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok;
int     ID;

    GLOVE_PRN_messgf = messgf;              // General API message print function.
    GLOVE_PRN_errorf = errorf;              // API error message print function.
    GLOVE_PRN_debugf = debugf;              // Debug information print function.

    if( GLOVE_API_started )                 // Start the API once...
    {
        return(TRUE);
    }

    // Initialize GLOVE port handles...
    for( ID=0; (ID < GLOVE_PORT_MAX); ID++ )
    {
        GLOVE_Init(ID);
    }

    // GLOVE Timer...
    GLOVE_Timer = new TIMER("GLOVE",TIMER_MODE_RESET);

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(GLOVE_API_stop);           // Install stop function.
        GLOVE_API_started = TRUE;             // Set started flag.

        MODULE_start(GLOVE_PRN_messgf);       // Register module.
    }
    else
    {
        GLOVE_errorf("GLOVE_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    GLOVE_API_stop( void )
{
int     ID;

    if( !GLOVE_API_started )           // API not started in the first place...
    {
         return;
    }

    GLOVE_CloseAll();
    delete GLOVE_Timer;

    GLOVE_API_started = FALSE;         // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    GLOVE_API_check( void )
{
BOOL    ok=TRUE;

    if( !GLOVE_API_started )             // API not started...
    {                                  // Start module automatically...
        ok = GLOVE_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_Check( int ID )
{
BOOL    ok=TRUE;

    if( !GLOVE_API_check() )
    {
        ok = FALSE;
    }
    else
    if( ID == GLOVE_PORT_INVALID )
    {
        ok = FALSE;
    }
    else
    if( !GLOVE_Port[ID].used )
    {
        ok = FALSE;
    }
    else
    if( GLOVE_Port[ID].comH == COM_INVALID )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

int    GLOVE_comX=1;
long   GLOVE_baud=115200;
BOOL   GLOVE_streaming=TRUE;
STRING GLOVE_CalibFile="";
double GLOVE_FrameHz=100.0;
STRING GLOVE_flagon[GLOVE_FLAG_MAX]={ "TimeStamp","GloveStatus" }; 
STRING GLOVE_flagoff[GLOVE_FLAG_MAX];

/******************************************************************************/

BOOL GLOVE_CnfgLoad( char *file )
{
char *path;

    if( (path=FILE_Calibration(file)) == NULL )
    {
        GLOVE_errorf("GLOVE_CnfgLoad(%s) Cannot find file.\n",file);
        return(FALSE);
    }

    // Reset configuration variable table..
    CONFIG_reset();

    // List of configuration variables...
    CONFIG_set("comX",GLOVE_comX);
    CONFIG_set("BAUD",GLOVE_baud);
    CONFIG_setBOOL("Stream",GLOVE_streaming);
    CONFIG_set("Calibration",GLOVE_CalibFile);
    CONFIG_set("FrameHz",GLOVE_FrameHz);
    CONFIG_set("FlagOn",GLOVE_flagon,GLOVE_FLAG_MAX);
    CONFIG_set("FlagOff",GLOVE_flagoff,GLOVE_FLAG_MAX);

    // Load configuration file...
    if( !CONFIG_read(path) )
    {
        return(FALSE);
    }

    GLOVE_debugf("GLOVE_CnfgLoad(%s) %s.\n",file,path);
    CONFIG_list(GLOVE_debugf);

    return(TRUE);
}

/******************************************************************************/

BOOL GLOVE_CalibLoad( char *file, matrix &data )
{
BOOL ok;
char *path;

    if( STR_null(file) )
    {
        return(TRUE);
    }

    if( (path=FILE_Calibration(file)) == NULL )
    {
        GLOVE_errorf("GLOVE_CalibLoad(%s) Cannot find file.\n",file);
        return(FALSE);
    }

    ok = matrix_read(path,data);

    STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_CalibLoad(%s) %s %s.\n",file,path,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_CalibLoad( int ID )
{
BOOL ok;

    // Check if API started and handle ID is okay...
    if( !GLOVE_Check(ID) )
    {
        return(FALSE);
    }

    ok = GLOVE_CalibLoad(GLOVE_Port[ID].calibfile,GLOVE_Port[ID].calibdata);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_CalibLoad( void )
{
BOOL ok;

    ok = GLOVE_CalibLoad(GLOVE_ID);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_OpenParameters( void )
{
BOOL    ok=TRUE;

    return(ok);
}

/******************************************************************************/

#define GLOVE_STATUS_CONNECTED    0
#define GLOVE_STATUS_SENSORS      1
#define GLOVE_STATUS_HAND         2

char   *GLOVE_StatusStep[] = { "Connected","Sensors","Hand",NULL };

BOOL    GLOVE_Status( int ID )
{
BOOL    ok;
int     step;
BOOL    initialized,connected;

    for( ok=TRUE,step=GLOVE_STATUS_CONNECTED; ((step <= GLOVE_STATUS_HAND) && ok); step++ )
    {
        switch( step )
        {
            case GLOVE_STATUS_CONNECTED :
               if( !GLOVE_QueryConnected(ID,initialized,connected) )
               {
                   GLOVE_errorf("GLOVE_Status(ID=%d) Not responding.\n",ID);
                   ok = FALSE;
                   break;
               }

               if( !initialized )
               {
                   GLOVE_errorf("GLOVE_Status(ID=%d) Not initialized.\n",ID);
                   ok = FALSE;
                   break;
               }

               if( !connected )
               {
                   GLOVE_errorf("GLOVE_Status(ID=%d) Glove not connected.\n",ID);
                   ok = FALSE;
                   break;
               }
               break;

            case GLOVE_STATUS_SENSORS :
               if( (ok=GLOVE_QuerySensors(ID,GLOVE_Port[ID].sensors)) )
               {
                   GLOVE_Port[ID].selected = GLOVE_Port[ID].sensors;
               }
               break;

            case GLOVE_STATUS_HAND :
               ok = GLOVE_QueryHand(ID,GLOVE_Port[ID].hand);
               break;
        }

        STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_Status(ID=%d) %s %s.\n",ID,GLOVE_StatusStep[step],STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

#define GLOVE_CONFIG_FLAGON       0
#define GLOVE_CONFIG_FLAGOFF      1

char   *GLOVE_ConfigStep[] = { "FlagON","FlagOFF",NULL };

BOOL    GLOVE_Config( int ID )
{
BOOL    ok;
int     step;

    for( ok=TRUE,step=GLOVE_CONFIG_FLAGON; ((step <= GLOVE_CONFIG_FLAGOFF) && ok); step++ )
    {
        switch( step )
        {
            case GLOVE_CONFIG_FLAGON :
               ok = GLOVE_CnfgFlag(ID,GLOVE_flagon,TRUE);
               break;

            case GLOVE_CONFIG_FLAGOFF :
               ok = GLOVE_CnfgFlag(ID,GLOVE_flagoff,TRUE);
               break;
        }

        STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_Config(ID=%d) %s %s.\n",ID,GLOVE_ConfigStep[step],STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

#define GLOVE_OPEN_LOADCONFIG     0
#define GLOVE_OPEN_PARAMETERS     1
#define GLOVE_OPEN_COMOPEN        2
#define GLOVE_OPEN_COMSETUP       3
#define GLOVE_OPEN_HANDLE         4
#define GLOVE_OPEN_COMRESET       5
#define GLOVE_OPEN_STATUS         6
#define GLOVE_OPEN_FLAGS          7
#define GLOVE_OPEN_LOADCALIB      8
#define GLOVE_OPEN_OBJECTS        9
#define GLOVE_OPEN_DONE          10

char   *GLOVE_OpenStep[] = { "LoadConfig","Parameters","COM-Open","COM-Setup","Handle","COM-Reset","Status","Flags","LoadCalib","Objects","Done",NULL };

/******************************************************************************/

int     GLOVE_Open( char *cnfg )
{
int     ID=GLOVE_PORT_INVALID;
int     step;
BOOL    ok;
int     comH=COM_INVALID;

//  Check that GLOVE API is running...
    if( !GLOVE_API_check() )
    {
        return(GLOVE_PORT_INVALID);
    }

    GLOVE_debugf("GLOVE_Open(%s)\n",cnfg);

//  Go through each step of GLOVE_Open() and continue only if successful...
    for( ok=TRUE,step=GLOVE_OPEN_LOADCONFIG; ((step <= GLOVE_OPEN_DONE) && ok); step++ )
    {
        switch( step )
        {
            case GLOVE_OPEN_LOADCONFIG :
               ok = GLOVE_CnfgLoad(cnfg);
               break;

            case GLOVE_OPEN_PARAMETERS :         // Set defaults and then check parameters...
               ok = GLOVE_OpenParameters();
               break;

            case GLOVE_OPEN_COMOPEN :            // Open RS232 communications port... 
               ok = ((comH=COM_Open(GLOVE_comX)) != COM_INVALID);
               STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_Open(...) Open COM%d %s.\n",GLOVE_comX,STR_OkFailed(ok));
               break;

            case GLOVE_OPEN_COMSETUP :           // Set GLOVE RS232 characteristics...
               ok = COM_Setup(comH,GLOVE_baud,GLOVE_DCB);
               STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_Open(...) Setup COM%d %s.\n",GLOVE_comX,STR_OkFailed(ok));

               COM_Reset(comH);
               break;

            case GLOVE_OPEN_HANDLE :             // Find a free handle ID...
               if( (ID=GLOVE_Free()) == GLOVE_PORT_INVALID )
               {
                   GLOVE_errorf("GLOVE_Open(...) No free handles.\n");
                   ok = FALSE;
                   break;
               }

               GLOVE_debugf("ID=%d\n",ID);
               GLOVE_Init(ID);                   // Clear the handle item...

               GLOVE_Port[ID].used = TRUE;
               GLOVE_Port[ID].started = FALSE;
               GLOVE_Port[ID].comH = comH;
               GLOVE_Port[ID].comX = GLOVE_comX;
               GLOVE_Port[ID].baudrate = GLOVE_baud;

               strncpy(GLOVE_Port[ID].calibfile,GLOVE_CalibFile,STRLEN);
               break;

            case GLOVE_OPEN_COMRESET :           // Reset COM buffer...
                ok = COM_Reset(ID);
                break;

            case GLOVE_OPEN_STATUS :
               ok = GLOVE_Status(ID);
               break;

            case GLOVE_OPEN_FLAGS :
               ok = GLOVE_Config(ID);
               break;

            case GLOVE_OPEN_LOADCALIB :
               ok = GLOVE_CalibLoad(ID);
               break;

            case GLOVE_OPEN_OBJECTS :
               GLOVE_Port[ID].session   = new TIMER(STR_stringf("GLOVE[%d] Session",ID));
               GLOVE_Port[ID].framerate = new TIMER_Frequency(STR_stringf("GLOVE[%d] FrameRate",ID));
               GLOVE_Port[ID].frametime = new TIMER_Interval(STR_stringf("GLOVE[%d] FrameTime",ID));
               GLOVE_Port[ID].framelast = new TIMER(STR_stringf("GLOVE[%d] FrameLast",ID));
               GLOVE_Port[ID].framewait = new DATAPROC(STR_stringf("GLOVE[%d] FrameWait",ID));
               break;

            case GLOVE_OPEN_DONE :
               GLOVE_Use(ID);                    // Set global variable for default ID...
               break;
        }

        STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_Open(...) %s %s.\n",GLOVE_OpenStep[step],STR_OkFailed(ok));
    }
    
    // Something went wrong, so clean up...
    if( !ok )
    {
        COM_Close(comH);
        GLOVE_Init(ID);
        ID = GLOVE_PORT_INVALID;
    }

    return(ID);
}

/******************************************************************************/

void    GLOVE_Close( int ID )
{
BOOL    ok;

    // Check if API started and handle ID is okay...
    if( !GLOVE_Check(ID) )
    {
        return;
    }

    // First, make sure GLOVE is stopped...
    GLOVE_Stop(ID);

    // Close GLOVE RS232 port...
    ok = COM_Close(GLOVE_Port[ID].comH);
    STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_Close(ID=%d) Close COM%d %s.\n",ID,GLOVE_Port[ID].comX,STR_OkFailed(ok));

    delete GLOVE_Port[ID].session;
    delete GLOVE_Port[ID].framerate;
    delete GLOVE_Port[ID].frametime;
    delete GLOVE_Port[ID].framelast;
    delete GLOVE_Port[ID].framewait;

    // Clear the handle...
    GLOVE_Init(ID);
}

/******************************************************************************/

void    GLOVE_CloseAll( void )
{
int     ID;

    for( ID=0; (ID < GLOVE_PORT_MAX); ID++ )
    {
        GLOVE_Close(ID);
    }
}

/******************************************************************************/

BOOL    GLOVE_Start( int ID, BOOL streaming )
{
BOOL    ok=TRUE;

    // Make sure GLOVE handle is valid...
    if( !GLOVE_Check(ID) )
    {
        return(FALSE);
    }

    // Make sure we're not already started...
    if( GLOVE_Port[ID].started )
    {
        return(TRUE);
    }

    // Set various values to begin data collection...
    GLOVE_Port[ID].size = GLOVE_RecordSize(ID);
    GLOVE_Port[ID].frame = 0;
    GLOVE_Port[ID].sync = 0;
    GLOVE_Port[ID].overwrite = 0;
    GLOVE_Port[ID].got = 0;
    GLOVE_Port[ID].error = FALSE;
    GLOVE_Port[ID].stage = GLOVE_STAGE_START;
    GLOVE_Port[ID].framedata = FALSE;

    // Extra processing for streaming data...
    if( (GLOVE_Port[ID].streaming=streaming) )
    {
        if( !LOOPTASK_start(GLOVE_LoopTask) )
        {
            GLOVE_errorf("GLOVE_Start(ID=%d) Cannot start LoopTask.\n",ID);
            ok = FALSE;
        }
        else
        if( !GLOVE_Stream(ID,ON) )
        {
            GLOVE_errorf("GLOVE_Start(ID=%d) Cannot start streaming.\n",ID);
            ok = FALSE;
        }
    }

    if( !ok )
    {
        LOOPTASK_stop(GLOVE_LoopTask);
        return(FALSE);
    }

    GLOVE_Port[ID].session->Reset();
    GLOVE_Port[ID].framerate->Reset();
    GLOVE_Port[ID].frametime->Reset();
    GLOVE_Port[ID].framewait->Reset();

    GLOVE_Port[ID].started = TRUE;

    GLOVE_debugf("GLOVE_Start(ID=%d,streaming=%s) record=%d.\n",ID,STR_YesNo(GLOVE_Port[ID].streaming),GLOVE_Port[ID].size);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_Start( int ID )
{
BOOL    ok;

    ok = GLOVE_Start(ID,GLOVE_streaming);

    return(ok);
}

/******************************************************************************/

void    GLOVE_Stop( int ID )
{
BOOL    ok;
double  session;

    if( !GLOVE_Started(ID) )
    {
        return;
    }

    session = GLOVE_Port[ID].session->Elapsed();

    GLOVE_Port[ID].framerate->Calculate();
    GLOVE_Port[ID].frametime->Calculate();

    if( GLOVE_Port[ID].streaming )
    {
        if( (ok=GLOVE_Stream(ID,OFF)) )
        {
            COM_Reset(GLOVE_Port[ID].comH);
        }

        LOOPTASK_stop(GLOVE_LoopTask);
    }

    GLOVE_Port[ID].started = FALSE;

    GLOVE_debugf("GLOVE_Stop(ID=%d) %.1lf msec, %ld frames, %ld sync, %ld overwrite (errors %s).\n",ID,session,GLOVE_Port[ID].frame,GLOVE_Port[ID].sync,GLOVE_Port[ID].overwrite,STR_YesNo(GLOVE_Port[ID].error));

    GLOVE_Port[ID].framerate->Results(GLOVE_debugf);
    GLOVE_Port[ID].frametime->Results(GLOVE_debugf);
    GLOVE_Port[ID].framewait->Results(GLOVE_debugf);
}

/******************************************************************************/

int GLOVE_Sensors( int ID )
{
int sensors=0;

    if( GLOVE_Check(ID) )
    {
        sensors = GLOVE_Port[ID].selected;
    }

    return(sensors);
}

/******************************************************************************/

double GLOVE_FrameRate( int ID )
{
double framerate=0;

    if( GLOVE_Check(ID) )
    {
        framerate = GLOVE_Port[ID].frameHz;
    }

    return(framerate);
}

/******************************************************************************/

BOOL    GLOVE_Started( int ID )
{
BOOL    ok=TRUE;

    if( !GLOVE_Check(ID) )
    {
        ok = FALSE;
    }
    else
    if( !GLOVE_Port[ID].started )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_Streaming( int ID )
{
BOOL    ok=TRUE;

    if( !GLOVE_Started(ID) )
    {
        ok = FALSE;
    }
    else
    if( !GLOVE_Port[ID].streaming )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_Response( int ID, BOOL framed, BYTE *recv, int &size )
{
BOOL    ok,done=FALSE,frame=FALSE;
int     b=0;
BYTE    buff;

    GLOVE_Timer->Reset();

    do
    {
        if( GLOVE_Timer->Expired(GLOVE_Timeout) )
        {
            GLOVE_errorf("GLOVE_Response(...) Timeout.\n");
            ok = FALSE;
            continue;
        }

        ok = COM_Read(GLOVE_Port[ID].comH,&buff,1,done,0L);

        if( ok && done )
        {
            if( (b == size) || framed )
            {
                if( buff == 0 )
                {
                    frame = TRUE;
                    continue;
                }
            }

            if( b >= size )
            {
                GLOVE_errorf("GLOVE_Response(...) Buffer over-run (size=%d).\n",size);
                ok = FALSE;
                continue;
            }

            recv[b++] = buff;
       }
    }
    while( ok && !frame );

    if( frame )
    {
        size = b;
    }

    return(ok && frame);
}

/******************************************************************************/

BOOL GLOVE_Response( int ID, BYTE *recv, int &size )
{
BOOL ok;

    ok = GLOVE_Response(ID,FALSE,recv,size);

    return(ok);
}

/******************************************************************************/

#define GLOVE_BUFF_COMMAND 0
#define GLOVE_BUFF_DATA    1

BYTE    GLOVE_Buff[2][GLOVE_BUFF];     // Buffer.
int     GLOVE_BuffIndx[2]={0,0};       // Current index into buffer.
int     GLOVE_BuffMark[2]={0,0};       // Current marker into buffer.

/******************************************************************************/

void    GLOVE_BuffInit( int item )
{
    memset(GLOVE_Buff[item],0,GLOVE_BUFF);  // Clear buffer.
    GLOVE_BuffIndx[item] = 0;               // Reset buffer index.
    GLOVE_BuffMark[item] = 0;               // Reset buffer marker.
}

/******************************************************************************/

void    GLOVE_BuffSet( int item )
{
    GLOVE_BuffMark[item] = GLOVE_BuffIndx[item];
}

/******************************************************************************/

int     GLOVE_BuffGet( int item )
{
    return(GLOVE_BuffMark[item]);
}

/******************************************************************************/

void GLOVE_BuffPutByte( int item, BYTE data )
{
    if( GLOVE_BuffIndx[item] >= GLOVE_BUFF )
    {
        GLOVE_errorf("GLOVE_Buff[%d] Buffer exceeded.\n",item);
    }
    else
    {
        GLOVE_Buff[item][GLOVE_BuffIndx[item]++] = data;
    }
}

/******************************************************************************/

BYTE GLOVE_BuffGetByte( int item )
{
BYTE data;

    if( GLOVE_BuffIndx[item] >= GLOVE_BUFF )
    {
        GLOVE_errorf("GLOVE_Buff[%d] Buffer exceeded.\n",item);
    }
    else
    {
        data = GLOVE_Buff[item][GLOVE_BuffIndx[item]++];
    }

    return(data);
}

/******************************************************************************/

void    GLOVE_BuffPutWord( int item, WORD data )
{
    GLOVE_BuffPutData(item,&data,sizeof(WORD));
}

/******************************************************************************/

WORD GLOVE_BuffGetWord( int item )
{
WORD data=0;

    GLOVE_BuffGetData(item,&data,sizeof(WORD));

    return(data);
}

/******************************************************************************/

void    GLOVE_BuffPutData( int item, void *data, int size )
{
BYTE   *bptr=(BYTE *)data;
int     i;

    for( i=0; (i < size); i++ )
    {
        GLOVE_BuffPutByte(item,bptr[i]);
    }
}
/******************************************************************************/

void    GLOVE_BuffGetData( int item, void *data, int size )
{
BYTE   *bptr=(BYTE *)data;
int     i;

    for( i=0; (i < size); i++ )
    {
        bptr[i] = GLOVE_BuffGetByte(item);
    }
}

/******************************************************************************/

int     GLOVE_BuffSize( int item )
{
    return(GLOVE_BuffIndx[item]);
}

/******************************************************************************/

void    GLOVE_BuffCommand( BYTE code )
{
    GLOVE_BuffInit(GLOVE_BUFF_COMMAND);
    GLOVE_BuffPutByte(GLOVE_BUFF_COMMAND,code);
    GLOVE_BuffSet(GLOVE_BUFF_COMMAND);
}

/******************************************************************************/

void    GLOVE_BuffQuery( BYTE code )
{
    GLOVE_BuffInit(GLOVE_BUFF_COMMAND);
    GLOVE_BuffPutByte(GLOVE_BUFF_COMMAND,GLOVE_QUERY);
    GLOVE_BuffPutByte(GLOVE_BUFF_COMMAND,code);
    GLOVE_BuffSet(GLOVE_BUFF_COMMAND);
}

/******************************************************************************/

BOOL    GLOVE_BuffWrite( int comH )
{
BOOL    ok=FALSE;

    if( comH != COM_INVALID )
    {
        ok = COM_Write(comH,GLOVE_Buff[GLOVE_BUFF_COMMAND],GLOVE_BuffSize(GLOVE_BUFF_COMMAND));
    }

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_BuffSend( int ID )
{
BOOL    ok=FALSE;

    if( GLOVE_Check(ID) )
    {
        ok = GLOVE_BuffWrite(GLOVE_Port[ID].comH);
    }

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_BuffSend( int ID, BOOL framed, void *buff, int size )
{
BOOL    ok;
int     command,recv,reply;

    if( (command=GLOVE_BuffGet(GLOVE_BUFF_COMMAND)) == 0 )
    {
        GLOVE_errorf("GLOVE_BuffSend(ID=%d,...) Empty command buffer.\n",ID);
        return(FALSE);
    }

    recv = command + size;

    // Send GLOVE command and wait for reply...
    if( (ok=GLOVE_BuffSend(ID)) )
    {
        // Only wait for reply if not streaming data...
        if( !GLOVE_Streaming(ID) )
        {
            if( (ok=GLOVE_Response(ID,framed,GLOVE_Recv,recv)) )
            {
                reply = recv - command;
   
                if( reply != size )
                {
                    GLOVE_errorf("GLOVE_BuffSend(ID=%d,...) Reply (%d) did not match size (%d).\n",ID,reply,size);
                    ok = FALSE;
                }
                else
                {
                    memcpy(buff,&GLOVE_Recv[command],size);
                }
            }
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_BuffSend( int ID, void *buff, int size )
{
BOOL ok;

    ok = GLOVE_BuffSend(ID,FALSE,buff,size);

    return(ok);
}

/******************************************************************************/
/* Implementations of GLOVE commands...                                       */
/******************************************************************************/

BOOL GLOVE_Command( int ID, int action, BYTE command, BYTE *data, int size )
{
BOOL ok;

    switch( action )
    {
        case GLOVE_SET :
           GLOVE_BuffCommand(command);
           GLOVE_BuffPutData(GLOVE_BUFF_COMMAND,data,size);
           break;

        case GLOVE_GET :
           GLOVE_BuffQuery(command);
           break;
    }

    ok = GLOVE_BuffSend(ID,data,size);

    STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_%s%s(ID=%d,...) %s.\n",STR_TextCode(GLOVE_ActionText,action),STR_TextCode(GLOVE_CommandText,command),ID,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SamplePeriod( int ID, int action, WORD &w1, WORD &w2 )
{
BOOL ok=FALSE;

    GLOVE_BuffInit(GLOVE_BUFF_DATA);
    GLOVE_BuffPutWord(GLOVE_BUFF_DATA,w1);
    GLOVE_BuffPutWord(GLOVE_BUFF_DATA,w2);

    ok = GLOVE_Command(ID,action,GLOVE_SAMPLEPERIOD,GLOVE_Buff[GLOVE_BUFF_DATA],GLOVE_BuffSize(GLOVE_BUFF_DATA));

    if( ok && (action == GLOVE_GET) )
    {
        GLOVE_BuffInit(GLOVE_BUFF_DATA);
        w1 = GLOVE_BuffGetWord(GLOVE_BUFF_DATA);
        w2 = GLOVE_BuffGetWord(GLOVE_BUFF_DATA);
    }

printf("%s=%s w1=%d w2=%d.\n",STR_TextCode(GLOVE_ActionText,action),STR_OkFailed(ok),w1,w2);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SampleRate( int ID, int action, double &rateHz )
{
BOOL ok;
int period;
WORD w1,w2;

    if( action == GLOVE_SET )
    {
        period = (int)(115200.0 / rateHz);

        if( period < INT16_MAX )
        {
            w1 = (WORD)period;
            w2 = 1;
        }
        else
        {
            w1 = (WORD)(period / 2);
            w2 = 2;
        }
    }

    ok = GLOVE_SamplePeriod(ID,action,w1,w2);

    if( ok && (action == GLOVE_GET) )
    {
        period = (int)w1 * (int)w2;
        rateHz = 115200.0 * (double)period;
    }

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetSampleRate( int ID, double &rateHz )
{
BOOL ok;

    ok = GLOVE_SampleRate(ID,GLOVE_GET,rateHz);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SetSampleRate( int ID, double rateHz )
{
BOOL ok;

    ok = GLOVE_SampleRate(ID,GLOVE_SET,rateHz);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_QueryConnected( int ID, BOOL &initialized, BOOL &connected )
{
BOOL    ok;
BYTE    buff;
int     size=sizeof(buff);

    GLOVE_BuffQuery(GLOVE_CONNECTED);

    connected = FALSE;
    initialized = FALSE;

    if( (ok=GLOVE_BuffSend(ID,&buff,size)) )
    {
        if( buff & GLOVE_CONNECTED_INITIALIZED )
        {
            initialized = TRUE;
        }

        if( buff & GLOVE_CONNECTED_GLOVE )
        {
            connected = TRUE;
        }
    }

    STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_QueryConnected(ID=%d,initialized=%s,connected=%s) %s.\n",ID,STR_YesNo(initialized),STR_YesNo(connected),STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_QuerySensors( int ID, int &sensors )
{
BOOL    ok;
BYTE    buff;
int     size=sizeof(buff);

    GLOVE_BuffQuery(GLOVE_SENSORS);

    sensors = 0;

    if( (ok=GLOVE_BuffSend(ID,&buff,size)) )
    {
        sensors = (int)buff;
    }

    STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_QuerySensors(ID=%d,sensors=%d) %s.\n",ID,sensors,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_QueryHand( int ID, int &hand )
{
BOOL    ok;
BYTE    buff;
int     size=sizeof(buff);

    GLOVE_BuffQuery(GLOVE_HAND);

    hand = GLOVE_HAND_INVALID;

    if( (ok=GLOVE_BuffSend(ID,&buff,size)) )
    {
        switch( buff )
        {
            case GLOVE_HAND_RIGHT :
            case GLOVE_HAND_LEFT :
               hand = (int)buff;
               break;
        }
    }

    STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_QueryHand(ID=%d,hand=%s) %s.\n",ID,STR_TextCode(GLOVE_HandText,hand),STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_Flag( int ID, int action, BYTE code, BOOL &flag )
{
BOOL    ok;
BYTE    buff;
int     size;

    switch( action )
    {
        case GLOVE_SET :
           GLOVE_BuffCommand(code);
           GLOVE_BuffPutByte(GLOVE_BUFF_COMMAND,(BYTE)flag);
           size = 0;
           break;

        case GLOVE_GET :
           GLOVE_BuffQuery(code);
           size = 1;
           break;
    }

    if( (ok=GLOVE_BuffSend(ID,&buff,size)) )
    {
        if( action == GLOVE_GET )
        {
            flag = (BOOL)buff;
        }
    }

    STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_Flag(ID=%d,action=%s,code=%s,flag=%s) %s.\n",ID,STR_TextCode(GLOVE_ActionText,action),STR_TextCode(GLOVE_FlagText,code),STR_YesNo(flag),STR_OkFailed(ok));

    if( ok )
    {
        GLOVE_Flag(ID,code) = flag;
    }

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_FlagLight( int ID, int action, BOOL &flag )
{
BOOL ok;

    ok = GLOVE_Flag(ID,action,GLOVE_FLAG_LIGHT,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_FlagSwitchLight( int ID, int action, BOOL &flag )
{
BOOL ok;

    ok = GLOVE_Flag(ID,action,GLOVE_FLAG_SWITCHLIGHT,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_FlagSwitchStatus( int ID, int action, BOOL &flag )
{
BOOL ok;

    ok = GLOVE_Flag(ID,action,GLOVE_FLAG_SWITCHSTATUS,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_FlagGloveStatus( int ID, int action, BOOL &flag )
{
BOOL ok;

    ok = GLOVE_Flag(ID,action,GLOVE_FLAG_GLOVESTATUS,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SetLight( int ID, BOOL flag )
{
BOOL ok;

    ok = GLOVE_FlagLight(ID,GLOVE_SET,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetLight( int ID, BOOL &flag )
{
BOOL ok;

    ok = GLOVE_FlagLight(ID,GLOVE_GET,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SetSwitchLight( int ID, BOOL flag )
{
BOOL ok;

    ok = GLOVE_FlagSwitchLight(ID,GLOVE_SET,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetSwitchLight( int ID, BOOL &flag )
{
BOOL ok;

    ok = GLOVE_FlagSwitchLight(ID,GLOVE_GET,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SetSwitchStatus( int ID, BOOL flag )
{
BOOL ok;

    ok = GLOVE_FlagSwitchStatus(ID,GLOVE_SET,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetSwitchStatus( int ID, BOOL &flag )
{
BOOL ok;

    ok = GLOVE_FlagSwitchStatus(ID,GLOVE_GET,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SetGloveStatus( int ID, BOOL flag )
{
BOOL ok;

    ok = GLOVE_FlagGloveStatus(ID,GLOVE_SET,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetGloveStatus( int ID, BOOL &flag )
{
BOOL ok;

    ok = GLOVE_FlagGloveStatus(ID,GLOVE_GET,flag);

    return(ok);
}

/******************************************************************************/

void GLOVE_GloveStatus( int ID, BYTE *buff, int &b )
{
BOOL flag;

    if( !GLOVE_Check(ID) )
    {
        return;
    }

    GLOVE_Port[ID].glovestatus = buff[b];
    b += GLOVE_SIZE_GLOVESTATUS;

    // Process glove status flags...
    flag = ((GLOVE_Port[ID].glovestatus & GLOVE_GLOVESTATUS_SWITCH) != 0);

    if( flag != GLOVE_Flag(ID,GLOVE_FLAG_SWITCHSTATUS) )
    {
        GLOVE_Flag(ID,GLOVE_FLAG_SWITCHSTATUS) = flag;
    }

    flag = ((GLOVE_Port[ID].glovestatus & GLOVE_GLOVESTATUS_LIGHT) != 0);

    if( flag != GLOVE_Flag(ID,GLOVE_FLAG_LIGHT) )
    {
        GLOVE_Flag(ID,GLOVE_FLAG_LIGHT) = flag;
    }
}

/******************************************************************************/

void GLOVE_TimeStamp( int ID, BYTE *buff, int &b )
{
    if( !GLOVE_Check(ID) )
    {
        return;
    }

    GLOVE_Port[ID].timestamp = GLOVE_Raw2TimeStamp(&buff[b]);
    b += GLOVE_SIZE_TIMESTAMP;
}

/******************************************************************************/

double  GLOVE_Raw2TimeStamp( BYTE *buff )
{
double  timestamp=0.0;
long   *ticks;
int     b;

    for( b=0; (b < sizeof(long)); b++ )
    {
        if( buff[0] & (1 << b) )
        {
            buff[b+1] = 0;
        }
    }

    ticks = (long *)&buff[1];

    timestamp = GLOVE_MSECPERTICK * (double)(*ticks);

    return(timestamp);
}

/******************************************************************************/

void    GLOVE_Raw2Data( int ID, BYTE *buff )
{
int     sensor,b;

    // Process glove sensor data...
    for( sensor=0,b=0; (sensor < GLOVE_Port[ID].selected); sensor++,b++ )
    {
        GLOVE_Port[ID].data[sensor] = buff[b];
    }

    // Process time-stamp if required...
    if( GLOVE_Flag(ID,GLOVE_FLAG_TIMESTAMP) )
    {
        GLOVE_TimeStamp(ID,buff,b);
    }

    // Process status byte if required...
    if( GLOVE_Flag(ID,GLOVE_FLAG_GLOVESTATUS) )
    {
        GLOVE_GloveStatus(ID,buff,b);
    }
}

/******************************************************************************/

int     GLOVE_RecordSize( int ID )
{
int     size;

    // Size of data record is number of sensors...
    size = GLOVE_Port[ID].selected;

    // Plus time stamp (if selected)...
    if( GLOVE_Flag(ID,GLOVE_FLAG_TIMESTAMP) )
    {
        size += GLOVE_SIZE_TIMESTAMP;
    }

    // Pllus status (if selected)...
    if( GLOVE_Flag(ID,GLOVE_FLAG_GLOVESTATUS) )
    {
        size += GLOVE_SIZE_GLOVESTATUS;
    }

    return(size);
}

/******************************************************************************/

void GLOVE_GetData( int ID, BYTE data[], double &timestamp, BYTE &glovestatus )
{
int s;

    for( s=0; (s < GLOVE_Port[ID].selected); s++ )
    {
        data[s] = GLOVE_Port[ID].data[s];
    }

    timestamp = GLOVE_Port[ID].timestamp;    
    glovestatus = GLOVE_Port[ID].glovestatus;    
}

/******************************************************************************/

BOOL    GLOVE_GetRecord( int ID, BYTE data[], double &timestamp, BYTE &glovestatus )
{
BOOL    ok;
int     sensor;
BYTE    buff[GLOVE_BUFF];
int     size;

    // Make sure API and Glove is running...
    // Make sure glove has been started...
    if( !GLOVE_Started(ID) )
    {
        return(FALSE);
    }

    // Size of data record...
    size = GLOVE_Port[ID].size;

    // Set command to retreieve a single data record...
    GLOVE_BuffCommand(GLOVE_DATA_SINGLE);

    GLOVE_Port[ID].frametime->Before();

    // Send command and receive response...
    if( (ok=GLOVE_BuffSend(ID,TRUE,buff,size)) )
    {   // Convert raw data into our format...
        GLOVE_Raw2Data(ID,buff);
    }

    GLOVE_Port[ID].frametime->After();

    STR_printf(ok,GLOVE_debugf,GLOVE_errorf,"GLOVE_DataRecord(ID=%d,...) %s.\n",ID,STR_OkFailed(ok));

    if( ok )
    {
        GLOVE_GetData(ID,data,timestamp,glovestatus);

        GLOVE_Port[ID].framerate->Loop();
        GLOVE_Port[ID].frame++;
    }

    return(ok);
}

/******************************************************************************/

void GLOVE_MatrixDimension( int ID, matrix &mtx )
{
int sensors;

    if( (sensors=GLOVE_Sensors(ID)) != 0 )
    {
        matrix_dim(mtx,sensors,1);
    }
}

/******************************************************************************/

BOOL    GLOVE_GetRecord( int ID, matrix &angles, double &timestamp, BYTE &glovestatus )
{
BOOL ok;
BYTE data[GLOVE_SENSOR];

    // Make sure glove has been started...
    if( !GLOVE_Started(ID) )
    {
        return(FALSE);
    }

    GLOVE_MatrixDimension(ID,angles);

    if( (ok=GLOVE_GetRecord(ID,data,timestamp,glovestatus)) )
    {
        GLOVE_Raw2Angles(data,GLOVE_Port[ID].calibdata,angles);
    }

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_GetRecord( int ID, BYTE data[], double &timestamp )
{
BOOL    ok;
BYTE    glovestatus;

    ok = GLOVE_GetRecord(ID,data,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_GetRecord( int ID, matrix &angles, double &timestamp )
{
BOOL    ok;
BYTE    glovestatus;

    ok = GLOVE_GetRecord(ID,angles,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_GetRecord( int ID, BYTE data[] )
{
BOOL    ok;
double  timestamp;
BYTE    glovestatus;

    ok = GLOVE_GetRecord(ID,data,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_GetRecord( int ID, matrix &angles )
{
BOOL    ok;
double  timestamp;
BYTE    glovestatus;

    ok = GLOVE_GetRecord(ID,angles,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_Stream( int ID, BOOL flag )
{
BOOL    ok;

    if( flag )
    {
        // Set & send stream data command...
        GLOVE_BuffCommand(GLOVE_DATA_STREAM);
        ok = GLOVE_BuffSend(ID);
    }
    else
    {
        // Set & send control break command...
        GLOVE_BuffCommand(GLOVE_BREAK);
        ok = GLOVE_BuffSend(ID);
    }

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_ReadByte( int ID, BYTE &byte, BOOL &ok, BOOL &done )
{
    if( !(ok=COM_Read(GLOVE_Port[ID].comH,&byte,1,done,0L)) )
    {
        GLOVE_Port[ID].error = TRUE;
    }

    return(ok && done);
}

/******************************************************************************/

void    GLOVE_LoopTask( int ID )
{
BOOL    ok,done;
int     size,got,want,now;
BYTE    byte;

    if( !GLOVE_Streaming(ID) )
    {
        return;
    }

    switch( GLOVE_Port[ID].stage )
    {
        case GLOVE_STAGE_START :
           GLOVE_Port[ID].got = 0;
           GLOVE_Port[ID].query = FALSE;
           GLOVE_Port[ID].stage = GLOVE_STAGE_WAITING;
           break;

        case GLOVE_STAGE_WAITING :
           GLOVE_Port[ID].frametime->Before();

           if( !GLOVE_ReadByte(ID,byte,ok,done) )
           {
               break;
           }


           if( byte == GLOVE_QUERY )
           {
               GLOVE_Port[ID].query = TRUE;
               break;
           }

           GLOVE_Port[ID].command = byte;
           GLOVE_Port[ID].stage = GLOVE_STAGE_COMMAND;
           break;

        case GLOVE_STAGE_COMMAND :
           // Process streaming data record...
           if( GLOVE_Port[ID].command == GLOVE_DATA_STREAM )
           {
               GLOVE_Port[ID].stage = GLOVE_STAGE_DATA;
               break;
           }

           // Otherwise, synchronise with end of miscellaneous record...
           GLOVE_Port[ID].stage = GLOVE_STAGE_SYNC;
           break;

        case GLOVE_STAGE_DATA :
           // What we want is what we need less what we have...
           want = GLOVE_Port[ID].size - GLOVE_Port[ID].got;
           now = 0;

           if( !(ok=COM_Read(GLOVE_Port[ID].comH,&GLOVE_Port[ID].buff[GLOVE_Port[ID].got],want,now,done,0L)) )
           {
               GLOVE_Port[ID].error = TRUE;
               break;
           }

           GLOVE_Port[ID].got += now;

           if( GLOVE_Port[ID].got == GLOVE_Port[ID].size )
           {
               GLOVE_Port[ID].stage = GLOVE_STAGE_NUL;
           }
           break;

        case GLOVE_STAGE_NUL :
           if( !GLOVE_ReadByte(ID,byte,ok,done) )
           {
               break;
           }

           if( byte != 0x00 )
           {
               GLOVE_Port[ID].sync++;
               GLOVE_Port[ID].stage = GLOVE_STAGE_SYNC;
               break;
           }

           // Process streaming data record...
           if( GLOVE_Port[ID].command == GLOVE_DATA_STREAM )
           {
               GLOVE_Port[ID].frametime->After();
               GLOVE_Port[ID].framerate->Loop();
               GLOVE_Port[ID].framelast->Reset();

               GLOVE_Raw2Data(ID,GLOVE_Port[ID].buff);

               GLOVE_Port[ID].frame++;

               if( GLOVE_Port[ID].framedata )
               {
                   GLOVE_Port[ID].overwrite++;
               }
               else
               {
                   GLOVE_Port[ID].framedata = TRUE;
               }
           }

           GLOVE_Port[ID].stage = GLOVE_STAGE_START;
           break;

        case GLOVE_STAGE_SYNC :
           if( !GLOVE_ReadByte(ID,byte,ok,done) )
           {
               break;
           }

           if( byte == 0x00 )
           {
               GLOVE_Port[ID].stage = GLOVE_STAGE_START;
           }
           break;
    }
}

/******************************************************************************/

void   _GLOVE_LoopTask( int ID )
{
BOOL    ok,done;
int     size,got,want,now,i;
BYTE    byte;

    if( !GLOVE_Streaming(ID) )
    {
        return;
    }

    if( !(ok=COM_Read(GLOVE_Port[ID].comH,&byte,1,done,0L)) )
    {
        GLOVE_Port[ID].error = TRUE;
        return;
    }

    if( !done ) 
    {
        return;
    }

    if( byte == 0x00 )
    {
        if( TIMER_EveryHz(1.0) ) 
        {
            printf("%d bytes.\n",GLOVE_Port[ID].got);
            printf("%.*s 0x%02X\n",GLOVE_Port[ID].got,GLOVE_Port[ID].buff,GLOVE_Port[ID].buff[GLOVE_Port[ID].got-1]);
        }

        GLOVE_Port[ID].got = 0;
        GLOVE_Port[ID].frame++;
        return;
    }

    if( GLOVE_Port[ID].got < GLOVE_BUFF )
    {
        GLOVE_Port[ID].buff[GLOVE_Port[ID].got] = byte;
    }

    GLOVE_Port[ID].got++;
}

/******************************************************************************/

void    GLOVE_LoopTask( void )
{
int     ID;

    for( ID=0; (ID < GLOVE_PORT_MAX); ID++ )
    {
        GLOVE_LoopTask(ID);
    }
}

/******************************************************************************/

BOOL    GLOVE_Fresh( int ID )
{
BOOL    fresh=FALSE;

    if( GLOVE_Started(ID) )
    {
        fresh = GLOVE_Port[ID].framedata;
    }

    return(fresh);
}

/******************************************************************************/

BOOL GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, BYTE data[], double &timestamp, BYTE &glovestatus )
{
BOOL ok;

    // Make sure glove has been started...
    if( !GLOVE_Started(ID) )
    {
        return(FALSE);
    }

    // If not in streaming mode, request a record...
    if( !GLOVE_Streaming(ID) )
    {
        if( (ok=GLOVE_GetRecord(ID,data,timestamp,glovestatus)) )
        {
            fresh = TRUE;
            frameage = 0.0;
        }

        return(ok);
    }

    // An error of glove port so return...
    if( GLOVE_Port[ID].error )
    {
        return(FALSE);
    }

    // Is a new frame waiting...
    if( (fresh=GLOVE_Port[ID].framedata) )
    {
        frameage = GLOVE_Port[ID].framelast->Elapsed();
        GLOVE_Port[ID].framewait->Data(frameage);
        GLOVE_Port[ID].framedata = FALSE;
    }
    else
    {
        frameage = 0.0;
    }

    GLOVE_GetData(ID,data,timestamp,glovestatus);

    return(TRUE);
}

/******************************************************************************/

BOOL GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, matrix &angles, double &timestamp, BYTE &glovestatus )
{
BOOL ok;
BYTE data[GLOVE_SENSOR];

    // Make sure glove has been started...
    if( !GLOVE_Started(ID) )
    {
        return(FALSE);
    }

    GLOVE_MatrixDimension(ID,angles);

    if( (ok=GLOVE_LastRecord(ID,frameage,fresh,data,timestamp,glovestatus)) )
    {
        GLOVE_Raw2Angles(data,GLOVE_Port[ID].calibdata,angles);
    }

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, BYTE data[], double &timestamp, BYTE &glovestatus )
{
BOOL    ok;
double  frameage;

    ok = GLOVE_LastRecord(ID,frameage,fresh,data,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, matrix &angles, double &timestamp, BYTE &glovestatus )
{
BOOL    ok;
double  frameage;

    ok = GLOVE_LastRecord(ID,frameage,fresh,angles,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, BYTE data[], double &timestamp )
{
BOOL    ok;
BYTE    glovestatus;

    ok = GLOVE_LastRecord(ID,frameage,fresh,data,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, matrix &angles, double &timestamp )
{
BOOL    ok;
BYTE    glovestatus;

    ok = GLOVE_LastRecord(ID,frameage,fresh,angles,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, BYTE data[], double &timestamp )
{
BOOL    ok;
double  frameage;
BYTE    glovestatus;

    ok = GLOVE_LastRecord(ID,frameage,fresh,data,timestamp,glovestatus);

    return(ok);
}


/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, matrix &angles, double &timestamp )
{
BOOL    ok;
double  frameage;
BYTE    glovestatus;

    ok = GLOVE_LastRecord(ID,frameage,fresh,angles,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, BYTE data[] )
{
BOOL    ok;
double  timestamp;
BYTE    glovestatus;

    ok = GLOVE_LastRecord(ID,frameage,fresh,data,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, double &frameage, BOOL &fresh, matrix &angles )
{
BOOL    ok;
double  timestamp;
BYTE    glovestatus;

    ok = GLOVE_LastRecord(ID,frameage,fresh,angles,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, BYTE data[] )
{
BOOL    ok;
double  frameage,timestamp;
BYTE    glovestatus;

    ok = GLOVE_LastRecord(ID,frameage,fresh,data,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_LastRecord( int ID, BOOL &fresh, matrix &angles )
{
BOOL    ok;
double  frameage,timestamp;
BYTE    glovestatus;

    ok = GLOVE_LastRecord(ID,frameage,fresh,angles,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

void GLOVE_Close( void )
{
    GLOVE_Close(GLOVE_ID);
}

/******************************************************************************/

BOOL GLOVE_Start( void )
{
BOOL ok;

    ok = GLOVE_Start(GLOVE_ID);

    return(ok);
}

/******************************************************************************/

void GLOVE_Stop( void )
{
    GLOVE_Stop(GLOVE_ID);
}

/******************************************************************************/

void GLOVE_MatrixDimension( matrix &mtx )
{
    GLOVE_MatrixDimension(GLOVE_ID,mtx);
}

/******************************************************************************/

int GLOVE_Sensors( void )
{
int sensors;

    sensors = GLOVE_Sensors(GLOVE_ID);

    return(sensors);
}

/******************************************************************************/

double GLOVE_FrameRate( void )
{
int framerate;

    framerate = GLOVE_FrameRate(GLOVE_ID);

    return(framerate);
}

/******************************************************************************/

BOOL GLOVE_Started( void )
{
BOOL flag;

    flag = GLOVE_Started(GLOVE_ID);

    return(flag);
}

/******************************************************************************/

BOOL GLOVE_Streaming( void )
{
BOOL flag;

    flag = GLOVE_Streaming(GLOVE_ID);

    return(flag);
}

/******************************************************************************/

BOOL GLOVE_GetRecord( BYTE data[], double &timestamp, BYTE &glovestatus )
{
BOOL ok;

    ok = GLOVE_GetRecord(GLOVE_ID,data,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetRecord( matrix &angles, double &timestamp, BYTE &glovestatus )
{
BOOL ok;

    ok = GLOVE_GetRecord(GLOVE_ID,angles,timestamp,glovestatus);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_GetRecord( BYTE data[], double &timestamp )
{
BOOL ok;

    ok = GLOVE_GetRecord(GLOVE_ID,data,timestamp);

    return(ok);
}

/******************************************************************************/

BOOL    GLOVE_GetRecord( matrix &angles, double &timestamp )
{
BOOL ok;

    ok = GLOVE_GetRecord(GLOVE_ID,angles,timestamp);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_GetRecord( BYTE data[] )
{
BOOL ok;

    ok = GLOVE_GetRecord(GLOVE_ID,data);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_GetRecord( matrix &angles )
{
BOOL ok;

    ok = GLOVE_GetRecord(GLOVE_ID,angles);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( double &frameage, BOOL &fresh, BYTE data[], double &timestamp, BYTE &glovestatus )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,frameage,fresh,data,timestamp,glovestatus);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( double &frameage, BOOL &fresh, matrix &angles, double &timestamp, BYTE &glovestatus )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,frameage,fresh,angles,timestamp,glovestatus);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( double &frameage, BOOL &fresh, BYTE data[], double &timestamp )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,frameage,fresh,data,timestamp);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( double &frameage, BOOL &fresh, matrix &angles, double &timestamp )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,frameage,fresh,angles,timestamp);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( double &frameage, BOOL &fresh, BYTE data[] )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,frameage,fresh,data);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( double &frameage, BOOL &fresh, matrix &angles )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,frameage,fresh,angles);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( BOOL &fresh, BYTE data[], double &timestamp, BYTE &glovestatus )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,fresh,data,timestamp,glovestatus);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( BOOL &fresh, matrix &angles, double &timestamp, BYTE &glovestatus )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,fresh,angles,timestamp,glovestatus);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( BOOL &fresh, BYTE data[], double &timestamp )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,fresh,data,timestamp);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( BOOL &fresh, matrix &angles, double &timestamp )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,fresh,angles,timestamp);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( BOOL &fresh, BYTE data[] )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,fresh,data);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( BOOL &fresh, matrix &angles )
{
BOOL ok;

    ok = GLOVE_LastRecord(GLOVE_ID,fresh,angles);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( BYTE data[] )
{
BOOL ok,fresh;

    ok = GLOVE_LastRecord(GLOVE_ID,fresh,data);

    return(ok);
}

/**************************************************************************************/

BOOL GLOVE_LastRecord( matrix &angles )
{
BOOL ok,fresh;

    ok = GLOVE_LastRecord(GLOVE_ID,fresh,angles);

    return(ok);
}

/******************************************************************************/

BOOL   &GLOVE_Flag( int code )
{
    return(GLOVE_Flag(GLOVE_ID,code));
}

/******************************************************************************/

BOOL GLOVE_SetLight( BOOL flag )
{
BOOL ok;

    ok = GLOVE_SetLight(GLOVE_ID,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetLight( BOOL &flag )
{
BOOL ok;

    ok = GLOVE_GetLight(GLOVE_ID,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SetSwitchLight( BOOL flag )
{
BOOL ok;

    ok = GLOVE_SetSwitchLight(GLOVE_ID,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetSwitchLight( BOOL &flag )
{
BOOL ok;

    ok = GLOVE_GetSwitchLight(GLOVE_ID,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SetSwitchStatus( BOOL flag )
{
BOOL ok;

    ok = GLOVE_SetSwitchStatus(GLOVE_ID,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetSwitchStatus( BOOL &flag )
{
BOOL ok;

    ok = GLOVE_GetSwitchStatus(GLOVE_ID,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_SetGloveStatus( BOOL flag )
{
BOOL ok;

    ok = GLOVE_SetGloveStatus(GLOVE_ID,flag);

    return(ok);
}

/******************************************************************************/

BOOL GLOVE_GetGloveStatus( BOOL &flag )
{
BOOL ok;

    ok = GLOVE_GetGloveStatus(GLOVE_ID,flag);

    return(ok);
}

/******************************************************************************/

void GLOVE_Raw2Angles( BYTE data[], matrix &calib, matrix &angles )
{
int s;
double m=1.0,c=0.0,x;

    for( s=1; (s <= GLOVE_SENSOR); s++ )
    {
        if( !angles.exist(s,1) )
        {
            break;
        }

        m = 1.0;
        c = 0.0;
        x = (double)data[s-1];

        if( calib.exist(s,1) )
        {
            m = calib(s,1);
            c = calib(s,2);
        }

        // Linear calibration: y = m(x + c)...
        angles(s,1) = m * (x + c);
    }
}

/**************************************************************************************/

