/******************************************************************************/
/*                                                                            */ 
/* MODULE  : ROBOT_Utility.cpp                                                */ 
/*                                                                            */ 
/* PURPOSE : Robot test and utility program                                   */ 
/*                                                                            */ 
/* DATE    : 18/Oct/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V2.0  JNI 18/Oct/2000 - Re-developed from DW's example.                    */ 
/*                                                                            */ 
/* V2.0a JNI 14/Aug/2001 - Allow encoder and spring zero via keyboard.        */ 
/*                                                                            */ 
/* V2.1  JNI 01/Feb/2002 - Allow encoder reset as command-line option.        */ 
/*                                                                            */ 
/* V2.2  JNI 22/Feb/2002 - Removed default SMALL. Must specifiy ROBOT.        */ 
/*                                                                            */ 
/* V3.0  JNI 08/Oct/2003 - Added new force fields and various other options   */ 
/*                         during development of Ian Howard's 2D robot.       */ 
/*                                                                            */ 
/* V3.1  JNI 15/Mar/2004 - Added I/O latency processing ("/T" option).        */ 
/*                                                                            */ 
/* V3.2  JNI 20/Jul/2006 - Renamed during the big shake-up of 2006.           */ 
/*                                                                            */ 
/* V3.3  JNI 24/Oct/2012 - Controller test option 'G'.                        */ 
/*                                                                            */ 
/* V4.0  JNI 16/Dec/2014 - Changes required for 3BOTs and Windows 7.          */ 
/*                                                                            */ 
/* V4.1  JNI 30/Sep/2015 - Default force field is NONE (not SPRING).          */ 
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME "ROBOT_Utility"
#define MODULE_TEXT "ROBOT Utility"
#define MODULE_DATE "16/12/2014"
#define MODULE_VERS "4.0"

/******************************************************************************/

#include <motor.h>                               // Includes everything we need.


/******************************************************************************/

// #define OPTO

/******************************************************************************/

STRING RobotName="";

BOOL ProgramExitFlag=FALSE;

BOOL DebugRobot=FALSE;
BOOL DebugLoopTask=FALSE;
BOOL DebugSensoray=FALSE;
BOOL LatencyFlag=FALSE;

BOOL ControllerTestFlag=FALSE;
int  ControllerTestChannel=0;

#define FIELD_NONE       0
#define FIELD_SPRING     1
#define FIELD_VISCOUS    2
#define FIELD_INERTIAL   3
#define FIELD_CIRCULAR   4
#define FIELD_HOLE       5
#define FIELD_FIXED      6
#define FIELD_CALIBRATE  7
#define FIELD_TORQUE_MU  8
#define FIELD_TORQUE_Nm  9
#define FIELD_VS        10

struct  STR_TextItem  FieldText[] = 
{ 
    { FIELD_NONE     ,"FieldNone"     },
    { FIELD_SPRING   ,"Spring"        },
    { FIELD_VISCOUS  ,"Viscous"       },
    { FIELD_INERTIAL ,"Inertial"      },
    { FIELD_CIRCULAR ,"Circular"      },
    { FIELD_HOLE     ,"Hole"          },
    { FIELD_FIXED    ,"Fixed"         },
    { FIELD_CALIBRATE,"Calibrate"     },
    { FIELD_TORQUE_MU,"Torque MU"     },
    { FIELD_TORQUE_Nm,"Torque Nm"     },
    { FIELD_VS       ,"ViscousSpring" },
    { STR_TEXT_ENDOFTABLE }
};

BOOL FieldUnstable[] = { FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,TRUE,TRUE,FALSE };

// Default field type is NONE (V4.0)
int    FieldType=FIELD_SPRING;
float  FieldConstant[4]={ -1.0,0.0,0.0,0.0 };
float  FieldFixed[3]={ 0.0,0.0,0.0 };
float  FieldFixedGain=0.0;
matrix FieldRotate;
float  FieldRotateZ=0.0;
TIMER  FieldTimer("Field");
double FieldTime;
int    FieldIndex=-2;
double FieldScale=1.0;
int    FieldCount,FieldDirections,FieldScales;
matrix FieldData;
double FieldTheta;
double FieldRamp=0.0;

matrix Fa(3,1);
matrix Fr(3,1);
matrix Ta(3,1);
matrix Tr(3,1);
matrix Mr(3,1);
matrix Frow;
matrix Fcal;
matrix JT;

TIMER  SpringTimer("SPRING");
STRING FileName="";
BOOL   FileSave=FALSE;

/******************************************************************************/
matrix  RobotPosition(SPMX_POMX_MTX);            // Current position.
matrix  RobotPositionLast;                       // Last position.
double  RobotDistanceStep=0.0;
double  RobotDistanceStepTotal=0.0;
matrix  RobotVelocity(SPMX_POMX_MTX);            // Velocity.
matrix  RobotAcceleration(SPMX_POMX_MTX);        // Acceleration.

matrix  T_Nm(3,1);                               // Torque in Newton Metres.
matrix  T_MU(3,1);                               // Torque in Motor Units.

matrix  SpringHome(SPMX_POMX_MTX);               // Starting position.
matrix  SpringStretch(SPMX_POMX_MTX);            // Stretch of spring.
matrix  CircleCentre(SPMX_POMX_MTX);
matrix  RobotForces(3,1);                        // Forces in 3 dimensions (x,y,z).
matrix  RobotTorques(3,1);                       // Torques.
matrix  RobotAngles(3,1);                        // Angles for 3 axes.
matrix  RobotAngularVelocity(3,1);               // Angular velocity for 3 axes.
matrix  RobotAngularAcceleration(3,1);           // Angular acceleration for 3 axes.
double  SpringStretchDistance;

double  EncoderCount[ROBOT_AXIS];
double  EncoderCountLast[ROBOT_AXIS];
double  EncoderCountZero[ROBOT_AXIS];
double  EncoderCountStep[ROBOT_AXIS];
double  EncoderMaxCPS[ROBOT_AXIS];
double  EncoderCPS[ROBOT_AXIS];
matrix  EncoderCountStepMatrix(ROBOT_AXIS,1);
BOOL    EncoderZero=TRUE;                        // Zero encoders at current position.

BOOL    FieldZero=TRUE;                          // Zero spring at current position.
BOOL    EncoderReset=FALSE;                      // Reset encoders before starting spring.
double  ForceMax=40.0;                           // Maximum force.
double  ForceUnstable=15.0;                      // Maximum force for unstable field.

matrix  OptoPosn(SPMX_POMX_MTX);
matrix  OptoZero(SPMX_POMX_MTX);

/******************************************************************************/

