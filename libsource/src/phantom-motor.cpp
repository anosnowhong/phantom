/******************************************************************************/
/* PHANTOM: Force (Torque) output functions.                                  */
/******************************************************************************/

void    PHANTOM_MotorPut( int ID, int axis, long MU )
{
    // If PHANTOM not started, output zero torque (V2.6)...
    if( !PHANTOM_Started(ID) )
    {
        MU = 0;
    }

    // Check for maximum Motor Unit output only if motors engaged...
    if( (MU > PHANTOM_MaxMU[ID][axis]) && !PHANTOM_Panic(ID) )
    {
        PHANTOM_errorf("Robot=%s Motor[%d] saturated.\n",PHANTOM_KeyWord[ID][PHANTOM_CNFG_CFG],axis);
        PHANTOM_Exit();
    }

    // This is the actual output of motor in Motor Units...
    PHANTOM_MotorMU[ID][axis] = PHANTOM_RampMU(ID,MU) * PHANTOM_signM[ID][axis];

    // Output value to controller...
    if( PHANTOM_AxisIndex[ID][axis] != -1 )
    {
        PHANTOM_Item[ID].Robot->MotorUnits(PHANTOM_AxisIndex[ID][axis],PHANTOM_MotorMU[ID][axis]);
    }
}

/******************************************************************************/

void    PHANTOM_TorqueNm( int ID, float Nm1, float Nm2, float Nm3 )
{
float   torque[PHANTOM_AXIS];

    torque[PHANTOM_AXIS_1] = Nm1;
    torque[PHANTOM_AXIS_2] = Nm2;
    torque[PHANTOM_AXIS_3] = Nm3;

    PHANTOM_TorqueNm(ID,torque);
}

/******************************************************************************/

void    PHANTOM_TorqueNm( int ID, float Nm[] )
{
int     motor;

    for( motor=0; (motor < PHANTOM_AXIS); motor++ )
    {
        PHANTOM_TorqueMU(ID,motor,PHANTOM_ConvNm2MU(ID,motor,Nm[motor]));
    }
}

/******************************************************************************/

void    PHANTOM_TorqueMU( int ID, long MU[] )
{
int     motor;

    for( motor=0; (motor < PHANTOM_AXIS); motor++ )
    {
        PHANTOM_MotorPut(ID,motor,MU[motor]);
    }
}

/******************************************************************************/

void    PHANTOM_TorqueMU( int ID, int motor, long MU )
{
    PHANTOM_MotorPut(ID,motor,MU);
}

/******************************************************************************/

float   PHANTOM_MotorTorqueNm( int ID, int axis )
{
    return(PHANTOM_MotorNm[ID][axis]);
}

/******************************************************************************/

long    PHANTOM_MotorTorqueMU( int ID, int axis )
{
    return(PHANTOM_MotorMU[ID][axis]);
}

/******************************************************************************/

void    PHANTOM_MotorTorqueNm( int ID, float Nm[] )
{
int     axis;

    for( axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        Nm[axis] = PHANTOM_MotorNm[ID][axis];
    }
}

/******************************************************************************/

void    PHANTOM_MotorTorqueMU( int ID, long MU[] )
{
int     axis;

    for( axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        MU[axis] = PHANTOM_MotorMU[ID][axis];
    }
}

/******************************************************************************/

void    PHANTOM_ControlSet( int ID, int type, void *func )
{
    PHANTOM_Item[ID].ControlType = type;

    PHANTOM_Item[ID].ControlRaw   = NULL;

    PHANTOM_Item[ID].ControlMP    = NULL;
    PHANTOM_Item[ID].ControlMPF   = NULL;
    PHANTOM_Item[ID].ControlMPVF  = NULL;
    PHANTOM_Item[ID].ControlMPVAF = NULL;

    PHANTOM_Item[ID].ControlDP    = NULL;
    PHANTOM_Item[ID].ControlDPF   = NULL;
    PHANTOM_Item[ID].ControlDPVF  = NULL;
    PHANTOM_Item[ID].ControlDPVAF = NULL;

    switch( PHANTOM_Item[ID].ControlType )
    {
        case PHANTOM_CONTROL_NONE :
           break;

        case PHANTOM_CONTROL_RAW :
           PHANTOM_Item[ID].ControlRaw = (void (*)( long EU[], long MU[] ))func;
           break;

        case PHANTOM_CONTROL_MP :
           PHANTOM_Item[ID].ControlMP = (void (*)( matrix &p ))func;
           break;

        case PHANTOM_CONTROL_MPF :
           PHANTOM_Item[ID].ControlMPF = (void (*)( matrix &p, matrix &f ))func;
           break;

        case PHANTOM_CONTROL_MPVF :
           PHANTOM_Item[ID].ControlMPVF = (void (*)( matrix &p, matrix &v, matrix &f ))func;
           break;

        case PHANTOM_CONTROL_MPVAF :
           PHANTOM_Item[ID].ControlMPVAF = (void (*)( matrix &p, matrix &v, matrix &a, matrix &f ))func;
           break;

        case PHANTOM_CONTROL_DP :
           PHANTOM_Item[ID].ControlDP = (void (*)( double p[] ))func;
           break;

        case PHANTOM_CONTROL_DPF :
           PHANTOM_Item[ID].ControlDPF = (void (*)( double p[], double f[] ))func;
           break;

        case PHANTOM_CONTROL_DPVF :
           PHANTOM_Item[ID].ControlDPVF = (void (*)( double p[], double v[], double f[] ))func;
           break;

        case PHANTOM_CONTROL_DPVAF :
           PHANTOM_Item[ID].ControlDPVAF = (void (*)( double p[], double v[], double a[], double f[] ))func;
           break;
    }
}

