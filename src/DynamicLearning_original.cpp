/******************************************************************************/
/*                                                                            */
/* MODULE  : DynamicLearing.cpp                                               */
/*                                                                            */
/* PURPOSE : Dynamic learning paradigm example program.                       */
/*                                                                            */
/* DATE    : 07/Sep/2006                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 07/Sep/2006 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 15/Nov/2007 - Added visual-motor rotations.                      */
/*                                                                            */
/* V1.2  JNI 01/Dec/2008 - Added vertical retrace sync and photo-transistor.  */
/*                                                                            */
/* V1.3  JNI 18/Dec/2008 - Added meta-config files.                           */
/*                                                                            */
/* V1.4  JNI 03/Mar/2010 - Added OptoTrak Rigid Body example.                 */
/*                                                                            */
/* V1.5  JNI 09/Dec/2010 - Changes for subject wellbeing data.                */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME "DynamicLearning"

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

#define CONFIG_FILES 256
int ConfigFiles=0;
int ConfigIndex;
STRING ConfigFile[CONFIG_FILES];
STRING ConfigSaveFile;
STRING MetaConfigFile="";

STRING DataName="";
STRING DataFile="";
STRING TrialListFile="";

STRING RobotName="";
int    RobotID=ROBOT_INVALID;
BOOL   RobotFT=FALSE;
double ForceMax=100.0;
matrix RobotPosition(3,1);
matrix RobotVelocity(3,1);
matrix RobotForces(3,1);
double RobotSpeed;
matrix HandleForces(3,1);
matrix HandleTorques(3,1);
double PhotoTransistor=0.0;

double LoopTaskFrequency;
double LoopTaskPeriod;

matrix TextPosition(3,1);
matrix HomePosition(3,1);
double HomeTolerance=0.50;
double HomeToleranceTime=0.1;
double MovementReactionTimeOut=0.5;
double MovementDurationTimeOut=0.8;
double ErrorWait=2.0;
double TrialDelay=0.0;
double InterTrialDelay=0.5;
int    RestTrials=0;
double RestWait=0.0;
double NotMovingSpeed=0.5; // cm/sec
double NotMovingTime=0.1;
TIMER  NotMovingTimer("NotMoving");

#define TARGETS 32
double  TargetAngles[TARGETS];
int TargetCount;
BOOL TargetTestFlag=FALSE;

matrix CursorPosition(3,1);
int CursorColor=RED;           
double CursorRadius=0.5;

matrix ForceFieldForces(3,1);
BOOL   ForceFieldStarted=FALSE;
matrix ForceFieldPosition(3,1);
RAMPER ForceFieldRamp;
double ForceFieldRampTime=0.1;

BOOL VisualFeedback;

int HomeColor=WHITE;
int NotHomeColor=GREY;
double HomeRadius=1.0;

int TargetColor=YELLOW;
double TargetRadius=1.0;

TIMER_Interval  RobotForcesFunctionLatency("ForcesFunction");
double ForcesFunctionLatency;
TIMER_Frequency RobotForcesFunctionFrequency("ForcesFunction");
double ForcesFunctionPeriod;

TIMER_Frequency GraphicsDisplayFrequency("DisplayFrequency");
TIMER_Frequency GraphicsIdleFrequency("IdleFrequency");
TIMER_Interval  GraphicsDisplayLatency("DisplayLatency");
TIMER_Interval  GraphicsSwapBufferLatency("SwapBufferLatency");
TIMER_Interval  GraphicsClearStereoLatency("ClearStereoLatency");
TIMER_Interval  GraphicsClearMonoLatency("ClearMonoLatency");

double GraphicsVerticalRetraceSyncTime=0.0;    // Time (sec) before vertical retrace to draw graphics frame
double GraphicsVerticalRetraceCatchTime=0.05;  // Time (msec) to devote to catching vertical retrace
TIMER  GraphicsTargetTimer("GraphicsTarget");
matrix GraphicsSyncPosition(3,1);
double GraphicsSyncRadius=0.5;
int    GraphicsSyncColor=WHITE;

BOOL   TriggerFlag=FALSE;

//int    GraphicsMode=GRAPHICS_DISPLAY_2D;
int    GraphicsMode=GRAPHICS_DISPLAY_DEFAULT;

STRING GraphicsModeString="";

STRING GraphicsString="";

// List of WAV files for beeping.
struct WAVELIST WaveList[] = 
{
    { "HighBip","HIGH_BIP.WAV",NULL },
    { "LowBip","LOW_BIP.WAV",NULL },
    { "Bip","MID_BIP.WAV",NULL },
    { "","",NULL },
};

TIMER_Interval WaveListPlayInterval("WaveListPlay");

#define FRAMEDATA_ROWS 10000
MATDAT FrameData("FrameData");
BOOL   FrameRecord=FALSE;

TIMER  MovementDurationTimer("MovementDuration");
TIMER  MovementReactionTimer("MovementReaction");
TIMER  MovementFinishedTimer("MovementFinished");

TIMER  TrialTimer("Trial");
TIMER  InterTrialDelayTimer("InterTrialDelay");
double TrialTime;
double TrialDuration=0.0;
int    Trial;
BOOL   TrialRunning=FALSE;

// Trial types.
#define TRIAL_NULL      0
#define TRIAL_EXPOSURE  1
#define TRIAL_CATCH     2

// Field types.
#define FIELD_NONE      0
#define FIELD_VISCOUS   1
#define FIELD_VISMOTROT 2
#define FIELD_CHANNEL   3

int    Trials=0;
int    TotalTrials;
int    TotalNullTrials=0;
int    TotalExposureTrials=0;
int    TotalCatchTrials=0;

int    ExposureTrials[2]={ 0,10000 };
int    ExposureFieldType=FIELD_NONE;
double ExposureFieldConstant=0.0;
double ExposureFieldAngle=0.0;

int    CatchTrials[3]={ 0,0,0 };
double CatchTargetAngles[TARGETS];
int    CatchTargetCount;
int    CatchFieldType=FIELD_NONE;
double CatchFieldConstant=0.0;
double CatchFieldAngle=0.0;

// Permute list objects to randomize targets.
PERMUTELIST TargetPermute; 
PERMUTELIST CatchTargetPermute;

int    NullTrial=0;
int    ExposureTrial=0;
int    CatchTrial=0;

MATDAT TrialData("TrialData");

// Trial data variables.
int    TrialType;
int    FieldType;
double FieldConstant;
double FieldAngle;
matrix TargetPosition(3,1);
int    TargetIndex;
double TargetAngle;
double TargetDistance;
int    MissTrials=0;
int    MissTrialsTotal=0;
double MissTrialsPercent=0.0;
double MovementReactionTime=0.0;
double MovementDurationTime=0.0;
matrix StartPosition(3,1);
matrix FinishPosition(3,1);

double PositionDelaySeconds=0.0;
int    PositionDelayItems;
matrix PositionDelayList;

int    RobotFieldType;
double RobotFieldConstant;
double RobotFieldAngle;
matrix RobotFieldMatrix;

TIMER  ExperimentTimer("Experiment");
double ExperimentTime;

//#include "OptoTrak.cpp"

/******************************************************************************/