matrix SensorForces(AXIS_XYZ,1);
matrix SensorTorques(AXIS_XYZ,1);
matrix AccelerometerXY(AXIS_XY,1);
WINFIT *ForceWindow=NULL;
TIMER_Interval SensorReadLatencyTimer("SensorReadLatency");
TIMER_Interval SensorGetLatencyTimer("SensorGetLatency");
TIMER_Frequency SensorFrequencyTimer("SensorFrequency");
TIMER SensorBiasResetTimer("SensorBiasReset");

TIMER_Frequency  RobotControlLoopFrequencyTimer("RobotControlLoop");
TIMER_Frequency  RobotNonControlLoopFrequencyTimer("RobotNonControlLoop");

/******************************************************************************/

BOOL TrialFlag=FALSE;
MATDAT TrialData("TrialData");
int Trials=2000;
int Trial;

/******************************************************************************/

void TrialInitialize( void )
{
    Trial = 1;

    // Add each variable to the TrialData matrix.
    TrialData.AddVariable(VAR(RobotPosition));
    TrialData.AddVariable(VAR(RobotAngles));
    TrialData.AddVariable(VAR(RobotForces));
    TrialData.AddVariable(VAR(RobotTorques));
    TrialData.AddVariable(VAR(SensorForces));
    TrialData.AddVariable(VAR(SensorTorques));
    
    // Set rows of TrialData to the number of trials.
    TrialData.SetRows(Trials);
}

/******************************************************************************/

BOOL TrialSave( void )
{
BOOL ok=FALSE;

    // Put values in the trial data
    TrialData.RowSave(Trial);

    // Set-up the data file on the first trial.
    if( Trial == 1 )
    {
        // Open the file for trial data.
        if( !DATAFILE_Open(FileName,TrialData) )
        {
            printf("DATAFILE: Cannot open file: %s\n",FileName);
            return(FALSE);
        }
    }

    // Write the trial data to the file.
    ok = DATAFILE_TrialSave(Trial);
    printf("%s %s Trial=%d.\n",FileName,STR_OkFailed(ok),Trial);

    Trial++;

    return(ok);
}

/******************************************************************************/

void TrialClose( void )
{
    // Only continue if data file has been opened.
    if( !DATAFILE_Opened() )
    {
        return;
    }

    // Close data file.
    DATAFILE_Close();
}

/******************************************************************************/

BOOL SensorOpen( void )
{
    if( !ROBOT_SensorOpen() )
    {
        printf("SENSOR: Cannot open sensors.\n");
        return(FALSE);
    }

    if( ROBOT_SensorOpened_DAQFT() )
    {
        printf("SENSOR: DAQFT sensor opened.\n");
    }

    if( ROBOT_SensorOpened_Accelerometer() )
    {
        printf("SENSOR: Accelerometers opened.\n");
    }

    ForceWindow = new WINFIT("ForceWindow",200,AXIS_XYZ);

    return(TRUE);
}

/******************************************************************************/

void SensorClose( void )
{
    if( !ROBOT_SensorOpened() )
    {
        return;
    }
     
    ROBOT_SensorClose();

    SensorReadLatencyTimer.Results();
    SensorGetLatencyTimer.Results();
    SensorFrequencyTimer.Results();
}

/******************************************************************************/

void SensorRead( void )
{
    if( !ROBOT_SensorOpened() )
    {
    	return;
    }

    // Read forces from F/T sensor...
    SensorFrequencyTimer.Loop();

    SensorReadLatencyTimer.Before();
    ROBOT_SensorRead(); // This actually reads the ADC channels from the Sensoray
    SensorReadLatencyTimer.After();

    // Get DAQFT reading if present.
    SensorGetLatencyTimer.Before();
    if( ROBOT_SensorOpened_DAQFT() )
    {
        ROBOT_Sensor_DAQFT(SensorForces);

        // Calculate motor torques from sensor forces.
        //ROBOT_Forces2MotorTorques(SensorForces,SensorTorques);
    }
    SensorGetLatencyTimer.After();

    // Get accelerometer reading if present.
    if( ROBOT_SensorOpened_Accelerometer() )
    {
        ROBOT_Sensor_Accelerometer(AccelerometerXY);
    }
}

/******************************************************************************/

void SensorBiasReset( void )
{
BOOL ok;

    if( ROBOT_SensorOpened_DAQFT() )
    {
        SensorBiasResetTimer.Reset();
        ok = ROBOT_SensorBiasReset_DAQFT();
        printf("%s: DAQFT bias reset (%.3lf sec): %s.\n",RobotName,SensorBiasResetTimer.ElapsedSeconds(),STR_OkFailed(ok));
    }

    if( ROBOT_SensorOpened_Accelerometer() )
    {
        SensorBiasResetTimer.Reset();
        ok = ROBOT_SensorBiasResetRetrograde_Accelerometer();
        printf("%s: Accelerometer bias reset (%.3lf sec): %s.\n",RobotName,SensorBiasResetTimer.ElapsedSeconds(),STR_OkFailed(ok));
    }
}

/******************************************************************************/

BOOL SensorBiasWait( void )
{
BOOL flag=FALSE;

    if( ROBOT_SensorOpened_DAQFT() )
    {
        flag |= ROBOT_SensorBiasWait_DAQFT();
    }

    if( ROBOT_SensorOpened_Accelerometer() )
    {
        flag |= ROBOT_SensorBiasWait_Accelerometer();
    }

    return(flag);
}

/******************************************************************************/

#ifdef OPTO
void OPTOTRAK_Read( void )
{
UINT frame;
matrix posn,seen;

    if( !TIMER_EveryHz(OPTO_Frequency) )
    {
        return;
    }

    frame = OPTO_GetPosn(posn,seen);

    if( seen.B(1,7) )
    {
        OptoPosn = SPMX_ptmx2pomx(posn[7]);
    }
    else
    {
        if( TIMER_EveryHz(4.0) )
        {
            printf("Cannot see OptoTrak marker.\n");
        }
    }
}
#endif

/******************************************************************************/
/* Display functions - Optionally installed as LOOPTASK functions.            */
/******************************************************************************/

void DisplayPosition( void )
{
    printf("P %5.2lf %5.1lf %5.1lf",RobotPosition(1,1),RobotPosition(2,1),RobotPosition(3,1));
    printf(" | ");
    printf("V %6.1lf %6.1lf %6.1lf",RobotVelocity(1,1),RobotVelocity(2,1),RobotVelocity(3,1));
    printf(" | ");
    printf("A %6.1lf %6.1lf %6.1lf",RobotAcceleration(1,1),RobotAcceleration(2,1),RobotAcceleration(3,1));
    printf("\n");
}

/******************************************************************************/

void _DisplayPosition( void )
{
double Vdouble;
int Vint;
WORD Vword;
BYTE Vbyte;

    Vdouble = RobotVelocity(1,1);
    Vint = (int)Vdouble;
    Vword = (WORD)Vdouble;
    Vbyte = (BYTE)Vdouble;

    printf("x=%.1lf (abs(x)>=10.0)=%d (abs(x)<10.0)=%d Undef()=%d int=%d word=%d byte=%d\n",Vdouble,(fabs(Vdouble) >= 10.0),(fabs(Vdouble) < 10.0),UndefinedDouble(Vdouble),Vint,Vword,Vbyte);
}

