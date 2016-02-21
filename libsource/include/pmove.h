/******************************************************************************/
/*                                                                            */ 
/* MODULE  : PMove.h                                                          */ 
/*                                                                            */ 
/* PURPOSE : Passive Movement with robot.                                     */ 
/*                                                                            */ 
/* DATE    : 01/Nov/2007                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JJ  01/Nov/2007 - Initial development at ROBOT_PassiveTrajectory.    */ 
/*                                                                            */ 
/* V1.0  JNI 19/Aug/2015 - Tweaked, renamed to PMOVE, moved into MOTOR.LIB.   */ 
/*                                                                            */ 
/******************************************************************************/

#define PMOVE_AXISCOUNT   10  // Max

#define PMOVE_HOLDTIME    0.0 // Default value (sec)
#define PMOVE_RAMPTIME    0.1 // Default value (sec)

/******************************************************************************/

class PMOVE
{
public:
    PMOVE( void );
   ~PMOVE( void );

    BOOL PMOVE::Open( int axiscount, double movetime, matrix &springconstant, matrix &positiontolerance, matrix &velocitytolerance, double holdtime, double ramptime );
    BOOL PMOVE::Open( int axiscount, double movetime, matrix &springconstant, matrix &positiontolerance, matrix &velocitytolerance, double ramptime );
    BOOL PMOVE::Open( int axiscount, double movetime, matrix &springconstant, matrix &positiontolerance, matrix &velocitytolerance );

    BOOL PMOVE::Open( int axiscount, double movetime, double springconstant, double positiontolerance, double velocitytolerance, double holdtime, double ramptime );
    BOOL PMOVE::Open( int axiscount, double movetime, double springconstant, double positiontolerance, double velocitytolerance, double ramptime );
    BOOL PMOVE::Open( int axiscount, double movetime, double springconstant, double positiontolerance, double velocitytolerance );

    BOOL Start( matrix &startposition, matrix &endposition );

    void StateNext( int state );

    BOOL Update( matrix &RobotPosition, matrix &RobotVelocity, matrix &RobotForces );

    void Stop( void );

    BOOL NotFinished( void );
    BOOL Finished( void );

    void CurrentState( int &state, double &currenttime, double &rampvalue, matrix &position );

private:	

#define PMOVE_STATE_STOPPED   0
#define PMOVE_STATE_IDLE      1
#define PMOVE_STATE_MOVING    2
#define PMOVE_STATE_FINISH    3
#define PMOVE_STATE_HOLD      4
#define PMOVE_STATE_RAMPDOWN  5

    int    State;
    int    StateLast;
    TIMER  StateTimer;

    TIMER  Timer;

    double MoveTime;
    double HoldTime;
    double RampTime;
    double RampValue;
    double CurrentTime;

    int    AxisCount;

    matrix StartPosition;
    matrix EndPosition;
    matrix TrajectoryDistance;
    matrix SpringConstant;
    matrix PositionTolerance;
    matrix VelocityTolerance;
    TIMER  VelocityToleranceTimer[PMOVE_AXISCOUNT];
    double VelocityToleranceTime;
    matrix TrajectoryPosition;
    matrix SpringStretch;

};

/******************************************************************************/

