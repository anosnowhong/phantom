/******************************************************************************/
/* ROBOT: Force (Torque) output functions.                                    */
/******************************************************************************/

void ROBOT_MotorSetNm( int ID, int axis, double Nm )
{
long MU;
int index;

    // If axis not is use, do nothing...
    if( (index=ROBOT_AxisIndex[ID][axis]) == -1 )
    {
        return;
    }

    if( UndefinedDouble(Nm) )
    {
        ROBOT_errorf("Robot=%s Undefined torque value, axis=%d.\n",ROBOT_KeyWord[ID],axis);

        // Set robot unsafe flag.
        ROBOT_UnSafe(ID);
        return;
    }

    ROBOT_MotorNm[ID][index] = Nm;

    // Convert torque to DAC units...
    MU = ROBOT_ConvNm2MU(ID,index,Nm);

    // Output DAC units to motor...
    ROBOT_MotorSetMU(ID,axis,MU);
}

/******************************************************************************/

void ROBOT_MotorSetMU( int ID, int axis, long MU )
{
int index;
long sign=0;

    // If ROBOT not started, output zero torque (V2.6)...
    if( !ROBOT_Started(ID) )
    {
        MU = 0;
    }

    // If ROBOT not in safe state, output zero torque.
    if( !ROBOT_Item[ID].safe )
    {
        MU = 0;
    }

    // If axis not is use, do nothing...
    if( (index=ROBOT_AxisIndex[ID][axis]) == -1 )
    {
        return;
    }

    // This is the actual torque output of the motor in Motor Units...
    ROBOT_MotorMU[ID][index] = ROBOT_RampMU(ID,MU) * ROBOT_signM[ID][index];

    // Apply torque calibration gains and offsets...
    ROBOT_MotorMU[ID][index] = (long)((ROBOT_TorqueGain[ID][index] * (double)ROBOT_MotorMU[ID][index]) + ROBOT_TorqueOffset[ID][index]);

    // Check for maximum Motor Unit output only if motors engaged...
    if( !ROBOT_Panic(ID) )
    {
        // Clamp value to Maximum Motor Unit output...
        if( labs(ROBOT_MotorMU[ID][index]) > ROBOT_MaxMU[ID][index] )
        {
            sign = ROBOT_MotorMU[ID][index] / labs(ROBOT_MotorMU[ID][index]);
            ROBOT_MotorMU[ID][index] = ROBOT_MaxMU[ID][index] * sign;
        }
    }

    // Output value to controller...
    ROBOT_Item[ID].Robot->MotorSet(index,ROBOT_MotorMU[ID][index]);
}

/******************************************************************************/

void ROBOT_MotorSetMU( int ID, long MU[] )
{
int axis;

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        ROBOT_MotorSetMU(ID,axis,MU[axis]);
    }
}

/******************************************************************************/

void ROBOT_MotorSetNm( int ID, double Nm[] )
{
int axis;

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        ROBOT_MotorSetNm(ID,axis,Nm[axis]);
    }
}

/******************************************************************************/

double ROBOT_MotorGetNm( int ID, int axis )
{
int index;
double Nm=0.0;

    // Current torque setting (Nm) for motor...
    if( (index=ROBOT_AxisIndex[ID][axis]) != -1 )
    {
        Nm = ROBOT_MotorNm[ID][index];
    }

    return(Nm);
}

/******************************************************************************/

long ROBOT_MotorGetMU( int ID, int axis )
{
int index;
long MU=0;

    // Current torque setting (Nm) for motor...
    if( (index=ROBOT_AxisIndex[ID][axis]) != -1 )
    {
        MU = ROBOT_MotorMU[ID][index];
    }

    return(MU);
}

/******************************************************************************/

void ROBOT_MotorGetNm( int ID, double Nm[] )
{
int axis;

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        Nm[axis] = ROBOT_MotorGetNm(ID,axis);
    }
}

