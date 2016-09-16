/******************************************************************************/
/*                                                                            */
/* MODULE  : ROBOT.cpp                                                        */
/*                                                                            */
/* PURPOSE : Robotic manipulandum / haptic interface functions.               */
/*                                                                            */
/* DATE    : 19/Sep/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V2.0  JNI 19/Sep/2000 - Development taken over and module re-worked.       */
/*                                                                            */
/* V2.T  JNI 20/Feb/2001 - Disabled temperature tracking (temporary).         */
/*                                                                            */
/* V2.1  JNI 13/Mar/2001 - Pseudo overheating probably caused by setting      */
/*                         cool-down temperature at 55C. It was 100C in the   */
/*                         old module. Changed CFG files and restored code.   */
/*                                                                            */
/* V2.2  JNI 26/Mar/2001 - Force calibration problem. Tested SMALL PHANTOM    */
/*                         with F/T sensor. Actual forces were about 1/3 of   */
/*                         API levels. Needed to convert link lengths from cm */
/*                         to m because torque units of Nm. Also found forces */
/*                         multiplied by 256 in old Sensable code.            */
/*                                                                            */
/* V2.3  JNI 04/Apr/2001 - Allowed zero value for frequency of temperature    */
/*                         checking task to disable temperature tracking.     */
/*                                                                            */
/* V2.4  JNI 04/Apr/2001 - Temporary reversion to erroneous force calculation */
/*                         (pre-V2.2) for consistency in on-going experiments.*/
/*                                                                            */
/* V2.5  JNI 14/Aug/2001 - Change motor saturation check. Exit only if motors */
/*                         running (not in panic mode).                       */
/*                                                                            */
/*                       - Add functions to access PHANTOM motors / encoders  */
/*                         for rod-linkage experiments.                       */
/*                                                                            */
/*       JNI 21/Aug/2001 - Change Motor Force LOOPTASK function to output     */
/*                         forces only if Application-Defined Force Function  */
/*                         has been set.                                      */
/*                                                                            */
/* V2.6  JNI 01/Oct/2001 - Moved application-defined force function to handle */
/*                         structure.                                         */
/*                                                                            */
/*                       - Changed PHANTOM_MotorPut(...) function to output   */
/*                         zero torque if not started. This proved to be of   */
/*                         great importance for the ISA interface card usually*/
/*                         used with the SMALL PHANTOM. It seems the torque   */
/*                         registers on this card need to be reset to zero.   */
/*                         It explains the dangerous behaviour of the SMALL   */
/*                         PHANTOM when engaging the PANIC switch with the    */
/*                         software enable bit set on the amplifier.          */
/*                                                                            */
/* V2.7  JNI 06/Nov/2001 - Added temperature tracking messages to monitor     */
/*                         heating and cooling of motors.                     */
/*                                                                            */
/* V2.8  JNI 05/Feb/2002 - Added forces function to handle raw encoder and    */
/*                         motor units (EU and MU).                           */
/*                                                                            */
/* V2.9  JNI 21/Feb/2002 - Added function to trip PANIC switch using software.*/
/*                                                                            */
/* V3.0  JNI 18/Mar/2002 - Added extra safety stuff after accidents with BIG  */
/*                         & SMALL PHANTOMs.                                  */
/*                                                                            */
/* V4.0  JNI 11/Oct/2002 - Replaced direct I/O interface with ROBOT object to */
/*                         allow PHANTOMs to run on various I/O cards.        */
/*                                                                            */
/* V4.1  JNI 11/Mar/2004 - Minor change to support multiple Sensoray cards.   */
/*                                                                            */
/* V4.2  JNI 07/May/2004 - RTMX option for user-defined co-ordinate frame.    */
/*                                                                            */
/* V4.3  JNI 07/Oct/2004 - Added configuration variables for DAQ F/T sensor.  */
/*                                                                            */
/*                       - Changed module name from PHANTOM to ROBOT.         */
/*                                                                            */
/* V5.0  JNI 25/Apr/2007 - Changes for Kalman Filter.                         */
/*                                                                            */
/*                       - Removed single-axis "rod" functions.               */
/*                                                                            */
/*                       - Added sub-address for virtual Sensoray cards.      */
/*                                                                            */
/* V5.1  JNI 26/Mar/2008 - Change from DAQFT to ASENSOR module to allow       */
/*                         multiple sensors.                                  */
/*                                                                            */
/* V5.2  JNI 04/Feb/2009 - Added Extended Kalman Filter.                      */
/*                                                                            */
/* V5.3  JNI 22/May/2009 - Added code to catch the "undefined value" in torque*/
/*                         variables. These slip through all double-based     */
/*                         sanity checks and are then converted to massive    */
/*                         values when cast to an int varialbe resulting in   */
/*                         very nasty forces.                                 */
/*                                                                            */
/* V5.4  JNI 20/Apr/2010 - Added Robot type variable to configuration file to */
/*                         allow single-axis lever actuators of the type used */
/*                         in force matching / sensory cancellation paradimgs.*/
/*                                                                            */
/* V5.5  JNI 11/Jul/2013 - Added complier directives to include or exclude    */
/*                         OptoTrak code via the "ROBOT_OPTO" define.         */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "ROBOT"
#define MODULE_TEXT     "Robotic Manipulandum API"
#define MODULE_DATE     "20/04/2010"
#define MODULE_VERSION  "5.4"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

BOOL    ROBOT_API_started = FALSE;

#define ROBOT_OPTO

/******************************************************************************/

PRINTF  ROBOT_PRN_messgf=NULL;                 // General messages printf function.
PRINTF  ROBOT_PRN_errorf=NULL;                 // Error messages printf function.
PRINTF  ROBOT_PRN_debugf=NULL;                 // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int ROBOT_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ROBOT_PRN_messgf,buff));
}

/******************************************************************************/

int ROBOT_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ROBOT_PRN_errorf,buff));
}

/******************************************************************************/

int ROBOT_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ROBOT_PRN_debugf,buff));
}

/******************************************************************************/
/* Configuration and calibration parameter files for the robot...             */
/******************************************************************************/

struct  ROBOT_CnfgFile     ROBOT_Cnfg[] =
{
    { "CFG",TRUE,ROBOT_CnfgCFG,ROBOT_TestCFG },
    { "CAL",TRUE,ROBOT_CnfgCAL,ROBOT_TestCAL },
    { NULL,FALSE,NULL,NULL }
};

/******************************************************************************/

BOOL    ROBOT_reset[ROBOT_MAX];                       // Robot has been reset?

double  ROBOT_link[ROBOT_MAX][ROBOT_LINK];            // Length of [N]th link in arm.
double  ROBOT_marker[ROBOT_MAX][ROBOT_MARKER];        // Distance to [N]th OptoTrak marker on 2nd link.

double  ROBOT_PositionOffset[ROBOT_MAX][AXIS_XYZ];    // XYZ position of base.
double  ROBOT_AngleOffset[ROBOT_MAX][ROBOT_DOFS];     // Angular offset for each encoder.
matrix  ROBOT_RTMX[ROBOT_MAX];
matrix  ROBOT_invROMX[ROBOT_MAX];

#define ROBOT_CONTROLLER   4
STRING  ROBOT_controller[ROBOT_MAX][ROBOT_CONTROLLER];
STRING  ROBOT_motors[ROBOT_MAX][ROBOT_DOFS];
STRING  ROBOT_encoders[ROBOT_MAX][ROBOT_DOFS];

STRING  ROBOT_manipulandum[ROBOT_MAX];                // Robot manipulandum type (V5.4)

struct  STR_TextItem  ROBOT_ManipulandumTypeText[] = 
{
    { ROBOT_MANIPULANDUM_MULTIAXIS,"MultiAxis" },
    { ROBOT_MANIPULANDUM_SINGLEAXIS,"SingleAxis" },
    { STR_TEXT_ENDOFTABLE },
};

double  ROBOT_signXYZ[ROBOT_MAX][AXIS_XYZ];           // Sign for XYZ axis (determined by configuration of arm).
long    ROBOT_signM[ROBOT_MAX][ROBOT_DOFS];           // Sign for motor on [N]th axis.
double  ROBOT_signE[ROBOT_MAX][ROBOT_DOFS];           // Sign for encoder on [N]th axis.

double  ROBOT_EU2Rad[ROBOT_MAX][ROBOT_DOFS];          // Convert Encoder Units to angle (radians) for [N]th axis.
double  ROBOT_Nm2MU[ROBOT_MAX][ROBOT_DOFS];           // Convert torque (Nm) to Motor Units for [N]th axis.
double  ROBOT_TorqueGain[ROBOT_MAX][ROBOT_DOFS];      // Torque gain from force calibration.
double  ROBOT_TorqueOffset[ROBOT_MAX][ROBOT_DOFS];    // Torque offset from force calibration.

long    ROBOT_MaxMU[ROBOT_MAX][ROBOT_DOFS];           // Maximum motor units.

double  ROBOT_RampTime[ROBOT_MAX];                    // Time for ramping up motor torque (seconds).

double  ROBOT_LoopTaskFrequency[ROBOT_MAX];           // Frequency of ROBOT LoopTask.
double  ROBOT_LoopTaskPeriod[ROBOT_MAX];              // Period of ROBOT LoopTask.

int     ROBOT_FilterType[ROBOT_MAX];                  // Filter type.
BOOL    ROBOT_FilterPosition[ROBOT_MAX];              // Use filtered position instead of raw values.

double  ROBOT_KF_w[ROBOT_MAX][ROBOT_DOFS];            // Kalman Filter parameter.
double  ROBOT_KF_v[ROBOT_MAX][ROBOT_DOFS];            // Kalman Filter parameter.

int     ROBOT_QuadraticFitWindowSize[ROBOT_MAX];      // Window size for fitting quadratic.