/******************************************************************************/

void DisplayEncoder( void )
{
    printf("EU[] %6.0lf %6.0lf %6.0lf %6.0lf %6.0lf %6.0lf kC/sec %4.0lf %4.0lf %4.0lf\n",
          EncoderCount[0],EncoderCount[1],EncoderCount[2],
          EncoderCountZero[0]-EncoderCount[0],
          EncoderCountZero[1]-EncoderCount[1],
          EncoderCountZero[2]-EncoderCount[2],
          EncoderMaxCPS[0]/1000.0,
          EncoderMaxCPS[1]/1000.0,
          EncoderMaxCPS[2]/1000.0);
}

/******************************************************************************/

void DisplayAngle( void )
{
double r[ROBOT_AXIS],a[ROBOT_AXIS];

    ROBOT_AnglesRaw(r);
    ROBOT_Angles(a);

    printf("Ang[] Raw %6.5lf %6.5lf %6.5lf Adj %6.5lf %6.5lf %6.5lf\n",R2D(r[0]),R2D(r[1]),R2D(r[2]),R2D(a[0]),R2D(a[1]),R2D(a[2]));
}

/******************************************************************************/

void DisplayTemperature( void )
{
double T[ROBOT_AXIS];

    ROBOT_TempMotor(T);
    printf("Temp[1,2,3] %5.1f %5.1f %5.1f\n",T[0],T[1],T[2]);
}

/******************************************************************************/

void DisplayTorque( void )
{
    printf("R %3.1f R_Nm %5.2f %5.2f %5.2f M_Nm %4.1f %4.1f %4.1f MU %6ld %6ld %6ld\n",
            ROBOT_RampValue(),
            ROBOT_MotorGetNm(0),
            ROBOT_MotorGetNm(1),
            ROBOT_MotorGetNm(2),
            SensorTorques(1,1),
            SensorTorques(2,1),
            SensorTorques(3,1),
            ROBOT_MotorGetMU(0),
            ROBOT_MotorGetMU(1),
            ROBOT_MotorGetMU(2));
}

/******************************************************************************/

void DisplayForce( void )
{
    printf("Force[xyz,mag] R %5.1lf %5.1lf %5.1lf %5.1lf M %5.1lf %5.1lf %5.1lf",
            RobotForces(1,1),
            RobotForces(2,1),
            RobotForces(3,1),
            norm(RobotForces),
            SensorForces(1,1),
            SensorForces(2,1),
            SensorForces(3,1));

    if( ROBOT_2D() )
    {
        SensorForces(SPMX_POMX_ZC) = 0.0;
    }

    printf(" %5.1lf\n",norm(SensorForces));
}

/******************************************************************************/

void DisplayStretch( void )
{
    printf("Stretch[x,y,z,norm] %4.1lf %4.1lf %4.1lf %4.1lf\n",
            SpringStretch(SPMX_POMX_XC),
            SpringStretch(SPMX_POMX_YC),
            SpringStretch(SPMX_POMX_ZC),
            norm(SpringStretch));
}

/******************************************************************************/

void DisplayMarker0( void )
{
matrix  posn;
int marker=0;

    ROBOT_PositionMarker(marker,posn);
    printf("%4.1lf,%4.1lf,%4.1lf\n",posn(1,1),posn(2,1),posn(3,1));
}

/******************************************************************************/

void DisplaySensors( void )
{
    printf("F[xyz] %5.1lf %5.1lf %5.1lf (N) ",SensorForces(1,1),SensorForces(2,1),SensorForces(3,1));
    printf("A(xy) %5.1lf %5.1lf (m/sec^2)\n",AccelerometerXY(1,1),AccelerometerXY(2,1));
}

/******************************************************************************/

struct DisplayItem
{
    void (*func)( void );
    float  freq;
    char  *text;
    char   code;
    BOOL   used;
};

struct  DisplayItem DisplayList[] = 
{
    { DisplayPosition   ,4.0,"Position"   ,'P',FALSE },
    { DisplayEncoder    ,4.0,"Encoder"    ,'E',FALSE },
    { DisplayAngle      ,4.0,"Angle"      ,'A',FALSE },
    { DisplayTemperature,1.0,"Temperature",'T',FALSE },
    { DisplayTorque     ,4.0,"Torque"     ,'Q',FALSE },
    { DisplayForce      ,4.0,"Force"      ,'F',FALSE },
    { DisplayStretch    ,4.0,"Stretch"    ,'S',FALSE },
    { DisplayMarker0    ,4.0,"Marker(0)"  ,'M',FALSE },
    { DisplaySensors    ,4.0,"Sensors"    ,'N',FALSE },
    { NULL              ,0.0,NULL         ,' ',FALSE },
};

/******************************************************************************/

#define DATA_POINTS  100000
#ifdef  OPTO
#define DATA_COLUMNS 38
#else
#define DATA_COLUMNS 35
#endif
int DataPoint=0;
matrix Data(DATA_POINTS,DATA_COLUMNS);
matrix DataRow;

/******************************************************************************/

void ForcesFunctionTorques( long EU[], long MU[] )
{
int axis;
double value;

    SensorRead();

    for( axis=0; (axis < 3); axis++ )
    {
        value = (FieldFixedGain * FieldFixed[axis]);

        switch( FieldType )
        {
            case FIELD_TORQUE_MU :
               MU[axis] = (long)value;
               break;

            case FIELD_TORQUE_Nm :
               MU[axis] = ROBOT_ConvNm2MU(ROBOT_ID,axis,value);
               break;
        }
    }
}

/******************************************************************************/

#define CALIBRATION_STATE_INITIALIZE   0
#define CALIBRATION_STATE_START        1
#define CALIBRATION_STATE_RAMPUP       2 
#define CALIBRATION_STATE_SETTLE       3
#define CALIBRATION_STATE_MEAUSRE      4
#define CALIBRATION_STATE_RAMPDOWN     5
#define CALIBRATION_STATE_CENTRE       6
#define CALIBRATION_STATE_NEXT         7
#define CALIBRATION_STATE_FINISH       8 

char *CalibrationStateText[] = { "Inilialize","Start","RampUp","Settle","Measure","RampDown","Centre","Next","Finish" };
int   CalibrationState=CALIBRATION_STATE_INITIALIZE;
int   CalibrationStateLast=-1;
TIMER CalibrationStateTimer("CalibrationState");

/******************************************************************************/

