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

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

void *PMOVE_StateText[] = { "Stopped","Idle","Moving","Finish","Hold","RampDown",NULL };

/******************************************************************************/

PMOVE::PMOVE( void )
{
    State = PMOVE_STATE_STOPPED;

    MoveTime = 0.0;
    HoldTime = 0.0;
    RampTime = 0.0;

    AxisCount = 0;
}

/******************************************************************************/

PMOVE::~PMOVE( void )
{
}

/******************************************************************************/

BOOL PMOVE::Open( int axiscount, double movetime, matrix &springconstant, matrix &positiontolerance, matrix &velocitytolerance, double holdtime, double ramptime )
{
int i;

    AxisCount = axiscount;
    SpringConstant = springconstant;
    MoveTime = movetime;
    PositionTolerance = positiontolerance;
    VelocityTolerance = velocitytolerance;
    VelocityToleranceTime = 0.02;
    HoldTime = holdtime;
    RampTime = ramptime;
    RampValue = 0.0;

    Timer.Reset();

    StartPosition.dim(AxisCount,1);
    EndPosition.dim(AxisCount,1);
    TrajectoryDistance.dim(AxisCount,1);
    TrajectoryPosition.dim(AxisCount,1);
    SpringStretch.dim(AxisCount,1);

    if( (AxisCount == 0) || (MoveTime == 0.0) )
    {
        return(FALSE);
    }

    for( i=1; (i <= AxisCount); i++ )
    {
        if( SpringConstant(i,1) > 0.0 )
        {
            return(FALSE);
        }
    }

    StateNext(PMOVE_STATE_IDLE);

    return(TRUE);
}

/******************************************************************************/

BOOL PMOVE::Open( int axiscount, double movetime, matrix &springconstant, matrix &positiontolerance, matrix &velocitytolerance, double ramptime )
{
BOOL ok=FALSE;

    return(ok);
}

/******************************************************************************/

BOOL PMOVE::Open( int axiscount, double movetime, matrix &springconstant, matrix &positiontolerance, matrix &velocitytolerance )
{
BOOL ok=FALSE;

    return(ok);
}

/******************************************************************************/

BOOL PMOVE::Open( int axiscount, double movetime, double springconstant, double positiontolerance, double velocitytolerance, double holdtime, double ramptime )
{
BOOL ok=FALSE;

    return(ok);
}

/******************************************************************************/

BOOL PMOVE::Open( int axiscount, double movetime, double springconstant, double positiontolerance, double velocitytolerance, double ramptime )
{
BOOL ok=FALSE;

    return(ok);
}

/******************************************************************************/

BOOL PMOVE::Open( int axiscount, double movetime, double springconstant, double positiontolerance, double velocitytolerance )
{
BOOL ok=FALSE;

    return(ok);
}

/******************************************************************************/

