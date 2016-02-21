 /******************************************************************************/
/*                                                                            */
/* MODULE  : PHANToM.cpp                                                      */
/*                                                                            */
/* PURPOSE : PHANToM haptic interface functions.                              */
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
/******************************************************************************/

#define MODULE_NAME     "PHANTOM"
#define MODULE_TEXT     "PHANTOM Haptic Interface API"
#define MODULE_DATE     "27/09/2004"
#define MODULE_VERSION  "4.3"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>
#include <phantom.h>
#include <daqft.h>

/******************************************************************************/

BOOL    PHANTOM_API_started = FALSE;

/******************************************************************************/

PRINTF  PHANTOM_PRN_messgf=NULL;                 // General messages printf function.
PRINTF  PHANTOM_PRN_errorf=NULL;                 // Error messages printf function.
PRINTF  PHANTOM_PRN_debugf=NULL;                 // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     PHANTOM_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(PHANTOM_PRN_messgf,buff));
}

/******************************************************************************/

int     PHANTOM_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(PHANTOM_PRN_errorf,buff));
}

/******************************************************************************/

int     PHANTOM_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(PHANTOM_PRN_debugf,buff));
}

/******************************************************************************/
/* Configuration and calibration parameters for PHANToM...                    */
/******************************************************************************/

char    PHANTOM_CnfgPath[MAXPATH]="";

struct  PHANTOM_CnfgFile     PHANTOM_Cnfg[] =
{
    { "CAL",TRUE ,PHANTOM_CnfgCAL,PHANTOM_TestCAL },
    { "CFG",TRUE ,PHANTOM_CnfgCFG,PHANTOM_TestCFG },
    { NULL ,FALSE,NULL           ,NULL            }
};

/******************************************************************************/

float   PHANTOM_link[PHANTOM_MAX][PHANTOM_LINK];           // Length of [N]th link in arm.
float   PHANTOM_marker[PHANTOM_MAX][PHANTOM_MARKER];       // Distance to [N]th OptoTrak marker on 2nd link.

float   PHANTOM_base[PHANTOM_MAX][AXIS_XYZ];               // XYZ position of base.
float   PHANTOM_angle[PHANTOM_MAX][PHANTOM_AXIS];          // Angular offset for each encoder.
matrix  PHANTOM_RTMX[PHANTOM_MAX];
matrix  PHANTOM_invROMX[PHANTOM_MAX];

#define PHANTOM_CONTROLLER   3
STRING  PHANTOM_controller[PHANTOM_MAX][PHANTOM_CONTROLLER];
STRING  PHANTOM_motors[PHANTOM_MAX][ROBOT_AXIS];
STRING  PHANTOM_encoders[PHANTOM_MAX][ROBOT_AXIS];

float   PHANTOM_signXYZ[PHANTOM_MAX][AXIS_XYZ];            // Sign for XYZ axis (determined by configuration of arm).
long    PHANTOM_signM[PHANTOM_MAX][PHANTOM_AXIS];          // Sign for motor on [N]th axis.
long    PHANTOM_signE[PHANTOM_MAX][PHANTOM_AXIS];          // Sign for encoder on [N]th axis.

float   PHANTOM_EU2Rad[PHANTOM_MAX][PHANTOM_AXIS];         // Convert Encoder Units to angle (radians) for [N]th axis.
float   PHANTOM_Nm2MU[PHANTOM_MAX][PHANTOM_AXIS];          // Convert torque (Nm) to Motor Units for [N]th axis.
float   PHANTOM_TorqueGain[PHANTOM_MAX][PHANTOM_AXIS];     // Torque gain from force calibration.
float   PHANTOM_TorqueOffset[PHANTOM_MAX][PHANTOM_AXIS];   // Torque offset from force calibration.

long    PHANTOM_MaxMU[PHANTOM_MAX][PHANTOM_AXIS];          // Maximum motor units.

float   PHANTOM_RampTime[PHANTOM_MAX];                     // Time for ramping up motor torque (seconds).

float   PHANTOM_LoopTaskFreq[PHANTOM_MAX];                 // Frequency of PHANTOM LoopTask.

int     PHANTOM_MotionWindow[PHANTOM_MAX];                // Window size for motion algorithm.

STRING  PHANTOM_FT_Name[PHANTOM_MAX];                     // DAQ F/T variables...
int     PHANTOM_FT_ADC[PHANTOM_MAX][DAQFT_GAUGES];
double  PHANTOM_FT_ZRotation[PHANTOM_MAX];

/******************************************************************************/
/* Working variables...                                                       */
/******************************************************************************/

int     PHANTOM_ID = PHANTOM_INVALID;                      // Current working PHANTOM Handle.

float   PHANTOM_sin[PHANTOM_MAX][PHANTOM_AXIS];            // Sine of current [N]th axis encoder angle.
float   PHANTOM_cos[PHANTOM_MAX][PHANTOM_AXIS];            // Cosine of current [N]th axis encoder angle.
float   PHANTOM_Y_COS[PHANTOM_MAX];                        // Y offset of tip from base (without X rotation about base).
float   PHANTOM_Z_SIN[PHANTOM_MAX];                        // Z offset of tip from base (X rotation irrelevant in Z).

long    PHANTOM_EncoderLast[PHANTOM_MAX][PHANTOM_AXIS];    // Last value for encoders. 
long    PHANTOM_EncoderTurn[PHANTOM_MAX][PHANTOM_AXIS];    // Turn counter for encoders.

long    PHANTOM_MotorMU[PHANTOM_MAX][PHANTOM_AXIS];        // Input to motors (in MUs).
float   PHANTOM_MotorNm[PHANTOM_MAX][PHANTOM_AXIS];        // Torque input to motors (Nm).

long    PHANTOM_EncoderEU[PHANTOM_MAX][PHANTOM_AXIS];      // Current encoder values.

int     PHANTOM_AxisIndex[PHANTOM_MAX][PHANTOM_AXIS];      // Axis index.
int     PHANTOM_XYZIndex[PHANTOM_MAX][AXIS_XYZ];           // XYZ index.

matrix  PHANTOM_JT[PHANTOM_MAX];

/******************************************************************************/

STRING  PHANTOM_KeyWord[PHANTOM_MAX][PHANTOM_CNFG];
STRING  PHANTOM_Name[PHANTOM_MAX][PHANTOM_CNFG];

/******************************************************************************/

BOOL    PHANTOM_Test=FALSE;

struct  PHANTOM_ItemHandle   PHANTOM_Item[PHANTOM_MAX];

/******************************************************************************/

void    PHANTOM_LoopTask0( void )   { PHANTOM_PanicLoop(0); PHANTOM_EncoderLoop(0); PHANTOM_ControlLoop(0); PHANTOM_RobotLoop(0); }
void    PHANTOM_LoopTask1( void )   { PHANTOM_PanicLoop(1); PHANTOM_EncoderLoop(1); PHANTOM_ControlLoop(1); PHANTOM_RobotLoop(1); }
void    PHANTOM_LoopTask2( void )   { PHANTOM_PanicLoop(2); PHANTOM_EncoderLoop(2); PHANTOM_ControlLoop(2); PHANTOM_RobotLoop(2); }

LOOPTASK PHANTOM_LoopTask[PHANTOM_MAX] = { PHANTOM_LoopTask0,PHANTOM_LoopTask1,PHANTOM_LoopTask2 };

/******************************************************************************/

