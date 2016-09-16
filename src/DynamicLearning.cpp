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
#define PRINTF3(v) printf("#v %5.2lf %5.1lf %5.1lf\n", v(1, 1), v(2, 1), v(3, 1))

STRING ConfigFile;
STRING ConfigSaveFile;

STRING DataName = "";
STRING DataFile = "";
STRING TrialListFile = "";

STRING RobotName = "";
int    RobotID = ROBOT_INVALID;
BOOL   RobotFT = FALSE;
double ForceMax = 100.0;
matrix RobotPosition(3, 1);
matrix RobotVelocity(3, 1);
matrix RobotForces(3, 1);
double RobotSpeed;
matrix HandleForces(3, 1);
matrix HandleTorques(3, 1);
double PhotoTransistor = 0.0;

/// spring and plate-channel force will be added during the trial
double SpringForceConstant = 0.0;
double UpForce = 0.0;
matrix PlateStartPos(3, 1);
double PlateForceConstant = 0.0;

double LoopTaskFrequency;
double LoopTaskPeriod;

matrix TextPosition(3, 1);
matrix HomePosition(3, 1);
double HomeTolerance = 0.50;
double HomeFinishTolerance = 0.50; //when finishing the movement
double HomeToleranceTime = 0.1;
double MovementReactionTimeOut = 0.5;
double MovementDurationTimeOut = 0.8;
double MovementDurationEval[2];
double ErrorWait = 2.0;
double TrialDelay = 0.0;
double InterTrialDelay = 0.5;

#define AXES 20
int	PreTrials = 0;
int TrialsPerAxis = 0;
int InterTrials = 0;
double RotationAxisX[AXES];
double RotationAxisY[AXES];
double RotationAxisZ[AXES];
double RotationAngle[AXES];
int AxisCount;
matrix CurRotationAxis(3, 1);
double CurRotationAngle;
matrix CurRotationMatrix(3, 3);

int    RestTrials = 0;
double RestWait = 60.0;
double NotMovingSpeed = 2.0; // cm/sec
double NotMovingTime = 0.1;
TIMER  NotMovingTimer("NotMoving");

#define TARGETS 32
double  TargetAnglesAZ[TARGETS]; //Azimuth
double  TargetAnglesEL[TARGETS]; //Elevation
int TargetCount;
BOOL TargetTestFlag = FALSE;
double TargetTolerance = 0.0;

matrix CursorPosition(3, 1);
matrix GazeDirection(3, 1);
int CursorColor = RED;
double CursorRadius = 0.5;

matrix ForceFieldForces(3, 1);
BOOL   ForceFieldStarted = FALSE;
matrix ForceFieldPosition(3, 1);
RAMPER ForceFieldRamp;
double ForceFieldRampTime = 0.1;

BOOL VisualFeedback;

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

////////////////////////////////////////////////////////////////////////
//////// these are added from GraphicsDiplay in oculus_demo.cpp

TIMER_Interval  GraphicsDrawLatencyTimer("GraphicsDrawLatency");
TIMER_Frequency GraphicsDrawFrequencyTimer("GraphicsDrawFrequency");
TIMER_Frequency GraphicsIdleFrequencyTimer("GraphicsIdleFrequency");

//////// these are added from GraphicsStart in oculus_demo.cpp
// Graphics Monitor Window variables.
int    MonitorX = 100;     // Monitor window X-position (pixels)
int    MonitorY = 100;     // Monitor window Y-position (pixels)
int    MonitorWid = 480;   // Monitor window width (pixels)
int    MonitorViews = 4;   // Monitor window views (can be 1 or 4).
double MonitorFreq = 60.0; // Monitor window display rate (Hz)

// Text Display variables.
matrix TextRotation(3, 1);
int    TextColor = WHITE;
float  TextSize = 0.75;
////////////////////////////////////////////////////////////////////////


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

STRING GraphicsString = "";
int GraphicsStringColor = WHITE;

// List of WAV files for beeping.
struct WAVELIST WaveList[] =
{
	{ "HighBip", "HIGH_BIP.WAV", NULL },
	{ "LowBip", "LOW_BIP.WAV", NULL },
	{ "MidBip", "MID_BIP.WAV", NULL },
	{ "GoBip", "GO_BIP.WAV", NULL },
	{ "ErrorBip", "ERROR_BIP.WAV", NULL },
	{ "ReminderBip", "REMINDER_BIP.WAV", NULL },
	{ "", "", NULL },
};

TIMER_Interval WaveListPlayInterval("WaveListPlay");

#define FRAMEDATA_ROWS 10000
MATDAT FrameData("FrameData");
BOOL   FrameRecord = FALSE;

TIMER  MovementDurationTimer("MovementDuration");
TIMER  MovementReactionTimer("MovementReaction");
TIMER  MovementFinishedTimer("MovementFinished");

TIMER  TrialTimer("CurTrial");
TIMER  InterTrialDelayTimer("InterTrialDelay");
double TrialTime;
double TrialDuration = 0.0;
int    CurTrial;
BOOL   TrialRunning = FALSE;

// CurTrial types.
#define TRIAL_NULL      0
#define TRIAL_EXPOSURE  1
#define TRIAL_CATCH     2

// Field types.
#define FIELD_NONE      0
#define FIELD_VISCOUS   1
#define FIELD_VISMOTROT 2
#define FIELD_CHANNEL   3

int    TotalTrials;
int    TotalNullTrials = 0;
int    TotalExposureTrials = 0;

int    ExposureTrials[2] = { 0, 10000 };
int    ExposureFieldType = FIELD_VISMOTROT;
double ExposureFieldConstant = 0.0;
double ExposureFieldAngle = 0.0;

// Permute list objects to randomize targets.
PERMUTELIST TargetPermute;

int    NullTrial = 0;
int    ExposureTrial = 0;

MATDAT TrialData("TrialData");

// Trial data variables.
int    TrialType;
matrix TargetPosition(3, 1);
int    TargetIndex;
double TargetAngleAZ;
double TargetAngleEL;
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

matrix PeakCursorPosition; // position when the cursor started to return
double PeakCursorDistance = 0; // corresponding distance
BOOL bTargetReached = false;

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

/******************************************************************************/

void ProgramExit(void);

/******************************************************************************/

