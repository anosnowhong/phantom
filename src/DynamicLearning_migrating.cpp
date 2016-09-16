/******************************************************************************/
/*                                                                            */
/* MODULE  : oldBOT-OculusDemo.cpp                                            */
/*                                                                            */
/* PURPOSE : 3BOT and Oculus Rift HMD demo using OCULUS module in MOTOR.LIB   */
/*                                                                            */
/* DATE    : 05/Dec/2014                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 05/Dec/2014 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 12/Jan/2015 - Conversion to use new OCULUS module in MOTOR.LIB   */
/*                                                                            */
/* V1.2  JNI 21/Jan/2015 - OCULUS module uses traditional coordinate system.  */
/*                                                                            */
/* V1.3  JNI 06/Feb/2015 - Added graphics monitor window and text examples.   */
/*                                                                            */
/* V2.0  JNI 30/Sep/2015 - Version "oldBOT" for SangHoon Yeo / Big Phantom.   */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME "oldBOT-OculusMotor"

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

TIMER_Interval  GraphicsDrawLatencyTimer("GraphicsDrawLatency");
TIMER_Frequency GraphicsDrawFrequencyTimer("GraphicsDrawFrequency");
TIMER_Frequency GraphicsIdleFrequencyTimer("GraphicsIdleFrequency");

/******************************************************************************/

STRING RobotName = "";
int    RobotID = ROBOT_INVALID;

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////// migrated from DynamicLearning.cpp
#define CONFIG_FILES 256
int ConfigFiles = 0;
int ConfigIndex;
STRING ConfigFile[CONFIG_FILES];
STRING ConfigSaveFile;
STRING MetaConfigFile = "";

STRING DataName = "";
STRING DataFile = "";
STRING TrialListFile = "";

//STRING RobotName = "";
//int    RobotID = ROBOT_INVALID;
BOOL   RobotFT = FALSE;
double ForceMax = 100.0;
matrix RobotPosition(3, 1);
matrix RobotVelocity(3, 1);
matrix RobotForces(3, 1);
double RobotSpeed;
matrix HandleForces(3, 1);
matrix HandleTorques(3, 1);
double PhotoTransistor = 0.0;

double LoopTaskFrequency;
double LoopTaskPeriod;

matrix TextPosition(3, 1);
matrix HomePosition(3, 1);
double HomeTolerance = 0.50;
double HomeToleranceTime = 0.1;
double MovementReactionTimeOut = 0.5;
double MovementDurationTimeOut = 0.8;
double ErrorWait = 2.0;
double TrialDelay = 0.0;
double InterTrialDelay = 0.5;
int    RestTrials = 0;
double RestWait = 0.0;
double NotMovingSpeed = 0.5; // cm/sec
double NotMovingTime = 0.1;
TIMER  NotMovingTimer("NotMoving");

#define TARGETS 32
double  TargetAngles[TARGETS];
int TargetCount;
BOOL TargetTestFlag = FALSE;

matrix CursorPosition(3, 1);
int CursorColor = RED;
double CursorRadius = 0.5;

matrix ForceFieldForces(3, 1);
BOOL   ForceFieldStarted = FALSE;
matrix ForceFieldPosition(3, 1);
RAMPER ForceFieldRamp;
double ForceFieldRampTime = 0.1;

BOOL VisualFeedback;

int HomeColor = WHITE;
int NotHomeColor = GREY;
double HomeRadius = 1.0;

int TargetColor = YELLOW;
double TargetRadius = 1.0;

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

double GraphicsVerticalRetraceSyncTime = 0.0;    // Time (sec) before vertical retrace to draw graphics frame
double GraphicsVerticalRetraceCatchTime = 0.05;  // Time (msec) to devote to catching vertical retrace
TIMER  GraphicsTargetTimer("GraphicsTarget");
matrix GraphicsSyncPosition(3, 1);
double GraphicsSyncRadius = 0.5;
int    GraphicsSyncColor = WHITE;

BOOL   TriggerFlag = FALSE;

//int    GraphicsMode=GRAPHICS_DISPLAY_2D;
int    GraphicsMode = GRAPHICS_DISPLAY_DEFAULT;

STRING GraphicsModeString = "";

STRING GraphicsString = "ULALALALA";

// List of WAV files for beeping.
struct WAVELIST WaveList[] =
{
	{ "HighBip", "HIGH_BIP.WAV", NULL },
	{ "LowBip", "LOW_BIP.WAV", NULL },
	{ "Bip", "MID_BIP.WAV", NULL },
	{ "", "", NULL },
};

TIMER_Interval WaveListPlayInterval("WaveListPlay");

#define FRAMEDATA_ROWS 10000
MATDAT FrameData("FrameData");
BOOL   FrameRecord = FALSE;

TIMER  MovementDurationTimer("MovementDuration");
TIMER  MovementReactionTimer("MovementReaction");
TIMER  MovementFinishedTimer("MovementFinished");

TIMER  TrialTimer("Trial");
TIMER  InterTrialDelayTimer("InterTrialDelay");
double TrialTime;
double TrialDuration = 0.0;
int    Trial;
BOOL   TrialRunning = FALSE;

// Trial types.
#define TRIAL_NULL      0
#define TRIAL_EXPOSURE  1
#define TRIAL_CATCH     2

// Field types.
#define FIELD_NONE      0
#define FIELD_VISCOUS   1
#define FIELD_VISMOTROT 2
#define FIELD_CHANNEL   3

int    Trials = 0;
int    TotalTrials;
int    TotalNullTrials = 0;
int    TotalExposureTrials = 0;
int    TotalCatchTrials = 0;

int    ExposureTrials[2] = { 0, 10000 };
int    ExposureFieldType = FIELD_NONE;
double ExposureFieldConstant = 0.0;
double ExposureFieldAngle = 0.0;

