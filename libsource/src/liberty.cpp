/******************************************************************************/
/*                                                                            */
/* MODULE  : LIBERTY.cpp                                                      */
/*                                                                            */
/* PURPOSE : Polhemus Liberty motion tracking system.                         */
/*                                                                            */
/* DATE    : 06/Jul/2009                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 06/Jul/2009 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 08/Jan/2015 - 3BOT-inspired continued development and bug fixes. */
/*                                                                            */
/* V1.2  JNI 20/Apr/2015 - Add distortion level to frame data.                */
/*                                                                            */
/******************************************************************************/

#include <motor.h>

/******************************************************************************/

#define         LIBERTY_SENSOR_MAX  16

CPDIdev		LIBERTY_Device;
CPDImdat    	LIBERTY_SensorDataList;
BINHDR          LIBERTY_BINHDR;
int             LIBERTY_SensorDataCount;
BOOL            LIBERTY_OpenFlag=FALSE;
BOOL            LIBERTY_StartFlag=FALSE;
DWORD		LIBERTY_FrameCount;
DWORD		LIBERTY_FrameCountLast=0L;
double          LIBERTY_FrameRate;
ePiFrameRate    LIBERTY_FrameRateCode;
BOOL		LIBERTY_CommandLog=FALSE;
matrix          LIBERTY_HemisphereVector(3,1);
matrix          LIBERTY_PositionOffset(3,1);
matrix          LIBERTY_PositionRotation(3,3);
BOOL            LIBERTY_ErrorFlag=FALSE;
DWORD           LIBERTY_GetSensorMask=0L;
DWORD           LIBERTY_SetSensorMask=0L;
int             LIBERTY_SensorCount=0;
int             LIBERTY_SensorList[LIBERTY_SENSOR_MAX];
BOOL            LIBERTY_SensorFlag[LIBERTY_SENSOR_MAX];
matrix          LIBERTY_Posn[LIBERTY_SENSOR_MAX];
matrix          LIBERTY_Ornt[LIBERTY_SENSOR_MAX];

double          LIBERTY_FrameRateList[] = { 120.0, 240.0, 0.0 };
ePiFrameRate    LIBERTY_FrameRateCodeList[] = { PI_FRATE_120,PI_FRATE_240 };

TIMER_Interval  LIBERTY_LastFrameCountLatency("LastFrameCountLatency");
TIMER_Interval  LIBERTY_LastFrameLatency("LastFrameLatency");
TIMER_Interval  LIBERTY_GetFrameLatency("GetFrameLatency");
TIMER_Frequency LIBERTY_FrameFrequency("FrameFrequency");

/******************************************************************************/

#define LIBERTY_errorf printf
#define LIBERTY_messgf printf
#define LIBERTY_debugf printf

/******************************************************************************/

void LIBERTY_Error( char *str )
{
    LIBERTY_errorf("LIBERTY: %s (%s).\n",str,LIBERTY_Device.GetLastResultStr());
}

/******************************************************************************/

void LIBERTY_ConfigSetup( void )
{
int i;

    LIBERTY_FrameRate = 240.0;

    LIBERTY_HemisphereVector.zeros();
    LIBERTY_HemisphereVector(2,1) = 1.0;

    LIBERTY_PositionOffset.zeros();
    matrix_I(LIBERTY_PositionRotation,3); // Identity matrix.

    LIBERTY_SensorCount = 0;

    for( i=0; (i < LIBERTY_SENSOR_MAX); i++ )
    {
        LIBERTY_SensorList[i] = (i == 0) ? 1 : 0;
        LIBERTY_SensorFlag[i] = FALSE;
    }

    // Reset configuration variable list.
    CONFIG_reset();

    // Set up variable list for configuration.
    CONFIG_set("FrameRate",LIBERTY_FrameRate);
    CONFIG_set("HemisphereVector",LIBERTY_HemisphereVector);
    CONFIG_set("PositionOffset",LIBERTY_PositionOffset);
    CONFIG_set("PositionRotation",LIBERTY_PositionRotation);
    CONFIG_set("SensorList",LIBERTY_SensorList,LIBERTY_SENSOR_MAX);
    CONFIG_setBOOL("CommandLog",LIBERTY_CommandLog);
}

/******************************************************************************/

