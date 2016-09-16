/* NAMING POLICY:

- Array (prefix 'a') starts with 0

- matrix (prefix 'm') starts with 1

- for array or matrix of indices, put 0 or 1 at the end to clarify
*/

#define MODULE_NAME "SenLearning"

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

// belows are to be set in Parameters, which parses the command line input
#define CONFIG_FILES 256
int ConfigFiles=0;
int ConfigIndex;
STRING ConfigFile[CONFIG_FILES];
STRING ConfigSaveFile;
STRING MetaConfigFile="";
STRING DataName="";
STRING DataFile="";
STRING TrialListFile="";

BOOL	DEBUG		= 0;	// cfg: debug flag

/******************************************************************************/
// robot
/******************************************************************************/

STRING 	RobotName	= ""; 			// will be read from cfg file
int 	RobotID 	= ROBOT_INVALID;// will be assigned by ROBOT_open in DeviceStart
BOOL 	RobotFT 	= FALSE;		// determines whether to use force/torque sensor in RobotForcesFunction
double	ForceMax	= 90.0;		// hardcoded here to limit the maximum force applied in RobotForcesFunction
                                                                                                               
// belows will be updated in RobotForcesFunction, and will be used everywhere, and will be saved as FrameData
matrix 	RobotPosition(3,1);		// FrameData
matrix 	RobotOffset(3,1);		// FrameData
matrix 	RobotVelocity(3,1);		// FrameData
double 	RobotSpeed;

matrix 	RobotForces(3,1);		// FrameData: determined from force field will be assigned to the robot
matrix 	HandleForces(3,1);		// FrameData: when RobotFT is on
matrix 	HandleTorques(3,1); 	// FrameData: when RobotFT is on

double 	LoopTaskFrequency; 		// assigned in DeviceStart, but not used anywhere
double 	LoopTaskPeriod;			// assigned in DeviceStart, but not used anywhere

TIMER_Interval  RobotForcesFunctionLatency("ForcesFunction");		// measured in RobotForcesFunction
double 	ForcesFunctionLatency;										// FrameData variable
TIMER_Frequency RobotForcesFunctionFrequency("ForcesFunction");		// measured in RobotForcesFunction
double 	ForcesFunctionPeriod;										// FrameData variable

/******************************************************************************/
// force field and movement related variables and timers
/******************************************************************************/

// force field types.
#define FIELD_NONE      0
#define FIELD_VISCOUS   1
#define FIELD_VISMOTROT 2
#define FIELD_CHANNEL   3
#define FIELD_VISUAL		4  	// state dependent dimming cursor

// force field data structure
matrix	ForceFieldForces(3,1);		// assigned in RobotForcesFunction, according to the state and RobotFieldType    
BOOL  	ForceFieldStarted=FALSE;	// becomes true when TrialStart:ForceFieldStart                                
matrix 	ForceFieldPosition(3,1);	// used in position dependent force field (e.g RobotFieldType==FIELD_CHANNEL)  
matrix	ForceFieldProjection(3,3);	// projection matrix for channel trials
RAMPER 	ForceFieldRamp;				// this may slowly increase the force field when it is turned on,                     
									// started in DeviceStart, goes up in TrialStart:ForceFieldStart, goes down in TrialStop:ForceFieldStop
double 	ForceFieldRampTime=0.1;                                                                                 

// TrialData related to force field
int    	FieldType;			// field type for specific trial, assigned in TrialListSubset
double 	FieldConstant;		// field constant for specific trial, assigned in TrialListSubset (F = constant * fieldmatrix * velocity)
double	FieldSensitivity;	// sensitivity of visual information to the state 
double	FieldAngle;

char 	*FieldText[] = { "None","Viscous","Visuomotor","Channel"};

// Belows are copied from above trial varilables when TrialSetup during STATE_SETUP
int    	RobotFieldType;
double 	RobotFieldConstant;
double 	RobotFieldSensitivity;	
double 	RobotFieldAngle;	
matrix 	RobotFieldMatrix(3,3);

double 	CatchFieldConstant;		// cfg
double 	CatchFieldDamp;			// cfg

BOOL	bCursorJump;
BOOL	bCursorAlpha;

double 	AlphaConstant;		// cfg
double 	AlphaSensitivity;	// cfg
double 	AlphaAngle;			//cfg

double 	JumpSTDMax;
double 	JumpSTDSensitivity;


// movement related timers
double 	MovementReactionTime=0.0;					// TrialData measured in StateProcessLoopTask
TIMER  	MovementReactionTimer("MovementReaction");	// when timeout, status becomes STATE_TIMEOUT in StateProcessLoopTask
double 	MovementReactionTimeOut=0.5;				
double 	MovementDurationTime=0.0;					// TrialData measured in MovementDurationTime
TIMER  	MovementDurationTimer("MovementDuration");	// when timeout, status becomes STATE_TIMEOUT in StateProcessLoopTask
double 	MovementDurationTimeOut=2.0;				
double 	aMovementDurationEval[ 4 ];
TIMER  	MovementFinishedTimer("MovementFinished");	// checked in StateProcessLoopTask:MovementFinished
TIMER  	NotMovingTimer("NotMoving");				// used in StateProcess:RobotNotMoving		
double 	NotMovingSpeed;
double 	NotMovingTime;


//peak speed analysis
double 	PeakSpeed;
double	PeakDesiredSpeed=50.0;
double	SpeedError=8.0;

// Homing timers and variables to generate minjerk homing trajectory
BOOL	bGuidedHoming;								// whether to use guided homing or not
TIMER	HomingTimer("HomingTimer"); 				// this is used for minjerk homing
			
matrix 	HomingStartPosition;
double 	HomingDistance;
double 	HomingDuration;
matrix	PassiveMinJerk(1,10000);

matrix 	MovementStartPosition(3,1); 	//TrialData: this will be measured and stored
matrix 	MovementFinishPosition(3,1); 	//TrialData: this will be measured and stored
/******************************************************************************/
// TrialData and related 
/******************************************************************************/

// TrialData:
//		- contains for each trials
//		- each row entries are set in TrialListSubset using TrialData.RowSave(Trial)
//		- each row is read in TrialSetup using TrialData.RowLoad(Trial)
//			, which updates all global variables that belong to TrialData
//		- after variables are changed, the TrialData is updated using TrialData.RowSave(Trial) in TrialSave
MATDAT 	TrialData("TrialData");


// Trial types: each type has its own field varables (FieldType, FieldConstant, FieldSensitivity) specified in cfg
#define TRIAL_PRE_EXPO  0	// pre-exposure or post-exposure trials (without force field)
#define TRIAL_EXPOSURE  1	// trials under force field
#define TRIAL_CATCH     2	// probe trial?
#define TRIAL_POST_EXPO 3	// pre-exposure or post-exposure trials (without force field)


#define	GROUPS			10 	// maximum number of groups	

// TrialData variable
int    	TrialType;

// trial data: 
int    	Trials=0;						// cfg
int		nPreExposureBlock;				// cfg
int 	nExposureBlock;					// cfg
int 	nPostExposureBlock;				// cfg
int 	nTrialsPerBlock;				// computed from cfg
int	    nBlocks; 						// computed from cfg

int 	CatchFreq;						// cfg

int    	ExposureTrials=0;				// cfg specifies the beginning and end indices of exposure trials
int    	NullTrials=0;					// determined as a trials before and after exposure trials
int    	CatchTrials=0;					// determined in TrialListSubset



#define	HOMES 20					// this is maximum number of homes
double 	aHomePosition[HOMES*3];			// cfg: array of home positions, this will be converted to 3 x n matrix later
int		nHomePosition;

#define	TARGETS 20							// this is maximum number of homes
double	aTargetDistance[TARGETS];
double	aTargetAngle[TARGETS];
int		nTargetDistance;
int		nTargetAngle;

// current index
int    	curTrialIndex1;		// current trial index: increment when TrialNext

