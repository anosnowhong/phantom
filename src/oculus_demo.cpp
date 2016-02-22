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

STRING RobotName="";
int    RobotID=ROBOT_INVALID;
matrix RobotPosition(3,1);             // cm
matrix MousePosition(3, 1);
matrix RobotPositionRaw(3,1);          // cm
matrix RobotPositionOffset(3,1);       // cm
matrix RobotVelocity(3,1);             // cm/sec
matrix RobotForces(3,1);               // N
double RobotForceMax=5.0;              // N
double RobotSpringConstant=-1.0;       // N/cm
double RobotForceMagnitude=0.0;        // N

TIMER_Frequency RobotLoopFrequencyTimer("RobotLoopFrequency");
TIMER_Interval  RobotLoopLatencyTimer("RobotLoopLatency");

matrix SpherePosition(3,1);
matrix SpherePosition_Goal(3,1);
double SphereRadius=10.0; // cm
int    SphereColor=GREEN;
BOOL   SphereInsideFlag=FALSE;
BOOL   draw_MOUSE=FALSE;

double CursorRadius=1.0; // cm
int    CursorColor=RED;

// Graphics Monitor Window variables.
int    MonitorX=100;     // Monitor window X-position (pixels)
int    MonitorY=100;     // Monitor window Y-position (pixels)
int    MonitorWid=480;   // Monitor window width (pixels)
int    MonitorViews=4;   // Monitor window views (can be 1 or 4).
double MonitorFreq=60.0; // Monitor window display rate (Hz)

// Text Display variables.
matrix TextPosition(3,1);
matrix TextRotation(3,1);
int    TextColor=WHITE;
float  TextSize=0.75;

/******************************************************************************/

void ProgramExit( void );
void RobotStop( void );

/******************************************************************************/

void GraphicsTimingResults( void )
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

void GraphicsIdle( void )
{
    GraphicsIdleFrequencyTimer.Loop();

    // Text displaying force magnitude.
    if( RobotForceMagnitude > 0.0 )
    {
        GRAPHICS_TextSet(STR_stringf("Force=%.1lfN",RobotForceMagnitude));
    }
    else
    {
        GRAPHICS_TextClear();
    }
}

/******************************************************************************/

void GraphicsDraw( void )
{
    GraphicsDrawFrequencyTimer.Loop();
    GraphicsDrawLatencyTimer.Before();

    // Fixed object.
    GRAPHICS_WireSphere(&SpherePosition,SphereRadius,SphereColor);
    //GRAPHICS_WireSphere(&SpherePosition_Goal,SphereRadius,SphereColor);
	if (draw_MOUSE == TRUE)
	{
		//Mouse cursor
		MOUSE_GetPosn(MousePosition);
		GRAPHICS_Sphere(&MousePosition, CursorRadius, CursorColor);
	}
	else
	{
		// Robot cursor.
		GRAPHICS_Sphere(&RobotPosition, CursorRadius, CursorColor);
	}

    GraphicsDrawLatencyTimer.Before();
}

/******************************************************************************/

void GraphicsKeyboard( BYTE key, int x, int y )
{
    // Process keyboard input.
    if( key == ESC )
    {
        ProgramExit();
    }
}

/******************************************************************************/

void GraphicsStart( void )
{
    // Start the graphics system.
	if (!GRAPHICS_GraphicsStart())
    {
        printf("Cannot start graphics system.\n");
        return;
    }

    // Start the graphics monitor window.
    if( !GRAPHICS_MonitorStart(MonitorX,MonitorY,MonitorWid,MonitorViews,MonitorFreq) )
    {
        printf("Cannot start graphics monitor window.\n");
        return;
    }

    // Default position for text display.
    TextPosition.zeros();      // Centred in the workspace above the green sphere.

    // Default rotation for text display.
    TextRotation.zeros();
    TextRotation(1,1) = 40.0;  // Rotate text forward (x-axis) for better visibility.

    // Set default values for text display.
    GRAPHICS_TextDefault(&TextPosition,&TextRotation,TextColor,TextSize);

    // Register call-back functions and pass control to graphics system.
    GRAPHICS_MainLoop(GraphicsKeyboard,GraphicsDraw,GraphicsIdle);
}