void WaveListPlay(char *name)
{
	// Play WAV file (does interval timing).
	WaveListPlayInterval.Before();
	WAVELIST_Play(WaveList, name);
	WaveListPlayInterval.After();
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
	CONFIG_set(VAR(TargetAnglesAZ), TARGETS);
	CONFIG_set(VAR(TargetAnglesEL), TARGETS);
	CONFIG_set(VAR(TargetRadius));
	CONFIG_set(VAR(TargetTolerance));
	CONFIG_set(VAR(HomeRadius));
	CONFIG_set(VAR(HomePosition));
	CONFIG_set(VAR(HomeTolerance));
	CONFIG_set(VAR(HomeFinishTolerance));
	CONFIG_set(VAR(HomeToleranceTime));
	CONFIG_set(VAR(NotMovingSpeed));
	CONFIG_set(VAR(NotMovingTime));
	CONFIG_set(VAR(HomeToleranceTime));

	CONFIG_set(VAR(MovementReactionTimeOut));
	CONFIG_set(VAR(MovementDurationTimeOut));
	CONFIG_set(VAR(MovementDurationEval), 2);
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
	CONFIG_set(VAR(SpringForceConstant));
	CONFIG_set(VAR(PlateForceConstant));
	CONFIG_set(VAR(UpForce));

	CONFIG_set(VAR(PreTrials));
	CONFIG_set(VAR(TrialsPerAxis));
	CONFIG_set(VAR(InterTrials));
	CONFIG_set(VAR(RotationAxisX), AXES);
	CONFIG_set(VAR(RotationAxisY), AXES);
	CONFIG_set(VAR(RotationAxisZ), AXES);
	CONFIG_set(VAR(RotationAngle), AXES);

	CONFIG_set("FieldType", ExposureFieldType);
	CONFIG_set("FieldConstant", ExposureFieldConstant);
	CONFIG_set("FieldAngle", ExposureFieldAngle);
}

/******************************************************************************/

void ConfigInit(void)
{
	int i;

	for (i = 0; (i < TARGETS); i++)
	{
		TargetAnglesAZ[i] = 999;
		TargetAnglesEL[i] = 999;
	}

	for (i = 0; (i < AXES); i++)
	{
		RotationAxisX[i] = 999;
		RotationAxisY[i] = 999;
		RotationAxisZ[i] = 999;
		RotationAxisZ[i] = 999;
	}

	TargetCount = 0;
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
	for (TargetCount = 0; ((TargetAnglesAZ[TargetCount] != 999) && (TargetCount < TARGETS)); TargetCount++);

	if (TargetCount == 0)
	{
		printf("%s: No targets specified.\n", file);
		ok = FALSE;
	}

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
		printf("%d: AZ %.1lf (deg), EL %.1lf (deg)\n", i, TargetAnglesAZ[i], TargetAnglesEL[i]);
	}

	// Count the number of axes
	for (AxisCount = 0; ((RotationAxisX[AxisCount] != 999) && (AxisCount < AXES)); AxisCount++);

	if (AxisCount == 0)
	{
		printf("%s: No axis specified.\n", file);
		ok = FALSE;
	}

	printf("AxisCount=%d\n", AxisCount);
	for (i = 0; (i < AxisCount); i++)
	{
		printf("%d: [%.1lf, %.1lf, %.1lf]\n", i, RotationAxisX[i], RotationAxisY[i], RotationAxisZ[i]);
	}
	return(ok);
}

/******************************************************************************/

BOOL ConfigSave(char *file)
{
	BOOL ok;

	ConfigSetup();

	// Save configuration file.
	ok = CONFIG_write(file);

	printf("ConfigSave(%s) %s.\n", file, STR_OkFailed(ok));

	return(ok);
}

/******************************************************************************/

BOOL ConfigSaveFiles(void)
{
	int i;
	BOOL ok = TRUE;

	strncpy(ConfigSaveFile, STR_stringf("%s.CFG", DataName), STRLEN);

	ok = ConfigSave(ConfigSaveFile);

	return(ok);
}

/******************************************************************************/

void GraphicsDisplayText(char *string, float size, matrix &pos, int color)
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


	GRAPHICS_ColorSet(color);

	for (i = 0; (i < w); i++)
		glutStrokeCharacter(font, string[i]);

	glPopMatrix();
}

/******************************************************************************/

void GraphicsDisplayText(void)
{
	static matrix P(3, 1);

	if (!STR_null(GraphicsString))
	{
		P = TextPosition;
		P(2, 1) -= 3.0;
		GraphicsDisplayText(GraphicsString, 1.0, P, GraphicsStringColor);
	}
}

/******************************************************************************/

void GraphicsText(char *text)
{
	if (text != NULL)
	{
		strncpy(GraphicsString, text, STRLEN);
	}
}

void GraphicsText(char *text, int color)
{
	if (text != NULL)
	{
		strncpy(GraphicsString, text, STRLEN);
		GraphicsStringColor = color;
	}
}


/******************************************************************************/

void FrameProcess(void)
{
	// Is frame recording in progres.
	if (!FrameRecord)
	{
		return;
	}

	// Load GRAPHICS-related frame data variables.
	GRAPHICS_FrameData();

	// Save current variables for FrameData.
	FrameData.RowSave();
}

/******************************************************************************/

void FrameStart(void)
{
	// Start recording frame data.
	FrameData.Reset();
	FrameRecord = TRUE;
}

/******************************************************************************/

void FrameStop(void)
{
	// Stop recording frame data.
	FrameRecord = FALSE;
}

/******************************************************************************/

void RecordEyePosition(void)
{
	if (FrameRecord == TRUE){
		smi_SampleHMDStruct ep;
		EYETRACKER_HMD_DATA(&ep);
		GazeDirection(1, 1) = ep.gazeDirection.x;
		GazeDirection(2, 1) = ep.gazeDirection.y;
		GazeDirection(3, 1) = ep.gazeDirection.z;
		//printf("%f",ep.gazeDirection.x);
	}
}

void StateProcessLoopTask(void);

/******************************************************************************/

void ForceFieldStart(void)
{
	ForceFieldPosition = RobotPosition;
	PlateStartPos = RobotPosition;

	ForceFieldStarted = TRUE;
	ForceFieldRamp.Up();

	printf("Force field started \n");
}

/******************************************************************************/

void ForceFieldStop(void)
{
	ForceFieldRamp.Down();
	ForceFieldStarted = FALSE;

	printf("Force field stopped \n");
}

/******************************************************************************/