BOOL LIBERTY_ConfigLoad( char *file )
{
int i;
BOOL ok=TRUE;
char *path;

    if( (path=FILE_Calibration(file)) == NULL )
    {
        LIBERTY_errorf("LIBERTY_ConfigLoad(%s) Cannot find file.\n",file);
        return(FALSE);
    }

    // Setup and initialize configuration variables.
    LIBERTY_ConfigSetup();

    // Load configuration file.
    if( !CONFIG_read(path) )
    {
        LIBERTY_errorf("LIBERTY_ConfigLoad(%s) Cannot read file.\n",file);
        return(FALSE);
    }

    for( i=0; ((LIBERTY_FrameRateList[i] != LIBERTY_FrameRate) && (LIBERTY_FrameRateList[i] != 0.0)); i++ );
    if( LIBERTY_FrameRateList[i] == 0.0 )
    {
        LIBERTY_errorf("LIBERTY_ConfigLoad(%s) Invalid frame rate.\n",file);
        ok = FALSE;
    }
    else
    {
        LIBERTY_FrameRateCode = LIBERTY_FrameRateCodeList[i];
    }
   
    for( i=0; (i < LIBERTY_SENSOR_MAX); i++ )
    {
        if( LIBERTY_SensorList[i] > 0 )
        {
            LIBERTY_SensorFlag[LIBERTY_SensorList[i]-1] = TRUE;
            LIBERTY_SensorCount++;
        }
    }

    if( LIBERTY_SensorCount == 0 )
    {
        LIBERTY_errorf("LIBERTY_ConfigLoad(%s) No sensors specified.\n",file);
        ok = FALSE;
    }

    LIBERTY_messgf("LIBERTY_ConfigLoad(%s) Load %s.\n",path,STR_OkFailed(ok));
    if( ok )
    {
        CONFIG_list(LIBERTY_messgf);
    }

    return(ok);

}

/******************************************************************************/

BOOL LIBERTY_Open( char *cnfg )
{
BOOL ok=FALSE;

    if( LIBERTY_ConfigLoad(cnfg) )
    {
        ok = LIBERTY_Open(LIBERTY_FrameRateCode,LIBERTY_HemisphereVector,LIBERTY_SensorList);
    }

    return(ok);
}

/******************************************************************************/

char *LIBERTY_OpenStateText[] = { "CommandLog","ConnectUSB","SetHemisphere","SetFrameRate","SetMetricUnits","SetSensorMask","SetDataList","HemisphereTracking","GetBitErrors" };