/******************************************************************************/

void RobotForcesFunction( matrix &P, matrix &V, matrix &F )
{
int i;
static matrix D(3,1);
static double d,e;

    RobotLoopFrequencyTimer.Loop();
    RobotLoopLatencyTimer.Before();

    RobotPositionRaw = P;
    RobotPosition = P;
    RobotVelocity = V;
    RobotForces.zeros();

	//printf("%f", RobotPosition(3, 1));
	RobotPosition(3, 1) += 45;
	RobotPosition(2, 1) -= 20;
	RobotPosition(1, 1) += 20;
	//printf("===========%f", RobotPosition(3, 1));
	D = RobotPosition - SpherePosition;
    d = norm(D);
    e = d - SphereRadius; // Scalar encroachment into surface of sphere.

    if( ROBOT_JustActivated(RobotID) )
    {
        SphereInsideFlag = (e < 0);
    }

    if( SphereInsideFlag )
    {
        e = d - (SphereRadius-CursorRadius);
    }
    else
    {
        e = d - (SphereRadius+CursorRadius);
    }

    if( ROBOT_Activated(RobotID) && ((SphereInsideFlag && (e > 0.0)) || (!SphereInsideFlag && (e < 0.0))) )
    {
        RobotForces = RobotSpringConstant * e * (D/d);
    }

    RobotForces.clampnorm(RobotForceMax);
    RobotForceMagnitude = norm(RobotForces);

    F = RobotForces; 

    RobotLoopLatencyTimer.After();
}

/******************************************************************************/

BOOL RobotStart( void )
{
BOOL ok=FALSE;

    if( (RobotID=ROBOT_Open(RobotName)) == ROBOT_INVALID )
    {
        printf("Cannot open: \n",RobotName);
    }
    else
    if( !ROBOT_Start(RobotID,RobotForcesFunction) )
    {
        printf("Cannot start: %s\n",RobotName);
    }
    else
    {
        ok = TRUE;
        printf("Robot started: %s\n",RobotName);
    }

    return(ok);
}

/******************************************************************************/

void RobotStop( void )
{
    ROBOT_Stop(RobotID);
    ROBOT_Close(RobotID);
}

/******************************************************************************/

void RobotTimingResults( void )
{
    RobotLoopFrequencyTimer.Results();
    RobotLoopLatencyTimer.Results();
}

/******************************************************************************/

void ProgramExit( void )
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

void Usage( void )
{
    printf("Usage:\n\n");
    printf("%s /R:RobotName\n",MODULE_NAME);

    exit(0);
}

/******************************************************************************/

BOOL Parameters( int argc, char *argv[] )
{
BOOL ok;
char *data;
int i;

    for( ok=TRUE,i=1; ((i < argc) && ok); i++ )
    {
        switch( CMDARG_code(argv[i],&data) )
        {
            case 'R' :
               ok = CMDARG_data(RobotName,data,STRLEN);
               break;

			case 'O' :
				draw_MOUSE = TRUE;
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

    if( ok && STR_null(RobotName) )
    {
        printf("Robot name not specified.\n");
        ok = FALSE;
    }

    if( !ok )
    {
        Usage();
    }

    return(ok);
}

/******************************************************************************/

void main( int argc, char *argv[] )
{
    // Process command-line parameteres.
    if( !Parameters(argc,argv) )
    {
        exit(0);
    }

    // Position of sphere in simulation.
    SpherePosition.zeros();
    //SpherePosition_Goal.zeros();
	//RobotFakePosition.zeros();
    
	SpherePosition(3, 1) = -10.0;
	//SpherePosition_Goal(2, 1) += 30.0;

    // Start robot.
    if( RobotStart() )
    {
        // Start graphics.
        GraphicsStart();
    }
}

/******************************************************************************/