// Analogue sensor details from configuration file.
STRING  ROBOT_SensorName[ROBOT_MAX][ROBOT_SENSOR_MAX];
double  ROBOT_SensorZRotation[ROBOT_MAX][ROBOT_SENSOR_MAX];
matrix  ROBOT_SensorROMX[ROBOT_MAX][ROBOT_SENSOR_MAX];

UINT    ROBOT_OptoTrakFrame=0;
TIMER_Interval ROBOT_OptoTrakReadLatency("ROBOT_OptoTrakReadLatency");

/******************************************************************************/
/* Working variables...                                                       */
/******************************************************************************/

int     ROBOT_ID = ROBOT_INVALID;                     // Current working ROBOT Handle.

double  ROBOT_sin[ROBOT_MAX][ROBOT_AXIS];             // Sine of current [N]th axis encoder angle.
double  ROBOT_cos[ROBOT_MAX][ROBOT_AXIS];             // Cosine of current [N]th axis encoder angle.
double  ROBOT_Y_COS[ROBOT_MAX];                       // Y offset of tip from base (without X rotation about base).
double  ROBOT_Z_SIN[ROBOT_MAX];                       // Z offset of tip from base (X rotation irrelevant in Z).

long    ROBOT_MotorMU[ROBOT_MAX][ROBOT_DOFS];         // Input to motors (in MUs).
double  ROBOT_MotorNm[ROBOT_MAX][ROBOT_DOFS];         // Torque input to motors (Nm).

int     ROBOT_AxisIndex[ROBOT_MAX][ROBOT_DOFS];       // Axis (channel) index.
int     ROBOT_XYZIndex[ROBOT_MAX][AXIS_XYZ];          // XYZ index for translation DOFs
int     ROBOT_XYZRotation[ROBOT_MAX][AXIS_XYZ];       // XYZ index for rotational DOFs.

matrix  ROBOT_J[ROBOT_MAX];       // Jacobian.
matrix  ROBOT_JT[ROBOT_MAX];      // Jacobian Transpose.

/******************************************************************************/

STRING  ROBOT_KeyWord[ROBOT_MAX];
STRING  ROBOT_Name[ROBOT_MAX];

/******************************************************************************/

TIMER_Interval ROBOT_LoopTaskLatencyTimer[ROBOT_MAX];
double ROBOT_LoopTaskLatency[ROBOT_MAX];

/******************************************************************************/

BOOL    ROBOT_Test=FALSE;

struct  ROBOT_ItemHandle   ROBOT_Item[ROBOT_MAX];

/******************************************************************************/

void     ROBOT_LoopTaskN( int ID )
{
    ROBOT_LoopTaskLatencyTimer[ID].Before();

    ROBOT_RobotLoop(ID);
    ROBOT_PanicLoop(ID);
    ROBOT_EncoderLoop(ID);
    ROBOT_ControlLoop(ID);
 
    ROBOT_LoopTaskLatency[ID] = ROBOT_LoopTaskLatencyTimer[ID].After();
}

/******************************************************************************/

void     ROBOT_LoopTask0( void )   { ROBOT_LoopTaskN(0); }
void     ROBOT_LoopTask1( void )   { ROBOT_LoopTaskN(1); }
void     ROBOT_LoopTask2( void )   { ROBOT_LoopTaskN(2); }

LOOPTASK ROBOT_LoopTask[ROBOT_MAX] = { ROBOT_LoopTask0,ROBOT_LoopTask1,ROBOT_LoopTask2 };

/******************************************************************************/

#include "robot-encoder.cpp"         // Function to input values from encoders.
#include "robot-motor.cpp"           // Function to output torques to motors.
#include "robot-ramp.cpp"            // Force ramping functions.
#include "robot-temperature.cpp"     // Temperature tracking for motors over-heating.
#include "robot-convert.cpp"         // Functions to inverconvert various units.
#include "robot-sensor.cpp"          // Functions for sensors (e.g. DAQ F/T sensor).

/******************************************************************************/

void ROBOT_Init( int ID )
{
int axis,item;

    if( ID == ROBOT_INVALID )
    {
        return;
    }

    // Robot controller...
    ROBOT_Item[ID].Robot = NULL;

    // LoopTask frequency...
    ROBOT_LoopTaskFrequency[ID] = 0.0;

    // Various flags...
    ROBOT_Item[ID].started = FALSE;
    ROBOT_Item[ID].HWpanic = FALSE;
    ROBOT_Item[ID].SWpanic = FALSE;
    ROBOT_Item[ID].safe = TRUE;

    // Robot has been reset.
    ROBOT_reset[ID] = TRUE;

    // Default manipulandum type (vBOT, Phantom, etc).
    ROBOT_Item[ID].ManipulandumType = ROBOT_MANIPULANDUM_MULTIAXIS;
    memset(ROBOT_manipulandum[ID],0,STRLEN);

    // Zero application-defined force functions (V2.6)...
    ROBOT_Item[ID].ControlType = ROBOT_CONTROL_NONE;

    ROBOT_Item[ID].ControlRaw = NULL;

    ROBOT_Item[ID].ControlMP = NULL;
    ROBOT_Item[ID].ControlMPF = NULL;
    ROBOT_Item[ID].ControlMPVF = NULL;
    ROBOT_Item[ID].ControlMPVAF = NULL;

    ROBOT_Item[ID].ControlDP = NULL;
    ROBOT_Item[ID].ControlDPF = NULL;
    ROBOT_Item[ID].ControlDPVF = NULL;
    ROBOT_Item[ID].ControlDPVAF = NULL;

    ROBOT_Item[ID].FilterType = ROBOT_FILTER_NONE;
    ROBOT_Item[ID].EKF = NULL;

    ROBOT_QuadraticFitWindowSize[ID] = 0;
    ROBOT_Item[ID].QuadraticFitWindow = NULL;

    ROBOT_Y_COS[ID] = 0.0;
    ROBOT_Z_SIN[ID] = 0.0;

    // RTMX for user-define co-ordinate frame (V4.2)...
    ROBOT_RTMX[ID] = I(4);

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        ROBOT_AxisIndex[ID][axis] = -1;

        ROBOT_KF_w[ID][axis] = 0.0;
        ROBOT_KF_v[ID][axis] = 0.0;

        ROBOT_MotorMU[ID][axis] = 0;
        ROBOT_MotorNm[ID][axis] = 0.0;

        ROBOT_Item[ID].Axis[axis].Encoder.count_xraw = 0.0;
        ROBOT_Item[ID].Axis[axis].Encoder.count_x = 0.0;
        ROBOT_Item[ID].Axis[axis].Encoder.count_dx = 0.0;
        ROBOT_Item[ID].Axis[axis].Encoder.count_ddx = 0.0;

        ROBOT_TorqueGain[ID][axis] = 1.0;
        ROBOT_TorqueOffset[ID][axis] = 0.0;
    }

    for( axis=0; (axis < ROBOT_AXIS); axis++ )
    {
        ROBOT_sin[ID][axis] = 0.0;
        ROBOT_cos[ID][axis] = 0.0;
    }

    for( axis=0; (axis < AXIS_XYZ); axis++ )
    {
        ROBOT_XYZIndex[ID][axis] = axis;
        ROBOT_XYZRotation[ID][axis] = -1;
    }

    ROBOT_J[ID].dim(3,3);
    ROBOT_JT[ID].dim(3,3);

    ROBOT_FilterType[ID] = ROBOT_FILTER_NONE;
    ROBOT_FilterPosition[ID] = FALSE;

    for( item=0; (item < ROBOT_CONTROLLER); item++ )
    {
        memset(ROBOT_controller[ID][item],0,STRLEN);
    }

    ROBOT_Item[ID].DAQFT = NULL;
    ROBOT_Item[ID].DAQFT_Sensor = -1;

    ROBOT_Item[ID].Accelerometer = NULL;
    ROBOT_Item[ID].Accelerometer_Sensor = -1;

    ROBOT_Item[ID].PhotoTransistor = NULL;
    ROBOT_Item[ID].PhotoTransistor_Sensor = -1;

    for( item=0; (item < ROBOT_SENSOR_MAX); item++ )
    {
        ROBOT_Item[ID].SensorList[item].asensor = NULL;

        memset(ROBOT_SensorName[ID][item],0,STRLEN);
        ROBOT_SensorZRotation[ID][item] = 0.0;
        ROBOT_SensorROMX[ID][item] = I(3);
    }
}

/******************************************************************************/

int ROBOT_Free( void )
{
int ID,free;

    if( !ROBOT_Check() )                  // Check if API started...
    {
        return(ROBOT_INVALID);
    }

    for( free=ROBOT_INVALID,ID=0; (ID < ROBOT_MAX); ID++ )
    {
        if( ROBOT_Item[ID].Robot == NULL )
        {
            free = ID;
            break;
        }
    }

    return(free);
}

/******************************************************************************/

void ROBOT_Use( int ID )
{
    ROBOT_ID = ID;
}

/******************************************************************************/

void ROBOT_Defaults( int ID )
{
    // Set any default configuration values here...
}

/******************************************************************************/

void ROBOT_CnfgHDR( int ID, int cnfg )
{
static STRING temp1,temp2;

    if( ROBOT_Cnfg[cnfg].HDR )
    {
        if( cnfg == 0 )
        {
            CONFIG_set("KeyWord",ROBOT_KeyWord[ID]);
            CONFIG_set("Name",ROBOT_Name[ID]);
        }
        else
        {
            CONFIG_set("KeyWord",temp1);
            CONFIG_set("Name",temp2);
        }
    }
}

/******************************************************************************/

void ROBOT_CnfgCAL( int ID )
{
    CONFIG_reset();


    ROBOT_CnfgHDR(ID,ROBOT_CNFG_CAL);

    CONFIG_setBOOL("reset",ROBOT_reset[ID]);
    CONFIG_set("link",ROBOT_link[ID],ROBOT_LINK); 
    CONFIG_set("marker",ROBOT_marker[ID],ROBOT_MARKER);
    CONFIG_set("baseXYZ",ROBOT_PositionOffset[ID],AXIS_XYZ);
    CONFIG_set("angle",ROBOT_AngleOffset[ID],ROBOT_DOFS);
    CONFIG_set("RTMX",ROBOT_RTMX[ID]);
}