#include "phantom-encoder.cpp"         // Function to input values from encoders.
#include "phantom-motor.cpp"           // Function to output torques to motors.
#include "phantom-ramp.cpp"            // Force ramping functions.
#include "phantom-temperature.cpp"     // Temperature tracking for motors over-heating.
#include "phantom-convert.cpp"         // Functions to inverconvert various units.
#include "phantom-FT.cpp"              // Functions for DAQ F/T sensor.

/******************************************************************************/

void    PHANTOM_Init( int ID )
{
int     axis,change;

    // Robot controller...
    PHANTOM_Item[ID].Robot = NULL;

    // LoopTask frequency...
    PHANTOM_LoopTaskFreq[ID] = 0.0;

    // Various flags...
    PHANTOM_Item[ID].started = FALSE;
    PHANTOM_Item[ID].HWpanic = FALSE;
    PHANTOM_Item[ID].SWpanic = FALSE;

    // Zero application-defined force functions (V2.6)...
    PHANTOM_Item[ID].ControlType = PHANTOM_CONTROL_NONE;

    PHANTOM_Item[ID].ControlRaw = NULL;

    PHANTOM_Item[ID].ControlMP = NULL;
    PHANTOM_Item[ID].ControlMPF = NULL;
    PHANTOM_Item[ID].ControlMPVF = NULL;
    PHANTOM_Item[ID].ControlMPVAF = NULL;

    PHANTOM_Item[ID].ControlDP = NULL;
    PHANTOM_Item[ID].ControlDPF = NULL;
    PHANTOM_Item[ID].ControlDPVF = NULL;
    PHANTOM_Item[ID].ControlDPVAF = NULL;

    PHANTOM_Item[ID].MotionWindow = NULL;

    PHANTOM_Y_COS[ID] = 0.0;
    PHANTOM_Z_SIN[ID] = 0.0;

    // RTMX for user-define co-ordinate frame (V4.2)...
    PHANTOM_RTMX[ID] = SPMX_RTMX_NULL;

    for( axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        PHANTOM_AxisIndex[ID][axis] = axis;

        PHANTOM_sin[ID][axis] = 0.0;
        PHANTOM_cos[ID][axis] = 0.0;

        PHANTOM_MotorMU[ID][axis] = 0;
        PHANTOM_MotorNm[ID][axis] = 0.0;

        PHANTOM_EncoderEU[ID][axis] = PHANTOM_EU_INIT;

        PHANTOM_EncoderLast[ID][axis] = 0;
        PHANTOM_EncoderTurn[ID][axis] = 0;

        PHANTOM_TorqueGain[ID][axis] = 1.0;
        PHANTOM_TorqueOffset[ID][axis] = 0.0;
    }

    for( axis=0; (axis < AXIS_XYZ); axis++ )
    {
        PHANTOM_XYZIndex[ID][axis] = axis;
    }

    PHANTOM_Item[ID].FT = NULL;

    memset(PHANTOM_FT_Name[ID],0,STRLEN);
    PHANTOM_FT_ZRotation[ID] = 0.0;
}

/******************************************************************************/

int     PHANTOM_Free( void )
{
int     ID,free;

    if( !PHANTOM_Check() )                  // Check if API started...
    {
        return(PHANTOM_INVALID);
    }

    for( free=PHANTOM_INVALID,ID=0; (ID < PHANTOM_MAX); ID++ )
    {
        if( PHANTOM_Item[ID].Robot == NULL )
        {
            free = ID;
            break;
        }
    }

    return(free);
}

/******************************************************************************/

void    PHANTOM_Use( int ID )
{
    PHANTOM_ID = ID;
}

/******************************************************************************/

void    PHANTOM_CnfgHDR( int ID, int cnfg )
{
    if( PHANTOM_Cnfg[cnfg].HDR )
    {
        CONFIG_set("KeyWord",PHANTOM_KeyWord[ID][cnfg]);
        CONFIG_set("Name",PHANTOM_Name[ID][cnfg]);
    }
}

/******************************************************************************/

void   PHANTOM_CnfgCAL( int ID )
{
    PHANTOM_RTMX[ID] = SPMX_RTMX_NULL;

    CONFIG_reset();

    PHANTOM_CnfgHDR(ID,PHANTOM_CNFG_CAL);

    CONFIG_set("link",PHANTOM_link[ID],PHANTOM_LINK); 
    CONFIG_set("marker",PHANTOM_marker[ID],PHANTOM_MARKER);
    CONFIG_set("baseXYZ",PHANTOM_base[ID],AXIS_XYZ);
    CONFIG_set("angle",PHANTOM_angle[ID],PHANTOM_AXIS);
    CONFIG_set("RTMX",&PHANTOM_RTMX[ID]);
}

/******************************************************************************/

void    PHANTOM_CnfgCFG( int ID )
{
    CONFIG_reset();

    PHANTOM_CnfgHDR(ID,PHANTOM_CNFG_CFG);

    CONFIG_set("Controller",PHANTOM_controller[ID],PHANTOM_CONTROLLER);
    CONFIG_set("Motors",PHANTOM_motors[ID],ROBOT_AXIS);
    CONFIG_set("Encoders",PHANTOM_encoders[ID],ROBOT_AXIS);

    CONFIG_set("signXYZ",PHANTOM_signXYZ[ID],AXIS_XYZ);
    CONFIG_set("signM",PHANTOM_signM[ID],PHANTOM_AXIS);
    CONFIG_set("signE",PHANTOM_signE[ID],PHANTOM_AXIS);
    CONFIG_set("AxisIndex",PHANTOM_AxisIndex[ID],PHANTOM_AXIS);
    CONFIG_set("XYZIndex",PHANTOM_XYZIndex[ID],AXIS_XYZ);
    CONFIG_set("EU2Rad",PHANTOM_EU2Rad[ID],PHANTOM_AXIS);
    CONFIG_set("Nm2MU",PHANTOM_Nm2MU[ID],PHANTOM_AXIS);
    CONFIG_set("TorqueGain",PHANTOM_TorqueGain[ID],PHANTOM_AXIS);
    CONFIG_set("TorqueOffset",PHANTOM_TorqueOffset[ID],PHANTOM_AXIS);
    CONFIG_set("MaxMU",PHANTOM_MaxMU[ID],PHANTOM_AXIS);
    CONFIG_set("RampTime",PHANTOM_RampTime[ID]);
    CONFIG_set("LoopTaskFreq",PHANTOM_LoopTaskFreq[ID]);
    CONFIG_set("MotionWindow",PHANTOM_MotionWindow[ID]);

    CONFIG_set("FT-Name",PHANTOM_FT_Name[ID]);
    CONFIG_set("FT-ADC",PHANTOM_FT_ADC[ID],DAQFT_GAUGES);
    CONFIG_set("FT-ZRotation",PHANTOM_FT_ZRotation[ID]);
}

/******************************************************************************/

void    PHANTOM_CnfgVAR( int ID, int cnfg )
{
    (*PHANTOM_Cnfg[cnfg].load)(ID);
}

/******************************************************************************/

BOOL    PHANTOM_TestCAL( int ID )
{
    return(TRUE);
}

/******************************************************************************/

BOOL    PHANTOM_TestCFG( int ID )
{
    if( PHANTOM_RampTime[ID] == 0.0 )
    {
        PHANTOM_errorf("Ramp Time (RampTime) is zero.\n");
        return(FALSE);
    }

/*    if( PHANTOM_TempSuspend[ID] > PHANTOM_TEMP_MAX )
    {
        PHANTOM_errorf("Suspend Temperature (TempSuspend) is too high (maximum %.1f).\n",PHANTOM_TEMP_MAX);
        return(FALSE);
    }
*/
    return(TRUE);
}