void CalibrationProcess( void )
{
BOOL ok;

    if( !ROBOT_Activated() || SensorBiasWait() )
    {
        return;
    }

    if( CalibrationState != CalibrationStateLast )
    {
        printf("STATE: ");
        if( CalibrationStateLast != -1 )
        {
            printf("[%s] -> ",CalibrationStateText[CalibrationStateLast]);
        }

        printf("[%s]\n",CalibrationStateText[CalibrationState]);
        CalibrationStateLast = CalibrationState;

        CalibrationStateTimer.Reset();
    }

    switch( CalibrationState )
    {
        case CALIBRATION_STATE_INITIALIZE :
           SpringHome = RobotPosition;
           FieldIndex = 1;
           CalibrationState++;
           break;

        case CALIBRATION_STATE_START :
           FieldTheta = FieldData(FieldIndex,1);
           FieldScale = FieldData(FieldIndex,2);
           FieldRamp = 0.0;
           ForceWindow->Reset();
           SensorBiasReset();
           printf("%d/%d: %5.1lf k=%0.2lf\n",FieldIndex,FieldCount,FieldTheta,FieldScale);
           CalibrationState++;
           break;

        case CALIBRATION_STATE_RAMPUP :
           FieldRamp = (CalibrationStateTimer.ElapsedSeconds() / 2.0);
           if( FieldRamp >= 1.0 )
           {
               FieldRamp = 1.0;
               CalibrationState++;
           }
           break;

        case CALIBRATION_STATE_SETTLE :
           if( CalibrationStateTimer.ExpiredSeconds(FieldConstant[2]) )
           {
               CalibrationState++;
               break;
           }
           break;

        case CALIBRATION_STATE_MEAUSRE :
           if( !CalibrationStateTimer.ExpiredSeconds(FieldConstant[2]) )
           {
               ForceWindow->Point(SensorForces);
               break;
           }

           JT = ROBOT_JT[ROBOT_ID];

           Fr(1,1) = RobotForces(1,1);
           Fr(2,1) = RobotForces(2,1);
           Fr(3,1) = RobotForces(3,1);

           Tr(1,1) = ROBOT_MotorGetNm(0);
           Tr(2,1) = ROBOT_MotorGetNm(1);
           Tr(3,1) = ROBOT_MotorGetNm(2);

           Mr(1,1) = ROBOT_MotorGetMU(0);
           Mr(2,1) = ROBOT_MotorGetMU(1);
           Mr(3,1) = ROBOT_MotorGetMU(2);

           // Get mean of forces taken during measurement window...
           ForceWindow->Mean(Fa);
           // Calculate "measured" torques based on measured forces...
           ROBOT_Forces2MotorTorques(ROBOT_ID,Fa,Ta);

           CalibrationState++;
           break;

        case CALIBRATION_STATE_RAMPDOWN :
           FieldRamp = 1.0 - CalibrationStateTimer.ElapsedSeconds();
           if( FieldRamp <= 0.0 )
           {
               FieldRamp = 0.0;
               CalibrationState++;
           }
           break;

        case CALIBRATION_STATE_CENTRE :
           FieldRamp = CalibrationStateTimer.ElapsedSeconds();

           if( FieldRamp > 1.0 )
           {
               FieldRamp = 0.0;
               CalibrationState++;
           }

           SpringStretch = RobotPosition - SpringHome;
           RobotForces = (-5.0 * FieldRamp) * SpringStretch;
           break;

        case CALIBRATION_STATE_NEXT :
           Frow = scalar(FieldTheta) % Fr % scalar(norm(Fr)) % Fa % scalar(norm(Fa)) % Tr % Ta % JT[1] % JT[2] % JT[3] % Mr;
           // 1 Angle
           // 2,3,4 Requested Force x,y,z
           // 5 Requested Force normal
           // 6,7,8 Measured Force x,y,z
           // 9 Measured Force normal
           // 10,11,12 Requested Torques
           // 13,14,15 Actual Torques
           // 16,19,22 Rows of JT
           // 25,26,27 DAC Units
           Fcal %= T(Frow);

           if( ++FieldIndex > FieldCount )
           {
               CalibrationState++;
               break;
           }

           CalibrationState = CALIBRATION_STATE_START;
           break;

        case CALIBRATION_STATE_FINISH :
           ProgramExitFlag = TRUE;
           break;
    }

    if( (CalibrationState >= CALIBRATION_STATE_RAMPUP) && (CalibrationState <= CALIBRATION_STATE_RAMPDOWN) )
    {
        RobotForces(1,1) = FieldConstant[1] * FieldRamp * cos(D2R(FieldTheta)) * FieldScale;
        RobotForces(2,1) = FieldConstant[1] * FieldRamp * sin(D2R(FieldTheta)) * FieldScale;
        RobotForces(3,1) = 0.0;
    }

    //RobotForces.zero();
}

/******************************************************************************/

#define CONTROLLER_TEST_STATE_INIT     0
#define CONTROLLER_TEST_STATE_START    1
#define CONTROLLER_TEST_STATE_UP       2
#define CONTROLLER_TEST_STATE_RAMPUP   3
#define CONTROLLER_TEST_STATE_HOLD     4
#define CONTROLLER_TEST_STATE_DOWN     5
#define CONTROLLER_TEST_STATE_RAMPDOWN 6
#define CONTROLLER_TEST_STATE_NEXT     7
#define CONTROLLER_TEST_STATE_DONE     8

char  *ControllerTestStateText[] = { "Initialize","Start","RampUp","RampingUp","Hold","RampDown","RampingDown","Next","Done" };
BOOL   ControllerTestStateTorque[] = { FALSE,FALSE,TRUE,TRUE,TRUE,TRUE,TRUE,FALSE,FALSE };
int    ControllerTestState=CONTROLLER_TEST_STATE_INIT;
TIMER  ControllerTestStateTimer("ControllerTestStateTimer");
TIMER  ControllerTestCPSTimer("ControllerTestCPSTimer",TIMER_MODE_RESET);
double ControllerTestCPSTime=0.25;
long   ControllerTestLastEU=0L;
double ControllerTestCPS=0.0;
double ControllerTestTorque=0.0;
double ControllerTestTorqueValue=0.5;
double ControllerTestTorqueSign=1.0;
double ControllerTestRampTime=2.0;
double ControllerTestHoldTime=10.0;
double ControllerTestExitDelay=1.0;
RAMPER ControllerTestRamp("ControllerTestRamp");

/******************************************************************************/

void ControllerTestStateNext( int state )
{
    if( ControllerTestState != state )
    {
        //printf("%s[%d] > %s[%d] %.0lf(msec)\n",ControllerTestStateText[ControllerTestState],ControllerTestState,ControllerTestStateText[state],state,ControllerTestStateTimer.Elapsed());
    }

    ControllerTestState = state;
    ControllerTestStateTimer.Reset();
}

/******************************************************************************/