/******************************************************************************/

void ROBOT_CnfgCFG( int ID )
{
int i;

    CONFIG_reset();

    ROBOT_CnfgHDR(ID,ROBOT_CNFG_CFG);

    CONFIG_set("Controller",ROBOT_controller[ID],ROBOT_CONTROLLER);
    CONFIG_set("Motors",ROBOT_motors[ID],ROBOT_DOFS);
    CONFIG_set("Encoders",ROBOT_encoders[ID],ROBOT_DOFS);
    CONFIG_set("Manipulandum",ROBOT_manipulandum[ID]);
    CONFIG_set("FilterType",ROBOT_FilterType[ID]);
    CONFIG_setBOOL("FilterPosition",ROBOT_FilterPosition[ID]);
    CONFIG_set("KF_w",ROBOT_KF_w[ID],ROBOT_DOFS);
    CONFIG_set("KF_v",ROBOT_KF_v[ID],ROBOT_DOFS);
    CONFIG_set("signXYZ",ROBOT_signXYZ[ID],AXIS_XYZ);
    CONFIG_set("signM",ROBOT_signM[ID],ROBOT_DOFS);
    CONFIG_set("signE",ROBOT_signE[ID],ROBOT_DOFS);
    CONFIG_set("AxisIndex",ROBOT_AxisIndex[ID],ROBOT_DOFS);
    CONFIG_set("XYZIndex",ROBOT_XYZIndex[ID],AXIS_XYZ);
    CONFIG_set("XYZRotation",ROBOT_XYZRotation[ID],AXIS_XYZ);
    CONFIG_set("EU2Rad",ROBOT_EU2Rad[ID],ROBOT_DOFS);
    CONFIG_set("Nm2MU",ROBOT_Nm2MU[ID],ROBOT_DOFS);
    CONFIG_set("TorqueGain",ROBOT_TorqueGain[ID],ROBOT_DOFS);
    CONFIG_set("TorqueOffset",ROBOT_TorqueOffset[ID],ROBOT_DOFS);
    CONFIG_set("MaxMU",ROBOT_MaxMU[ID],ROBOT_DOFS);
    CONFIG_set("RampTime",ROBOT_RampTime[ID]);
    CONFIG_set("LoopTaskFreq",ROBOT_LoopTaskFrequency[ID]);
    CONFIG_set("MotionWindow",ROBOT_QuadraticFitWindowSize[ID]);
    CONFIG_set("QuadraticFitWindow",ROBOT_QuadraticFitWindowSize[ID]);

    // F/T sensor.
    CONFIG_set("FT-Name",ROBOT_SensorName[ID][0]);
    CONFIG_set("FT-ZRotation",ROBOT_SensorZRotation[ID][0]);
    CONFIG_set("FT-ROMX",ROBOT_SensorROMX[ID][0]);

    // Analogue sensors (e.g. DAQFT, accelerometers, etc.)
    for( i=0; (i < ROBOT_SENSOR_MAX); i++ )
    {
        CONFIG_set(STR_stringf("SensorName[%d]",i),ROBOT_SensorName[ID][i]);
        CONFIG_set(STR_stringf("SensorZRotation[%d]",i),ROBOT_SensorZRotation[ID][i]);
        CONFIG_set(STR_stringf("SensorROMX[%d]",i),ROBOT_SensorROMX[ID][i]);
    }
}

/******************************************************************************/

void    ROBOT_CnfgVAR( int ID, int cnfg )
{
    (*ROBOT_Cnfg[cnfg].load)(ID);
}

/******************************************************************************/

BOOL    ROBOT_TestCAL( int ID )
{
    return(TRUE);
}

/******************************************************************************/

BOOL    ROBOT_TestCFG( int ID )
{
    if( ROBOT_RampTime[ID] == 0.0 )
    {
        ROBOT_errorf("Ramp Time (RampTime) is zero.\n");
        return(FALSE);
    }

/*    if( ROBOT_TempSuspend[ID] > ROBOT_TEMP_MAX )
    {
        ROBOT_errorf("Suspend Temperature (TempSuspend) is too high (maximum %.1f).\n",ROBOT_TEMP_MAX);
        return(FALSE);
    }
*/
    return(TRUE);
}

/******************************************************************************/

BOOL    ROBOT_TestVAR( int ID, int cnfg )
{
    return((*ROBOT_Cnfg[cnfg].test)(ID));
}

/******************************************************************************/

BOOL    ROBOT_CnfgSave( int ID, int cnfg )
{
BOOL    ok=FALSE;
char   *file,*path;

    file = ROBOT_Item[ID].cnfg[cnfg];
    if( (path=FILE_Calibration(file)) == NULL )
    {
        ROBOT_errorf("ROBOT_CnfgSave(...) Cannot find file (%s).\n",file);
        return(FALSE);
    }

    ROBOT_debugf("ROBOT_CnfgSave(...) %s\n",path);

    if( !ROBOT_TestVAR(ID,cnfg) )
    {
        ROBOT_errorf("ROBOT_CnfgSave(...) CONFIG_Test%s(%s) Failed.\n",ROBOT_Cnfg[cnfg].EXT,file);
        return(FALSE);
    }

    ROBOT_debugf("ROBOT_CnfgSave(...) CONFIG_Test%s(%s) OK.\n",ROBOT_Cnfg[cnfg].EXT,file);

    ROBOT_CnfgVAR(ID,cnfg);

    if( CONFIG_write(path) )
    {
        ROBOT_debugf("ROBOT_CnfgSave(...) CONFIG_write(%s) OK.\n",path);
        ok = TRUE;
    }
    else
    {
        ROBOT_errorf("ROBOT_CnfgSave(...) CONFIG_read(%s) Failed.\n",path);
    }

    return(TRUE);
}

/******************************************************************************/