/******************************************************************************/

void ROBOT_MotorGetMU( int ID, long MU[] )
{
int axis;

    for( axis=0; (axis < ROBOT_AXIS); axis++ )
    {
        MU[axis] = ROBOT_MotorGetMU(ID,axis);
    }
}

/******************************************************************************/

void ROBOT_MotorGetNm( int ID, matrix &Nm )
{
int axis;

    Nm.dim(ROBOT_DOFS,1);

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        Nm(axis+1,1) = ROBOT_MotorGetNm(ID,axis);
    }
}

/******************************************************************************/

void ROBOT_ControlSet( int ID, int type, void *func )
{
    ROBOT_Item[ID].ControlType = type;

    ROBOT_Item[ID].ControlRaw   = NULL;
    ROBOT_Item[ID].ControlVoid  = NULL;

    ROBOT_Item[ID].ControlMP    = NULL;
    ROBOT_Item[ID].ControlMPF   = NULL;
    ROBOT_Item[ID].ControlMPVF  = NULL;
    ROBOT_Item[ID].ControlMPVAF = NULL;

    ROBOT_Item[ID].ControlDP    = NULL;
    ROBOT_Item[ID].ControlDPF   = NULL;
    ROBOT_Item[ID].ControlDPVF  = NULL;
    ROBOT_Item[ID].ControlDPVAF = NULL;

    switch( ROBOT_Item[ID].ControlType )
    {
        case ROBOT_CONTROL_NONE :
           break;

        case ROBOT_CONTROL_RAW :
           ROBOT_Item[ID].ControlRaw = (void (*)( long EU[], long MU[] ))func;
           ROBOT_Item[ID].FilterType = ROBOT_FILTER_NONE;
           break;

        case ROBOT_CONTROL_MP :
           ROBOT_Item[ID].ControlMP = (void (*)( matrix &p ))func;
           ROBOT_Item[ID].FilterType = ROBOT_FILTER_NONE;
           break;

        case ROBOT_CONTROL_MPF :
           ROBOT_Item[ID].ControlMPF = (void (*)( matrix &p, matrix &f ))func;
           ROBOT_Item[ID].FilterType = ROBOT_FILTER_NONE;
           break;

        case ROBOT_CONTROL_MPVF :
           ROBOT_Item[ID].ControlMPVF = (void (*)( matrix &p, matrix &v, matrix &f ))func;
           break;

        case ROBOT_CONTROL_MPVAF :
           ROBOT_Item[ID].ControlMPVAF = (void (*)( matrix &p, matrix &v, matrix &a, matrix &f ))func;
           break;

        case ROBOT_CONTROL_DP :
           ROBOT_Item[ID].ControlDP = (void (*)( double p[] ))func;
           ROBOT_Item[ID].FilterType = ROBOT_FILTER_NONE;
           break;

        case ROBOT_CONTROL_DPF :
           ROBOT_Item[ID].ControlDPF = (void (*)( double p[], double f[] ))func;
           ROBOT_Item[ID].FilterType = ROBOT_FILTER_NONE;
           break;

        case ROBOT_CONTROL_DPVF :
           ROBOT_Item[ID].ControlDPVF = (void (*)( double p[], double v[], double f[] ))func;
           break;

        case ROBOT_CONTROL_DPVAF :
           ROBOT_Item[ID].ControlDPVAF = (void (*)( double p[], double v[], double a[], double f[] ))func;
           break;

        case ROBOT_CONTROL_VOID :
           ROBOT_Item[ID].ControlVoid = (void (*)( void ))func;
           break;
    }
}

/******************************************************************************/

