/******************************************************************************/
/*                                                                            */
/* MODULE  : FOB.cpp                                                          */
/*                                                                            */
/* PURPOSE : Flock of Birds API functions.                                    */
/*                                                                            */
/* DATE    : 04/Nov/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 04/Nov/2000 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 14/Jun/2001 - Fixed (hopefully) problem with ROMX (FOB-DATA.CPP).*/
/*                         Linear order of values from FOB means that matrix  */
/*                         column changes less frequently that row.           */
/*                                                                            */
/* V1.2  JNI 12/Jul/2001 - Made COM port variable (FOB configuration file) an */
/*                         array so that multiple ports can be specified.     */
/*                                                                            */
/* V1.3  JNI 30/Nov/2001 - Track positions through hemisphere changes.        */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "FOB"
#define MODULE_TEXT     "Flock of Birds API"
#define MODULE_DATE     "30/11/2001"
#define MODULE_VERSION  "1.3"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

struct  FOB_PortHandle  FOB_Port[FOB_PORT_MAX];
FOB_RawFrame            FOB_RawData[FOB_PORT_MAX];
int                     FOB_PortCount=0;
int                     FOB_BirdCount=0;
int                     FOB_ID=FOB_PORT_INVALID;
TIMER                   FOB_Timer("FOB_Timer",TIMER_MODE_RESET);

/******************************************************************************/

BOOL    FOB_API_started = FALSE;

/******************************************************************************/

PRINTF  FOB_PRN_messgf=NULL;                     // General messages printf function.
PRINTF  FOB_PRN_errorf=NULL;                     // Error messages printf function.
PRINTF  FOB_PRN_debugf=NULL;                     // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     FOB_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(FOB_PRN_messgf,buff));
}

/******************************************************************************/

int     FOB_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(FOB_PRN_errorf,buff));
}

/******************************************************************************/

int     FOB_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(FOB_PRN_debugf,buff));
}

/******************************************************************************/
/* FOB configuration file variables...                                        */
/******************************************************************************/

int     FOB_comX[FOB_PORT_MAX];        // Com port number (array to multiple ports).
int     FOB_birds;                     // Number of birds in flock.
long    FOB_baudrate;                  // Baud rate for com port.
float   FOB_framerate;                 // Frame rate (Hz).
WORD    FOB_scale;                     // Scale (inches).
BYTE    FOB_datamode;                  // Data collection mode.
BYTE    FOB_dataformat[FOB_BIRD_MAX];  // Data format (can be different for each bird).
BYTE    FOB_groupmode;                 // Group mode.
BYTE    FOB_hemisphere[FOB_BIRD_MAX];  // Hemisphere (can be different for each bird).
BYTE    FOB_angleunit;                 // Units (degrees or radians) for angles.
WORD    FOB_filterstatus;              // FOB filter ON/OFF status flags...
WORD    FOB_DCalphamin[FOB_DC_TABLE];  // DC Filter table (alpha min).
WORD    FOB_DCalphamax[FOB_DC_TABLE];  // DC Filter table (alpha max).
WORD    FOB_DCtableVm[FOB_DC_TABLE];   // DC Filter table (Vm).
BYTE    FOB_outputchange;              // Sudden output change lock.
BYTE    FOB_transmittermode;           // Transmitter operation mode.
BOOL    FOB_tracking;                  // Hemispheres tracking.
int     FOB_axis;                      // Axis system.
BYTE    FOB_sync;                      // SYNC mode.

STRING  FOB_str_datamode;              // String versions of configuration variables...
STRING  FOB_str_dataformat[FOB_BIRD_MAX];
STRING  FOB_str_groupmode;
STRING  FOB_str_hemisphere[FOB_BIRD_MAX];
STRING  FOB_str_angleunit;
STRING  FOB_str_filters[FOB_FILTER_MAX];
STRING  FOB_str_outputchange;
STRING  FOB_str_transmittermode;
STRING  FOB_str_axis;
STRING  FOB_str_rtmx;
STRING  FOB_str_sync;

matrix  FOB_RTMX;                      // RTMX for user-definted co-ordinate frame.

int     FOB_filters[FOB_FILTER_MAX];   // Array for selected filters.

int     FOB_cfg_framerate;
int     FOB_cfg_scale;
int     FOB_cfg_hemisphere;
int     FOB_cfg_angleunit; 
int     FOB_cfg_filters;
int     FOB_cfg_DCalphamax;
int     FOB_cfg_DCalphamin;
int     FOB_cfg_DCtableVm;
int     FOB_cfg_outputchange;
int     FOB_cfg_transmittermode;
int     FOB_cfg_axis;
int     FOB_cfg_rtmx;
int     FOB_cfg_sync;

BOOL    FOB_cfg_read[CONFIG_CNFG];

float   FOB_frameHz;                   // Frame rate (Hz).
float   FOB_framemsec;                 // Frame interval (msec).

/******************************************************************************/

#include "fob-open.cpp"                // Open and Close functions.
#include "fob-info.cpp"                // FOB information structures and tables.  
#include "fob-data.cpp"                // Data conversions.
#include "fob-cmds.cpp"                // FOB RS232 commands.
#include "fob-stub.cpp"                // Default FOB ID function stubs.
#include "fob-task.cpp"                // Data stream look task stuff.

/******************************************************************************/