BOOL ROBOT_Hardware( int ID )
{
int axis,code,step,items,item;
double link[2];
BOOL ok;

#define ROBOT_TYPE_HOSTCARD       0
#define ROBOT_TYPE_ADDRESS        1
#define ROBOT_TYPE_CONTROLLER     2
#define ROBOT_TYPE_AXIS           3
#define ROBOT_TYPE_FILTERS        4
#define ROBOT_TYPE_MOTORS         5
#define ROBOT_TYPE_ENCODERS       6
#define ROBOT_TYPE_GEARING        7
#define ROBOT_TYPE_MANIPULANDUM   8

    for( ok=TRUE,item=0,step=ROBOT_TYPE_HOSTCARD; ((step <= ROBOT_TYPE_MANIPULANDUM) && ok); step++ )
    {
        switch( step )
        {
            case ROBOT_TYPE_HOSTCARD :
               for( items=0; ((items < ROBOT_CONTROLLER) && !STR_null(ROBOT_controller[ID][items])); items++ );

               if( (code=STR_TextCode(CONTROLLER_HostCardText,ROBOT_controller[ID][item])) == STR_NOTFOUND )
               {
                   ROBOT_errorf("ROBOT[%d] Invalid controller (%s).\n",ID,ROBOT_controller[ID][item]);
                   ok = FALSE;
                   break;
               }

               ROBOT_Item[ID].Hardware.Controller.CardType = code;
               item++;
               break;

            case ROBOT_TYPE_ADDRESS :
               ROBOT_Item[ID].Hardware.Controller.Address = CONFIG_buff2long(ROBOT_controller[ID][item++]);
               ROBOT_Item[ID].Hardware.Controller.SubAddress = 0;

               if( (ROBOT_Item[ID].Hardware.Controller.CardType == CONTROLLER_HOSTCARD_SENSORAY) && (items == ROBOT_CONTROLLER) )
               {
                   ROBOT_Item[ID].Hardware.Controller.SubAddress = CONFIG_buff2int(ROBOT_controller[ID][item++]);
               }
               break;

            case ROBOT_TYPE_CONTROLLER :
               if( (code=STR_TextCode(CONTROLLER_ControllerTypeText,ROBOT_controller[ID][item])) == STR_NOTFOUND )
               {
                   ROBOT_errorf("ROBOT[%d] Invalid controller (%s).\n",ID,ROBOT_controller[ID][item]);
                   ok = FALSE;
                   break;
               }

               ROBOT_Item[ID].Hardware.Controller.ControllerType = code;
               item++;
               break;

            case ROBOT_TYPE_AXIS :
               ROBOT_Item[ID].Hardware.AxisCount = ROBOT_DOF(ID); 
               break;

            case ROBOT_TYPE_FILTERS :
               if( (ROBOT_QuadraticFitWindowSize[ID] != 0) && (ROBOT_Item[ID].FilterType == ROBOT_FILTER_NONE) )
               {
                   ROBOT_Item[ID].FilterType = ROBOT_FILTER_QUADRATICFIT;
               }
               else
               {
                   ROBOT_Item[ID].FilterType = ROBOT_FilterType[ID];
               }

               switch( ROBOT_Item[ID].FilterType )
               {
                   case ROBOT_FILTER_NONE :
                      break;

                   case ROBOT_FILTER_QUADRATICFIT :
                      ROBOT_Item[ID].QuadraticFitWindow = new WINFIT(ROBOT_Item[ID].name,ROBOT_QuadraticFitWindowSize[ID],ROBOT_DOFS);
                      break;

                   case ROBOT_FILTER_KALMANFILTER :
                      break;

                   case ROBOT_FILTER_EKF :
                      ROBOT_Item[ID].EKF = new EKF(ROBOT_Name[ID]);
                      link[0] = ROBOT_link[ID][0]/100.0;
                      link[1] = ROBOT_link[ID][1]/100.0;
                      ROBOT_Item[ID].EKF->Open(ROBOT_TDOF(ID),ROBOT_KF_w[ID][0],ROBOT_KF_v[ID][0],ROBOT_LoopTaskPeriod[ID],link);
                      if( !ROBOT_Item[ID].EKF->Opened() )
                      {
                          ROBOT_errorf("ROBOT[%d] Cannot open EKF.\n",ID);
                          ok = FALSE;
                      }
                      break;
               }
               break;

            case ROBOT_TYPE_MOTORS :
               for( axis=0; ((axis < ROBOT_Item[ID].Hardware.AxisCount) && ok); axis++ )
               {
                   if( (axis == 0) || !STR_null(ROBOT_motors[ID][axis]) )
                   {
                       if( (code=STR_TextCode(MOTOR_TypeText,ROBOT_motors[ID][axis])) == STR_NOTFOUND )
                       {
                           ROBOT_errorf("ROBOT[%d] Invalid motor (%s).\n",ID,ROBOT_motors[ID][axis]);
                           ok = FALSE;
                           continue;
                       }
                   }

                   ROBOT_Item[ID].Hardware.Axis[axis].Motor.Type = code;
                   ROBOT_Item[ID].Hardware.Axis[axis].Motor.Channel = axis;
               }
               break;

            case ROBOT_TYPE_ENCODERS :
               for( axis=0; ((axis < ROBOT_Item[ID].Hardware.AxisCount) && ok); axis++ )
               {
                   if( (axis == 0) || !STR_null(ROBOT_encoders[ID][axis]) )
                   {
                       if( (code=STR_TextCode(ENCODER_TypeText,ROBOT_encoders[ID][axis])) == STR_NOTFOUND )
                       {
                           ROBOT_errorf("ROBOT[%d] Invalid encoder (%s).\n",ID,ROBOT_encoders[ID][axis]);
                           ok = FALSE;
                           continue;
                       }
                   }

                   ROBOT_Item[ID].Hardware.Axis[axis].Encoder.Type = code;
                   ROBOT_Item[ID].Hardware.Axis[axis].Encoder.Channel = axis;
                   ROBOT_Item[ID].Hardware.Axis[axis].Encoder.Sign = (int)ROBOT_signE[ID][axis];

                   if( ROBOT_Item[ID].FilterType == ROBOT_FILTER_KALMANFILTER )
                   {
                       ROBOT_Item[ID].Hardware.Axis[axis].Encoder.KF_w = ROBOT_KF_w[ID][0];
                       ROBOT_Item[ID].Hardware.Axis[axis].Encoder.KF_v = ROBOT_KF_v[ID][0];
                       ROBOT_Item[ID].Hardware.Axis[axis].Encoder.KF_dt = ROBOT_LoopTaskPeriod[ID];
                   }
                   else
                   {
                       ROBOT_Item[ID].Hardware.Axis[axis].Encoder.KF_w = 0.0;
                       ROBOT_Item[ID].Hardware.Axis[axis].Encoder.KF_v = 0.0;
                       ROBOT_Item[ID].Hardware.Axis[axis].Encoder.KF_dt = 0.0;
                   }
               }
               break;

            case ROBOT_TYPE_GEARING :
               for( axis=0; ((axis < ROBOT_Item[ID].Hardware.AxisCount) && ok); axis++ )
               {
                   ROBOT_Item[ID].Hardware.Axis[axis].Gearing = 1.0;
               }
               break;

            case ROBOT_TYPE_MANIPULANDUM :
               if( STR_null(ROBOT_manipulandum[ID]) )
               {
                   break;
               }

               if( (code=STR_TextCode(ROBOT_ManipulandumTypeText,ROBOT_manipulandum[ID])) == STR_NOTFOUND )
               {
                   ROBOT_errorf("ROBOT[%d] Invalid manipulandum (%s).\n",ID,ROBOT_manipulandum[ID]);
                   ok = FALSE;
                   break;
               }

               ROBOT_Item[ID].ManipulandumType = code;
               break;
        }
    }

    if( !ok )
    {
        return(FALSE);
    }

    ROBOT_debugf("ROBOT[%d] Hardware...\n",ID);
    ROBOT_HardwareDetails(&ROBOT_Item[ID].Hardware,ROBOT_debugf);

    return(TRUE);
}

/******************************************************************************/

int ROBOT_DOF( int ID )
{
int axis,dof;

    for( dof=0,axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        if( ROBOT_AxisIndex[ID][axis] != -1 )
        {
            dof++;
        }
    }

    return(dof);
}

/******************************************************************************/

int ROBOT_TDOF( int ID )
{
int axis,dof;

    for( dof=0,axis=0; (axis < AXIS_XYZ); axis++ )
    {
        if( ROBOT_XYZIndex[ID][axis] != -1 )
        {
            dof++;
        }
    }

    return(dof);
}

/******************************************************************************/

int ROBOT_RDOF( int ID )
{
int axis,dof;

    for( dof=0,axis=0; (axis < AXIS_XYZ); axis++ )
    {
        if( ROBOT_XYZRotation[ID][axis] != -1 )
        {
            dof++;
        }
    }

    return(dof);
}

/******************************************************************************/

BOOL ROBOT_3D( int ID )
{
BOOL flag;

    flag = (ROBOT_TDOF(ID) == 3);

    return(flag);
}

/******************************************************************************/

BOOL ROBOT_2D( int ID )
{
BOOL flag;

    flag = (ROBOT_TDOF(ID) == 2);

    return(flag);
}

/******************************************************************************/