/******************************************************************************/

BOOL    PHANTOM_TestVAR( int ID, int cnfg )
{
    return((*PHANTOM_Cnfg[cnfg].test)(ID));
}

/******************************************************************************/

BOOL    PHANTOM_CnfgSave( int ID, int cnfg )
{
BOOL    ok=FALSE;
char   *file,*path;

    file = PHANTOM_Item[ID].cnfg[cnfg];
    if( (path=FILE_Calibration(file)) == NULL )
    {
        PHANTOM_errorf("PHANTOM_CnfgSave(...) Cannot find file (%s).\n",file);
        return(FALSE);
    }

    PHANTOM_debugf("PHANTOM_CnfgSave(...) %s\n",path);

    if( !PHANTOM_TestVAR(ID,cnfg) )
    {
        PHANTOM_errorf("PHANTOM_CnfgSave(...) CONFIG_Test%s(%s) Failed.\n",PHANTOM_Cnfg[cnfg].EXT,file);
        return(FALSE);
    }

    PHANTOM_debugf("PHANTOM_CnfgSave(...) CONFIG_Test%s(%s) OK.\n",PHANTOM_Cnfg[cnfg].EXT,file);

    PHANTOM_CnfgVAR(ID,cnfg);

    if( CONFIG_write(path) )
    {
        PHANTOM_debugf("PHANTOM_CnfgSave(...) CONFIG_write(%s) OK.\n",path);
        ok = TRUE;
    }
    else
    {
        PHANTOM_errorf("PHANTOM_CnfgSave(...) CONFIG_read(%s) Failed.\n",path);
    }

    return(TRUE);
}

/******************************************************************************/

BOOL    PHANTOM_Hardware( int ID )
{
int     axis,item,step;
BOOL    ok;

#define ROBOT_TYPE_CONTROLLER     0
#define ROBOT_TYPE_ADDRESS        1
#define ROBOT_TYPE_AMPLIFIER      2
#define ROBOT_TYPE_AXIS           3
#define ROBOT_TYPE_MOTORS         4
#define ROBOT_TYPE_ENCODERS       5

    for( ok=TRUE,step=ROBOT_TYPE_CONTROLLER; ((step <= ROBOT_TYPE_ENCODERS) && ok); step++ )
    {
        switch( step )
        {
            case ROBOT_TYPE_CONTROLLER :
               if( (item=STR_TextCode(CONTROLLER_TypeText,PHANTOM_controller[ID][0])) == STR_NOTFOUND )
               {
                   PHANTOM_errorf("PHANTOM[%d] Invalid controller (%s).\n",ID,PHANTOM_controller[ID][0]);
                   ok = FALSE;
                   break;
               }

               PHANTOM_Item[ID].Hardware.Controller = item;
               break;

            case ROBOT_TYPE_AMPLIFIER :
               if( (item=STR_TextCode(CONTROLLER_AmplifierText,PHANTOM_controller[ID][2])) == STR_NOTFOUND )
               {
                   PHANTOM_errorf("PHANTOM[%d] Invalid amplifier (%s).\n",ID,PHANTOM_controller[ID][2]);
                   ok = FALSE;
                   break;
               }

               PHANTOM_Item[ID].Hardware.Amplifier = item;
               break;

            case ROBOT_TYPE_ADDRESS :
               PHANTOM_Item[ID].Hardware.Address = CONFIG_buff2long(PHANTOM_controller[ID][1]); 
               break;

            case ROBOT_TYPE_AXIS :
               PHANTOM_Item[ID].Hardware.Axis = PHANTOM_AXIS; 
               break;

            case ROBOT_TYPE_MOTORS :
               for( axis=0; ((axis < PHANTOM_Item[ID].Hardware.Axis) && ok); axis++ )
               {
                   if( (axis == 0) || !STR_null(PHANTOM_motors[ID][axis]) )
                   {
                       if( (item=STR_TextCode(MOTOR_TypeText,PHANTOM_motors[ID][axis])) == STR_NOTFOUND )
                       {
                           PHANTOM_errorf("PHANTOM[%d] Invalid motor (%s).\n",ID,PHANTOM_motors[ID][axis]);
                           ok = FALSE;
                           continue;
                       }
                   }

                   PHANTOM_Item[ID].Hardware.Motor[axis] = item;
               }
               break;

            case ROBOT_TYPE_ENCODERS :
               for( axis=0; ((axis < PHANTOM_Item[ID].Hardware.Axis) && ok); axis++ )
               {
                   if( (axis == 0) || !STR_null(PHANTOM_encoders[ID][axis]) )
                   {
                       if( (item=STR_TextCode(ENCODER_TypeText,PHANTOM_encoders[ID][axis])) == STR_NOTFOUND )
                       {
                           PHANTOM_errorf("PHANTOM[%d] Invalid encoder (%s).\n",ID,PHANTOM_encoders[ID][axis]);
                           ok = FALSE;
                           continue;
                       }
                   }

                   PHANTOM_Item[ID].Hardware.Encoder[axis] = item;
               }
               break;
        }
    }

    if( !ok )
    {
        return(FALSE);
    }

    PHANTOM_debugf("PHANTOM[%d] Hardware...\n",ID);
    ROBOT_HardwareDetails(&PHANTOM_Item[ID].Hardware,PHANTOM_debugf);

    return(TRUE);
}

/******************************************************************************/

int PHANTOM_DOF( int ID )
{
int axis,dof;

    for( dof=0,axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        if( PHANTOM_AxisIndex[ID][axis] != -1 )
        {
            dof++;
        }
    }

    return(dof);
}

/******************************************************************************/

BOOL PHANTOM_3D( int ID )
{
BOOL flag;

    flag = (PHANTOM_DOF(ID) == 3);

    return(flag);
}

/******************************************************************************/

BOOL PHANTOM_2D( int ID )
{
BOOL flag;

    flag = (PHANTOM_DOF(ID) == 2);

    return(flag);
}

/******************************************************************************/

float PHANTOM_XYZ( int ID, int axis, float xyz[] )
{
float value=0.0;

    if( PHANTOM_XYZIndex[ID][axis] != -1 )
    {
        value = xyz[PHANTOM_XYZIndex[ID][axis]];
    }

    return(value);
}

/******************************************************************************/

void PHANTOM_XYZ( int ID, int axis, float value, float xyz[] )
{
    if( PHANTOM_XYZIndex[ID][axis] != -1 )
    {
        xyz[PHANTOM_XYZIndex[ID][axis]] = value;
    }
}

/******************************************************************************/

void PHANTOM_XYZ( int ID, float xyz[], float &x, float &y, float &z )
{
    x = PHANTOM_XYZ(ID,AXIS_X,xyz);
    y = PHANTOM_XYZ(ID,AXIS_Y,xyz);
    z = PHANTOM_XYZ(ID,AXIS_Z,xyz);
}

/******************************************************************************/

void PHANTOM_XYZ( int ID, float x, float y, float z, float xyz[] )
{
int axis;

    for( axis=0; (axis < AXIS_XYZ); axis++ )
    {
        xyz[axis] = 0;
    }

    PHANTOM_XYZ(ID,AXIS_X,x,xyz);
    PHANTOM_XYZ(ID,AXIS_Y,y,xyz);
    PHANTOM_XYZ(ID,AXIS_Z,z,xyz);
}

/******************************************************************************/