void ForcesFunctionControllerTest( long EU[], long MU[] )
{
    switch( ControllerTestState )
    {
        case CONTROLLER_TEST_STATE_INIT :
           ControllerTestCPSTimer.Reset();
           ControllerTestTorqueSign = 1.0;
           ControllerTestRamp.Zero();
           ControllerTestStateNext(CONTROLLER_TEST_STATE_START);
           break;

        case CONTROLLER_TEST_STATE_START :
           if( ROBOT_Activated(ROBOT_ID) )
           {
               ControllerTestStateNext(CONTROLLER_TEST_STATE_UP);
           }
           break;

        case CONTROLLER_TEST_STATE_UP :
           ControllerTestRamp.Up();
           ControllerTestStateNext(CONTROLLER_TEST_STATE_RAMPUP);
           break;

        case CONTROLLER_TEST_STATE_RAMPUP :
           if( ControllerTestRamp.RampComplete() )
           {
               ControllerTestStateNext(CONTROLLER_TEST_STATE_HOLD);
           }
           break;

        case CONTROLLER_TEST_STATE_HOLD :
           if( ControllerTestStateTimer.ExpiredSeconds(ControllerTestHoldTime) )
           {
               ControllerTestStateNext(CONTROLLER_TEST_STATE_DOWN);
           }
           break;

        case CONTROLLER_TEST_STATE_DOWN :
           ControllerTestRamp.Down();
           ControllerTestStateNext(CONTROLLER_TEST_STATE_RAMPDOWN);
           break;

        case CONTROLLER_TEST_STATE_RAMPDOWN :
           if( ControllerTestRamp.RampComplete() )
           {
               ControllerTestStateNext(CONTROLLER_TEST_STATE_NEXT);
           }
           break;

        case CONTROLLER_TEST_STATE_NEXT :
           if( ControllerTestTorqueSign == 1.0 )
           {
               ControllerTestTorqueSign = -1.0;
               ControllerTestStateNext(CONTROLLER_TEST_STATE_START);
           }
           else
           {
               ControllerTestStateNext(CONTROLLER_TEST_STATE_DONE);
           }
           break;

        case CONTROLLER_TEST_STATE_DONE :
           if( ControllerTestStateTimer.ExpiredSeconds(ControllerTestExitDelay) )
           {
               ProgramExitFlag = TRUE;
           }
           break;
    }

    if( ControllerTestCPSTimer.ExpiredSeconds(ControllerTestCPSTime) )
    {
        ControllerTestCPS = ((double)(ControllerTestLastEU - EU[ControllerTestChannel]) / ControllerTestCPSTime);
        ControllerTestLastEU = EU[ControllerTestChannel];
    }

    if( ControllerTestStateTorque[ControllerTestState] )
    {
        ControllerTestTorque = ControllerTestRamp.RampDouble(ControllerTestTorqueSign * ControllerTestTorqueValue);
        MU[ControllerTestChannel] = ROBOT_ConvNm2MU(ROBOT_ID,ControllerTestChannel,ControllerTestTorque);
    }
}

/******************************************************************************/

void RobotNonControlLoop( void )
{
int axis;

    RobotNonControlLoopFrequencyTimer.Loop();
    SensorRead();

    if( DataPoint < DATA_POINTS )
    {
        for( axis=0; (axis < ROBOT_AXIS); axis++ )
        {
            T_Nm(axis+1,1) = ROBOT_MotorGetNm(axis);
            T_MU(axis+1,1) = ROBOT_MotorGetMU(axis);
        }

        DataRow = scalar(LOOPTASK_dt) % RobotAngles % RobotPosition % RobotVelocity % RobotAcceleration % RobotForces % SensorForces % RobotAngularVelocity % T_Nm % T_MU % SensorTorques % EncoderCountStepMatrix % scalar(RobotDistanceStep);

#ifdef OPTO
    DataRow = DataRow % (OptoPosn - OptoZero);
#endif
        matrix_array_put(Data,T(DataRow),++DataPoint);
    }
}

/******************************************************************************/

void _ForcesFunctionSpring( matrix &p, matrix &v, matrix &f )
{
    RobotControlLoopFrequencyTimer.Loop();

    RobotPosition = p;
    RobotVelocity = v;
    RobotForces.zeros();

    if( ROBOT_JustActivated() )
    {
        SpringHome = RobotPosition;
    }

    // Stretch is difference from zero position...
    SpringStretch = RobotPosition - SpringHome;
    SpringStretchDistance = norm(SpringStretch);

    // Set forces depending on field type...
    switch( FieldType )
    {
        case FIELD_NONE :
           RobotForces.zeros();
           break;

        case FIELD_SPRING :
           RobotForces = FieldConstant[0] * SpringStretch;
           break;

        case FIELD_VISCOUS :
           RobotForces = FieldConstant[0] * RobotVelocity;
           break;
 
        case FIELD_VS :
           RobotForces = (FieldConstant[0] * SpringStretch) + (FieldConstant[1] * RobotVelocity);
           break;
    }

    //RobotForces(2,1) = 0.0;
    RobotForces.clampnorm(ForceMax);

    if( ROBOT_Activated() )
    {    
        f = RobotForces;
    }
    else
    {
        f.zeros();
    }
}

/******************************************************************************/