// timers
double 	TrialTime; 				// FrameData to be saved
TIMER  	TrialTimer("Trial");	// records to TrialTime when TrialsStart, RobotForceFunction, TrialStop
double 	InterTrialDelay=0.5;	// used in InterTrialDelayTimer
TIMER  	InterTrialDelayTimer("InterTrialDelay");	// to pause for InterTrialDelay
double 	TrialDuration=0.0;		// TrialData to be saved, measured in TrialStop
BOOL   	TrialRunning=FALSE; 	// becomes TRUE when TrialStart, FALSE when TrialAbort or TrialStop. Used in StateProcess

int    	RestTrials = 0; 		// if nonzero, rest every X trials (in StateProcess)


#define EVAL_SLOW		0
#define EVAL_GOOD_SLOW	1
#define EVAL_GREAT		2
#define EVAL_GOOD_FAST	3
#define EVAL_FAST		4
#define EVAL_OVERSHOOT	5
#define EVAL_MISS		6 // added for senLearning

int		TrialEvaluation; 		// TrialData: how good was the movement
BOOL 	OvershootFlag = 0;		// determined during moving

/******************************************************************************/
// FrameData and related
/******************************************************************************/
MATDAT 	FrameData("FrameData"); // like TrialData, this will be save using FrameData.RowSave() in FrameProcess
#define	FRAMEDATA_ROWS 10000 	// used static array, rows allocated in Initialize

BOOL   	FrameRecord=FALSE;		// turned on/off in FrameStart/FrameStop, used in RobotForcesFunction:FrameProcess to save framedata
BOOL   	TriggerFlag=FALSE; 		// FrameData variable that specifies whether trial is triggered (not a control variable)

/******************************************************************************/
// Eye tracker variables
/******************************************************************************/

// Eye tracker variables. (1): this number based on James' email
STRING EyeTrackerConfig="";
BOOL   EyeTrackerFlag=FALSE;
int    EyeTrackerFrameCount=0;
BOOL   EyeTrackerFrameReady=FALSE;
double EyeTrackerTimeStamp=0.0;
double EyeTrackerEyeXY[2]={ 0.0,0.0 };
TIMER_Frequency EyeTrackerFrameFrequency("EyeTrackerFrameFrequency");

/******************************************************************************/
// states and related timers
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
#define STATE_EYETRACKER  15 // Eye tracker calibration state. (2)
#define STATE_MAX         16

int 	State=STATE_INITIALIZE;
int 	StateLast;

// StateTimer: set duration to pause for some states
TIMER 	StateTimer("State");
double 	TrialDelay=0.0;			// pause between trials
double 	ErrorWait=2.0;			// pause duration when error
double 	RestWait=0.0;			// wait duration during rest

// StateGraphics: stores same state as State, but mainly used in GraphicsDisplay. This changes only when draw=TRUE in GraphicsIdle (glutIdleFunc)
int 	StateGraphics=STATE_INITIALIZE;
int 	StateGraphicsLast;
TIMER 	StateGraphicsTimer("StateGraphics");

// Eye tracker calibration state text added (2)-1
char 	*StateText[] = { "Initialize","Setup","Home","Start","Delay","Go","TargetWait","MoveWait","Moving","Finish","InterTrial","Exit","TimeOut","Error","Rest", "EyeTracker" }; 

// StateLoopTask: if it is on, corresponding state will call StateProcessLoopTask (STATE_MOVEWAIT, STATE_MOVING)
BOOL 	StateLoopTask[STATE_MAX] = { FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE };
int 	StateErrorResume; 		// used in ErrorState to specify the state the program will resume from

// timer for whole experiment
TIMER  	ExperimentTimer("Experiment");	// reset when TrialList, STATE_INITIALIZE, and measured in TrialSave and ProgramExit
double 	ExperimentTime;					// TrialData, specified the elapsed time since the beging of the experiment

/******************************************************************************/
// visualization of target, cursor and home
/******************************************************************************/

// configurations display
matrix 	TextPosition(3,1);			// this is not set in code, neither in cfg file

// home
double 	HomeTolerance=0.50;				// cfg: used in RobotHome and CursorHome (boolean check)
int		HomeColor=WHITE;
int 	NotHomeColor=GREY;
double 	HomeRadius=1.0;

matrix 	HomePosition(3,1);				// TrialData: = mHomePosition[ HomeIndex ]

// vailables read from cfg: target is defined as angle and distance (but only one distance is allowed for now)
double 	TargetTolerance;			// cfg: tolerance for position at target (in degree)

double	OvershootTolerance;				// cfg: amount of overshoot allowed

// TrialData variables
double	TargetAngle;				// TrialData
double 	TargetDistance;				// TrialData
matrix 	TargetPosition(3,1);		// TrialData: calculated for each trial by HomePosition + (target angle & distance) in TrialListSubset

// additional target variables
BOOL	TargetTestFlag=FALSE;	// Set in parameters (i.e. from command line): if on, the program will display all target positions
int 	TargetColor=YELLOW;	
int		FinishColor=WHITE; 		// when cursor reached the finish position
double 	TargetRadius=1.0;

// Permute to randomize targets, permuted in TrialListSubset
PERMUTELIST	CatchTargetPermute;	// permute angles for catch trials
PERMUTELIST HomePermute;		// permute home pos/ori (added by shyeo)

// cursor variables
double 	PositionDelaySeconds=0.0; 	// read from cfg, also saved in Trial Data 
int    	PositionDelayItems;			// used for delay handling in RobotForcesFunction
matrix 	PositionDelayList;			// used for delay handling in RobotForcesFunction
matrix 	CursorPosition(3,1);		// FrameData: updated in RobotForceFunction, if PositionDelaySeconds is non-zero, the update will be delayed.
double	CursorAlpha = 1.0;			// FrameData: alpha value of cursor (when mode FIELD_VISUAL)
double	CursorJumpSTD;				// FrameData: STD of cursor jump(when mode FIELD_VISUAL)

int		CursorColor=RED;        	// used in GraphicsDisplay
double 	CursorRadius=0.5;			// used in GraphicsDisplay

#define SCREEN_Y_MIN 	-16
#define SCREEN_Y_MAX 	16

BOOL	bOffsetCursor;
matrix	CursorOffsetTrial(3,1);			// offset from robot position to cursor position THAT IS USED in TRIAL
matrix	CursorOffset(3,1);				// CURRENT offset from robot position to cursor position (can be different from CursotOffsetTrial when homing)
matrix	OldCursorOffset(3,1);			// this is for gradual change
matrix	CursorVisOffset(3,1);			// visual only offset of cursor from CursorPosition (this is to add noise)
double 	CursorOffsetMaxDistance;

TIMER  	JumpTimer("CursorJump");
double	JumpTimeOut;
double 	ProjSpeed; // projected speed along the direction of the visual clarity
/******************************************************************************/
// error handling
/******************************************************************************/

// summary of miss trials: whenever there is something wrong in StateProcess, MissTrial is called
int    MissTrials=0;
int    MissTrialsTotal=0;
double MissTrialsPercent=0.0;

// error functions (Error...): use MessageSet to display the error message


/******************************************************************************/
// sound and display
/******************************************************************************/

// display control
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
int    GraphicsMode=GRAPHICS_DISPLAY_2D;
STRING GraphicsModeString="";
STRING GraphicsString="";

// sound: all sounds are played using WaveListPlay
struct WAVELIST WaveList[] = 
{
    { "HighBip","HIGH_BIP.WAV",NULL },
    { "LowBip","LOW_BIP.WAV",NULL },
    { "Bip","MID_BIP.WAV",NULL },
    { "","",NULL },
};
TIMER_Interval WaveListPlayInterval("WaveListPlay");


/******************************************************************************/
/******************************************************************************/
//
// 									FUNCTIONS
//
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/

void ProgramExit( void );

/******************************************************************************/
#define ASSERT(e) \
	if (!(e) && DEBUG ) \
	{ \
		printf("%s(%u) : ASSERTION FAILED!!!\n", __FILE__, __LINE__); \
		ProgramExit(); \
	}

/******************************************************************************/
	