int    CatchTrials[3] = { 0, 0, 0 };
double CatchTargetAngles[TARGETS];
int    CatchTargetCount;
int    CatchFieldType = FIELD_NONE;
double CatchFieldConstant = 0.0;
double CatchFieldAngle = 0.0;

// Permute list objects to randomize targets.
PERMUTELIST TargetPermute;
PERMUTELIST CatchTargetPermute;

int    NullTrial = 0;
int    ExposureTrial = 0;
int    CatchTrial = 0;

MATDAT TrialData("TrialData");

// Trial data variables.
int    TrialType;
int    FieldType;
double FieldConstant;
double FieldAngle;
matrix TargetPosition(3, 1);
int    TargetIndex;
double TargetAngle;
double TargetDistance;
int    MissTrials = 0;
int    MissTrialsTotal = 0;
double MissTrialsPercent = 0.0;
double MovementReactionTime = 0.0;
double MovementDurationTime = 0.0;
matrix StartPosition(3, 1);
matrix FinishPosition(3, 1);

double PositionDelaySeconds = 0.0;
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

int State = STATE_INITIALIZE;
int StateLast;
int StateGraphics = STATE_INITIALIZE;
int StateGraphicsLast;
char *StateText[] = { "Initialize", "Setup", "Home", "Start", "Delay", "Go", "TargetWait", "MoveWait", "Moving", "Finish", "InterTrial", "Exit", "TimeOut", "Error", "Rest" };
BOOL StateLoopTask[STATE_MAX] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };
TIMER StateTimer("State");
TIMER StateGraphicsTimer("StateGraphics");
int StateErrorResume;
///////////////////////////////////////////////////////////////////////////////////////////////
////// START START START START START START START  //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

//matrix RobotPosition(3, 1);             // cm
matrix RobotFakePosition(3, 1);
matrix original_point(3, 1, 0.0);
matrix target_point(3, 1, 3.0);

matrix RobotPositionRaw(3, 1);          // cm
matrix RobotPositionOffset(3, 1);       // cm
//matrix RobotVelocity(3, 1);             // cm/sec
//matrix RobotForces(3, 1);               // N
double RobotForceMax = 5.0;              // N
double RobotSpringConstant = -1.0;       // N/cm
double RobotForceMagnitude = 0.0;        // N

TIMER_Frequency RobotLoopFrequencyTimer("RobotLoopFrequency");
TIMER_Interval  RobotLoopLatencyTimer("RobotLoopLatency");

matrix SpherePosition(3, 1);
matrix SpherePosition_Goal(3, 1);
double SphereRadius = 5.0; // cm
int    SphereColor = GREEN;
BOOL   SphereInsideFlag = FALSE;

//double CursorRadius = 1.0; // cm
//int    CursorColor = RED;

//double TargetRadius = 2.5; // cm
//int	   TargetColor = TURQUOISE;

// Graphics Monitor Window variables.
int    MonitorX = 100;     // Monitor window X-position (pixels)
int    MonitorY = 100;     // Monitor window Y-position (pixels)
int    MonitorWid = 480;   // Monitor window width (pixels)
int    MonitorViews = 4;   // Monitor window views (can be 1 or 4).
double MonitorFreq = 60.0; // Monitor window display rate (Hz)

// Text Display variables.
//matrix TextPosition(3, 1);
matrix TextRotation(3, 1);
int    TextColor = WHITE;
float  TextSize = 0.75;

/******************************************************************************/

void ProgramExit(void);
void RobotStop(void);

/******************************************************************************/

void GraphicsTimingResults(void)
{
	printf("----------------------------------------------------------------\n");

	GraphicsDrawFrequencyTimer.Results();
	GraphicsDrawLatencyTimer.Results();
	GraphicsIdleFrequencyTimer.Results();

	GRAPHICS_TimingResults();
	GRAPHICS_AdaptiveDisplayResults();

	printf("----------------------------------------------------------------\n");
}

/******************************************************************************/

