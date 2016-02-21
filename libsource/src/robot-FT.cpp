/******************************************************************************/

BOOL ROBOT_DAQ_Check( int ID )
{
BOOL flag=TRUE;

    if( !ROBOT_Check(ID) )
    {
        flag = FALSE;
    }
    else
    if( ROBOT_Robot(ID)->Sensoray() == NULL )
    {
        flag = FALSE;  // DAQ F/T supported only on Sensoray interfaces...
    }
    else  
    if( !ROBOT_2D(ID) )
    {
        flag = FALSE;  // DAQ F/T supported only on 2D Robot...
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_DAQ_DAQFT_Opened( int ID )
{
BOOL flag=TRUE;

    if( !ROBOT_DAQ_Check(ID) )
    {
        flag = FALSE;
    }
    else
    if( ROBOT_Item[ID].DAQ_DAQFT == NULL )
    {
        flag = FALSE;
    }
    else
    if( !ROBOT_Item[ID].DAQ_DAQFT->Opened() )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_DAQ_ACCXY_Opened( int ID )
{
BOOL flag=TRUE;

    if( !ROBOT_DAQ_Check(ID) )
    {
        flag = FALSE;
    }
    else
    if( ROBOT_Item[ID].DAQ_ACCXY == NULL )
    {
        flag = FALSE;
    }
    else
    if( !ROBOT_Item[ID].DAQ_ACCXY->Opened() )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_DAQ_Open( int ID )
{
int i,j,c;
BOOL ok=FALSE;
DAQFT *ptr=NULL;
int device,DAQ,address,chancount,chanlist[DAQFT_CHANNELS];

    // Check if DAQ istalled...
    if( !ROBOT_DAQ_Check(ID) )
    {
        return(FALSE);
    }

    for( ok=TRUE,c=0,i=0; ((i < ROBOT_DAQFT) && ok); i++ )
    {
        if( STR_null(ROBOT_FT_Name[ID][i]) )
        {
            continue;
        }

        // Open DAQ F/T object...
        ptr = new DAQFT(ROBOT_FT_Name[ID][i]);
        if( !ptr->Open() )
        {
            ROBOT_errorf("ROBOT_FT_Open(ID=%d) Cannot open DAQ sensor %s.\n",ID,ROBOT_FT_Name[ID][i]);
            ok = FALSE;
            continue;
        }

        device = ptr->Device(DAQ,address,chancount,chanlist);

        switch( device )
        {
            case DAQFT_DEVICE_DAQFT :
               ROBOT_Item[ID].DAQ_DAQFT = ptr;
               ROBOT_Item[ID].DAQ_DAQFT_ChannelCount = chancount;
               ROBOT_Item[ID].DAQ_DAQFT_ZRotation = ROBOT_FT_ZRotation[ID][i];
               ROBOT_Item[ID].DAQ_DAQFT_ROMX = ROBOT_FT_ROMX[ID][i];

               for( j=0; (j < chancount); j++ )
               {
                   ROBOT_Item[ID].DAQ_DAQFT_ChannelList[j] = c;
                   ROBOT_Item[ID].DAQ_ChannelList[c] = chanlist[j];
                   c++;
               }
               break;

            case DAQFT_DEVICE_ACCXY :
               ROBOT_Item[ID].DAQ_ACCXY = ptr;
               ROBOT_Item[ID].DAQ_ACCXY_ChannelCount = chancount;
               ROBOT_Item[ID].DAQ_ACCXY_ZRotation = ROBOT_FT_ZRotation[ID][i];
               ROBOT_Item[ID].DAQ_ACCXY_ROMX = ROBOT_FT_ROMX[ID][i];

               for( j=0; (j < chancount); j++ )
               {
                   ROBOT_Item[ID].DAQ_ACCXY_ChannelList[j] = c;
                   ROBOT_Item[ID].DAQ_ChannelList[c] = chanlist[j];
                   c++;
               }
               break;
        }
    }

    if( !ok )
    {
        ROBOT_DAQ_Close(ID);
    }

    ROBOT_Item[ID].DAQ_ChannelList[c++] = SENSORAY_ADC_EOL;
    ROBOT_Item[ID].DAQ_ChannelCount = c;
        
    ok = ROBOT_Robot(ID)->Sensoray()->ADC_Setup10V(ROBOT_Item[ID].DAQ_ChannelList);

    if( !ok )
    {
        ROBOT_errorf("ROBOT_FT_Open(ID=%d) Sensoray setup ADC failed.\n",ID);
        ROBOT_DAQ_Close();

        return(FALSE);
    }

    return(TRUE);
}

/******************************************************************************/

void ROBOT_DAQ_Close( int ID )
{
int i;
DATAPROC **g;
BOOL ok;

    if( !ROBOT_DAQ_Check(ID) )
    {
        return;
    }

    if( ROBOT_Item[ID].DAQ_DAQFT != NULL )
    {
        if( ROBOT_Item[ID].DAQ_DAQFT->Opened() )
        {
            g = ROBOT_Item[ID].DAQ_DAQFT->Data();

            for( i=0; (i < ROBOT_Item[ID].DAQ_DAQFT_ChannelCount); i++ )
            {
                if( g[i] != NULL )
                {
                    ok = g[i]->Save();
                    printf("%s %s.\n",g[i]->File(),STR_OkFailed(ok));
                }
            }

            ROBOT_Item[ID].DAQ_DAQFT->Close();
        }

        delete ROBOT_Item[ID].DAQ_DAQFT;
        ROBOT_Item[ID].DAQ_DAQFT = NULL;
    }

    if( ROBOT_Item[ID].DAQ_ACCXY != NULL )
    {
        if( ROBOT_Item[ID].DAQ_ACCXY->Opened() )
        {
            g = ROBOT_Item[ID].DAQ_ACCXY->Data();

            for( i=0; (i < ROBOT_Item[ID].DAQ_ACCXY_ChannelCount); i++ )
            {
                if( g[i] != NULL )
                {
                    ok = g[i]->Save();
                    printf("%s %s.\n",g[i]->File(),STR_OkFailed(ok));
                }
            }

            ROBOT_Item[ID].DAQ_ACCXY->Close();
        }

        delete ROBOT_Item[ID].DAQ_ACCXY;
        ROBOT_Item[ID].DAQ_ACCXY = NULL;
    }
}

/******************************************************************************/

void ROBOT_DAQ_Read( int ID, matrix &f, matrix &t, matrix &a )
{
double volts[SENSORAY_ADC_CHANNELS];
double sensor[DAQFT_CHANNELS];
static matrix angles(3,1);
static matrix R;
int i,j;

    // Make sure F/T sensor is installed & opened...
    if( !(ROBOT_DAQ_DAQFT_Opened(ID) || ROBOT_DAQ_ACCXY_Opened(ID)) )
    {
        return;
    }

    for( i=0; (i < SENSORAY_ADC_CHANNELS); i++ )
    {
        volts[i] = 0.0;
    }

    ROBOT_Angles(ID,angles);
    ROBOT_Robot(ID)->Sensoray()->ADC_SampleVolts(volts);

    if( ROBOT_DAQ_DAQFT_Opened(ID) )
    {
        for( i=0; (i < ROBOT_Item[ID].DAQ_DAQFT_ChannelCount); i++ )
        {
            j = ROBOT_Item[ID].DAQ_DAQFT_ChannelList[i];
            sensor[i] = volts[j];
        }

        ROBOT_Item[ID].DAQ_DAQFT->ConvertFT(sensor,f,t);

        // Align forces and torques with rotation of arm...
        SPMX_romxZ(D2R(ROBOT_Item[ID].DAQ_DAQFT_ZRotation)-angles(3,1),R);
        f = ROBOT_Item[ID].DAQ_DAQFT_ROMX * R * f;
        t = ROBOT_Item[ID].DAQ_DAQFT_ROMX * R * t;
    }

    if( ROBOT_DAQ_ACCXY_Opened(ID) )
    {
        for( i=0; (i < ROBOT_Item[ID].DAQ_ACCXY_ChannelCount); i++ )
        {
            j = ROBOT_Item[ID].DAQ_ACCXY_ChannelList[i];
            sensor[i] = volts[j];
        }

        ROBOT_Item[ID].DAQ_ACCXY->ConvertXY(sensor,a);
    }
}

/******************************************************************************/

void ROBOT_DAQ_Read( int ID, matrix &f, matrix &a )
{
matrix t;

    ROBOT_DAQ_Read(ID,f,t,a);
}

/******************************************************************************/

BOOL ROBOT_DAQ_DAQFT_Read( int ID, matrix &f, matrix &t )
{
matrix a;

    if( !ROBOT_DAQ_DAQFT_Opened(ID) )
    {
        return(FALSE);
    }

    ROBOT_DAQ_Read(ID,f,t,a);

    return(TRUE);
} 

/******************************************************************************/

BOOL ROBOT_DAQ_ACCXY_Read( int ID, matrix &a )
{
matrix f,t;

    if( !ROBOT_DAQ_ACCXY_Opened(ID) )
    {
        return(FALSE);
    }

    ROBOT_DAQ_Read(ID,f,t,a);

    return(TRUE);
} 

/******************************************************************************/

void ROBOT_FT_MotorTorques( int ID, matrix &f, matrix &t )
{
double xyz[AXIS_XYZ];
double api[AXIS_XYZ];
matrix F(AXIS_XYZ,1);

    // Make sure F/T sensor is installed & opened...
    if( !ROBOT_DAQ_DAQFT_Opened(ID) )
    {
        return;
    }

    // Put force matrix into Application xyz array...
    SPMX_mtx2xyz(f,xyz);

    // Map from Application xyz to API xyz...
    ROBOT_XYZ2API(ID,xyz,api);

    // Put API xyz force array back into matrix...
    SPMX_xyz2mtx(api,F);

    // Torques (Nm) are jacobian transpose * forces...
    if( ROBOT_JT[ID].isnotempty() )
    {
        t = ROBOT_JT[ID] * F;
    }
}

/******************************************************************************/

BOOL ROBOT_DAQ_DAQFT_BiasWait( int ID )
{
BOOL flag=FALSE;

    // Make sure F/T sensor is installed & opened...
    if( !ROBOT_DAQ_DAQFT_Opened(ID) )
    {
        return(FALSE);
    }

    flag = ROBOT_Item[ID].DAQ_DAQFT->BiasWait();

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_DAQ_ACCXY_BiasWait( int ID )
{
BOOL flag=FALSE;

    // Make sure F/T sensor is installed & opened...
    if( !ROBOT_DAQ_ACCXY_Opened(ID) )
    {
        return(FALSE);
    }

    flag = ROBOT_Item[ID].DAQ_ACCXY->BiasWait();

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_DAQ_DAQFT_BiasReset( int ID )
{
BOOL done=FALSE;
TIMER TimeOut;

    // Make sure F/T sensor is installed & opened...
    if( !ROBOT_DAQ_DAQFT_Opened(ID) )
    {
        return(FALSE);
    }

    ROBOT_Item[ID].DAQ_DAQFT->BiasReset();

    TimeOut.Reset();

    while( !done && !TimeOut.ExpiredSeconds(DAQFT_BIAS_TIMEOUT) )
    {
        if( ROBOT_Item[ID].DAQ_DAQFT->BiasDone() )
        {
            done = ROBOT_Item[ID].DAQ_DAQFT->BiasSave();
            break;
        }
    }

    return(done);
}

/******************************************************************************/

BOOL ROBOT_DAQ_ACCXY_BiasReset( int ID )
{
BOOL done=FALSE;
TIMER TimeOut;

    // Make sure F/T sensor is installed & opened...
    if( !ROBOT_DAQ_ACCXY_Opened(ID) )
    {
        return(FALSE);
    }

    ROBOT_Item[ID].DAQ_ACCXY->BiasReset();

    TimeOut.Reset();

    while( !done && !TimeOut.ExpiredSeconds(DAQFT_BIAS_TIMEOUT) )
    {
        if( ROBOT_Item[ID].DAQ_ACCXY->BiasDone() )
        {
            done = ROBOT_Item[ID].DAQ_ACCXY->BiasSave();
            break;
        }
    }

    return(done);
}

/******************************************************************************/

BOOL ROBOT_DAQ_DAQFT_Opened( void )
{
BOOL flag;

    flag = ROBOT_DAQ_DAQFT_Opened(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_DAQ_ACCXY_Opened( void )
{
BOOL flag;

    flag = ROBOT_DAQ_ACCXY_Opened(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_DAQ_Open( void )
{
BOOL flag;

    flag = ROBOT_DAQ_Open(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

void ROBOT_DAQ_Close( void )
{
    ROBOT_DAQ_Close(ROBOT_ID);
}

/******************************************************************************/

void ROBOT_DAQ_Read( matrix &f, matrix &t, matrix &a )
{
    ROBOT_DAQ_Read(ROBOT_ID,f,t,a);
}

/******************************************************************************/

void ROBOT_DAQ_Read( matrix &f, matrix &a )
{
    ROBOT_DAQ_Read(ROBOT_ID,f,a);
}

/******************************************************************************/

BOOL ROBOT_DAQ_DAQFT_Read( matrix &f, matrix &t )
{
    return(ROBOT_DAQ_DAQFT_Read(ROBOT_ID,f,t));
} 

/******************************************************************************/

BOOL ROBOT_DAQ_ACCXY_Read( matrix &a )
{
    return(ROBOT_DAQ_ACCXY_Read(ROBOT_ID,a));
}

/******************************************************************************/

void ROBOT_FT_Torques( matrix &f, matrix &t )
{
    ROBOT_FT_Torques(ROBOT_ID,f,t);
}

/******************************************************************************/

BOOL ROBOT_DAQ_DAQFT_BiasWait( void )
{
BOOL flag;

    flag = ROBOT_DAQ_DAQFT_BiasWait(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_DAQ_ACCXY_BiasWait( void )
{
BOOL flag;

    flag = ROBOT_DAQ_ACCXY_BiasWait(ROBOT_ID);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_DAQ_DAQFT_BiasReset( void )
{
BOOL ok;

    ok = ROBOT_DAQ_DAQFT_BiasReset(ROBOT_ID);

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_DAQ_ACCXY_BiasReset( void )
{
BOOL ok;

    ok = ROBOT_DAQ_ACCXY_BiasReset(ROBOT_ID);

    return(ok);
}

/******************************************************************************/