//void ForcesFunctionSpring( matrix &p, matrix &v, matrix &a, matrix &f )
void ForcesFunctionSpring( matrix &p, matrix &v, matrix &f )
{
static matrix a(3,1);
double radians;
int axis;

    // Zero forces...
    f.zeros();

    SensorRead();

    // Set or reset home position for spring...
    if( ROBOT_PanicOn2Off() )
    {
        //DataPoint = 0;
        EncoderZero = TRUE;
        printf("Resume!\n");
    }
    else
    if( ROBOT_PanicOff2On() )
    {
        printf("Panic!\n");
    }

    if( ROBOT_Panic() || FieldZero )
    {
        SpringHome = p;
        OptoZero = OptoPosn;
        SpringTimer.Reset();
        FieldZero = FALSE;
    }

    ROBOT_Encoder(EncoderCount);

    for( axis=0; (axis < ROBOT_AXIS); axis++ )
    {
        RobotTorques(axis+1,1) = ROBOT_MotorGetNm(axis);

        if( EncoderZero )
        {
            EncoderCountZero[axis] = EncoderCount[axis];
            EncoderCountLast[axis] = EncoderCount[axis];

            EncoderCPS[axis] = 0;
            EncoderMaxCPS[axis] = 0;
        }

        EncoderCountStep[axis] = EncoderCount[axis] - EncoderCountLast[axis];
        EncoderCountLast[axis] = EncoderCount[axis];
        EncoderCountStepMatrix(axis+1,1) = EncoderCountStep[axis];
        EncoderCPS[axis] = fabs(EncoderCountStep[axis]) / (LOOPTASK_dt/1000.0);

        if( EncoderMaxCPS[axis] < EncoderCPS[axis] )
        {
            EncoderMaxCPS[axis] = EncoderCPS[axis];
        }
    }

    EncoderZero = FALSE;

    // Make position, RobotVelocityocity and acceleration global...
    RobotPosition = p;
    RobotVelocity = v;
    RobotAcceleration = a;
    ROBOT_Angles(RobotAngles,RobotAngularVelocity,RobotAngularAcceleration);

    if( RobotPositionLast.isempty() )
    {
        RobotPositionLast.dim(SPMX_POMX_MTX);
        RobotDistanceStep = 0.0;
        RobotDistanceStepTotal = 0.0;
    }
    else
    {
        RobotDistanceStep = norm(RobotPosition-RobotPositionLast);
        RobotDistanceStepTotal += RobotDistanceStep;
    }

    RobotPositionLast = RobotPosition;

    // Stretch is difference from zero position...
    SpringStretch = RobotPosition - SpringHome;
    SpringStretchDistance = norm(SpringStretch);

    // Set forces depending on field type...
    switch( FieldType )
    {
        case FIELD_NONE :
           f.zeros();
           break;

        case FIELD_SPRING :   // SPRING: Forces increase as position moves further away from zero...
           f = FieldConstant[0] * FieldRotate * SpringStretch;
		   //printf("===================\n");
		   //printf("ROBOT_POSITION: %G, %G\n", RobotPosition(1, 1), RobotPosition(2, 1), RobotPosition(3, 1));
		   //printf("SPRING_HOME: %G, %G\n", SpringHome(1, 1), SpringHome(2, 1), SpringHome(3, 1));
		   //printf("%d, %d  Field constant: %f\n", FieldRotate.col, FieldRotate.row, FieldConstant[0]);
		   //printf("%d, %d  ",f.col, f.row);
		   //printf("%G, %G, %G\n",f(1,1),f(2,1),f(3,1));
           //f = (FieldConstant[0] * SpringStretch) + (-0.4 * RobotVelocity);
           break;

        case FIELD_VISCOUS :  // VISCOUS: Forces increase inversely with RobotVelocityocity...
           f = FieldConstant[0] * FieldRotate * RobotVelocity;
           break;

        case FIELD_INERTIAL : // INTERTIAL: Forces increase inversely with acceleration...
           f = FieldConstant[0] * RobotAcceleration;
           break;

        case FIELD_CIRCULAR : // CIRCULAR: Forces drag arm around in a circle...
           radians = 2.0 * PI * FieldConstant[2] * SpringTimer.ElapsedSeconds();

           CircleCentre(1,1) = SpringHome(1,1) + (FieldConstant[1] / 2.0) * (1.0 - cos(radians));
           CircleCentre(2,1) = SpringHome(2,1) + (FieldConstant[1] / 2.0) * sin(radians);
           CircleCentre(3,1) = RobotPosition(3,1);

           SpringStretch = RobotPosition - CircleCentre;

           f = FieldConstant[0] * SpringStretch - (0.1 * RobotVelocity);
           break;

        case FIELD_HOLE :   // HOLE: Trapped in a hole...
           if( SpringStretchDistance > FieldConstant[1] )
           {
               f = FieldConstant[0] * (SpringStretchDistance - FieldConstant[1]) * (SpringStretch / SpringStretchDistance);
               f -= 0.03 * RobotVelocity;
           }
           break;

        case FIELD_FIXED :
           matrix_float(FieldFixed,f);
           f *= FieldFixedGain;
           break;

        case FIELD_CALIBRATE :
           CalibrationProcess();
           f = RobotForces;
           break;
    }

    // Check for maximum force...
    f.clampnorm(ForceMax);

    // Make forces available in global variable...
    RobotForces = f;
}

/******************************************************************************/

void Usage( void )
{
    printf("--------------------------------------------------------------\n");
    printf("Usage: %s /P:ROBOT [options]\n",MODULE_NAME);
    printf(" /S:k       Position dependent (spring) field (k=N/cm).\n");
    printf(" /V:k       Velocity dependent (viscious) field (k=N/cm/sec).\n");
    printf(" /A:k       Acceleration dependent (inertial) field (k=N/cm/sec/sec).\n");
    printf(" /C:k,r,f   Circular field (k=N/cm,r=cm,f=Hz).\n");
    printf(" /H:k,r     Hole field (k=N/cm,r=cm).\n");
    printf(" /X:x,y,z   Contsant force.\n");
    printf(" /B:d,f,t,l Calibrate forces (d=directions,f=force(N),t=time(sec),n=levels).\n");
    printf(" /F:max     Force maximum (N).\n");
    printf(" /R:angle   Z-axis rotation angle (degrees).\n");
    printf(" /L:file    Log position, velocity, acceleration & force data.\n");
    printf(" /M:s1,...  Torque output in MU (s1,s2,s3=step(MU)).\n");
    printf(" /Q:s1,...  Torque output in Nm (s1,s2,s3=step(Nm)).\n");
    printf(" /E         Encoder Reset.\n");
    printf(" /T         Time controller latencies.\n");
    printf(" /Z:file    Force sensor test trials.\n");
    printf(" /D:abc     Display options below.\n");
    printf("            P - Position, velocity & acceleration.\n");
    printf("            M - Marker [0].\n");
    printf("            E - Raw Encoder Values.\n");
    printf("            A - Angles.\n");
    printf("            T - Temperature.\n");
    printf("            Q - Torques.\n");
    printf("            F - Forces.\n");
    printf("            S - Spring Stretch.\n");
    printf("            N - Sensor values.\n");
    printf("            D - ROBOT Debug.\n");
    printf("            L - LOOPTASK Debug.\n");
    printf("            X - SENSORAY Debug.\n");
    printf("--------------------------------------------------------------\n");
}

/******************************************************************************/

void Help( void )
{
    Usage();
    ExitProgram(0);
}

/******************************************************************************/