matrix TargetAngleVector( double angle );
	
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
// set parameters to be read from cfg file
{
    // Reset configuration variable list.
    CONFIG_reset();

    // Set up variable list for configuration.
	CONFIG_setBOOL(VAR(DEBUG));
    CONFIG_set(VAR(RobotName));
		
    CONFIG_setBOOL(VAR(RobotFT));
	CONFIG_set(VAR(RobotOffset));
    CONFIG_set(VAR(ForceMax));
	
    CONFIG_set("GraphicsMode",GraphicsModeString);
    CONFIG_set("GraphicsSyncTime",GraphicsVerticalRetraceSyncTime);
    CONFIG_set("GraphicsCatchTime",GraphicsVerticalRetraceCatchTime);
    CONFIG_set("GraphicsSyncPos",GraphicsSyncPosition);
	
	CONFIG_set(VAR(EyeTrackerConfig)); // Eye tracker configuration file. (3)

    CONFIG_set(VAR(TextPosition));
    CONFIG_set(VAR(HomeRadius));
	CONFIG_set(VAR(CursorRadius));
    
    CONFIG_set(VAR(aHomePosition), HOMES*3);
	CONFIG_set(VAR(HomeTolerance));
	    
	CONFIG_set(VAR(aTargetDistance), TARGETS);
	CONFIG_set(VAR(aTargetAngle), TARGETS);
	CONFIG_set(VAR(TargetDistance));
    CONFIG_set(VAR(TargetRadius));
	CONFIG_set(VAR(TargetTolerance));
    	
	CONFIG_set(VAR(MovementReactionTimeOut));
    CONFIG_set(VAR(MovementDurationTimeOut));
	CONFIG_set(VAR(aMovementDurationEval),4);
    CONFIG_set(VAR(OvershootTolerance));
	
	CONFIG_set("PositionDelay",PositionDelaySeconds);
    CONFIG_set(VAR(ErrorWait));
    CONFIG_set(VAR(TrialDelay));
    CONFIG_set(VAR(InterTrialDelay));
    CONFIG_set(VAR(RestTrials));
    CONFIG_set(VAR(RestWait));
    CONFIG_set(VAR(NotMovingSpeed));
	CONFIG_set(VAR(NotMovingTime));
    CONFIG_set(VAR(ForceFieldRampTime));
		
	CONFIG_set(VAR(CatchFieldConstant));
	CONFIG_set(VAR(CatchFieldDamp));
		
	//CONFIG_set(VAR(HomingForceConstant));
	//CONFIG_set(VAR(HomingDampConstant));
	
    CONFIG_set(VAR(Trials));
	CONFIG_set(VAR(nPreExposureBlock));
	CONFIG_set(VAR(nExposureBlock));
	CONFIG_set(VAR(nPostExposureBlock));
	CONFIG_set(VAR(CatchFreq));

	CONFIG_setBOOL(VAR(bOffsetCursor));
	CONFIG_set(VAR(CursorOffsetMaxDistance));

	CONFIG_setBOOL(VAR(bCursorJump));
	CONFIG_setBOOL(VAR(bCursorAlpha));
	
	CONFIG_set(VAR(AlphaConstant));
	CONFIG_set(VAR(AlphaSensitivity));
	CONFIG_set(VAR(AlphaAngle));
	
	CONFIG_set(VAR(JumpTimeOut));
	CONFIG_set(VAR(JumpSTDMax));
	CONFIG_set(VAR(JumpSTDSensitivity));

	
	CONFIG_setBOOL(VAR(bGuidedHoming));
	
}

/******************************************************************************/

void ConfigInit( void )
{
int i;
    	
	for( i=0; (i < HOMES); i++ )
	{
		aHomePosition[ 3*i ]	= 999;
		aHomePosition[ 3*i + 1]	= 999;
		aHomePosition[ 3*i + 2]	= 999;
						
		aTargetAngle[ i ]		= 999;
		aTargetDistance[ i ]	= 999;
		
	}
}

/******************************************************************************/
BOOL ConfigParse( void )
{
int i;

	nHomePosition = 0;
	nTargetDistance = 0;
	nTargetAngle = 0;

	for( i = 0; i < HOMES; i += 3 )
	{
		if (aHomePosition[ i ] == 999)
		{
			break;
		}
		else
		{
			nHomePosition++;
		}
	}
	
	for (i = 0; i < TARGETS; i++ )
	{
		if (aTargetDistance[ i ] == 999)
		{
			break;
		}
		else
		{
			nTargetDistance++;
		}
	
	}
	
	for (i = 0; i < TARGETS; i++ )
	{
		if (aTargetAngle[ i ] == 999)
		{
			break;
		}
		else
		{
			nTargetAngle++;
		}
	
	}
	
	nTrialsPerBlock = nTargetDistance * nTargetAngle * nHomePosition;
	nBlocks = nPreExposureBlock + nExposureBlock + nPostExposureBlock;
	Trials = nTrialsPerBlock * nBlocks;
	
	
	printf("ConfigParse nHomePosition: %d nTargetDistance: %d nTargetAngle: %d \n", nHomePosition, nTargetDistance, nTargetAngle);
	
	
	return true;
}
/******************************************************************************/