BOOL LIBERTY_Open( ePiFrameRate FrameRateCode, matrix &HemisphereVector, BOOL SensorList[] )
{
PDI3vec HV;
CPDIbiterr BE;
STRING BEs;
TIMER_Interval Latency;
int state,i;
BOOL ok;
double dt;
DWORD mask;

    // Check if connection already established.
    if( LIBERTY_Device.CnxReady() )
    {
        return(TRUE);
    }

    for( ok=TRUE,state=0; ((state <= 7) && ok); state++ )
    {
        Latency.Before();

        switch( state )
        {
            case 0 : // Set command logging flag.
                if( LIBERTY_CommandLog )
                {
                    LIBERTY_Device.Log(TRUE);

                    if( !ok )
                    {
                        LIBERTY_Error("Cannot enable command logging (trace)");
                        ok = FALSE;
                    }
                }
                break;

            case 1 : // Connect specifically via USB.
                if( LIBERTY_Device.DiscoverCnx() != PI_CNX_USB )
                {
                    LIBERTY_Error("Did not discover a USB connection with device");
                    ok = FALSE;
                }
                else
                if( !LIBERTY_Device.ConnectUSB() )
                {
                    LIBERTY_Error("Cannot establish USB connection to device");
                    ok = FALSE;
                }
                break;

            case 2 : // Set current sensor hemisphere (for all sensors).
                // Vector to describe hemisphere.
                for( i=0; (i < 3); i++ )
                {
                    HV[i] = LIBERTY_HemisphereVector(i+1,1);
                }

                // Set hemisphere vector (for all sensors).
                if( !LIBERTY_Device.SetSHemisphere(-1,HV) )
                {
                    LIBERTY_Error("Cannot set hemisphere");
                    ok = FALSE;
                }
                break;

            case 3 : // Set frame rate.
                if( !LIBERTY_Device.SetFrameRate(FrameRateCode) )
                {
                    LIBERTY_Error("Cannot set frame rate");
                    ok = FALSE;
                }
                break;

            case 4 : // Set metric output (cm).
                if( !LIBERTY_Device.SetMetric(TRUE) )
                {
                    LIBERTY_Error("Cannot set metric output");
                    ok = FALSE;
                }
                break;

            case 5 : // Enable the required sensors and disable the rest.
                if( !LIBERTY_Device.GetStationMap(LIBERTY_GetSensorMask) )
                {
                    LIBERTY_Error("Cannot get sensor mask");
                    ok = FALSE;
                    break;
                }

                LIBERTY_messgf("LIBERTY: GetSensorMask=0x%04lX\n",LIBERTY_GetSensorMask);

                LIBERTY_SensorCount = 0;
                LIBERTY_SetSensorMask = 0L;

                for( i=0; (i < LIBERTY_SENSOR_MAX); i++ )
                {
                    if( SensorList[i] == 0 )
                    {
                        break;
                    }

                    LIBERTY_SetSensorMask |= (1 << (SensorList[i]-1));

                    LIBERTY_Posn[LIBERTY_SensorCount].dim(3,1);
                    LIBERTY_Ornt[LIBERTY_SensorCount].dim(3,1);
                    LIBERTY_SensorCount++;
                }

                if( !LIBERTY_Device.SetStationMap(LIBERTY_SetSensorMask) )
                {
                    LIBERTY_Error("Cannot set sensor mask");
                    ok = FALSE;
                    break;
                }

                LIBERTY_messgf("LIBERTY: SetSensorMask=0x%04lX\n",LIBERTY_SetSensorMask);
                break;

            case 6 : // Configure sensor output data list (default for all senors).
                // Append DISTortion LEVel data to frame (V1.3)
                LIBERTY_SensorDataList.Append(PDI_MODATA_DISTLEV);

                if( !LIBERTY_Device.SetSDataList(-1,LIBERTY_SensorDataList) )
                {
                    LIBERTY_Error("Cannot set sensor data list");
                    ok = FALSE;
                }
                break;

            case 7 : // Enable hemisphere-tracking.
                 // Vector of zeros to enable hemisphere-tracking.
                for( i=0; (i < 3); i++ )
                {
                    HV[i] = 0;
                }

                // Set hemisphere vector (for all sensors).
                if( !LIBERTY_Device.SetSHemisphere(-1,HV) )
                {
                    LIBERTY_Error("Cannot set hemisphere");
                    ok = FALSE;
                }
                break;

            case 8 : // Retrieve any bit errors from the tracker.
                if( !LIBERTY_Device.GetBITErrs(BE) )
                {
                    LIBERTY_Error("Cannot retrieve BIT errors");
                    ok = FALSE;
                    break;
                }

                // Are there any bit errors?
                if( !BE.IsClear() )
                {
                    BE.Parse(BEs,STRLEN);
                    LIBERTY_errorf("LIBERTY: %s\n",BEs);
                    ok = FALSE;
                }
                break;
        }

        dt = Latency.After();
        LIBERTY_messgf("LIBERTY: %s %s (%.3lf msec).\n",LIBERTY_OpenStateText[state],STR_OkFailed(ok),dt);
    }

    if( !ok )
    {
        LIBERTY_Close();
    }
    else
    {
        LIBERTY_OpenFlag = TRUE;
    }

    return(ok);
}

/******************************************************************************/

void LIBERTY_Close( void )
{
    // Disconnect only if a connection established.
    if( LIBERTY_Device.CnxReady() )
    {
        LIBERTY_Device.Disconnect();
    }

    LIBERTY_OpenFlag = FALSE;
}

/******************************************************************************/

BOOL LIBERTY_Started( void )
{
    if( !LIBERTY_OpenFlag )
    {
        return(FALSE);
    }

    return(LIBERTY_StartFlag);
}

/******************************************************************************/

