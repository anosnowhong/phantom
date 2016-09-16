/******************************************************************************/
/*                                                                            */
/* MODULE  : NI-DAQ.cpp                                                       */
/*                                                                            */
/* PURPOSE : National Instruments - Data AQuisition API                       */
/*                                                                            */
/* DATE    : 18/Sep/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 23/Aug/2001 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "NI-DAQ"
#define MODULE_TEXT     "NI-DAQ API"
#define MODULE_DATE     "23/08/2001"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>                               // Includes (almost) everything we need.

/******************************************************************************/

BOOL    NIDAQ_API_started = FALSE;

/******************************************************************************/

PRINTF  NIDAQ_PRN_messgf=NULL;                   // General messages printf function.
PRINTF  NIDAQ_PRN_errorf=NULL;                   // Error messages printf function.
PRINTF  NIDAQ_PRN_debugf=NULL;                   // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     NIDAQ_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(NIDAQ_PRN_messgf,buff));
}

/******************************************************************************/

int     NIDAQ_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(NIDAQ_PRN_errorf,buff));
}

/******************************************************************************/

int     NIDAQ_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(NIDAQ_PRN_debugf,buff));
}

/******************************************************************************/

int     NIDAQ_HW_Devices=0;
int     NIDAQ_HW_List[] = { NIDAQ_HW_DAQPAD6020EUSB,NIDAQ_HW_PCI6035E,NIDAQ_HW_INVALID };
void   *NIDAQ_AI_Buffer=NULL;
int     NIDAQ_Device=NIDAQ_HW_INVALID;

struct  NIDAQ_HW_DeviceInformation     NIDAQ_HW_Device[NIDAQ_DEVICE_MAX+1];
struct  NIDAQ_AI_SessionInformation    NIDAQ_AI_Session[NIDAQ_DEVICE_MAX+1];

int     NIDAQ_DO_Trigger[NIDAQ_DEVICE_MAX+1];

i16     NIDAQ_AI_i16[NIDAQ_CHANNEL_MAX];
f64     NIDAQ_AI_f64[NIDAQ_CHANNEL_MAX];

/******************************************************************************/

#include "ni-daq-status.cpp"           // NI-DAQ status (error) descriptions.
#include "ni-daq-devices.cpp"          // NI-DAQ device names.

/******************************************************************************/

struct  STR_TextItem    NIDAQ_PolarityText[3] = 
{
    { NIDAQ_POLARITY_BI,"BI" },
    { NIDAQ_POLARITY_UNI,"UNI" },
    { STR_TEXT_ENDOFTABLE }
};

/******************************************************************************/

struct  STR_TextItem    NIDAQ_InputModeText[4] = 
{
    { NIDAQ_INPUTMODE_DIFF,"DIFF" },
    { NIDAQ_INPUTMODE_RSE,"RSE" },
    { NIDAQ_INPUTMODE_NRSE,"NRSE" },
    { STR_TEXT_ENDOFTABLE }
};

/******************************************************************************/
/* General API Functions...                                                   */
/******************************************************************************/

BOOL NIDAQ_Error( i16 status, char *func )
{
BOOL    ok=TRUE,done=TRUE;
char   *type;
int     code;

    if( status != noError )
    {
        type = (status < 0) ? "Failed" : "Execute with Error";
        code = -abs(status);

        NIDAQ_errorf("%s(...) %s %s(%d).\n",func,type,NIDAQ_StatusDescription(code),status);
        ok = FALSE;
    }

    return(!ok);
}

/******************************************************************************/

