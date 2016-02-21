/******************************************************************************/
/* ROBOT: Force ramp functions.                                             */
/******************************************************************************/

BOOL    ROBOT_RampFlag0( void )    { return(ROBOT_RampFlag(0));    }
BOOL    ROBOT_RampFlag1( void )    { return(ROBOT_RampFlag(1));    }
BOOL    ROBOT_RampFlag2( void )    { return(ROBOT_RampFlag(2));    }

BOOL  (*ROBOT_RampFunc[])( void ) = { ROBOT_RampFlag0,ROBOT_RampFlag1,ROBOT_RampFlag2 };

/******************************************************************************/

double ROBOT_RampValue( int ID )
{
double value=0.0;

    if( ROBOT_Check(ID) )
    {
        value = ROBOT_Item[ID].Robot->Ramp->RampCurrent();
    }

    return(value);
}

/******************************************************************************/
  
void ROBOT_RampZero( int ID )
{
    if( ROBOT_Check(ID) )
    {
        ROBOT_Item[ID].Robot->Ramp->Reset();
    }
}

/******************************************************************************/

long ROBOT_RampMU( int ID, long MU )
{
long ramped=0L;

    if( ROBOT_Check(ID) )
    {
        ramped = ROBOT_Item[ID].Robot->Ramp->RampLong(MU);
    }

    return(ramped);    
}

/******************************************************************************/

BOOL    ROBOT_RampFlag( int ID )
{
BOOL    flag=TRUE;

    if( !ROBOT_Check(ID) )
    {
        flag = FALSE;
    }
    else
    if( ROBOT_Panic(ID) )                 // Panic switch activated, so zero ramp...
    {
        flag = FALSE;
    }
    else
    if( ROBOT_Cooling(ID) )               // Motors cooling down, so zero ramp...
    {
        flag = FALSE;
    }
    else
    {
        flag = ROBOT_Item[ID].Robot->Controller->Activated();
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_Ramped( int ID )
{
BOOL flag=TRUE;

    if( !ROBOT_Check(ID) )
    {
        flag = FALSE;
    }
    else
    {
        flag = ROBOT_Item[ID].Robot->Ramp->RampComplete();
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_RampTimeSet( int ID, double ramptime )
{
BOOL flag=FALSE;

    if( ROBOT_Check(ID) )
    {
        if( !ROBOT_Started(ID) )
        {
            if( ramptime > 0.0 )
            {
                ROBOT_Item[ID].Robot->Ramp->RampTimeSet(ramptime);
                ROBOT_RampTime[ID] = ramptime;
                flag = TRUE;
            }
        }
    }

    if( !flag )
    {
        ROBOT_errorf("Application RampTimeSet(ID=%d,ramptime=%.2lfsec) %s.\n",ID,STR_OkFailed(flag));
    }

    return(flag);
}

/******************************************************************************/

