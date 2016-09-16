/******************************************************************************/
/*                                                                            */
/* MODULE  : TRACKING.cpp                                                     */
/*                                                                            */
/* PURPOSE : Position TRACKING API for general access to tracking hardware.   */
/*                                                                            */
/* DATE    : 30/Jan/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 30/Jan/2002 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 18/Mar/2002 - Added normal API start / stop stuff.               */
/*                                                                            */
/* V1.2  JNI 06/Jul/2009 - Added support for Polhemus Liberty.                */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "TRACKING"
#define MODULE_TEXT     "Position Tracking API"
#define MODULE_DATE     "06/07/2009"
#define MODULE_VERSION  "1.2"
#define MODULE_LEVEL    4

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

BOOL    TRACKING_API_started=FALSE;
int     TRACKING_Device=TRACKING_INVALID;
int     TRACKING_Marker=0;
matrix  TRACKING_OPTO_seen;
matrix  TRACKING_OPTO_posn;
matrix  TRACKING_FOB_posn[FOB_BIRD_MAX];
matrix  TRACKING_ROBOT_posn(SPMX_POMX_MTX);
matrix  TRACKING_LIBERTY_posn[LIBERTY_SENSOR_MAX];

/******************************************************************************/

struct  STR_TextItem  TRACKING_DeviceText[] =
{
    { TRACKING_NONE    ,"NONE"    },
    { TRACKING_OPTOTRAK,"OPTO"    },
    { TRACKING_FOB     ,"FOB"     },
    { TRACKING_ROBOT   ,"ROBOT"   },
    { TRACKING_MOUSE   ,"MOUSE"   },
    { TRACKING_LIBERTY ,"LIBERTY" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

STRING TRACKING_Config[TRACKING_MAX];

void TRACKING_ConfigVariables( void )
{
int device;

    for( device=0; !STR_TextEoT(TRACKING_DeviceText,device); device++ )
    {
        CONFIG_set(TRACKING_DeviceText[device].text,TRACKING_Config[device]);
    }
}

/******************************************************************************/

PRINTF TRACKING_PRN_messgf=NULL;           // General messages printf function.
PRINTF TRACKING_PRN_errorf=NULL;           // Error messages printf function.
PRINTF TRACKING_PRN_debugf=NULL;           // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int TRACKING_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(TRACKING_PRN_messgf,buff));
}

/******************************************************************************/

int TRACKING_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(TRACKING_PRN_errorf,buff));
}

/******************************************************************************/

int TRACKING_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(TRACKING_PRN_debugf,buff));
}

/******************************************************************************/

