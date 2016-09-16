/******************************************************************************/
/*                                                                            */
/* MODULE  : ASENSOR.cpp                                                      */
/*                                                                            */
/* PURPOSE : Analog Sensor API.                                               */
/*                                                                            */
/* DATE    : 23/Nov/2004                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 24/Nov/2004 - Initial development (for ATI DAQ F/T sensors).     */
/*                                                                            */
/* V2.0  JNI 26/Mar/2008 - Renamed & generalized to "ASENSOR" (from "DAQFT"). */
/*                                                                            */
/* V2.1  JNI 08/Dec/2008 - Added PhotoTransistor sensor type.                 */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "ASENSOR"
#define MODULE_TEXT     "Analog Sensor API"
#define MODULE_DATE     "08/12/2008"
#define MODULE_VERSION  "2.1"
#define MODULE_LEVEL    3

/******************************************************************************/

#include "motor.h"

/******************************************************************************/

BOOL ASENSOR_API_started = FALSE;

/******************************************************************************/

PRINTF  ASENSOR_PRN_messgf=NULL;         // General messages printf function.
PRINTF  ASENSOR_PRN_errorf=NULL;         // Error messages printf function.
PRINTF  ASENSOR_PRN_debugf=NULL;         // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int ASENSOR_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ASENSOR_PRN_messgf,buff));
}

/******************************************************************************/

int ASENSOR_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ASENSOR_PRN_errorf,buff));
}

/******************************************************************************/

int ASENSOR_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ASENSOR_PRN_debugf,buff));
}

/******************************************************************************/