BOOL PMOVE::Start( matrix &startposition, matrix &endposition )
{
BOOL ok=FALSE;

    Stop();

    if( State == PMOVE_STATE_IDLE )
    {
        StartPosition = startposition;
        EndPosition = endposition;
        TrajectoryDistance = EndPosition - StartPosition;

        RampValue = 1.0;

        Timer.Reset();
        StateNext(PMOVE_STATE_MOVING);

        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

void PMOVE::StateNext( int state )
{
    printf("PMOVE-STATE: %s[%d] > %s[%d] (%.0lf msec).\n",PMOVE_StateText[State],State,PMOVE_StateText[state],state,StateTimer.Elapsed());
    StateTimer.Reset();
    StateLast = State;
    State = state;
}

/******************************************************************************/

BOOL PMOVE::Update( matrix &RobotPosition, matrix &RobotVelocity, matrix &RobotForces )
{
int i;
double y;
BOOL done;

    CurrentTime = Timer.ElapsedSeconds();

    SpringStretch.zeros();
    RobotForces.zeros();

    if( NotFinished() )
    {
        SpringStretch = (RobotPosition - TrajectoryPosition);
    }
		
    switch( State )
    {
        case PMOVE_STATE_STOPPED :
            break;

        case PMOVE_STATE_IDLE :
            break;

        case PMOVE_STATE_MOVING :
            if( CurrentTime > MoveTime ) 
            {	
                StateNext(PMOVE_STATE_FINISH);	
                TrajectoryPosition = EndPosition;
                break;
            }

            y = (1.0+sin((PI*(CurrentTime/MoveTime))-(PI/2)))/2;

            for( i=1; (i <= AxisCount); i++ )
            {
                TrajectoryPosition(i,1) = StartPosition(i,1) + (y * TrajectoryDistance(i,1));
            }

            SpringStretch = (RobotPosition - TrajectoryPosition);
            break;

        case PMOVE_STATE_FINISH :
            for( done=TRUE,i=1; ((i <= AxisCount) && done); i++ )
            {
                if( (PositionTolerance(i,1) != 0.0) && (fabs(SpringStretch(i,1)) > PositionTolerance(i,1)) )
                {
                    /*if( TIMER_EveryHz(10.0) )
                    {
                        printf("PMOVE: PositionTolerance(%d)=%0.3lf, fabs(SpringStretch(%d))=%.3lf\n",i,PositionTolerance(i,1),i,fabs(SpringStretch(i,1)));
                    }*/

                    done = FALSE;
                }

                if( (VelocityTolerance(i,1) != 0.0) && (fabs(RobotVelocity(i,1)) > VelocityTolerance(i,1)) )
                {
                    /*if( TIMER_EveryHz(10.0) )
                    {
                        printf("PMOVE: VelocityTolerance(%d)=%0.3lf, fabs(RobotVelocity(%d))=%.3lf\n",i,VelocityTolerance(i,1),i,fabs(RobotVelocity(i,1)));
                    }*/

                    VelocityToleranceTimer[i].Reset();
                }
 
                if( (VelocityTolerance(i,1) != 0.0) && !VelocityToleranceTimer[i].ExpiredSeconds(VelocityToleranceTime) )
                {
                    done = FALSE;
                }
            }

            if( done )
            {
/*for( i=1; (i <= AxisCount); i++ )
{
    if( PositionTolerance(i,1) != 0.0 )
    {
        printf("PMOVE: Axis=%d PositionTolerance=%0.3lf, fabs(SpringStretch)=%.3lf\n",i,PositionTolerance(i,1),fabs(SpringStretch(i,1)));
    }

    if( VelocityTolerance(i,1) != 0.0 )
    {
        printf("PMOVE: Axis=%d VelocityTolerance=%0.3lf, fabs(RobotVelocity)=%.3lf\n",i,VelocityTolerance(i,1),fabs(RobotVelocity(i,1)));
    }
}*/
                StateNext((HoldTime == 0.0) ? PMOVE_STATE_RAMPDOWN : PMOVE_STATE_HOLD);
                Timer.Reset();
            }
            break;

        case PMOVE_STATE_HOLD :
            if( Timer.ExpiredSeconds(HoldTime) )
            {
                StateNext(PMOVE_STATE_RAMPDOWN);
                Timer.Reset();
            }
            break;

        case PMOVE_STATE_RAMPDOWN :
            if( (CurrentTime >= RampTime) || (RampValue < 0.0) )
            {
                StateNext(PMOVE_STATE_IDLE);
                RampValue = 0.0;
                break;
            }

            RampValue = 1.0 - (CurrentTime/RampTime);
            break;
    }

    if( NotFinished() )
    {
        for( i=1; (i <= AxisCount); i++ )
        {
            RobotForces(i,1) = RampValue * SpringConstant(i,1) * SpringStretch(i,1);
        }
    }

    done = !Finished();

    return(done);
}

/******************************************************************************/

void PMOVE::Stop( void )
{
    StateNext(PMOVE_STATE_IDLE);
    RampValue = 0.0;
}

/******************************************************************************/

void PMOVE::CurrentState( int &state, double &currenttime, double &rampvalue, matrix &position )
{
    state = State;
    currenttime = CurrentTime;
    rampvalue = RampValue;
    position = TrajectoryPosition;
}

/******************************************************************************/

BOOL PMOVE::NotFinished( void )
{
BOOL flag;

    flag = ((State >= PMOVE_STATE_MOVING) && (State <= PMOVE_STATE_RAMPDOWN));

    return(flag);
}

/******************************************************************************/

BOOL PMOVE::Finished( void )
{
BOOL flag;

    flag = !NotFinished();

    return(flag);
}

/******************************************************************************/