matrix getPlateForce(void)
{
	matrix f(3, 1);

	// normal vector
	double n1 = CurRotationMatrix(3, 1);
	double n2 = CurRotationMatrix(3, 2);
	double n3 = CurRotationMatrix(3, 3);

	matrix dev = PlateStartPos - RobotPosition;

	// deviation in the direction of normal
	double vdev = n1 * dev(1, 1) + n2 * dev(2, 1) + n3 * dev(3, 1);

	f(1, 1) = -n1 * vdev * PlateForceConstant;
	f(2, 1) = -n2 * vdev * PlateForceConstant;
	f(3, 1) = -n3 * vdev * PlateForceConstant;

	return f;
}

//add eye position here
void RobotForcesFunction(matrix &position, matrix &velocity, matrix &forces)
{
	int i, j;
	static matrix P, R, _R;

	// Monitor timing of Forces Function (values saved to FrameData).
	ForcesFunctionPeriod = RobotForcesFunctionFrequency.Loop();
	RobotForcesFunctionLatency.Before();

	TrialTime = TrialTimer.ElapsedSeconds();

	// Kinematic data passed from robot API.
	RobotPosition = position;
	RobotVelocity = velocity;
	RobotSpeed = norm(RobotVelocity);

	////////////////////////////////////////////////////////////////////////////////////////////
	//Translate cursor /////////////////////// this is added from oculus_demo.cpp
	RobotPosition(2, 1) += 43.5;
	RobotPosition(3, 1) -= 51.1;
	////////////////////////////////////////////////////////////////////////////////////////////

	// Zero forces.
	ForceFieldForces.zeros();
	RobotForces.zeros();

	// Read raw sensor values from Sensoray card.
	ROBOT_SensorRead(RobotID);

	// Get Force/Torque sensor if required.
	if (RobotFT && ROBOT_SensorOpened_DAQFT(RobotID))
	{
		ROBOT_Sensor_DAQFT(RobotID, HandleForces, HandleTorques);
	}

	// Get PhotoTransistor if required.
	if (ROBOT_SensorOpened_PhotoTransistor(RobotID))
	{
		ROBOT_Sensor_PhotoTransistor(RobotID, PhotoTransistor);
	}

	// Process position delay window, if required.
	P = RobotPosition;
	if (PositionDelaySeconds > 0.0)
	{
		if (PositionDelayList.isempty())
		{
			PositionDelayItems = 1 + (int)(PositionDelaySeconds / milliseconds2seconds(LOOPTASK_Period));
			PositionDelayList.dim(3, PositionDelayItems);
		}

		for (i = 1; (i < PositionDelayItems); i++)
		{
			for (j = 1; (j <= 3); j++)
			{
				PositionDelayList(j, i) = PositionDelayList(j, i + 1);
			}
		}

		i = PositionDelayItems;
		for (j = 1; (j <= 3); j++)
		{
			PositionDelayList(j, i) = RobotPosition(j, 1);
		}

		P = PositionDelayList[1];
	}

	////////////////////// ADD SPRING FORCE
	ForceFieldForces = SpringForceConstant * (HomePosition - P);

	// ADD PLATE FORCE
	ForceFieldForces += getPlateForce();
	

	// always visuomotor rotation
	CursorPosition = HomePosition + (CurRotationMatrix * (P - HomePosition));

	// Process force-field type.
	switch (ForceFieldStarted ? RobotFieldType : FIELD_NONE)
	{
	case FIELD_NONE:
		break;

	case FIELD_VISCOUS:   // Viscous force field.
		ForceFieldForces = RobotFieldConstant * RobotFieldMatrix * RobotVelocity;
		break;

	case FIELD_VISMOTROT: // Visual motor rotation.
		break;

	case FIELD_CHANNEL:
		/*
		// Next rotate position to vertical (angle = 0).
		SPMX_romxZ(D2R(TargetAngle), R);
		SPMX_romxZ(D2R(-TargetAngle), _R);

		P = R * (RobotPosition - ForceFieldPosition);

		// Calculate perpendicular (X) channel forces.
		ForceFieldForces(1, 1) = RobotFieldConstant * P(1, 1);

		// Rotate back to original.
		ForceFieldForces = _R * ForceFieldForces;
		*/
		break;
	}

	double dist = norm(CursorPosition - HomePosition);
	if (dist > PeakCursorDistance)
	{
		PeakCursorPosition = CursorPosition;
		PeakCursorDistance = dist;
	}


	// Process Finite State Machine.
	StateProcessLoopTask();

	// Monitor timing of Forces Function (values saved to FrameData).
	ForcesFunctionLatency = RobotForcesFunctionLatency.After();

	//OptoTrakRead();

	//EYETRACKER READ(until oculus scene start, then we can record hmd position.)
	//Update the eye position here, the frequency is really high, which may lead
	//the gazedirection ball looks shaky in graphics.
	//RecordEyePosition();

	RobotForces = ForceFieldRamp.RampCurrent() * ForceFieldForces;

	// add anti-gravitational force
	RobotForces(3, 1) = RobotForces(3, 1) + UpForce;

	// Save frame data.
	FrameProcess();

	// Set forces to pass to robot API and clamp for safety.
	forces = RobotForces;
	forces.clampnorm(ForceMax);
}

/******************************************************************************/

void DeviceStop(void)
{
	// Stop and close robot.
	ROBOT_Stop(RobotID);
	ROBOT_SensorClose(RobotID);
	ROBOT_Close(RobotID);

	ForceFieldRamp.Stop();
	ForceFieldRamp.Close();

	//Close Eyetracker 
	EYETRACKER_HMD_Close();

	//OptoTrakStop();

	RobotID = ROBOT_INVALID;
}