#define STATE_INITIALIZE   0
#define STATE_SETUP        1
#define STATE_HOME         2
#define STATE_START        3
#define STATE_DELAY        4
#define STATE_GO           5
#define STATE_TARGETWAIT   6
#define STATE_MOVEWAIT     7
#define STATE_MOVING       8
#define STATE_FINISH       9
#define STATE_INTERTRIAL  10
#define STATE_EXIT        11
#define STATE_TIMEOUT     12
#define STATE_ERROR       13
#define STATE_REST        14
#define STATE_MAX         15

int State=STATE_INITIALIZE;
int StateLast;
int StateGraphics=STATE_INITIALIZE;
int StateGraphicsLast;
char *StateText[] = { "Initialize","Setup","Home","Start","Delay","Go","TargetWait","MoveWait","Moving","Finish","InterTrial","Exit","TimeOut","Error","Rest" };
BOOL StateLoopTask[STATE_MAX] = { FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE };
TIMER StateTimer("State");
TIMER StateGraphicsTimer("StateGraphics");
int StateErrorResume;

/******************************************************************************/

void ProgramExit( void );

/******************************************************************************/

void WaveListPlay( char *name )
{
    // Play WAV file (does interval timing).
    WaveListPlayInterval.Before();
    WAVELIST_Play(WaveList,name);
    WaveListPlayInterval.After();
}

/******************************************************************************/

void TriggerOn( void )
{
    TriggerFlag = TRUE;
}

/******************************************************************************/

void TriggerOff( void )
{
    TriggerFlag = FALSE;
}

/******************************************************************************/

void ConfigSetup( void )
{
    // Reset configuration variable list.
    CONFIG_reset();

    // Set up variable list for configuration.
    CONFIG_set(VAR(RobotName));
    CONFIG_setBOOL(VAR(RobotFT));
    CONFIG_set(VAR(ForceMax));
    CONFIG_set("GraphicsMode",GraphicsModeString);
    CONFIG_set("GraphicsSyncTime",GraphicsVerticalRetraceSyncTime);
    CONFIG_set("GraphicsCatchTime",GraphicsVerticalRetraceCatchTime);
    CONFIG_set("GraphicsSyncPos",GraphicsSyncPosition);
    CONFIG_set(VAR(TextPosition));
    CONFIG_set(VAR(CursorRadius));
    CONFIG_set(VAR(TargetDistance));
    CONFIG_set(VAR(TargetAngles),TARGETS);
    CONFIG_set(VAR(TargetRadius));
    CONFIG_set(VAR(HomeRadius));
    CONFIG_set(VAR(HomePosition));
    CONFIG_set(VAR(HomeTolerance));
    CONFIG_set(VAR(HomeToleranceTime));
    CONFIG_set(VAR(MovementReactionTimeOut));
    CONFIG_set(VAR(MovementDurationTimeOut));
    CONFIG_set("PositionDelay",PositionDelaySeconds);
    CONFIG_set(VAR(ErrorWait));
    CONFIG_set(VAR(TrialDelay));
    CONFIG_set(VAR(InterTrialDelay));
    CONFIG_set(VAR(RestTrials));
    CONFIG_set(VAR(RestWait));
    CONFIG_set(VAR(NotMovingSpeed));
    CONFIG_set(VAR(NotMovingTime));
    //CONFIG_set(VAR(OptoTrakMarkers));
    //CONFIG_set(VAR(OptoTrakConfig));
    CONFIG_set(VAR(ForceFieldRampTime));

    CONFIG_set(VAR(Trials));

    CONFIG_label(VAR(ExposureTrials),2);
    CONFIG_set("FieldType",ExposureFieldType);
    CONFIG_set("FieldConstant",ExposureFieldConstant);
    CONFIG_set("FieldAngle",ExposureFieldAngle);

    CONFIG_label(VAR(CatchTrials),3);
    CONFIG_set("TargetAngles",CatchTargetAngles,TARGETS);
    CONFIG_set("FieldType",CatchFieldType);
    CONFIG_set("FieldConstant",CatchFieldConstant);
    CONFIG_set("FieldAngle",CatchFieldAngle);
}

/******************************************************************************/

void ConfigInit( void )
{
int i;

    for( i=0; (i < TARGETS); i++ )
    {
        TargetAngles[i] = 360.0;
        CatchTargetAngles[i] = 360.0;
    }

    TargetCount = 0;
    CatchTargetCount = 0;
}

/******************************************************************************/

BOOL ConfigLoad( char *file )
{
int i;
BOOL ok=TRUE;

    // Setup and initialize configuration variables.
    ConfigSetup();
    ConfigInit();

    // Load configuration file.
    if( !CONFIG_read(file) )
    {
        printf("ConfigLoad(%s) Cannot read file.\n",file);
        return(FALSE);
    }

    // Set graphics mode (if specified).
    if( !STR_null(GraphicsModeString) )
    {
        if( !GRAPHICS_DisplayMode(GraphicsMode,GraphicsModeString) )
        {
            ok = FALSE;
        }
    }

    // Count the number of targets.
    for( TargetCount=0; ((TargetAngles[TargetCount] < 360.0) && (TargetCount < TARGETS)); TargetCount++ );

    if( TargetCount == 0 )
    {
        printf("%s: No targets specified.\n",file);
        ok = FALSE;
    }

    // Count the number of catch trial targets (if specified).
    for( CatchTargetCount=0; ((CatchTargetAngles[CatchTargetCount] < 360.0) && (CatchTargetCount < TARGETS)); CatchTargetCount++ );

    if( STR_null(RobotName) )
    {
        printf("No robot specified.\n");
        ok = FALSE;
    }

    printf("ConfigLoad(%s) Load %s.\n",file,STR_OkFailed(ok));
    CONFIG_list(printf);

    printf("TargetCount=%d\n",TargetCount);
    for( i=0; (i < TargetCount); i++ )
    {
        printf("%d: %.1lf (deg)\n",i,TargetAngles[i]);
    }

    printf("TargetCount=%d\n",TargetCount);
    for( i=0; (i < TargetCount); i++ )
    {
        printf("%d: %.1lf (deg)\n",i,TargetAngles[i]);
    }

    printf("CatchTargetCount=%d\n",CatchTargetCount);
    for( i=0; (i < CatchTargetCount); i++ )
    {
        printf("%d: %.1lf (deg)\n",i,CatchTargetAngles[i]);
    }

    return(ok);
}

/******************************************************************************/