BOOL ASENSOR_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL ok=TRUE;

    if( ASENSOR_API_started )               // Start the API once...
    {
        return(TRUE);
    }

    ASENSOR_PRN_messgf = messgf;            // General API message print function.
    ASENSOR_PRN_errorf = errorf;            // API error message print function.
    ASENSOR_PRN_debugf = debugf;            // Debug information print function.

    if( ok )
    {
        ATEXIT_API(ASENSOR_API_stop);       // Install stop function.
        ASENSOR_API_started = TRUE;         // Set started flag.

        MODULE_start(ASENSOR_PRN_messgf);   // Register module.
    }
    else
    {
        ASENSOR_errorf("ASENSOR_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void ASENSOR_API_stop( void )
{
    if( !ASENSOR_API_started )      // API not started in the first place...
    {
         return;
    }

    ASENSOR_API_started = FALSE;    // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL ASENSOR_API_check( void )
{
BOOL ok=TRUE;

    if( !ASENSOR_API_started )      // API not started...
    {                               // Start module automatically...
        ok = ASENSOR_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        ASENSOR_debugf("ASENSOR_API_check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/******************************************************************************/

ASENSOR::ASENSOR(  )
{
    Init();
}

/******************************************************************************/

ASENSOR::ASENSOR( char *name, int window, int bias, int vdata )
{
    Init(name,window,bias,vdata);
}

/******************************************************************************/

ASENSOR::ASENSOR( char *name, int window, int bias )
{
    Init(name,window,bias,0);
}

/******************************************************************************/

ASENSOR::ASENSOR( char *name )
{
    Init(name,0,0,0);
}

/******************************************************************************/

ASENSOR::~ASENSOR(  )
{
    Close();
}

/******************************************************************************/

void ASENSOR::Init( void )
{
int i;

    // Make sure API started...
    if( !ASENSOR_API_check() )
    {
        return;
    }

    memset(ObjectName,0,STRLEN);

    OpenFlag = FALSE;

    Window = NULL;
    WindowSize = 0;
    BiasPoints = 0;
    BiasCurrent = 0;
    BiasState = ASENSOR_BIAS_CLEAR;
    memset(BiasFile,0,STRLEN);
    memset(BiasHistoryFile,0,STRLEN);
    VoltPoints = 0;

    for( i=0; (i < ASENSOR_CHANNELS); i++ )
    {
        BiasData[i] = NULL;
        VoltData[i] = NULL;
    }
}

/******************************************************************************/

void ASENSOR::Init( char *name, int window, int bias, int voltdata )
{
    Init();

    strncpy(ObjectName,name,STRLEN);
    WindowSize = window;
    BiasPoints = (bias > 0) ? bias : ASENSOR_BIAS_POINTS;
    VoltPoints = voltdata;
    BiasCurrent = BiasPoints;
}

/******************************************************************************/

struct  STR_TextItem    ASENSOR_DeviceText[] =
{
    { ASENSOR_DEVICE_DAQFT,"DAQFT" },
    { ASENSOR_DEVICE_ACCELEROMETER_XY,"ACCELEROMETER-XY" },
    { ASENSOR_DEVICE_PHOTOTRANSISTOR,"PHOTOTRANSISTOR" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem    ASENSOR_DAQ_Text[] =
{
    { ASENSOR_DAQ_NIDAQ,"NIDAQ" },
    { ASENSOR_DAQ_SENSORAY,"Sensoray" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

STRING ASENSOR_DeviceType;
STRING ASENSOR_DAQ_Type;
int    ASENSOR_DAQ_Address;
int    ASENSOR_DAQ_Channels[ASENSOR_CHANNELS];
BOOL   ASENSOR_CalibrationFlags[ASENSOR_CALIBRATION_FLAGS];

/******************************************************************************/

void ASENSOR::ConfigSetup( void )
{
int i;

    for( i=0; (i < ASENSOR_CHANNELS); ASENSOR_DAQ_Channels[i++]=-1 );
    for( i=0; (i < ASENSOR_CALIBRATION_FLAGS); ASENSOR_CalibrationFlags[i++]=FALSE );

    CONFIG_reset();

    CONFIG_set("Device",ASENSOR_DeviceType);
    CONFIG_set("DAQ",ASENSOR_DAQ_Type);
    CONFIG_set("Address",ASENSOR_DAQ_Address);
    CONFIG_set("Channels",ASENSOR_DAQ_Channels,ASENSOR_CHANNELS);
    CONFIG_setBOOL("Calibration",ASENSOR_CalibrationFlags,ASENSOR_CALIBRATION_FLAGS);
}

/******************************************************************************/

BOOL ASENSOR::ConfigLoad( char *file )
{
int i;

    ConfigSetup();

    if( !CONFIG_read(file) )
    {
        ASENSOR_errorf("ASENSOR: Cannot load configuration file: %s\n",file);
        return(FALSE);
    }

    if( (DeviceType=STR_TextCode(ASENSOR_DeviceText,ASENSOR_DeviceType)) == STR_NOTFOUND )
    {
        ASENSOR_errorf("ASENSOR: Invalid device type (%s) in configuration file: %s\n",file,ASENSOR_DeviceType);
        return(FALSE);
    }

    if( (DAQ_Type=STR_TextCode(ASENSOR_DAQ_Text,ASENSOR_DAQ_Type)) == STR_NOTFOUND )
    {
        ASENSOR_errorf("ASENSOR: Invalid DAQ type (%s) in configuration file: %s\n",file,ASENSOR_DAQ_Type);
        return(FALSE);
    }

    DAQ_Address = ASENSOR_DAQ_Address;
    DAQ_ChannelCount = 0;

    for( i=0; ((i < ASENSOR_CHANNELS) && (ASENSOR_DAQ_Channels[i] != -1)); i++ )
    {
        DAQ_Channels[i] = ASENSOR_DAQ_Channels[i];
        DAQ_ChannelCount++;
    }

    for( i=0; (i < ASENSOR_CALIBRATION_FLAGS); i++ )
    {
        CalibrationFlags[i] = ASENSOR_CalibrationFlags[i];
    }
    
    return(TRUE);
}

/******************************************************************************/

char *ASENSOR_OpenText[] = { "Configuration","Calibration","Gain","Bias","Variables",NULL };

/******************************************************************************/

BOOL ASENSOR::Open( BOOL reset )
{
BOOL ok;
STRING file;
char *path;
int step,i;

    if( STR_null(ObjectName) )
    {
        ASENSOR_errorf("ASENSOR::Open() Sensor name not set.\n");
        return(FALSE);
    }

    for( ok=TRUE,step=ASENSOR_OPEN_CONFIGURATION; ((step <= ASENSOR_OPEN_VARIABLES) && ok); step++ )
    {
        switch( step )
        {
            case ASENSOR_OPEN_CONFIGURATION :
               strncpy(file,STR_stringf("%s.CFG",ObjectName),STRLEN);

               if( (path=FILE_Calibration(file)) == NULL )
               {
                   ASENSOR_errorf("ASENSOR::Open() Cannot find configuration file: %s\n",file);
                   ok = FALSE;
                   break;
               }

               strncpy(file,path,STRLEN);
               ok = ConfigLoad(file);
               break;

            case ASENSOR_OPEN_CALIBRATION :
               // Default identity calibration matrix for sensor.
               Calibration = I(DAQ_ChannelCount);

               if( !CalibrationFlags[ASENSOR_CALIBRATION_MTX] )
               {
                   break;
               }

               strncpy(file,STR_stringf("%s.MTX",ObjectName),STRLEN);
               if( (path=FILE_Calibration(file)) == NULL )
               {
                   ASENSOR_errorf("ASENSOR::Open() Cannot find calibration file: %s\n",file);
                   ok = FALSE;
                   break;
               }

               strncpy(file,path,STRLEN);
               if( !matrix_read(file,Calibration) )
               {
                   ASENSOR_errorf("ASENSOR::Open() Cannot load calibration file: %s\n",file);
                   ok = FALSE;
                   break;
               }

               // For some reason the ATI DAQ F/T sensor calibration matrix must be transposed.
               if( DeviceType == ASENSOR_DEVICE_DAQFT )
               {
                   Calibration.transpose();
               }
               break;

            case ASENSOR_OPEN_GAIN :
               Gain.dim(1,DAQ_ChannelCount);
               Gain.ones();

               if( !CalibrationFlags[ASENSOR_CALIBRATION_GAIN] )
               {
                   break;
               }

               strncpy(file,STR_stringf("%s.K",ObjectName),STRLEN);
               if( (path=FILE_Calibration(file)) == NULL )
               {
                   ASENSOR_errorf("ASENSOR::Open() Cannot find gain file: %s\n",file);
                   ok = FALSE;
                   break;
               }

               strncpy(file,path,STRLEN);
               if( !matrix_read(file,Gain) )
               {
                   ASENSOR_errorf("ASENSOR::Open() Cannot load gain file: %s\n",file);
                   ok = FALSE;
                   break;
               }

               MakeColumnVector(Gain);

               // For some reason the ATI DAQ F/T sensor gains must be inverted.
               if( DeviceType == ASENSOR_DEVICE_DAQFT )
               {
                   for( i=1; (i <= DAQ_ChannelCount); i++ )
                   {
                       if( Gain(1,i) != 0.0 )
                       {
                           Gain(1,i) = 1.0 / Gain(1,i);
                       }
                   }
               }
               break;

            case ASENSOR_OPEN_BIAS :
               BiasVolts.dim(1,DAQ_ChannelCount);
               BiasVolts.zeros();

               if( !CalibrationFlags[ASENSOR_CALIBRATION_BIAS] )
               {
                   break;
               }

               strncpy(file,STR_stringf("%s.B",ObjectName),STRLEN);

               if( (path=FILE_Calibration(file)) == NULL )
               {
                   ASENSOR_errorf("ASENSOR::Open() Cannot find bias file: %s\n",file);
                   ok = FALSE;
                   break;
               }

               strncpy(file,path,STRLEN);
               if( !matrix_read(file,BiasVolts) )
               {
                   ASENSOR_errorf("ASENSOR::Open() Cannot load bias file: %s\n",file);
                   ok = FALSE;
                   break;
               }

               strncpy(BiasFile,file,STRLEN);
               strncpy(BiasHistoryFile,STR_stringf("%sH",file),STRLEN);
               MakeColumnVector(BiasVolts);
               //disp(BiasVolts);
               break;

            case ASENSOR_OPEN_VARIABLES :
               SensorVolts.dim(1,DAQ_ChannelCount);
               SensorValues.dim(1,DAQ_ChannelCount);

               for( i=0; (i < DAQ_ChannelCount); i++ )
               {
                   BiasData[i] = new DATAPROC(STR_stringf("%s[%i]",ObjectName,i),BiasPoints,DATAPROC_FLAG_CIRCULAR);

                   if( VoltPoints > 0 )
                   {
                       VoltData[i] = new DATAPROC(STR_stringf("%s[%i]",ObjectName,i),VoltPoints,DATAPROC_FLAG_CIRCULAR);
                   }
               }

               if( WindowSize > 0 )
               {
                   Window = new WINFIT(ObjectName,WindowSize,DAQ_ChannelCount);
               }
               break;
        }

        STR_printf(ok,ASENSOR_debugf,ASENSOR_errorf,"ASENSOR::Open() %s[%d] %s.\n",ASENSOR_OpenText[step],step,STR_OkFailed(ok));
    }

    if( ok )
    {
        OpenFlag = TRUE;

        if( reset )
        {
            BiasResetAnterograde();
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL ASENSOR::Open( void )
{
BOOL ok;

    ok = Open(FALSE);

    return(ok);
}

/******************************************************************************/

void ASENSOR::Close( void )
{
int i;

    if( Window != NULL )
    {
        delete Window;
        Window = NULL;
    }

    for( i=0; (i < DAQ_ChannelCount); i++ )
    {
        if( BiasData[i] != NULL )
        {
            delete BiasData[i];
            BiasData[i] = NULL;
        }

        if( VoltData[i] != NULL )
        {
            delete VoltData[i];
            VoltData[i] = NULL;
        }
    }

    OpenFlag = FALSE;
}

/******************************************************************************/

BOOL ASENSOR::Opened( void )
{
    return(OpenFlag);
}

/******************************************************************************/

int ASENSOR::Device( int &DAQ, int &addr, int &chancount, int chanlist[] )
{
int c;

    DAQ = DAQ_Type;
    addr = DAQ_Address;
    chancount = DAQ_ChannelCount;

    for( c=0; (c < DAQ_ChannelCount); c++ )
    {
        chanlist[c] = DAQ_Channels[c];
    }

    return(DeviceType);
}

/******************************************************************************/

void ASENSOR::Calculate( void )
{
    SensorValues.zeros();

    if( !BiasWait() )
    {
        SensorValues = ((SensorVolts-BiasVolts) * Calibration) ^ Gain;
    }
}

/******************************************************************************/

void ASENSOR::VoltsDoubleToColumnVector( double v[], matrix &V )
{
    V.dim(1,DAQ_ChannelCount);
    matrix_double(v,V);
}

/******************************************************************************/

void ASENSOR::MakeRowVector( matrix &M )
{
    if( M.cols() > 1 )
    {
        M.transpose();
    }
}

/******************************************************************************/

void ASENSOR::MakeColumnVector( matrix &M )
{
    if( M.rows() > 1 )
    {
        M.transpose();
    }
}

/******************************************************************************/

void ASENSOR::BiasReset( int type )
{
int i;

    if( !CalibrationFlags[ASENSOR_CALIBRATION_BIAS] )
    {
        return;
    }

    if( BiasState == ASENSOR_BIAS_PENDING )
    {
        return;
    }

    switch( type )
    {
         case ASENSOR_BIAS_ANTEROGRADE : 
            // Start collecting sensor bias voltages from now.
            BiasCurrent = 0;
            BiasState = ASENSOR_BIAS_PENDING;

            for( i=0; (i < DAQ_ChannelCount); i++ )
            {
                BiasData[i]->Reset();
            }
            break;

         case ASENSOR_BIAS_RETROGRADE :
            if( BiasCurrent < BiasPoints )
            {
                BiasState = ASENSOR_BIAS_PENDING;
                break;
            }

            BiasState = ASENSOR_BIAS_DONE;
            BiasSet();
            break;
    }
}

/******************************************************************************/

void ASENSOR::BiasReset( void )
{
    BiasReset(ASENSOR_BIAS_ANTEROGRADE);
}

/******************************************************************************/

void ASENSOR::BiasResetAnterograde( void )
{
    BiasReset(ASENSOR_BIAS_ANTEROGRADE);
}

/******************************************************************************/

void ASENSOR::BiasResetRetrograde( void )
{
    BiasReset(ASENSOR_BIAS_RETROGRADE);
}

/******************************************************************************/

BOOL ASENSOR::BiasWait( void )
{
BOOL flag=FALSE;

    if( (BiasCurrent < BiasPoints) && (BiasState == ASENSOR_BIAS_PENDING) )
    {
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

void ASENSOR::BiasSample( matrix &volts )
{
int i;

    // If we've collected enough points, set the bias.
    if( (BiasCurrent >= BiasPoints) && (BiasState == ASENSOR_BIAS_PENDING) )
    {
        BiasState = ASENSOR_BIAS_DONE;
        BiasSet();
    }

    // Add data point for each gauge.
    for( i=0; (i < DAQ_ChannelCount); i++ )
    {
        BiasData[i]->Data(volts(1,i+1));
    }

    // Another bias point collected.
    BiasCurrent++;
}

/******************************************************************************/

void ASENSOR::BiasSample( double volts[] )
{
matrix V;

    VoltsDoubleToColumnVector(volts,V);
    BiasSample(V);
}

/******************************************************************************/

void ASENSOR::BiasSet( void )
{
int i;
matrix V;

    V.dim(1,DAQ_ChannelCount);

    // Calculate mean for each gauge.
    for( i=0; (i < DAQ_ChannelCount); i++ )
    {
        V(1,i+1) = BiasData[i]->Mean();
    }

    // Set bias with mean values.
    BiasSet(V);
}

/******************************************************************************/

void ASENSOR::BiasSet( matrix &volts )
{
    BiasVolts = volts;
}

/******************************************************************************/

void ASENSOR::BiasSet( double volts[] )
{
matrix V;

    VoltsDoubleToColumnVector(volts,V);
    BiasSet(V);
}

/******************************************************************************/

BOOL ASENSOR::BiasDone( void )
{
BOOL flag=FALSE;

    if( (BiasState == ASENSOR_BIAS_DONE) || !CalibrationFlags[ASENSOR_CALIBRATION_BIAS] )
    {
        BiasState = ASENSOR_BIAS_CLEAR;
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

BOOL ASENSOR::BiasSave( void )
{
BOOL ok;
matrix B,H;
time_t t;
int i;

    if( !CalibrationFlags[ASENSOR_CALIBRATION_BIAS] )
    {
        return(TRUE);
    }

    //disp(BiasVolts);
    TIMER_delay(10.0);
    B.duplicate(BiasVolts);
    MakeRowVector(B);

    //printf("ASENSOR::BiasSave()\n");
    //disp(B);

    ok = matrix_write(BiasFile,B);
    STR_printf(ok,ASENSOR_debugf,ASENSOR_errorf,"ASENSOR::BiasSave() %s %s.\n",BiasFile,STR_OkFailed(ok));

    if( ok )
    {
        time(&t);
        H.dim(1,DAQ_ChannelCount+1);
        H(1,1) = (double)t;
        for( i=0; (i < DAQ_ChannelCount); i++ )
        {
            H(1,i+2) = B(i+1,1);
        }

        ok = matrix_append(BiasHistoryFile,H);
        STR_printf(ok,ASENSOR_debugf,ASENSOR_errorf,"ASENSOR::BiasSave() %s %s.\n",BiasHistoryFile,STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

void ASENSOR::SampleVolts( matrix &volts )
{
int i;
matrix V;

    V = volts;
    MakeColumnVector(V);

    // Record sensor volts in the bias buffer.
    BiasSample(V);

    // Sensor volts are filtered by a sliding mean window (default size 1).
    if( Window == NULL )
    {
        SensorVolts = V;
    }
    else
    {
        Window->Point(T(V));
        Window->Mean(SensorVolts);
    }

    // Calculate sensor values from volts.
    Calculate();

    // Save raw sensor volts if required.
    for( i=0; (i < DAQ_ChannelCount); i++ )
    {
        if( VoltData[i] != NULL )
        {
            VoltData[i]->Data(V(1,i+1));
        }
    }
}

/******************************************************************************/

void ASENSOR::SampleVolts( double volts[] )
{
matrix V;

    VoltsDoubleToColumnVector(volts,V);
    SampleVolts(V);
}

/******************************************************************************/

void ASENSOR::ConvertValues( double volts[], matrix &values )
{
matrix V;

    VoltsDoubleToColumnVector(volts,V);
    SampleVolts(V);
    CurrentValues(values);
}

/******************************************************************************/

void ASENSOR::CurrentValues( matrix &values )
{
    values = SensorValues;
    MakeRowVector(values);
}

/******************************************************************************/

double ASENSOR::ConvertValue( double volts )
{
    SampleVolts(scalar(volts));
    return(SensorValues(1,1));
}

/******************************************************************************/

double ASENSOR::CurrentValue( void )
{
    return(SensorValues(1,1));
}

/******************************************************************************/

BOOL ASENSOR::VoltDataInUse( void )
{
BOOL flag;
int i;

    for( flag=FALSE,i=0; ((i < DAQ_ChannelCount) && !flag); i++ )
    {
        if( VoltData[i] != NULL )
        {
            flag = TRUE;
        }
    }

    return(flag);
}

/******************************************************************************/

BOOL ASENSOR::VoltDataSave( void )
{
BOOL ok;
int i;

    for( ok=TRUE,i=0; ((i < DAQ_ChannelCount) && ok); i++ )
    {
        if( VoltData[i] != NULL )
        {
            ok = VoltData[i]->Save();
            STR_printf(ok,ASENSOR_debugf,ASENSOR_errorf,"ASENSOR::VoltDataSave() %s %s.\n",VoltData[i]->File(),STR_OkFailed(ok));
        }
    }

    return(ok);
}

/******************************************************************************/

void ASENSOR_DAQFT_FT( ASENSOR *asensor, double volts[], matrix &f, matrix &t )
{
matrix V;

    asensor->VoltsDoubleToColumnVector(volts,V);
    asensor->SampleVolts(V);
    ASENSOR_DAQFT_FT(asensor,f,t);
}

/******************************************************************************/

void ASENSOR_DAQFT_FT( ASENSOR *asensor, matrix &f, matrix &t )
{
matrix D;

    asensor->CurrentValues(D);
    
    matrix_dim(f,AXIS_XYZ,1);
    f(1,1) = D(1,1);
    f(2,1) = D(2,1);
    f(3,1) = D(3,1);

    matrix_dim(t,AXIS_XYZ,1);
    t(1,1) = D(4,1);
    t(2,1) = D(5,1);
    t(3,1) = D(6,1);
}

/******************************************************************************/

void ASENSOR_DAQFT_Forces( ASENSOR *asensor, double volts[], matrix &f )
{
matrix t;

    ASENSOR_DAQFT_FT(asensor,volts,f,t);
}

/******************************************************************************/

void ASENSOR_DAQFT_Forces( ASENSOR *asensor, matrix &f )
{
matrix t;

    ASENSOR_DAQFT_FT(asensor,f,t);
}

/******************************************************************************/