/******************************************************************************/

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
	printf("Opening robot....\n");
	if ((RobotID = ROBOT_Open(RobotName)) == ROBOT_INVALID)
	{
		printf("%s: Open failed.\n", RobotName);
		ok = FALSE;
	}
	else
	{
		printf("Starting robot....\n");

		if (!ROBOT_Start(RobotID, RobotForcesFunction))
		{
			printf("%s: Start failed.\n", RobotName);
			ok = FALSE;
		}
		else
		{
			printf("Starting robot sensor....\n");
			if (!ROBOT_SensorOpen(RobotID))
			{
				printf("%s: Cannot open sensor(s).\n", RobotName);
				ok = FALSE;
			}
			else
				if (RobotFT)
				{
					printf("Opening robot DAQFT....\n");
					if (!ROBOT_SensorOpened_DAQFT())
					{
						printf("%s: F/T sensor not opened.\n", RobotName);
						RobotFT = FALSE;
					}
				}
		}
	}

	if (ok)
	{
		printf("Open force field ramp....\n");
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
	
	if (ok)
	{
		printf("Open eyetracker hmd....\n");
		EYETRACKER_HMD_Open();
		ok = EYETRACKER_HMD_Start();
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

BOOL RobotActive(void)
{
	BOOL flag = FALSE;

	if (ROBOT_Activated(RobotID) && ROBOT_Ramped(RobotID))
	{
		flag = TRUE;
	}

	return(flag);
}

/******************************************************************************/

double RobotDistance(matrix &home)
{
	double distance;

	distance = norm(RobotPosition - home);

	return(distance);
}
/******************************************************************************/

double CursorDistance(matrix &home)
{
	double distance;

	distance = norm(CursorPosition - home);

	return(distance);
}
/******************************************************************************/

BOOL RobotAt(matrix &home, double tolerance)
{
	BOOL flag = FALSE;

	if (RobotDistance(home) <= tolerance)
	{
		flag = TRUE;
	}

	return(flag);
}

/******************************************************************************/
BOOL CursorAt(matrix &home, double tolerance)
{
	BOOL flag = FALSE;

	if (CursorDistance(home) <= tolerance)
	{
		flag = TRUE;
	}

	return(flag);
}

/******************************************************************************/

BOOL RobotHome(void)
{
	BOOL flag;

	flag = RobotAt(HomePosition, HomeTolerance);

	return(flag);
}

/******************************************************************************/
BOOL CursorHome(void)
{
	BOOL flag;

	flag = CursorAt(HomePosition, HomeTolerance);

	return(flag);
}

/******************************************************************************/

BOOL RobotNotMoving(void)
{
	BOOL flag;

	if (NotMovingSpeed == 0.0)
	{
		return(TRUE);
	}

	if (RobotSpeed > NotMovingSpeed)
	{
		NotMovingTimer.Reset();
	}

	flag = NotMovingTimer.ExpiredSeconds(NotMovingTime);

	return(flag);
}

/******************************************************************************/

BOOL MovementStarted(void)
{
	BOOL flag;

	//flag = !CursorAt(HomePosition, HomeTolerance);
	flag = (RobotSpeed > NotMovingSpeed);

	return(flag);
}

/******************************************************************************/

BOOL MovementFinished(void)
{
	BOOL flag = FALSE;

	// Is robot in the finish position?
	if (!CursorAt(HomePosition, HomeFinishTolerance) || PeakCursorDistance < HomeFinishTolerance)
	{
		MovementFinishedTimer.Reset();
	}

	// Has the robot been in the finish position for the required amount of time?
	if (MovementFinishedTimer.ExpiredSeconds(HomeToleranceTime))
	{
		flag = TRUE;
	}

	return(flag);
}

/******************************************************************************/


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

void TrialSetup(void)
{
	// Load trial variables from TrialData.
	TrialData.RowLoad(CurTrial);

	// Set robot force field variables.
	TrialRunning = FALSE;
	StateGraphicsNext(State);
	TriggerOff();
}

/******************************************************************************/

void TrialStart(void)
{
	printf("Starting CurTrial %d...\n", CurTrial);
	printf("TargetAngleAZ=%.1lf(deg) TargetAngleEL=%.1lf(deg)  TrialType=%d CurRotationAxis=[%.2lf, %.2lf, %.2lf] CurRotationAngle=%.2lf\n", TargetAngleAZ, TargetAngleEL, TrialType, CurRotationAxis(1, 1), CurRotationAxis(2, 1), CurRotationAxis(3, 1), CurRotationAngle);
	printf("Rotation Matrix: \n %.2lf %.2lf %.2lf \n %.2lf %.2lf %.2lf \n %.2lf %.2lf %.2lf \n", CurRotationMatrix(1, 1), CurRotationMatrix(1, 2), CurRotationMatrix(1, 3), CurRotationMatrix(2, 1), CurRotationMatrix(2, 2), CurRotationMatrix(2, 3), CurRotationMatrix(3, 1), CurRotationMatrix(3, 2), CurRotationMatrix(3, 3));
	TrialTimer.Reset();
	TrialTime = TrialTimer.ElapsedSeconds();
	TrialRunning = TRUE;

	// Start force field.
	ForceFieldStart();

	// Start recording frame data for trial.
	FrameStart();
}

/******************************************************************************/

void TrialStop(void)
{
	TrialRunning = FALSE;
	printf("Stopping CurTrial %d...\n", CurTrial);

	// Stop recording frame for trial.
	FrameStop();

	// Stop force field.
	ForceFieldStop();

	TrialDuration = TrialTimer.ElapsedSeconds();
	InterTrialDelayTimer.Reset();

}

/******************************************************************************/

void TrialAbort(void)
{
	TrialRunning = FALSE;
	printf("Aborting CurTrial %d...\n", CurTrial);

	// Stop recording frame data for trial.
	FrameStop();

	// Stop force field.
	ForceFieldStop();
}

/******************************************************************************/

BOOL TrialSave(void)
{
	BOOL ok = FALSE;

	ExperimentTime = ExperimentTimer.ElapsedSeconds();
	MissTrials = MissTrialsTotal;

	// Put values in the trial data
	TrialData.RowSave(CurTrial);

	// Set-up the data file on the first trial.
	if (CurTrial == 1)
	{
		// Open the file for trial data.
		if (!DATAFILE_Open(DataFile, TrialData, FrameData))
		{
			printf("DATAFILE: Cannot open file: %s\n", DataFile);
			return(FALSE);
		}
	}

	// Write the trial data to the file.
	printf("Saving trial %d: %d frames of data collected in %.2lf seconds.\n", CurTrial, FrameData.GetRow(), TrialDuration);
	ok = DATAFILE_TrialSave(CurTrial);
	printf("%s %s CurTrial=%d.\n", DataFile, STR_OkFailed(ok), CurTrial);

	return(ok);
}

/******************************************************************************/

void TrialExit(void)
{
	BOOL ok;
	int i;
	STRING file;

	// Only continue if data file has been opened.
	if (!DATAFILE_Opened())
	{
		return;
	}

	// Append program source file to data file.
	strncpy(file, STR_stringf("%s.cpp", MODULE_NAME), STRLEN);
	printf("Appending %s to data file...\n", file);
	ok = DATAFILE_AppendFile(file);
	printf("%s\n", STR_OkFailed(ok));

	// Append configuration files to data file.

	strncpy(file, ConfigFile, STRLEN);
	printf("Appending %s to data file...\n", file);

	ok = DATAFILE_AppendFile(file);
	printf("%s\n", STR_OkFailed(ok));

	// Close data file.
	DATAFILE_Close();

	// Also save configuration files separately using data file name.
	ConfigSaveFiles();
}

/******************************************************************************/

BOOL TrialNext(void)
{
	BOOL flag = FALSE;

	if (CurTrial < TotalTrials)
	{
		CurTrial++;
		flag = TRUE;
	}

	return(flag);
}

/******************************************************************************/

void BeepGo(void)
{
	WaveListPlay("GoBip");
}

/******************************************************************************/

void BeepError(void)
{
	WaveListPlay("ErrorBip");
}

/******************************************************************************/

void MessageClear(void)
{
	GraphicsText("");
	GRAPHICS_ClearColor(); // Default background color.
}

/******************************************************************************/

void MessageSet(char *text, int color)
{
	MessageClear();

	GraphicsText(text, color);
	/*
	if (background != -1)
	{
	GRAPHICS_ClearColor(background);
	}
	*/
}

/******************************************************************************/

void MessageSet(char *text)
{
	//int background = -1;

	MessageSet(text, WHITE);
}

/******************************************************************************/

void ErrorFrameDataFull(void)
{
	MessageSet("Frame data full", RED);
	printf("Error: Frame data full\n");
}

/******************************************************************************/

void ErrorMessage(char *str)
{
	MessageSet(str, RED);
	printf("Error: %s\n", str);
}

/******************************************************************************/

void ErrorMoveWaitTimeOut(void)
{
	MessageSet("REACTION TOO SLOW", RED);
	printf("Error: MoveWaitTimeOut\n");
}

/******************************************************************************/

void ErrorMoveTimeOut(void)
{
	MessageSet("MOVEMENT TOO SLOW", RED);
	printf("Error: MoveTimeOut\n");
}

/******************************************************************************/

void ErrorMoveTooSoon(void)
{
	MessageSet("MOVE AFTER BEEP", RED);
	printf("Error: MoveTooSoon\n");
}

/******************************************************************************/

void ErrorRobotInactive(void)
{
	MessageSet("HANDLE SWITCH OFF", RED);
	printf("Error: RobotInactive\n");
}

/******************************************************************************/

void ErrorState(int state)
{
	BeepError();
	StateErrorResume = state;
	StateNext(STATE_ERROR);
}

/******************************************************************************/

void ErrorResume(void)
{
	MessageClear();
	StateNext(StateErrorResume);
}

/******************************************************************************/

void MissTrial(void)
{
	MissTrialsTotal++;
	MissTrialsPercent = 100.0 * ((double)MissTrialsTotal / (double)CurTrial);
	printf("\nMiss Trials = %d/%d (%.0lf%%)\n\n", MissTrialsTotal, CurTrial, MissTrialsPercent);

	ErrorState(STATE_SETUP);
}

/******************************************************************************/

void StateProcessLoopTask(void)
{
	// Only some states are processing in the LoopTask.
	if (!StateLoopTask[State])
	{
		return;
	}

	// State processing.
	switch (State)
	{
	case STATE_MOVEWAIT:
		if (MovementStarted())
		{
			StartPosition = CursorPosition; //updating starting position == will be the center of visuomotor rotation
			MovementDurationTimer.Reset();
			MovementReactionTime = MovementReactionTimer.ElapsedSeconds();
			StateNext(STATE_MOVING);
			break;
		}

		if (MovementReactionTimer.ExpiredSeconds(MovementReactionTimeOut))
		{
			StateNext(STATE_TIMEOUT);
			break;
		}
		break;

	case STATE_MOVING:
		if (MovementFinished())
		{
			FinishPosition = CursorPosition;

			MovementDurationTime = MovementDurationTimer.ElapsedSeconds();

			StateNext(STATE_FINISH);
			break;
		}

		if (MovementDurationTimer.ExpiredSeconds(MovementDurationTimeOut))
		{
			StateNext(STATE_TIMEOUT);
			break;
		}
		break;
	}
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
			TrialSetup();

			StateNext(STATE_HOME);
		}
		break;

	case STATE_HOME:
		// Start trial when robot in home position (and stationary and active).
		if (RobotNotMoving() && CursorHome() && RobotActive())
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
			PeakCursorDistance = 0;
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
		// CurTrial has finished so stop trial.
		TrialStop();

		// Save the data for this trial.
		if (!TrialSave())
		{
			printf("Cannot save CurTrial %d.\n", CurTrial);
			StateNext(STATE_EXIT);
			break;
		}

		// Catch too-slow trials, otherwise give speed feedback
		if (MovementDurationTime >= MovementDurationTimeOut)
		{
			ErrorMoveTimeOut();
			BeepError();
			InterTrialDelayTimer.Reset();
		}
		else if (norm(PeakCursorPosition - TargetPosition) > TargetTolerance) //missed trial
		{
			bTargetReached = false;
			STRING msg;
			//sprintf(msg, "FAST %.2lf", MovementDurationTime);
			sprintf(msg, "MISSED");
			MessageSet(msg, RED);
			WaveListPlay("LowBip");
		}
		else if (MovementDurationTime < MovementDurationEval[0]) // fast
		{
			bTargetReached = true;
			STRING msg;
			//sprintf(msg, "FAST %.2lf", MovementDurationTime);
			sprintf(msg, "FAST");
			MessageSet(msg, YELLOW);
			WaveListPlay("MidBip");
		}
		else if (MovementDurationTime < MovementDurationEval[1]) // great
		{
			bTargetReached = true;
			STRING msg;
			//sprintf(msg, "GREAT! %.2lf", MovementDurationTime);
			sprintf(msg, "GREAT!");
			MessageSet(msg, GREEN);
			WaveListPlay("HighBip");
		}
		else // slow
		{
			bTargetReached = true;
			STRING msg;
			//sprintf(msg, "SLOW %.2lf", MovementDurationTime);
			sprintf(msg, "SLOW");
			MessageSet(msg, YELLOW);
			WaveListPlay("MidBip");
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
			if ((CurTrial % RestTrials) == 0)
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

void GraphicsResults(void)
{
	printf("----------------------------------------------------------------\n");

	printf("Monitor Refresh Rate %.0lf Hz.\n", GRAPHICS_VerticalRetraceFrequency);
	GraphicsDisplayFrequency.Results();
	GraphicsDisplayLatency.Results();
	GraphicsSwapBufferLatency.Results();
	GraphicsClearStereoLatency.Results();
	GraphicsClearMonoLatency.Results();
	GraphicsIdleFrequency.Results();

	if (GraphicsVerticalRetraceSyncTime != 0.0)
	{
		GRAPHICS_VerticalRetraceResults();
	}

	printf("----------------------------------------------------------------\n");
}

/******************************************************************************/

void Results(void)
{
	// Print results for various timers and things.
	RobotForcesFunctionLatency.Results();
	RobotForcesFunctionFrequency.Results();
	GraphicsResults();
	WaveListPlayInterval.Results();
	//OptoTrakResults();
}

/******************************************************************************/

void ProgramExit(void)
{
	// Do trial data exit stuff.
	TrialExit();

	// Stop, close and other final stuff.
	DeviceStop();
	GRAPHICS_Stop();
	Results();
	WAVELIST_Close(WaveList);

	printf("ExperimentTime = %.0lf minutes.\n", ExperimentTimer.ElapsedMinutes());

	// Exit the program.
	exit(0);
}

/******************************************************************************/

matrix TargetAngleVector(double angleAZ, double angleEL)
{
	static matrix vector(3, 1);

	double cosEL = cos(D2R(angleEL));
	double sinEL = sin(D2R(angleEL));
	double cosAZ = cos(D2R(angleAZ));
	double sinAZ = sin(D2R(angleAZ));

	// Create a unit vector for target angle.
	vector(1, 1) = cosEL * sinAZ;
	vector(2, 1) = cosEL * cosAZ;
	vector(3, 1) = sinEL;

	return(vector);
}


/******************************************************************************/

void DrawCircle(matrix &centre, matrix &normal, float r, int num_segments, int color, float alpha)
{
	float theta = 2 * PI / float(num_segments);
	glPushMatrix();
	GRAPHICS_Translate(centre);
	float ang = acos(normal(3, 1) / norm(normal)) * 180 / PI;

	glRotatef(ang, -1 * normal(2, 1), normal(1, 1), 0.0f);

	float x = r;//we start at angle = 0 
	float y = 0;

	glBegin(GL_LINE_LOOP);

	//glLineWidth(lineWidth);
	GRAPHICS_ColorSet(color, alpha);

	for (int ii = 0; ii < num_segments; ii++)
	{
		float q = theta * ii;
		x = r * cos(q);
		y = r * sin(q);
		glVertex3f(x, y, 0);//output vertex 
	}
	glVertex3f(r, 0, 0);

	glEnd();

	glPopMatrix();
}

/******************************************************************************/
void GraphicsScene(void) // this is created by shyeo, isolating the gl routines only from GraphicsDisplay
{
	int attr;
	static matrix posn;

	// Display text.
	//strncpy(GraphicsString, StateText[StateGraphics], STRLEN);
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

	// Display home position and all target positions at start of trial.
	if ((StateGraphics >= STATE_SETUP) && (StateGraphics <= STATE_INTERTRIAL))
	{
		attr = CursorHome() ? RED : GREY;
		GRAPHICS_WireSphere(&HomePosition, HomeRadius, attr);

		// big sphere
		//GRAPHICS_WireSphere(&StartPosition, TargetDistance, WHITE);
		//GRAPHICS_Circle(&StartPosition, TargetDistance, WHITE);

		matrix normal(3, 1);
		matrix pos = HomePosition;
		int c_ = DARKGREY;
		int a_ = 1.0;
		normal(1, 1) = 0.0;
		normal(2, 1) = 0.0;
		normal(3, 1) = 1.0;
		DrawCircle(pos, normal, TargetDistance, 60, c_, a_);

		normal(1, 1) = 1.0;
		normal(2, 1) = 0.0;
		normal(3, 1) = 0.0;
		DrawCircle(pos, normal, TargetDistance, 60, c_, a_);

		normal(1, 1) = 1.0;
		normal(2, 1) = 1.0;
		normal(3, 1) = 0.0;
		DrawCircle(pos, normal, TargetDistance, 60, c_, a_);

		normal(1, 1) = 0.0;
		normal(2, 1) = 1.0;
		normal(3, 1) = 0.0;
		DrawCircle(pos, normal, TargetDistance, 60, c_, a_);

		normal(1, 1) = 0.0;
		normal(2, 1) = 0.0;
		normal(3, 1) = 1.0;
		pos(3, 1) = pos(3, 1) + TargetDistance / sqrt((float)2);
		DrawCircle(pos, normal, TargetDistance / sqrt((float)2), 60, c_, a_);

		normal(1, 1) = 0.0;
		normal(2, 1) = 0.0;
		normal(3, 1) = 1.0;
		pos(3, 1) = pos(3, 1) - 2 * TargetDistance / sqrt((float)2);
		DrawCircle(pos, normal, TargetDistance / sqrt((float)2), 60, c_, a_);

		for (int j = 0; j < TargetCount; j++)
		{
			matrix posn = TargetDistance * TargetAngleVector(TargetAnglesAZ[j], TargetAnglesEL[j]);

			GRAPHICS_LightingDisable();
			glLineWidth(1.0);

			glPushMatrix();

			GRAPHICS_Translate(posn);
			GRAPHICS_ColorSet(DARKGREY, 1.0);
			glutWireSphere(TargetRadius, 6, 6);
			glPopMatrix();
			GRAPHICS_LightingEnable();

		}
		// Display all target spheres
	}



	// Display target spheres when trial running.
	if ((StateGraphics >= STATE_GO) && (StateGraphics <= STATE_FINISH))
	{
		// Display target for movement.
		attr = CursorAt(TargetPosition, TargetTolerance) ? RED : YELLOW;
		GRAPHICS_WireSphere(&TargetPosition, TargetRadius, attr);
		/*
		// Display graphics sync target for phototransistor.
		if (!GraphicsSyncPosition.iszero())
		{
		GRAPHICS_WireSphere(&GraphicsSyncPosition, GraphicsSyncRadius, GraphicsSyncColor);
		}*/
	}
	// Display finish position.
	if ((StateGraphics > STATE_MOVING) && (StateGraphics <= STATE_INTERTRIAL))
	{
		attr = bTargetReached ? GREEN : RED;
		GRAPHICS_WireSphere(&PeakCursorPosition, CursorRadius, attr, 0.2);
	}

	// Display robot position cursor.
	if ((StateGraphics != STATE_ERROR) && (VisualFeedback || RobotHome()))
	{
		if ((StateGraphics >= STATE_SETUP) && (StateGraphics < STATE_INTERTRIAL))
		{
			//attr = CursorAt(TargetPosition, TargetTolerance) ? GREEN : CursorColor;
			//if (StateGraphics != S0TATE_HOME) GRAPHICS_Sphere(&CursorPosition, CursorRadius, RED);
			GRAPHICS_Sphere(&CursorPosition, CursorRadius, RED);
		}

	}

	// Display eye gaze direction.
	//Update the RecordEyePosition here may looks smooth on the graphics, but the update frequency is lower than robot arm.
	RecordEyePosition();
	GRAPHICS_Sphere(&GazeDirection, CursorRadius, YELLOW);

}
/******************************************************************************/
void GraphicsDisplay(void)
{
	// just use one from oculus_demo.cpp
	GraphicsDrawFrequencyTimer.Loop();
	GraphicsDrawLatencyTimer.Before();

	GraphicsScene();

	GraphicsDrawLatencyTimer.Before();
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
		GRAPHICS_Sphere(&posn, HomeRadius, RED);

		for (target = 0; (target < TargetCount); target++)
		{
			posn = HomePosition + (TargetAngleVector(TargetAnglesAZ[target], TargetAnglesEL[target]) * TargetDistance);
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
	BOOL draw = FALSE;

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

	GraphicsIdleFrequencyTimer.Loop();

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
	if (draw)
	{
		StateGraphicsNext(State); // Safe to set graphics state at this point.
		if (TargetTestFlag)
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

void GraphicsKeyboard(unsigned char key, int x, int y)
{
	// Process keyboard input.
	switch (key)
	{
	case ESC:
		ProgramExit();
		break;
	}
}

/******************************************************************************/

BOOL GraphicsStart()
{
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

	// Default position for text display.
	TextPosition.zeros();      // Centred in the workspace above the green sphere.

	// Default rotation for text display.
	TextRotation.zeros();
	TextRotation(1, 1) = 40.0;  // Rotate text forward (x-axis) for better visibility.

	// Set default values for text display.
	GRAPHICS_TextDefault(&TextPosition, &TextRotation, TextColor, TextSize);

	// Register call-back functions and pass control to graphics system.
	GRAPHICS_MainLoop(GraphicsKeyboard, GraphicsDisplay, GraphicsIdle);

	printf("-----------------------------GraphicsStart exit\n");

	return(TRUE);
}

/******************************************************************************/

void GraphicsMainLoop(void)
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

void Usage(void)
{
	printf("----------------------------------\n");
	printf("%s /C:Config(1)[,...Config(n)] /M:MetaConfig /D:DataFile\n", MODULE_NAME);
	printf("----------------------------------\n");

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
			ok = CMDARG_data(ConfigFile, data, STRLEN);
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

	if (STR_null(ConfigFile))
	{
		printf("Configuration file(s) not specified.\n");
		return(FALSE);
	}

	if (STR_null(DataName))
	{
		printf("Data file not specified.\n");
		return(FALSE);
	}

	printf("ConfigFile = %s\n", ConfigFile);

	return(TRUE);
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
	TrialData.AddVariable(VAR(RobotFieldType));
	TrialData.AddVariable(VAR(CurRotationAxis));
	TrialData.AddVariable(VAR(CurRotationAngle));
	TrialData.AddVariable(VAR(CurRotationMatrix));
	TrialData.AddVariable(VAR(PositionDelaySeconds));
	TrialData.AddVariable(VAR(TargetIndex));
	TrialData.AddVariable(VAR(TargetAngleAZ));
	TrialData.AddVariable(VAR(TargetAngleEL));
	TrialData.AddVariable(VAR(TargetPosition));
	TrialData.AddVariable(VAR(TargetDistance));
	TrialData.AddVariable(VAR(StartPosition));
	TrialData.AddVariable(VAR(PeakCursorPosition));
	TrialData.AddVariable(VAR(PeakCursorDistance));
	TrialData.AddVariable(VAR(bTargetReached));
	TrialData.AddVariable(VAR(FinishPosition));
	TrialData.AddVariable(VAR(VisualFeedback));
	TrialData.AddVariable("MissTrials", MissTrials);        // Required subject wellbeing variable
	TrialData.AddVariable("TrialDuration", TrialDuration);  // Required subject wellbeing variable
	TrialData.AddVariable(VAR(MovementReactionTime));
	TrialData.AddVariable(VAR(MovementDurationTime));
	TrialData.AddVariable(VAR(SpringForceConstant));


	// Set rows of TrialData to the number of trials.
	TotalTrials = PreTrials + (TrialsPerAxis + InterTrials) * AxisCount;
	TrialData.SetRows(TotalTrials);

	// Add each variable to the FrameData matrix.
	FrameData.AddVariable(VAR(TrialTime));                 // Required subject wellbeing variable
	FrameData.AddVariable(VAR(State));
	FrameData.AddVariable(VAR(StateGraphics));
	FrameData.AddVariable(VAR(TriggerFlag));
	//FrameData.AddVariable(VAR(PhotoTransistor));
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
	FrameData.AddVariable(VAR(GazeDirection));
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
int getAxisPhase(int trialNum)
{
	if (trialNum <= PreTrials)
	{
		return -1;
	}
	else
	{
		for (int i = 0; i < AxisCount; i++)
		{
			int idx1 = PreTrials + i * (TrialsPerAxis + InterTrials);
			int idx2 = PreTrials + i * (TrialsPerAxis + InterTrials) + TrialsPerAxis;
			if (trialNum > idx1 && trialNum <= idx2)
			{
				return i;
			}
		}
	}

	return -1;
}

void AxisAngleToSO3(matrix axis, double angle, matrix &SO3)
{
	if (axis.iszero())
	{
		SO3.zeros();
		SO3(1, 1) = 1;
		SO3(2, 2) = 1;
		SO3(3, 3) = 1;

		return;
	}

	axis = axis / norm(axis);

	double c = cos(D2R(angle));
	double s = sin(D2R(angle));
	double t = 1.0 - c;

	double x = axis(1, 1);
	double y = axis(2, 1);
	double z = axis(3, 1);

	double m00, m01, m02, m10, m11, m12, m20, m21, m22;

	m00 = c + x * x * t;
	m11 = c + y * y * t;
	m22 = c + z * z * t;

	double tmp1 = x * y * t;
	double tmp2 = z * s;
	m10 = tmp1 + tmp2;
	m01 = tmp1 - tmp2;
	tmp1 = x * z * t;
	tmp2 = y * s;
	m20 = tmp1 - tmp2;
	m02 = tmp1 + tmp2;
	tmp1 = y * z * t;
	tmp2 = x * s;
	m21 = tmp1 + tmp2;
	m12 = tmp1 - tmp2;

	SO3(1, 1) = m00;
	SO3(1, 2) = m01;
	SO3(1, 3) = m02;
	SO3(2, 1) = m10;
	SO3(2, 2) = m11;
	SO3(2, 3) = m12;
	SO3(3, 1) = m20;
	SO3(3, 2) = m21;
	SO3(3, 3) = m22;

	return;
}

BOOL BuildTrialList(void)
{
	int item, i;

	if (TargetCount == 0)
	{
		printf("No targets specified.\n");
		return(FALSE);
	}

	// Set-up permite lists for exposure and catch-trial targets.
	TargetPermute.Init(0, TargetCount - 1, TRUE);

	NullTrial = 0;
	ExposureTrial = 0;

	// Create list of trials.
	for (CurTrial = 1; (CurTrial <= TotalTrials); CurTrial++)
	{
		// Target angle and target position.
		TargetIndex = TargetPermute.GetNext();
		TargetAngleAZ = TargetAnglesAZ[TargetIndex];
		TargetAngleEL = TargetAnglesEL[TargetIndex];
		TargetPosition = HomePosition + (TargetAngleVector(TargetAngleAZ, TargetAngleEL) * TargetDistance);

		// Start and finish position of movement.
		//StartPosition = HomePosition;
		//FinishPosition = HomePosition;

		// Visual feedback flag.
		VisualFeedback = TRUE;

		// Set trial type depending on trial number, etc...
		int phase = getAxisPhase(CurTrial);
		if (phase > -1)
		{
			// Exposure trial.
			TrialType = TRIAL_EXPOSURE;
			RobotFieldType = ExposureFieldType;
			ExposureTrial++;
			CurRotationAxis(1, 1) = RotationAxisX[phase];
			CurRotationAxis(2, 1) = RotationAxisY[phase];
			CurRotationAxis(3, 1) = RotationAxisZ[phase];

			CurRotationAngle = RotationAngle[phase];

			AxisAngleToSO3(CurRotationAxis, CurRotationAngle, CurRotationMatrix);

			//printf("Rotation Matrix Set: \n %.2lf %.2lf %.2lf \n %.2lf %.2lf %.2lf \n %.2lf %.2lf %.2lf \n", CurRotationMatrix(1, 1), CurRotationMatrix(1, 2), CurRotationMatrix(1, 3), CurRotationMatrix(2, 1), CurRotationMatrix(2, 2), CurRotationMatrix(2, 3), CurRotationMatrix(3, 1), CurRotationMatrix(3, 2), CurRotationMatrix(3, 3));
		}
		else
		{
			// NULL trial (pre-exposure or post-exposure).
			TrialType = TRIAL_NULL;
			RobotFieldType = FIELD_NONE;
			CurRotationAxis(1, 1) = 0.0;
			CurRotationAxis(2, 1) = 0.0;
			CurRotationAxis(3, 1) = 0.0;
			CurRotationMatrix.zeros();
			CurRotationMatrix(1, 1) = 1.0;
			CurRotationMatrix(2, 2) = 1.0;
			CurRotationMatrix(3, 3) = 1.0;
			CurRotationAngle = 0.0;;
			NullTrial++;
		}

		// The following variables are filled in as trial is run...
		MissTrials = 0;
		MovementReactionTime = 0.0;
		MovementDurationTime = 0.0;

		// Save TrialData row for this trial.
		TrialData.RowSave(CurTrial);

	}


	printf("%d Total Trials, %d Null, %d Exposure.\n", TotalTrials, NullTrial, ExposureTrial);

	return(TRUE);
}

/******************************************************************************/

BOOL TrialList(void)
{
	int item, i;
	BOOL ok = TRUE;

	//TotalTrials = 0;


	if ((TotalTrials == 0) || !ok)
	{
		return(FALSE);
	}

	// Set rows of TrialData to the number of trials.
	TrialData.SetRows(TotalTrials);

	printf("Making list of %d trials (ESCape to abort)...\n", TotalTrials);

	// Loop over configuration files, appending each to growing trial list.

	if (!ConfigLoad(ConfigFile))
	{
		ok = FALSE;
	}

	// Create subset of trials for this configuration file.
	if (!BuildTrialList())
	{
		ok = FALSE;
	}

	if (!ok)
	{
		return(FALSE);
	}

	// Save trial list to file.
	ok = DATAFILE_Save(TrialListFile, TrialData);
	printf("%s %s Trials=%d.\n", TrialListFile, STR_OkFailed(ok), TrialData.GetRows());

	// Reset trial number, etc.
	CurTrial = 1;
	TrialSetup();
	ExperimentTimer.Reset();
	StateNext(STATE_INITIALIZE);

	return(TRUE);
}

/******************************************************************************/

void main(int argc, char *argv[])
{


	// Set base LOOPTASK function to specific frequency.
	// LOOPTASK_FREQUENCY(500.0);

	// Process command-line parameters.
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
	if (!ConfigLoad(ConfigFile))
	{
		exit(0);
	}

	// Initialize variables, etc.
	Initialize();

	// Create list of trials to run.
	TrialList();

	WAVE_SetVolume(10.0);

	WaveListPlay("ReminderBip");

	// Start the robot.
	if (DeviceStart())
	{
		printf("start ok!");
		// Start the graphics system.
		if (GraphicsStart())
		{
			// The experiment is run as part of graphics processing.
			GraphicsMainLoop();
		}
	}

	// Exit the program.
	ProgramExit();
}

/******************************************************************************/