BOOL ConfigLoad( char *file )
// call ConfigSetup, ConfigInit 
// count targets for normal/catch trials and print out
{
int i;
BOOL ok=TRUE;
int tempcount = 0;

    // Setup and initialize configuration variables.
    ConfigSetup();		// set bunch of parameters to be read from cfg
    ConfigInit(); 		// reset angles (to 360) and counts (to 0)

    // Load configuration file.
    if( !CONFIG_read(file) )
    {
        printf("ConfigLoad(%s) Cannot read file.\n",file);
        return(FALSE);
    }
	
	printf("aTargetAngle: %1lf \n", aTargetAngle[0]);
	
	if( !ConfigParse() )
	{
	    printf("ConfigParse returned an error\n");
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
	
	// Eye tracker in use? (4)
    EyeTrackerFlag = !STR_null(EyeTrackerConfig);

    printf("ConfigLoad(%s) Load %s.\n",file,STR_OkFailed(ok));
    CONFIG_list(printf);
		
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
float boundingOffset = 0.5;
float drawTextWidth, drawTextHeight;
matrix boundingBox(2,4);
BOOL bound;

	p = pos;
    w = strlen(string);
	
	if ( string[w - 1] == '#' )
	{
		bound = TRUE; // draw bounding box of the text in this case
	}
	else
	{
		bound = FALSE;
	}
	
    glPushMatrix();
	
	p(1,1) -= w * (s * 100.0) / 2;
	
	if ( bound )
	{
		w = w - 1;
		
		// this is pretty adhoc
		drawTextWidth = glutStrokeLength(font, (unsigned char *) string) * s * (w - 1) / w; // (w-1)/w multiplied because the last character will not be displayed
		drawTextHeight = s * 100;
		
		boundingBox(1,1) = p(1,1) - boundingOffset;
		boundingBox(1,2) = p(1,1) + boundingOffset + drawTextWidth;
		boundingBox(1,3) = p(1,1) + boundingOffset + drawTextWidth;
		boundingBox(1,4) = p(1,1) - boundingOffset;
		
		
		boundingBox(2,1) = p(2,1) - boundingOffset;
		boundingBox(2,2) = p(2,1) - boundingOffset;
		boundingBox(2,3) = p(2,1) + boundingOffset + drawTextHeight;
		boundingBox(2,4) = p(2,1) + boundingOffset + drawTextHeight;
		
		
		glBegin(GL_QUADS);
		glColor3f(255, 0, 0);
		glVertex3f( boundingBox(1,1), boundingBox(2,1), 0);
		glVertex3f( boundingBox(1,2), boundingBox(2,2), 0);
		glVertex3f( boundingBox(1,3), boundingBox(2,3), 0);
		glVertex3f( boundingBox(1,4), boundingBox(2,4), 0);
		glEnd();
	}

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
static matrix d, dt, temp;

    //ForceFieldPosition = RobotPosition;
	ForceFieldPosition = CursorPosition;
	 
	//d = TargetPosition - RobotPosition;
	d = TargetPosition - CursorPosition;
	matrix_transpose(dt, d);
	temp = dt * d;
	ForceFieldProjection = 1/temp(1,1) * d * dt;
	
	ForceFieldProjection = -1 * ForceFieldProjection;
	ForceFieldProjection(1,1) += 1;
	ForceFieldProjection(2,2) += 1;
	ForceFieldProjection(3,3) += 1;
	 
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

matrix CreateMinimumJerk(double Ampl, double Time, int numpoints)
// create 1D minimum trajectory profile
{
    double t3,t4,t5,p;
    int i;
    matrix time;
    matrix y;
    
	time.dim(1,numpoints);
	y.dim(1,numpoints);
	for(i=1; i<=numpoints; i++)
	{
		time(1,i)=(double(i)/double(numpoints));
		t3=pow(time(1,i),3);
		t4=pow(time(1,i),4);
		t5=pow(time(1,i),5);
		p=(10.0*t3) - (15.0*t4) + (6.0*t5);
		y(1,i)=p*Ampl;
        
	}
    
	return(y);
}

BOOL RobotActive( void );

/******************************************************************************/

void RobotForcesFunction( matrix &position, matrix &velocity, matrix &forces )
{
int i,j;
static matrix P,R,_R;
double t;
int n;
static matrix p(1,3);
double offsetDist;
double sigma, xoffset, yoffset;


    // Monitor timing of Forces Function (values saved to FrameData).
    ForcesFunctionPeriod = RobotForcesFunctionFrequency.Loop();
    RobotForcesFunctionLatency.Before();

    TrialTime = TrialTimer.ElapsedSeconds();

    // Kinematic data passed from robot API.
    RobotPosition = position + RobotOffset;
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
      
	CursorPosition = P + CursorOffset;

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
			// apply projection matrix to the channel axis and get normal vector P
			P = ForceFieldProjection * (CursorPosition - ForceFieldPosition); // get perpendicular component
			ForceFieldForces = CatchFieldConstant * P + CatchFieldDamp * ForceFieldProjection * velocity;
			break;
			
		case FIELD_VISUAL : // do nothing here
			break;
    }

	// add position dependent force field in HOME state
	if (bGuidedHoming && State == STATE_HOME && curTrialIndex1 != 1 && RobotActive() )
	{
		t = HomingTimer.ElapsedSeconds();
		
		if (t < 0.15)
		{
			n = 1;
		}
		else if (t > HomingDuration + 0.15)
		{
			n = 10000;
		}
		else
		{
			n = 10000 * (t - 0.15) / HomingDuration;
		}
		
		p = HomingStartPosition + PassiveMinJerk(1,n) * (HomePosition - HomingStartPosition);
		
		ForceFieldForces(1,1) = -20.0 * (CursorPosition(1,1) - p(1,1));
		ForceFieldForces(2,1) = -20.0 * (CursorPosition(2,1) - p(2,1));
		ForceFieldForces(3,1) = 0.0;
		
		if ( bOffsetCursor )
		{
			CursorOffset = OldCursorOffset + PassiveMinJerk(1,n) * (CursorOffsetTrial - OldCursorOffset);
		}
		
	}
	
	//ProjSpeed = cos(D2R(TargetAngle + RobotFieldAngle)) * RobotVelocity(1,1) + sin(D2R(TargetAngle + RobotFieldAngle)) * RobotVelocity(2,1);
	ProjSpeed = cos(D2R(RobotFieldAngle)) * RobotVelocity(1,1) + sin(D2R(RobotFieldAngle)) * RobotVelocity(2,1);
	
	// cursor alpha for FIELD_VISUAL
	if( bCursorAlpha && StateGraphics != STATE_ERROR && RobotFieldType == FIELD_VISUAL)
	{
		if (StateGraphics >= STATE_GO && StateGraphics <= STATE_MOVING)
		{
			CursorAlpha = RobotFieldConstant + RobotFieldSensitivity * ProjSpeed;
						
			if (CursorAlpha < 0) CursorAlpha= 0; 
			if (CursorAlpha > 1) CursorAlpha= 1; 
		}
		else
		{
			CursorAlpha = 1.0;
		}
	}
	
	
	
	//posn = CursorPosition + CursorOffset;
			
		
    // Process Finite State Machine.
    StateProcessLoopTask();

    // Monitor timing of Forces Function (values saved to FrameData).
    ForcesFunctionLatency = RobotForcesFunctionLatency.After();

    //OptoTrakRead();

    RobotForces = ForceFieldRamp.RampCurrent() * ForceFieldForces;

	// Get next frame of eye tracker data. (5)
    if( EyeTrackerFlag )
    {
        BOOL ok = EYET_FrameNext(EyeTrackerTimeStamp,EyeTrackerEyeXY,EyeTrackerFrameReady);

        if( EyeTrackerFrameReady )
        {
            EyeTrackerFrameCount++;
            EyeTrackerFrameFrequency.Loop();
        }
    }


    // Save frame data.
    FrameProcess();

    // Set forces to pass to robot API and clamp for safety.
    RobotForces.clampnorm(ForceMax); // clamp RobotForces, not forces (changed by shyeo)
	forces = RobotForces;
    
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

	// Stop eye tracker if required. (6)
    if( EyeTrackerFlag )
    {
        EYET_Close();
        EyeTrackerFrameFrequency.Results();
    }
}

/******************************************************************************/
BOOL DeviceStart( void )
// open and start robot / ForceFieldRamp
{
BOOL ok=TRUE;
int c;

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

	// Start eye tracker if required. (7)
    if( ok && EyeTrackerFlag )
    {
        // The GraphicsText function sets a string to be displayed by OpenGL. 
        ok = EYET_Open(EyeTrackerConfig,GraphicsText);
        printf("EYET_Open(%s) %s.\n",EyeTrackerConfig,STR_OkFailed(ok));
        EyeTrackerFrameFrequency.Reset();
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

double RobotDistance( matrix &home ) // this is the distance of Robot (NOT cursor when there is an offset)
{
double distance;

    distance = norm(RobotPosition-home);
	
    return(distance);
}

/******************************************************************************/


double CursorDistance( matrix &home ) // this is the non-visual distance WIHTOUT CursorOffset
{
double distance;

    distance = norm(CursorPosition-home);
	
    return(distance);
}

/******************************************************************************/

double AngDiff(double& ori1, double& ori2)
{
	double diff;
	
	diff = abs( ori1 - ori2 );
	
	while (TRUE)
	{
		if ( diff >= 360 ) diff -= 360;
		else break;
			
	}
	
	if (diff > 180) diff = 360 - diff;

	return (diff);
}

/******************************************************************************/

BOOL RobotAt( matrix &pos, double tolerance )
{
	if( RobotDistance(pos) <= tolerance )
    {
        return TRUE;
    }
	else return FALSE;
}

/******************************************************************************/
BOOL CursorAt( matrix &pos, double tolerance)
{
    if( CursorDistance(pos) <= tolerance )
    {
        return TRUE;
    }
	
    else return FALSE;
}

/******************************************************************************/

BOOL RobotHome( void )
{
BOOL flag;

    flag = RobotAt(HomePosition, HomeTolerance);

    return(flag);
}

/******************************************************************************/

BOOL CursorHome( void )
{
BOOL flag;

    flag = CursorAt(HomePosition, HomeTolerance);

    return(flag);
}

/******************************************************************************/

BOOL MovementStarted( void )
{
BOOL flag;

    flag = !CursorAt(HomePosition, HomeTolerance);

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



BOOL MovementFinished( void )
{
BOOL flag=FALSE;

    // Is robot in the finish position?
	//if( !RobotAt(TargetPosition, TargetTolerance) )
	if( CursorHome() || !RobotNotMoving() ) // robot has reached to some position other than home
    {
        MovementFinishedTimer.Reset();
    }
	else
	{
		flag = TRUE;
	}
	
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

BOOL SingleTrialFrameData( char *FileName, int Trial, MATDAT &FrameData )
{
MATDAT TD("TrialData");
BOOL ok;

    TD.AddVariable(VAR(Trial));
    TD.SetRows(1);

    // Open the file for trial data.
    if( !DATAFILE_Open(FileName,TD,FrameData) )
    {
        return(FALSE);
    }
	
    TD.RowSave();
	
    ok = DATAFILE_TrialSave(1);

    DATAFILE_Close();
	
    // Select the original data file for the trial data.
    DATAFILE_ListSelect(DataFile);

    return(ok);
}

/******************************************************************************/

void TrialSetup( void )
{
double s, c;

	printf("----------------------------------------------------------------\n", curTrialIndex1);
	printf("Setting-up Trial %d...\n", curTrialIndex1);

    // Load trial variables from TrialData.
    TrialData.RowLoad( curTrialIndex1 );

	TargetPosition = HomePosition + (TargetAngleVector(TargetAngle) * TargetDistance);
	
	////// robot applies no force in this experiment /////
	RobotFieldType = FieldType;
	RobotFieldConstant = FieldConstant;
	RobotFieldSensitivity = FieldSensitivity;	
	RobotFieldAngle = FieldAngle;	
	//RobotFieldMatrix(3,3);

    
    TrialRunning = FALSE;
    StateGraphicsNext(State);
    TriggerOff();
	
	ForceFieldRamp.Up(); // this is for Homing force
}

/******************************************************************************/

void TrialStart( void )
{
    printf("Starting Trial %d...\n", curTrialIndex1 );
    printf("TargetAngle=%.1lf(deg) TargetDistance=%.1lf TrialType=%d FieldType=%d FieldConstant=%.2lf\n",TargetAngle, TargetDistance, TrialType,FieldType,FieldConstant);
    TrialTimer.Reset();
    TrialTime = TrialTimer.ElapsedSeconds();
    TrialRunning = TRUE;
    
    // Start recording frame data for trial.
    FrameStart();
	
			
}

/******************************************************************************/

void TrialStop( void )
{
    TrialRunning = FALSE;
    printf("Stopping Trial %d...\n", curTrialIndex1);

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
    printf("Aborting Trial %d...\n", curTrialIndex1);

    // Stop recording frame data for trial.
    FrameStop();

    // Stop force field.
    ForceFieldStop();
	
}

/******************************************************************************/

BOOL TrialSave( void )
{
BOOL ok=FALSE;
STRING file;

    ExperimentTime = ExperimentTimer.ElapsedSeconds();
    MissTrials = MissTrialsTotal;

    // Put values in the trial data
    TrialData.RowSave( curTrialIndex1 );

	// Save configuration file on the first trial.
    if( curTrialIndex1 == 1 )
    {
        strncpy(file,STR_stringf("%s.CFG",DataFile),STRLEN);
        if( !ConfigSave(file) )
        {
            return(FALSE);
        }
    }
	
    // Set-up the data file on the first trial.
    if( curTrialIndex1 == 1 )
    {
		 // Open the file for trial data.
		ok = DATAFILE_Open(DataFile,TrialData,FrameData);
		
		if( !ok )
        {
            printf("DATAFILE: Cannot open file: %s\n",DataFile);
            return(FALSE);
        }
    }

    // Write the trial data to the file.
    printf("Saving trial %d: %d frames of data collected in %.2lf seconds.\n", curTrialIndex1, FrameData.GetRow(), TrialDuration);
    
	ok = DATAFILE_TrialSave( curTrialIndex1 );
    
	printf("%s %s Trial=%d.\n",DataFile,STR_OkFailed(ok), curTrialIndex1);

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

    if( curTrialIndex1 < Trials )
    {
        curTrialIndex1++;
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

void BeepDone( void )
{
    WaveListPlay("Bip");
}

/******************************************************************************/

void BeepError( void )
{
    WaveListPlay("LowBip");
}

/******************************************************************************/

BOOL MessageFlag=FALSE;

void MessageSet( char *text, char *wave, int bgnd )
{
    GraphicsText(text);

    if( wave != NULL )
    {
        WaveListPlay(wave);
    }

    GRAPHICS_ClearColorPush();

    if( bgnd != -1 )
    {
        GRAPHICS_ClearColor(bgnd);
    }

    MessageFlag = TRUE;
}

/******************************************************************************/

void MessageSet( char *text, char *wave )
{
    MessageSet(text,wave,-1);
}
/******************************************************************************/

void MessageSet( char *text)
{
    GraphicsText(text);

    GRAPHICS_ClearColorPush();

    //if( bgnd != -1 )
    //{
    //    GRAPHICS_ClearColor(bgnd);
    //}

    MessageFlag = TRUE;
}

/******************************************************************************/

void MessageClear( void )
{
    if( MessageFlag )
    {
        GRAPHICS_ClearColorPop();
        GraphicsText("");
        MessageFlag = FALSE;
    }
}

/******************************************************************************/

void ErrorFrameDataFull( void )
{
	if ( DEBUG )
	{
		MessageSet("Frame data full", "LowBip", RED);
	}
	else
	{
		MessageSet("Trial Timed Out", "LowBip", RED);
	}
	
    printf("Error: Frame data full\n");
}

/******************************************************************************/

void ErrorMessage( char *str )
{
    MessageSet(str, "LowBip", RED);
    printf("Error: %s\n",str);
}

/******************************************************************************/

void ErrorMoveWaitTimeOut( void )
{
    MessageSet("Waited Too Long", "LowBip", RED);
    printf("Error: MoveWaitTimeOut\n");
}

/******************************************************************************/

void ErrorMoveTimeOut( void )
{
    MessageSet("Trial Timed Out", "LowBip", RED);
    printf("Error: MoveTimeOut\n");
}

/******************************************************************************/

//this is not used
void ErrorMoveTooSoon( void )
{
    MessageSet("Move After Sound");
    printf("Error: MoveTooSoon\n");
}

/******************************************************************************/

void ErrorRobotInactive( void )
{
    MessageSet("Handle Switch", "LowBip", RED);
    printf("Error: RobotInactive\n");
}

/******************************************************************************/

void ErrorState( int state )
{
    //BeepError();
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
    MissTrialsPercent = 100.0 * ((double)MissTrialsTotal / (double) curTrialIndex1);
    printf("\nMiss Trials = %d/%d (%.0lf%%)\n\n",MissTrialsTotal, curTrialIndex1, MissTrialsPercent);

    ErrorState(STATE_SETUP);
}

/******************************************************************************/

void Overshoot( void )
{
    MessageSet("OVERSHOT TARGET", "LowBip", -1);
}

/******************************************************************************/

void TooSlow( void )
{
	if(DEBUG)
	{
		MessageSet(STR_stringf("TOO SLOW: %1lf", (MovementDurationTime - 0.2) * 10 / TargetDistance + 0.2), "LowBip", -1);
	}
	else
	{
		MessageSet("TOO SLOW", "LowBip", -1);
	}
	
	WaveListPlay("LowBip");
}
/******************************************************************************/

void TooFast( void )
{
	if (DEBUG)
	{
		MessageSet(STR_stringf("TOO FAST: %1lf", (MovementDurationTime - 0.2) * 10 / TargetDistance + 0.2), "LowBip", -1);
	}
	else
	{
		MessageSet("TOO FAST", "LowBip", -1);
	}
}
/******************************************************************************/

void GreatFeedback( void )
{
	if(DEBUG)
	{
		
		MessageSet(STR_stringf("GREAT: %1lf", (MovementDurationTime - 0.2) * 10 / TargetDistance + 0.2), "HighBip", -1);
	}
	else
	{
		MessageSet("GREAT", "HighBip", -1);
	}
}

/******************************************************************************/

void OvershootFeedback( void )
{
    MessageSet("OVERSHOOT", "LowBip", -1);
}
/******************************************************************************/

void GoodSlowFeedback( void )
{
    if (DEBUG )
	{
		MessageSet(STR_stringf("SLOW: %1lf", (MovementDurationTime - 0.2) * 10 / TargetDistance + 0.2), "Bip", -1);
	}
	else
	{
		MessageSet("GOOD", "Bip", -1);
	}
}

void GoodFastFeedback( void )
{
    if ( DEBUG )
	{
		MessageSet(STR_stringf("FAST: %1lf", (MovementDurationTime - 0.2) * 10 / TargetDistance + 0.2), "Bip", -1);
	}
	else
	{
		MessageSet("GOOD", "Bip", -1);
	}
}

void MissFeedback( void )
{
	if ( DEBUG )
	{
		MessageSet(STR_stringf("MISSED: %1lf", (MovementDurationTime - 0.2) * 10 / TargetDistance + 0.2), "LowBip", RED);
	}
	else
	{
		MessageSet("MISSED!#", "LowBip", RED);
	}
}

/******************************************************************************/

void StateProcessLoopTask( void )
{
static matrix pos;

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

				//MovementStartPosition = RobotPosition;
				MovementStartPosition = CursorPosition;
				
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
				TriggerOff();
				MovementDurationTime = MovementDurationTimer.ElapsedSeconds();
				MovementFinishPosition = CursorPosition;			   
				
				if (!CursorAt(TargetPosition, TargetTolerance))
				{
					MissFeedback();
					TrialEvaluation = EVAL_MISS;
				}
				else
				{
					// devaluation based on duration
					if (OvershootFlag)
					{
						OvershootFeedback();
						TrialEvaluation = EVAL_OVERSHOOT;
					}
					else if (MovementDurationTime < (aMovementDurationEval[0] - 0.2) * TargetDistance / 10 + 0.2)
					{
						// too fast
						TooFast();
						TrialEvaluation = EVAL_FAST;
					}
					else if (MovementDurationTime < (aMovementDurationEval[1] - 0.2) * TargetDistance / 10 + 0.2)
					{
						// good
						GoodFastFeedback();
						TrialEvaluation = EVAL_GOOD_SLOW;
					}
					else if (MovementDurationTime < (aMovementDurationEval[2] - 0.2) * TargetDistance / 10 + 0.2)
					{
						// great
						GreatFeedback();
						TrialEvaluation = EVAL_GREAT;
					}
					else if (MovementDurationTime < (aMovementDurationEval[3] - 0.2) * TargetDistance / 10 + 0.2)
					{
						// good
						GoodSlowFeedback();
						TrialEvaluation = EVAL_GOOD_FAST;	
					}
					else if (MovementDurationTime < MovementDurationTimeOut )
					{
						// too slow
						TooSlow();
						TrialEvaluation = EVAL_SLOW;
					}
					else
					{
						TrialEvaluation = EVAL_SLOW;
					}
				}
								
				printf("Movement finished: duration %.2lf seconds. Evaluation %d\n", MovementDurationTime, TrialEvaluation);
				
				StateNext(STATE_FINISH);
				//BeepDone();
				break;
			}

			// update peak speed
			if (PeakSpeed < RobotSpeed) PeakSpeed = RobotSpeed;

			if ( MovementDurationTimer.ExpiredSeconds(MovementDurationTimeOut) )
			{
			   StateNext(STATE_TIMEOUT);
			   break;
			}
			
			// check overshoot
			//if( CursorDistance(HomePosition) > TargetDistance + TargetTolerance + OvershootTolerance)
			//{
			//	OvershootFlag = 1;
			//}
   
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

	// Eye tracker finite state machine processing. (8)
    EYET_StateProcess();    	

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

			// Eye tracker calibration if required. (9)
			if( EyeTrackerFlag )
			{
				EYET_CalibrateStart(TRUE); // TRUE = test calibration afterwards.
				StateNext(STATE_EYETRACKER);
				break;
			}

			StateNext(STATE_SETUP);
			break;

        case STATE_SETUP :
			// Setup details of next trial, but only when robot stationary and active.
			//if( RobotNotMoving() && RobotActive() ) 
			if( RobotActive() ) // RobotNotMoving removed by shyeo
			{
				TrialSetup(); // load next row in TrialData, set a new home position

				PeakSpeed = 0.0; // reset peak speed to measure peak speed of coming trial

				StateNext(STATE_HOME);

				if (bGuidedHoming)
				{
					// minimum jerk homing timer;
					HomingTimer.Reset();
					//HomingStartPosition = RobotPosition;
					HomingStartPosition = CursorPosition;
					HomingDistance = norm(HomePosition - HomingStartPosition);
					HomingDuration = (HomingDistance / 20) + 0.2;
				}	

				OldCursorOffset = CursorOffset;
							
				// overshoot flag
				OvershootFlag = 0;
				
				// reset movement measurements
				MovementStartPosition.zeros();
				MovementFinishPosition.zeros();
				
			}
			break;

        case STATE_HOME :
           // Wait and start trial when robot in home position (and stationary and active).
    	   if( CursorHome() && RobotActive() )
           {
			   ForceFieldRamp.Down();
               StateNext(STATE_START);
               break;
           }
		   
		   if (!RobotActive() && bGuidedHoming)
		   {
				// reset minimum jerk homing timer;
				HomingTimer.Reset();
				HomingStartPosition = CursorPosition;
				HomingDistance = norm(HomePosition - HomingStartPosition);
				HomingDuration = (HomingDistance / 20) + 0.2;
				
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
           if( StateTimer.ExpiredSeconds(TrialDelay) ) // delay period ended
           {
				StateNext(STATE_GO);
				break;
           }

		   if( !CursorHome() || RobotSpeed > NotMovingSpeed ) // wait again until cursor is homed and not moving
		   //if( RobotSpeed > NotMovingSpeed )
           {
              StateTimer.Reset();
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
			   // Start force field.
			   ForceFieldStart();
				
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
			   printf("Cannot save Trial %d.\n", curTrialIndex1);
			   StateNext(STATE_EXIT);
			   break;
			}
			
			InterTrialDelayTimer.Reset();
			
			// Go to STATE_INTERTRIAL, if there is one. (fixed. Trial number increase at the beginngin of the trial
			if( curTrialIndex1 < Trials )
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

			// this is the very beginning of the next trial
			MessageClear();
			TrialNext();

			// Optional rest between blocks of trials.
			if( RestTrials != 0 )
			{
			   // Rest every X number of trials.
			   if( (curTrialIndex1 % RestTrials) == 0 && curTrialIndex1 != Trials )
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
			  TrialAbort(); // Abort the current trial.
			  MissTrial();  // Generate miss trial.
			  break;
 
			case STATE_MOVING : // should not enter here
				ErrorMessage("Trial Timed Out");
				TrialAbort(); // Abort the current trial.
				MissTrial();  // Generate miss trial.
				break;

			default :
			  ErrorMessage(STR_stringf("%s TimeOut",StateText[StateLast]));
			  break;
			}

			
			break;

        case STATE_ERROR :
			if( StateTimer.ExpiredSeconds(ErrorWait) )
			{
			   ErrorResume();
			}
			break;

        case STATE_REST :
			if( !StateTimer.ExpiredSeconds(RestWait) )
			{
				break;
			}

			// Eye tracker calibration if required. (10)
			if( EyeTrackerFlag )
			{
				EYET_CalibrateStart(TRUE);
				StateNext(STATE_EYETRACKER);
				break;
			}

			StateNext(STATE_SETUP);
			break;

		case STATE_EYETRACKER :
			// Eye tracker state, stay here until it becomes idle. (11)
			if( EYET_StateIdle() )
			{
				StateNext(STATE_SETUP);
				break;
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
   vector(1,1) = cos(D2R(angle));
   vector(2,1) = sin(D2R(angle));
   vector(3,1) = 0.0;

   return(vector);
}

/******************************************************************************/
void DrawMarker( matrix *pos, double radius, int color)
// draw an object with orientation (shyeo)
{
	glPushMatrix();
	GRAPHICS_Circle(pos,radius,color);
	glPopMatrix();
}

/******************************************************************************/
void DrawMarker( matrix *pos, double radius, int color, float alpha)
// draw an object with orientation (shyeo)
{
	glPushMatrix();
	GRAPHICS_Circle(pos,radius,color,alpha);
	glPopMatrix();
}

/******************************************************************************/

void DrawQuad( matrix quad )
{
	glBegin(GL_QUADS);
	glColor3f(255, 255, 255);
	glVertex3f(quad(1,1), quad(2,1), 0);
	glVertex3f(quad(3,1), quad(4,1), 0);
	glVertex3f(quad(5,1), quad(6,1), 0);
	glVertex3f(quad(7,1), quad(8,1), 0);
	glEnd();
}

/******************************************************************************/

void DrawScene( void )
{
static matrix posn(3,1);
static matrix quad(8,1);
int i,attr;
double sigma;
double xoffset, yoffset;
matrix CursorVisualPos(3,1);

// Display home position at start of trial.
	//if( (StateGraphics >= STATE_SETUP) && (StateGraphics <= STATE_MOVING) )
	//if( (StateGraphics >= STATE_HOME) && (StateGraphics <= STATE_MOVING) ) // changed by shyeo to fix a problem that shows previous home position before showing the new one, after discussing with James
	if(StateGraphics >= STATE_HOME && StateGraphics < STATE_GO)
	{
		//attr = (RobotHome() && StateTimer.ElapsedSeconds() >= 0.1) ? HomeColor : NotHomeColor;
		attr = (CursorHome() && StateTimer.ElapsedSeconds() >= 0.1) ? WHITE : GREY;
		DrawMarker(&HomePosition, HomeRadius + 0.05, attr);
		DrawMarker(&HomePosition, HomeRadius, BLACK);
	}
	
	if(StateGraphics >= STATE_GO && StateGraphics < STATE_MOVEWAIT)
	{
		DrawMarker(&HomePosition, HomeRadius, BLACK);
	}

	// Display target spheres when trial running.
	if( (StateGraphics >= STATE_GO) && (StateGraphics <= STATE_INTERTRIAL) )
	{
		// Display target for movement.
		//attr = RobotAt(TargetPosition, TargetTolerance) ? WHITE : YELLOW;
		attr = WHITE; // always yellow
		DrawMarker(&TargetPosition, TargetRadius + 0.05,attr);		
		DrawMarker(&TargetPosition, TargetRadius, BLACK);		
		
	}
	
	xoffset = 0;
	yoffset = 0;
	
	if( bCursorJump && StateGraphics != STATE_ERROR  && RobotFieldType == FIELD_VISUAL)
	{
		if (StateGraphics >= STATE_GO && StateGraphics <= STATE_MOVING)
		{
			if ( JumpTimer.ExpiredSeconds(JumpTimeOut) )
			{
				
				CursorJumpSTD = JumpSTDMax - ProjSpeed * JumpSTDSensitivity;
				if ( CursorJumpSTD < 0) CursorJumpSTD = 0;
				if ( CursorJumpSTD > JumpSTDMax) CursorJumpSTD = JumpSTDMax;
				
				CursorVisOffset(1,1) = RandomGaussian(0.0, CursorJumpSTD);
				CursorVisOffset(2,1) = RandomGaussian(0.0, CursorJumpSTD);
				JumpTimer.Reset();
			}
			
		}
		else
		{

			CursorVisOffset(1,1) = 0;
			CursorVisOffset(2,1) = 0;
			CursorVisOffset(3,1) = 0;
		}
	}
	
	// cursor position and alpha values should be implemented in RobotForcesFunction	
	if( (StateGraphics >= STATE_GO) && (StateGraphics < STATE_FINISH) )
	{
		CursorVisualPos = CursorPosition + CursorVisOffset;
		DrawMarker(&CursorVisualPos, CursorRadius, CursorColor, CursorAlpha);
	}
	else
	{
		DrawMarker(&CursorPosition, CursorRadius, CursorColor, 1.0); //show the result
	}
		
	
	// display useful informations for debugging
/*		if (DEBUG)
	{
		posn(1,1) = 0;
		posn(2,1) = 15;
		posn(3,1) = 0;
		
		GraphicsDisplayText(STR_stringf("xoffset: %1lf",xoffset), 0.5, posn);
		
		posn(2,1) = posn(2,1) - 1;
				
		GraphicsDisplayText(STR_stringf("yoffset: %1lf",yoffset), 0.5, posn);
		
		posn(2,1) = posn(2,1) - 1;
		
		GraphicsDisplayText(STR_stringf("std: %1lf",CursorJumpSTD), 0.5, posn);
		
		
		
	
		posn(2,1) = posn(2,1) - 1;
		
		GraphicsDisplayText(STR_stringf("ForceField: %s",FieldText[RobotFieldType]), 0.5, posn);

		posn(2,1) = posn(2,1) - 1;
		
		GraphicsDisplayText(STR_stringf("Velocity: [%.1lf %.1lf %.1lf]",RobotVelocity(1,1), RobotVelocity(2,1), RobotVelocity(3,1)), 0.5, posn);
		
		posn(2,1) = posn(2,1) - 1;
		
		GraphicsDisplayText(STR_stringf("alpha: %.2lf",CursorAlpha), 0.5, posn);
		
		posn(2,1) = posn(2,1) - 1;
		
		GraphicsDisplayText(STR_stringf("RobotFieldConstant: %.1lf",RobotFieldConstant), 0.5, posn);
		
		posn(2,1) = posn(2,1) - 1;
		
		GraphicsDisplayText(STR_stringf("RobotFieldAngle: %.1lf",RobotFieldAngle), 0.5, posn);
		
		posn(2,1) = posn(2,1) - 1;
		
		GraphicsDisplayText(STR_stringf("RobotFieldSensitivity: %.2lf",RobotFieldSensitivity), 0.5, posn);
		
	}
	else	*/
	{
		posn(1,1) = 28;
		posn(2,1) = 19;
		posn(3,1) = 0;
		
		GraphicsDisplayText(STR_stringf("%s",StateText[State]), 0.4, posn);
		
		posn(2,1) = posn(2,1) - 0.75;
		
		GraphicsDisplayText(STR_stringf("%d / %d", curTrialIndex1, Trials), 0.4, posn);
		
	}
}

/******************************************************************************/


void GraphicsDisplay( void )
{
int attr;
static matrix posn(3,1);


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

		// Display eye tracker graphics if we're in the eye tracker state. (12)
        if( StateGraphics == STATE_EYETRACKER )
        {
            EYET_GraphicsDisplay();
            continue;
        }

		// main drawing routine
		DrawScene();
		        
		
		
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
int i,item,attr,target;
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
		GRAPHICS_Sphere(&HomePosition,HomeRadius,HomeColor);
		GRAPHICS_Sphere(&TargetPosition,TargetRadius,TargetColor);
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
// Process FSM (StateProcess)
// update display using GraphicsDisplay or GraphicsDisplayTargetTest (when TargetTestFlag is on)
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
        draw = TRUE;
    }

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
// assign keys that are used during display
{
    // Process keyboard input.
    switch( key )
    {
		case ESC : 
          ProgramExit();
          break;
	  
		case 'c' : 
		case 'C' : 
			// Eye tracker calibration if 'C' key is pressed before a trial has started. (13)
			if( ((State == STATE_SETUP) || (State == STATE_HOME)) && EyeTrackerFlag )
			{
				EYET_CalibrateStart(TRUE);
				StateNext(STATE_EYETRACKER);
			}
		break;

		default :
			// Eye tracker keyboard call-back function. (14)
			EYET_KeyEvent(key);
    }
}

/******************************************************************************/

BOOL GraphicsStart( int argc, char *argv[] )
{
    // Start graphics window...
    if( !GRAPHICS_Start(GraphicsMode) )
    {
        printf("Cannot start GRAPHICS system.\n");
        return(FALSE);
    }

	// Set background ("clear") color...
    GRAPHICS_ClearColor(BLUE);
	
    // Set standard openGL parameters.
    //GRAPHICS_OpenGL(GRAPHICS_FLAG_NONE,BLACK);
	//GRAPHICS_OpenGL(GRAPHICS_FLAG_LIGHTING | GRAPHICS_FLAG_TRANSPARENCY,BLACK);
	GRAPHICS_OpenGL(GRAPHICS_FLAG_TRANSPARENCY,BLACK);

    return(TRUE);
}

/******************************************************************************/

void GraphicsMainLoop( void )
// call glut functions GrahpicsKeyboard, GraphicsDiaplay, GraphicsIdle 
// and enter glutMainLoop
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
// command line parser
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
            case 'C' : // config file
               ok = CMDARG_data(ConfigFile,data,CONFIG_FILES);
               break;

            case 'M' : // metaconfig file
               ok = CMDARG_data(MetaConfigFile,data);
               break;

            case 'D' : // data destination
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
// add variables to TrialData and FrameData
{
int i;
    // Patch High-Performance-Counter bug using LoopTask.
    LOOPTASK_TimerSkipPatch();
    
    // Open list of wave files.
    if( !WAVELIST_Open(WaveList) )
    {
        printf("WAVELIST: Cannot load WAV files.\n");
    }

    // Seed random number generator.
    randomize();

	
	printf("Add Variables to the TrialData matrix: ");
	
    // Add each variable to the TrialData matrix.
    TrialData.AddVariable("ExperimentTime",ExperimentTime);// Required subject wellbeing variable
    TrialData.AddVariable(VAR(TrialDelay));
    TrialData.AddVariable(VAR(TrialType));
	TrialData.AddVariable(VAR(FieldType));
    TrialData.AddVariable(VAR(FieldConstant));
    TrialData.AddVariable(VAR(FieldSensitivity));
	TrialData.AddVariable(VAR(FieldAngle));
    TrialData.AddVariable(VAR(PositionDelaySeconds));
    TrialData.AddVariable(VAR(TargetDistance));
	TrialData.AddVariable(VAR(TargetAngle));
	TrialData.AddVariable(VAR(TargetPosition));
    TrialData.AddVariable("MissTrials",MissTrials);        // Required subject wellbeing variable
    TrialData.AddVariable("TrialDuration",TrialDuration);  // Required subject wellbeing variable
    TrialData.AddVariable(VAR(MovementReactionTime));
    TrialData.AddVariable(VAR(MovementDurationTime));
	TrialData.AddVariable(VAR(HomePosition));
	TrialData.AddVariable(VAR(TrialEvaluation));
					
	TrialData.AddVariable(VAR(MovementStartPosition));
	TrialData.AddVariable(VAR(MovementFinishPosition));
	TrialData.AddVariable(VAR(CursorOffsetTrial)); // this is a fixed per trial, CursorOffset (used in real time) can be different (can be smoothly increasing to CursorOffsetTrial)
	TrialData.AddVariable(VAR(CursorVisOffset));
		
	
    // Set rows of TrialData to the number of trials (= Trials, which is counted from ConfigLoad)
    TrialData.SetRows(Trials); 

    // Add each variable to the FrameData matrix.
    FrameData.AddVariable(VAR(TrialTime));                 // Required subject wellbeing variable
    FrameData.AddVariable(VAR(State));
    FrameData.AddVariable(VAR(StateGraphics));
    FrameData.AddVariable(VAR(TriggerFlag));
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
	FrameData.AddVariable(VAR(CursorAlpha));
	FrameData.AddVariable(VAR(CursorJumpSTD));
				
   /* if( OptoTrakMarkers > 0 )
    {
        OptoTrakInitialize();
        FrameData.AddVariable(VAR(OptoTrakFrame));
        FrameData.AddVariable(VAR(OptoTrakPosition),OptoTrakMarkers);
    }*/
	
	// Eye tracker frame data variables if required. (15)
    if( EyeTrackerFlag )
    {
        FrameData.AddVariable(VAR(EyeTrackerFrameCount));
        FrameData.AddVariable(VAR(EyeTrackerTimeStamp));
        FrameData.AddVariable(VAR(EyeTrackerEyeXY),2);
    }


    // Set rows of FrameData to maximum.
    FrameData.SetRows(FRAMEDATA_ROWS);
	
	// minimum jerk trajectory for homing
	PassiveMinJerk=CreateMinimumJerk(1.0, 1.0, 10000);
		
	printf("DONE\n");
}

/******************************************************************************/
BOOL TrialList( void )
{
static matrix mPermuteBlock;
static matrix mPermuteAll;
static matrix mCombiPerBlock; // combination of parameters per block
static matrix mHomePosition(Trials,3);
static matrix mTargetDistance(Trials,1), mTargetAngle(Trials,1);

int		i,j,k, expStartIdx, expEndIdx, idx;
int		homeIdx, targetAngleIdx, targetDistanceIdx;
BOOL 	redo = TRUE;
BOOL 	ok;

double	randAngle;

	// computer number of trials for each phase
	expStartIdx = nPreExposureBlock * nTrialsPerBlock + 1;
	expEndIdx = (nPreExposureBlock + nExposureBlock) * nTrialsPerBlock;
	
			
	// build block that will be permuted
	mCombiPerBlock.dim(nTrialsPerBlock, 3);
	idx = 1;
	for (i = 1; i <= nHomePosition; i++)
		for (j = 1; j <= nTargetAngle; j++)
			for (k = 1; k <= nTargetDistance; k++)
			{
				mCombiPerBlock( idx, 1) = i; // HomePosition
				mCombiPerBlock( idx, 2) = j; // TargetAngle
				mCombiPerBlock( idx, 3) = k; // TargetDistance
				idx++;
			}

	// below will be counted later
	NullTrials		= 0;
	ExposureTrials	= 0;
	//CatchTrials		= 0; %no catch trials

	// permute index per block: the index will refer mCombiPerBlock
	mPermuteBlock.dim(nTrialsPerBlock, 1);
	for (i = 1; i <= nTrialsPerBlock; i++)
	{
		mPermuteBlock(i, 1) = i;
	}
	
	mPermuteAll.dim(Trials, 1);
	// build permuted index vector for full experiment
	for (i = 1; i <= nBlocks; i++)
	{
		mPermuteBlock = permute(mPermuteBlock);
		for (j = 1; j <= nTrialsPerBlock; j++ )
		{
			mPermuteAll( (i-1) * nTrialsPerBlock + j, 1) = mPermuteBlock(j, 1);
		}
	}
	
	// set home position based on the permuted indices
	for (i = 1; i <= Trials; i++)
	{
		idx = mPermuteAll(i, 1);
		homeIdx = mCombiPerBlock(idx, 1);
		
		targetDistanceIdx = mCombiPerBlock(idx, 3);
						
		mHomePosition(i,1)  = aHomePosition[(homeIdx - 1) * 3 + 0];
		mHomePosition(i,2)  = aHomePosition[(homeIdx - 1) * 3 + 1];
		mHomePosition(i,3)  = aHomePosition[(homeIdx - 1) * 3 + 2];
		
		targetAngleIdx = mCombiPerBlock(idx, 2);
		mTargetAngle(i, 1)  = aTargetAngle[targetAngleIdx - 1];
		
		targetDistanceIdx = mCombiPerBlock(idx, 3);
		mTargetDistance(i,1)  = aTargetDistance[targetDistanceIdx - 1];
	}
	
	// set exposure: these variable will be save to TrialData
	for ( i = 1; i <= Trials; i++ )
	{
		HomePosition(1, 1) 	= mHomePosition(i, 1);
		HomePosition(2, 1) 	= mHomePosition(i, 2);
		HomePosition(3, 1) 	= mHomePosition(i, 3);
		
		TargetDistance		= mTargetDistance( i, 1 );
		TargetAngle 		= mTargetAngle( i, 1 );
				
		if (i >= expStartIdx && i < expEndIdx)
		{
			TrialType 		= TRIAL_EXPOSURE;
			
			FieldType 		= FIELD_VISUAL;
			FieldConstant 	= AlphaConstant;
			FieldSensitivity	= AlphaSensitivity;
			FieldAngle 		= AlphaAngle;
						
			ExposureTrials++;
		}
		else // pre and after exposure
		{
			if ( i < expStartIdx )
			{
				TrialType 		= TRIAL_PRE_EXPO;
			}
			else
			{
				TrialType 		= TRIAL_POST_EXPO;
			}
				
		
			FieldType 		= FIELD_NONE;
			FieldConstant 	= 1.0;
			FieldSensitivity	=  0;
			FieldAngle 		= 0;
			
			NullTrials++;
		}
		
		// randomize CursorOffset
		randAngle = RandomUniform(0,360);
		CursorOffsetTrial = CursorOffsetMaxDistance * RandomUniform() * TargetAngleVector( randAngle );
						
		TrialData.RowSave( i );
	}

	printf("HomePosition:\n");
	disp(mHomePosition);
	
	printf("TargetAngle:\n");
	disp(mTargetAngle);
	
	printf("TargetDistance:\n");
	disp(mTargetDistance);
	
	printf("DONE\n");
				
    // Save trial list to file.
    ok = DATAFILE_Save(TrialListFile,TrialData);
    printf("%s %s Trials=%d.\n",TrialListFile,STR_OkFailed(ok),TrialData.GetRows());
	printf("%d null trials \n", NullTrials);
	printf("%d exposure trials \n", ExposureTrials);
	printf("%d catch trials \n", CatchTrials);
	
    // Reset trial number, etc.
    curTrialIndex1 = 0;
	TrialNext();
	
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

