/******************************************************************************/

BOOL PHANTOM_FT_Installed( int ID )
{
BOOL flag=TRUE;

    if( !PHANTOM_Check(ID) )
    {
        flag = FALSE;
    }
    else  
    if( STR_null(PHANTOM_FT_Name[ID]) )
    {
        flag = FALSE;
    }
    else
    if( PHANTOM_Robot(ID)->Sensoray() == NULL )
    {
        flag = FALSE;  // DAQ F/T supported only on Sensoray interfaces...
    }
    else  
    if( !PHANTOM_2D(ID) )
    {
        flag = FALSE;  // DAQ F/T supported only on 2D Robot...
    }

    return(flag);
}

/******************************************************************************/

BOOL PHANTOM_FT_Opened( int ID )
{
BOOL flag=TRUE;

    if( !PHANTOM_FT_Installed(ID) )
    {
        flag = FALSE;
    }
    else
    if( PHANTOM_Item[ID].FT == NULL )
    {
        flag = FALSE;
    }
    else
    if( !PHANTOM_Item[ID].FT->Opened() )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL PHANTOM_FT_Open( int ID )
{
int ADC[DAQFT_GAUGES+1],i;
BOOL ok=FALSE;

    // Check if DAQ F/T istalled...
    if( !PHANTOM_FT_Installed(ID) )
    {
        return(FALSE);
    }

    for( i=0; (i < DAQFT_GAUGES); i++ )
    {
        ADC[i] = PHANTOM_FT_ADC[ID][i];
    }

    ADC[i] = SENSORAY_ADC_EOL;

    // Open DAQ F/T object...
    PHANTOM_Item[ID].FT = new DAQFT(PHANTOM_FT_Name[ID],-1,-1,10000);

    if( PHANTOM_Item[ID].FT->Opened() )
    {
        ok = PHANTOM_Robot(ID)->Sensoray()->ADC_Setup10V(ADC);
    }

    if( !ok )
    {
        PHANTOM_errorf("PHANTOM_FT_Open(ID=%d) Cannot open F/T sensor: %s.\n",ID,PHANTOM_FT_Name[ID]);
        delete PHANTOM_Item[ID].FT;
        PHANTOM_Item[ID].FT = NULL;

        return(FALSE);
    }

    return(TRUE);
}

/******************************************************************************/

void PHANTOM_FT_Close( int ID )
{
int i;
DATA **g;

    // Make sure F/T sensor is installed & opened...
    if( !PHANTOM_FT_Opened(ID) )
    {
        return;
    }

    g = PHANTOM_Item[ID].FT->Data();

    for( i=0; (i < DAQFT_GAUGES); i++ )
    {
        if( g[i] != NULL )
        {
            ok = g[i]->Save();
        }
    }

    PHANTOM_Item[ID].FT->Close();

    delete PHANTOM_Item[ID].FT;
    PHANTOM_Item[ID].FT = NULL;
}

/******************************************************************************/

void PHANTOM_FT_Read( int ID, matrix &f, matrix &t )
{
double gauges[DAQFT_GAUGES];
matrix angles(3,1);

    // Make sure F/T sensor is installed & opened...
    if( !PHANTOM_FT_Opened(ID) )
    {
        return;
    }

    PHANTOM_Angles(ID,angles);
    PHANTOM_Robot(ID)->Sensoray()->ADC_SampleVolts(gauges);
    PHANTOM_Item[ID].FT->Convert(gauges,&f,&t);

    // Align forces and torques with rotation of arm...
    f = SPMX_romxZ(D2R(PHANTOM_FT_ZRotation[ID])+angles(2,1)) * f;
    t = SPMX_romxZ(D2R(PHANTOM_FT_ZRotation[ID])+angles(2,1)) * t;
}

/******************************************************************************/

void PHANTOM_FT_BiasReset( int ID )
{
    // Make sure F/T sensor is installed & opened...
    if( !PHANTOM_FT_Opened(ID) )
    {
        return;
    }

    PHANTOM_Item[ID].FT->BiasReset();
}

/******************************************************************************/

BOOL PHANTOM_FT_BiasWait( int ID )
{
BOOL flag=FALSE;

    // Make sure F/T sensor is installed & opened...
    if( !PHANTOM_FT_Opened(ID) )
    {
        return(flag);
    }

    flag = PHANTOM_Item[ID].FT->BiasWait();

    return(flag);
}

/******************************************************************************/

BOOL PHANTOM_FT_Installed( void )
{
BOOL flag;

    flag = PHANTOM_FT_Installed(PHANTOM_ID);

    return(flag);
}

/******************************************************************************/

BOOL PHANTOM_FT_Opened( void )
{
BOOL flag;

    flag = PHANTOM_FT_Opened(PHANTOM_ID);

    return(flag);
}

/******************************************************************************/

BOOL PHANTOM_FT_Open( void )
{
BOOL flag;

    flag = PHANTOM_FT_Open(PHANTOM_ID);

    return(flag);
}

/******************************************************************************/

void PHANTOM_FT_Close( void )
{
    PHANTOM_FT_Close(PHANTOM_ID);
}

/******************************************************************************/

void PHANTOM_FT_Read( matrix &f, matrix &t )
{
    PHANTOM_FT_Read(PHANTOM_ID,f,t);
}

/******************************************************************************/

void PHANTOM_FT_BiasReset( void )
{
    PHANTOM_FT_BiasReset(PHANTOM_ID);
}

/******************************************************************************/

BOOL PHANTOM_FT_BiasWait( void )
{
BOOL flag;

    flag = PHANTOM_FT_BiasWait(PHANTOM_ID);

    return(flag);
}

/******************************************************************************/