BOOL    NIDAQ_HW_Supported( int code )
{
int     HW;
BOOL    ok=FALSE;

    for( ok=FALSE,HW=0; (NIDAQ_HW_List[HW] != NIDAQ_HW_INVALID); HW++ )
    {
        if( NIDAQ_HW_List[HW] == code )
        {
            ok = TRUE;
            break;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_Devices( void )
{
i16     status;
i16     deviceNumber,deviceNumberCode;
int     device,devices;
BOOL    ok,more;
u32     infoType;
u32     infoValue;

    // Clear device list...
    for( device=1; (device <= NIDAQ_DEVICE_MAX); device++ )
    {
        memset(&NIDAQ_HW_Device[device],0x00,sizeof(struct NIDAQ_HW_DeviceInformation));
        NIDAQ_HW_Device[device].device = NIDAQ_HW_INVALID;
        NIDAQ_HW_Device[device].supported = FALSE;
    }

    // Check a range of NI-DAQ device numbers for valid devices...
    for( ok=TRUE,more=TRUE,devices=0,device=1; ((device <= NIDAQ_DEVICE_MAX) && ok && more); device++ )
    {
        deviceNumber = (i16)device;
        infoType = ND_DEVICE_TYPE_CODE;

        // Try to get device information for this NI-DAQ device number...
        status = Get_DAQ_Device_Info(deviceNumber,ND_DEVICE_TYPE_CODE,&infoValue);

        // Trap "Unknown Device" status...
        if( status == unknownDeviceError )
        {
            more = FALSE;
            continue;
        }

        // Check return status for error...
        if( NIDAQ_Error(status,"Get_DAQ_Device") )
        {
            ok = FALSE;
            continue;
        }

        deviceNumberCode = (i16)infoValue;

        // Initialize device...
        status = Init_DA_Brds(deviceNumber,&deviceNumberCode);
        if( NIDAQ_Error(status,"Init_DA_Brds") )
        {
            ok = FALSE;
            continue;
        }

        // Save device information in our list...
        strncpy(NIDAQ_HW_Device[device].description,NIDAQ_DeviceDescription((int)deviceNumberCode),STRLEN);
        NIDAQ_HW_Device[device].device = (int)deviceNumberCode;
        NIDAQ_HW_Device[device].supported = NIDAQ_HW_Supported((int)deviceNumberCode);
        devices++;
    }

    if( !ok )
    {
        NIDAQ_errorf("NIDAQ_Devices() Error finding NI-DAQ devices.\n");
        return(FALSE);
    }

    // No NI-DAQ devices so no point starting API...
    if( devices == 0 )
    {
        NIDAQ_errorf("NIDAQ_Devices() No NI-DAQ devices found.\n");
        return(FALSE);
    }

    // List devices...
    for( device=1; (device <= devices); device++ )
    {
        NIDAQ_messgf("%d: %s Supported %s.\n",device,NIDAQ_HW_Device[device].description,STR_YesNo(NIDAQ_HW_Device[device].supported));
    }

    NIDAQ_HW_Devices = devices;
    NIDAQ_messgf("NIDAQ_Devices() %d device(s) found.\n",NIDAQ_HW_Devices);

    return(ok);
}

/******************************************************************************/

int     NIDAQ_SetDevice( char *description )
{
int     device;
BOOL    done;

    // Make sure API is running...
    if( !NIDAQ_API_check() )
    {
        return(NIDAQ_HW_INVALID);
    }

    NIDAQ_Device = NIDAQ_HW_INVALID;

    // Find NI-DAQ description in device list..
    for( done=FALSE,device=1; ((NIDAQ_HW_Device[device].device != NIDAQ_HW_INVALID) && (device <= NIDAQ_DEVICE_MAX)); device++ )
    {
        if( !NIDAQ_HW_Device[device].supported )
        {
            continue;
        }

        if( description == NULL )
        {
            done = TRUE;
        }
        else
        if( _stricmp(description,NIDAQ_HW_Device[device].description) == 0 )
        {
            done = TRUE;
        }

        if( done )
        {
            NIDAQ_Device = device;
            break;
        }
    }

    return(NIDAQ_Device);
}

/******************************************************************************/

int     NIDAQ_SetDevice( int code )
{
int     device;
BOOL    done;

    // Make sure API is running...
    if( !NIDAQ_API_check() )
    {
        return(NIDAQ_HW_INVALID);
    }

    NIDAQ_Device = NIDAQ_HW_INVALID;

    // Find NI-DAQ hardware code in device list...
    for( done=FALSE,device=1; ((NIDAQ_HW_Device[device].device != NIDAQ_HW_INVALID) && (device <= NIDAQ_DEVICE_MAX)); device++ )
    {
        if( !NIDAQ_HW_Device[device].supported )
        {
            continue;
        }

        if( code == NIDAQ_HW_WHATEVER )
        {
            done = TRUE;
        }
        else
        if( NIDAQ_HW_Device[device].device == code )
        {
            done = TRUE;
        }

        if( done )
        {
            NIDAQ_Device = device;
            break;
        }
    }

    return(NIDAQ_Device);
}

/******************************************************************************/

int     NIDAQ_SetDevice( void )
{
int     code=NIDAQ_HW_WHATEVER;
int     device;

    device = NIDAQ_SetDevice(code);

    return(device);
}

/******************************************************************************/

int NIDAQ_DeviceCode( int device )
{
int code=NIDAQ_HW_INVALID;

    if( (device >= 1) && (device <= NIDAQ_DEVICE_MAX) )
    {
        code = NIDAQ_HW_Device[device].device;
    }

    return(code);
}

/******************************************************************************/

char *NIDAQ_DeviceName( int device )
{
char *name=NULL;
int code;

    if( (code=NIDAQ_DeviceCode(device)) != NIDAQ_HW_INVALID )
    {
        name = NIDAQ_DeviceDescription(code);
    }

    return(name);
}

/******************************************************************************/

BOOL NIDAQ_Installed( int &device )
{
BOOL ok=TRUE;

    if( (device=NIDAQ_SetDevice()) == NIDAQ_HW_INVALID )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL NIDAQ_Installed( void )
{
BOOL ok;
int device;

    ok = NIDAQ_Installed(device);

    return(ok);
}

/******************************************************************************/

void    NIDAQ_API_stop( void )
{
    if( !NIDAQ_API_started  )          // API not started in the first place...
    {
         return;
    }

    NIDAQ_API_started = FALSE;         // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    NIDAQ_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
int     dev;
BOOL    ok;

    if( NIDAQ_API_started )                 // Start the API once...
    {
        return(TRUE);
    }

    NIDAQ_PRN_messgf = messgf;              // General API message print function.
    NIDAQ_PRN_errorf = errorf;              // API error message print function.
    NIDAQ_PRN_debugf = debugf;              // Debug information print function.

    if( !NIDAQ_Devices() )                  // Initialize NI-DAQ devices...
    {
        NIDAQ_errorf("NIDAQ_API_start(...) Failed.\n");
        return(FALSE);
    }

    for( dev=0; (dev <= NIDAQ_DEVICE_MAX); dev++ )
    {
        memset(&NIDAQ_AI_Session[dev],0x00,sizeof(struct NIDAQ_AI_SessionInformation));
        NIDAQ_AI_Session[dev].running = FALSE;
        NIDAQ_AI_Session[dev].complete = FALSE;

        NIDAQ_DO_Trigger[dev] = NIDAQ_CHANNEL_NULL;
    }

    ATEXIT_API(NIDAQ_API_stop);             // Install stop function.
    NIDAQ_API_started = TRUE;               // Set started flag.
    MODULE_start(NIDAQ_PRN_messgf);         // Register module.

    // Select default NI-DAQ device...
    ok = NIDAQ_SetDevice();

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_API_check( void )
{
BOOL    ok=TRUE;

    if( NIDAQ_API_started )            // API started...
    {                                  // Start module automatically...
        return(TRUE);
    }

//  Start API...
    ok = NIDAQ_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
    NIDAQ_debugf("NIDAQ_API_check() Start %s.\n",ok ? "OK" : "Failed");

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_Check( int dev )
{
BOOL    ok=TRUE;

    if( !NIDAQ_API_check() )
    {
        return(FALSE);
    }

    if( dev == NIDAQ_HW_INVALID )
    {
        ok = FALSE;
    }
    else
    if( (dev < 1) && (dev > NIDAQ_DEVICE_MAX) )
    {
        ok = FALSE;
    }

    if( !ok )
    {
        NIDAQ_debugf("NIDAQ_Check(dev=%d) Invalid device.\n",dev);
    }

    return(ok);
}

/******************************************************************************/
/* Analog Input Functions...                                                  */
/******************************************************************************/

void   *NIDAQ_AI_BufferAllocate( int channels, int frames )
{
void   *ptr;
int     items,size;
BOOL    ok;

    items = channels * frames;
    size = items * sizeof(i16);

    ok = ((ptr=malloc(size)) != NULL);

    STR_printf(ok,NIDAQ_messgf,NIDAQ_errorf,"NIDAQ_AI_BufferAllocate(channels=%d,frames=%d) bytes=%d %s.\n",channels,frames,size,STR_OkFailed(ok));

    return(ptr);
}

/******************************************************************************/

void    NIDAQ_AI_BufferFree( void **ptr )
{
    if( *ptr != NULL )
    {
        free(*ptr);
       *ptr = NULL;
    }
}

/******************************************************************************/

BOOL    NIDAQ_AI_Setup( int dev, int chan, int mode, int polar )
{
i16     status;
i16     deviceNumber;
i16     channel;
i16     inputMode;
i16     inputRange;
i16     polarity;
i16     driveAIS;
BOOL    ok;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    deviceNumber = (i16)dev;      // Device number.
    channel = (i16)chan;          // Which channel (-1 for all).
    inputMode = (i16)mode;        // Differential or singled-ended.
    inputRange = 0;               // Ignored on E series devices.
    polarity = (i16)polar;        // Unipolar or bipolar input.
    driveAIS = 0;                 // Ignored on newer devices.

    status = AI_Configure(deviceNumber,channel,inputMode,inputRange,polarity,driveAIS);
    ok = !NIDAQ_Error(status,"AI_Configure");

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Setup( int dev, int chan[], int mode[], int polar[] )
{
int     channel;
BOOL    ok;

    for( ok=TRUE,channel=0; ((chan[channel] != NIDAQ_CHANNEL_END) && (channel < NIDAQ_CHANNEL_MAX) && ok); channel++ )
    {
        ok = NIDAQ_AI_Setup(dev,chan[channel],mode[channel],polar[channel]);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Setup( int chan, int mode, int polar )
{
BOOL    ok;

    ok = NIDAQ_AI_Setup(NIDAQ_Device,chan,mode,polar);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Setup( int chan[], int mode[], int polar[] )
{
BOOL    ok;

    ok = NIDAQ_AI_Setup(NIDAQ_Device,chan,mode,polar);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_SetupAll( int dev, int mode, int polar )
{
BOOL    ok;

    ok = NIDAQ_AI_Setup(dev,NIDAQ_CHANNEL_ALL,mode,polar);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_SetupAll( int mode, int polar )
{
BOOL    ok;

    ok = NIDAQ_AI_Setup(NIDAQ_Device,NIDAQ_CHANNEL_ALL,mode,polar);

    return(ok);
}

/******************************************************************************/

i16     NIDAQ_AI_Gain( double d_gain )
{
i16     i_gain;

    if( d_gain == 0.5 )
    {
        i_gain = -1;
    }
    else
    {
        i_gain = (i16)d_gain;
    }

    return(i_gain);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Start( int dev, int chan[], double gain[], int frames, double sr, void *ptr )
{
i16     status;
i16     deviceNumber,numChans,chanVector[NIDAQ_CHANNEL_MAX],gainVector[NIDAQ_CHANNEL_MAX];
f64     rate;
u32     count;
i16     rateTimebase,sampTimebase,scanTimebase;
u16     rateInterval,sampInterval,scanInterval;
i16    *buffer;
int     channels;
BOOL    ok=FALSE;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    if( NIDAQ_AI_Session[dev].running )
    {
        NIDAQ_errorf("NIDAQ_AI_Start(dev=%d,...) Collection already started.\n",dev);
        return(FALSE);
    }

    memset(&NIDAQ_AI_Session,NUL,sizeof(struct NIDAQ_AI_SessionInformation));

    for( channels=0; ((chan[channels] != NIDAQ_CHANNEL_END) && (channels < NIDAQ_CHANNEL_MAX)); channels++ )
    {
        NIDAQ_AI_Session[dev].channel[channels].channel = chan[channels];
        NIDAQ_AI_Session[dev].channel[channels].gain = (gain != NULL) ? gain[channels] : 1.0;
        NIDAQ_AI_Session[dev].channel[channels].offset = 0.0;

        chanVector[channels] = (i16)NIDAQ_AI_Session[dev].channel[channels].channel;
        gainVector[channels] = NIDAQ_AI_Gain(NIDAQ_AI_Session[dev].channel[channels].gain);
   }

    if( channels == 0 )
    {
        NIDAQ_errorf("NIDAQ_AI_Start(dev=%d,...) No channels specified.\n",dev);
        return(FALSE);
    }

    NIDAQ_AI_Session[dev].complete = FALSE;
    NIDAQ_AI_Session[dev].channels = channels;
    NIDAQ_AI_Session[dev].frames = frames;
    NIDAQ_AI_Session[dev].sr = sr;
    NIDAQ_AI_Session[dev].duration = (double)frames * sr;
    NIDAQ_AI_Session[dev].buffered = (ptr != NULL);
    NIDAQ_AI_Session[dev].buffer = ptr;

    deviceNumber = (i16)dev;
    numChans = (i16)channels;
    buffer = (i16*)ptr;
    count = (u32)(channels * frames);
    rate = (f64)sr;

    status = DAQ_Rate(rate,NIDAQ_RATE_FREQUENCY,&rateTimebase,&rateInterval);

    if( NIDAQ_Error(status,"DAQ_Rate") )
    {
        return(FALSE);
    }

    NIDAQ_debugf("DAQ_Rate(rate=%.1f,timebase=%d,interval=%u)\n",sr,rateTimebase,rateInterval);

    // Sample time gives the interval between A/D conversions within a scan sequence...
    sampTimebase = rateTimebase;
    sampInterval = rateInterval / channels;

//  Scan time gives the interval between scan sequences...
    scanTimebase = rateTimebase;
    scanInterval = rateInterval;

    ok = TRUE;

//  Configure scan sequence (collection session) for multiple channels...
//    if( channels > 1 )
    {
        status = SCAN_Setup(deviceNumber,numChans,chanVector,gainVector);
        ok = !NIDAQ_Error(status,"SCAN_Setup");
    }

    // Something's wrong, so get out...
    if( !ok )
    {
        return(FALSE);
    }

    // If buffered collection, start session now...
    if( NIDAQ_AI_Session[dev].buffered )
    {
/*        if( channels == 1 )  // Start single-channel session...
        {
            status = DAQ_Start(deviceNumber,chanVector[0],gainVector[0],buffer,count,sampTimebase,sampInterval);
            ok = !NIDAQ_Error(status,"DAQ_Start");
        }
        else                 // Start multi-channel session...
*/        {
            status = SCAN_Start(deviceNumber,buffer,count,sampTimebase,sampInterval,scanTimebase,scanInterval);
            ok = !NIDAQ_Error(status,"SCAN_Start");
        }
    }

    if( ok )
    {
        NIDAQ_AI_Session[dev].running = TRUE;
    }

    STR_printf(ok,NIDAQ_messgf,NIDAQ_errorf,"NIDAQ_AI_Start(dev=%d,channels=%d,frames=%d,buffered=%s) %s.\n",dev,channels,frames,STR_YesNo(NIDAQ_AI_Session[dev].buffered),STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Start( int dev, int chan[], double gain[], double duration, double sr )
{
int     frames,channels;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    if( NIDAQ_AI_Session[dev].running )
    {
        NIDAQ_errorf("NIDAQ_AI_Start(dev=%d,...) Collection already started.\n",dev);
        return(FALSE);
    }

    // Calculate number of frames based on collection time and frequency...
    frames = (int)(duration * sr);

    // Count number of channels...
    for( channels=0; ((chan[channels] != NIDAQ_CHANNEL_END) && (channels < NIDAQ_CHANNEL_MAX)); channels++ );

    // Allocate buffer space...
    if( (NIDAQ_AI_Buffer=NIDAQ_AI_BufferAllocate(channels,frames)) == NULL )
    {
        NIDAQ_errorf("NIDAQ_AI_Start(dev=%d,...) Cannot allocate memory.\n",dev);
        return(FALSE);
    }

    // Start collection...
    if( !NIDAQ_AI_Start(dev,chan,gain,frames,sr,NIDAQ_AI_Buffer) )
    {
        NIDAQ_AI_BufferFree(&NIDAQ_AI_Buffer);
        return(FALSE);
    }

    NIDAQ_debugf("NIDAQ_AI_Start(dev=%d,chans=%d,...,duration=%.3f,rate=%.1f) frames=%d.\n",dev,channels,duration,sr,frames);

    return(TRUE);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Start( int chan[], double gain[], double duration, double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_Start(NIDAQ_Device,chan,gain,duration,sr);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Start( int chan[], double duration, double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_Start(NIDAQ_Device,chan,NULL,duration,sr);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Start( int dev, int chan, double gain, double duration, double sr )
{
int     c[NIDAQ_CHANNEL_MAX];
double  g[NIDAQ_CHANNEL_MAX];
BOOL    ok;

    c[0] = chan; 
    c[1] = NIDAQ_CHANNEL_END;

    g[0] = gain;

    ok = NIDAQ_AI_Start(dev,c,g,duration,sr);

    return(ok);
}


/******************************************************************************/

BOOL    NIDAQ_AI_Start( int dev, int chan, double duration, double sr )
{
int     c[NIDAQ_CHANNEL_MAX];
BOOL    ok;

    c[0] = chan; 
    c[1] = NIDAQ_CHANNEL_END;

    ok = NIDAQ_AI_Start(dev,c,NULL,duration,sr);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Start( int chan, double gain, double duration, double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_Start(NIDAQ_Device,chan,gain,duration,sr);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Start( int chan, double duration, double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_Start(NIDAQ_Device,chan,duration,sr);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan[], double gain[], double duration, double sr, int dio, double post_trigger )
{
BOOL    ok;
int     channels,frames,SAF,SAC;
i16     status;
i16     deviceNumber;
i16     startTrig;
u32     signal;
u32     source;
u32     sourceSpec;
i16     extConv;
i16     stopTrig;
u32     ptsAfterStoptrig;

//  Make sure device number is valid...
    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

//  Setup (optional) internal digital I/O trigger line...
    if( !NIDAQ_AI_TriggerSetup(dev,dio) )
    {
        return(FALSE);
    }

    for( channels=0; ((chan[channels] != NIDAQ_CHANNEL_END) && (channels < NIDAQ_CHANNEL_MAX)); channels++ );
    frames = (int)(duration * sr);

    deviceNumber = (i16)dev;

    if( post_trigger == 0.0 )               // No post-trigger period, specify start trigger...
    {
        startTrig = TRUE;
        extConv = 0;

        status = DAQ_Config(deviceNumber,startTrig,extConv);

        if( (ok=!NIDAQ_Error(status,"DAQ_Config")) )
        {
            NIDAQ_debugf("DAQ_Config(dev=%d,startTrip=%d,extConv=%d)\n",
                         deviceNumber,
                         startTrig,
                         extConv);

            signal = ND_IN_START_TRIGGER;
            source = ND_PFI_0;
            sourceSpec = ND_LOW_TO_HIGH;

            status = Select_Signal(deviceNumber,signal,source,sourceSpec);
            ok = !NIDAQ_Error(status,"Select_Signal");
        }

        NIDAQ_debugf("NIDAQ_AI_TriggerStart(...) (%s) Start.\n",STR_OkFailed(ok));
    }
    else                                    // Post-trigger period, specify stop trigger...
    {
        if( post_trigger < 0.0 )            // Pre-trigger period (seconds)...
        {
            post_trigger += duration;       // Convert it to a post-trigger period.
        }

        SAF = (int)(post_trigger * sr);     // Stop After Frames...
        SAC = channels * SAF;               // Stop After Count (DAC conversions)...

        stopTrig = TRUE;
        ptsAfterStoptrig = (u32)SAC;

        status = DAQ_StopTrigger_Config(deviceNumber,stopTrig,ptsAfterStoptrig);

        if( (ok=!NIDAQ_Error(status,"DAQ_StopTrigger_Config")) )
        {
            NIDAQ_debugf("DAQ_StopTrigger_Config(dev=%d,stopTrip=%d,ptrAfterStoptrig=%ld)\n",
                         deviceNumber,
                         stopTrig,
                         ptsAfterStoptrig);

            signal = ND_IN_STOP_TRIGGER;
            source = ND_PFI_1;
            sourceSpec = ND_LOW_TO_HIGH;

            status = Select_Signal(deviceNumber,signal,source,sourceSpec);
            ok = !NIDAQ_Error(status,"Select_Signal");
        }

        NIDAQ_debugf("NIDAQ_AI_TriggerStart(...) (%s) Stop time %.3f (sec).\n",STR_OkFailed(ok),post_trigger);
    }

    if( ok )
    {
        ok = NIDAQ_AI_Start(dev,chan,gain,duration,sr);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int chan[], double gain[], double duration, double sr, int dio, double post_trigger )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(NIDAQ_Device,chan,gain,duration,sr,dio,post_trigger);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan, double gain, double duration, double sr, int dio, double post_trigger )
{
int     c[NIDAQ_CHANNEL_MAX];
double  g[NIDAQ_CHANNEL_MAX];
BOOL    ok;

    c[0] = chan; c[1] = NIDAQ_CHANNEL_END;
    g[0] = gain;

    ok = NIDAQ_AI_TriggerStart(dev,c,g,duration,sr,dio,post_trigger);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int chan, double gain, double duration, double sr, int dio, double post_trigger )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(NIDAQ_Device,chan,gain,duration,sr,dio,post_trigger);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan[], double gain[], double duration, double sr, double post_trigger )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(dev,chan,gain,duration,sr,NIDAQ_CHANNEL_NULL,post_trigger);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int chan[], double gain[], double duration, double sr, double post_trigger )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(chan,gain,duration,sr,NIDAQ_CHANNEL_NULL,post_trigger);
    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan, double gain, double duration, double sr, double post_trigger )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(dev,chan,gain,duration,sr,NIDAQ_CHANNEL_NULL,post_trigger);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int chan, double gain, double duration, double sr, double post_trigger )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(chan,gain,duration,sr,NIDAQ_CHANNEL_NULL,post_trigger);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan[], double gain[], double duration, double sr, int dio )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(dev,chan,gain,duration,sr,dio,0.0);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int chan[], double gain[], double duration, double sr, int dio )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(NIDAQ_Device,chan,gain,duration,sr,dio,0.0);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan, double gain, double duration, double sr, int dio )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(dev,chan,gain,duration,sr,dio,0.0);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int chan, double gain, double duration, double sr, int dio )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(NIDAQ_Device,chan,gain,duration,sr,dio,0.0);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan[], double gain[], double duration, double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(dev,chan,gain,duration,sr,NIDAQ_CHANNEL_NULL,0.0);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int chan[], double gain[], double duration, double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(NIDAQ_Device,chan,gain,duration,sr,NIDAQ_CHANNEL_NULL,0.0);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int dev, int chan, double gain, double duration, double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(dev,chan,gain,duration,sr,NIDAQ_CHANNEL_NULL,0.0);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerStart( int chan, double gain, double duration, double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_TriggerStart(NIDAQ_Device,chan,gain,duration,sr,NIDAQ_CHANNEL_NULL,0.0);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_TriggerSetup( int dev, int dio )
{
BOOL    ok;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    NIDAQ_DO_Trigger[dev] = dio;            // Save trigger channel number...

    if( dio == NIDAQ_CHANNEL_NULL )         // No internal trigger channel specified...
    {
        return(TRUE);
    }

    if( (ok=NIDAQ_DIG_ForOutput(dev,dio)) ) // Config channel for output...
    {
        ok = NIDAQ_DIG_Output(dev,dio,LO);  // Make sure trigger line is low...
    }

    return(ok);
}

/******************************************************************************/

void    NIDAQ_AI_Trigger( int dev )
{
BOOL    ok;

    if( NIDAQ_Check(dev) )
    {
        if( NIDAQ_DO_Trigger[dev] != NIDAQ_CHANNEL_NULL )
        {
            ok = NIDAQ_DIG_Output(dev,NIDAQ_DO_Trigger[dev],HI);
        }
    }    
}

/******************************************************************************/

void    NIDAQ_AI_Trigger( void )
{
    NIDAQ_AI_Trigger(NIDAQ_Device);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Progress( int dev, int &frames, BOOL &complete )
{
i16     status;
i16     deviceNumber,daqStopped;
u32     retrieved;
BOOL    ok=FALSE;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    if( !NIDAQ_AI_Session[dev].running )
    {
        NIDAQ_errorf("NIDAQ_AI_Progress(dev=%d,...) Collection not started.\n",dev);
        return(FALSE);
    }

    if( NIDAQ_AI_Session[dev].complete )
    {
        complete = TRUE;
        return(TRUE);
    }

    deviceNumber = (i16)dev;

    status = DAQ_Check(deviceNumber,&daqStopped,&retrieved);

    if( !NIDAQ_Error(status,"DAQ_Check") )
    {
        frames = (int)retrieved / NIDAQ_AI_Session[dev].channels;
        complete = (BOOL)daqStopped;
        ok = TRUE;
    }

    if( ok && complete )
    {
        NIDAQ_debugf("DAQ_Check(deviceNumber=%d,daqStopped=%d,retrieved=%ld)\n",deviceNumber,daqStopped,retrieved);
        NIDAQ_debugf("NIDAQ_AI_Progress(dev=%d,frames=%d,complete=TRUE)\n",dev,frames);

        NIDAQ_AI_Session[dev].complete = TRUE;
        NIDAQ_AI_Session[dev].running = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Progress( int &frames, BOOL &complete )
{
BOOL    ok;

    ok = NIDAQ_AI_Progress(NIDAQ_Device,frames,complete);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_IsComplete( int dev, int &frames )
{
BOOL    ok,complete;

    ok = NIDAQ_AI_Progress(dev,frames,complete);

    return(ok && !complete);
}

/******************************************************************************/

BOOL    NIDAQ_AI_IsComplete( int &frames )
{
BOOL    ok,complete;

    ok = NIDAQ_AI_Progress(NIDAQ_Device,frames,complete);

    return(ok && complete);
}

/******************************************************************************/

BOOL    NIDAQ_AI_WaitComplete( int dev, int &frames, double msec )
{
BOOL    ok,complete,TO=FALSE;
double  elapsed;
TIMER   timer("NIDAQ_AI_WaitComplete()");

    timer.Reset();

    do
    {
        ok = NIDAQ_AI_Progress(dev,frames,complete);

        if( msec != 0.0 )
        {
            TO = timer.Expired(msec);
        }
    }
    while( ok && !complete && !TO );

    elapsed = timer.Elapsed();

    STR_printf(ok,NIDAQ_debugf,NIDAQ_errorf,"NIDAQ_AI_WaitComplete(dev=%d,wait=%.1lf) %s Timeout=%s Complete=%s (%.1lf msec).\n",
               dev,msec,
               STR_OkFailed(ok),STR_YesNo(TO),STR_YesNo(complete),elapsed);

    return(ok && complete);
}

/******************************************************************************/

BOOL    NIDAQ_AI_WaitComplete( int dev, int &frames )
{
BOOL    ok;

    ok = NIDAQ_AI_WaitComplete(dev,frames,0.0);

    return(ok);}

/******************************************************************************/

BOOL    NIDAQ_AI_WaitComplete( int &frames, double msec )
{
BOOL    ok;

    ok = NIDAQ_AI_WaitComplete(NIDAQ_Device,frames,msec);

    return(ok);}

/******************************************************************************/

BOOL    NIDAQ_AI_WaitComplete( int &frames )
{
BOOL    ok;

    ok = NIDAQ_AI_WaitComplete(NIDAQ_Device,frames,0.0);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Voltage( int dev, int channel, i16 ADC, double &voltage )
{
BOOL    ok;
i16     status;
i16     deviceNumber;
i16     gain;
f64     gainAdjust;
f64     offset;
u32     count;
i16    *binArray;
f64     voltArray;

    deviceNumber = (i16)dev;
    channel = (i16)NIDAQ_AI_Session[dev].channel[channel].channel;
    gain = NIDAQ_AI_Gain(NIDAQ_AI_Session[dev].channel[channel].gain);
    gainAdjust = 1.0;
    offset = (f64)NIDAQ_AI_Session[dev].channel[channel].offset;
    count = 1;
    binArray = &ADC;
    voltArray = 0.0;

    status = DAQ_VScale(deviceNumber,channel,gain,gainAdjust,offset,count,binArray,&voltArray);

    if( (ok=!NIDAQ_Error(status,"DAQ_VScale")) )
    {
        voltage = voltArray;
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Monitor( int dev, double voltages[], int &frames, BOOL &ready, BOOL &complete )
{
i16     status;
i16     deviceNumber;
i16     channel;
i16     sequential;
u32     numPts;
i16     monitorBuffer[NIDAQ_CHANNEL_MAX];
u32     newestPtIndex;
i16     daqStopped;
BOOL    ok=TRUE;
int     c;

    ready = FALSE;
    complete = FALSE;

    // Valid device code...
    if( !NIDAQ_Check(dev) )
    {
        NIDAQ_errorf("NIDAQ_AI_Monitor(dev=%d,...) Invalid device.\n",dev);
        return(FALSE);
    }

    // Is an AI session currently running...
    if( !NIDAQ_AI_Session[dev].running )
    {
        NIDAQ_errorf("NIDAQ_AI_Monitor(dev=%d,...) Invalid session.\n",dev);
        return(FALSE);
    }

    // Or is it already complete...
    if( NIDAQ_AI_Session[dev].complete )
    {
        complete = TRUE;
        return(TRUE);
    }

    // Check progress of session...
    if( !NIDAQ_AI_Progress(dev,frames,complete) )
    {
        return(FALSE);
    }

    // If session complete return immediately...
    if( complete )
    {
        return(TRUE);
    }

    // Set parameters and call DAQ monitor function...
    deviceNumber = (i16)dev;
    channel = NIDAQ_CHANNEL_ALL;
    sequential = NIDAQ_MONITOR_CURRENT;
    numPts = (u32)NIDAQ_AI_Session[dev].channels;

    status = DAQ_Monitor(deviceNumber,channel,sequential,numPts,monitorBuffer,&newestPtIndex,&daqStopped);

    // Ignore "NO DATA" errors as these are handled else where...
    if( status != dataNotAvailError )
    {
        // Check status code for any other error...
        if( (ok=!NIDAQ_Error(status,"DAQ_Monitor")) )
        {
            ready = TRUE;
        }
    }

    if( !ready )
    {
        if( ok )
        {
            NIDAQ_debugf("NIDAQ_AI_Monitor(dev=%d,...) Data not ready.\n",dev);
        }

        return(ok);
    }

    // Convert raw monitor values to voltages (provided data was ready)...
    for( ok=TRUE,c=0; ((c < NIDAQ_AI_Session[dev].channels) && ok); c++ )
    {
        ok = NIDAQ_AI_Voltage(dev,c,monitorBuffer[c],voltages[c]);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Monitor( int dev, double voltages[], BOOL &ready, BOOL &complete )
{
BOOL    ok;
int     frames;

    ok = NIDAQ_AI_Monitor(dev,voltages,frames,ready,complete);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Monitor( int dev, double voltages[] )
{
BOOL    ok,ready,complete;

    ok = NIDAQ_AI_Monitor(dev,voltages,ready,complete);

    return(ok && ready);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Monitor( double voltages[], int &frames, BOOL &ready, BOOL &complete )
{
BOOL    ok;

    ok = NIDAQ_AI_Monitor(NIDAQ_Device,voltages,frames,ready,complete);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Monitor( double voltages[], BOOL &ready, BOOL &complete )
{
BOOL    ok;

    ok = NIDAQ_AI_Monitor(NIDAQ_Device,voltages,ready,complete);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Monitor( double voltages[] )
{
BOOL    ok;

    ok = NIDAQ_AI_Monitor(NIDAQ_Device,voltages);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Abort( int dev, BOOL release )
{
i16     status;
i16     deviceNumber;
BOOL    ok=FALSE;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    if( !NIDAQ_AI_Session[dev].running )
    {
        NIDAQ_errorf("NIDAQ_AI_Abort(dev=%d,...) Collection not started.\n",dev);
        return(FALSE);
    }

    deviceNumber = (i16)dev;

    status = DAQ_Clear(deviceNumber);

    if( !NIDAQ_Error(status,"DAQ_Clear") )
    {
        ok = TRUE;
    }

    // Optionally release buffer memory...
    if( release )
    {
        if( NIDAQ_AI_Buffer != NULL )
        {
            NIDAQ_AI_BufferFree(&NIDAQ_AI_Buffer);
        }
    }

    NIDAQ_AI_Session[dev].running = FALSE;

    STR_printf(ok,NIDAQ_debugf,NIDAQ_errorf,"NIDAQ_AI_Abort(dev=%d,release=%s) %s.\n",dev,STR_YesNo(release),STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Abort( int dev )
{
BOOL    ok;

    ok = NIDAQ_AI_Abort(dev,TRUE);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Abort( void )
{
BOOL    ok;

    ok = NIDAQ_AI_Abort(NIDAQ_Device);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_StopNow( int dev )
{
BOOL    complete=FALSE;
int     frames;

    // First, check on the progress of the collection sessionn...
    if( !NIDAQ_AI_Progress(dev,frames,complete) )
    {
        return(FALSE);
    }

    // If it's not complete, abort it prematurely...
    if( !complete )
    {
        if( !NIDAQ_AI_Abort(dev,FALSE) )    // Abort session (FALSE = don't release memory).
        {
            return(FALSE);
        }

        NIDAQ_AI_Session[dev].complete = TRUE;
        NIDAQ_AI_Session[dev].frames = frames;
    }

    return(TRUE);
}

/******************************************************************************/

BOOL    NIDAQ_AI_StopNow( void )
{
BOOL    ok;

    ok = NIDAQ_AI_StopNow(NIDAQ_Device);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Voltages( int dev, matrix &voltages, BOOL release )
{
i16     status;
i16     deviceNumber;
i16     chan;
i16     gain;
f64     gainAdjust;
f64     offset;
u32     count;
i16    *rawArray,*binArray=NULL;
f64    *voltArray=NULL;
int     channels,channel,frames,frame;
BOOL    ok=TRUE;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    if( !NIDAQ_AI_Session[dev].complete )
    {
        NIDAQ_errorf("NIDAQ_AI_Voltages(dev=%d,...) Invalid or incomplete session.\n",dev);
        return(FALSE);
    }

    frames = NIDAQ_AI_Session[dev].frames;
    channels = NIDAQ_AI_Session[dev].channels;

    if( (binArray=(i16*)calloc(frames,sizeof(i16))) == NULL )
    {
        ok = FALSE;
    }
    else
    if( (voltArray=(f64*)calloc(frames,sizeof(f64))) == NULL )
    {
        ok = FALSE;
    }

    if( !ok )
    {
        if( binArray != NULL ) free(binArray);
        if( voltArray != NULL ) free(voltArray);

        NIDAQ_errorf("NIDAQ_AI_Voltages(dev=%d,...) Cannot allocate memory.\n",dev);
        return(FALSE);
    }

    matrix_dim(voltages,frames,channels);
    NIDAQ_debugf("NIDAQ_AI_Voltages(...) Matrix allocated (%dx%d).\n",voltages.rows(),voltages.cols());

    for( ok=TRUE,channel=0; ((channel < channels) && ok); channel++ )
    {
        deviceNumber = (i16)dev;
        chan = (i16)NIDAQ_AI_Session[dev].channel[channel].channel;
        gain = NIDAQ_AI_Gain(NIDAQ_AI_Session[dev].channel[channel].gain);
        gainAdjust = 1.0;
        offset = (f64)NIDAQ_AI_Session[dev].channel[channel].offset;
        count = (u32)NIDAQ_AI_Session[dev].frames;
        rawArray = (i16 *)NIDAQ_AI_Session[dev].buffer;

        for( frame=0; (frame < frames); frame++ )
        {
            binArray[frame] = rawArray[(frame*channels)+channel];
        }

        status = DAQ_VScale(deviceNumber,chan,gain,gainAdjust,offset,count,binArray,voltArray);
        ok = !NIDAQ_Error(status,"DAQ_VScale");

        for( frame=0; ((frame < frames) && ok); frame++ )
        {
            voltages(frame+1,channel+1) = (double)voltArray[frame];
        }
    }

    free(binArray);
    free(voltArray);

    if( release )
    {
        NIDAQ_AI_BufferFree(&NIDAQ_AI_Session[dev].buffer);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Voltages( matrix &voltages, BOOL release )
{
BOOL    ok;

    ok = NIDAQ_AI_Voltages(NIDAQ_Device,voltages,release);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Voltages( int dev, matrix &voltages )
{
BOOL    ok;

    ok = NIDAQ_AI_Voltages(dev,voltages,TRUE);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_Voltages( matrix &voltages )
{
BOOL    ok;

    ok = NIDAQ_AI_Voltages(NIDAQ_Device,voltages,TRUE);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Start( int dev, int chan[], double gain[], double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_Start(dev,chan,gain,0,sr,NULL);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Start( int dev, int chan[], double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_Start(dev,chan,NULL,0,sr,NULL);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Start( int chan[], double gain[], double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_Start(NIDAQ_Device,chan,gain,0,sr,NULL);

    return(ok);
}


/******************************************************************************/

BOOL    NIDAQ_AI_NB_Start( int chan[], double sr )
{
BOOL    ok;

    ok = NIDAQ_AI_Start(NIDAQ_Device,chan,NULL,0,sr,NULL);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Read( int dev, double voltages[] )
{
i16     status;
i16     deviceNumber;
f64    *reading;
int     channel;
BOOL    ok;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    deviceNumber = (i16)dev;      // Device number.
    reading = NIDAQ_AI_f64;       // Array of double-precision floats (f64).

    status = AI_VRead_Scan(deviceNumber,reading);

    if( (ok=!NIDAQ_Error(status,"AI_VRead_Scan")) )
    {
        for( ok=TRUE,channel=0; ((channel < NIDAQ_AI_Session[dev].channels) && ok); channel++ )
        {
            voltages[channel] = (double)reading[channel];
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Read( int dev, int values[] )
{
i16     status;
i16     deviceNumber;
i16    *reading;
int     channel;
BOOL    ok;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    deviceNumber = (i16)dev;      // Device number.
    reading = NIDAQ_AI_i16;       // Array of A/D integers (i16).

    status = AI_Read_Scan(deviceNumber,reading);

    if( (ok=!NIDAQ_Error(status,"AI_Read_Scan")) )
    {
        for( channel=0; (channel < NIDAQ_AI_Session[dev].channels); channel++ )
        {
            values[channel] = (int)reading[channel];
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Read( double voltages[] )
{
BOOL    ok;

    ok = NIDAQ_AI_NB_Read(NIDAQ_Device,voltages);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Read( int values[] )
{
BOOL    ok;

    ok = NIDAQ_AI_NB_Read(NIDAQ_Device,values);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Read( int dev, int chan, double gain, double &voltage )
{
i16     status;
i16     deviceNumber;
i16     c,g;
f64     reading;
BOOL    ok;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    deviceNumber = (i16)dev;      // Device number.
    c = (i16)chan;                // Channel number for reading.
    g = NIDAQ_AI_Gain(gain);      // Gain for reading.

    status = AI_VRead(deviceNumber,c,g,&reading);

    if( (ok=!NIDAQ_Error(status,"AI_VRead")) )
    {
        voltage = (double)reading;
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Read( int chan, double gain, double &voltage )
{
BOOL    ok;

    ok = NIDAQ_AI_NB_Read(NIDAQ_Device,chan,gain,voltage);

    return(ok);
}


/******************************************************************************/

BOOL    NIDAQ_AI_NB_Read( int dev, int chan, double gain, int &value )
{
i16     status;
i16     deviceNumber;
i16     c,g;
i16     reading;
BOOL    ok;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    deviceNumber = (i16)dev;      // Device number.
    c = (i16)chan;                // Channel number for reading.
    g = NIDAQ_AI_Gain(gain);      // Gain for reading.

    status = AI_Read(deviceNumber,c,g,&reading);

    if( (ok=!NIDAQ_Error(status,"AI_Read")) )
    {
        value = (int)reading;
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_AI_NB_Read( int chan, double gain, int &value )
{
BOOL    ok;

    ok = NIDAQ_AI_NB_Read(NIDAQ_Device,chan,gain,value);

    return(ok);
}

/******************************************************************************/
/* Digital I/O Functions...                                                   */
/******************************************************************************/

BOOL    NIDAQ_DIG_Setup( int dev, int chan, int mode )
{
i16     status;
i16     deviceNumber;
i16     port;
i16     line;
i16     dir;
BOOL    ok;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    deviceNumber = (i16)dev;      // Device number.
    port = 0;                     // Port number (ignored on E series).
    line = (i16)chan;             // Digital I/O line.
    dir = (i16)mode;              // Direction (input or output).

    status = DIG_Line_Config(deviceNumber,port,line,dir);
    ok = !NIDAQ_Error(status,"DIG_Line_Config");

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Setup( int chan, int mode )
{
BOOL    ok;

    ok = NIDAQ_DIG_Setup(NIDAQ_Device,chan,mode);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Setup( int dev, int chan[], int mode[] )
{
int     channel;
BOOL    ok;

    for( ok=TRUE,channel=0; ((chan[channel] != NIDAQ_CHANNEL_END) && (channel < NIDAQ_CHANNEL_MAX) && ok); channel++ )
    {
        ok = NIDAQ_DIG_Setup(dev,chan[channel],mode[channel]);
    }

    return(ok);
}


/******************************************************************************/

BOOL    NIDAQ_DIG_Setup( int chan[], int mode[] )
{
BOOL    ok;

    ok = NIDAQ_DIG_Setup(NIDAQ_Device,chan,mode);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Setup( int dev, int chan[], int mode )
{
int     channel;
BOOL    ok;

    for( ok=TRUE,channel=0; ((chan[channel] != NIDAQ_CHANNEL_END) && (channel < NIDAQ_CHANNEL_MAX) && ok); channel++ )
    {
        ok = NIDAQ_DIG_Setup(dev,chan[channel],mode);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Setup( int chan[], int mode )
{
BOOL    ok;

    ok = NIDAQ_DIG_Setup(NIDAQ_Device,chan,mode);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_ForOutput( int dev, int chan[] )
{
BOOL    ok;

    ok = NIDAQ_DIG_Setup(dev,chan,NIDAQ_DIGITAL_OUTPUT);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_ForOutput( int dev, int chan )
{
BOOL    ok;

    ok = NIDAQ_DIG_Setup(dev,chan,NIDAQ_DIGITAL_OUTPUT);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_ForOutput( int chan[] )
{
BOOL    ok;

    ok = NIDAQ_DIG_ForOutput(NIDAQ_Device,chan);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_ForOutput( int chan )
{
BOOL    ok;

    ok = NIDAQ_DIG_ForOutput(NIDAQ_Device,chan);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Output( int dev, int chan, BOOL value )
{
i16     status;
i16     deviceNumber;
i16     port;
i16     line;
i16     state;
BOOL    ok;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    deviceNumber = (i16)dev;      // Device number.
    port = 0;                     // Port number (ignored on E series).
    line = (i16)chan;             // Digital I/O line.
    state = (i16)value;           // State of digital line.

    status = DIG_Out_Line(deviceNumber,port,line,state);
    ok = !NIDAQ_Error(status,"DIG_Out_Line");

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Output( int chan, BOOL value )
{
BOOL    ok;

    ok = NIDAQ_DIG_Output(NIDAQ_Device,chan,value);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Output( int dev, int chan[], BOOL value[] )
{
int     channel;
BOOL    ok;

    for( ok=TRUE,channel=0; ((chan[channel] != NIDAQ_CHANNEL_END) && (channel < NIDAQ_CHANNEL_MAX) && ok); channel++ )
    {
        ok = NIDAQ_DIG_Output(dev,chan[channel],value[channel]);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Output( int chan[], BOOL value[] )
{
BOOL    ok;

    ok = NIDAQ_DIG_Output(NIDAQ_Device,chan,value);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_SetOutput( int dev, int chan[], BOOL value[] )
{
BOOL    ok;

    if( (ok=NIDAQ_DIG_Setup(dev,chan,NIDAQ_DIGITAL_OUTPUT)) )
    {
        ok = NIDAQ_DIG_Output(dev,chan,value);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_SetOutput( int dev, int chan, BOOL value )
{
BOOL    ok;

    if( (ok=NIDAQ_DIG_Setup(dev,chan,NIDAQ_DIGITAL_OUTPUT)) )
    {
        ok = NIDAQ_DIG_Output(dev,chan,value);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_SetOutput( int chan[], BOOL value[] )
{
BOOL    ok;

    if( (ok=NIDAQ_DIG_ForOutput(NIDAQ_Device,chan)) )
    {
        ok = NIDAQ_DIG_Output(NIDAQ_Device,chan,value);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_SetOutput( int chan, BOOL value )
{
BOOL    ok;

    if( (ok=NIDAQ_DIG_ForOutput(NIDAQ_Device,chan)) )
    {
        ok = NIDAQ_DIG_Output(NIDAQ_Device,chan,value);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_ForInput( int dev, int chan[] )
{
BOOL    ok;

    ok = NIDAQ_DIG_Setup(dev,chan,NIDAQ_DIGITAL_INPUT);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_ForInput( int dev, int chan )
{
BOOL    ok;

    ok = NIDAQ_DIG_Setup(dev,chan,NIDAQ_DIGITAL_INPUT);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_ForInput( int chan[] )
{
BOOL    ok;

    ok = NIDAQ_DIG_ForInput(NIDAQ_Device,chan);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_ForInput( int chan )
{
BOOL    ok;

    ok = NIDAQ_DIG_ForInput(NIDAQ_Device,chan);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Input( int dev, int chan, BOOL &value )
{
i16     status;
i16     deviceNumber;
i16     port;
i16     line;
i16     state;
BOOL    ok;

    if( !NIDAQ_Check(dev) )
    {
        return(FALSE);
    }

    deviceNumber = (i16)dev;      // Device number.
    port = 0;                     // Port number (ignored on E series).
    line = (i16)chan;             // Digital I/O line.
    state = FALSE;                // State of digital line.

    status = DIG_In_Line(deviceNumber,port,line,&state);
    ok = !NIDAQ_Error(status,"DIG_In_Line");

    if( ok )
    {
        value = state;
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Input( int chan, BOOL &value )
{
BOOL    ok;

    ok = NIDAQ_DIG_Input(NIDAQ_Device,chan,value);

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Input( int dev, int chan[], BOOL value[] )
{
int     channel;
BOOL    ok;

    for( ok=TRUE,channel=0; ((chan[channel] != NIDAQ_CHANNEL_END) && (channel < NIDAQ_CHANNEL_MAX) && ok); channel++ )
    {
        ok = NIDAQ_DIG_Input(dev,chan[channel],value[channel]);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_Input( int chan[], BOOL value[] )
{
BOOL    ok;

    ok = NIDAQ_DIG_Input(NIDAQ_Device,chan,value);

    return(ok);
}


/******************************************************************************/

BOOL    NIDAQ_DIG_GetInput( int dev, int chan[], BOOL value[] )
{
BOOL    ok;

    if( (ok=NIDAQ_DIG_Setup(dev,chan,NIDAQ_DIGITAL_INPUT)) )
    {
        ok = NIDAQ_DIG_Input(dev,chan,value);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_GetInput( int dev, int chan, BOOL &value )
{
BOOL    ok;

    if( (ok=NIDAQ_DIG_Setup(dev,chan,NIDAQ_DIGITAL_INPUT)) )
    {
        ok = NIDAQ_DIG_Input(dev,chan,value);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_GetInput( int chan[], BOOL value[] )
{
BOOL    ok;

    if( (ok=NIDAQ_DIG_ForInput(NIDAQ_Device,chan)) )
    {
        ok = NIDAQ_DIG_Input(NIDAQ_Device,chan,value);
    }

    return(ok);
}

/******************************************************************************/

BOOL    NIDAQ_DIG_GetInput( int chan, BOOL &value )
{
BOOL    ok;

    if( (ok=NIDAQ_DIG_ForInput(NIDAQ_Device,chan)) )
    {
        ok = NIDAQ_DIG_Input(NIDAQ_Device,chan,value);
    }

    return(ok);
}

/******************************************************************************/