void ROBOT_Control( int ID, matrix &mp, matrix &mv, matrix &ma, matrix &mf )
{
static double dp[ROBOT_DOFS],dv[ROBOT_DOFS],da[ROBOT_DOFS],df[ROBOT_DOFS];

    // Zero forces...
    mf.zeros();

    // Copy matrix varibles to double arrays
    matrix_double(mp,dp);
    matrix_double(mv,dv);
    matrix_double(ma,da);
    matrix_double(mf,df);

    switch( ROBOT_Item[ID].ControlType )
    {
        case ROBOT_CONTROL_MP :
           (*ROBOT_Item[ID].ControlMP)(mp);
           break;

        case ROBOT_CONTROL_MPF :
           (*ROBOT_Item[ID].ControlMPF)(mp,mf);
           break;

        case ROBOT_CONTROL_MPVF :
           (*ROBOT_Item[ID].ControlMPVF)(mp,mv,mf);
           break;

        case ROBOT_CONTROL_MPVAF :
           (*ROBOT_Item[ID].ControlMPVAF)(mp,mv,ma,mf);
           break;

        case ROBOT_CONTROL_DP :
           (*ROBOT_Item[ID].ControlDP)(dp);
           break;

        case ROBOT_CONTROL_DPF :
           (*ROBOT_Item[ID].ControlDPF)(dp,df);
           matrix_double(df,mf);
           break;

        case ROBOT_CONTROL_DPVF :
           (*ROBOT_Item[ID].ControlDPVF)(dp,dv,df);
           matrix_double(df,mf);
           break;

        case ROBOT_CONTROL_DPVAF :
           (*ROBOT_Item[ID].ControlDPVAF)(dp,dv,da,df);
           matrix_double(df,mf);
           break;
    }
}

/******************************************************************************/

void ROBOT_Control( int ID, long EU[], long MU[] )
{
int axis;

    // Zero Motor Units...
    for( axis=0; (axis < ROBOT_DOFS); MU[axis++]=0 );

    // Execute control function...
    (*ROBOT_Item[ID].ControlRaw)(EU,MU);
}

/******************************************************************************/

void ROBOT_ControlLoop( int ID )
{
static matrix P(ROBOT_DOFS,1),V(ROBOT_DOFS,1),A(ROBOT_DOFS,1),F(ROBOT_DOFS,1);
static double xraw,x,dx,ddx;
static long   EU[ROBOT_DOFS];
static long   MU[ROBOT_DOFS];
int axis,index;

    // Execute only if Application-Defined function exists... (V2.5)
    if( ROBOT_Item[ID].ControlType == ROBOT_CONTROL_NONE ) 
    {
        return;
    }

    // Execute void-type application defined function immediately and return.
    if( ROBOT_Item[ID].ControlType == ROBOT_CONTROL_VOID ) 
    {
        (*ROBOT_Item[ID].ControlVoid)();
        return;
    }

    // Special processing for motors only (raw EU and MU values)... (V2.8)
    if( ROBOT_Item[ID].ControlType == ROBOT_CONTROL_RAW )
    {
        for( axis=0; (axis < ROBOT_DOFS); axis++ )
        {
            // Read encoder if this axis is used...
            if( (index=ROBOT_AxisIndex[ID][axis]) != -1 )
            {
                ROBOT_Encoder(ID,index,xraw,x,dx,ddx);   // Get raw encoder units...
                EU[axis] = (long)xraw;
            }
        }

        ROBOT_Control(ID,EU,MU);          // Call Application-Defined function...
        ROBOT_MotorSetMU(ID,MU);          // Output torques (raw motor units)...

        return;
    }

    ROBOT_Position(ID,P,V,A);        // Calculate position, velocity, acceleration.
    ROBOT_Control(ID,P,V,A,F);       // Call Application-Defined force function.
    ROBOT_Force(ID,F);               // Output forces / torques.
}

/******************************************************************************/

void ROBOT_MotorReset( int ID, int axis )
{
    ROBOT_MotorSetMU(ID,axis,0);
}

/******************************************************************************/

void ROBOT_MotorReset( int ID )
{
int axis;

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        ROBOT_MotorReset(ID,axis);
    }
}

/******************************************************************************/

