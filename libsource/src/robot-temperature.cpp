/******************************************************************************/
/* ROBOT: Temperature tracking for motors...                                */
/******************************************************************************/

double ROBOT_TempMotor( int ID, int motor )
{
    return(ROBOT_Item[ID].Robot->TempTrak->T_Core(motor));
}

/******************************************************************************/

void ROBOT_TempMotor( int ID, double T[] )
{
int axis;

    for( axis=0; (axis < ROBOT_AXIS); axis++ )
    {
        T[axis] = ROBOT_TempMotor(ID,axis);
    }
}

/******************************************************************************/

void    ROBOT_TempPrint( int ID, PRINTF prnf )
{
  (*prnf)("%s[%d] Temperature %5.1lf %5.1lf %5.1lf\n",
          ROBOT_Item[ID].name,ID,
          ROBOT_TempMotor(ID,ROBOT_AXIS_1),
          ROBOT_TempMotor(ID,ROBOT_AXIS_2),
          ROBOT_TempMotor(ID,ROBOT_AXIS_3));
}
  
/******************************************************************************/

BOOL    ROBOT_TempStart( int ID )
{
    if( !ROBOT_Item[ID].Robot->TempTrakStart() )
    {
        ROBOT_errorf("PHANOMT_Start(ID=%d) Cannot start TEMPTRAK.\n");
        return(FALSE);
    }

    ROBOT_debugf("ROBOT_TempStart(ID=%d);\n",ID);

    return(TRUE);
}

/******************************************************************************/

void    ROBOT_TempStop( int ID )
{
    if( !ROBOT_Item[ID].Robot->TempTrakStop() )
    {
        ROBOT_errorf("PHANOMT_Start(ID=%d) Cannot stop TEMPTRAK.\n");
    }

    ROBOT_debugf("ROBOT_TempStop(ID=%d);\n",ID);
}

/******************************************************************************/

BOOL    ROBOT_TempWait( int ID )
{
BOOL    Exit=FALSE,Cool=FALSE;

//  Check if motors are over-heated...
    if( !ROBOT_Item[ID].Robot->TempTrak->OverHeated() )
    {                                // Motor temperatures do not exceed suspend limit.
        return(TRUE);
    }

    ROBOT_messgf("ROBOT_TempWait(...) Motors to cooling... (ESCape to Abort)\n");

//  Wait for motors to cool...
    do
    {
        if( !TIMER_EveryHz(ROBOT_LoopTaskFrequency[ID]) )
        {
            continue;
        }

        Cool = ROBOT_Item[ID].Robot->TempTrak->Cooled();

        if( TIMER_EveryHz(1.0) )
        {
            ROBOT_TempPrint(ID,ROBOT_messgf);
        }

        Exit = KB_ESC(KB_NOWAIT);
    }
    while( !Exit && !Cool );

    if( Cool )
    {
        ROBOT_messgf("ROBOT_TempWait(...) Motors have cooled.\n");
    }
    else
    if( Exit )
    {
        ROBOT_errorf("ROBOT_TempWait(...) Aborted.\n");
    }

    return(Cool);
}

/******************************************************************************/

