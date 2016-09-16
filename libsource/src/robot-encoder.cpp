/******************************************************************************/
/* ROBOT: Encoder functions...                                                */
/******************************************************************************/

void ROBOT_EncoderRead( int ID, int axis )
{
double xraw=0.0,x=0.0,dx=0.0,ddx=0.0;

    // If handle is valid, read encoder via ROBOT object...
    if( ROBOT_Check(ID) )
    {
        // Physically read the encoder.
        ROBOT_Item[ID].Robot->EncoderRead(axis,xraw,x,dx);

        ROBOT_Item[ID].Axis[axis].Encoder.count_xraw = xraw;
        ROBOT_Item[ID].Axis[axis].Encoder.count_x = x;
        ROBOT_Item[ID].Axis[axis].Encoder.count_dx = dx;
        ROBOT_Item[ID].Axis[axis].Encoder.count_ddx = ddx;
    }
}

/******************************************************************************/

void ROBOT_EncoderLoop( int ID )
{
int axis,index;

    // Read encoder values for all axes...
    for( axis=0; (axis < ROBOT_AXIS); axis++ )
    {
        // Read encoder if this axis is used...
        if( (index=ROBOT_AxisIndex[ID][axis]) != -1 )
        {
            ROBOT_EncoderRead(ID,index);
        }
    }
}

/******************************************************************************/

BOOL ROBOT_EncoderReset( int ID, int axis )
{
BOOL ok=FALSE;
double xraw,x,dx,ddx;

    // If handle is valid, reset encoder via ROBOT object...
    if( ROBOT_Check(ID) )
    {
        ROBOT_Item[ID].Robot->EncoderReset(axis);
        ROBOT_Item[ID].Robot->EncoderRead(axis,xraw,x,dx);

        ok = (xraw == 0.0);
    }

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_EncoderReset( int ID )
{
int axis,index;
BOOL ok;

    if( !ROBOT_Check(ID) )
    {
        return(FALSE);
    }

    // Reset encoders for all axes...
    for( ok=TRUE,axis=0; (axis < ROBOT_AXIS); axis++ )
    {
        // Reset encoder if this axis is used...
        if( (index=ROBOT_AxisIndex[ID][axis]) != -1 )
        {
            if( !ROBOT_EncoderReset(ID,index) )
            {
                ok = FALSE;
            }
        }
    }

    if( ok )
    {
        // Set reset flag in robot calibration file.
        ROBOT_reset[ID] = TRUE;
        ok = ROBOT_CnfgSave(ID,ROBOT_CNFG_CAL);
    }

    return(ok);
}

/******************************************************************************/

void ROBOT_Encoder( int ID, int axis, double &xraw, double &x, double &dx, double &ddx )
{
    if( !ROBOT_Check(ID) )
    {
        return;
    }

    xraw = ROBOT_Item[ID].Axis[axis].Encoder.count_xraw;
    x = ROBOT_Item[ID].Axis[axis].Encoder.count_x;
    dx = ROBOT_Item[ID].Axis[axis].Encoder.count_dx;
    ddx = ROBOT_Item[ID].Axis[axis].Encoder.count_ddx;
}

/******************************************************************************/

void ROBOT_Encoder( int ID, double xraw[], double x[], double dx[], double ddx[] )
{
int axis,index;

    if( !ROBOT_Check(ID) )
    {
        return;
    }

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        // Read encoder if this axis is used...
        if( (index=ROBOT_AxisIndex[ID][axis]) != -1 )
        {
            ROBOT_Encoder(ID,index,xraw[axis],x[axis],dx[axis],ddx[axis]);
        }
    } 
}

/******************************************************************************/

void ROBOT_Encoder( int ID, double xraw[] )
{
int axis,index;
double x[ROBOT_DOFS],dx[ROBOT_DOFS],ddx[ROBOT_DOFS];

    if( !ROBOT_Check(ID) )
    {
        return;
    }

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        // Read encoder if this axis is used...
        if( (index=ROBOT_AxisIndex[ID][axis]) != -1 )
        {
            ROBOT_Encoder(ID,index,xraw[axis],x[axis],dx[axis],ddx[axis]);
        }
    }
}

/******************************************************************************/