void StateProcess(void)
{
	double d;

	// Check that robot is in a safe state.
	if (!ROBOT_Safe(ROBOT_ID))
	{
		printf("Robot not safe.\n");
		ProgramExit();
	}

	// Special processing while a trial is running.
	if (TrialRunning)
	{
		if (!RobotActive())
		{
			// If robot is not active, abort current trial.
			ErrorRobotInactive();
			TrialAbort();
			MissTrial();
		}
		else
			if (FrameData.Full())
			{
				// Abort current trail if frame data is full.
				ErrorFrameDataFull();
				TrialAbort();
				MissTrial();
			}
	}

	// Some states are processing in the LoopTask.
	if (StateLoopTask[State])
	{
		return;
	}

	// State processing.
	switch (State)
	{
	case STATE_INITIALIZE:
		// Initialization state.
		if (TargetTestFlag)
		{
			break;
		}

		ExperimentTimer.Reset();
		StateNext(STATE_SETUP);
		break;

	case STATE_SETUP:


		// Setup details of next trial, but only when robot stationary and active.


		if (RobotNotMoving() && RobotActive())
		{
			printf("Dynamic learning: before Trialsetup i am here=====!!!!\n");
			TrialSetup();
			StateNext(STATE_HOME);
		}
		break;

	case STATE_HOME:
		// Start trial when robot in home position (and stationary and active).
		if (RobotNotMoving() && RobotHome() && RobotActive())
		{
			StateNext(STATE_START);
			break;
		}
		break;

	case STATE_START:
		// Start trial.
		TrialStart();
		StateNext(STATE_DELAY);
		break;

	case STATE_DELAY:
		// Delay period before go signal.
		if (StateTimer.ExpiredSeconds(TrialDelay))
		{
			StateNext(STATE_GO);
			break;
		}

		if (MovementStarted())
		{
			ErrorMoveTooSoon();
			TrialAbort();
			MissTrial();
		}
		break;

	case STATE_GO:
		// Wait until graphics state matches.
		if (State != StateGraphics)
		{
			break;
		}

		StateNext(STATE_TARGETWAIT);
		break;

	case STATE_TARGETWAIT:
		// Wait for estimated target display delay.
		if (GraphicsTargetTimer.ExpiredSeconds(GRAPHICS_DisplayDelayTarget(TargetPosition)))
		{
			// Target should be displayed now (within a few milliseconds).
			TriggerOn();
			MovementReactionTimer.Reset();
			BeepGo();
			StateNext(STATE_MOVEWAIT);
		}
		break;

	case STATE_MOVEWAIT:
		// Process in the robot forces function (LoopTask)
		break;

	case STATE_MOVING:
		// Process in the robot forces function (LoopTask)
		break;

	case STATE_FINISH:
		// Trial has finished so stop trial.
		TrialStop();

		// Save the data for this trial.
		if (!TrialSave())
		{
			printf("Cannot save Trial %d.\n", Trial);
			StateNext(STATE_EXIT);
			break;
		}

		// Catch too-slow trials.
		if (MovementDurationTime >= MovementDurationTimeOut)
		{
			ErrorMoveTimeOut();
			BeepError();
			InterTrialDelayTimer.Reset();
		}

		// Go to next trial, if there is one.
		if (TrialNext())
		{
			StateNext(STATE_INTERTRIAL);
		}
		else
		{
			StateNext(STATE_EXIT);
		}
		break;

	case STATE_INTERTRIAL:
		// Wait for the intertrial delay to expire.
		if (!InterTrialDelayTimer.ExpiredSeconds(InterTrialDelay))
		{
			break;
		}

		MessageClear();

		// Optional rest between blocks of trials.
		if (RestTrials != 0)
		{
			// Rest every X number of trials.
			if ((Trial % RestTrials) == 0)
			{
				StateNext(STATE_REST);
				break;
			}
		}

		StateNext(STATE_SETUP);
		break;

	case STATE_EXIT:
		ProgramExit();
		break;

	case STATE_TIMEOUT:
		switch (StateLast) // Which state had the timeout?
		{
		case STATE_MOVEWAIT:
			ErrorMoveWaitTimeOut();
			break;

		case STATE_MOVING:
			ErrorMoveTimeOut();
			break;

		default:
			ErrorMessage(STR_stringf("%s TimeOut", StateText[StateLast]));
			break;
		}

		TrialAbort(); // Abort the current trial.
		MissTrial();  // Generate miss trial.
		break;

	case STATE_ERROR:
		if (StateTimer.ExpiredSeconds(ErrorWait))
		{
			ErrorResume();
		}
		break;

	case STATE_REST:
		if (StateTimer.ExpiredSeconds(RestWait))
		{
			StateNext(STATE_SETUP);
		}
		break;
	}
}

/******************************************************************************/