BOOL LIBERTY_Start( void )
{
    if( !LIBERTY_OpenFlag )
    {
        return(FALSE);
    }

    if( LIBERTY_StartFlag )
    {
        return(TRUE);
    }

    if( !LIBERTY_Device.StartContPno(0) )
    {
        LIBERTY_Error("Start continous output");
        LIBERTY_Stop();
        return(FALSE);
    }

    LIBERTY_SensorDataCount = LIBERTY_SensorDataList.NumItems();
    LIBERTY_messgf("LIBERTY: SensorDataCount=%d\n",LIBERTY_SensorDataCount);

    LIBERTY_StartFlag = TRUE;
    LIBERTY_ErrorFlag = FALSE;
    LIBERTY_FrameCountLast = 0L;

    LIBERTY_LastFrameCountLatency.Reset();
    LIBERTY_LastFrameLatency.Reset();
    LIBERTY_GetFrameLatency.Reset();
    LIBERTY_FrameFrequency.Reset();

    return(TRUE);
}

/******************************************************************************/

void LIBERTY_Stop( void )
{
    if( !LIBERTY_OpenFlag )
    {
        return;
    }

    if( !LIBERTY_StartFlag )
    {
        return;
    }

    if( !LIBERTY_Device.StopContPno() )
    {
        LIBERTY_Error("Stop continous output");
    }

    LIBERTY_StartFlag = FALSE;
}

/******************************************************************************/

BOOL LIBERTY_GetFrame( BOOL &ready, DWORD &frame, matrix posn[], matrix ornt[] )
{
static int distlev[LIBERTY_SENSOR_MAX];
BOOL ok;

    ok = LIBERTY_GetFrame(ready,frame,posn,ornt,distlev);

    return(ok);
}

/******************************************************************************/

BOOL LIBERTY_GetFrame( BOOL &ready, DWORD &frame, matrix posn[], matrix ornt[], int distlev[] )
{
BYTE *buff,sn;
DWORD size;
float *fptr;
int b,s,i,j;
BOOL ok=TRUE,flag=FALSE;

    ready = FALSE;

    if( LIBERTY_ErrorFlag || !(LIBERTY_OpenFlag && LIBERTY_StartFlag) )
    {
        return(FALSE);
    }

    LIBERTY_GetFrameLatency.Before();

    for( s=0; (s < LIBERTY_SensorCount); s++ )
    {
        if( posn[s].isempty() )
        {
            posn[s].dim(3,1);
        }

        if( ornt[s].isempty() )
        {
            ornt[s].dim(3,1);
        }
    }

    // First, get frame count.
    LIBERTY_LastFrameCountLatency.Before();
    if( !LIBERTY_Device.LastHostFrameCount(LIBERTY_FrameCount) )
    {
        LIBERTY_Error("Last frame count");
        LIBERTY_ErrorFlag = TRUE;
        return(FALSE);
    }
    LIBERTY_LastFrameCountLatency.After();

    // Is a new frame ready?
    if( LIBERTY_FrameCount == LIBERTY_FrameCountLast )
    {
        return(TRUE);
    }

    LIBERTY_FrameCountLast = LIBERTY_FrameCount;

    LIBERTY_FrameFrequency.Loop();

    LIBERTY_LastFrameLatency.Before();
    if( !LIBERTY_Device.LastPnoPtr(buff,size) )
    {
        LIBERTY_Error("Last frame data");
        LIBERTY_ErrorFlag = TRUE;
        return(FALSE);
    }
    LIBERTY_LastFrameLatency.After();

    if( (buff == NULL) || (size == 0) )
    {
        LIBERTY_ErrorFlag = TRUE;
        return(FALSE);
    }

    ready = TRUE;
    frame = LIBERTY_FrameCount;
    //flag = TIMER_EveryHz(4.0);

    if( flag )
    {
        printf("frame=%ld size=%d\n",LIBERTY_FrameCount,size);
    }

    // Process data frame header...
    for( b=0,s=0; ((s < LIBERTY_SensorCount) && (b < size) && ok); s++ )
    {
        memcpy((void *)&LIBERTY_BINHDR,&buff[b],sizeof(BINHDR));
        b += sizeof(BINHDR);

        if( flag )
        {
            printf("s=%d station=%d, length=%d\n",s,LIBERTY_BINHDR.station,LIBERTY_BINHDR.length);
        }

        posn[s].dim(3,1);
        ornt[s].dim(3,1);

        for( j=0; (j < LIBERTY_SensorDataCount); j++ )
        {
            switch( LIBERTY_SensorDataList.ItemAt(j) )
            {
                case PDI_ODATA_SPACE :
                    b += 1;
                    break;

                case PDI_MODATA_CRLF :
                    b += 2;
                    break;

                case PDI_MODATA_FRAMECOUNT :
                    b += sizeof(DWORD);
                    break;

                case PDI_MODATA_POS : // Position
                    fptr = (float *)&buff[b];

                    for( i=0; (i < 3); i++ )
                    {
                        posn[s](i+1,1) = (double)(fptr[i]);
                        b += sizeof(float);
                    }

                    // Apply co-ordinate system rotation and offset.
                    posn[s] = LIBERTY_PositionRotation * posn[s];
                    posn[s] += LIBERTY_PositionOffset;
                    break;

                case PDI_MODATA_ORI : // Orientation (Euler)
                    fptr = (float *)&buff[b];

                    for( i=0; (i < 3); i++ )
                    {
                        ornt[s](i+1,1) = (double)(fptr[i]);
                        b += sizeof(float);
                    }
                    break;

                case PDI_MODATA_DISTLEV : // Distortion level (V1.3)
                    if( distlev != NULL )
                    {
                        distlev[s] = *((DWORD *)&buff[b]);
                    }

                    b += sizeof(DWORD);
                    break;

                default :
                    LIBERTY_errorf("LIBERTY_GetFrame(...) Unknown data type (%d)\n",LIBERTY_SensorDataList.ItemAt(j));
                    ok = FALSE;
            }
        }
    }

/*if( TIMER_EveryHz(4.0) && (distlev != NULL) )
{
    for( s=0; (s < LIBERTY_SensorCount); s++ )
    {
        printf("dislev[%d]=%d\n",s,distlev[s]);
    }
}*/

    if( !ok  )
    {
        LIBERTY_ErrorFlag = TRUE;
        return(FALSE);
    }

    LIBERTY_GetFrameLatency.After();

    return(TRUE);
}

