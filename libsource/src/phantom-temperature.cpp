/******************************************************************************/
/* PHANTOM: Temperature tracking for motors...                                */
/******************************************************************************/

double PHANTOM_TempMotor( int ID, int motor )
{
    return(PHANTOM_Item[ID].Robot->TempTrak->T_Core(motor));
}

/******************************************************************************/

void PHANTOM_TempMotor( int ID, double T[] )
{
int axis;

    for( axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        T[axis] = PHANTOM_TempMotor(ID,axis);
    }
}

/******************************************************************************/

void    PHANTOM_TempPrint( int ID, PRINTF prnf )
{
  (*prnf)("%s[%d] Temperature %5.1lf %5.1lf %5.1lf\n",
          PHANTOM_Item[ID].name,ID,
          PHANTOM_TempMotor(ID,PHANTOM_AXIS_1),
          PHANTOM_TempMotor(ID,PHANTOM_AXIS_2),
          PHANTOM_TempMotor(ID,PHANTOM_AXIS_3));
}
  
/******************************************************************************/

BOOL    PHANTOM_TempStart( int ID )
{
    if( !PHANTOM_Item[ID].Robot->TempTrakStart() )
    {
        PHANTOM_errorf("PHANOMT_Start(ID=%d) Cannot start TEMPTRAK.\n");
        return(FALSE);
    }

    PHANTOM_debugf("PHANTOM_TempStart(ID=%d);\n",ID);

    return(TRUE);
}

/******************************************************************************/

void    PHANTOM_TempStop( int ID )
{
    if( !PHANTOM_Item[ID].Robot->TempTrakStop() )
    {
        PHANTOM_errorf("PHANOMT_Start(ID=%d) Cannot stop TEMPTRAK.\n");
    }

    PHANTOM_debugf("PHANTOM_TempStop(ID=%d);\n",ID);
}

/******************************************************************************/

BOOL    PHANTOM_TempWait( int ID )
{
BOOL    Exit=FALSE,Cool=FALSE;

//  Check if motors are over-heated...
    if( !PHANTOM_Item[ID].Robot->TempTrak->OverHeated() )
    {                                // Motor temperatures do not exceed suspend limit.
        return(TRUE);
    }

    PHANTOM_messgf("PHANTOM_TempWait(...) Motors to cooling... (ESCape to Abort)\n");

//  Wait for motors to cool...
    do
    {
        if( !TIMER_EveryHz(PHANTOM_LoopTaskFreq[ID]) )
        {
            continue;
        }

        Cool = PHANTOM_Item[ID].Robot->TempTrak->Cooled();

        if( TIMER_EveryHz(1.0) )
        {
            PHANTOM_TempPrint(ID,PHANTOM_messgf);
        }

        Exit = KB_ESC(KB_NOWAIT);
    }
    while( !Exit && !Cool );

    if( Cool )
    {
        PHANTOM_messgf("PHANTOM_TempWait(...) Motors have cooled.\n");
    }
    else
    if( Exit )
    {
        PHANTOM_errorf("PHANTOM_TempWait(...) Aborted.\n");
    }

    return(Cool);
}

/******************************************************************************/

