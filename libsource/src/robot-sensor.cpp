/******************************************************************************/

BOOL ROBOT_SensorConfigured( int ID )
{
BOOL flag=FALSE;
int i;

    if( ROBOT_Check(ID) )
    {
        for( i=0; ((i < ROBOT_SENSOR_MAX) && !flag); i++ )
        {
            if( !STR_null(ROBOT_SensorName[ID][i]) )
            {
                flag = TRUE;
                break;
            }
        }
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorCheck( int ID )
{
BOOL flag=TRUE;

    if( !ROBOT_Check(ID) )
    {
        flag = FALSE;
    }
    else
    if( ROBOT_SensorConfigured(ID) )
    {
        if( ROBOT_Robot(ID)->Sensoray() == NULL )
        {
            flag = FALSE;  // DAQ F/T supported only on Sensoray interfaces...
        }
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpened( int ID, int item )
{
BOOL flag=TRUE;

    if( !ROBOT_SensorCheck(ID) )
    {
        flag = FALSE;
    }
    else
    if( ROBOT_Item[ID].SensorList[item].asensor == NULL )
    {
        flag = FALSE;
    }
    else
    if( !ROBOT_Item[ID].SensorList[item].asensor->Opened() )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpened( int ID )
{
BOOL flag;
int i;

    for( flag=FALSE,i=0; (i < ROBOT_SENSOR_MAX); i++ )
    {
        if( ROBOT_SensorOpened(ID,i) )
        {
            flag = TRUE;
        }
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpened_DAQFT( int ID )
{
BOOL flag=TRUE;

    if( !ROBOT_SensorCheck(ID) )
    {
        flag = FALSE;
    }
    else
    if( ROBOT_Item[ID].DAQFT == NULL )
    {
        flag = FALSE;
    }
    else
    if( !ROBOT_Item[ID].DAQFT->Opened() )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpened_Accelerometer( int ID )
{
BOOL flag=TRUE;

    if( !ROBOT_SensorCheck(ID) )
    {
        flag = FALSE;
    }
    else
    if( ROBOT_Item[ID].Accelerometer == NULL )
    {
        flag = FALSE;
    }
    else
    if( !ROBOT_Item[ID].Accelerometer->Opened() )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpened_PhotoTransistor( int ID )
{
BOOL flag=TRUE;

    if( !ROBOT_SensorCheck(ID) )
    {
        flag = FALSE;
    }
    else
    if( ROBOT_Item[ID].PhotoTransistor == NULL )
    {
        flag = FALSE;
    }
    else
    if( !ROBOT_Item[ID].PhotoTransistor->Opened() )
    {
        flag = FALSE;
    }

    return(flag);
}


/******************************************************************************/

BOOL ROBOT_SensorOpen( int ID )
{
int i,j,s,c;
BOOL ok=FALSE;
int DAQ,address;

    // First check basic sensor requirements...
    if( !ROBOT_SensorCheck(ID) )
    {
        return(FALSE);
    }

    for( ok=TRUE,s=0,c=0,i=0; ((i < ROBOT_SENSOR_MAX) && ok); i++ )
    {
        if( STR_null(ROBOT_SensorName[ID][i]) )
        {
            continue;
        }

        // Open DAQ F/T object...
        ROBOT_Item[ID].SensorList[i].asensor = new ASENSOR(ROBOT_SensorName[ID][i]);
        if( !ROBOT_Item[ID].SensorList[i].asensor->Open() )
        {
            ROBOT_errorf("ROBOT_SensorOpen(ID=%d) Cannot open sensor %s.\n",ID,ROBOT_SensorName[ID][i]);
            ok = FALSE;
            continue;
        }

        s++;

        ROBOT_Item[ID].SensorList[i].DeviceType = ROBOT_Item[ID].SensorList[i].asensor->Device(DAQ,address,ROBOT_Item[ID].SensorList[i].ChannelCount,ROBOT_Item[ID].SensorList[i].ChannelList);
        ROBOT_Item[ID].SensorList[i].ZRotation = ROBOT_SensorZRotation[ID][i];
        ROBOT_Item[ID].SensorList[i].ROMX = ROBOT_SensorROMX[ID][i];

        for( j=0; (j < ROBOT_Item[ID].SensorList[i].ChannelCount); j++ )
        {
            ROBOT_Item[ID].SensorList[i].ChannelIndex[j] = c;
            ROBOT_Item[ID].SensorChannelList[c] = ROBOT_Item[ID].SensorList[i].ChannelList[j];
            c++;
        }

        switch( ROBOT_Item[ID].SensorList[i].DeviceType )
        {
            case ASENSOR_DEVICE_DAQFT :
               ROBOT_Item[ID].DAQFT = ROBOT_Item[ID].SensorList[i].asensor;
               ROBOT_Item[ID].DAQFT_Sensor = i;
               break;

            case ASENSOR_DEVICE_ACCELEROMETER_XY :
               ROBOT_Item[ID].Accelerometer = ROBOT_Item[ID].SensorList[i].asensor;
               ROBOT_Item[ID].Accelerometer_Sensor = i;
               break;

            case ASENSOR_DEVICE_PHOTOTRANSISTOR :
               ROBOT_Item[ID].PhotoTransistor = ROBOT_Item[ID].SensorList[i].asensor;
               ROBOT_Item[ID].PhotoTransistor_Sensor = i;
               break;
        }
    }

    if( !ok )
    {
        ROBOT_SensorClose(ID);
        return(FALSE);
    }

    // No sensors to open.
    if( c == 0 )
    {
        return(TRUE);
    }

    ROBOT_Item[ID].SensorChannelList[c] = SENSORAY_ADC_EOL;
    ROBOT_Item[ID].SensorChannelCount = c;
        
    ok = ROBOT_Robot(ID)->Sensoray()->ADC_Setup10V(ROBOT_Item[ID].SensorChannelList);

    if( !ok )
    {
        ROBOT_errorf("ROBOT_SensorOpen(ID=%d) Sensoray setup ADC failed.\n",ID);
        ROBOT_SensorClose(ID);
        return(FALSE);
    }

    ROBOT_debugf("ROBOT_SensorOpen(ID=%d) Sensors=%d Channels=%d.\n",ID,s,c);

    return(TRUE);
}

/******************************************************************************/

void ROBOT_SensorClose( int ID )
{
int i;
BOOL ok;

    if( !ROBOT_SensorCheck(ID) )
    {
        return;
    }

    for( i=0; (i < ROBOT_SENSOR_MAX); i++ )
    {
        if( ROBOT_Item[ID].SensorList[i].asensor == NULL )
        {
            continue;
        }

        if( ROBOT_Item[ID].SensorList[i].asensor->Opened() )
        {
            if( ROBOT_Item[ID].SensorList[i].asensor->VoltDataInUse() )
            {
                printf("SENSOR: Saving VoltData...\n");
                ok = ROBOT_Item[ID].SensorList[i].asensor->VoltDataSave();
                printf("SENSOR: %s.\n",STR_OkFailed(ok));
            }

            ROBOT_Item[ID].SensorList[i].asensor->Close();
        }

        delete ROBOT_Item[ID].SensorList[i].asensor;
        ROBOT_Item[ID].SensorList[i].asensor = NULL;
    }
}

/******************************************************************************/

void ROBOT_SensorRead( int ID, double raw_volts[] )
{
double volts[SENSORAY_ADC_CHANNELS];
double sensor[ASENSOR_CHANNELS];
int i,j,k;

    // Are there any open sensors?
    if( !ROBOT_SensorOpened(ID) )
    {
        return;
    }

    for( i=0; (i < SENSORAY_ADC_CHANNELS); i++ )
    {
        volts[i] = 0.0;
    }

    ROBOT_Robot(ID)->Sensoray()->ADC_SampleVolts(volts);
    if( raw_volts != NULL )
    {
        for( i=0; (i < ROBOT_Item[ID].SensorChannelCount); i++ )
        {
            raw_volts[i] = volts[i];
        }
    }

    /*if( TIMER_EveryHz(8.0) )
    {
        for( i=0; (i < ROBOT_Item[ID].SensorChannelCount); i++ )
        {
            printf("[%d]=%.1lf ",i,volts[i]);
        }
        printf("\n");
    }*/

    for( i=0; (i < ROBOT_SENSOR_MAX); i++ )
    {
        if( !ROBOT_SensorOpened(ID,i) )
        {
            continue;
        }

        for( j=0; (j < ROBOT_Item[ID].SensorList[i].ChannelCount); j++ )
        {
            k = ROBOT_Item[ID].SensorList[i].ChannelIndex[j];
            sensor[j] = volts[k];
        }

        ROBOT_Item[ID].SensorList[i].asensor->SampleVolts(sensor);
    }
}

/******************************************************************************/

void ROBOT_SensorRead( int ID )
{
    ROBOT_SensorRead(ID,NULL);
}

/******************************************************************************/

BOOL ROBOT_Sensor_DAQFT( int ID, matrix &f, matrix &t )
{
static matrix angles(3,1);
static matrix R;
double Zrot;
int i;

    if( !ROBOT_SensorOpened_DAQFT(ID) )
    {
        return(FALSE);
    }

    ROBOT_Angles(ID,angles);
    ASENSOR_DAQFT_FT(ROBOT_Item[ID].DAQFT,f,t);
    i = ROBOT_Item[ID].DAQFT_Sensor;

    // Align forces and torques with rotation of end-point.
    //SPMX_romxZ(D2R(ROBOT_Item[ID].SensorList[i].ZRotation)-angles(3,1),R);

    Zrot = -D2R(ROBOT_Item[ID].SensorList[i].ZRotation)+angles(3,1);
    SPMX_romxZ(Zrot,R);

    /*if( TIMER_EveryHz(8.0) )
    {
        printf("ROBOT_Sensor_DAQFT(...) Zrot: %4.0f(deg)\n",R2D(Zrot));
    }*/

    //f = ROBOT_Item[ID].SensorList[i].ROMX * R * f;
    //t = ROBOT_Item[ID].SensorList[i].ROMX * R * t;

    f = R * ROBOT_Item[ID].SensorList[i].ROMX * f;
    t = R * ROBOT_Item[ID].SensorList[i].ROMX * t;

    return(TRUE);
}

/******************************************************************************/

BOOL ROBOT_Sensor_DAQFT( int ID, matrix &f )
{
matrix t;
BOOL flag;

    flag = ROBOT_Sensor_DAQFT(ID,f,t);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_Sensor_Accelerometer( int ID, matrix &a )
{
static matrix angles(3,1);
static matrix R;
static matrix XYZ(3,1);
static matrix nXYZ(3,1);
double Zrot;
int i;
static BOOL first=TRUE;

    if( !ROBOT_SensorOpened_Accelerometer(ID) )
    {
        return(FALSE);
    }

    ROBOT_Angles(ID,angles);
    ROBOT_Item[ID].Accelerometer->CurrentValues(a);
    i = ROBOT_Item[ID].Accelerometer_Sensor;

    XYZ(1,1) = a(1,1);
    XYZ(2,1) = a(2,1);
    XYZ(3,1) = 0.0;

    // Align acceleration vector.
    //SPMX_romxZ(D2R(ROBOT_Item[ID].SensorList[i].ZRotation)-angles(3,1),R);
    //Zrot = -D2R(ROBOT_Item[ID].SensorList[i].ZRotation)+angles(3,1);

    Zrot = D2R(ROBOT_Item[ID].SensorList[i].ZRotation) + angles(3,1);
    SPMX_romxZ(Zrot,R);

    XYZ = R * ROBOT_Item[ID].SensorList[i].ROMX * XYZ;

    a(1,1) = XYZ(1,1);
    a(2,1) = XYZ(2,1);

    return(TRUE);
}

/******************************************************************************/

BOOL ROBOT_Sensor_PhotoTransistor( int ID, double &luminance )
{
int i;

    if( !ROBOT_SensorOpened_PhotoTransistor(ID) )
    {
        return(FALSE);
    }

    luminance = ROBOT_Item[ID].PhotoTransistor->CurrentValue();

    return(TRUE);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasWait_DAQFT( int ID )
{
BOOL flag=FALSE;

    // Make sure sensor is installed & opened...
    if( !ROBOT_SensorOpened_DAQFT(ID) )
    {
        return(FALSE);
    }

    flag = ROBOT_Item[ID].DAQFT->BiasWait();

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasWait_Accelerometer( int ID )
{
BOOL flag=FALSE;

    // Make sure sensor is installed & opened...
    if( !ROBOT_SensorOpened_Accelerometer(ID) )
    {
        return(FALSE);
    }

    flag = ROBOT_Item[ID].Accelerometer->BiasWait();

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasReset( int ID, int item, int type, BOOL save )
{
BOOL ok=TRUE,done=FALSE;
TIMER TimeOut;

    // Make sure sensor is installed & opened...
    if( !ROBOT_SensorOpened(ID,item) )
    {
        return(FALSE);
    }

    ROBOT_Item[ID].SensorList[item].asensor->BiasReset(type);

    TimeOut.Reset();

    while( !done && !TimeOut.ExpiredSeconds(ASENSOR_BIAS_TIMEOUT) )
    {
        if( ROBOT_Item[ID].SensorList[item].asensor->BiasDone() )
        {
            done = TRUE;

            if( save )
            {
                ok = ROBOT_Item[ID].SensorList[item].asensor->BiasSave();
            }
        }
    }

    if( !done )
    {
        ROBOT_errorf("ROBOT_SensorBiasReset(ID=%d) %s Bias timeout.\n",ID,ROBOT_SensorName[ID][item]);
    }

    if( !ok )
    {
        ROBOT_errorf("ROBOT_SensorBiasReset(ID=%d) %s Bias save failed.\n",ID,ROBOT_SensorName[ID][item]);
    }

    return(done && ok);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasReset_DAQFT( int ID, int type, BOOL save )
{
BOOL flag=FALSE;
int item;

    if( ROBOT_SensorOpened_DAQFT(ID) )
    {
        item = ROBOT_Item[ID].DAQFT_Sensor;
        flag = ROBOT_SensorBiasReset(ID,item,type,save);
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasResetAnterograde_DAQFT( int ID )
{
BOOL flag=FALSE;

    flag = ROBOT_SensorBiasReset_DAQFT(ID,ASENSOR_BIAS_ANTEROGRADE,TRUE);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasResetRetrograde_DAQFT( int ID )
{
BOOL flag=FALSE;

    flag = ROBOT_SensorBiasReset_DAQFT(ID,ASENSOR_BIAS_RETROGRADE,FALSE);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasReset_DAQFT( int ID )
{
BOOL flag=FALSE;

    flag = ROBOT_SensorBiasResetAnterograde_DAQFT(ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasReset_Accelerometer( int ID, int type, BOOL save )
{
BOOL flag=FALSE;
int item;

    if( ROBOT_SensorOpened_Accelerometer(ID) )
    {
        item = ROBOT_Item[ID].Accelerometer_Sensor;
        flag = ROBOT_SensorBiasReset(ID,item,type,save);
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasResetAnterograde_Accelerometer( int ID )
{
BOOL flag=FALSE;

    flag = ROBOT_SensorBiasReset_Accelerometer(ID,ASENSOR_BIAS_ANTEROGRADE,TRUE);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasResetRetrograde_Accelerometer( int ID )
{
BOOL flag=FALSE;

    flag = ROBOT_SensorBiasReset_Accelerometer(ID,ASENSOR_BIAS_RETROGRADE,FALSE);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasReset_Accelerometer( int ID )
{
BOOL flag=FALSE;

    flag = ROBOT_SensorBiasResetAnterograde_Accelerometer(ID);

    return(flag);
}

/******************************************************************************/
/* Default handle functions.                                                  */
/******************************************************************************/

BOOL ROBOT_SensorConfigured( void )
{
BOOL flag;

    flag = ROBOT_SensorConfigured(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpened( void )
{
BOOL flag;

    flag = ROBOT_SensorOpened(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpened_DAQFT( void )
{
BOOL flag;

    flag = ROBOT_SensorOpened_DAQFT(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpened_Accelerometer( void )
{
BOOL flag;

    flag = ROBOT_SensorOpened_Accelerometer(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpened_PhotoTransistor( void )
{
BOOL flag;

    flag = ROBOT_SensorOpened_PhotoTransistor(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorOpen( void )
{
BOOL flag;

    flag = ROBOT_SensorOpen(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

void ROBOT_SensorClose( void )
{
    ROBOT_SensorClose(ROBOT_ID);
}

/******************************************************************************/

void ROBOT_SensorRead( double raw_volts[] )
{
    ROBOT_SensorRead(ROBOT_ID,raw_volts);
}

/******************************************************************************/

void ROBOT_SensorRead( void )
{
    ROBOT_SensorRead(ROBOT_ID);
}

/******************************************************************************/

BOOL ROBOT_Sensor_DAQFT( matrix &f, matrix &t )
{
BOOL flag;

    flag = ROBOT_Sensor_DAQFT(ROBOT_ID,f,t);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_Sensor_DAQFT( matrix &f )
{
BOOL flag;

    flag = ROBOT_Sensor_DAQFT(ROBOT_ID,f);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_Sensor_Accelerometer( matrix &a )
{
BOOL flag;

    flag = ROBOT_Sensor_Accelerometer(ROBOT_ID,a);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_Sensor_PhotoTransistor( double &luminance )
{
BOOL flag;

    flag = ROBOT_Sensor_PhotoTransistor(ROBOT_ID,luminance);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasWait_DAQFT( void )
{
BOOL flag;

    flag = ROBOT_SensorBiasWait_DAQFT(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasWait_Accelerometer( void )
{
BOOL flag;

    flag = ROBOT_SensorBiasWait_DAQFT(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasResetAnterograde_DAQFT( void )
{
BOOL flag;

    flag = ROBOT_SensorBiasResetAnterograde_DAQFT(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasResetRetrograde_DAQFT( void )
{
BOOL flag;

    flag = ROBOT_SensorBiasResetRetrograde_DAQFT(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasReset_DAQFT( void )
{
BOOL flag;

    flag = ROBOT_SensorBiasReset_DAQFT(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasResetAnterograde_Accelerometer( void )
{
BOOL flag;

    flag = ROBOT_SensorBiasResetAnterograde_Accelerometer(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasResetRetrograde_Accelerometer( void )
{
BOOL flag;

    flag = ROBOT_SensorBiasResetRetrograde_Accelerometer(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_SensorBiasReset_Accelerometer( void )
{
BOOL flag;

    flag = ROBOT_SensorBiasReset_Accelerometer(ROBOT_ID);

    return(flag);
}

/******************************************************************************/
/* Force end-point forces into joint torques using current Jacobian.          */
/******************************************************************************/

void ROBOT_Forces2MotorTorques( int ID, matrix &f, matrix &t )
{
double xyz[AXIS_XYZ];
double api[AXIS_XYZ];
matrix F(AXIS_XYZ,1);

    // Put force matrix into Application xyz array.
    SPMX_mtx2xyz(f,xyz);

    // Map from Application xyz to API xyz.
    ROBOT_XYZ2API(ID,xyz,api);

    // Put API xyz force array back into matrix.
    SPMX_xyz2mtx(api,F);

    // Torques (Nm) are jacobian transpose * forces.
    if( ROBOT_JT[ID].isnotempty() )
    {
        t = ROBOT_JT[ID] * F;
    }
}

/******************************************************************************/

void ROBOT_Forces2MotorTorques( matrix &f, matrix &t )
{
    ROBOT_Forces2MotorTorques(ROBOT_ID,f,t);
}

/******************************************************************************/

