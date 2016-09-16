/******************************************************************************/
/* ROBOT: Weight compensation functions (ROBOT-WC.cpp)                        */
/******************************************************************************/

/*double ROBOT_WC_LinkMass[4] = { 0.910, 0.740, 0.700, 0.430 };
//double ROBOT_WC_LinkLength[4] = { 0.450, 0.535, 0.135, 0.450 };
double ROBOT_WC_LinkLength[4] = { 0.450, 0.535, 0.135, 0.450 };
//double ROBOT_WC_LinkCoM[4] = { 0.082, 0.375, 0.050, 0.160 };
double ROBOT_WC_LinkCoM[4] = { 0.082, -0.240, 0.050, 0.160 };*/

#define ROBOT_WC_LINKS   4
double  ROBOT_WC_LinkMass[ROBOT_MAX][ROBOT_WC_LINKS];
double  ROBOT_WC_LinkLength[ROBOT_MAX][ROBOT_WC_LINKS];
double  ROBOT_WC_LinkCoM[ROBOT_MAX][ROBOT_WC_LINKS];

/******************************************************************************/

void ROBOT_WC_CalculateTorques( int ID, double angles[], double torques[] )
{
static double a1,a2,a3,alpha;
static double t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
static double grav=9.8;
static double m1,m2,m3,m4;
static double l1,l2,l3,l4;
static double c1,c2,c3,c4;
static int i;

    for( i=0; (i < 3); i++ )
    {
        torques[i] = 0.0;
    }

    if( !ROBOT_Check(ID)  )
    {
        return;
    }

    if( !(ROBOT_WC_Flag[ID] && ROBOT_CnfgLoaded[ID][ROBOT_CNFG_WC]) )
    {
        return;
    }

    // 3BOT angles (rad).
    a1 = angles[0];
    a2 = angles[1];
    a3 = angles[2]-3.1415;

    // Rotary table angle (rad).
    alpha = D2R(ROBOT_WC_Alpha[ID]);

    // Link masses (kg).
    m1 = ROBOT_WC_LinkMass[ID][0];
    m2 = ROBOT_WC_LinkMass[ID][1];
    m3 = ROBOT_WC_LinkMass[ID][2];
    m4 = ROBOT_WC_LinkMass[ID][3];

    // Link lengths (m).
    l1 = ROBOT_WC_LinkLength[ID][0];
    l2 = ROBOT_WC_LinkLength[ID][1];
    l3 = ROBOT_WC_LinkLength[ID][2];
    l4 = ROBOT_WC_LinkLength[ID][3];

    // Link centre-of-mass (m).
    c1 = ROBOT_WC_LinkCoM[ID][0];
    c2 = ROBOT_WC_LinkCoM[ID][1];
    c3 = ROBOT_WC_LinkCoM[ID][2];
    c4 = ROBOT_WC_LinkCoM[ID][3];

    t2 = cos(alpha);
    t3 = sin(a1);
    t4 = cos(a2);
    t5 = cos(a3);
    t6 = sin(alpha);
    t7 = t4*t6;
    t8 = cos(a1);
    t9 = sin(a2);
    t10 = t2*t8*t9;
    t11 = t7+t10;
    t12 = t5*t6;
    t13 = sin(a3);
    t14 = t2*t8*t13;
    t15 = t12+t14;

    torques[0] = -1.0 * (grav*m2*(c2*t2*t3*t5+l1*t2*t3*t4)+grav*m4*(c4*t2*t3*t4+l3*t2*t3*t5)+c1*grav*m1*t2*t3*t4+c3*grav*m3*t2*t3*t5);
    torques[1] = -1.0 * (c1*grav*m1*t11+c4*grav*m4*t11+grav*l1*m2*t11);
    torques[2] = -1.0 * (c2*grav*m2*t15+c3*grav*m3*t15+grav*l3*m4*t15);

    for( i=0; (i < 3); i++ )
    {
        range(&torques[i],ROBOT_WC_TorqueMax[ID]);
    }
}

/******************************************************************************/

void ROBOT_WC_GetTorques( int ID, double torques[] )
{
int i;

    if( !(ROBOT_Check(ID) && ROBOT_WC_Flag[ID]) )
    {
        return;
    }

    for( i=0; (i < 3); i++ )
    {
        torques[i] = ROBOT_WC_Torques[ID][i-1];
    }
}

/******************************************************************************/

void ROBOT_WC_GetTorques( int ID, matrix &torques )
{
int i;

    torques.dim(3,1);

    if( !(ROBOT_Check(ID) && ROBOT_WC_Flag[ID]) )
    {
        return;
    }

    for( i=1; (i <= 3); i++ )
    {
        torques(i,1) = ROBOT_WC_Torques[ID][i-1];
    }
}

/******************************************************************************/