/******************************************************************************/

void    PHANTOM_Control( int ID, matrix &mp, matrix &mv, matrix &ma, matrix &mf )
{
static double dp[PHANTOM_AXIS],dv[PHANTOM_AXIS],da[PHANTOM_AXIS],df[PHANTOM_AXIS];

    // Zero forces...
    zero(mf);

    matrix_double(mp,dp);
    matrix_double(mv,dv);
    matrix_double(ma,da);
    matrix_double(mf,df);

    switch( PHANTOM_Item[ID].ControlType )
    {
        case PHANTOM_CONTROL_MP :
           (*PHANTOM_Item[ID].ControlMP)(mp);
           break;

        case PHANTOM_CONTROL_MPF :
           (*PHANTOM_Item[ID].ControlMPF)(mp,mf);
           break;

        case PHANTOM_CONTROL_MPVF :
           (*PHANTOM_Item[ID].ControlMPVF)(mp,mv,mf);
           break;

        case PHANTOM_CONTROL_MPVAF :
           (*PHANTOM_Item[ID].ControlMPVAF)(mp,mv,ma,mf);
           break;

        case PHANTOM_CONTROL_DP :
           (*PHANTOM_Item[ID].ControlDP)(dp);
           break;

        case PHANTOM_CONTROL_DPF :
           (*PHANTOM_Item[ID].ControlDPF)(dp,df);
           matrix_double(df,mf);
           break;

        case PHANTOM_CONTROL_DPVF :
           (*PHANTOM_Item[ID].ControlDPVF)(dp,dv,df);
           matrix_double(df,mf);
           break;

        case PHANTOM_CONTROL_DPVAF :
           (*PHANTOM_Item[ID].ControlDPVAF)(dp,dv,da,df);
           matrix_double(df,mf);
           break;
    }
}

/******************************************************************************/

void    PHANTOM_Control( int ID, long EU[], long MU[] )
{
int     axis;

    // Zero Motor Units...
    for( axis=0; (axis < PHANTOM_AXIS); MU[axis++]=0 );

    // Execute control function...
    (*PHANTOM_Item[ID].ControlRaw)(EU,MU);
}

/******************************************************************************/

void    PHANTOM_ControlLoop( int ID )
{
static  matrix  p(3,1),v(3,1),a(3,1),f(3,1);
static  long    EU[PHANTOM_AXIS];
static  long    MU[PHANTOM_AXIS];

    // Execute only if Application-Defined function exists... (V2.5)
    if( PHANTOM_Item[ID].ControlType == PHANTOM_CONTROL_NONE ) 
    {
        return;
    }

    // Special processing for motors only (raw EU and MU values)... (V2.8)
    if( PHANTOM_Item[ID].ControlType == PHANTOM_CONTROL_RAW )
    {
        PHANTOM_Encoder(ID,EU);             // Get raw encoder units...
        PHANTOM_Control(ID,EU,MU);          // Call Application-Defined function...
        PHANTOM_TorqueMU(ID,MU);            // Output torques (raw motor units)...

        return;
    }

    // Default control processing...
    PHANTOM_Posn(ID,p);                     // Get position (x,y,z) for tip of PHANOMT's arm...
    PHANTOM_QuadraticMotion(ID,p,v,a);
    PHANTOM_Control(ID,p,v,a,f);            // Call Application-Defined force function...
    PHANTOM_Force(ID,f);                    // Output forces...
}

/******************************************************************************/

void    PHANTOM_MotorReset( int ID, int axis )
{
    PHANTOM_MotorPut(ID,axis,0);
}

/******************************************************************************/

void    PHANTOM_MotorReset( int ID )
{
int     axis;

    for( axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        PHANTOM_MotorReset(ID,axis);
    }
}

/******************************************************************************/