BOOL TRACKING_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL ok;

    TRACKING_PRN_messgf = messgf;           // General API message print function.
    TRACKING_PRN_errorf = errorf;           // API error message print function.
    TRACKING_PRN_debugf = debugf;           // Debug information print function.

    if( TRACKING_API_started )              // Start the API once...
    {
        return(TRUE);
    }

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(TRACKING_API_stop);      // Install stop function.
        TRACKING_API_started = TRUE;        // Set started flag.
        MODULE_start(TRACKING_PRN_messgf);  // Register module.
    }
    else
    {
        TRACKING_errorf("TRACKING_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void TRACKING_API_stop( void )
{
int ID;

    if( !TRACKING_API_started )        // API not started in the first place...
    {
         return;
    }

    TRACKING_Close();                  // Close tracking device.
    TRACKING_API_started = FALSE;      // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL TRACKING_API_check( void )
{
BOOL ok=TRUE;

    if( !TRACKING_API_started )        // API not started...
    {                                  // Start module automatically...
        ok = TRACKING_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        STR_printf(ok,TRACKING_debugf,TRACKING_errorf,"TRACKING_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

int TRACKING_DeviceCode( char *name )
{
int code;

    if( (code=STR_TextCode(TRACKING_DeviceText,name)) == STR_NOTFOUND )
    {
        code = TRACKING_INVALID;
    }

    return(code);
}

/******************************************************************************/

BOOL TRACKING_Check( void )
{
BOOL ok=TRUE;

    if( !TRACKING_API_check() )
    {
        ok = FALSE;
    }
    else
    if( TRACKING_Device == TRACKING_INVALID )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL TRACKING_Open( STRING select[]  )
{
BOOL ok;

    ok = TRACKING_Open(select[TRACKING_DEVICE],select[TRACKING_CONFIG],select[TRACKING_MARKER]);

    return(ok);
}

/******************************************************************************/

BOOL TRACKING_Open( char *device, char *config, char *marker )
{
BOOL ok;

    if( (ok=TRACKING_Open(device,config)) )
    {
        if( !STR_null(marker,STRLEN) )
        {
            TRACKING_Marker = atoi(marker);
            TRACKING_debugf("TRACKING_Marker=%d\n",TRACKING_Marker);
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL TRACKING_Open( char *device, char *config )
{
BOOL ok=FALSE;

    // Make sure API is running...
    if( !TRACKING_API_check() )
    {
        return(FALSE);
    }

    // Make sure a tracking device in not already open...
    if( TRACKING_Device != TRACKING_INVALID )
    {
        TRACKING_errorf("TRACKING_Open(device=%s,config=%s) Already open.\n",device,config);
        return(FALSE);
    }

    // Map device name to device code...
    if( (TRACKING_Device=TRACKING_DeviceCode(device)) == TRACKING_INVALID )
    {
        TRACKING_errorf("TRACKING_Open(device=%s,config=%s) Invalid device name.\n",device,config);
        return(FALSE);
    }

    switch( TRACKING_Device )
    {
        case TRACKING_NONE :
           ok = TRUE;
           break;

        case TRACKING_OPTOTRAK :
           ok = OPTO_Open(config);
           break;

        case TRACKING_FOB :
           ok = FOB_Open(config);
           break;

        case TRACKING_ROBOT :
           ok = (ROBOT_Open(config) != ROBOT_INVALID);
           break;

        case TRACKING_MOUSE :
           ok = MOUSE_Open(config);
           break;

        case TRACKING_LIBERTY :
           ok = LIBERTY_Open(config);
           break;
    }

    if( !ok )
    {
        TRACKING_Device = TRACKING_INVALID;
    }

    STR_printf(ok,TRACKING_debugf,TRACKING_errorf,"TRACKING_Open(device=%s,config=%s) %s.\n",device,config,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

void TRACKING_Close( void )
{
BOOL ok=FALSE;

    // Make sure a device is current...
    if( !TRACKING_Check() )
    {
        return;
    }

    // First mke sure we've stopped...
    TRACKING_Stop();

    // Nothing can go wrong...
    ok = TRUE;

    switch( TRACKING_Device )
    {
        case TRACKING_NONE :
           break;

        case TRACKING_OPTOTRAK :
           OPTO_Close();
           break;

        case TRACKING_FOB :
           FOB_Close();
           break;

        case TRACKING_ROBOT :
           ROBOT_Close();
           break;

        case TRACKING_MOUSE :
           MOUSE_Close();
           break;

        case TRACKING_LIBERTY :
           LIBERTY_Close();
           break;
    }

    STR_printf(ok,TRACKING_debugf,TRACKING_errorf,"TRACKING_Close() %s.\n",STR_OkFailed(ok));

    TRACKING_Device = TRACKING_INVALID;
    TRACKING_Marker = TRACKING_INVALID;
}

/******************************************************************************/

BOOL TRACKING_Type( int device )
{
BOOL flag=FALSE;

    if( TRACKING_Check() )
    {
        flag = (TRACKING_Device == device);
    }

    return(flag);
}

/******************************************************************************/

BOOL TRACKING_Started( void )
{
BOOL flag=FALSE;

    // Make sure a device is current...
    if( !TRACKING_Check() )
    {
        return(FALSE);
    }

    switch( TRACKING_Device )
    {
        case TRACKING_NONE :
           flag = TRUE;
           break;

        case TRACKING_OPTOTRAK :
           flag = OPTO_Started();
           break;

        case TRACKING_FOB :
           flag = FOB_Started();
           break;

        case TRACKING_ROBOT :
           flag = ROBOT_Started();
           break;

        case TRACKING_MOUSE :
           flag = MOUSE_Started();
           break;

        case TRACKING_LIBERTY :
           flag = LIBERTY_Started();
           break;
    }

    return(flag);
}

/******************************************************************************/

BOOL TRACKING_Start( void )
{
BOOL ok=FALSE;

    // Make sure a device is current...
    if( !TRACKING_Check() )
    {
        return(FALSE);
    }

    // Already started?
    if( TRACKING_Started() )
    {
        return(TRUE);
    }

    switch( TRACKING_Device )
    {
        case TRACKING_NONE :
           ok = TRUE;
           break;

        case TRACKING_OPTOTRAK :
           ok = OPTO_Start();
           break;

        case TRACKING_FOB :
           ok = FOB_Start();
           break;

        case TRACKING_ROBOT :
           ok = ROBOT_Start();
           break;

        case TRACKING_MOUSE :
           ok = MOUSE_Start();
           break;

        case TRACKING_LIBERTY :
           ok = LIBERTY_Start();
           break;
    }

    STR_printf(ok,TRACKING_debugf,TRACKING_errorf,"TRACKING_Start() %s.\n",STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

void TRACKING_Stop( void )
{
BOOL ok=FALSE;

    // Make sure we're started in the first place...
    if( !TRACKING_Started() )
    {
        return;
    }

    // Nothing can go wrong...
    ok = TRUE;

    switch( TRACKING_Device )
    {
        case TRACKING_NONE :
           break;

        case TRACKING_OPTOTRAK :
           OPTO_Stop();
           break;

        case TRACKING_FOB :
           FOB_Stop();
           break;

        case TRACKING_ROBOT :
           ROBOT_Stop();
           break;

        case TRACKING_MOUSE :
           break;

        case TRACKING_LIBERTY :
           LIBERTY_Stop();
           break;
    }

    STR_printf(ok,TRACKING_debugf,TRACKING_errorf,"TRACKING_Stop() %s.\n",STR_OkFailed(ok));
}

/******************************************************************************/

BOOL TRACKING_GetPosn( int marker, matrix &pomx )
{
BOOL ok=FALSE;
UINT frame;

    if( pomx.isempty() )
    {
        pomx.dim(3,1);
    }

    // Make sure we're started in the first place...
    if( !TRACKING_Started() )
    {
        return(FALSE);
    }

    switch( TRACKING_Device )
    {
        case TRACKING_NONE :
           ok = TRUE;
           break;

        case TRACKING_OPTOTRAK :
           frame = OPTO_GetPosn(TRACKING_OPTO_posn,TRACKING_OPTO_seen);

           if( marker == 0 )
           {
               marker = 1;
           }

           if( (ok=TRACKING_OPTO_seen.B(OPTO_SEEN_ROW,marker)) )
           {
               matrix_array_get(pomx,SPMX_ptmx2pomx(TRACKING_OPTO_posn),marker);
           }
           break;

        case TRACKING_FOB :
           if( (ok=FOB_Data(TRACKING_FOB_posn,NULL,NULL,NULL,NULL)) )
           {
               pomx = TRACKING_FOB_posn[marker];
           }
           break;

        case TRACKING_ROBOT :
           ROBOT_Position(TRACKING_ROBOT_posn);
           pomx = TRACKING_ROBOT_posn;
           ok = TRUE;
           break;

        case TRACKING_MOUSE :
           ok = MOUSE_GetPosn(pomx);
           break;

        case TRACKING_LIBERTY :
           if( (ok=LIBERTY_GetPosn(TRACKING_LIBERTY_posn)) )
           {
               pomx = TRACKING_LIBERTY_posn[marker];
           }
           break;
    }

    return(ok);
}

/******************************************************************************/

BOOL TRACKING_GetPosn( matrix &pomx )
{
BOOL ok;

    ok = TRACKING_GetPosn(TRACKING_Marker,pomx);

    return(ok);
}

/******************************************************************************/