BOOL    ROBOT_CnfgLoadType( int ID, int cnfg )
{
BOOL    ok=FALSE;
char   *file,*path;

    ROBOT_CnfgVAR(ID,cnfg);

    file = ROBOT_Item[ID].cnfg[cnfg];

    if( (path=FILE_Calibration(file)) == NULL )
    {
        ROBOT_errorf("ROBOT_CnfgLoad(...) Cannot find file (%s).\n",file);
        return(FALSE);
    }

    ROBOT_debugf("ROBOT_CnfgLoad(...) %s\n",path);

    if( !CONFIG_read(path) )
    {
        ROBOT_errorf("ROBOT_CnfgLoad(...) CONFIG_read(%s) Failed.\n",path);
        return(ok);
    }

    ROBOT_debugf("ROBOT_CnfgLoad(...) CONFIG_read(%s) OK.\n",path);

    if( !CONFIG_flag(CONFIG_FLAG_READ) )    // Make sure all parameters have been read...
    {
        ROBOT_debugf("ROBOT_CnfgLoad(...) CONFIG_read(%s) Parameters missing.\n",path);
        CONFIG_list(CONFIG_FLAG_READ,FALSE,ROBOT_debugf);
    }

    if( ROBOT_TestVAR(ID,cnfg) )
    {
        ROBOT_debugf("ROBOT_CnfgLoad(...) CONFIG_Test%s(%s) OK.\n",ROBOT_Cnfg[cnfg].EXT,path);
        ok = TRUE;
    }
    else
    {
        ROBOT_errorf("ROBOT_CnfgLoad(...) CONFIG_Test%s(%s) Failed.\n",ROBOT_Cnfg[cnfg].EXT,path);
    }

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_CnfgLoad( int ID, char *name )
{
int cfg,i;
BOOL ok;

    for( ok=TRUE,cfg=0; ((ROBOT_Cnfg[cfg].EXT != NULL) && ok); cfg++ )
    {
        if( cfg == 0 )
        {
            strncpy(ROBOT_Item[ID].cnfg[cfg],STR_stringf("%s.%s",name,ROBOT_Cnfg[cfg].EXT),STRLEN);
        }

        ok = ROBOT_CnfgLoadType(ID,cfg);

        if( ok && (cfg == 0) )
        {
            for( i=1; (ROBOT_Cnfg[i].EXT != NULL); i++ )
            {
                strncpy(ROBOT_Item[ID].cnfg[i],STR_stringf("%s.%s",ROBOT_KeyWord[ID],ROBOT_Cnfg[i].EXT),STRLEN);
            }
        }
    }

    return(ok);
}

/******************************************************************************/

void    ROBOT_CnfgPrnt( int ID, PRINTF prnf, int cnfg )
{
    ROBOT_CnfgLoadType(ID,cnfg);
    CONFIG_list(prnf);
}

/******************************************************************************/

void    ROBOT_CnfgPrnt( int ID, PRINTF prnf )
{
int     cnfg;

    for( cnfg=0; (ROBOT_Cnfg[cnfg].EXT != NULL); cnfg++ )
    {
        ROBOT_CnfgPrnt(ID,prnf,cnfg);
    }
}

/******************************************************************************/

BOOL    ROBOT_Started( int ID )
{
BOOL    started=FALSE;

    if( ROBOT_Check(ID) )
    {
        started = ROBOT_Item[ID].started;
    }

    return(started);
}

/******************************************************************************/

ROBOT *ROBOT_Robot( int ID )
{
ROBOT *robot=NULL;

    if( ROBOT_Check(ID) )
    {
        robot = ROBOT_Item[ID].Robot;
    }

    return(robot);
}

/******************************************************************************/

BOOL    ROBOT_Safe( int ID )
{
BOOL    safe=FALSE;

    if( ROBOT_Check(ID) )
    {
        if( ROBOT_Item[ID].Robot->Safe() )
        {
            safe = ROBOT_Item[ID].safe;
        }
    }

    return(safe);
}

/******************************************************************************/

void ROBOT_UnSafe( int ID )
{
    if( ROBOT_Check(ID) )
    {
        ROBOT_Item[ID].safe = FALSE;
        ROBOT_MotorReset(ID);
    }
}

/******************************************************************************/

BOOL    ROBOT_Activated( int ID )
{
BOOL    flag=FALSE;

    if( ROBOT_Check(ID) )
    {
        flag = ROBOT_Item[ID].Robot->Activated();
    }

    return(flag);
}

/******************************************************************************/

BOOL    ROBOT_JustActivated( int ID )
{
BOOL    flag=FALSE;

    if( ROBOT_Check(ID) )
    {
        flag = ROBOT_Item[ID].Robot->JustActivated();
    }

    return(flag);
}

/******************************************************************************/

BOOL    ROBOT_JustDeactivated( int ID )
{
BOOL    flag=FALSE;

    if( ROBOT_Check(ID) )
    {
        flag = ROBOT_Item[ID].Robot->JustDeactivated();
    }

    return(flag);
}

/******************************************************************************/

BOOL    ROBOT_Panic( int ID )
{
BOOL    panic=TRUE;

    if( ROBOT_Check(ID) )
    {
        ROBOT_Item[ID].HWpanic = !ROBOT_Activated(ID);
        panic = ROBOT_Item[ID].HWpanic || ROBOT_Item[ID].SWpanic;
        ROBOT_Item[ID].SWpanic = FALSE;
    }

    return(panic);
}

/******************************************************************************/

void    ROBOT_PanicNow( int ID )
{
    if( ROBOT_Check(ID) )
    {
        ROBOT_Item[ID].SWpanic = TRUE;
        ROBOT_debugf("ROBOT_PanicNow(%s)\n",ROBOT_Item[ID].name);
    }
}

/******************************************************************************/

BOOL    ROBOT_PanicOff2On( int ID )
{
BOOL    flag=FALSE;

    if( ROBOT_Check(ID) )
    {
        flag = ROBOT_Item[ID].Robot->JustDeactivated();
    }

    return(flag);
}

/******************************************************************************/

BOOL    ROBOT_PanicOn2Off( int ID )
{
BOOL    flag=FALSE;

    if( ROBOT_Check(ID) )
    {
        flag = ROBOT_Item[ID].Robot->JustActivated();
    }

    return(flag);
}

/******************************************************************************/

void    ROBOT_RobotLoop( int ID )
{
    if( ROBOT_Check(ID) )
    {
        ROBOT_Item[ID].Robot->LoopTask();
    }
}

/******************************************************************************/

BOOL    ROBOT_Cooling( int ID )
{
BOOL    cooling=FALSE;

    if( ROBOT_Check(ID) )
    {
        cooling = ROBOT_Item[ID].Robot->TempTrakCooling();
    }

    return(cooling);
}

/******************************************************************************/

BOOL    ROBOT_Ping( int ID )
{
BOOL    ok=TRUE;

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Safety( int ID )
{
BOOL ok=TRUE;

    // Read safety line from controller...
    if( !ROBOT_Safe(ID) )
    {
        ok = FALSE;
    }

    STR_printf(ok,ROBOT_debugf,ROBOT_errorf,"ROBOT_Safety(%s) %s.\n",ROBOT_Item[ID].name,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

void ROBOT_ControllerClose( int ID )
{
    // Make sure handle is valid...
    if( !ROBOT_Check(ID) )
    {
        return;
    }

    switch( ROBOT_Item[ID].Robot->Controller->HostCardType() )
    {
        case CONTROLLER_HOSTCARD_MOUSE :
            MOUSE_Close();
            break;

        case CONTROLLER_HOSTCARD_OPTOTRAK :
#ifdef ROBOT_OPTO
            OPTO_Stop();
            OPTO_Close();
            ROBOT_OptoTrakReadLatency.Results();
#endif
            break;
    }

    if( ROBOT_Item[ID].Robot != NULL )
    {
        ROBOT_Item[ID].Robot->Ramp->Close();
        ROBOT_Item[ID].Robot->Close();
        delete ROBOT_Item[ID].Robot;
        ROBOT_Item[ID].Robot = NULL;
    }
}

/******************************************************************************/

BOOL ROBOT_ControllerOpen( int ID )
{
BOOL ok=TRUE;

    ROBOT_Item[ID].Robot = new ROBOT(&ROBOT_Item[ID].Hardware,ROBOT_Item[ID].name);

    if( !ROBOT_Item[ID].Robot->Open() )
    {
        ok = FALSE;
    }
    else
    if( !ROBOT_Item[ID].Robot->Ramp->Open(ROBOT_RampTime[ID],0.0,ROBOT_RampFunc[ID]) )
    {
        ROBOT_errorf("ROBOT_Open(%s) Cannot open RAMP object.\n",ROBOT_Item[ID].name);
        ok = FALSE;
    }
    else
    { 
        switch( ROBOT_Item[ID].Robot->Controller->HostCardType() )
        {
            case CONTROLLER_HOSTCARD_MOUSE :
                ok = MOUSE_Open();
                break;

            case CONTROLLER_HOSTCARD_OPTOTRAK :
#ifdef ROBOT_OPTO
                if( !OPTO_Open(STR_stringf("%s.OPT",ROBOT_Item[ID].name)) )
                {
                    ok = FALSE;
                }
                else
                {
                    ok = OPTO_Start();
                }
#endif
                break;
        }
    }

    if( !ok )
    {
        ROBOT_errorf("ROBOT_Open(%s) Cannot open controller.\n",ROBOT_Item[ID].name);
        ROBOT_ControllerClose(ID);
    }

    return(ok);
}

/******************************************************************************/

int ROBOT_Open( char *name )
{
int ID;
BOOL ok;

    // Make sure API is running...
    if( !ROBOT_Check() )
    {
        return(ROBOT_INVALID);
    }

    ROBOT_debugf("ROBOT_Open(%s)\n",name);

    // Find a free handle ID...
    if( (ID=ROBOT_Free()) == ROBOT_INVALID )
    {
        ROBOT_errorf("ROBOT_Open(%s) No free handles.\n",name);
        return(ROBOT_INVALID);
    }

    ROBOT_debugf("ID=%d\n",ID);

    // Clear the handle item...
    ROBOT_Init(ID);

    // Set default values before reading configuration files.
    ROBOT_Defaults(ID);

    // Load configuration variables from each file type...
    if( !ROBOT_CnfgLoad(ID,name) )
    {
        return(ROBOT_INVALID);
    }

    if( ROBOT_LoopTaskFrequency[ID] == 0.0 )
    {
        ROBOT_LoopTaskFrequency[ID] = LOOPTASK_frequency();
    }

    ROBOT_LoopTaskPeriod[ID] = 1.0 / ROBOT_LoopTaskFrequency[ID];

    ROBOT_invROMX[ID] = SPMX_rtmx2romx(inv(ROBOT_RTMX[ID]));

    // Set unique name for robot...
    strncpy(ROBOT_Item[ID].name,ROBOT_KeyWord[ID],STRLEN);

    // Set hardware details for controller (V4.0)...
    if( !ROBOT_Hardware(ID) )
    {
        return(ROBOT_INVALID);
    }

    // Check if interface operating...
    if( !ROBOT_Ping(ID) )
    {
        ROBOT_errorf("ROBOT_Open(%s) Interface not operating.\n",name);
        return(ROBOT_INVALID);
    }

    // Create robot controller and open...
    if( !ROBOT_ControllerOpen(ID) )
    {
        return(ROBOT_INVALID);
    }

    ROBOT_debugf("ROBOT %s.\n",ROBOT_Item[ID].name);

    // Zero ramping...
    ROBOT_Item[ID].Robot->Ramp->Reset();

    // Make sure motor torques are zero (reset)...
    ROBOT_MotorReset(ID);

    // Set default handle ID to this one...
    ROBOT_Use(ID);

    return(ID);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( long EU[], long MU[] ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_RAW,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( matrix &p ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_MP,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( matrix &p, matrix &f ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_MPF,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( matrix &p, matrix &v, matrix &f ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_MPVF,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_MPVAF,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( void ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_VOID,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( double p[] ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_DP,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( double p[], double f[] ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_DPF,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( double p[], double v[], double f[] ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_DPVF,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID, void (*func)( double p[], double v[], double a[], double f[] ) )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_DPVAF,func);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Start( int ID )
{
BOOL    ok;

    ok = ROBOT_Start(ID,ROBOT_CONTROL_NONE,NULL);

    return(ok);
}

/******************************************************************************/

BOOL    ROBOT_Enable( int ID )
{
BOOL    ok=FALSE;

    // Turn amplifiers on...
    if( ROBOT_Check(ID) )
    {
        ok = ROBOT_Item[ID].Robot->Start();
    }

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_Disable( int ID )
{
BOOL ok=TRUE;

    // Turn amplifiers off...
    if( ROBOT_Check(ID) )
    {
        ROBOT_Item[ID].Robot->Stop();
    }

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_Start( int ID, int type, void *func )
{
    if( !ROBOT_Check(ID) )                // Check if API started and handle ID okay...
    {
        return(FALSE);
    }

    if( ROBOT_Started(ID) )               // Already started...
    {
        return(TRUE);
    }

    // Do safety check here... (V3.0)
    if( !ROBOT_Safety(ID) )
    {
        ROBOT_errorf("ROBOT_Start(%s) Failed safety check.\n",ROBOT_Item[ID].name);
        return(FALSE);
    }

    // Check if encoder reset required.
    if( !ROBOT_reset[ID] )
    {
        ROBOT_errorf("ROBOT_Start(%s) Robot encoders need to be reset.\n",ROBOT_Item[ID].name);
        return(FALSE);
    }

    if( !ROBOT_Enable(ID) )               // Turn motor controller on...
    {
        ROBOT_errorf("ROBOT_Start(%s) Cannot enable.\n",ROBOT_Item[ID].name);
        return(FALSE);
    }

    ROBOT_LoopTaskLatencyTimer[ID].Reset();
    ROBOT_LoopTaskLatency[ID] = 0.0;

    // Set force calculation function for motor loop...
    ROBOT_ControlSet(ID,type,func);

    if( !ROBOT_LoopTaskStart(ID) )        // Start LoopTask functions...
    {
        ROBOT_LoopTaskStop(ID);           // Failed...

        ROBOT_errorf("ROBOT_Start(%s) Cannot start LoopTask.\n",ROBOT_Item[ID].name);
        return(FALSE);
    }

    if( !ROBOT_Item[ID].Robot->Ramp->Start() )
    {
        ROBOT_errorf("ROBOT_Start(%s) Cannot start RAMP object.\n",ROBOT_Item[ID].name);
        return(FALSE);
    }

    if( !ROBOT_Item[ID].Robot->TempTrakStart() )
    {
        ROBOT_errorf("ROBOT_Start(%s) Cannot start TEMPTRAK object.\n",ROBOT_Item[ID].name);
        return(FALSE);
    }

    ROBOT_Item[ID].started = TRUE;

    return(TRUE);
}

/******************************************************************************/

int ROBOT_Start( char *name, int type, void *func )
{
int ID;

    if( (ID=ROBOT_Open(name)) == ROBOT_INVALID )
    {
        return(ROBOT_INVALID);
    }

    if( !ROBOT_Start(ID,type,func) )
    {
        return(ROBOT_INVALID);
    }

    return(ID);
}

/******************************************************************************/

int ROBOT_Start( char *name, void (*func)( long EU[], long MU[] ) )
{
BOOL ok;

    ok = ROBOT_Start(name,ROBOT_CONTROL_RAW,func);

    return(ok);
}

/******************************************************************************/

int ROBOT_Start( char *name, void (*func)( matrix &p ) )
{
BOOL ok;

    ok = ROBOT_Start(name,ROBOT_CONTROL_MP,func);

    return(ok);
}

/******************************************************************************/

int ROBOT_Start( char *name, void (*func)( matrix &p, matrix &f ) )
{
BOOL ok;

    ok = ROBOT_Start(name,ROBOT_CONTROL_MPF,func);

    return(ok);
}

/******************************************************************************/

int ROBOT_Start( char *name, void (*func)( matrix &p, matrix &v, matrix &f ) )
{
BOOL ok;

    ok = ROBOT_Start(name,ROBOT_CONTROL_MPVF,func);

    return(ok);
}

/******************************************************************************/

int ROBOT_Start( char *name, void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) )
{
BOOL ok;

    ok = ROBOT_Start(name,ROBOT_CONTROL_MPVAF,func);

    return(ok);
}

/******************************************************************************/

int ROBOT_Start( char *name )
{
BOOL ok;

    ok = ROBOT_Start(name,ROBOT_CONTROL_NONE,NULL);

    return(ok);
}

/******************************************************************************/

void ROBOT_Stop( int ID )
{
    // Check if API started and handle ID okay...
    if( !ROBOT_Check(ID) )
    {
        return;
    }

    // Stop looptask functions...
    ROBOT_LoopTaskStop(ID);

    // Zero motor torques just to be safe (V2.6)...
    ROBOT_MotorReset(ID);

    ROBOT_Item[ID].Robot->Ramp->Stop();
    ROBOT_Item[ID].Robot->TempTrakStop();

    // Turn motor controller off...
    ROBOT_Disable(ID);             

    // Started flag...       
    ROBOT_Item[ID].started = FALSE;
}

/******************************************************************************/

void ROBOT_Close( int ID )
{
    // Stop ROBOT first...
    ROBOT_Stop(ID);

    // Close and delete robot controller...
    ROBOT_ControllerClose(ID);

    // Clear handle...
    ROBOT_Init(ID);
}

/******************************************************************************/

void ROBOT_CloseAll( void )
{
int ID;

    for( ID=0; (ID < ROBOT_MAX); ID++ )
    {
        ROBOT_Close(ID);
    }
}

/******************************************************************************/

BOOL ROBOT_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL ok;
int ID;

    if( ROBOT_API_started )               // Start the API once...
    {
        return(TRUE);
    }

    ROBOT_PRN_messgf = messgf;            // General API message print function.
    ROBOT_PRN_errorf = errorf;            // API error message print function.
    ROBOT_PRN_debugf = debugf;            // Debug information print function.

    for( ID=0; (ID < ROBOT_MAX); ID++ )
    {
        ROBOT_Init(ID);
    }

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(ROBOT_API_stop);       // Install stop function.
        ROBOT_API_started = TRUE;         // Set started flag.

        MODULE_start(ROBOT_PRN_messgf);   // Register module.
    }
    else
    {
        ROBOT_errorf("ROBOT_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void ROBOT_API_stop( void )
{
    if( !ROBOT_API_started )         // API not started in the first place...
    {
         return;
    }

    ROBOT_CloseAll();

    ROBOT_API_started = FALSE;       // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL ROBOT_Check( void )
{
BOOL ok=TRUE;

    if( !ROBOT_API_started )         // API not started...
    {                                // Start module automatically...
        ok = ROBOT_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        ROBOT_debugf("ROBOT_Check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/******************************************************************************/

BOOL ROBOT_Check( int ID )
{
BOOL ok=TRUE;

    if( !ROBOT_Check() )
    {
        ok = FALSE;
    }
    else
    if( ID == ROBOT_INVALID )
    {
        ok = FALSE;
    }
    else
    if( ROBOT_Item[ID].Robot == NULL )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

void ROBOT_Info( int ID, PRINTF prnf )
{
    if( ROBOT_Check(ID) )                 // Check if API started and handle ID okay...
    {
        ROBOT_CnfgPrnt(ID,prnf);
    }
}

/******************************************************************************/

void ROBOT_PanicLoop( int ID )
{
    ROBOT_Item[ID].HWpanic = !ROBOT_Item[ID].Robot->Activated();
}

/******************************************************************************/

void ROBOT_Jacobian( int ID )
{
double link[2];

    // Convert link lengths from centimetres to metres...  
    link[ROBOT_LINK_1] = ROBOT_link[ID][ROBOT_LINK_1] / 100.0;
    link[ROBOT_LINK_2] = ROBOT_link[ID][ROBOT_LINK_2] / 100.0;

    ROBOT_J[ID](1,1) =  ROBOT_cos[ID][ROBOT_AXIS_1] * (link[ROBOT_LINK_1] * ROBOT_cos[ID][ROBOT_AXIS_2] + link[ROBOT_LINK_2] * ROBOT_cos[ID][ROBOT_AXIS_3]);
    ROBOT_J[ID](2,1) = -ROBOT_sin[ID][ROBOT_AXIS_1] * (link[ROBOT_LINK_1] * ROBOT_cos[ID][ROBOT_AXIS_2] + link[ROBOT_LINK_2] * ROBOT_cos[ID][ROBOT_AXIS_3]);
    ROBOT_J[ID](3,1) =  0;
    ROBOT_J[ID](1,2) =  link[ROBOT_LINK_1] * -ROBOT_sin[ID][ROBOT_AXIS_2] * ROBOT_sin[ID][ROBOT_AXIS_1];
    ROBOT_J[ID](2,2) =  link[ROBOT_LINK_1] * -ROBOT_sin[ID][ROBOT_AXIS_2] * ROBOT_cos[ID][ROBOT_AXIS_1];
    ROBOT_J[ID](3,2) =  link[ROBOT_LINK_1] *  ROBOT_cos[ID][ROBOT_AXIS_2];
    ROBOT_J[ID](1,3) =  link[ROBOT_LINK_2] * -ROBOT_sin[ID][ROBOT_AXIS_3] * ROBOT_sin[ID][ROBOT_AXIS_1];
    ROBOT_J[ID](2,3) =  link[ROBOT_LINK_2] * -ROBOT_sin[ID][ROBOT_AXIS_3] * ROBOT_cos[ID][ROBOT_AXIS_1];
    ROBOT_J[ID](3,3) =  link[ROBOT_LINK_2] * -ROBOT_cos[ID][ROBOT_AXIS_3];

    ROBOT_JT[ID] = T(ROBOT_J[ID]);
}

/******************************************************************************/

void ROBOT_Force( int ID, matrix &f )
{
BOOL ok;
int axis;
double  fm,xyz[AXIS_XYZ],api[AXIS_XYZ],Nm[ROBOT_DOFS];
matrix  T(3,1),F(3,1);

    // Forces (fx,fy,fz) are in Newtons in cartesian space...
    // This function converts them to torques for the various motors...

    xyz[AXIS_X] = f(1,1);
    xyz[AXIS_Y] = f(2,1);
    xyz[AXIS_Z] = f(3,1);

    // Map from Application xyz to API xyz...
    ROBOT_XYZ2API(ID,xyz,api);

    F(1,1) = api[AXIS_X];
    F(2,1) = api[AXIS_Y];
    F(3,1) = api[AXIS_Z];

    // Specific processing for different types of manipulanda (V5.4).
    switch( ROBOT_Item[ID].ManipulandumType )
    {
        case ROBOT_MANIPULANDUM_MULTIAXIS : // vBOT, Phantom, etc.
           // Torques (Nm) are product of Jacobian transposed * force.
           T = ROBOT_JT[ID] * F;

           fm = norm(F);
           break;

        case ROBOT_MANIPULANDUM_SINGLEAXIS : // Single axis force lever actuators.
           // Torque (Nm) is product of link length and force.
           for( axis=0; (axis < AXIS_XYZ); axis++ )
           {
               T(axis+1,1) = F(axis+1,1) * (ROBOT_link[ID][axis]/100.0);
           }

           fm = mmax(F);
           break;

        default :
            ROBOT_errorf("Unknown manipulandum type: %d\n",ROBOT_Item[ID].ManipulandumType);
            ROBOT_UnSafe(ID);     // Set robot unsafe flag.
            break;
    }

    // Set safe flag to FALSE if forces exceed hard-coded maximum...
    if( ROBOT_Item[ID].safe && (fm > ROBOT_FORCE_MAX) )
    {
        ROBOT_errorf("Robot=%s Maximum force (%.1f N) exceeded.\n",ROBOT_KeyWord[ID],ROBOT_FORCE_MAX);
        ROBOT_errorf("Fx=%.1lf, Fy=%.1lf, Fz=%.1lf\n",F(1,1),F(2,1),F(3,1));

        // Set robot unsafe flag.
        ROBOT_UnSafe(ID);
    }

    if( !ROBOT_Item[ID].safe )
    {
        return;
    }

    // Clear torque values.
    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        Nm[axis] = 0.0;
    }

    // Set torque values.
    for( axis=0; (axis < AXIS_XYZ); axis++ )
    {
        Nm[axis] = T(axis+1,1);
    }

    // Now put the torque values out to the motors...
    ROBOT_MotorSetNm(ID,Nm);
}

/******************************************************************************/

BOOL    ROBOT_LoopTaskStart( int ID )
{
BOOL    ok=FALSE;

    if( LOOPTASK_start(ROBOT_LoopTask[ID],ROBOT_LoopTaskFrequency[ID]) )
    {
        ok = TRUE;
        ROBOT_LoopTaskFrequency[ID] = LOOPTASK_frequency(ROBOT_LoopTask[ID]);
        ROBOT_Item[ID].Robot->LoopTaskFrequency(ROBOT_LoopTaskFrequency[ID]);
    }

    STR_printf(ok,ROBOT_debugf,ROBOT_errorf,"ROBOT_LoopTaskStart(ID=%d) %s %.1lf Hz.\n",ID,STR_OkFailed(ok),ROBOT_LoopTaskFrequency[ID]);

    return(ok);
}

/******************************************************************************/

void    ROBOT_LoopTaskStop( int ID )
{
    LOOPTASK_stop(ROBOT_LoopTask[ID]);
}

/******************************************************************************/

double ROBOT_LoopTaskGetFrequency( int ID )
{
double freq=0.0;

    if( ROBOT_Check(ID) )
    {
        freq = LOOPTASK_frequency(ROBOT_LoopTask[ID]);
    }

    return(freq);
}

/******************************************************************************/

double ROBOT_LoopTaskGetPeriod( int ID )
{
double freq,period=0.0;

    if( (freq=ROBOT_LoopTaskGetFrequency(ID)) != 0.0 )
    {
        period = 1.0/freq;
    }

    return(period);
}

/******************************************************************************/

void ROBOT_AnglesRaw( int ID, matrix &AP, matrix &AV, matrix &AA )
{
    ROBOT_Angles(ID,AP,AV,AA,FALSE); // FALSE = Don't apply angular offset.
}

/******************************************************************************/

void ROBOT_AnglesRaw( int ID, double angpos[], double angvel[], double angacc[] )
{
    ROBOT_Angles(ID,angpos,angvel,angacc,FALSE); // FALSE = Don't apply angular offset.
}

/******************************************************************************/

void ROBOT_AnglesRaw( int ID, matrix &AP )
{
matrix AV,AA;

    ROBOT_AnglesRaw(ID,AP,AV,AA);
}

/******************************************************************************/

void ROBOT_AnglesRaw( int ID, double angpos[] )
{
double angvel[ROBOT_DOFS];
double angacc[ROBOT_DOFS];

    ROBOT_AnglesRaw(ID,angpos,angvel,angacc);
}

/******************************************************************************/

void ROBOT_Angles( int ID, matrix &AP )
{
matrix AV,AA;

    ROBOT_Angles(ID,AP,AV,AA);
}

/******************************************************************************/

void ROBOT_Angles( int ID, matrix &AP, matrix &AV, matrix &AA )
{
    ROBOT_Angles(ID,AP,AV,AA,TRUE); // TRUE = Apply angular offset.
}

/******************************************************************************/

void ROBOT_Angles( int ID, double angpos[] )
{
double angvel[ROBOT_DOFS];
double angacc[ROBOT_DOFS];

    ROBOT_Angles(ID,angpos,angvel,angacc);
}

/******************************************************************************/

void ROBOT_Angles( int ID, double angpos[], double angvel[], double angacc[] )
{
    ROBOT_Angles(ID,angpos,angvel,angacc,TRUE); // TRUE = Apply angular offset.
}

/******************************************************************************/

void ROBOT_Angles( int ID, matrix &AP, matrix &AV, matrix &AA, BOOL offset )
{
static double angpos[ROBOT_DOFS];
static double angvel[ROBOT_DOFS];
static double angacc[ROBOT_DOFS];

    ROBOT_Angles(ID,angpos,angvel,angacc,offset);

    AP.dim(3,1);
    AP(1,1) = angpos[0];
    AP(2,1) = angpos[1];
    AP(3,1) = angpos[2];

    AV.dim(3,1);
    AV(1,1) = angvel[0];
    AV(2,1) = angvel[1];
    AV(3,1) = angvel[2];

    AA.dim(3,1);
    AA(1,1) = angvel[0];
    AA(2,1) = angvel[1];
    AA(3,1) = angvel[2];
}

/******************************************************************************/

void ROBOT_Angles( int ID, double angpos[], double angvel[], double angacc[], BOOL offset )
{
int  axis,index;
double xraw,x,dx,ddx;
//printf("",angpos[]);
//  Looking at the robot...
//  Axis 1 (encoder 0) is the base -ve is left and +ve is right.
//  Axis 2 (encoder 1) is the lower on big wheel +ve is away and -ve is near.
//  Axis 3 (encoder 2) is the upper on big wheel +ve is down and -ve is up.
  
//  angle[0] +ve to left -ve to right.
//  angle[1] -ve up and +ve down.
//  angle[2] link 2 to horizontal -90 at right angle and -ve away and +ve towards.

    for( axis=0; (axis < ROBOT_AXIS); axis++ )
    {
        angpos[axis] = 0.0;
        angvel[axis] = 0.0;
        angacc[axis] = 0.0;
		//printf("============%G,%G,%G\n",angpos[0],angpos[1],angpos[2]);

        // Read encoder if this axis is used...
        if( (index=ROBOT_AxisIndex[ID][axis]) != -1 )
        {
            // Get encoder reading.
            ROBOT_Encoder(ID,index,xraw,x,dx,ddx);

			//printf("xraw %G, x %G, dx %G , ddx %G\n", xraw, x, dx, ddx);
            // Conver encoder values to radians.
            switch( ROBOT_Item[ID].FilterType )
            {
                case ROBOT_FILTER_NONE :
                case ROBOT_FILTER_QUADRATICFIT :
                case ROBOT_FILTER_EKF :
                   angpos[axis] = ROBOT_ConvEU2Rad(ID,index,xraw);
                   break;

                case ROBOT_FILTER_KALMANFILTER :
					
                   if( ROBOT_FilterPosition[ID] )
                   {
                       angpos[axis] = ROBOT_ConvEU2Rad(ID,index,x);
                   }
                   else
                   {					
                       angpos[axis] = ROBOT_ConvEU2Rad(ID,index,xraw);
                   }

                   angvel[axis] = ROBOT_ConvEU2Rad(ID,index,dx);
                   angacc[axis] = ROBOT_ConvEU2Rad(ID,index,ddx);
                   break;
            }
        }

		//printf("AFTER eu2rad angpos : [%G %G %G]\n", angpos[0], angpos[1], angpos[2]);
        // Adjust 3rd axis by -90 degrees (because it's like a bent elbow at zero)...
        if( (axis == ROBOT_AXIS_3) && (ROBOT_Item[ID].ManipulandumType == ROBOT_MANIPULANDUM_MULTIAXIS) )
        {
            angpos[axis] = r90 - angpos[axis];
            angvel[axis] = -angvel[axis];
            angacc[axis] = -angacc[axis];
        }

		//printf("after adjust direction angpos : [%G %G %G]\n", angpos[0], angpos[1], angpos[2]);
        // Adjustment for encoder angle offsets...
        if( offset )
        {
			//printf("offset is ture\n");
            angpos[axis] += ROBOT_AngleOffset[ID][axis];
        }
    }
}

/******************************************************************************/

void ROBOT_ManipulandumPosition( int ID, double angle[], double link[], double xyz[], double offset[] )
{
int axis;

    switch( ROBOT_Item[ID].ManipulandumType )
    {
        case ROBOT_MANIPULANDUM_MULTIAXIS : // vBOT, Phantom, etc.
			//printf("phantom case\n");
            for( axis=0; (axis < ROBOT_AXIS); axis++ )
            {
                ROBOT_cos[ID][axis] = cos(angle[axis]);
                ROBOT_sin[ID][axis] = sin(angle[axis]);
				//printf("calculate cos %G, sin %G\n", ROBOT_cos[ID][axis], ROBOT_sin[ID][axis]);
            }
   
            ROBOT_Y_COS[ID] = (link[ROBOT_LINK_1] * ROBOT_cos[ID][ROBOT_AXIS_2]) +
                              (link[ROBOT_LINK_2] * ROBOT_cos[ID][ROBOT_AXIS_3]);

            ROBOT_Z_SIN[ID] = (link[ROBOT_LINK_1] * ROBOT_sin[ID][ROBOT_AXIS_2]) -
                              (link[ROBOT_LINK_2] * ROBOT_sin[ID][ROBOT_AXIS_3]);

            // API x,y,z positions for tip of arm...
            xyz[AXIS_X] = ROBOT_Y_COS[ID] * ROBOT_sin[ID][ROBOT_AXIS_1];
            xyz[AXIS_Y] = ROBOT_Y_COS[ID] * ROBOT_cos[ID][ROBOT_AXIS_1];
            xyz[AXIS_Z] = ROBOT_Z_SIN[ID];
            break;

        case ROBOT_MANIPULANDUM_SINGLEAXIS : // Single axis force lever actuators.
            for( axis=0; (axis < AXIS_XYZ); axis++ )
            {
                // Arc length position of lever (cm).
                xyz[axis] = angle[axis] * link[axis];
            }
            break;

        default :
            break;
    }
}

/******************************************************************************/

void ROBOT_AngularToCartesianPosition( int ID, double angle[], double link[], double xyz[], double offset[] )
{
int axis;
static double api[AXIS_XYZ];
//printf("api xyz: %G,%G,%G\n",api[0],api[1],api[2]);
static matrix posn(4,1),seen(1,1);
BOOL ok;

    switch( ROBOT_Item[ID].Robot->Controller->HostCardType() )
    {
        case CONTROLLER_HOSTCARD_MOUSE :
			
            ok = MOUSE_GetPosn(posn);
            if( ok )
            {
                api[AXIS_X] = posn(1,1);
                api[AXIS_Y] = posn(2,1);
                api[AXIS_Z] = posn(3,1);
            }
            break;

        case CONTROLLER_HOSTCARD_OPTOTRAK :
			
#ifdef ROBOT_OPTO
            ROBOT_OptoTrakReadLatency.Before();
            ROBOT_OptoTrakFrame = OPTO_GetPosn(posn,seen);
            ROBOT_OptoTrakReadLatency.After();

            if( seen.B(1,1) )
            {
                api[AXIS_X] = posn(1,1);
                api[AXIS_Y] = posn(2,1);
                api[AXIS_Z] = posn(3,1);
            }
#endif
            break;

        default :
            ROBOT_ManipulandumPosition(ID,angle,link,api,offset);
            break;
    }

    // Convert API xyz to Application xyz...
    ROBOT_API2XYZ(ID,api,xyz,offset);
}

/******************************************************************************/

void ROBOT_AngularToCartesianVelocity( int ID, double angvel[], double xyz[] )
{
static matrix AV(3,1),V(3,1);
static double api[ROBOT_AXIS];
int axis;

    AV(1,1) = angvel[0];
    AV(2,1) = angvel[1];
    AV(3,1) = angvel[2];

    switch( ROBOT_Item[ID].ManipulandumType )
    {
        case ROBOT_MANIPULANDUM_MULTIAXIS : // vBOT, Phantom, etc.
            // Calculate translation velocity from angular velocity.
            V = ROBOT_J[ID] * AV;
            // Convert to cm / sec.
            V *= 100.0;
            break;

        case ROBOT_MANIPULANDUM_SINGLEAXIS : // Single axis force lever actuators.
            for( axis=0; (axis < AXIS_XYZ); axis++ )
            {
                // Arc length velocity of lever (cm/sec).
                V(axis+1,1) = AV(axis+1,1) * ROBOT_link[ID][axis];
            }
            break;

        default :
            break;
    }

    api[0] = V(1,1);
    api[1] = V(2,1);
    api[2] = V(3,1);

    ROBOT_API2XYZ(ID,api,xyz);
}

/******************************************************************************/

void ROBOT_AngularToCartesianAcceleration( int ID, double angacc[], double xyz[] )
{
}

/******************************************************************************/

void ROBOT_Position( int ID, double p_xyz[], double v_xyz[], double a_xyz[] )
{
static double angpos[ROBOT_AXIS];
static double angvel[ROBOT_AXIS];
static double angacc[ROBOT_AXIS];
static double xraw[ROBOT_AXIS];
static double xfilt[ROBOT_AXIS];
static double dxfilt[ROBOT_AXIS];
static double ddxfilt[ROBOT_AXIS];
int i;
    ROBOT_Angles(ID,angpos,angvel,angacc,TRUE);

    // Calculate Jacobian from current angles.
    ROBOT_Jacobian(ID);

	//printf("before tf p_xyz : [%G %G %G]\n",p_xyz[0],p_xyz[1],p_xyz[2]);
	//printf("before tf angpos : [%G %G %G]\n", angpos[0], angpos[1], angpos[2]);
	//printf("link length %G,%G,%G\n", ROBOT_link[ID][0], ROBOT_link[ID][1], ROBOT_link[ID][2]);
	//printf("offset xyz %G,%G,%G\n", ROBOT_PositionOffset[ID][0], ROBOT_PositionOffset[ID][1], ROBOT_PositionOffset[ID][2]);
	
    // Calculate position.
    ROBOT_AngularToCartesianPosition(ID,angpos,ROBOT_link[ID],p_xyz,ROBOT_PositionOffset[ID]);

    if( (v_xyz == NULL) || (a_xyz == NULL) )
    {
        return;
    }

    switch( ROBOT_Item[ID].FilterType )
    {
        case ROBOT_FILTER_NONE :
           break;

        case ROBOT_FILTER_QUADRATICFIT :
           ROBOT_QuadraticFit(ID,p_xyz,NULL,v_xyz,a_xyz);
           break;

        case ROBOT_FILTER_KALMANFILTER :
           ROBOT_AngularToCartesianVelocity(ID,angvel,v_xyz);
           ROBOT_AngularToCartesianAcceleration(ID,angvel,a_xyz);
           break;

        case ROBOT_FILTER_EKF :
           switch( ROBOT_TDOF(ID) )
           {
               case 2 :
                  xraw[0] = angpos[1];
                  xraw[1] = angpos[2];
                  ROBOT_Item[ID].EKF->Calculate(xraw,xfilt,dxfilt,ddxfilt);
                  for( i=0; (i < 2); i++ )
                  {
                      if( ROBOT_FilterPosition[ID] )
                      {
                          p_xyz[i] = (xfilt[i]*100.0) + ROBOT_PositionOffset[ID][i];
                      }

                      v_xyz[i] = dxfilt[i]*100.0;
                      a_xyz[i] = ddxfilt[i]*100.0;
                  }
                  break;

               case 3 :
                  break;
           }
           break;
    }
}

/******************************************************************************/

void ROBOT_Position( int ID, double p_xyz[] )
{
    ROBOT_Position(ID,p_xyz,NULL,NULL);
}

/******************************************************************************/

void ROBOT_Position( int ID, matrix &P, matrix &V, matrix &A )
{
static double p_xyz[AXIS_XYZ]={0,0,0},v_xyz[AXIS_XYZ]={0,0,0},a_xyz[AXIS_XYZ]={0,0,0};

    ROBOT_Position(ID,p_xyz,v_xyz,a_xyz);

    P.dim(3,1);
    P(1,1) = p_xyz[0];
    P(2,1) = p_xyz[1];
    P(3,1) = p_xyz[2];

    V.dim(3,1);
    V(1,1) = v_xyz[0];
    V(2,1) = v_xyz[1];
    V(3,1) = v_xyz[2];

    A.dim(3,1);
    A(1,1) = a_xyz[0];
    A(2,1) = a_xyz[1];
    A(3,1) = a_xyz[2];
}

/******************************************************************************/

void ROBOT_Position( int ID, matrix &P )
{
static double p_xyz[AXIS_XYZ]={0,0,0},v_xyz[AXIS_XYZ]={0,0,0},a_xyz[AXIS_XYZ]={0,0,0};

    ROBOT_Position(ID,p_xyz,NULL,NULL);

    P.dim(3,1);
    P(1,1) = p_xyz[0];
    P(2,1) = p_xyz[1];
    P(3,1) = p_xyz[2];
}

/******************************************************************************/
/* Calculate position of OptoTrak marker at tip of arm.                       */
/******************************************************************************/

void ROBOT_PositionMarker( int ID, int marker, double angpos[], double xyz[] )
{
static double link[ROBOT_LINK];

    link[0] = ROBOT_link[ID][0];
    link[1] = ROBOT_marker[ID][marker];

    ROBOT_AngularToCartesianPosition(ID,angpos,link,xyz,ROBOT_PositionOffset[ID]);
}

/******************************************************************************/

void ROBOT_PositionMarker( int ID, int marker, double xyz[] )
{
static double angpos[ROBOT_AXIS];

    ROBOT_Angles(ID,angpos);
    ROBOT_PositionMarker(ID,marker,angpos,xyz);
}

/******************************************************************************/

void ROBOT_PositionMarker( int ID, int marker, matrix &P )
{
double xyz[AXIS_XYZ];

    ROBOT_PositionMarker(ID,marker,xyz);
    SPMX_xyz2mtx(xyz,P);
}

/******************************************************************************/

void ROBOT_QuadraticFit( int ID, matrix &P, matrix &p, matrix &v, matrix &a )
{
    // Put current position into window.
    ROBOT_Item[ID].QuadraticFitWindow->Point(P,LOOPTASK_dt);

    // Fit quadratic to calculate velocity and acceleration.
    ROBOT_Item[ID].QuadraticFitWindow->QuadraticFit(p,v,a);
}

/******************************************************************************/

void ROBOT_QuadraticFit( int ID, double praw_xyz[], double p_xyz[], double v_xyz[], double a_xyz[] )
{
static matrix Praw(3,1),P(3,1),V(3,1),A(3,1);
int i;

    for( i=0; (i < 3); i++ )
    {
        Praw(i+1,1) = praw_xyz[i];
    }

    // Put current position into window.
    ROBOT_Item[ID].QuadraticFitWindow->Point(Praw,LOOPTASK_dt);

    // Fit quadratic to calculate velocity and acceleration.
    ROBOT_Item[ID].QuadraticFitWindow->QuadraticFit(P,V,A);

    for( i=0; (i < 3); i++ )
    {
        if( p_xyz != NULL )
        {
            p_xyz[i] = P(i+1,1);
        }

        v_xyz[i] = V(i+1,1);
        a_xyz[i] = A(i+1,1);
    }
}

/******************************************************************************/

void ROBOT_dXdt( int ID, matrix &P, matrix &v )
{
    // Put current data into window...
    ROBOT_Item[ID].QuadraticFitWindow->Point(P,LOOPTASK_dt);
    ROBOT_Item[ID].QuadraticFitWindow->dXdt(v);
}

/******************************************************************************/

UINT ROBOT_OptoTrakFrameLast( int ID )
{
UINT frame=0;

    if( ROBOT_Item[ID].Robot->Controller->HostCardType() == CONTROLLER_HOSTCARD_OPTOTRAK )
    {
        frame = ROBOT_OptoTrakFrame;
    }

    return(frame);
}

/******************************************************************************/

double ROBOT_LoopTaskLatencyGet( int ID )
{
double value=0.0;

    if( ROBOT_Check(ID) )
    {
        value = ROBOT_LoopTaskLatency[ID];
    }

    return(value);
}

/******************************************************************************/

#include "robot-default.cpp"         // General function for accessing default ROBOT ID.

/******************************************************************************/