BOOL Display( char *options )
{
int     i,len,task;
BOOL    ok=FALSE;

    for( ok=TRUE,len=strlen(options),i=0; ((i < len) && ok); i++ )
    {
        switch( options[i] )
        {
            case 'D' :
               DebugRobot = TRUE;
               break;

            case 'L' :
               DebugLoopTask = TRUE;
               break;

            case 'X' :
               DebugSensoray = TRUE;
               break;

            default :
               for( task=0; ((DisplayList[task].func != NULL) && (DisplayList[task].code != options[i])); task++ );

               if( DisplayList[task].func == NULL )
               {
                   ok = FALSE;
                   break;
               }

               DisplayList[task].used = TRUE;
               break;
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL Parameters( int argc, char *argv[] )
{
BOOL ok;
int i,d;
char *data;

    for( ok=TRUE,i=1; ((i < argc) && ok); i++ )
    {
        switch( CMDARG_code(argv[i],&data) )
        {
            case 'W' :
               ExitProgramWait = TRUE;
               break;

            case '?' :
               Usage();
               ExitProgram();
               break;

            case 'P' :
               ok = CMDARG_data(RobotName,data,STRLEN);
               break;

            case 'S' :
               FieldType = FIELD_SPRING;
               FieldConstant[0] = -1.0;
               ok = CMDARG_data(FieldConstant[0],data,-200.0,2.0);
               break;

            case 'V' :
               FieldType = FIELD_VISCOUS;
               FieldConstant[0] = -0.1;
               ok = CMDARG_data(FieldConstant[0],data,-2.0,0.2);
               break;

            case 'U' :
               FieldType = FIELD_VS;
               ok = CMDARG_data(FieldConstant,data,2);
               printf("Viscous Spring: Spring=%.2lfN/cm, Damping=%.2lfN/cm/sec\n",FieldConstant[0],FieldConstant[1]);
               break;

            case 'A' :
               FieldType = FIELD_INERTIAL;
               FieldConstant[0] = -0.01;
               ok = CMDARG_data(FieldConstant[0],data,-0.02,0.01);
               break;

            case 'R' :
               if( data == NULL )
               {
                   FieldRotateZ = 90.0;
               }
               else
               {
                   ok = CMDARG_data(FieldRotateZ,data);
               }

               FieldRotate = SPMX_romxZ(D2R(FieldRotateZ)) * I(3);
               break;

            case 'C' :
               FieldType = FIELD_CIRCULAR;
               FieldConstant[0] = -1.0;   // Field strength (k)...
               FieldConstant[1] =  3.5;   // Radius (r)...
               FieldConstant[2] =  1.0;   // Frequency (f)...

               if( !CMDARG_data(FieldConstant,data,3) )
               {
                   ok = FALSE;
               }
               else
               if( (FieldConstant[0] < -20.0) || FieldConstant[0] > 0.0 )
               {
                   ok = FALSE;
               }
               else
               if( (FieldConstant[1] < 2.0) || (FieldConstant[1] > 10.0) )
               {
                   ok = FALSE;
               }
               else
               if( FieldConstant[2] > 4.0 )
               {
                   ok = FALSE;
               }
               break;

            case 'H' :
               FieldType = FIELD_HOLE;
               FieldConstant[0] = -1.0;   // Field strength (k)...
               FieldConstant[1] =  3.5;   // Radius (r)...

               if( !CMDARG_data(FieldConstant,data,2) )
               {
                   ok = FALSE;
               }
               else
               if( (FieldConstant[0] < -100.0) || FieldConstant[0] > 0.0 )
               {
                   ok = FALSE;
               }
               else
               if( FieldConstant[1] < 1.0 )
               {
                   ok = FALSE;
               }
               break;

            case 'X' :
               FieldType = FIELD_FIXED;
               ok = CMDARG_data(FieldFixed,data,-5.0,5.0,3);
               break;

            case 'M' :
               FieldType = FIELD_TORQUE_MU;
               ok = CMDARG_data(FieldFixed,data,0,500,3);
               break;

            case 'Q' :
               FieldType = FIELD_TORQUE_Nm;
               ok = CMDARG_data(FieldFixed,data,0.0,1.0,3);
               break;

            case 'B' :
               FieldType = FIELD_CALIBRATE;
               FieldConstant[0] = 36.0;     // Number of directions.
               FieldConstant[1] = 5.0;      // Force (Newtons).
               FieldConstant[2] = 1.0;      // Time for measurement (seconds).
               FieldConstant[3] = 1.0;      // Number of force increments.

               if( !(ok=CMDARG_data(FieldConstant,data,4)) )
               {
                   break;
               }

               FieldDirections = (int)FieldConstant[0];
               FieldScales = (int)FieldConstant[3];
               FieldScale = 1.0 / FieldConstant[3];
               FieldCount = FieldDirections * FieldScales;

               FieldData.dim(FieldCount,2);

               for( d=0,i=1; (i <= FieldCount); i++ )
               {
                   FieldData(i,1) = R2D(2.0 * PI * ((double)d / (double)FieldDirections));
                   FieldData(i,2) = FieldScale;

                   if( ++d == FieldDirections )
                   {
                       d = 0;
                       FieldScale += 1.0 / FieldConstant[3];
                   }
               }

               FieldData = permute(FieldData);

               printf("CALIBRATION: %d points, %d directions, %.1lf Newtons (%d levels).\n",FieldCount,FieldDirections,FieldConstant[1],FieldScales);
               disp(FieldData);
               break;
	
            case 'D' :
               if( data == NULL )
               {
                   ok = FALSE;
                   break;
               }

               ok = Display(_strupr(data));
               break;

            case 'E' :
               EncoderReset = TRUE;
               break;

            case 'F' :
               ok = CMDARG_data(ForceMax,data);
               break;

            case 'L' :
               if( (ok=CMDARG_data(FileName,data,STRLEN)) )
               {
                   FileSave = TRUE;
               }
               break;

            case 'Z' :
               if( (ok=CMDARG_data(FileName,data,STRLEN)) )
               {
                   TrialFlag = TRUE;
               }
               break;

            case 'T' :
               LatencyFlag = TRUE;
               break;

            case 'G' :
               if( (ok=CMDARG_data(ControllerTestChannel,data,0,2)) )
               {
                   ControllerTestFlag = TRUE;
               }
               break;

            default :
               ok = FALSE;
               break;
        }

        if( !ok )
        {
            printf("\nInvalid parameter: %s\n\n",argv[i]);
        }
    }

    if( STR_null(RobotName,STRLEN) )
    {
        printf("Must specify which robot.\n");
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

void    DisplayStart( void )
{
int     task;

    for( task=0; (DisplayList[task].func != NULL); task++ )
    {
        if( DisplayList[task].used )
        {
            printf("LOOPTASK_start(%s)\n",DisplayList[task].text);
            LOOPTASK_start(DisplayList[task].func,DisplayList[task].freq);
        }
    }
}

/******************************************************************************/

void DisplayStop( void )
{
int task;

    for( task=0; (DisplayList[task].func != NULL); task++ )
    {
        LOOPTASK_stop(DisplayList[task].func);
    }
}

/******************************************************************************/

void SpringClose( void )
{
    if( ControllerTestFlag )
    {
        ControllerTestRamp.Stop();
        ControllerTestRamp.Close();
    }

    ROBOT_Stop();
    SensorClose();
    ROBOT_Close();
    printf("RobotDistanceStepTotal = %.0lf cm.\n",RobotDistanceStepTotal);

    RobotControlLoopFrequencyTimer.Results();
    RobotNonControlLoopFrequencyTimer.Results();
}

/******************************************************************************/

void main( int argc, char *argv[] )
{
BOOL Exit;
BYTE code;
BOOL ok;

	LOOPTASK_FREQUENCY(22.0);

    printf("%s V%s - %s.\n",MODULE_NAME,MODULE_VERS,MODULE_TEXT);

    FieldRotate = I(3);

    if( !Parameters(argc,argv) )
    {
        Usage();
        ExitProgram();
    }

    printf("%s ROBOT\n",RobotName);
    printf("%s.\n",STR_TextCode(FieldText,FieldType));

    if( (FieldConstant[0] > 0.0) || (FieldRotateZ != 0.0) || FieldUnstable[FieldType] )
    {
        printf("-------------------------------------------\n");
        printf("WARNING! This force field will be unstable.\n");
        printf("Press Y to continue, any other key to exit.\n");
        printf("(Forces will be limited to %c %.1lf N)\n",PLUSMINUS,ForceUnstable);
        printf("-------------------------------------------\n");

        while( !KB_key(code,KB_UPPER) )
        {
            if( TIMER_EveryHz(4.0) )
            {
		Beep(500,100);
            }
        }

        if( code != 'Y' )
        {
            ExitProgram();
        }

        // Limit forces for unstable fields...
        if( ForceMax > ForceUnstable )
        {
            ForceMax = ForceUnstable;
        }
    }

    if( !LOOPTASK_API_start(printf,printf,DebugLoopTask ? printf : NULL) )
    {
        printf("Cannot start LOOPTASK API.\n");
        ExitProgram();
    }

    if( !SENSORAY_API_start(printf,printf,DebugSensoray ? printf : NULL) )
    {
        printf("Cannot start SENSORAY API.\n");
        ExitProgram();
    }

    if( !ROBOT_API_start(printf,printf,DebugRobot ? printf : NULL) )
    {
        printf("Cannot start ROBOT API.\n");
        ExitProgram();
    }

    printf("-----------------------API started \n");


    if( TrialFlag )
    {
        TrialInitialize();
    }

#ifdef OPTO
    if( !OPTO_Start(OPTO_CONFIG_FILE) )
    {
        printf("Cannot start OptoTrak.\n");
        ExitProgram();
    }

    printf("OptoTrak started.\n");
#endif

    printf("-----------------------before robot open \n");

    // Open the ROBOT...
    if( ROBOT_Open(RobotName) == ROBOT_INVALID )
    {
        printf("Cannot open %s ROBOT.\n",RobotName);
        ExitProgram();
    }

    printf("-----------------------after robot open \n");

    printf("ROBOT_Open(%s) OK.\n",RobotName);

    ok = SensorOpen();
    printf("SensorOpen() %s.\n",STR_OkFailed(ok));

    printf("ESCape to exit, any other key to conitnue...\n");
    if( KB_ESC(KB_WAIT) )
    {
        SpringClose();
        ExitProgram();
    }

    // Reset encoders before we start? (V2.1)
    if( ControllerTestFlag || EncoderReset )
    {
        ok = ROBOT_EncoderReset();
        printf("ROBOT_EncoderReset() %s.\n",STR_OkFailed(ok));
    }

    FieldTimer.Reset();

    // Start the ROBOT running...
    if( ControllerTestFlag )
    {
        if( (ok=ROBOT_Start(ForcesFunctionControllerTest)) )
        {
            ok = ControllerTestRamp.Start(ControllerTestRampTime);
        }
    }
    else
    if( (FieldType == FIELD_TORQUE_MU) || (FieldType == FIELD_TORQUE_Nm) )
    {
        ok = ROBOT_Start(ForcesFunctionTorques);
    }
    else
    {
        //ok = ROBOT_Start(ForcesFunctionSpring,RobotNonControlLoop);
        ok = ROBOT_Start(ForcesFunctionSpring);
    }

    if( !ok )
    {
        printf("Cannot start %s ROBOT.\n",RobotName);
        SpringClose();
        ExitProgram();
    }

    printf("ROBOT_Start(...) OK.\n");

    // Start LOOPTASK display functions...
    DisplayStart();

    printf("E=Encoder Zero, Z=Spring Zero, SPACE=Data Zero, ESCape=Exit...\n");

    if( (FieldType == FIELD_TORQUE_MU) || (FieldType == FIELD_TORQUE_Nm) || (FieldType == FIELD_FIXED) )
    {
        printf("Fixed Field: +=Increase, -=Decrease.\n");
    }

    if( LatencyFlag )
    {
        ROBOT_Robot()->Controller->LatencyStart();
    }

    Exit = FALSE;
    while( !Exit )
    {
#ifdef OPTO
        OPTOTRAK_Read();
#endif
        if( !ROBOT_Safe() )
        {
            printf("ROBOT_Safe() failed.\n");
            Exit = TRUE;
            continue;
        }

        if( ProgramExitFlag )
        {
            Exit = TRUE;
            continue;
        }

        if( ControllerTestFlag )
        {
            if( TIMER_EveryHz(1.0/ControllerTestCPSTime) )
            {
                printf("Encoder[%d] %5.0lf kCount/sec\n",ControllerTestChannel,ControllerTestCPS/1000.0);
            }
        }

        if( !KB_key(code,KB_UPPER) )
        {
            continue;
        }

        switch( code )
        {
            case 'E' :
               EncoderZero = TRUE;
               break;

            case 'Z' :
               FieldZero = TRUE;
               break;

            case SPACE :
               DataPoint = 0;
               if( TrialFlag )
               {
                   ok = TrialSave();
               }
               break;

            case 'U' :
            case '+' :
               FieldFixedGain += 1.0;
               break;

            case 'D' :
            case '-' :
               FieldFixedGain -= 1.0;
               break;

            case 'B' :
               printf("Sensor bias reset...\n");
               SensorBiasReset();
               break;

            default :
               printf("Keyboard exit.\n");
               Exit = TRUE;
               break;
        }
    }

    printf("ProgramExitExit=%s Exit=%s.\n",STR_YesNo(ProgramExitFlag),STR_YesNo(Exit));

    if( TrialFlag )
    {
        TrialClose();
    }

    if( LatencyFlag )
    {
        ROBOT_Robot()->Controller->LatencyStop();
        ROBOT_Robot()->Controller->LatencyResults();
    }

    // Stop LOOPTASK display functions...
    DisplayStop();

    // Close and Stop ROBOT...
    SpringClose();

#ifdef OPTO
    OPTO_Stop();
#endif

    // Write calibration information...
    if( FieldType == FIELD_CALIBRATE )
    {
        ok = matrix_write(STR_stringf("%s.DAT",RobotName),Fcal);
    }

    // Write data...
    if( FileSave && (DataPoint > 0) )
    {
        printf("Writing data...\n");
        ok = matrix_write(FileName,Data,DataPoint);
        printf("%s %s\n",FileName,STR_OkFailed(ok));
    }

    ExitProgram();
}

/******************************************************************************/

