/******************************************************************************/
/* ROBOT: Functions to interconvert various units.                          */
/******************************************************************************/

double ROBOT_ConvEU2Rad( int ID, int encoder, double EU )
{
double Rad;

    Rad = ROBOT_EU2Rad[ID][encoder] * EU;
	//0.000045472
	//printf("ROBOT_radiu :%G, %G, %G\n",ROBOT_EU2Rad[ID][0],ROBOT_EU2Rad[ID][1],ROBOT_EU2Rad[ID][2]);
    return(Rad);
}

/******************************************************************************/

double ROBOT_ConvEU2Rad( int ID, int encoder, long EU )
{
    return(ROBOT_ConvEU2Rad(ID,encoder,(double)EU));
}

/******************************************************************************/

long ROBOT_ConvNm2MU( int ID, int motor, double Nm )
{
long MU;

    MU = (long)(ROBOT_Nm2MU[ID][motor] * Nm);

    return(MU);
}

/******************************************************************************/

double ROBOT_ConvMU2Nm( int ID, int motor, long MU )
{
double Nm;

    Nm = (double)MU / ROBOT_Nm2MU[ID][motor];

    return(Nm);
}

/******************************************************************************/

double ROBOT_MapAPI2XYZ( int ID, int axis, double api[] )
{
double value=0.0;

    if( ROBOT_XYZIndex[ID][axis] != -1 )
    {
        value = api[ROBOT_XYZIndex[ID][axis]];
    }

    return(value);
}

/******************************************************************************/

void ROBOT_MapXYZ2API( int ID, int axis, double xyz, double api[] )
{
    if( ROBOT_XYZIndex[ID][axis] != -1 )
    {
        api[ROBOT_XYZIndex[ID][axis]] = xyz;
    }
}

/******************************************************************************/

void ROBOT_MapAPI2XYZ( int ID, double api[], double xyz[] )
{
int axis;

    for( axis=AXIS_X; (axis <= AXIS_Z); axis++ )
    {
        xyz[axis] = ROBOT_MapAPI2XYZ(ID,axis,api);
    }
}

/******************************************************************************/

void ROBOT_MapXYZ2API( int ID, double xyz[], double api[] )
{
int axis;

    for( axis=AXIS_X; (axis <= AXIS_Z); axis++ )
    {
        api[axis] = 0.0;
    }

    for( axis=AXIS_X; (axis <= AXIS_Z); axis++ )
    {
        ROBOT_MapXYZ2API(ID,axis,xyz[axis],api);
    } 
}

/******************************************************************************/

void ROBOT_API2XYZ( int ID, double api[], double xyz[], double offset[] )
{
int axis;

    // Convert API xyz to Applicaiton xyz...
    ROBOT_MapAPI2XYZ(ID,api,xyz);

    for( axis=AXIS_X; (axis <= AXIS_Z); axis++ )
    {
        // Configuration flip sign of each axis...
        xyz[axis] *= ROBOT_signXYZ[ID][axis];

        // Apply an optional offset.
        if( offset != NULL )
        {
            xyz[axis] += offset[axis];
        }
    }

    // Apply RTMX for user-defined co-ordinate frame...
    SPMX_doRT(ROBOT_RTMX[ID],xyz);
}

/******************************************************************************/

void ROBOT_API2XYZ( int ID, double api[], double xyz[] )
{
    ROBOT_API2XYZ(ID,api,xyz,NULL);
}

/******************************************************************************/

void ROBOT_XYZ2API( int ID, double xyz[], double api[], double offset[] )
{
int axis;

    // Take off ROMX for user-defined co-ordinate frame...
    SPMX_doR(ROBOT_invROMX[ID],xyz);

    // Configuration flip sign of each axis...
    for( axis=AXIS_X; (axis <= AXIS_Z); axis++ )
    {
        xyz[axis] *= ROBOT_signXYZ[ID][axis];

        // Apply an optional offset.
        if( offset != NULL )
        {
            xyz[axis] -= offset[axis];
        }

    }

    // Convert Application xyz to API xyz...
    ROBOT_MapXYZ2API(ID,xyz,api);
}

/******************************************************************************/

void ROBOT_XYZ2API( int ID, double xyz[], double api[] )
{
    ROBOT_XYZ2API(ID,xyz,api,NULL);
}

/******************************************************************************/