void    FOB_CnfgTable( void )
{
    CONFIG_reset();

    CONFIG_set(CONFIG_TYPE_INT,"comX",FOB_comX,FOB_PORT_MAX);
    CONFIG_set(CONFIG_TYPE_INT,"birds",&FOB_birds);
    CONFIG_set(CONFIG_TYPE_LONG,"baudrate",&FOB_baudrate);
    FOB_cfg_framerate = CONFIG_set(CONFIG_TYPE_FLOAT,"framerate",&FOB_framerate);
    FOB_cfg_scale = CONFIG_set(CONFIG_TYPE_SHORT,"scale",&FOB_scale);
    CONFIG_set(CONFIG_TYPE_STRING,"datamode",FOB_str_datamode);
    CONFIG_set(CONFIG_TYPE_STRING,"dataformat",FOB_str_dataformat,FOB_BIRD_MAX);
    CONFIG_set(CONFIG_TYPE_STRING,"groupmode",FOB_str_groupmode);
    FOB_cfg_hemisphere = CONFIG_set(CONFIG_TYPE_STRING,"hemisphere",FOB_str_hemisphere,FOB_BIRD_MAX);
    FOB_cfg_angleunit = CONFIG_set(CONFIG_TYPE_STRING,"angleunit",FOB_str_angleunit);
    FOB_cfg_filters = CONFIG_set(CONFIG_TYPE_STRING,"filters",FOB_str_filters,FOB_FILTER_MAX);
    FOB_cfg_DCalphamin = CONFIG_set(CONFIG_TYPE_SHORT,"DCalphaMin",FOB_DCalphamin,FOB_DC_TABLE);
    FOB_cfg_DCalphamax = CONFIG_set(CONFIG_TYPE_SHORT,"DCalphaMax",FOB_DCalphamax,FOB_DC_TABLE);
    FOB_cfg_DCtableVm = CONFIG_set(CONFIG_TYPE_SHORT,"DCtableVm",FOB_DCtableVm,FOB_DC_TABLE);
    FOB_cfg_outputchange = CONFIG_set(CONFIG_TYPE_STRING,"outputchange",FOB_str_outputchange);
    FOB_cfg_transmittermode = CONFIG_set(CONFIG_TYPE_STRING,"transmittermode",FOB_str_transmittermode);
    CONFIG_set(CONFIG_TYPE_BOOL,"tracking",&FOB_tracking);
    FOB_cfg_axis = CONFIG_set(CONFIG_TYPE_STRING,"axis",FOB_str_axis);
    FOB_cfg_rtmx = CONFIG_set(CONFIG_TYPE_STRING,"RTMX",FOB_str_rtmx);
    FOB_cfg_sync = CONFIG_set(CONFIG_TYPE_STRING,"sync",FOB_str_sync);
}

/******************************************************************************/

void    FOB_CnfgDefaults( void )
{
int     port,bird;

    for( port=0; (port < FOB_PORT_MAX); port++ )
    {
        FOB_comX[port] = 0;
    }

    for( bird=0; (bird < FOB_BIRD_MAX); bird++ )
    {
        memset(FOB_str_dataformat[FOB_BIRD_MAX],NUL,STRLEN);
        FOB_dataformat[bird] = FOB_DATAFORMAT_POSITION;

        memset(FOB_str_hemisphere[FOB_BIRD_MAX],NUL,STRLEN);
        FOB_hemisphere[bird] = FOB_HEMISPHERE_FRONT;
    }

    FOB_birds = 1;
    FOB_angleunit = FOB_ANGLEUNIT_DEGREES;
    FOB_scale = FOB_SCALE_36INCH;
    FOB_datamode = FOB_DATAMODE_POINT;                     
    FOB_baudrate = 115200;
    FOB_groupmode = FOB_GROUPMODE_OFF;
    FOB_transmittermode = FOB_TRANSMITMODE_COOLDOWN;
    FOB_tracking = FALSE;
    FOB_axis = SPMX_AXIS_NONE;
    memset(FOB_str_rtmx,0x00,STRLEN);
    FOB_sync = FOB_SYNC_NONE;

    matrix_empty(FOB_RTMX);
}

/******************************************************************************/

void    FOB_CnfgRead( void )
{
int     item;

    for( item=0; !CONFIG_EoT(item); item++ )
    {
        FOB_cfg_read[item] = CONFIG_flag(item,CONFIG_FLAG_READ);
    }
}

/******************************************************************************/

BOOL    FOB_TextCode( struct STR_TextItem xlat[], char *text, int &code, char *file, char *desc )
{
    if( (code=STR_TextCode(xlat,text)) == STR_TEXT_EOT )
    {
        FOB_errorf("FOB_Cnfg(%s) Invalid %s: %s.\n",file,desc,text);
        return(FALSE);
    }

    return(TRUE);
}

/******************************************************************************/