BOOL ConfigSave( char *file )
{
BOOL ok;

    ConfigSetup();

    // Save configuration file.
    ok = CONFIG_write(file);

    printf("ConfigSave(%s) %s.\n",file,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL ConfigSaveFiles( void )
{
int i;
BOOL ok=TRUE;

    for( ok=TRUE,i=0; ((i < ConfigFiles) && ok); i++ )
    {
        if( ConfigFiles == 1 )
        {
            strncpy(ConfigSaveFile,STR_stringf("%s.CFG",DataName),STRLEN);
        }
        else
        {
            strncpy(ConfigSaveFile,STR_stringf("%s[%d].CFG",DataName,i),STRLEN);
        }
      
        ok = ConfigSave(ConfigSaveFile);
    }

    return(ok);
}

/******************************************************************************/

void GraphicsDisplayText( char *string, float size, matrix &pos )
{
static matrix p;
void *font=GLUT_STROKE_MONO_ROMAN;
int i,w;
float s=size*0.015;

    p = pos;
    w = strlen(string);

    p(1,1) -= (w / 2) * (s * 100.0);
  
    glPushMatrix();

    glLineWidth(2.0);
    translate(p);
    glScalef(s,s,1);

    GRAPHICS_ColorSet(WHITE);

    for( i=0; (i < w); i++ )
        glutStrokeCharacter(font,string[i]);

    glPopMatrix();
}

/******************************************************************************/

void GraphicsDisplayText( void )
{
static matrix P(3,1);

    if( !STR_null(GraphicsString) )
    {
        P = TextPosition;
        P(2,1) -= 3.0;
        GraphicsDisplayText(GraphicsString,1.0,P);
    }
}

/******************************************************************************/

void GraphicsText( char *text )
{
    if( text != NULL )
    {
        strncpy(GraphicsString,text,STRLEN);
    }
}

/******************************************************************************/

void FrameProcess( void )
{
    // Is frame recording in progres.
    if( !FrameRecord )
    {
        return;
    }

    // Load GRAPHICS-related frame data variables.
    GRAPHICS_FrameData();

    // Save current variables for FrameData.
    FrameData.RowSave();
}

/******************************************************************************/

void FrameStart( void )
{
    // Start recording frame data.
    FrameData.Reset();
    FrameRecord = TRUE;
}

/******************************************************************************/

void FrameStop( void )
{
    // Stop recording frame data.
    FrameRecord = FALSE;
}

/******************************************************************************/

void StateProcessLoopTask( void );

/******************************************************************************/

void ForceFieldStart( void )
{
    ForceFieldPosition = RobotPosition;
    ForceFieldStarted = TRUE;
    ForceFieldRamp.Up();
}
        
/******************************************************************************/

void ForceFieldStop( void )
{
    ForceFieldRamp.Down();
    ForceFieldStarted = FALSE;
}

/******************************************************************************/

void RobotForcesFunction( matrix &position, matrix &velocity, matrix &forces )
{
int i,j;
static matrix P,R,_R;

    // Monitor timing of Forces Function (values saved to FrameData).
    ForcesFunctionPeriod = RobotForcesFunctionFrequency.Loop();
    RobotForcesFunctionLatency.Before();

    TrialTime = TrialTimer.ElapsedSeconds();

    // Kinematic data passed from robot API.
    RobotPosition = position;
    RobotVelocity = velocity;
    RobotSpeed = norm(RobotVelocity);

    // Zero forces.
    ForceFieldForces.zeros();
    RobotForces.zeros();

    // Read raw sensor values from Sensoray card.
    ROBOT_SensorRead(RobotID);

    // Get Force/Torque sensor if required.
    if( RobotFT && ROBOT_SensorOpened_DAQFT(RobotID) )
    {
        ROBOT_Sensor_DAQFT(RobotID,HandleForces,HandleTorques);
    }

    // Get PhotoTransistor if required.
    if( ROBOT_SensorOpened_PhotoTransistor(RobotID) )
    {
        ROBOT_Sensor_PhotoTransistor(RobotID,PhotoTransistor);
    }

    // Process position delay window, if required.
    P = RobotPosition;
    if( PositionDelaySeconds > 0.0 )
    {
        if( PositionDelayList.isempty() )
        {
            PositionDelayItems = 1 + (int)(PositionDelaySeconds / milliseconds2seconds(LOOPTASK_Period));
            PositionDelayList.dim(3,PositionDelayItems);
        }

        for( i=1; (i < PositionDelayItems); i++ )
        {
            for( j=1; (j <= 3); j++ )
            {
                PositionDelayList(j,i) = PositionDelayList(j,i+1);
            }
        }

        i = PositionDelayItems;
        for( j=1; (j <= 3); j++ )
        {
            PositionDelayList(j,i) = RobotPosition(j,1);
        }

        P = PositionDelayList[1];
    }
  
    CursorPosition = P;

    // Process force-field type.
    switch( ForceFieldStarted ? RobotFieldType : FIELD_NONE )
    {
        case FIELD_NONE :
           break;

        case FIELD_VISCOUS :   // Viscous force field.
           ForceFieldForces = RobotFieldConstant * RobotFieldMatrix * RobotVelocity;
           break;

        case FIELD_VISMOTROT : // Visual motor rotation.
           CursorPosition = HomePosition + (RobotFieldMatrix * (P-HomePosition));
           break;

        case FIELD_CHANNEL :
           // Next rotate position to vertical (angle = 0).
           SPMX_romxZ(D2R(TargetAngle),R);
           SPMX_romxZ(D2R(-TargetAngle),_R);

           P = R * (RobotPosition - ForceFieldPosition);

           // Calculate perpendicular (X) channel forces.
           ForceFieldForces(1,1) = RobotFieldConstant * P(1,1);

           // Rotate back to original.
           ForceFieldForces = _R * ForceFieldForces;
           break;
    }

    // Process Finite State Machine.
    StateProcessLoopTask();

    // Monitor timing of Forces Function (values saved to FrameData).
    ForcesFunctionLatency = RobotForcesFunctionLatency.After();

    //OptoTrakRead();

    RobotForces = ForceFieldRamp.RampCurrent() * ForceFieldForces;

    // Save frame data.
    FrameProcess();

    // Set forces to pass to robot API and clamp for safety.
    forces = RobotForces;
    forces.clampnorm(ForceMax);
}

/******************************************************************************/

void DeviceStop( void )
{
    // Stop and close robot.
    ROBOT_Stop(RobotID);
    ROBOT_SensorClose(RobotID);
    ROBOT_Close(RobotID);

    ForceFieldRamp.Stop();
    ForceFieldRamp.Close();

    //OptoTrakStop();

    RobotID = ROBOT_INVALID;
}

/******************************************************************************/

BOOL DeviceStart( void )
{
BOOL ok=TRUE;

/*
    if( OptoTrakMarkers > 0 )
    {
        ok = OptoTrakStart();
        printf("OptoTrak start: %s.\n",STR_OkFailed(ok));
    }
	*/

    if( !ok )
    {
        return(FALSE);
    }

    // Open and start robot.
    if( (RobotID=ROBOT_Open(RobotName)) == ROBOT_INVALID )
    {
        printf("%s: Open failed.\n",RobotName);
        ok = FALSE;
    }
    else
    if( !ROBOT_Start(RobotID,RobotForcesFunction) )
    {
        printf("%s: Start failed.\n",RobotName);
        ok = FALSE;
    }
    else
    if( !ROBOT_SensorOpen(RobotID) )
    {
        printf("%s: Cannot open sensor(s).\n",RobotName);
        ok = FALSE;
    }
    else
    if( RobotFT )
    {
        if( !ROBOT_SensorOpened_DAQFT() )
        {
            printf("%s: F/T sensor not opened.\n",RobotName);
            RobotFT = FALSE;
        }
    }

    if( ok )
    {
        if( !ForceFieldRamp.Open(ForceFieldRampTime) )
        {
            printf("ForceFieldRamp: Open failed.\n");
            ok = FALSE;
        }
        else
        if( !ForceFieldRamp.Start() )
        {
            printf("ForceFieldRamp: Start failed.\n");
            ok = FALSE;
        }
        else
        {
            ForceFieldRamp.Down();
        }
    }

    if( !ok )
    {
        DeviceStop();
        return(FALSE);
    }

    printf("%s: Started.\n",RobotName);

    // Reset bias of F/T sensor if required.
    if( RobotFT )
    {
        printf("Press any key to reset bias of F/T sensor(s)...\n");
        while( !KB_anykey() );

        ok = ROBOT_SensorBiasReset_DAQFT(RobotID);
        printf("%s: F/T sensor bias reset: %s.\n",RobotName,STR_OkFailed(ok));
    }

    LoopTaskFrequency = ROBOT_LoopTaskGetFrequency(RobotID);
    LoopTaskPeriod = ROBOT_LoopTaskGetPeriod(RobotID);

    return(ok);
}

/******************************************************************************/

BOOL RobotActive( void )
{
BOOL flag=FALSE;

    if( ROBOT_Activated(RobotID) && ROBOT_Ramped(RobotID) )
	{
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

double RobotDistance( matrix &home )
{
double distance;

    distance = norm(RobotPosition-home);

    return(distance);
}

/******************************************************************************/

BOOL RobotHome( matrix &home, double tolerance )
{
BOOL flag=FALSE;

    if( RobotDistance(home) <= tolerance )
    {
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

BOOL RobotHome( void )
{
BOOL flag;

    flag = RobotHome(HomePosition,HomeTolerance);

    return(flag);
}

/******************************************************************************/

BOOL MovementStarted( void )
{
BOOL flag;

    flag = !RobotHome(StartPosition,HomeTolerance);

    return(flag);
}

/******************************************************************************/

BOOL MovementFinished( void )
{
BOOL flag=FALSE;

    // Is robot in the finish position?
    if( !RobotHome(FinishPosition,HomeTolerance) )
    {
        MovementFinishedTimer.Reset();
    }

    // Has the robot been in the finish position for the required amount of time?
    if( MovementFinishedTimer.ExpiredSeconds(HomeToleranceTime) )
    {
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

BOOL RobotNotMoving( void )
{
BOOL flag;

    if( NotMovingSpeed == 0.0 )
    {
        return(TRUE);
    }

    if( RobotSpeed > NotMovingSpeed )
    {
        NotMovingTimer.Reset();
    }

    flag = NotMovingTimer.ExpiredSeconds(NotMovingTime);

    return(flag);
}

/******************************************************************************/

void StateNext( int state )
{
    if( State == state )
    {
        return;
    }

    printf("STATE: %s[%d] > %s[%d] (%.0lf msec).\n",StateText[State],State,StateText[state],state,StateTimer.Elapsed());
    StateTimer.Reset();
    StateLast = State;
    State = state;
}

/******************************************************************************/

void StateGraphicsNext( int state )
{
    if( StateGraphics == state )
    {
        return;
    }

    StateGraphicsTimer.Reset();
    StateGraphicsLast = StateGraphics;
    StateGraphics = state;

    // The visual target first appears in this state, so set graphics sync timer.
    if( StateGraphics == STATE_GO )
    {
        // Set graphics sync timer relative to offset of next vertical retrace.
        GraphicsTargetTimer.Reset(-GRAPHICS_VerticalRetraceOffsetTimeUntilNext());
    }
}

/******************************************************************************/

void TrialSetup( void )
{
    // Load trial variables from TrialData.
    TrialData.RowLoad(Trial);

    // Set robot force field variables.
    RobotFieldType = FieldType;
    RobotFieldConstant = FieldConstant;
    RobotFieldAngle = FieldAngle;
    SPMX_romxZ(D2R(RobotFieldAngle),RobotFieldMatrix);

    TrialRunning = FALSE;
    StateGraphicsNext(State);
    TriggerOff();
}

/******************************************************************************/

void TrialStart( void )
{
    printf("Starting Trial %d...\n",Trial);
    printf("TargetAngle=%.1lf(deg) TrialType=%d FieldType=%d FieldConstant=%.2lf\n",TargetAngle,TrialType,FieldType,FieldConstant);
    TrialTimer.Reset();
    TrialTime = TrialTimer.ElapsedSeconds();
    TrialRunning = TRUE;

    // Start force field.
    ForceFieldStart();

    // Start recording frame data for trial.
    FrameStart();
}

/******************************************************************************/

void TrialStop( void )
{
    TrialRunning = FALSE;
    printf("Stopping Trial %d...\n",Trial);

    // Stop recording frame for trial.
    FrameStop();

    // Stop force field.
    ForceFieldStop();

    TrialDuration = TrialTimer.ElapsedSeconds();
    InterTrialDelayTimer.Reset();

}

/******************************************************************************/

void TrialAbort( void )
{
    TrialRunning = FALSE;
    printf("Aborting Trial %d...\n",Trial);

    // Stop recording frame data for trial.
    FrameStop();

    // Stop force field.
    ForceFieldStop();
}

/******************************************************************************/

BOOL TrialSave( void )
{
BOOL ok=FALSE;

    ExperimentTime = ExperimentTimer.ElapsedSeconds();
    MissTrials = MissTrialsTotal;

    // Put values in the trial data
    TrialData.RowSave(Trial);

    // Set-up the data file on the first trial.
    if( Trial == 1 )
    {
        // Open the file for trial data.
        if( !DATAFILE_Open(DataFile,TrialData,FrameData) )
        {
            printf("DATAFILE: Cannot open file: %s\n",DataFile);
            return(FALSE);
        }
    }

    // Write the trial data to the file.
    printf("Saving trial %d: %d frames of data collected in %.2lf seconds.\n",Trial,FrameData.GetRow(),TrialDuration);
    ok = DATAFILE_TrialSave(Trial);
    printf("%s %s Trial=%d.\n",DataFile,STR_OkFailed(ok),Trial);

    return(ok);
}

/******************************************************************************/

void TrialExit( void )
{
BOOL ok;
int i;
STRING file;

    // Only continue if data file has been opened.
    if( !DATAFILE_Opened() )
    {
        return;
    }

    // Append program source file to data file.
    strncpy(file,STR_stringf("%s.cpp",MODULE_NAME),STRLEN);
    printf("Appending %s to data file...\n",file);
    ok = DATAFILE_AppendFile(file);
    printf("%s\n",STR_OkFailed(ok));

    // Append configuration files to data file.
    for( i=0; (i < ConfigFiles); i++ )
    {
        strncpy(file,ConfigFile[i],STRLEN);
        printf("Appending %s to data file...\n",file);

        ok = DATAFILE_AppendFile(file);
        printf("%s\n",STR_OkFailed(ok));
    }


    // Close data file.
    DATAFILE_Close();

    // Also save configuration files separately using data file name.
    ConfigSaveFiles();
}

/******************************************************************************/

BOOL TrialNext( void )
{
BOOL flag=FALSE;

    if( Trial < Trials )
    {
        Trial++;
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

void BeepGo( void )
{
    WaveListPlay("HighBip");
}

/******************************************************************************/

void BeepError( void )
{
    WaveListPlay("LowBip");
}

/******************************************************************************/

void MessageClear( void )
{
    GraphicsText("");
    GRAPHICS_ClearColor(); // Default background color.
}

/******************************************************************************/

void MessageSet( char *text, int background )
{
    MessageClear();
    
    GraphicsText(text);

    if( background != -1 )
    {
        GRAPHICS_ClearColor(background);
    }
}

/******************************************************************************/

void MessageSet( char *text )
{
int background=-1;

    MessageSet(text,background);
}

/******************************************************************************/

void ErrorFrameDataFull( void )
{
    MessageSet("Frame data full",RED);
    printf("Error: Frame data full\n");
}

/******************************************************************************/

void ErrorMessage( char *str )
{
    MessageSet(str,RED);
    printf("Error: %s\n",str);
}

/******************************************************************************/

void ErrorMoveWaitTimeOut( void )
{
    MessageSet("Too Slow",RED);
    printf("Error: MoveWaitTimeOut\n");
}

/******************************************************************************/

void ErrorMoveTimeOut( void )
{
    MessageSet("Too Slow",RED);
    printf("Error: MoveTimeOut\n");
}

/******************************************************************************/

void ErrorMoveTooSoon( void )
{
    MessageSet("Too Soon",RED);
    printf("Error: MoveTooSoon\n");
}

/******************************************************************************/

void ErrorRobotInactive( void )
{
    MessageSet("Handle switch",RED);
    printf("Error: RobotInactive\n");
}

/******************************************************************************/

void ErrorState( int state )
{
    BeepError();
    StateErrorResume = state;
    StateNext(STATE_ERROR);
}

/******************************************************************************/

void ErrorResume( void )
{
    MessageClear();
    StateNext(StateErrorResume);
}

/******************************************************************************/

void MissTrial( void )
{
    MissTrialsTotal++;
    MissTrialsPercent = 100.0 * ((double)MissTrialsTotal / (double)Trial);
    printf("\nMiss Trials = %d/%d (%.0lf%%)\n\n",MissTrialsTotal,Trial,MissTrialsPercent);

    ErrorState(STATE_SETUP);
}

/******************************************************************************/

void StateProcessLoopTask( void )
{
    // Only some states are processing in the LoopTask.
    if( !StateLoopTask[State] )
    {
        return;
    }

    // State processing.
    switch( State )
    {
        case STATE_MOVEWAIT :
           if( MovementStarted()  )
           {
               MovementDurationTimer.Reset();
               MovementReactionTime = MovementReactionTimer.ElapsedSeconds();
               StateNext(STATE_MOVING);
               break;
           }

           if( MovementReactionTimer.ExpiredSeconds(MovementReactionTimeOut) )
           {
               StateNext(STATE_TIMEOUT);
               break;
           }
           break;

        case STATE_MOVING :
           if( MovementFinished()  )
           {
               MovementDurationTime = MovementDurationTimer.ElapsedSeconds();
               StateNext(STATE_FINISH);
               break;
           }

           if( MovementDurationTimer.ExpiredSeconds(MovementDurationTimeOut) )
           {
               StateNext(STATE_TIMEOUT);
               break;
           }
           break;
    }
}

/******************************************************************************/

void StateProcess( void )
{
double d;

    // Check that robot is in a safe state.
    if( !ROBOT_Safe(ROBOT_ID) )
    {
        printf("Robot not safe.\n");
        ProgramExit();
    }

    // Special processing while a trial is running.
    if( TrialRunning )
    {
        if( !RobotActive() )
        {
            // If robot is not active, abort current trial.
            ErrorRobotInactive();
            TrialAbort();
            MissTrial();
        }
        else
        if( FrameData.Full() )
        {
            // Abort current trail if frame data is full.
            ErrorFrameDataFull();
            TrialAbort();
            MissTrial();
        }
    }

    // Some states are processing in the LoopTask.
    if( StateLoopTask[State] )
    {    
        return;
    }

    // State processing.
    switch( State )
    {
        case STATE_INITIALIZE :
           // Initialization state.
           if( TargetTestFlag )
           {
               break;
           }

           ExperimentTimer.Reset();
           StateNext(STATE_SETUP);
           break;

        case STATE_SETUP :

			
           // Setup details of next trial, but only when robot stationary and active.
			

           if( RobotNotMoving() && RobotActive() )
           {
			   printf("Dynamic learning: before Trialsetup i am here=====!!!!\n");
               TrialSetup();
               StateNext(STATE_HOME);
           }
           break;

        case STATE_HOME :
           // Start trial when robot in home position (and stationary and active).
           if( RobotNotMoving() && RobotHome() && RobotActive() )
           {
               StateNext(STATE_START);
               break;
           }
           break;

        case STATE_START :
           // Start trial.
           TrialStart();
           StateNext(STATE_DELAY);
           break;

        case STATE_DELAY :
           // Delay period before go signal.
           if( StateTimer.ExpiredSeconds(TrialDelay) )
           {
               StateNext(STATE_GO);
               break;
           }

           if( MovementStarted() )
           {
               ErrorMoveTooSoon();
               TrialAbort();
               MissTrial();
           }
           break;

        case STATE_GO :
           // Wait until graphics state matches.
           if( State != StateGraphics )
           {
               break;
           }

           StateNext(STATE_TARGETWAIT);
           break;

        case STATE_TARGETWAIT :
           // Wait for estimated target display delay.
           if( GraphicsTargetTimer.ExpiredSeconds(GRAPHICS_DisplayDelayTarget(TargetPosition)) )
           {
               // Target should be displayed now (within a few milliseconds).
               TriggerOn();
               MovementReactionTimer.Reset();
               BeepGo();
               StateNext(STATE_MOVEWAIT);
           }
           break;

        case STATE_MOVEWAIT :
           // Process in the robot forces function (LoopTask)
           break;

        case STATE_MOVING :
           // Process in the robot forces function (LoopTask)
           break;

        case STATE_FINISH :
           // Trial has finished so stop trial.
           TrialStop();

           // Save the data for this trial.
           if( !TrialSave() )
           {
               printf("Cannot save Trial %d.\n",Trial);
               StateNext(STATE_EXIT);
               break;
           }

           // Catch too-slow trials.
           if( MovementDurationTime >= MovementDurationTimeOut )
           {
               ErrorMoveTimeOut();
               BeepError();
               InterTrialDelayTimer.Reset();
           }

           // Go to next trial, if there is one.
           if( TrialNext() )
           {
               StateNext(STATE_INTERTRIAL);
           }
           else
           {
               StateNext(STATE_EXIT);
           }
           break;

        case STATE_INTERTRIAL :
           // Wait for the intertrial delay to expire.
           if( !InterTrialDelayTimer.ExpiredSeconds(InterTrialDelay) )
           {
               break;
           }

           MessageClear();

           // Optional rest between blocks of trials.
           if( RestTrials != 0 )
           {
               // Rest every X number of trials.
               if( (Trial % RestTrials) == 0 )
               {
                   StateNext(STATE_REST);
                   break;
               }
           }

           StateNext(STATE_SETUP);
           break;

        case STATE_EXIT :
           ProgramExit();
           break;

        case STATE_TIMEOUT :
           switch( StateLast ) // Which state had the timeout?
           {
               case STATE_MOVEWAIT :
                  ErrorMoveWaitTimeOut();
                  break;
 
               case STATE_MOVING :
                  ErrorMoveTimeOut();
                  break;

               default :
                  ErrorMessage(STR_stringf("%s TimeOut",StateText[StateLast]));
                  break;
           }

           TrialAbort(); // Abort the current trial.
           MissTrial();  // Generate miss trial.
           break;

        case STATE_ERROR :
           if( StateTimer.ExpiredSeconds(ErrorWait) )
           {
               ErrorResume();
           }
           break;

        case STATE_REST :
           if( StateTimer.ExpiredSeconds(RestWait) )
           {
               StateNext(STATE_SETUP);
           }
           break;
    }
}

/******************************************************************************/

void GraphicsResults( void )
{
    printf("----------------------------------------------------------------\n");

    printf("Monitor Refresh Rate %.0lf Hz.\n",GRAPHICS_VerticalRetraceFrequency);
    GraphicsDisplayFrequency.Results();
    GraphicsDisplayLatency.Results();
    GraphicsSwapBufferLatency.Results();
    GraphicsClearStereoLatency.Results();
    GraphicsClearMonoLatency.Results();
    GraphicsIdleFrequency.Results();

    if( GraphicsVerticalRetraceSyncTime != 0.0 )
    {
        GRAPHICS_VerticalRetraceResults();
    }

    printf("----------------------------------------------------------------\n");
}

/******************************************************************************/

void Results( void )
{
    // Print results for various timers and things.
    RobotForcesFunctionLatency.Results();
    RobotForcesFunctionFrequency.Results();
    GraphicsResults();
    WaveListPlayInterval.Results();
    //OptoTrakResults();
}

/******************************************************************************/

void ProgramExit( void )
{
    // Do trial data exit stuff.
    TrialExit();

    // Stop, close and other final stuff.
    DeviceStop();
    GRAPHICS_Stop();
    Results();
    WAVELIST_Close(WaveList);

    printf("ExperimentTime = %.0lf minutes.\n",ExperimentTimer.ElapsedMinutes());

    // Exit the program.
    exit(0);
}

/******************************************************************************/

matrix TargetAngleVector( double angle )
{
static matrix vector(3,1);

   // Create a unit vector for target angle.
   vector(1,1) = sin(D2R(angle));
   vector(2,1) = cos(D2R(angle));
   vector(3,1) = 0.0;

   return(vector);
}

/******************************************************************************/

void GraphicsDisplay( void )
{
int attr;
static matrix posn;

    // Mark time before we start drawing the graphics scene.
    GraphicsDisplayLatency.Before();

    // Clear "stereo" graphics buffers.
    GraphicsClearStereoLatency.Before();
    GRAPHICS_ClearStereo();
    GraphicsClearStereoLatency.After();

    // Loop for each eye (stereo 3D).
    GRAPHICS_EyeLoop(eye)
    {
        // Set view for each eye (stereo 3D).
        GRAPHICS_ViewCalib(eye);

        // Clear "mono" graphics buffers.
        GraphicsClearMonoLatency.Before();
        GRAPHICS_ClearMono();
        GraphicsClearMonoLatency.After();

        // Display text.
        GraphicsDisplayText();

        // Display rotating teapot during rest period.
        if( StateGraphics == STATE_REST )
        {
            glPushMatrix();
            GRAPHICS_ColorSet(GREEN);
            glRotated(ExperimentTimer.ElapsedSeconds() * 10.0 * PI,1.0,1.0,1.0);
            glutWireTeapot(10.0);
            glPopMatrix();
            continue;
        }

        // Display home position at start of trial.
        if( (StateGraphics >= STATE_SETUP) && (StateGraphics <= STATE_MOVING) )
        {
            attr = RobotHome() ? HomeColor : NotHomeColor;
            GRAPHICS_Circle(&StartPosition,HomeRadius,attr);
        }

        // Display target spheres when trial running.
        if( (StateGraphics >= STATE_GO) && (StateGraphics <= STATE_FINISH) )
        {
            // Display target for movement.
            GRAPHICS_Circle(&TargetPosition,TargetRadius,TargetColor);

            // Display graphics sync target for phototransistor.
            if( !GraphicsSyncPosition.iszero() )
            {
                GRAPHICS_Circle(&GraphicsSyncPosition,GraphicsSyncRadius,GraphicsSyncColor);
            }
        }

        // Display finish position.
        if( (StateGraphics > STATE_MOVING) && (StateGraphics <= STATE_INTERTRIAL) )
        {
            attr = RobotHome() ? HomeColor : NotHomeColor;
            GRAPHICS_Circle(&FinishPosition,HomeRadius,attr);
        }

        // Display robot position cursor.
        if( (StateGraphics != STATE_ERROR) && (VisualFeedback || RobotHome()) )
        {
            posn = CursorPosition;
            posn(3,1) += 2.0*HomeRadius;
            GRAPHICS_Circle(&posn,CursorRadius,CursorColor);
        }
    }

    // Mark time now that scene has been drawn.
    GraphicsDisplayLatency.After();

    // Display the graphics buffer we've just drawn.
    GraphicsSwapBufferLatency.Before();
    GRAPHICS_SwapBuffers();
    GraphicsSwapBufferLatency.After();

    // Mark time for display frequency.
    GraphicsDisplayFrequency.Loop(); 
}

/******************************************************************************/

void GraphicsDisplayTargetTest( void )
{
int item,attr,target;
static matrix posn;

    // Mark time before we start drawing the graphics scene.
    GraphicsDisplayLatency.Before();

    // Clear "stereo" graphics buffers.
    GraphicsClearStereoLatency.Before();
    GRAPHICS_ClearStereo();
    GraphicsClearStereoLatency.After();

    // Loop for each eye (stereo 3D).
    GRAPHICS_EyeLoop(eye)
    {
        // Set view for each eye (stereo 3D).
        GRAPHICS_ViewCalib(eye);

        // Clear "mono" graphics buffers.
        GraphicsClearMonoLatency.Before();
        GRAPHICS_ClearMono();
        GraphicsClearMonoLatency.After();

        // Display home position...
        posn = HomePosition;
        GRAPHICS_Sphere(&posn,HomeRadius,HomeColor);

        for( target=0; (target < TargetCount); target++ )
        {
            posn = HomePosition + (TargetAngleVector(TargetAngles[target]) * TargetDistance);
            GRAPHICS_Sphere(&posn,TargetRadius,TargetColor);
        }
    }

    // Mark time now that scene has been drawn.
    GraphicsDisplayLatency.After();

    // Display the graphics buffer we've just drawn.
    GraphicsSwapBufferLatency.Before();
    GRAPHICS_SwapBuffers();
    GraphicsSwapBufferLatency.After();

    // Mark time for display frequency.
    GraphicsDisplayFrequency.Loop(); 
}

/******************************************************************************/

void GraphicsIdle( void )
{
BOOL draw=FALSE;

/*
    if( TIMER_EveryHz(4.0) && (OptoTrakMarkers > 0) )
    {
        printf("Frame %d\n",OptoTrakFrame);
        for( int i=0; (i < OptoTrakMarkers); i++ )
        {
            printf("%02d: %.0lf,%.0lf,%.0lf\n",i,OptoTrakPosition[i](1,1),OptoTrakPosition[i](2,1),OptoTrakPosition[i](3,1));
        }
    }
	*/

    GraphicsIdleFrequency.Loop();

    // Process Finite State Machine.
    StateProcess();

	/*
    // If set, graphics frames are timed to occur just before next vertical retrace.
    if( GraphicsVerticalRetraceSyncTime != 0.0 )
    {
        // This function catches the vertical retrace, resetting a timer.
        GRAPHICS_VerticalRetraceCatch(GraphicsVerticalRetraceCatchTime);

        // Set draw flag if next vertical retrace is about to occur.
        if( GRAPHICS_VerticalRetraceOnsetTimeUntilNext() <= seconds2milliseconds(GraphicsVerticalRetraceSyncTime) )
        {
            // But make sure we haven't already drawn a frame in this vertical retrace cycle.
            if( GraphicsDisplayFrequency.ElapsedSeconds() >= (GRAPHICS_VerticalRetracePeriod/2.0) )
            {
                draw = TRUE;
            }
        }
    }
    else
    {
	*/
        draw = TRUE;
    //}

    // Draw graphics frame only if draw flag set.
    if( draw )
    {
        StateGraphicsNext(State); // Safe to set graphics state at this point.
        if( TargetTestFlag )
        {
            GraphicsDisplayTargetTest();
        }
        else
        {
            GraphicsDisplay();
        }
    }

    Sleep(0);
}

/******************************************************************************/

void GraphicsKeyboard( unsigned char key, int x, int y )
{
    // Process keyboard input.
    switch( key )
    {
       case ESC : 
          ProgramExit();
          break;
    }
}

/******************************************************************************/

BOOL GraphicsStart( int argc, char *argv[] )
{
    // Start graphics window...
    if( !GRAPHICS_Start(GraphicsMode) )
    //if( !GRAPHICS_GraphicsStart() )
    {
        printf("Cannot start GRAPHICS system.\n");
        return(FALSE);
    }

    // Set standard openGL parameters.
    GRAPHICS_OpenGL(GRAPHICS_FLAG_NONE,BLACK);

    return(TRUE);
}

/******************************************************************************/

void GraphicsMainLoop( void )
{
    // Set various GLUT call-back functions.
    glutKeyboardFunc(KB_GLUT_KeyboardFuncInstall(GraphicsKeyboard));
    glutDisplayFunc(GraphicsDisplay);
    glutIdleFunc(GraphicsIdle);

    // Reset frequency timing objects.
    GraphicsDisplayFrequency.Reset();
    GraphicsIdleFrequency.Reset();

    // Give control to GLUT's main loop.
    glutMainLoop();
}

/******************************************************************************/

void Usage( void )
{
    printf("----------------------------------\n");
    printf("%s /C:Config(1)[,...Config(n)] /M:MetaConfig /D:DataFile\n",MODULE_NAME);
    printf("----------------------------------\n");

    exit(0);
}

/******************************************************************************/

BOOL Parameters( int argc, char *argv[] )
{
char *data;
BOOL ok;
int i;

    printf("\n");

    // Loop over each command-line parameter.
    for( ok=TRUE,i=1; ((i < argc) && ok); i++ )
    {
        // Process the parameter's code.
        switch( CMDARG_code(argv[i],&data) )
        {
            case 'C' :
               ok = CMDARG_data(ConfigFile,data,CONFIG_FILES);
               break;

            case 'M' :
               ok = CMDARG_data(MetaConfigFile,data);
               break;

            case 'D' :
               if( !CMDARG_data(DataName,data,STRLEN) )
               {
                   ok = FALSE;
               }
               else
               if( strstr(DataName,".") != NULL )
               {
                   ok = FALSE;
               }
               else
               if( STR_null(DataName) )
               {
                   ok = FALSE;
               }
               else
               {
                   strncpy(DataFile,STR_stringf("%s.DAT",DataName),STRLEN);
                   strncpy(ConfigSaveFile,STR_stringf("%s.CFG",DataName),STRLEN);
                   strncpy(TrialListFile,STR_stringf("%s-L.DAT",DataName),STRLEN);
               }
               break;

            case 'T' :
               TargetTestFlag = TRUE;
               break;

            case '?' :
               Usage();
               break;

            default :
               ok = FALSE;
               break;
        }

        if( !ok )
        {
            printf("Invalid argument: %s\n",argv[i]);
        }
    }

    if( !ok )
    {
        return(FALSE);
    }

    // Multiple config files or a meta-config file which specifies multiple files.
    if( !STR_null(MetaConfigFile) )
    {
        if( !METACONFIG_Process(MetaConfigFile,ConfigFiles,ConfigFile) )
        {
            printf("Cannot load meta-configuration file: %s\n",MetaConfigFile);
            ConfigFiles = 0;
        }
    }
    else
    {
        for( ConfigFiles=0; (ConfigFiles < CONFIG_FILES); ConfigFiles++ )
        {
            if( STR_null(ConfigFile[ConfigFiles]) )
            {
                break;
            }
        }
    }

    if( ConfigFiles == 0 )
    {
        printf("Configuration file(s) not specified.\n");
        return(FALSE);
    }

    if( STR_null(DataName) )
    {
        printf("Data file not specified.\n");
        return(FALSE);
    }

    for( i=0; (i < ConfigFiles); i++ )
    {
        printf("%d %s\n",i,ConfigFile[i]);
    }

    return(TRUE);
}

/******************************************************************************/

void Initialize( void )
{
    // Patch High-Performance-Counter bug using LoopTask.
    LOOPTASK_TimerSkipPatch();
    
    // Open list of wave files.
    if( !WAVELIST_Open(WaveList) )
    {
        printf("WAVELIST: Cannot load WAV files.\n");
    }

    // Seed random number generator.
    randomize();

    // Add each variable to the TrialData matrix.
    TrialData.AddVariable("ExperimentTime",ExperimentTime);// Required subject wellbeing variable
    TrialData.AddVariable(VAR(TrialDelay));
    TrialData.AddVariable(VAR(TrialType));
    TrialData.AddVariable(VAR(FieldType));
    TrialData.AddVariable(VAR(FieldConstant));
    TrialData.AddVariable(VAR(FieldAngle));
    TrialData.AddVariable(VAR(PositionDelaySeconds));
    TrialData.AddVariable(VAR(TargetIndex));
    TrialData.AddVariable(VAR(TargetAngle));
    TrialData.AddVariable(VAR(TargetPosition));
    TrialData.AddVariable(VAR(TargetDistance));
    TrialData.AddVariable(VAR(StartPosition));
    TrialData.AddVariable(VAR(FinishPosition));
    TrialData.AddVariable(VAR(VisualFeedback));
    TrialData.AddVariable("MissTrials",MissTrials);        // Required subject wellbeing variable
    TrialData.AddVariable("TrialDuration",TrialDuration);  // Required subject wellbeing variable
    TrialData.AddVariable(VAR(MovementReactionTime));
    TrialData.AddVariable(VAR(MovementDurationTime));
    
    // Set rows of TrialData to the number of trials.
    TrialData.SetRows(Trials);

    // Add each variable to the FrameData matrix.
    FrameData.AddVariable(VAR(TrialTime));                 // Required subject wellbeing variable
    FrameData.AddVariable(VAR(State));
    FrameData.AddVariable(VAR(StateGraphics));
    FrameData.AddVariable(VAR(TriggerFlag));
    FrameData.AddVariable(VAR(PhotoTransistor));
    FrameData.AddVariable(VAR(ForcesFunctionLatency));
    FrameData.AddVariable(VAR(ForcesFunctionPeriod));
    FrameData.AddVariable("GraphicsSwapBuffersCount",GRAPHICS_SwapBuffersCount);
    FrameData.AddVariable("GraphicsSwapBuffersToVerticalRetraceTime",GRAPHICS_SwapBuffersToVerticalRetraceTime);
    FrameData.AddVariable("GraphicsTimeUntilVerticalRetraceOnset",GRAPHICS_VerticalRetraceNextOnsetTime);
    FrameData.AddVariable("GraphicsTimeUntilVerticalRetraceOffset",GRAPHICS_VerticalRetraceNextOffsetTime);
    FrameData.AddVariable(VAR(RobotPosition));
    FrameData.AddVariable(VAR(RobotVelocity));             // Required subject wellbeing variable
    FrameData.AddVariable(VAR(RobotForces));               // Required subject wellbeing variable
    FrameData.AddVariable(VAR(HandleForces));
    FrameData.AddVariable(VAR(HandleTorques));
    FrameData.AddVariable(VAR(CursorPosition));

	/*
    if( OptoTrakMarkers > 0 )
    {
        OptoTrakInitialize();
        FrameData.AddVariable(VAR(OptoTrakFrame));
        FrameData.AddVariable(VAR(OptoTrakPosition),OptoTrakMarkers);
    }
	*/

    // Set rows of FrameData to maximum.
    FrameData.SetRows(FRAMEDATA_ROWS);
}

/******************************************************************************/

BOOL TrialListSubset( void )
{
int item,i;
int Catch,CatchTrial,TrialOffset;

    if( TargetCount == 0 )
    {
        printf("No targets specified.\n");
        return(FALSE);
    }

    TrialOffset = TotalTrials;

    // Set-up permite lists for exposure and catch-trial targets.
    TargetPermute.Init(0,TargetCount-1,TRUE);

    if( CatchTargetCount > 0 )
    {
        CatchTargetPermute.Init(0,CatchTargetCount-1,TRUE);
    }

    NullTrial = 0;
    ExposureTrial = 0;
    CatchTrial = 0;
    
    // Create list of trials.
    for( Trial=1; (Trial <= Trials); Trial++ )
    {
        // Target angle and target position.
        TargetIndex = TargetPermute.GetNext();
        TargetAngle = TargetAngles[TargetIndex];
        TargetPosition = HomePosition + (TargetAngleVector(TargetAngle) * TargetDistance);

        // Start and finish position of movement.
        StartPosition = HomePosition;
        FinishPosition = HomePosition;

        // Visual feedback flag.
        VisualFeedback = TRUE;

        // Set trial type depending on trial number, etc...
        if( (Trial >= ExposureTrials[0]) && (Trial <= ExposureTrials[1]) )
        {
            // Exposure trial.
            TrialType = TRIAL_EXPOSURE;
            FieldType = ExposureFieldType;
            FieldConstant = ExposureFieldConstant;
            FieldAngle = ExposureFieldAngle;
            ExposureTrial++;
        }
        else
        {
            // NULL trial (pre-exposure or post-exposure).
            TrialType = TRIAL_NULL;
            FieldType = FIELD_NONE;
            NullTrial++;
        }

        // The following variables are filled in as trial is run...
        MissTrials = 0;
        MovementReactionTime = 0.0;
        MovementDurationTime = 0.0;

        // Save TrialData row for this trial.
        TrialData.RowSave(TrialOffset+Trial);
    }

    // Now put in the catch trials.
    for( Trial=CatchTrials[0]; (Trial < CatchTrials[1]); Trial+=CatchTrials[2] )
    {
        // Select a random trial to make a catch trial.
        Catch = irand(1,CatchTrials[2]-1);

        // Load the current trial data.
        TrialData.RowLoad(Catch+Trial);

        // Make it a catch trial details.
        TrialType = TRIAL_CATCH;
        FieldType = CatchFieldType;
        FieldConstant = CatchFieldConstant;
        FieldAngle = CatchFieldAngle;

        // No visual feedback for visuomotor rotation catch trials.
        if( FieldType == FIELD_VISMOTROT )
        {
            VisualFeedback = FALSE;
        }

        // Select a random target for catch trial if required.
        if( CatchTargetCount > 0 )
        {
            TargetIndex = CatchTargetPermute.GetNext();
            TargetAngle = CatchTargetAngles[TargetIndex];
            TargetPosition = HomePosition + (TargetAngleVector(TargetAngle) * TargetDistance);
        }

        // Overwrite trial with new catch-trial details.
        TrialData.RowSave(Catch+Trial);

        CatchTrial++;
    }    

    printf("%d Total Trials, %d Null, %d Exposure, %d Catch.\n",Trials,NullTrial,ExposureTrial,CatchTrial);

    return(TRUE);
}

/******************************************************************************/

BOOL TrialList( void )
{
int item,i;
BOOL ok=TRUE;

    TotalTrials = 0;

    // Single or multiple configuration file paradigm?
    if( ConfigFiles == 1 )
    {
  
        TotalTrials = Trials;
        ConfigIndex = 0;
    }
    else
    {
        // Loop over configuration files, counting the number of trials.
        for( ok=TRUE,ConfigIndex=1; (ok && (ConfigIndex < ConfigFiles)); ConfigIndex++ )
        {
            if( !ConfigLoad(ConfigFile[ConfigIndex]) )
            {
                ok = FALSE;
                continue;
            }

            TotalTrials += Trials;
            printf("%d %s Trials=%d TotalTrials=%d\n",ConfigIndex,ConfigFile[ConfigIndex],Trials,TotalTrials);
        }

        ConfigIndex = 1;
    }

    if( (TotalTrials == 0) || !ok )
    {
        return(FALSE);
    }

    // Set rows of TrialData to the number of trials.
    TrialData.SetRows(TotalTrials);

    printf("Making list of %d trials (ESCape to abort)...\n",TotalTrials);

    TotalTrials = 0;
    TotalNullTrials = 0;
    TotalExposureTrials = 0;
    TotalCatchTrials = 0;

    // Loop over configuration files, appending each to growing trial list.
    for( ok=TRUE; (ok && (ConfigIndex < ConfigFiles)); ConfigIndex++ )
    {
        if( ConfigIndex > 0 )
        {
            if( !ConfigLoad(ConfigFile[ConfigIndex]) )
            {
                ok = FALSE;
                continue;
            }
        }

        // Create subset of trials for this configuration file.
        if( !TrialListSubset() )
        {
            ok = FALSE;
            continue;
        }

        TotalTrials += Trials;
        TotalNullTrials += NullTrial;
        TotalExposureTrials += ExposureTrial;
        TotalCatchTrials += CatchTrial;

        printf("%d %s Trials=%d TotalTrials=%d\n",ConfigIndex,ConfigFile[ConfigIndex],Trials,TotalTrials);
    }

    if( !ok )
    {
        return(FALSE);
    }

    printf("%d Total Trials, %d Null, %d Exposure, %d Catch.\n",TotalTrials,TotalNullTrials,TotalExposureTrials,TotalCatchTrials);

    // Total number of trails.
    Trials = TotalTrials;

    // Save trial list to file.
    ok = DATAFILE_Save(TrialListFile,TrialData);
    printf("%s %s Trials=%d.\n",TrialListFile,STR_OkFailed(ok),TrialData.GetRows());

    // Reset trial number, etc.
    Trial = 1;
    TrialSetup();
    ExperimentTimer.Reset();
    StateNext(STATE_INITIALIZE);

    return(TRUE);
}

/******************************************************************************/

void main( int argc, char *argv[] )
{
    // Set base LOOPTASK function to specific frequency.
    // LOOPTASK_FREQUENCY(500.0);

    // Process command-line parameters.
    if( !Parameters(argc,argv) )
    {
        exit(0);
    }

    // Check disk space and file's existence.
    if( !DATAFILE_Check(DataFile) )
    {
        exit(0);
    }

    // Initialize configuration file variables once.
    ConfigInit();

    // Load the first (and possibly the only) configuration file.
    if( !ConfigLoad(ConfigFile[0]) )
    {
        exit(0);
    }

    // Initialize variables, etc.
    Initialize();

    // Create list of trials to run.
    TrialList();

    // Start the robot.
    if( DeviceStart() )
    {
        // Start the graphics system.
        if( GraphicsStart(argc,argv) )
        {
            // The experiment is run as part of graphics processing.
            GraphicsMainLoop();
        }
    }

    // Exit the program.
    ProgramExit();
}

/******************************************************************************/