void GraphicsDisplayTargetTest(void)
{
	int item, attr, target;
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
		GRAPHICS_Sphere(&posn, HomeRadius, HomeColor);

		for (target = 0; (target < TargetCount); target++)
		{
			posn = HomePosition + (TargetAngleVector(TargetAngles[target]) * TargetDistance);
			GRAPHICS_Sphere(&posn, TargetRadius, TargetColor);
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

void GraphicsIdle(void)
{
	GraphicsIdleFrequencyTimer.Loop();

	StateProcess();

	StateGraphicsNext(State); // Safe to set graphics state at this point.
	if (TargetTestFlag)
	{
		GraphicsDisplayTargetTest();
	}
	else
	{
		GraphicsDisplay();
	}

	Sleep(0);
	/*
	// Text displaying force magnitude.
	if (RobotForceMagnitude > 0.0)
	{
		GRAPHICS_TextSet(STR_stringf("Force=%.1lfN", RobotForceMagnitude));
	}
	else
	{
		GRAPHICS_TextClear();
	}
	*/
}

/******************************************************************************/
/* //this was replaced by GrahpicsDisplay from DynamicLearning
   //TODO: once working, need to think about latency issue
void GraphicsDraw(void)
{
	GraphicsDrawFrequencyTimer.Loop();
	GraphicsDrawLatencyTimer.Before();

	// Fixed object.
	GRAPHICS_WireSphere(&SpherePosition, SphereRadius, SphereColor);
	//GRAPHICS_WireSphere(&SpherePosition_Goal,SphereRadius,SphereColor);
	// Robot cursor.
	GRAPHICS_Sphere(&RobotPosition, CursorRadius, CursorColor);
	GRAPHICS_Sphere(&original_point, CursorRadius, YELLOW);

	//BBX
	GRAPHICS_WireSphere(&SpherePosition, SphereRadius, SphereColor);
	//GRAPHICS_Sphere(&RobotFakePosition,CursorRadius,CursorColor);

	GraphicsDrawLatencyTimer.Before();
}
*/
/******************************************************************************/

void GraphicsKeyboard(BYTE key, int x, int y)
{
	// Process keyboard input.
	if (key == ESC)
	{
		ProgramExit();
	}
}

/******************************************************************************/
/*
void GraphicsStart(void)
{
	// Start the graphics system.
	if (!GRAPHICS_GraphicsStart())
	{
		printf("Cannot start graphics system.\n");
		return;
	}

	// Start the graphics monitor window.
	if (!GRAPHICS_MonitorStart(MonitorX, MonitorY, MonitorWid, MonitorViews, MonitorFreq))
	{
		printf("Cannot start graphics monitor window.\n");
		return;
	}

	// Default position for text display.
	TextPosition.zeros();      // Centred in the workspace above the green sphere.

	// Default rotation for text display.
	TextRotation.zeros();
	TextRotation(1, 1) = 40.0;  // Rotate text forward (x-axis) for better visibility.

	// Set default values for text display.
	GRAPHICS_TextDefault(&TextPosition, &TextRotation, TextColor, TextSize);

	// Register call-back functions and pass control to graphics system.
	GRAPHICS_MainLoop(GraphicsKeyboard, GraphicsDraw, GraphicsIdle);
}
*/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/***END  END  END  END  END  END  END  END  ***********************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void GraphicsDisplayText(char *string, float size, matrix &pos)
{
	static matrix p;
	void *font = GLUT_STROKE_MONO_ROMAN;
	int i, w;
	float s = size*0.05;

	p = pos;
	w = strlen(string);

	p(1, 1) -= (w / 2) * (s * 100.0);
	p(2, 1) += 100;

	glPushMatrix();

	glLineWidth(2.0);
	translate(p);
	glRotated(90, 1.0, 0.0, 0.0);
	glScalef(s, s, 1);
	

	GRAPHICS_ColorSet(WHITE);

	for (i = 0; (i < w); i++)
		glutStrokeCharacter(font, string[i]);

	glPopMatrix();
}

void GraphicsDisplayText(void)
{
	static matrix P(3, 1);

	if (!STR_null(GraphicsString))
	{
		P = TextPosition;
		P(2, 1) -= 3.0;
		GraphicsDisplayText(GraphicsString, 1.0, P);
	}
}

double RobotDistance(matrix &home)
{
	double distance;

	distance = norm(RobotPosition - home);

	return(distance);
}

BOOL RobotHome(matrix &home, double tolerance)
{
	BOOL flag = FALSE;

	if (RobotDistance(home) <= tolerance)
	{
		flag = TRUE;
	}

	return(flag);
}


BOOL RobotHome(void)
{
	BOOL flag;

	flag = RobotHome(HomePosition, HomeTolerance);

	return(flag);
}

void GraphicsScene(void) // this is created by shyeo, isolating the gl routines only from GraphicsDisplay
{
	int attr;
	static matrix posn;

	// Display text.
	strncpy(GraphicsString, StateText[StateGraphics], STRLEN);
	GraphicsDisplayText();
	
	// Display rotating teapot during rest period.
	if (StateGraphics == STATE_REST)
	{
		glPushMatrix();
		GRAPHICS_ColorSet(GREEN);
		glRotated(ExperimentTimer.ElapsedSeconds() * 10.0 * PI, 1.0, 1.0, 1.0);
		glutWireTeapot(10.0);
		glPopMatrix();
		return;
	}
	
	// Display home position at start of trial.
	if ((StateGraphics >= STATE_SETUP) && (StateGraphics <= STATE_MOVING))
	{
		attr = RobotHome() ? HomeColor : NotHomeColor;
		//GRAPHICS_Circle(&StartPosition, HomeRadius, attr);
		GRAPHICS_Sphere(&StartPosition, HomeRadius, attr);
	}
	
	// Display target spheres when trial running.
	if ((StateGraphics >= STATE_GO) && (StateGraphics <= STATE_FINISH))
	{
		// Display target for movement.
		GRAPHICS_Sphere(&TargetPosition, TargetRadius, TargetColor);

		// Display graphics sync target for phototransistor.
		if (!GraphicsSyncPosition.iszero())
		{
			GRAPHICS_Sphere(&GraphicsSyncPosition, GraphicsSyncRadius, GraphicsSyncColor);
		}
	}

	// Display finish position.
	if ((StateGraphics > STATE_MOVING) && (StateGraphics <= STATE_INTERTRIAL))
	{
		attr = RobotHome() ? HomeColor : NotHomeColor;
		GRAPHICS_Sphere(&FinishPosition, HomeRadius, attr);
	}

	// Display robot position cursor.
	if ((StateGraphics != STATE_ERROR) && (VisualFeedback || RobotHome()))
	{
		posn = CursorPosition;
		posn(3, 1) += 2.0*HomeRadius;
		GRAPHICS_Sphere(&posn, CursorRadius, CursorColor);
	}
}


void GraphicsDisplay(void)
{
	/*
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

		
	}
	
	GraphicsScene();

	// Mark time now that scene has been drawn.
	GraphicsDisplayLatency.After();
	
	
	// Display the graphics buffer we've just drawn.
	GraphicsSwapBufferLatency.Before();
	GRAPHICS_SwapBuffers();
	GraphicsSwapBufferLatency.After();

	// Mark time for display frequency.
	GraphicsDisplayFrequency.Loop();
	*/

	
	// just use oculus_demo
	GraphicsDrawFrequencyTimer.Loop();
	GraphicsDrawLatencyTimer.Before();

	GraphicsScene();

	GraphicsDrawLatencyTimer.Before();
	
}


BOOL GraphicsStart()
{
	printf("-----------------------------GraphicsStart entered\n");
	/*
	// Start graphics window...
	if (!GRAPHICS_Start(GraphicsMode))
		//if( !GRAPHICS_GraphicsStart() )
	{
		printf("Cannot start GRAPHICS system.\n");
		return(FALSE);
	}

	// Set standard openGL parameters.
	GRAPHICS_OpenGL(GRAPHICS_FLAG_NONE, BLACK);
	*/

	// Start the graphics system.
	if (!GRAPHICS_GraphicsStart())
	{
		printf("Cannot start graphics system.\n");
		return(TRUE);
	}

	// Start the graphics monitor window.
	if (!GRAPHICS_MonitorStart(MonitorX, MonitorY, MonitorWid, MonitorViews, MonitorFreq))
	{
		printf("Cannot start graphics monitor window.\n");
		return(TRUE);
	}
	printf("-----------------------------GraphicsStart exit\n");

	return(TRUE);
	
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void GraphicsMainLoop(void)
{
	printf("-----------------------------GraphicsMainLoop entered\n");
	/*// Set various GLUT call-back functions.
	glutKeyboardFunc(KB_GLUT_KeyboardFuncInstall(GraphicsKeyboard));
	glutDisplayFunc(GraphicsDisplay);
	glutIdleFunc(GraphicsIdle);

	// Reset frequency timing objects.
	GraphicsDisplayFrequency.Reset();
	GraphicsIdleFrequency.Reset();

	// Give control to GLUT's main loop.
	glutMainLoop();
	*/
	//GRAPHICS_MainLoop(GraphicsKeyboard, GraphicsDraw, GraphicsIdle);
	GRAPHICS_MainLoop(GraphicsKeyboard, GraphicsDisplay, GraphicsIdle);
	
}


/******************************************************************************/

void RobotForcesFunction(matrix &P, matrix &V, matrix &F)
{
	int i;
	static matrix D(3, 1);
	static double d, e;

	RobotLoopFrequencyTimer.Loop();
	RobotLoopLatencyTimer.Before();

	RobotPositionRaw = P;

	RobotPosition(1, 1) = P(1, 1);
	RobotPosition(2, 1) = P(2, 1);
	RobotPosition(3, 1) = P(3, 1);

	//Translate cursor 
	RobotPosition(2, 1) += 43.5;
	RobotPosition(3, 1) -= 51.1;

	//After change the calibration file, need restore it here incase the cursor too far away
	//1.589987159,36.23830414,-57.38533783
	/*
	RobotPosition(1, 1) -= 1.589987159;
	RobotPosition(2, 1) += 79.7383041;
	RobotPosition(3, 1) -= 108.485338;
	*/

	RobotVelocity = V;
	RobotForces.zeros();

	CursorPosition = RobotPosition;

	D = RobotPosition - SpherePosition;
	d = norm(D);
	e = d - SphereRadius; // Scalar encroachment into surface of sphere.

	if (ROBOT_JustActivated(RobotID))
	{
		SphereInsideFlag = (e < 0);
	}

	if (SphereInsideFlag)
	{
		e = d - (SphereRadius - CursorRadius);
	}
	else
	{
		e = d - (SphereRadius + CursorRadius);
	}

	if (ROBOT_Activated(RobotID) && ((SphereInsideFlag && (e > 0.0)) || (!SphereInsideFlag && (e < 0.0))))
	{
		RobotForces = RobotSpringConstant * e * (D / d);
	}

	RobotForces.clampnorm(RobotForceMax);
	RobotForceMagnitude = norm(RobotForces);

	F = RobotForces;

	RobotLoopLatencyTimer.After();
}

/******************************************************************************/
/* // this is replaced by DeviceStart in tje origianl DynamicLearning
BOOL RobotStart(void)
{
	BOOL ok = FALSE;

	if ((RobotID = ROBOT_Open(RobotName)) == ROBOT_INVALID)
	{
		printf("Cannot open: \n", RobotName);
	}
	else
		if (!ROBOT_Start(RobotID, RobotForcesFunction))
		{
			printf("Cannot start: %s\n", RobotName);
		}
		else
		{
			ok = TRUE;
			printf("Robot started: %s\n", RobotName);
		}

	return(ok);
}
*/
/******************************************************************************/
void DeviceStop(void)
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
BOOL DeviceStart(void)
{
	BOOL ok = TRUE;

	/*
	if( OptoTrakMarkers > 0 )
	{
	ok = OptoTrakStart();
	printf("OptoTrak start: %s.\n",STR_OkFailed(ok));
	}
	*/

	if (!ok)
	{
		return(FALSE);
	}

	// Open and start robot.
	if ((RobotID = ROBOT_Open(RobotName)) == ROBOT_INVALID)
	{
		printf("%s: Open failed.\n", RobotName);
		ok = FALSE;
	}
	else
		if (!ROBOT_Start(RobotID, RobotForcesFunction))
		{
			printf("%s: Start failed.\n", RobotName);
			ok = FALSE;
		}
		else
			if (!ROBOT_SensorOpen(RobotID))
			{
				printf("%s: Cannot open sensor(s).\n", RobotName);
				ok = FALSE;
			}
			else
				if (RobotFT)
				{
					if (!ROBOT_SensorOpened_DAQFT())
					{
						printf("%s: F/T sensor not opened.\n", RobotName);
						RobotFT = FALSE;
					}
				}

	if (ok)
	{
		if (!ForceFieldRamp.Open(ForceFieldRampTime))
		{
			printf("ForceFieldRamp: Open failed.\n");
			ok = FALSE;
		}
		else
			if (!ForceFieldRamp.Start())
			{
				printf("ForceFieldRamp: Start failed.\n");
				ok = FALSE;
			}
			else
			{
				ForceFieldRamp.Down();
			}
	}

	if (!ok)
	{
		DeviceStop();
		return(FALSE);
	}

	printf("%s: Started.\n", RobotName);

	// Reset bias of F/T sensor if required.
	if (RobotFT)
	{
		printf("Press any key to reset bias of F/T sensor(s)...\n");
		while (!KB_anykey());

		ok = ROBOT_SensorBiasReset_DAQFT(RobotID);
		printf("%s: F/T sensor bias reset: %s.\n", RobotName, STR_OkFailed(ok));
	}

	LoopTaskFrequency = ROBOT_LoopTaskGetFrequency(RobotID);
	LoopTaskPeriod = ROBOT_LoopTaskGetPeriod(RobotID);

	return(ok);
}
/******************************************************************************/

void RobotStop(void)
{
	ROBOT_Stop(RobotID);
	ROBOT_Close(RobotID);
}

/******************************************************************************/

void RobotTimingResults(void)
{
	RobotLoopFrequencyTimer.Results();
	RobotLoopLatencyTimer.Results();
}

/******************************************************************************/

void ProgramExit(void)
{
	// Stop and close the 3BOT and the Oculus HMD.
	RobotStop();
	GRAPHICS_Stop();

	// Timing results.
	RobotTimingResults();
	GraphicsTimingResults();

	// Exit program.
	exit(0);
}

/******************************************************************************/

void Usage(void)
{
	printf("Usage:\n\n");
	printf("%s /R:RobotName\n", MODULE_NAME);

	exit(0);
}

/******************************************************************************/

BOOL Parameters(int argc, char *argv[])
{
	char *data;
	BOOL ok;
	int i;

	printf("\n");

	// Loop over each command-line parameter.
	for (ok = TRUE, i = 1; ((i < argc) && ok); i++)
	{
		// Process the parameter's code.
		switch (CMDARG_code(argv[i], &data))
		{
		case 'C':
			ok = CMDARG_data(ConfigFile, data, CONFIG_FILES);
			break;

		case 'M':
			ok = CMDARG_data(MetaConfigFile, data);
			break;

		case 'D':
			if (!CMDARG_data(DataName, data, STRLEN))
			{
				ok = FALSE;
			}
			else
				if (strstr(DataName, ".") != NULL)
				{
					ok = FALSE;
				}
				else
					if (STR_null(DataName))
					{
						ok = FALSE;
					}
					else
					{
						strncpy(DataFile, STR_stringf("%s.DAT", DataName), STRLEN);
						strncpy(ConfigSaveFile, STR_stringf("%s.CFG", DataName), STRLEN);
						strncpy(TrialListFile, STR_stringf("%s-L.DAT", DataName), STRLEN);
					}
			break;

		case 'T':
			TargetTestFlag = TRUE;
			break;

		case '?':
			Usage();
			break;

		default:
			ok = FALSE;
			break;
		}

		if (!ok)
		{
			printf("Invalid argument: %s\n", argv[i]);
		}
	}

	if (!ok)
	{
		return(FALSE);
	}

	// Multiple config files or a meta-config file which specifies multiple files.
	if (!STR_null(MetaConfigFile))
	{
		if (!METACONFIG_Process(MetaConfigFile, ConfigFiles, ConfigFile))
		{
			printf("Cannot load meta-configuration file: %s\n", MetaConfigFile);
			ConfigFiles = 0;
		}
	}
	else
	{
		for (ConfigFiles = 0; (ConfigFiles < CONFIG_FILES); ConfigFiles++)
		{
			if (STR_null(ConfigFile[ConfigFiles]))
			{
				break;
			}
		}
	}

	if (ConfigFiles == 0)
	{
		printf("Configuration file(s) not specified.\n");
		return(FALSE);
	}

	if (STR_null(DataName))
	{
		printf("Data file not specified.\n");
		return(FALSE);
	}

	for (i = 0; (i < ConfigFiles); i++)
	{
		printf("%d %s\n", i, ConfigFile[i]);
	}

	return(TRUE);
}



/******************************************************************************/
void ConfigSetup(void)
{
	// Reset configuration variable list.
	CONFIG_reset();

	// Set up variable list for configuration.
	CONFIG_set(VAR(RobotName));
	CONFIG_setBOOL(VAR(RobotFT));
	CONFIG_set(VAR(ForceMax));
	CONFIG_set("GraphicsMode", GraphicsModeString);
	CONFIG_set("GraphicsSyncTime", GraphicsVerticalRetraceSyncTime);
	CONFIG_set("GraphicsCatchTime", GraphicsVerticalRetraceCatchTime);
	CONFIG_set("GraphicsSyncPos", GraphicsSyncPosition);
	CONFIG_set(VAR(TextPosition));
	CONFIG_set(VAR(CursorRadius));
	CONFIG_set(VAR(TargetDistance));
	CONFIG_set(VAR(TargetAngles), TARGETS);
	CONFIG_set(VAR(TargetRadius));
	CONFIG_set(VAR(HomeRadius));
	CONFIG_set(VAR(HomePosition));
	CONFIG_set(VAR(HomeTolerance));
	CONFIG_set(VAR(HomeToleranceTime));
	CONFIG_set(VAR(MovementReactionTimeOut));
	CONFIG_set(VAR(MovementDurationTimeOut));
	CONFIG_set("PositionDelay", PositionDelaySeconds);
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

	CONFIG_label(VAR(ExposureTrials), 2);
	CONFIG_set("FieldType", ExposureFieldType);
	CONFIG_set("FieldConstant", ExposureFieldConstant);
	CONFIG_set("FieldAngle", ExposureFieldAngle);

	CONFIG_label(VAR(CatchTrials), 3);
	CONFIG_set("TargetAngles", CatchTargetAngles, TARGETS);
	CONFIG_set("FieldType", CatchFieldType);
	CONFIG_set("FieldConstant", CatchFieldConstant);
	CONFIG_set("FieldAngle", CatchFieldAngle);
}

/******************************************************************************/

void ConfigInit(void)
{
	int i;

	for (i = 0; (i < TARGETS); i++)
	{
		TargetAngles[i] = 360.0;
		CatchTargetAngles[i] = 360.0;
	}

	TargetCount = 0;
	CatchTargetCount = 0;
}

/******************************************************************************/

BOOL ConfigLoad(char *file)
{
	int i;
	BOOL ok = TRUE;

	// Setup and initialize configuration variables.
	ConfigSetup();
	ConfigInit();

	// Load configuration file.
	if (!CONFIG_read(file))
	{
		printf("ConfigLoad(%s) Cannot read file.\n", file);
		return(FALSE);
	}

	// Set graphics mode (if specified).
	if (!STR_null(GraphicsModeString))
	{
		if (!GRAPHICS_DisplayMode(GraphicsMode, GraphicsModeString))
		{
			ok = FALSE;
		}
	}

	// Count the number of targets.
	for (TargetCount = 0; ((TargetAngles[TargetCount] < 360.0) && (TargetCount < TARGETS)); TargetCount++);

	if (TargetCount == 0)
	{
		printf("%s: No targets specified.\n", file);
		ok = FALSE;
	}

	// Count the number of catch trial targets (if specified).
	for (CatchTargetCount = 0; ((CatchTargetAngles[CatchTargetCount] < 360.0) && (CatchTargetCount < TARGETS)); CatchTargetCount++);

	if (STR_null(RobotName))
	{
		printf("No robot specified.\n");
		ok = FALSE;
	}

	printf("ConfigLoad(%s) Load %s.\n", file, STR_OkFailed(ok));
	CONFIG_list(printf);

	printf("TargetCount=%d\n", TargetCount);
	for (i = 0; (i < TargetCount); i++)
	{
		printf("%d: %.1lf (deg)\n", i, TargetAngles[i]);
	}

	printf("TargetCount=%d\n", TargetCount);
	for (i = 0; (i < TargetCount); i++)
	{
		printf("%d: %.1lf (deg)\n", i, TargetAngles[i]);
	}

	printf("CatchTargetCount=%d\n", CatchTargetCount);
	for (i = 0; (i < CatchTargetCount); i++)
	{
		printf("%d: %.1lf (deg)\n", i, CatchTargetAngles[i]);
	}

	return(ok);
}

/******************************************************************************/

void Initialize(void)
{
	// Patch High-Performance-Counter bug using LoopTask.
	LOOPTASK_TimerSkipPatch();

	// Open list of wave files.
	if (!WAVELIST_Open(WaveList))
	{
		printf("WAVELIST: Cannot load WAV files.\n");
	}

	// Seed random number generator.
	randomize();

	// Add each variable to the TrialData matrix.
	TrialData.AddVariable("ExperimentTime", ExperimentTime);// Required subject wellbeing variable
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
	TrialData.AddVariable("MissTrials", MissTrials);        // Required subject wellbeing variable
	TrialData.AddVariable("TrialDuration", TrialDuration);  // Required subject wellbeing variable
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
	FrameData.AddVariable("GraphicsSwapBuffersCount", GRAPHICS_SwapBuffersCount);
	FrameData.AddVariable("GraphicsSwapBuffersToVerticalRetraceTime", GRAPHICS_SwapBuffersToVerticalRetraceTime);
	FrameData.AddVariable("GraphicsTimeUntilVerticalRetraceOnset", GRAPHICS_VerticalRetraceNextOnsetTime);
	FrameData.AddVariable("GraphicsTimeUntilVerticalRetraceOffset", GRAPHICS_VerticalRetraceNextOffsetTime);
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
matrix TargetAngleVector(double angle)
{
	static matrix vector(3, 1);

	// Create a unit vector for target angle.
	vector(1, 1) = sin(D2R(angle));
	vector(2, 1) = cos(D2R(angle));
	vector(3, 1) = 0.0;

	return(vector);
}
/******************************************************************************/

BOOL TrialListSubset(void)
{
	int item, i;
	int Catch, CatchTrial, TrialOffset;

	if (TargetCount == 0)
	{
		printf("No targets specified.\n");
		return(FALSE);
	}

	TrialOffset = TotalTrials;

	// Set-up permite lists for exposure and catch-trial targets.
	TargetPermute.Init(0, TargetCount - 1, TRUE);

	if (CatchTargetCount > 0)
	{
		CatchTargetPermute.Init(0, CatchTargetCount - 1, TRUE);
	}

	NullTrial = 0;
	ExposureTrial = 0;
	CatchTrial = 0;

	// Create list of trials.
	for (Trial = 1; (Trial <= Trials); Trial++)
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
		if ((Trial >= ExposureTrials[0]) && (Trial <= ExposureTrials[1]))
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
		TrialData.RowSave(TrialOffset + Trial);
	}

	// Now put in the catch trials.
	for (Trial = CatchTrials[0]; (Trial < CatchTrials[1]); Trial += CatchTrials[2])
	{
		// Select a random trial to make a catch trial.
		Catch = irand(1, CatchTrials[2] - 1);

		// Load the current trial data.
		TrialData.RowLoad(Catch + Trial);

		// Make it a catch trial details.
		TrialType = TRIAL_CATCH;
		FieldType = CatchFieldType;
		FieldConstant = CatchFieldConstant;
		FieldAngle = CatchFieldAngle;

		// No visual feedback for visuomotor rotation catch trials.
		if (FieldType == FIELD_VISMOTROT)
		{
			VisualFeedback = FALSE;
		}

		// Select a random target for catch trial if required.
		if (CatchTargetCount > 0)
		{
			TargetIndex = CatchTargetPermute.GetNext();
			TargetAngle = CatchTargetAngles[TargetIndex];
			TargetPosition = HomePosition + (TargetAngleVector(TargetAngle) * TargetDistance);
		}

		// Overwrite trial with new catch-trial details.
		TrialData.RowSave(Catch + Trial);

		CatchTrial++;
	}

	printf("%d Total Trials, %d Null, %d Exposure, %d Catch.\n", Trials, NullTrial, ExposureTrial, CatchTrial);

	return(TRUE);
}

/**************************************************************************************/
void StateGraphicsNext(int state)
{
	if (StateGraphics == state)
	{
		return;
	}

	StateGraphicsTimer.Reset();
	StateGraphicsLast = StateGraphics;
	StateGraphics = state;

	// The visual target first appears in this state, so set graphics sync timer.
	if (StateGraphics == STATE_GO)
	{
		// Set graphics sync timer relative to offset of next vertical retrace.
		GraphicsTargetTimer.Reset(-GRAPHICS_VerticalRetraceOffsetTimeUntilNext());
	}
}
/******************************************************************************/

void TriggerOn(void)
{
	TriggerFlag = TRUE;
}

/******************************************************************************/

void TriggerOff(void)
{
	TriggerFlag = FALSE;
}


/**************************************************************************************/

void TrialSetup(void)
{
	// Load trial variables from TrialData.
	TrialData.RowLoad(Trial);

	// Set robot force field variables.
	RobotFieldType = FieldType;
	RobotFieldConstant = FieldConstant;
	RobotFieldAngle = FieldAngle;
	SPMX_romxZ(D2R(RobotFieldAngle), RobotFieldMatrix);

	TrialRunning = FALSE;
	StateGraphicsNext(State);
	TriggerOff();
}

/******************************************************************************/

void StateNext(int state)
{
	if (State == state)
	{
		return;
	}

	printf("STATE: %s[%d] > %s[%d] (%.0lf msec).\n", StateText[State], State, StateText[state], state, StateTimer.Elapsed());
	StateTimer.Reset();
	StateLast = State;
	State = state;
}

/******************************************************************************/

BOOL TrialList(void)
{
	int item, i;
	BOOL ok = TRUE;

	TotalTrials = 0;

	// Single or multiple configuration file paradigm?
	if (ConfigFiles == 1)
	{

		TotalTrials = Trials;
		ConfigIndex = 0;
	}
	else
	{
		// Loop over configuration files, counting the number of trials.
		for (ok = TRUE, ConfigIndex = 1; (ok && (ConfigIndex < ConfigFiles)); ConfigIndex++)
		{
			if (!ConfigLoad(ConfigFile[ConfigIndex]))
			{
				ok = FALSE;
				continue;
			}

			TotalTrials += Trials;
			printf("%d %s Trials=%d TotalTrials=%d\n", ConfigIndex, ConfigFile[ConfigIndex], Trials, TotalTrials);
		}

		ConfigIndex = 1;
	}

	if ((TotalTrials == 0) || !ok)
	{
		return(FALSE);
	}

	// Set rows of TrialData to the number of trials.
	TrialData.SetRows(TotalTrials);

	printf("Making list of %d trials (ESCape to abort)...\n", TotalTrials);

	TotalTrials = 0;
	TotalNullTrials = 0;
	TotalExposureTrials = 0;
	TotalCatchTrials = 0;

	// Loop over configuration files, appending each to growing trial list.
	for (ok = TRUE; (ok && (ConfigIndex < ConfigFiles)); ConfigIndex++)
	{
		if (ConfigIndex > 0)
		{
			if (!ConfigLoad(ConfigFile[ConfigIndex]))
			{
				ok = FALSE;
				continue;
			}
		}

		// Create subset of trials for this configuration file.
		if (!TrialListSubset())
		{
			ok = FALSE;
			continue;
		}

		TotalTrials += Trials;
		TotalNullTrials += NullTrial;
		TotalExposureTrials += ExposureTrial;
		TotalCatchTrials += CatchTrial;

		printf("%d %s Trials=%d TotalTrials=%d\n", ConfigIndex, ConfigFile[ConfigIndex], Trials, TotalTrials);
	}

	if (!ok)
	{
		return(FALSE);
	}

	printf("%d Total Trials, %d Null, %d Exposure, %d Catch.\n", TotalTrials, TotalNullTrials, TotalExposureTrials, TotalCatchTrials);

	// Total number of trails.
	Trials = TotalTrials;

	// Save trial list to file.
	ok = DATAFILE_Save(TrialListFile, TrialData);
	printf("%s %s Trials=%d.\n", TrialListFile, STR_OkFailed(ok), TrialData.GetRows());

	// Reset trial number, etc.
	Trial = 1;
	TrialSetup();
	ExperimentTimer.Reset();
	StateNext(STATE_INITIALIZE);

	return(TRUE);
}

void main(int argc, char *argv[])
{
	// Process command-line parameteres.
	if (!Parameters(argc, argv))
	{
		exit(0);
	}

	// Check disk space and file's existence.
	if (!DATAFILE_Check(DataFile))
	{
		exit(0);
	}

	// Initialize configuration file variables once.
	ConfigInit();

	// Load the first (and possibly the only) configuration file.
	if (!ConfigLoad(ConfigFile[0]))
	{
		exit(0);
	}

	printf("--------------------configload finished\n");

	// Initialize variables, etc.
	Initialize();

	// Create list of trials to run.
	TrialList();

	// Position of sphere in simulation.
	SpherePosition.zeros();
	SpherePosition(3, 1) = -20.0;
	SpherePosition(2, 1) -= 20;
	SpherePosition_Goal.zeros();
	//RobotFakePosition.zeros();

	SpherePosition_Goal(3, 1) = -20.0;
	SpherePosition_Goal(1, 1) += 20.0;
	//printf("================%f, %f, %f", SpherePosition(1, 1), SpherePosition(2, 1), SpherePosition(3, 1));

	// Start robot.
	//if (RobotStart())
	if (DeviceStart())
	{
		// Start graphics.
		if (GraphicsStart())
		{
			GraphicsMainLoop();
		}
		

		/* // from DynamicLearning.cpp
		// Start the graphics system.
		if (GraphicsStart(argc, argv))
		{
			// The experiment is run as part of graphics processing.
			GraphicsMainLoop();
		}
		*/
	}
}

/******************************************************************************/