/******************************************************************************/

BOOL LIBERTY_GetFrame( BOOL &ready, matrix posn[], matrix ornt[], int distlev[] )
{
static BOOL ok;
static DWORD frame;

    ok = LIBERTY_GetFrame(ready,frame,posn,ornt,distlev);

    return(ok);
}

/******************************************************************************/

BOOL LIBERTY_GetFrame( BOOL &ready, matrix posn[], matrix ornt[] )
{
static BOOL ok;
static DWORD frame;

    ok = LIBERTY_GetFrame(ready,frame,posn,ornt);

    return(ok);
}

/******************************************************************************/

BOOL LIBERTY_GetPosn( BOOL &ready, matrix posn[] )
{
BOOL ok;
int s;

    ok = LIBERTY_GetFrame(ready,posn,LIBERTY_Ornt);

    for( s=0; (s < LIBERTY_SensorCount); s++ )
    {
        posn[s] = LIBERTY_Posn[s];
    }

    return(ok);
}

/******************************************************************************/

BOOL LIBERTY_GetPosn( matrix posn[] )
{
BOOL ok,ready;

    ok = LIBERTY_GetPosn(ready,posn);

    return(ok);
}

/******************************************************************************/

void LIBERTY_TimingResults( void )
{
    LIBERTY_FrameFrequency.Results();
    LIBERTY_GetFrameLatency.Results();
    LIBERTY_LastFrameCountLatency.Results();
    LIBERTY_LastFrameLatency.Results();
}

/******************************************************************************/

BOOL *LIBERTY_SensorFlagList( int s1, ... )
{
static BOOL list[LIBERTY_SENSOR_MAX];
int     s;
BOOL    ok;
va_list argp;

    for( s=0; (s < LIBERTY_SENSOR_MAX); s++ )
    {
        list[s] = FALSE;
    }

    list[s1-1] = TRUE;

    va_start(argp,s1);
 
    while( TRUE )
    {
        s = va_arg(argp,int);
        if( s == 0 )
        {
            break;
        }
        
        list[s-1] = TRUE;
    }

    va_end(argp);

    return(list);
}

/******************************************************************************/

int LIBERTY_Sensors( void )
{
    return(LIBERTY_SensorCount);
}

/******************************************************************************/