BOOL    FOB_TextByte( struct STR_TextItem xlat[], char *text, BYTE &code, char *file, char *desc )
{
BOOL    ok;
int     value;

    if( (ok=FOB_TextCode(xlat,text,value,file,desc)) )
    {
        code = (BYTE)value;
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_BoolByte( char *text, BYTE &code, char *file, char *desc )
{
int     value;

    if( (value=STR_Bool(text)) == STR_TEXT_EOT ) 	
    {
        FOB_errorf("FOB_Cnfg(%s) Invalid %s: %s.\n",file,desc,text);
        return(FALSE);
    }

    code = (BOOL)value;

    return(TRUE);
}

/******************************************************************************/

BOOL    FOB_CnfgAxis( char *type, int &axis )
{
BOOL    ok=TRUE;

    if( _stricmp(type,"NATIVE") == 0 )
    {
        axis = SPMX_AXIS_NONE;
    }
    else
    if( _stricmp(type,"OPENGL") == 0 )
    {
        axis = SPMX_AXIS_ZnXnY;
    }
    else
    if( (axis=SPMX_axistype(type)) == SPMX_AXIS_NONE )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CnfgLoad( char *file )
{
BOOL    ok,cfg;
int     bird,filter;
char   *path;

    if( (path=FILE_Calibration(file)) == NULL )
    {
        FOB_errorf("FOB_CnfgLoad(%s) Cannot find file.\n",file);
        return(FALSE);
    }

    FOB_CnfgTable();              // Set table of configuration variables...
    FOB_CnfgDefaults();           // Set configuration defaults...

    if( !CONFIG_read(path) )      // Load configuration file...
    {
        FOB_errorf("FOB_CnfgLoad(%s) Cannot read file.\n",file);
        return(FALSE);
    }

    FOB_CnfgRead();               // Set flags to indicate which variables were read...

    for( bird=0; (bird < FOB_birds); bird++ )
    {
        if( STR_null(FOB_str_dataformat[bird],STRLEN) )
        {
            FOB_dataformat[bird] = (bird > 0) ? FOB_dataformat[bird-1] : FOB_DATAFORMAT_POSITION;
        }
        else
        if( !FOB_TextByte(FOB_DataFormatText,FOB_str_dataformat[bird],FOB_dataformat[bird],file,"data format") )
        {
            ok = FALSE;
        }

        if( STR_null(FOB_str_hemisphere[bird],STRLEN) )
        {
            FOB_hemisphere[bird] = (bird > 0) ? FOB_hemisphere[bird-1] : FOB_HEMISPHERE_FRONT;
        }
        else
        if( !FOB_TextByte(FOB_HemisphereText,FOB_str_hemisphere[bird],FOB_hemisphere[bird],file,"hemisphere") )
        {
            ok = FALSE;
        }
    }

    // Start with all filters turned OFF...
    FOB_filterstatus = FOB_FILTER_ACNARROWOFF | FOB_FILTER_ACWIDEOFF | FOB_FILTER_DCOFF;

    for( filter=0; (filter < FOB_FILTER_MAX); filter++ )
    {
        if( STR_null(FOB_str_filters[filter],STRLEN) )
        {
            FOB_filters[filter] = FOB_FILTER_NONE;
            break;
        }

        if( !FOB_TextCode(FOB_FilterStatusText,FOB_str_filters[filter],FOB_filters[filter],file,"filters") )
        {
            ok = FALSE;
            break;
        }

        switch( FOB_filters[filter] )
        {
            case FOB_FILTER_NONE :
               break;

            case FOB_FILTER_ACNARROW :
               FOB_filterstatus &= ~FOB_FILTER_ACNARROWOFF;
               break;

            case FOB_FILTER_ACWIDE :
               FOB_filterstatus &= ~FOB_FILTER_ACWIDEOFF;
               break;
        
            case FOB_FILTER_DC :
               FOB_filterstatus &= ~FOB_FILTER_DCOFF;
               break;
        }
    }

    if( !FOB_TextByte(FOB_DataModeText,FOB_str_datamode,FOB_datamode,file,"data mode") )
    {
        ok = FALSE;
    }

    if( FOB_cfg_read[FOB_cfg_angleunit] )
    {
        if( !FOB_TextByte(FOB_AngleUnitText,FOB_str_angleunit,FOB_angleunit,file,"angle mode") )
        {
            ok = FALSE;
        }
    }

    if( !FOB_BoolByte(FOB_str_groupmode,FOB_groupmode,file,"group mode") )
    {
        ok = FALSE;
    }

    if( !FOB_BoolByte(FOB_str_outputchange,FOB_outputchange,file,"output change") )
    {
        ok = FALSE;
    }

    if( FOB_cfg_read[FOB_cfg_transmittermode] )
    {
        if( !FOB_TextByte(FOB_TransmitModeText,FOB_str_transmittermode,FOB_transmittermode,file,"transmitter mode") )
        {
            ok = FALSE;
        }
    }

    if( FOB_cfg_read[FOB_cfg_axis] )
    {
        if( !(ok=FOB_CnfgAxis(FOB_str_axis,FOB_axis)) )
        {
            FOB_errorf("FOB_Cnfg(%s) Invalid axis: %s.\n",file,FOB_str_axis);
        }
    }

    if( FOB_cfg_read[FOB_cfg_rtmx] )
    {
        if( !(cfg=matrix_read(FILE_Calibration(FOB_str_rtmx),FOB_RTMX)) )
        {
            ok = FALSE;
        }

        STR_printf(cfg,FOB_debugf,FOB_errorf,"FOB_Cnfg(%s) RTMX %s %s.\n",file,FOB_str_rtmx,STR_OkFailed(cfg));
    }

    if( FOB_cfg_read[FOB_cfg_sync] )
    {
        if( !FOB_TextByte(FOB_SyncText,FOB_str_sync,FOB_sync,file,"sync mode") )
        {
            ok = FALSE;
        }
    }

    return(ok);
}

/******************************************************************************/

void    FOB_DCB( DCB *dcb )
{
    dcb->ByteSize = 8;
    dcb->Parity = NOPARITY;
    dcb->StopBits = ONESTOPBIT;
    dcb->fAbortOnError = FALSE; // TRUE;
    dcb->fDtrControl = DTR_CONTROL_ENABLE;
    dcb->fDsrSensitivity = FALSE;
    dcb->fOutxDsrFlow = FALSE;
    dcb->fOutxCtsFlow = FALSE;
    dcb->fRtsControl = RTS_CONTROL_DISABLE;
}

/******************************************************************************/

void    FOB_Init( int ID )
{
    memset(&FOB_Port[ID],0x00,sizeof(struct FOB_PortHandle));
    FOB_Port[ID].used = FALSE;
    FOB_Port[ID].started = FALSE;
    FOB_Port[ID].comH = COM_INVALID;
}

/******************************************************************************/

int     FOB_Free( void )
{
int     ID,find;

//  Check if API started okay...
    if( !FOB_API_check() )                  
    {
        return(FOB_PORT_INVALID);
    }

//  Find free FOB port handle...
    for( find=FOB_PORT_INVALID,ID=0; (ID < FOB_PORT_MAX); ID++ )
    {
        if( !FOB_Port[ID].used )
        {
            find = ID;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

void    FOB_Use( int ID )
{
    FOB_ID = ID;
}

/******************************************************************************/

BOOL    FOB_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok;
int     ID;
matrix  FOB2XYZ;

    FOB_PRN_messgf = messgf;                // General API message print function.
    FOB_PRN_errorf = errorf;                // API error message print function.
    FOB_PRN_debugf = debugf;                // Debug information print function.

    if( FOB_API_started )                   // Start the API once...
    {
        return(TRUE);
    }

//  Initialize FOB port handles...
    for( ID=0; (ID < FOB_PORT_MAX); ID++ )
    {
        FOB_Init(ID);
    }

//  Rotate native FOB co-ordinate frame to make it XYZ...
//
//  FOB axis:  Y-----+       XYZ:     (2)Y        
//                  /|                   |  
//                 / |                   | 
//                X  Z                   +----- X(1)
//                                      /
//                                     /
//                                   Z(3)
//
//  Rotate FOB transmitter -90 degree around X, +90 around new Y to align axis.
//  But it only works in software with +90 X, -90 Y...
    FOB2XYZ = SPMX_romxY(-r90) * SPMX_romxX(+r90);

//  Install SPMX rotation to get from native FOB axis to default XYZ axis...
    SPMX_axisinit(&FOB2XYZ);

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(FOB_API_stop);           // Install stop function.
        FOB_API_started = TRUE;             // Set started flag.

        MODULE_start(FOB_PRN_messgf);       // Register module.
    }
    else
    {
        FOB_errorf("FOB_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    FOB_API_stop( void )
{
int     ID;

    if( !FOB_API_started )                  // API not started in the first place...
    {
         return;
    }

    FOB_CloseAll();

    FOB_API_started = FALSE;           // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    FOB_API_check( void )
{
BOOL    ok=TRUE;

    if( !FOB_API_started )             // API not started...
    {                                  // Start module automatically...
        ok = FOB_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Check( int ID )
{
BOOL    ok=TRUE;

    if( !FOB_API_check() )
    {
        ok = FALSE;
    }
    else
    if( ID == FOB_PORT_INVALID )
    {
        ok = FALSE;
    }
    else
    if( !FOB_Port[ID].used )
    {
        ok = FALSE;
    }
    else
    if( FOB_Port[ID].comH == COM_INVALID )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

char   *FOB_Error( BYTE code )
{
char   *buff;

    if( (buff=STR_Buff(STR_TEXT)) == NULL )
    {
        return(NULL);
    }

    if( code == FOB_ERROR_NONE )
    {
        strncpy(buff,"NO ERROR",STR_TEXT);
    }
    else
    {
        strncat(buff,FOB_ErrorFatal(code) ? "ERROR " : "WARNING ",STR_TEXT);
        strncat(buff,STR_stringf("[%02d] %s",code,STR_TextCode(FOB_ErrorText,code)),STR_TEXT);
    }

    return(buff);
}


/******************************************************************************/

int     FOB_PortsInFlock( void )
{
    return(FOB_PortCount);
}

/******************************************************************************/

int     FOB_MultiPortFlock( void )
{
    return(FOB_PortCount > 1);
}

/******************************************************************************/

int     FOB_BirdsInFlock( void )
{
    return(FOB_BirdCount);
}

/******************************************************************************/

int     FOB_BirdsOnPort( int ID )
{
int     birds=0;

    if( FOB_Check(ID) )
    {
        birds = FOB_Port[ID].birdsonport;
    }

    return(birds);
}

/******************************************************************************/

BOOL    FOB_MultiBirdPort( int ID )
{
    return(FOB_BirdsOnPort(ID) > 1);
}

/******************************************************************************/

BOOL    FOB_StandAlonePort( int ID )
{
    return(FOB_BirdsOnPort(ID) == 1);
}

/******************************************************************************/

int     FOB_BirdIndex( int ID, BYTE addr )
{
int     bird,find=FOB_BIRD_NULL;

    for( bird=0; (bird < FOB_BirdsOnPort(ID)); bird++ )
    {
        if( FOB_Port[ID].address[bird] == addr )
        {
            find = bird;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

BOOL    FOB_BirdStatusOK( int ID )
{
BOOL    ok,err;
int     bird;
WORD    status;

    // Multiple ports, so re-enter function for each port...
    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,ID=0; (ID < FOB_PortsInFlock()); ID++ )
        {
            if( !FOB_BirdStatusOK(ID) )
            {
                ok = FALSE;
            }
        }

        return(ok);
    }

    for( err=FALSE,ok=TRUE,bird=0; ((bird < FOB_BirdsOnPort(ID)) && ok); bird++ )
    {
        if( !(ok=FOB_GetBirdStatus(ID,bird,status)) )
        {
            FOB_errorf("FOB_GetBirdStatus(ID=%d) bird=%d Failed.\n",ID,bird);
            continue;
        }

        if( status & FOB_BIRDSTATUS_ERROR )
        {
            err = TRUE;
        }
        else
        if( status & FOB_BIRDSTATUS_MASTER )
        {
            err = (bird > 1);
        }

        STR_printf(err,FOB_errorf,FOB_debugf,"COM%d[ID=%d] bird=%d %s %s\n",
                   FOB_Port[ID].comX,ID,bird,
                   STR_TextFlag(FOB_BirdStatusFlag,status),
                   STR_TextCode(FOB_BirdStatusFormatText,status & FOB_BIRDSTATUS_FORMAT));
    }

    return(ok && !err);
}

/******************************************************************************/

BOOL    FOB_FlockStatusOK( int ID )
{
BOOL    ok,xmit;
BYTE    status[FOB_FBB_NORMAL];
int     addr,bird;

    if( !FOB_GetFlockStatus(ID,status) )
    {
        return(FALSE);
    }

    for( xmit=FALSE,ok=TRUE,bird=0; ((bird < FOB_BirdsInFlock()) && (bird < FOB_FBB_NORMAL) && ok); bird++ )
    {
        if( !(status[bird] & FOB_FLOCKSTATUS_USED) )
        {
            FOB_errorf("STATUS[0x%02X] Error: Not used.\n");
            ok = FALSE;
        }
        else
        if( !(status[bird] & FOB_FLOCKSTATUS_RUNNING) )
        {
            FOB_errorf("STATUS[0x%02X] Error: Not Running.\n");
            ok = FALSE;
        }
        else
        if( !(status[bird] & FOB_FLOCKSTATUS_SENSOR) )
        {
            FOB_errorf("STATUS[0x%02X] Error: No sensor.\n");
            ok = FALSE;
        }

        if( (status[bird] & FOB_FLOCKSTATUS_TRANSMIT) )
        {
            xmit = TRUE;
        }
    }

    if( ok && !xmit )
    {
        FOB_errorf("STATUS[XX] Error: No transmitter.\n");
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

WORD    FOB_PosnScale( int ID )
{
WORD    scale=0;

    if( FOB_Check(ID) )
    {
        scale = FOB_Port[ID].scale;
    }

    return(scale);
}

/******************************************************************************/

BOOL    FOB_DataMode( int ID, BYTE mode, BOOL flag )
{
BOOL    ok;

    switch( mode )
    {
        case FOB_DATAMODE_POINT :
           ok = TRUE;
           break;

        case FOB_DATAMODE_STREAM :
           ok = FOB_CmdStream(ID);
           break;

        case FOB_DATAMODE_DRC :
        default :
           ok = FALSE;
           break;
    }

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_DataMode(ID=%d) %s [%s %s].\n",
               ID,
               STR_OkFailed(ok),
               STR_TextCode(FOB_DataModeText,FOB_datamode),
               STR_OnOff(flag));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Flying( int ID )
{
BOOL    ok=TRUE,flying=FALSE;
DWORD   stat;

//  Multiple ports, so re-enter function for each port...
    if( ID == FOB_PORT_ALL )
    {
        for( flying=TRUE,ID=0; (ID < FOB_PortsInFlock()); ID++ )
        {
            if( !FOB_Flying(ID) )
            {
                flying = FALSE;
            }
        }

        return(flying);
    }

    if( !FOB_Check(ID) )
    {
        return(FALSE);
    }

//  Check RS232 control line to see if flock flying...
    if( !COM_ModemStatus(FOB_Port[ID].comH,&stat) )
    {
        FOB_errorf("FOB_Flying(ID=%d) COM%d Status Error.\n",ID,FOB_Port[ID].comX);
        ok = FALSE;
        return(FALSE);
    }
    else
    {
        flying = (stat & MS_DSR_ON);   // DSR line should be ON if flock flying...
    }

    return(ok && flying);
}

/******************************************************************************/

BOOL    FOB_Start( int ID )
{
BOOL    ok=FALSE;
int     skip,bird;

    // Multiple ports, so re-enter function for each port...
    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            ok = FOB_Start(ID);
        }

        if( !ok )
        {
            FOB_Stop(FOB_PORT_ALL);
        }

        return(ok);
    }

    if( !FOB_Check(ID) )
    {
        FOB_errorf("FOB_Start(ID=%d) Invalid port ID.\n",ID);
        return(FALSE);
    }

    if( !FOB_Flying(ID) )
    {
        FOB_errorf("FOB_Start(ID=%d) Bird(s) not flying.\n",ID);
        return(FALSE);
    }

    // Reset COM buffers...
    FOB_ResetCOM(ID);

    // Start LOOPTASK to handle streaming data...
    if( FOB_datamode == FOB_DATAMODE_STREAM )
    {
        if( !LOOPTASK_running(FOB_LoopTask) )
        {
            if( !LOOPTASK_start(FOB_LoopTask) )
            {
                FOB_errorf("FOB_Start(ID=%d) Cannot start LOOPTASK.\n",ID);
                return(FALSE);
            }
        }
    }

    // Turn on selected data mode ...
    if( !FOB_DataMode(ID,FOB_datamode,ON) )
    {
        FOB_errorf("FOB_Start(ID=%d) Data mode.\n",ID);
        return(FALSE);
    }

    // Set various session flags and other variables...
    FOB_Port[ID].started = TRUE;
    FOB_Port[ID].stage = FOB_STAGE_START;
    FOB_Port[ID].last = FOB_STAGE_START;
    FOB_Port[ID].overwrite = 0;
    FOB_Port[ID].got = 0;
    FOB_Port[ID].framedata = FALSE;
    FOB_Port[ID].datasync = TRUE;
    FOB_Port[ID].error = FALSE;

    // Reset session objects...
    FOB_Port[ID].session->Reset();
    FOB_Port[ID].requestrate->Reset();
    FOB_Port[ID].looprate->Reset();
    FOB_Port[ID].framerate->Reset();
    FOB_Port[ID].framewait->Reset();

    // Initialize bird-specific variables for port...
    for( FOB_Port[ID].size=0,bird=0; (bird < FOB_BirdsOnPort(ID)); bird++ )
    {
        // Calculate offsets and size of frame based on selected data format...
        FOB_Port[ID].data[bird] = FOB_Port[ID].size;
        FOB_Port[ID].size += FOB_FrameSize[FOB_Port[ID].dataformat[bird]];

        if( FOB_MultiBirdPort(ID) && FOB_groupmode )
        {
            FOB_Port[ID].addr[bird] = FOB_Port[ID].size;   // Offset for this bird's frame.
            FOB_Port[ID].size += sizeof(BYTE);             // Include bird address byte.
        }
    }

    for( bird=0; (bird < FOB_BIRD_MAX); bird++ )
    {
        // Starting hemisphere for tracking...
        FOB_Port[ID].tracking[bird] = FOB_HEMISPHERE_INIT;
    }

    FOB_debugf("FOB_Start(ID=%d) birds=%d size=%d.\n",ID,FOB_BirdsOnPort(ID),FOB_Port[ID].size);

    return(TRUE);
}

/******************************************************************************/

void    FOB_Stop( int ID )
{
BOOL    ok;
double  session;

    // Multiple ports, so re-enter function for each port...
    if( ID == FOB_PORT_ALL )
    {
        for( ID=0; (ID < FOB_PortsInFlock()); ID++ )
        {
            FOB_Stop(ID);
        }

        return;
    }

    // Make sure we're started...
    if( !FOB_Started(ID,FALSE) )
    {
        return;
    }

    // Calculate various session values...
    session = FOB_Port[ID].session->Elapsed();
    FOB_Port[ID].requestrate->Calculate();
    FOB_Port[ID].looprate->Calculate();
    FOB_Port[ID].framerate->Calculate();
    FOB_Port[ID].frametime->Calculate();

    // Stop LOOPTASK if it's running...
    LOOPTASK_stop(FOB_LoopTask);

    // Turn off selected data mode ...
    if( !FOB_DataMode(ID,FOB_datamode,OFF) )
    {
        FOB_errorf("FOB_Stop(ID=%d) Data mode.\n",ID);
    }

    // Clear started flag...
    FOB_Port[ID].started = FALSE;

    // Print statistics for FOB session...
    FOB_debugf("FOB_Stop(ID=%d) %.1lf msec, %ld frames, %ld sync, %ld overwrite (errors %s).\n",ID,session,FOB_Port[ID].frame,FOB_Port[ID].sync,FOB_Port[ID].overwrite,STR_YesNo(FOB_Port[ID].error));
    FOB_Port[ID].requestrate->Results(FOB_debugf);
    FOB_Port[ID].looprate->Results(FOB_debugf);
    FOB_Port[ID].framerate->Results(FOB_debugf);
    FOB_Port[ID].frametime->Results(FOB_debugf);
    FOB_Port[ID].framewait->Results(FOB_debugf);
}

/******************************************************************************/

BOOL    FOB_GetFirstByte( int ID, BYTE *buff, BOOL wait, BOOL &framed )
{
TIMER  *timeout;
BOOL    ok,done,TO=TRUE;

    timeout = new TIMER("FOB_GetFirstByte()");
    framed = FALSE;

    do
    {
        if( !(ok=COM_Read(FOB_Port[ID].comH,&buff[0],1,done,0L)) )
        {
            FOB_errorf("FOB_GetFirstByte(ID=%d) COM%d error.\n",ID,FOB_Port[ID].comX);
            continue;
        }

        if( !done )
        {
            if( wait )
            {
                if( (TO=timeout->Expired(10000.0/*FOB_TIMEOUT_FIRST*/)) )
                {
                    FOB_errorf("FOB_GetFirstByte(ID=%d) Timeout.\n",ID);
                }
            }

            continue;
        }

        if( (framed=FOB_FrameByte(buff[0])) )
        {
            FOB_Port[ID].datasync = TRUE;
            continue;
        }

        if( FOB_Port[ID].datasync )
        {
            FOB_errorf("FOB_GetFirstByte(ID=%d) Sync error [0x%02X].\n",ID,buff[0]);
            FOB_Port[ID].datasync = FALSE;
        }
    }
    while( ok && !framed && !TO );

    delete timeout;

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetFirstByte( int ID, BYTE *buff, BOOL &framed )
{
BOOL    ok;

    ok = FOB_GetFirstByte(ID,buff,FALSE,framed);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetFirstByte( int ID, BYTE *buff )
{
BOOL    ok,framed;

    ok = FOB_GetFirstByte(ID,buff,TRUE,framed);

    return(ok && framed);
}

/******************************************************************************/

BOOL    FOB_GetTheRest( int ID, BYTE *buff, int size, long wait )
{
BOOL    ok;

    ok = COM_Read(FOB_Port[ID].comH,&buff[1],size-1,wait);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetTheRest( int ID, BYTE *buff, int size )
{
BOOL    ok;

    ok = COM_Read(FOB_Port[ID].comH,&buff[1],size-1,10000.0/*FOB_TIMEOUT_REST*/);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetPoint( int ID, int bird, WORD buff[], int size )
{
BOOL    framed;
BYTE   *bptr=(BYTE *)buff;

//  Request point from flock...
    if( !FOB_CmdPoint(ID,bird) )
    {
        FOB_errorf("FOB_GetPoint(ID=%d,bird=%d,size=%d) Cannot request point.\n",ID,bird,size);
        return(FALSE);
    }

//  Get the first byte of the data, checking framing, etc...
    if( !FOB_GetFirstByte(ID,bptr,TRUE,framed) )
    {
        FOB_errorf("FOB_GetPoint(ID=%d,bird=%d,size=%d) Frame byte error.\n",ID,bird,size);
        return(FALSE);
    }

//  No fresh data is an error because we requested a point...
    if( !framed )
    {
        FOB_errorf("FOB_GetPoint(ID=%d,bird=%d,size=%d) Frame expected.\n",ID,bird,size);
        return(FALSE);
    }

//  Get remainder of frame...
    if( !FOB_GetTheRest(ID,bptr,size) )
    {
        FOB_errorf("FOB_GetPoint(ID=%d,bird=%d,size=%d) Frame data error.\n",ID,bird,size);
        return(FALSE);
    }        

    return(TRUE);
}

/******************************************************************************/

BOOL    FOB_GetPoint( int ID, WORD buff[], int size )
{
BOOL    ok;

    ok = FOB_GetPoint(ID,FOB_BIRD_NULL,buff,size);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetStream( int ID, WORD buff[], int size, BOOL &framed )
{
BOOL    ok=FALSE;
BYTE   *bptr=(BYTE *)buff;

    // Get the first byte of the data, checking framing...
    if( !(ok=FOB_GetFirstByte(ID,bptr,framed)) )
    {
        FOB_errorf("FOB_GetStream(ID=%d) Data frame error.\n",ID);
    }
    else      // Get the rest of the data frame...
    if( framed )
    {
        ok = FOB_GetTheRest(ID,bptr,size);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetNonGroup( int ID, FOB_RawFrame *data, BOOL &fresh )
{
BOOL    ok;
int     bird,word,size;
WORD    buff[FOB_FRAME_WORD];

    // Non-group mode, so deal with each bird separately...
    for( ok=TRUE,bird=0; ((bird < FOB_BirdsOnPort(ID)) && ok); bird++ )
    {
        size = FOB_FrameSize[FOB_Port[ID].dataformat[bird]];

        // Get the data frame...
        if( !FOB_GetPoint(ID,bird,buff,size) )
        {
            FOB_errorf("FOB_GetNonGroup(ID=%d) Frame failed.\n",ID);
            ok = FALSE;
            continue;
        }

        // Put the raw data for this bird into its frame...
        if( (word=FOB_Data2Frame(ID,bird,buff,data)) == 0 )
        {
            FOB_errorf("FOB_GetNonGroup(ID=%d) Data could not be framed.\n",ID);
            ok = FALSE;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetGroup( int ID, FOB_RawFrame *data, BOOL &fresh )
{
BOOL    ok=FALSE;
int     bird,indx,word;
short   timeout;
WORD    buff[FOB_FRAME_WORD];
BYTE   *bptr=(BYTE *)buff;

    // Get the data frame...
    switch( FOB_datamode )
    {
        case FOB_DATAMODE_POINT :
           ok = FOB_GetPoint(ID,buff,FOB_Port[ID].size);
           fresh = TRUE;
           break;

        case FOB_DATAMODE_STREAM :
           ok = FOB_GetStream(ID,buff,FOB_Port[ID].size,fresh);
           break;

        case FOB_DATAMODE_DRC :
        default :
           break;
    }

    if( !ok )
    {
        FOB_errorf("FOB_GetGroup(ID=%d) (%s) Failed.\n",ID,STR_TextCode(FOB_DataModeText,FOB_datamode));
        return(FALSE);
    }

    if( !fresh )
    {
        return(ok);
    }

    // Cut the data up into frames for each bird...
    for( ok=TRUE,bird=0; ((bird < FOB_BirdsOnPort(ID)) && ok); bird++ )
    {
        indx = FOB_MultiBirdPort(ID) ? FOB_BirdIndex(ID,bptr[FOB_Port[ID].addr[bird]]) : bird;

        if( indx == FOB_BIRD_NULL )
        {
            FOB_errorf("FOB_GetGroup(ID=%d) Invalid bird index.\n");
            ok = FALSE;
        }
        else
        if( (word=FOB_Data2Frame(ID,indx,(WORD *)&bptr[FOB_Port[ID].data[bird]],data)) == 0 )
        {
            FOB_errorf("FOB_GetGroup(ID=%d) Data could not be framed.\n",ID);
            ok = FALSE;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetFresh( int ID, FOB_RawFrame *data, BOOL &fresh )
{
BOOL    ok;

    if( FOB_groupmode || FOB_StandAlonePort(ID) )
    {
        ok = FOB_GetGroup(ID,data,fresh);
    }
    else
    {
        ok = FOB_GetNonGroup(ID,data,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetFrame( int ID, FOB_RawFrame *data )
{
BOOL    ok,fresh=FALSE;

    // Birds must be flying to get frame...
    if( !FOB_Flying(ID) )
    {
        FOB_errorf("FOB_GetFrame(ID=%d) Birds not flying.\n",ID);
        return(FALSE);
    }

    // Get fresh frame(s) if ready...
    FOB_Port[ID].frametime->Before();
    ok = FOB_GetFresh(ID,data,fresh);
    FOB_Port[ID].frametime->After();

    //  Didn't get frame (fresh or old) for some reason...
    if( !ok )
    {
        FOB_Port[ID].error = TRUE;
        return(FALSE);
    }

    // Frame number, freshness, timing, etc...
    FOB_Port[ID].frame++;
    FOB_Port[ID].requestrate->Loop();

    data->fresh = fresh;
    data->frame = FOB_Port[ID].frame;

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Started( int ID, BOOL verbose )
{
BOOL    ok=TRUE;

    // Multiple ports, so re-enter function for each port...
    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,ID=0; (ID < FOB_PortsInFlock()); ID++ )
        {
            if( !FOB_Started(ID,verbose) )
            {
                ok = FALSE;
            }
        }

        return(ok);
    }

    if( !FOB_Check(ID) )
    {
        ok = FALSE;

        if( verbose )
        {
            FOB_errorf("FOB_Started(ID=%d) Invalid FOB handle.\n",ID);
        }
    }
    else
    if( !FOB_Port[ID].started )
    {
        ok = FALSE;

        if( verbose )
        {
            FOB_errorf("FOB_Started(ID=%d) Data collection not started.\n",ID);
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Started( int ID )
{
BOOL    ok;

    ok = FOB_Started(ID,TRUE);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Started( void )
{
BOOL    ok;

    ok = FOB_Started(FOB_PORT_ALL,FALSE);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetPosn( int ID, matrix pomx[], BOOL &fresh )
{
BOOL    ok;

    if( !FOB_Started(ID) )
    {
        return(FALSE);
    }

    if( !FOB_DataTypePosition(ID) )
    {
        FOB_errorf("FOB_GetPosn(ID=%d) Position data not selected.\n",ID);
        return(FALSE);
    }

    ok = FOB_GetFrame(ID,&FOB_RawData[ID]);

    if( ok )
    {
        fresh = FOB_RawData[ID].fresh;
        FOB_PutPOMX(ID,FOB_RawData[ID].reading,pomx);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetPosn( int ID, matrix pomx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetPosn(ID,&pomx[bird],fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetPosn(ID,pomx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetAngles( int ID, matrix eamx[], BOOL &fresh )
{
BOOL    ok;

    if( !FOB_Started(ID) )
    {
        return(FALSE);
    }

    if( !FOB_DataTypeAngles(ID) )
    {
        FOB_errorf("FOB_GetAngles(ID=%d) Angles data not selected.\n",ID);
        return(FALSE);
    }

    ok = FOB_GetFrame(ID,&FOB_RawData[ID]);

    if( ok )
    {
        fresh = FOB_RawData[ID].fresh;
        FOB_PutEAMX(ID,FOB_RawData[ID].reading,eamx);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetAngles( int ID, matrix eamx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetAngles(ID,&eamx[bird],fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetAngles(ID,eamx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetPosnAngles( int ID, matrix pomx[], matrix eamx[], BOOL &fresh )
{
BOOL    ok;

    if( !FOB_Started(ID) )
    {
        return(FALSE);
    }

    if( !FOB_DataTypePosition(ID) )
    {
        FOB_errorf("FOB_GetPosnAngles(ID=%d) Position data not selected.\n",ID);
        return(FALSE);
    }

    if( !FOB_DataTypeAngles(ID) )
    {
        FOB_errorf("FOB_GetPosnAngles(ID=%d) Angles data not selected.\n",ID);
        return(FALSE);
    }

    ok = FOB_GetFrame(ID,&FOB_RawData[ID]);

    if( ok )
    {
        fresh = FOB_RawData[ID].fresh;
        FOB_PutPOMX(ID,FOB_RawData[ID].reading,pomx);
        FOB_PutEAMX(ID,FOB_RawData[ID].reading,eamx);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetPosnAngles( int ID, matrix pomx[], matrix eamx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetPosnAngles(ID,&pomx[bird],&eamx[bird],fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetPosnAngles(ID,pomx,eamx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetROMX( int ID, matrix romx[], BOOL &fresh )
{
BOOL    ok;

    if( !FOB_Started(ID) )
    {
        return(FALSE);
    }

    if( !FOB_DataTypeMatrix(ID) )
    {
        FOB_errorf("FOB_GetROMX(ID=%d) Matrix data not selected.\n",ID);
        return(FALSE);
    }

    ok = FOB_GetFrame(ID,&FOB_RawData[ID]);

    if( ok )
    {
        fresh = FOB_RawData[ID].fresh;
        FOB_PutROMX(ID,FOB_RawData[ID].reading,romx);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetROMX( int ID, matrix romx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetROMX(ID,&romx[bird],fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetROMX(ID,romx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetPosnROMX( int ID, matrix pomx[], matrix romx[], BOOL &fresh )
{
BOOL    ok=TRUE;

    if( !FOB_Started(ID) )
    {
        return(FALSE);
    }

    if( !FOB_DataTypePosition(ID) )
    {
        FOB_errorf("FOB_GetPosnROMX(ID=%d) Position data not selected.\n",ID);
        return(FALSE);
    }

    if( !FOB_DataTypeMatrix(ID) )
    {
        FOB_errorf("FOB_GetPosnROMX(ID=%d) Matrix data not selected.\n",ID);
        return(FALSE);
    }

    ok = FOB_GetFrame(ID,&FOB_RawData[ID]);

    if( ok )
    {
        fresh = FOB_RawData[ID].fresh;
        FOB_PutPOMX(ID,FOB_RawData[ID].reading,pomx);
        FOB_PutROMX(ID,FOB_RawData[ID].reading,romx);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetPosnROMX( int ID, matrix pomx[], matrix romx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetPosnROMX(ID,&pomx[bird],&romx[bird],fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetPosnROMX(ID,pomx,romx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetRTMX( int ID, matrix pomx[], matrix romx[], matrix rtmx[], BOOL &fresh )
{
BOOL    ok;

    if( !FOB_Started(ID) )
    {
        return(FALSE);
    }

    if( !FOB_DataTypePosition(ID) )
    {
        FOB_errorf("FOB_GetRTMX(ID=%d) Position data not selected.\n",ID);
        return(FALSE);
    }

    if( !FOB_DataTypeMatrix(ID) )
    {
        FOB_errorf("FOB_GetRTMX(ID=%d) Matrix data not selected.\n",ID);
        return(FALSE);
    }

    ok = FOB_GetFrame(ID,&FOB_RawData[ID]);

    if( ok )
    {
        fresh = FOB_RawData[ID].fresh;
        if( pomx != NULL ) FOB_PutPOMX(ID,FOB_RawData[ID].reading,pomx);
        if( romx != NULL ) FOB_PutROMX(ID,FOB_RawData[ID].reading,romx);
        if( rtmx != NULL ) FOB_PutRTMX(ID,FOB_RawData[ID].reading,rtmx);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetRTMX( int ID, matrix pomx[], matrix romx[], matrix rtmx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetRTMX(ID,&pomx[bird],&romx[bird],&rtmx[bird],fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetRTMX(ID,pomx,romx,rtmx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetRTMX( int ID, matrix rtmx[], BOOL &fresh )
{
BOOL    ok;

    ok = FOB_GetRTMX(ID,NULL,NULL,rtmx,fresh);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetRTMX( int ID, matrix rtmx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetRTMX(ID,NULL,NULL,&rtmx[bird],fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetRTMX(ID,NULL,NULL,rtmx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetQTMX( int ID, matrix qtmx[], BOOL &fresh )
{
BOOL    ok;

    if( !FOB_Started(ID) )
    {
        return(FALSE);
    }

    if( !FOB_DataTypeQuaternion(ID) )
    {
        FOB_errorf("FOB_GetQTMX(ID=%d) Quaternion data not selected.\n",ID);
        return(FALSE);
    }

    ok = FOB_GetFrame(ID,&FOB_RawData[ID]);

    if( ok )
    {
        fresh = FOB_RawData[ID].fresh;
        FOB_PutQTMX(ID,FOB_RawData[ID].reading,qtmx);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetQTMX( int ID, matrix qtmx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetQTMX(ID,&qtmx[bird],fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetQTMX(ID,qtmx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetPosnQTMX( int ID, matrix pomx[], matrix qtmx[], BOOL &fresh )
{
BOOL    ok;

    if( !FOB_Started(ID) )
    {
        return(FALSE);
    }

    if( !FOB_DataTypePosition(ID) )
    {
        FOB_errorf("FOB_GetPosnQTMX(ID=%d) Position data not selected.\n",ID);
        return(FALSE);
    }

    if( !FOB_DataTypeQuaternion(ID) )
    {
        FOB_errorf("FOB_GetPosnQTMX(ID=%d) Quaternion data not selected.\n",ID);
        return(FALSE);
    }

    ok = FOB_GetFrame(ID,&FOB_RawData[ID]);

    if( ok )
    {
        fresh = FOB_RawData[ID].fresh;
        FOB_PutPOMX(ID,FOB_RawData[ID].reading,pomx);
        FOB_PutQTMX(ID,FOB_RawData[ID].reading,qtmx);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetPosnQTMX( int ID, matrix pomx[], matrix qtmx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetPosnQTMX(ID,&pomx[bird],&qtmx[bird],fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetPosnQTMX(ID,pomx,qtmx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetData( int ID, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[], BOOL &fresh )
{
BOOL    ok=FALSE;
int     bird;

    if( !FOB_Started(ID) )
    {
        return(FALSE);
    }

    ok = FOB_GetFrame(ID,&FOB_RawData[ID]);

    if( ok )
    {
        fresh = FOB_RawData[ID].fresh;
        FOB_PutSPMX(ID,FOB_RawData[ID].reading,pomx,eamx,romx,qtmx,rtmx);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetData( int ID, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], BOOL &fresh )
{
BOOL    ok;

    ok = FOB_GetData(ID,pomx,eamx,romx,qtmx,NULL,fresh);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetData( int ID, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[] )
{
BOOL    ok,fresh;
int     bird;

    if( ID == FOB_PORT_ALL )
    {
        for( ok=TRUE,bird=0,ID=0; ((ID < FOB_PortsInFlock()) && ok); ID++ )
        {
            if( !(ok=FOB_GetData(ID,M(pomx,bird),M(eamx,bird),M(romx,bird),M(qtmx,bird),M(rtmx,bird),fresh)) )
            {
                continue;
            }

            bird += FOB_BirdsOnPort(ID);
        }
    }
    else
    {
        ok = FOB_GetData(ID,pomx,eamx,romx,qtmx,rtmx,fresh);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetData( int ID, matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[] )
{
BOOL    ok;
BOOL    fresh;

    ok = FOB_GetData(ID,pomx,eamx,romx,qtmx,fresh);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Master( int ID )
{
BOOL    master=FALSE;

    if( FOB_Check(ID) )
    {
        master = (FOB_Port[ID].birdsinflock > 0);
    }

    return(master);
}

/******************************************************************************/

int     FOB_Master( void )
{
int     ID,master;

    for( master=FOB_PORT_INVALID,ID=0; (ID < FOB_PORT_MAX); ID++ )
    {
        if( FOB_Master(ID) )
        {
            master = ID;
            break;
        }
    }

    return(master);
}

/******************************************************************************/

BYTE    FOB_DataFormat( int ID, int bird )
{
BYTE    dataformat=FOB_NULL_BYTE;

    if( FOB_Check(ID) )
    {
        dataformat = FOB_Port[ID].dataformat[bird];
    }

    return(dataformat);
}

/******************************************************************************/

BYTE    FOB_AngleUnit( void )
{
    return(FOB_angleunit);
}

/******************************************************************************/