BOOL    PHANTOM_CnfgLoad( int ID, int cnfg )
{
BOOL    ok=FALSE;
char   *file,*path;

    PHANTOM_CnfgVAR(ID,cnfg);

    file = PHANTOM_Item[ID].cnfg[cnfg];

    if( (path=FILE_Calibration(file)) == NULL )
    {
        PHANTOM_errorf("PHANTOM_CnfgLoad(...) Cannot find file (%s).\n",file);
        return(FALSE);
    }

    PHANTOM_debugf("PHANTOM_CnfgLoad(...) %s\n",path);

    if( !CONFIG_read(path) )
    {
        PHANTOM_errorf("PHANTOM_CnfgLoad(...) CONFIG_read(%s) Failed.\n",path);
        return(ok);
    }

    PHANTOM_debugf("PHANTOM_CnfgLoad(...) CONFIG_read(%s) OK.\n",path);

    if( !CONFIG_flag(CONFIG_FLAG_READ) )    // Make sure all parameters have been read...
    {
        PHANTOM_errorf("PHANTOM_CnfgLoad(...) CONFIG_read(%s) Parameters missing.\n",path);
        CONFIG_list(CONFIG_FLAG_READ,FALSE,PHANTOM_errorf);
    }
//    else
    if( PHANTOM_TestVAR(ID,cnfg) )
    {
        PHANTOM_debugf("PHANTOM_CnfgLoad(...) CONFIG_Test%s(%s) OK.\n",PHANTOM_Cnfg[cnfg].EXT,path);
        ok = TRUE;
    }
    else
    {
        PHANTOM_errorf("PHANTOM_CnfgLoad(...) CONFIG_Test%s(%s) Failed.\n",PHANTOM_Cnfg[cnfg].EXT,path);
    }

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_CnfgLoad( int ID, char *name )
{
int     cnfg;
BOOL    ok;

    for( ok=TRUE,cnfg=0; ((PHANTOM_Cnfg[cnfg].EXT != NULL) && ok); cnfg++ )
    {
        strncpy(PHANTOM_Item[ID].cnfg[cnfg],STR_stringf("%s%s.%s",
                       PHANTOM_CnfgPath,
                       name,
                       PHANTOM_Cnfg[cnfg].EXT),STRLEN);

        ok = PHANTOM_CnfgLoad(ID,cnfg);
    }

    return(ok);
}

/******************************************************************************/

void    PHANTOM_CnfgPrnt( int ID, PRINTF prnf, int cnfg )
{
    PHANTOM_CnfgLoad(ID,cnfg);
    CONFIG_list(prnf);
}

/******************************************************************************/

void    PHANTOM_CnfgPrnt( int ID, PRINTF prnf )
{
int     cnfg;

    for( cnfg=0; (PHANTOM_Cnfg[cnfg].EXT != NULL); cnfg++ )
    {
        PHANTOM_CnfgPrnt(ID,prnf,cnfg);
    }
}

/******************************************************************************/

BOOL    PHANTOM_Started( int ID )
{
BOOL    started=FALSE;

    if( PHANTOM_Check(ID) )
    {
        started = PHANTOM_Item[ID].started;
    }

    return(started);
}

/******************************************************************************/

ROBOT *PHANTOM_Robot( int ID )
{
ROBOT *robot=NULL;

    if( PHANTOM_Check(ID) )
    {
        robot = PHANTOM_Item[ID].Robot;
    }

    return(robot);
}

/******************************************************************************/

BOOL    PHANTOM_Safe( int ID )
{
BOOL    safe=FALSE;

    if( PHANTOM_Check(ID) )
    {
        safe = PHANTOM_Item[ID].Robot->Controller->Safe();
    }

    return(safe);
}

/******************************************************************************/

BOOL    PHANTOM_Enabled( int ID )
{
BOOL    enabled=FALSE;

    if( PHANTOM_Check(ID) )
    {
        enabled = PHANTOM_Item[ID].Robot->Controller->Running();
    }

    return(enabled);
}

/******************************************************************************/

BOOL    PHANTOM_EnabledYes2No( int ID )
{
BOOL    flag=FALSE;

    if( PHANTOM_Check(ID) )
    {
        flag = PHANTOM_Item[ID].Robot->Controller->RunningYes2No();
    }

    return(flag);
}

/******************************************************************************/

BOOL    PHANTOM_EnabledNo2Yes( int ID )
{
BOOL    flag=FALSE;

    if( PHANTOM_Check(ID) )
    {
        flag = PHANTOM_Item[ID].Robot->Controller->RunningNo2Yes();
    }

    return(flag);
}

/******************************************************************************/

BOOL    PHANTOM_Panic( int ID )
{
BOOL    panic=TRUE;

    if( PHANTOM_Check(ID) )
    {
        PHANTOM_Item[ID].HWpanic = !PHANTOM_Enabled(ID);
        panic = PHANTOM_Item[ID].HWpanic || PHANTOM_Item[ID].SWpanic;
        PHANTOM_Item[ID].SWpanic = FALSE;
    }

    return(panic);
}

/******************************************************************************/

void    PHANTOM_PanicNow( int ID )
{
    if( PHANTOM_Check(ID) )
    {
        PHANTOM_Item[ID].SWpanic = TRUE;
        PHANTOM_debugf("PHANTOM_PanicNow(ID=%d)\n",ID);
    }
}

/******************************************************************************/

BOOL    PHANTOM_PanicOff2On( int ID )
{
BOOL    flag=FALSE;

    if( PHANTOM_Check(ID) )
    {
        flag = PHANTOM_Item[ID].Robot->Controller->RunningYes2No();
    }

    return(flag);
}

/******************************************************************************/

BOOL    PHANTOM_PanicOn2Off( int ID )
{
BOOL    flag=FALSE;

    if( PHANTOM_Check(ID) )
    {
        flag = PHANTOM_Item[ID].Robot->Controller->RunningNo2Yes();
    }

    return(flag);
}

/******************************************************************************/

void    PHANTOM_RobotLoop( int ID )
{
    if( PHANTOM_Check(ID) )
    {
        PHANTOM_Item[ID].Robot->LoopTask();
    }
}

/******************************************************************************/

BOOL    PHANTOM_Cooling( int ID )
{
BOOL    cooling=FALSE;

    if( PHANTOM_Check(ID) )
    {
        cooling = PHANTOM_Item[ID].Robot->TempTrakCooling();
    }

    return(cooling);
}

/******************************************************************************/

BOOL    PHANTOM_Ping( int ID )
{
BOOL    ok=TRUE;

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Safety( int ID )
{
BOOL ok=TRUE;

    // Zero ramping...
    PHANTOM_Item[ID].Robot->RampReset();

    // Make sure motor torques are zero (reset)...
    PHANTOM_MotorReset(ID);

    // Read safety line from controller...
    if( !PHANTOM_Safe(ID) )
    {
        ok = FALSE;
    }

    STR_printf(ok,PHANTOM_debugf,PHANTOM_errorf,"PHANTOM_Safety(%s) %s.\n",PHANTOM_Item[ID].name,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

int     PHANTOM_Open( char *name )
{
int     ID;
BOOL    ok;

    // Make sure API is running...
    if( !PHANTOM_Check() )
    {
        return(PHANTOM_INVALID);
    }

    PHANTOM_debugf("PHANTOM_Open(%s)\n",name);

    // Find a free handle ID...
    if( (ID=PHANTOM_Free()) == PHANTOM_INVALID )
    {
        PHANTOM_errorf("PHANTOM_Open(%s) No free handles.\n",name);
        return(PHANTOM_INVALID);
    }

    PHANTOM_debugf("ID=%d\n",ID);

    // Clear the handle item...
    PHANTOM_Init(ID);

    // Load configuration variables from each file type...
    if( !PHANTOM_CnfgLoad(ID,name) )
    {
        return(PHANTOM_INVALID);
    }

    PHANTOM_invROMX[ID] = SPMX_rtmx2romx(inv(PHANTOM_RTMX[ID]));

    // Set unique name for Phantom...
    strncpy(PHANTOM_Item[ID].name,PHANTOM_KeyWord[ID][PHANTOM_CNFG_CFG],STRLEN);

    // Set hardware details for controller (V4.0)...
    if( !PHANTOM_Hardware(ID) )
    {
        return(PHANTOM_INVALID);
    }

    // Check if interface operating...
    if( !PHANTOM_Ping(ID) )
    {
        PHANTOM_errorf("PHANTOM_Open(%s) Interface not operating.\n",name);
        return(PHANTOM_INVALID);
    }

    // Create robot controller and open...
    PHANTOM_Item[ID].Robot = new ROBOT(&PHANTOM_Item[ID].Hardware,PHANTOM_Item[ID].name);

    PHANTOM_Item[ID].Robot->RampInit(PHANTOM_RampTime[ID],PHANTOM_RampFunc[ID]);

    if( !PHANTOM_Item[ID].Robot->Open() )
    {
        PHANTOM_errorf("PHANTOM_Open(%s) Cannot open controller.\n",name);
        return(PHANTOM_INVALID);
    }

    PHANTOM_debugf("PHANTOM %s.\n",PHANTOM_Item[ID].name);

    PHANTOM_debugf("%s.\n",PHANTOM_Name[ID][PHANTOM_CNFG_CAL]);

    // Create window object for quadratic motion algorithm...
    PHANTOM_Item[ID].MotionWindow = new WINDOW(PHANTOM_Item[ID].name,PHANTOM_MotionWindow[ID],3);

    // Do safety check here... (V3.0)
    if( !PHANTOM_Safety(ID) )
    {
        PHANTOM_errorf("PHANTOM_Open(%s) Failed safety check.\n",name);
        return(PHANTOM_INVALID);
    }

    // Set default handle ID to this one...
    PHANTOM_Use(ID);

    return(ID);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, void (*func)( long EU[], long MU[] ) )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_RAW,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, void (*func)( matrix &p ) )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_MP,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, void (*func)( matrix &p, matrix &f ) )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_MPF,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, void (*func)( matrix &p, matrix &v, matrix &f ) )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_MPVF,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_MPVAF,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, void (*func)( double p[] ) )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_DP,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, void (*func)( double p[], double f[] ) )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_DPF,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, void (*func)( double p[], double v[], double f[] ) )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_DPVF,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, void (*func)( double p[], double v[], double a[], double f[] ) )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_DPVAF,func);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID )
{
BOOL    ok;

    ok = PHANTOM_Start(ID,PHANTOM_CONTROL_NONE,NULL);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Enable( int ID )
{
BOOL    ok=FALSE;

    // Turn amplifiers on...
    if( PHANTOM_Check(ID) )
    {
        ok = PHANTOM_Item[ID].Robot->Start();
    }

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Disable( int ID )
{
BOOL    ok=TRUE;

    // Turn amplifiers off...
    if( PHANTOM_Check(ID) )
    {
        PHANTOM_Item[ID].Robot->Stop();
    }

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Start( int ID, int type, void *func )
{
    if( !PHANTOM_Check(ID) )                // Check if API started and handle ID okay...
    {
        return(FALSE);
    }

    if( PHANTOM_Started(ID) )               // Already started...
    {
        return(TRUE);
    }

    if( !PHANTOM_Enable(ID) )               // Turn motor controller on...
    {
        PHANTOM_errorf("PHANTOM_Start(ID=%d) Cannot enable.\n");
        return(FALSE);
    }

    // Set force calculation function for motor loop...
    PHANTOM_ControlSet(ID,type,func);      
  
    if( !PHANTOM_LoopTaskStart(ID) )        // Start LoopTask functions...
    {
        PHANTOM_LoopTaskStop(ID);           // Failed...

        PHANTOM_errorf("PHANTOM_Start(ID=%d) Cannot start LoopTask.\n");
        return(FALSE);
    }

    if( !PHANTOM_Item[ID].Robot->RampStart() )
    {
        PHANTOM_errorf("PHANTOM_Start(ID=%d) Cannot start RAMP object.\n");
        return(FALSE);
    }

    if( !PHANTOM_Item[ID].Robot->TempTrakStart() )
    {
        PHANTOM_errorf("PHANTOM_Start(ID=%d) Cannot start TEMPTRAK object.\n");
        return(FALSE);
    }

    PHANTOM_Item[ID].started = TRUE;

    return(TRUE);
}

/******************************************************************************/

int     PHANTOM_Start( char *name, int type, void *func )
{
int     ID;

    if( (ID=PHANTOM_Open(name)) == PHANTOM_INVALID )
    {
        return(PHANTOM_INVALID);
    }

    if( !PHANTOM_Start(ID,type,func) )
    {
        return(PHANTOM_INVALID);
    }

    return(ID);
}

/******************************************************************************/

int     PHANTOM_Start( char *name, void (*func)( long EU[], long MU[] ) )
{
BOOL    ok;

    ok = PHANTOM_Start(name,PHANTOM_CONTROL_RAW,func);

    return(ok);
}

/******************************************************************************/

int     PHANTOM_Start( char *name, void (*func)( matrix &p ) )
{
BOOL    ok;

    ok = PHANTOM_Start(name,PHANTOM_CONTROL_MP,func);

    return(ok);
}

/******************************************************************************/

int     PHANTOM_Start( char *name, void (*func)( matrix &p, matrix &f ) )
{
BOOL    ok;

    ok = PHANTOM_Start(name,PHANTOM_CONTROL_MPF,func);

    return(ok);
}

/******************************************************************************/

int     PHANTOM_Start( char *name, void (*func)( matrix &p, matrix &v, matrix &f ) )
{
BOOL    ok;

    ok = PHANTOM_Start(name,PHANTOM_CONTROL_MPVF,func);

    return(ok);
}

/******************************************************************************/

int     PHANTOM_Start( char *name, void (*func)( matrix &p, matrix &v, matrix &a, matrix &f ) )
{
BOOL    ok;

    ok = PHANTOM_Start(name,PHANTOM_CONTROL_MPVAF,func);

    return(ok);
}

/******************************************************************************/

int     PHANTOM_Start( char *name )
{
BOOL    ok;

    ok = PHANTOM_Start(name,PHANTOM_CONTROL_NONE,NULL);

    return(ok);
}

/******************************************************************************/

void    PHANTOM_Stop( int ID )
{
    // Check if API started and handle ID okay...
    if( !PHANTOM_Check(ID) )
    {
        return;
    }

    // Stop looptask functions...
    PHANTOM_LoopTaskStop(ID);

    // Zero motor torques just to be safe (V2.6)...
    PHANTOM_MotorReset(ID);

    PHANTOM_Item[ID].Robot->RampStop();
    PHANTOM_Item[ID].Robot->TempTrakStop();

    // Turn motor controller off...
    PHANTOM_Disable(ID);             

    // Started flag...       
    PHANTOM_Item[ID].started = FALSE;
}

/******************************************************************************/

void    PHANTOM_Close( int ID )
{
    // Make sure handle is valid...
    if( !PHANTOM_Check(ID) )
    {
        return;
    }

    // Stop PHANTOM first...
    PHANTOM_Stop(ID);

    // Close and delete robot controller...
    PHANTOM_Item[ID].Robot->Close();
    delete PHANTOM_Item[ID].Robot;
    PHANTOM_Item[ID].Robot = NULL;

    // Clear handle...
    PHANTOM_Init(ID);
}

/******************************************************************************/

void    PHANTOM_CloseAll( void )
{
int     ID;

    for( ID=0; (ID < PHANTOM_MAX); ID++ )
    {
        PHANTOM_Close(ID);
    }
}

/******************************************************************************/

void    PHANTOM_Exit( void )
{
    PHANTOM_errorf("PHANTOM API exit.\n");

    PHANTOM_CloseAll();
    exit(0);
}

/******************************************************************************/

BOOL    PHANTOM_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok;
int     ID;

    if( PHANTOM_API_started )               // Start the API once...
    {
        return(TRUE);
    }

    PHANTOM_PRN_messgf = messgf;            // General API message print function.
    PHANTOM_PRN_errorf = errorf;            // API error message print function.
    PHANTOM_PRN_debugf = debugf;            // Debug information print function.

    for( ID=0; (ID < PHANTOM_MAX); ID++ )
    {
        PHANTOM_Init(ID);
    }

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(PHANTOM_API_stop);       // Install stop function.
        PHANTOM_API_started = TRUE;         // Set started flag.

        MODULE_start(PHANTOM_PRN_messgf);   // Register module.
    }
    else
    {
        PHANTOM_errorf("PHANTOM_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    PHANTOM_API_stop( void )
{
int     ID;

    if( !PHANTOM_API_started )         // API not started in the first place...
    {
         return;
    }

    PHANTOM_CloseAll();

    PHANTOM_API_started = FALSE;       // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    PHANTOM_Check( void )
{
BOOL    ok=TRUE;

    if( !PHANTOM_API_started )         // API not started...
    {                                  // Start module automatically...
        ok = PHANTOM_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        PHANTOM_debugf("PHANTOM_Check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOM_Check( int ID )
{
BOOL    ok=TRUE;

    if( !PHANTOM_Check() )
    {
        ok = FALSE;
    }
    else
    if( ID == PHANTOM_INVALID )
    {
        ok = FALSE;
    }
    else
    if( PHANTOM_Item[ID].Robot == NULL )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

void    PHANTOM_Info( int ID, PRINTF prnf )
{
    if( PHANTOM_Check(ID) )                 // Check if API started and handle ID okay...
    {
        PHANTOM_CnfgPrnt(ID,prnf);
    }
}

/******************************************************************************/

void    PHANTOM_PanicLoop( int ID )
{
    PHANTOM_Item[ID].HWpanic = !PHANTOM_Item[ID].Robot->Controller->Running();
}

/******************************************************************************/

void    PHANTOM_Force( int ID, matrix &f )
{
float fx,fy,fz;

    fx = f(1,1);
    fy = f(2,1);
    fz = f(3,1);

    PHANTOM_Force(ID,fx,fy,fz);
}

/******************************************************************************/

void    PHANTOM_Force( int ID, float fx, float fy, float fz )
{
BOOL    ok;
int     motor,axis;
float   force[3];
matrix  T(3,1),F(3,1);

#ifdef  PHANTOM_OLD_FORCES        // V2.4
float   cm2m=1.0;
#else
float   cm2m=100.0;
#endif

    // Forces (fx,fy,fz) are in Newtons. Need to convert them
    // to torques for the various motors...

    // First, take off RTMX for user-defined co-ordinate frame...
    SPMX_doR(PHANTOM_invROMX[ID],fx,fy,fz);

    // Now apply XYZ axis-mapping...
    PHANTOM_XYZ(ID,fx,fy,fz,force);

    // Adjust for sign of XYZ axis...
    force[AXIS_X] *= PHANTOM_signXYZ[ID][AXIS_X];
    force[AXIS_Y] *= PHANTOM_signXYZ[ID][AXIS_Y];
    force[AXIS_Z] *= PHANTOM_signXYZ[ID][AXIS_Z];

    F(1,1) = force[AXIS_X];
    F(2,1) = force[AXIS_Y];
    F(3,1) = force[AXIS_Z];

    // Exit immediately if forces exceed hard-coded maximum...
    if( norm(F) > PHANTOM_FORCE_MAX )
    {
        PHANTOM_errorf("Robot=%s Maximum force (%.1f N) exceeded.\n",PHANTOM_KeyWord[ID][PHANTOM_CNFG_CFG],PHANTOM_FORCE_MAX);
        PHANTOM_Exit();
    }
  
    // Create (3x3) Jacobian transpose...
    matrix_dim(PHANTOM_JT[ID],3,3);

    PHANTOM_JT[ID](1,1) =  PHANTOM_cos[ID][PHANTOM_AXIS_1] * PHANTOM_Y_COS[ID];
    PHANTOM_JT[ID](1,2) = -PHANTOM_sin[ID][PHANTOM_AXIS_1] * PHANTOM_Y_COS[ID];
    PHANTOM_JT[ID](1,3) =  0;
    PHANTOM_JT[ID](2,1) =  PHANTOM_link[ID][PHANTOM_LINK_1] * -PHANTOM_sin[ID][PHANTOM_AXIS_2] * PHANTOM_sin[ID][PHANTOM_AXIS_1];
    PHANTOM_JT[ID](2,2) =  PHANTOM_link[ID][PHANTOM_LINK_1] * -PHANTOM_sin[ID][PHANTOM_AXIS_2] * PHANTOM_cos[ID][PHANTOM_AXIS_1];
    PHANTOM_JT[ID](2,3) =  PHANTOM_link[ID][PHANTOM_LINK_1] *  PHANTOM_cos[ID][PHANTOM_AXIS_2];
    PHANTOM_JT[ID](3,1) =  PHANTOM_link[ID][PHANTOM_LINK_2] * -PHANTOM_sin[ID][PHANTOM_AXIS_3] * PHANTOM_sin[ID][PHANTOM_AXIS_1];
    PHANTOM_JT[ID](3,2) =  PHANTOM_link[ID][PHANTOM_LINK_2] * -PHANTOM_sin[ID][PHANTOM_AXIS_3] * PHANTOM_cos[ID][PHANTOM_AXIS_1];
    PHANTOM_JT[ID](3,3) =  PHANTOM_link[ID][PHANTOM_LINK_2] * -PHANTOM_cos[ID][PHANTOM_AXIS_3];

    // Convert link lengths to metres...
    PHANTOM_JT[ID] /= cm2m;

    // Torques (Nm) are jacobian transpose * forces...
    T = PHANTOM_JT[ID] * F;

    // Apply torque calibration gains and offsets...
    PHANTOM_MotorNm[ID][PHANTOM_AXIS_1] = (PHANTOM_TorqueGain[ID][PHANTOM_AXIS_1] * T(1,1)) + PHANTOM_TorqueOffset[ID][PHANTOM_AXIS_1];
    PHANTOM_MotorNm[ID][PHANTOM_AXIS_2] = (PHANTOM_TorqueGain[ID][PHANTOM_AXIS_2] * T(2,1)) + PHANTOM_TorqueOffset[ID][PHANTOM_AXIS_2];
    PHANTOM_MotorNm[ID][PHANTOM_AXIS_3] = (PHANTOM_TorqueGain[ID][PHANTOM_AXIS_3] * T(3,1)) + PHANTOM_TorqueOffset[ID][PHANTOM_AXIS_3];

    // Now put the torque values out to the motors...
    PHANTOM_TorqueNm(ID,PHANTOM_MotorNm[ID]);
}

/******************************************************************************/

BOOL    PHANTOM_LoopTaskStart( int ID )
{
BOOL    ok=FALSE;

    if( PHANTOM_LoopTaskFreq[ID] == 0.0 )
    {
        ok = LOOPTASK_start(PHANTOM_LoopTask[ID]);
    }
    else
    {
        ok = LOOPTASK_start(PHANTOM_LoopTask[ID],PHANTOM_LoopTaskFreq[ID]);
    }

    if( ok )
    {
        PHANTOM_LoopTaskFreq[ID] = LOOPTASK_frequency(PHANTOM_LoopTask[ID]);
        PHANTOM_Item[ID].Robot->LoopTaskFrequency(PHANTOM_LoopTaskFreq[ID]);
    }

    STR_printf(ok,PHANTOM_debugf,PHANTOM_errorf,"PHANTOM_LoopTaskStart(ID=%d) %s (%.1lf Hz).\n",ID,STR_OkFailed(ok),PHANTOM_LoopTaskFreq[ID]);

    return(ok);
}

/******************************************************************************/

void    PHANTOM_LoopTaskStop( int ID )
{
    LOOPTASK_stop(PHANTOM_LoopTask[ID]);
}

/******************************************************************************/

void    PHANTOM_AngleOffset( int ID, float a1, float a2, float a3 )
{
    PHANTOM_angle[ID][0] = a1;
    PHANTOM_angle[ID][1] = a2;
    PHANTOM_angle[ID][2] = a3;
}

/******************************************************************************/

void    PHANTOM_AnglesRaw( int ID, float &a1, float &a2, float &a3 )
{
    PHANTOM_Angles(ID,a1,a2,a3,FALSE);      // Get raw angles (FALSE = No angular offset adjustment).
}

/******************************************************************************/

void    PHANTOM_AnglesRaw( int ID, double &a1, double &a2, double &a3 )
{
    PHANTOM_Angles(ID,a1,a2,a3,FALSE);      // Get raw angles (FALSE = No angular offset adjustment).
}

/******************************************************************************/

void    PHANTOM_AnglesRaw( int ID, matrix &mtx )
{
    PHANTOM_Angles(ID,mtx,FALSE);           // Get raw angles (FALSE = No angular offset adjustment).
}

/******************************************************************************/

void    PHANTOM_Angles( int ID, float &a1, float &a2, float &a3 )
{
    PHANTOM_Angles(ID,a1,a2,a3,TRUE);       // Get angles (TRUE = Angular offset adjustment).
}

/******************************************************************************/

void    PHANTOM_Angles( int ID, double &a1, double &a2, double &a3 )
{
    PHANTOM_Angles(ID,a1,a2,a3,TRUE);       // Get angles (TRUE = Angular offset adjustment).
}

/******************************************************************************/

void    PHANTOM_Angles( int ID, matrix &mtx )
{
    PHANTOM_Angles(ID,mtx,TRUE);
}

/******************************************************************************/

void    PHANTOM_Angles( int ID, float &a1, float &a2, float &a3, BOOL offset )
{
static  float  angle[PHANTOM_AXIS];

    PHANTOM_Angles(ID,angle,offset);

    a1 = angle[PHANTOM_AXIS_1];
    a2 = angle[PHANTOM_AXIS_2];
    a3 = angle[PHANTOM_AXIS_3];
}

/******************************************************************************/

void    PHANTOM_Angles( int ID, double &a1, double &a2, double &a3, BOOL offset )
{
static  float  f1,f2,f3;

    PHANTOM_Angles(ID,f1,f2,f3,offset);

    a1 = (double)f1;
    a2 = (double)f2;
    a3 = (double)f3;
}

/******************************************************************************/

void    PHANTOM_Angles( int ID, matrix &mtx, BOOL offset )
{
static  float  f1,f2,f3;

    PHANTOM_Angles(ID,f1,f2,f3,offset);

    mtx(PHANTOM_MTX_A1) = (double)f1;
    mtx(PHANTOM_MTX_A2) = (double)f2;
    mtx(PHANTOM_MTX_A3) = (double)f3;
}

/******************************************************************************/

void    PHANTOM_Angles( int ID, float angle[], BOOL offset )
{
int     axis;
long    EU;

//  Looking at the robot...
//  Axis 1 (encoder 0) is the base -ve is left and +ve is right.
//  Axis 2 (encoder 1) is the lower on big wheel +ve is away and -ve is near.
//  Axis 3 (encoder 2) is the upper on big wheel +ve is down and -ve is up.
  
//  angle[0] +ve to left -ve to right.
//  angle[1] -ve up and +ve down.
//  angle[2] link 2 to horizontal -90 at right angle and -ve away and +ve towards.

    for( axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        // First, check if this axis is used...
        if( PHANTOM_AxisIndex[ID][axis] == -1 )
        {
            angle[axis] = 0.0;
            continue;
        }

        // Read encoder...
        EU = PHANTOM_Encoder(ID,PHANTOM_AxisIndex[ID][axis]);

        // Conver encoder units (EU) to Get angle from encoder...
        angle[axis] = PHANTOM_ConvEU2Rad(ID,axis,EU);

        // Adjust 3rd axis by -90 degrees (because it's like a bent elbow at zero)...
        if( axis == PHANTOM_AXIS_3 )
        {
            angle[axis] = (r90) - angle[axis];
        }

        // Adjustment for encoder angle offsets...
        if( offset )
        {
            angle[axis] += PHANTOM_angle[ID][axis];
        }
    }
}

/******************************************************************************/

void    PHANTOM_PosnCalc( int ID, float angle[], float link[], float &x, float &y, float &z, BOOL adjust )
{
int     axis;
float   XYZ[AXIS_XYZ];

    for( axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        PHANTOM_cos[ID][axis] = cos(angle[axis]);
        PHANTOM_sin[ID][axis] = sin(angle[axis]);
    }

    PHANTOM_Y_COS[ID] = (link[PHANTOM_LINK_1] * PHANTOM_cos[ID][PHANTOM_AXIS_2]) +
                        (link[PHANTOM_LINK_2] * PHANTOM_cos[ID][PHANTOM_AXIS_3]);

    PHANTOM_Z_SIN[ID] = (link[PHANTOM_LINK_1] * PHANTOM_sin[ID][PHANTOM_AXIS_2]) -
                        (link[PHANTOM_LINK_2] * PHANTOM_sin[ID][PHANTOM_AXIS_3]);

    // Raw x,y,z position of tip of arm...
    XYZ[AXIS_X] = PHANTOM_Y_COS[ID] * PHANTOM_sin[ID][PHANTOM_AXIS_1];
    XYZ[AXIS_Y] = PHANTOM_Y_COS[ID] * PHANTOM_cos[ID][PHANTOM_AXIS_1];
    XYZ[AXIS_Z] = PHANTOM_Z_SIN[ID];

    if( adjust )                  // Raw position, so don't do adjustments...
    {
        for( axis=0; (axis < PHANTOM_AXIS); axis++ )
        {
            // Adjust for sign of each axis...
            XYZ[PHANTOM_XYZIndex[ID][axis]] *= PHANTOM_signXYZ[ID][axis];

            // Adjust for position of base in co-ordinate frame...
            XYZ[PHANTOM_XYZIndex[ID][axis]] += PHANTOM_base[ID][axis];
        }
    }

    // Re-map XYZ depending on configuration file...
    PHANTOM_XYZ(ID,XYZ,x,y,z);

    // Apply RTMX for user-defined co-ordinate frame...
    SPMX_doRT(PHANTOM_RTMX[ID],x,y,z);
}

/******************************************************************************/

void    PHANTOM_PosnRaw( int ID, float &x, float &y, float &z )
{
    PHANTOM_Posn(ID,x,y,z,FALSE);           // Get raw position (FALSE = Raw values).
}

/******************************************************************************/

void    PHANTOM_PosnRaw( int ID, double &x, double &y, double &z )
{
    PHANTOM_Posn(ID,x,y,z,FALSE);           // Get raw position (FALSE = Raw values).
}

/******************************************************************************/

void    PHANTOM_PosnRaw( int ID, matrix &mtx )
{
    PHANTOM_Posn(ID,mtx,FALSE);             // Get raw position (FALSE = Raw values).
}

/******************************************************************************/

void    PHANTOM_Posn( int ID, float &x, float &y, float &z )
{
    PHANTOM_Posn(ID,x,y,z,TRUE);            // Get position (TRUE = Adjusted values).
}

/******************************************************************************/

void    PHANTOM_Posn( int ID, double &x, double &y, double &z )
{
    PHANTOM_Posn(ID,x,y,z,TRUE);            // Get position (TRUE = Adjusted values).
}

/******************************************************************************/

void    PHANTOM_Posn( int ID, matrix &mtx )                   
{
    PHANTOM_Posn(ID,mtx,TRUE);              // Get position (TRUE = Adjusted values).
}

/******************************************************************************/

void    PHANTOM_Posn( int ID, float &x, float &y, float &z, BOOL adjust )
{
static  float  angle[PHANTOM_AXIS];
int     axis;
  
    PHANTOM_Angles(ID,angle,TRUE);
    PHANTOM_PosnCalc(ID,angle,PHANTOM_link[ID],x,y,z,adjust);
}

/******************************************************************************/

void    PHANTOM_Posn( int ID, double &x, double &y, double &z, BOOL adjust )
{ 
static  float  fx,fy,fz;

    PHANTOM_Posn(ID,fx,fy,fz,adjust);

    x = (double)fx;
    y = (double)fy;
    z = (double)fz;
}

/******************************************************************************/

void    PHANTOM_Posn( int ID, matrix &posn, BOOL adjust )
{
    SPMX_DimPOMX(posn);
    PHANTOM_Posn(ID,posn(SPMX_POMX_XC),posn(SPMX_POMX_YC),posn(SPMX_POMX_ZC),adjust);
}

/******************************************************************************/
/* Calculate position of OptoTrak marker at tip of arm.                       */
/******************************************************************************/

void    PHANTOM_Marker( int ID, float angle[], float &x, float &y, float &z, int marker )
{
static  float  link[PHANTOM_LINK];

    link[PHANTOM_LINK_1] = PHANTOM_link[ID][PHANTOM_LINK_1];
    link[PHANTOM_LINK_2] = PHANTOM_marker[ID][marker];

    PHANTOM_PosnCalc(ID,angle,link,x,y,z,TRUE);
}

/******************************************************************************/

void    PHANTOM_Marker( int ID, float a1, float a2, float a3, float &x, float &y, float &z, int marker )
{
static  float  angle[PHANTOM_AXIS];

    angle[PHANTOM_AXIS_1] = a1;
    angle[PHANTOM_AXIS_2] = a2;
    angle[PHANTOM_AXIS_3] = a3;

    PHANTOM_Marker(ID,angle,x,y,z,marker);
}

/******************************************************************************/

void    PHANTOM_Marker( int ID, float &x, float &y, float &z, int marker )
{
static  float  angle[PHANTOM_AXIS];

    PHANTOM_Angles(ID,angle,TRUE);
    PHANTOM_Marker(ID,angle,x,y,z,marker);
}

/******************************************************************************/

void    PHANTOM_Marker( int ID, double &x, double &y, double &z, int marker )
{
static  float  fx,fy,fz;

    PHANTOM_Marker(ID,fx,fy,fz,marker);

    x = (double)fx;
    y = (double)fy;
    z = (double)fz;
}

/******************************************************************************/

void    PHANTOM_Marker( int ID, matrix &posn, int marker )
{
    SPMX_DimPOMX(posn);
    PHANTOM_Marker(ID,posn(SPMX_POMX_XC),posn(SPMX_POMX_YC),posn(SPMX_POMX_ZC),marker);
}

/******************************************************************************/

void PHANTOM_QuadraticMotion( int ID, int dof, matrix &v, matrix &a )
{
matrix Y,X,P,T,XT;
int i;

    if( !PHANTOM_Item[ID].MotionWindow->Primed() ) return;

    PHANTOM_Item[ID].MotionWindow->GetWindow(dof,Y);
    PHANTOM_Item[ID].MotionWindow->GetWindow(0,T);

    matrix_dim(X,PHANTOM_Item[ID].MotionWindow->WindowSize(),3);

    for( i=1; (i <= X.rows()); i++ )
    {
        if( i == 1 )
        {
            X(i,2) = 0.0;
        }
        else
        {
            X(i,2) = X(i-1,2) - T(i-1,1);
        }

        X(i,1) = 1.0;
        X(i,3) = X(i,2) * X(i,2);
    }

    matrix_transpose(XT,X);

    P = inv(XT * X) * XT * Y;

    // Velocity...
    v(dof,1) = P(2,1) * 1000.0;

    // Acceleration...
    a(dof,1) = P(3,1) * 2.0 * 1000.0 * 1000.0;
}

/******************************************************************************/

void PHANTOM_QuadraticMotion( int ID, matrix &p, matrix &v, matrix &a )
{
int dof;

    // Put current position into window...
    PHANTOM_Item[ID].MotionWindow->Point(p,LOOPTASK_dt);

    for( dof=1; (dof <= 3); dof++ )
    {
        PHANTOM_QuadraticMotion(ID,dof,v,a);
    }
}

/******************************************************************************/

#include "phantom-default.cpp"         // General function for accessing default PHANTOM ID.

/******************************************************************************/
/* Functions to access motors / encoders for rod linkage experiements.   V2.5 */
/******************************************************************************/

long    PHANTOM_Force2TorqueMU( int axis, float length, float force )
{
float   fmax=12.0;      // Maximum force.
float   Ncm;            // Torque (Ncm).
long    MU;             // Motor Units.

    // Hack maximum force checking...
    force = mina(+fmax,force);
    force = maxa(-fmax,force);

    // Convert FORCE -> TORQUE -> MU and output to motor...
    Ncm = length * force;
    MU = PHANTOM_ConvNm2MU(PHANTOM_ID,axis,Ncm);

    return(MU);
}

/******************************************************************************/

float   PHANTOM_EU2Angle( int axis, long EU )
{
float   rev;            // Revolutions of shaft.
float   degrees;        // Angle in degrees.

    // Convert EU -> REVOLUTIONS -> DEGREES...
    rev = PHANTOM_ConvEU2Rev(PHANTOM_ID,axis,EU);
    degrees = rev * 360.0;

    return(degrees);
}

/******************************************************************************/

void    PHANTOM_RodForce( int axis, float force )
{
float   link=4.0;       // Rod length (cm).
long    MU;             // Motor Units.

    MU = PHANTOM_Force2TorqueMU(axis,link,force);
    PHANTOM_TorqueMU(PHANTOM_ID,axis,MU);
}

/******************************************************************************/

float   PHANTOM_RodAngle( int axis )
{
long    EU;             // Encoder units;
float   degrees;        // Angle in degrees.

    EU = PHANTOM_Encoder(PHANTOM_ID,axis);
    degrees = PHANTOM_EU2Angle(axis,EU);

    return(degrees);
}

/******************************************************************************/

