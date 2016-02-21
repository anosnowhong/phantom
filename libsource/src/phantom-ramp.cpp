/******************************************************************************/
/* PHANTOM: Force ramp functions.                                             */
/******************************************************************************/

BOOL    PHANTOM_RampFlag0( void )    { return(PHANTOM_RampFlag(0));    }
BOOL    PHANTOM_RampFlag1( void )    { return(PHANTOM_RampFlag(1));    }
BOOL    PHANTOM_RampFlag2( void )    { return(PHANTOM_RampFlag(2));    }

BOOL  (*PHANTOM_RampFunc[])( void ) = { PHANTOM_RampFlag0,PHANTOM_RampFlag1,PHANTOM_RampFlag2 };

/******************************************************************************/

double PHANTOM_RampValue( int ID )
{
double value=0.0;

    if( PHANTOM_Check(ID) )
    {
        value = PHANTOM_Item[ID].Robot->RampValue();
    }

    return(value);
}

/******************************************************************************/
  
void PHANTOM_RampZero( int ID )
{
    if( PHANTOM_Check(ID) )
    {
        PHANTOM_Item[ID].Robot->RampReset();
    }
}

/******************************************************************************/

long PHANTOM_RampMU( int ID, long MU )
{
long ramped=0L;

    if( PHANTOM_Check(ID) )
    {
        ramped = PHANTOM_Item[ID].Robot->RampLong(MU);
    }

    return(ramped);    
}

/******************************************************************************/

BOOL    PHANTOM_RampFlag( int ID )
{
BOOL    flag=TRUE;

    if( !PHANTOM_Check(ID) )
    {
        flag = FALSE;
    }
    else
    if( PHANTOM_Panic(ID) )                 // Panic switch activated, so zero ramp...
    {
        flag = FALSE;
    }
    else
    if( PHANTOM_Cooling(ID) )               // Motors cooling down, so zero ramp...
    {
        flag = FALSE;
    }
    else
    {
        flag = PHANTOM_Item[ID].Robot->Controller->Running();
    }

    return(flag);
}

/******************************************************************************/

BOOL PHANTOM_Ramped( int ID )
{
BOOL flag=TRUE;

    if( PHANTOM_Check(ID) )
    {
        flag = FALSE;
    }
    else
    {
        flag = PHANTOM_Item[ID].Robot->RampDone();
    }

    return(flag);
}

/******************************************************************************/

