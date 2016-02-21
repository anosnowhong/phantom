/******************************************************************************/
/*                                                                            */
/* MODULE  : ROBOT-OBJECT.cpp                                                 */
/*                                                                            */
/* PURPOSE : Robot object - hardware interface.                               */
/*                                                                            */
/* DATE    : 01/Aug/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 01/Aug/2002 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 23/Nov/2006 - Changed internal ramping to use a RAMPER object.   */
/*                                                                            */
/* V2.0  JNI 25/Apr/2007 - Changes for Kalman Filter on Encoders.             */
/*                                                                            */
/******************************************************************************/

#include <motor.h>

/******************************************************************************/

struct  STR_TextItem  ENCODER_TypeText[] = 
{
    { ENCODER_TYPE_NONE          ,"NONE"            },
    { ENCODER_TYPE_DRC_T23B      ,"DRC T23B"        },
    { ENCODER_TYPE_MAXON_HEDS5500,"Maxon HEDS 5500" },
    { ENCODER_TYPE_IED_58SA      ,"IED 58SA"        },
    { ENCODER_TYPE_IED_40SA      ,"IED 40SA"        },
    { ENCODER_TYPE_IED_58SAx4    ,"IED 58SA (x4)"   },
    { ENCODER_TYPE_IED_40SAx4    ,"IED 40SA (x4)"   },
    { ENCODER_TYPE_IED_60HA      ,"IED 60HA"        },
    { ENCODER_TYPE_IED_60HAx4    ,"IED 60HA (x4)"   },
    { STR_TEXT_ENDOFTABLE },
};

double ENCODER_UnitsPerRevolution[] = { 0,10720,10500,30000,30000,4*30000,4*30000,65536,4*65536 };
int ENCODER_Multiplier[] = { 0,4,4,1,1,4,4,1,4 };

/******************************************************************************/

struct  STR_TextItem  MOTOR_TypeText[] = 
{
    { MOTOR_TYPE_NONE      ,"NONE"       },
    { MOTOR_TYPE_MAXON_RE25,"Maxon RE25" },
    { MOTOR_TYPE_MAXON_RE35,"Maxon RE35" },
    { MOTOR_TYPE_MAXON_RE40,"Maxon RE40" },
    { MOTOR_TYPE_MAXON_RE75,"Maxon RE75" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

void ROBOT_HardwareDetails( struct ROBOT_Hardware *hardware, PRINTF prnf )
{
int axis;

    (*prnf)("Controller=%s[0x%05X:0x%02X]-%s\n",STR_TextCode(CONTROLLER_HostCardText,hardware->Controller.CardType),hardware->Controller.Address,hardware->Controller.SubAddress,STR_TextCode(CONTROLLER_ControllerTypeText,hardware->Controller.ControllerType));

    for( axis=0; (axis < hardware->AxisCount); axis++ )
    {
        (*prnf)("Axis[%d] Motor=%s Encoder=%s\n",axis,STR_TextCode(MOTOR_TypeText,hardware->Axis[axis].Motor.Type),STR_TextCode(ENCODER_TypeText,hardware->Axis[axis].Encoder.Type));
    }
}

/******************************************************************************/

ROBOT::ROBOT( void )
{
}

/******************************************************************************/

ROBOT::ROBOT( struct ROBOT_Hardware *hardware, char *name )
{
    Init(hardware,name);
}

/******************************************************************************/

ROBOT::~ROBOT( void )
{
    // Close evetything up...
    Close();
}

/******************************************************************************/

void ROBOT::Init( struct ROBOT_Hardware *hardware, char *name )
{
int axis;

    OpenFlag = FALSE;
    StartedFlag = FALSE;
    TempTrak = NULL;
    Controller = NULL;
    LoopTaskTimer = NULL;
    Ramp = NULL;

    EncoderInit();
    MotorInit();

    strncpy(ObjectName,name,STRLEN);

    Hardware = *hardware;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        EncoderInit(axis,
                    Hardware.Axis[axis].Encoder.Type,
                    Hardware.Axis[axis].Encoder.Channel,
                    Hardware.Axis[axis].Encoder.Sign,
                    Hardware.Axis[axis].Gearing,
                    Hardware.Axis[axis].Encoder.KF_w,
                    Hardware.Axis[axis].Encoder.KF_v,
                    Hardware.Axis[axis].Encoder.KF_dt);

        MotorInit(axis,
                  Hardware.Axis[axis].Motor.Type,
                  Hardware.Axis[axis].Motor.Channel,
                  Hardware.Axis[axis].Gearing);
    }

    Ramp = new RAMPER(name);
}

/******************************************************************************/

int ROBOT::Messgf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ROBOT_PRN_messgf,STR_stringf("ROBOT(%s) %s",ObjectName,buff)));
}

/******************************************************************************/

int ROBOT::Errorf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ROBOT_PRN_errorf,STR_stringf("ROBOT(%s) %s",ObjectName,buff)));
}

/******************************************************************************/

int ROBOT::Debugf( const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ROBOT_PRN_debugf,STR_stringf("ROBOT(%s) %s",ObjectName,buff)));
}

/******************************************************************************/

int ROBOT::Printf( BOOL ok, const char *mask, ... )
{
va_list args;
static char buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ok,ROBOT_PRN_debugf,ROBOT_PRN_errorf,STR_stringf("ROBOT(%s) %s",ObjectName,buff)));
}

/******************************************************************************/

#define ROBOT_STEP_CONTROLLER     0
#define ROBOT_STEP_ENCODER        1
#define ROBOT_STEP_MOTOR          2
#define ROBOT_STEP_RAMP           3
#define ROBOT_STEP_TEMPTRAK       4

char   *ROBOT_OpenText[] = { "Controller","Encoder","Motor","Ramp","TempTrak" };

/******************************************************************************/

BOOL ROBOT::Open( void )
{
int step;
BOOL ok;

    for( ok=TRUE,step=ROBOT_STEP_CONTROLLER; ((step <= ROBOT_STEP_TEMPTRAK) && ok); step++ )
    {
        switch( step )
        {
            case ROBOT_STEP_CONTROLLER :
               ok = ControllerOpen();
               break;

            case ROBOT_STEP_ENCODER :
               ok = EncoderOpen();
               break;

            case ROBOT_STEP_MOTOR :
               ok = MotorOpen();
               break;

            case ROBOT_STEP_RAMP :
               if( Ramp == NULL )
               {
                   Ramp = new RAMPER(ObjectName);
               }
               break;

            case ROBOT_STEP_TEMPTRAK :
               ok = TempTrakOpen();
               break;
        }

        Printf(ok,"Open() %s %s.\n",ROBOT_OpenText[step],STR_OkFailed(ok));
    }

    OpenFlag = ok;

    if( !ok )
    {
        Close();
    }


    return(ok);
}

/******************************************************************************/

void ROBOT::Close( void )
{
int step;

    for( step=ROBOT_STEP_TEMPTRAK; (step >= ROBOT_STEP_CONTROLLER) && TRUE; step-- )
    {
        switch( step )
        {
            case ROBOT_STEP_CONTROLLER :
               ControllerClose();
               break;

            case ROBOT_STEP_ENCODER :
               EncoderClose();
               break;

            case ROBOT_STEP_MOTOR :
               MotorClose();
               break;

            case ROBOT_STEP_RAMP :
               if( Ramp != NULL )
               {
                   delete Ramp;
                   Ramp = NULL;
               }
               break;

            case ROBOT_STEP_TEMPTRAK :
               TempTrakClose();
               break;
        }

        Debugf("Close() %s.\n",ROBOT_OpenText[step]);
    }

    OpenFlag = FALSE;
}

/******************************************************************************/

BOOL ROBOT::Started( void )
{
BOOL flag=FALSE;

    if( OpenFlag )
    {
        flag = StartedFlag;
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT::Start( void )
{
int axis;

    // Make sure ROBOT object is open...
    if( !OpenFlag )
    {
        Errorf("Start() Not open.\n");
        return(FALSE);
    }

    // Check is already started...
    if( StartedFlag )
    {
        return(TRUE);
    }

    // Start controller...
    if( !Controller->Start() )
    {
        Errorf("Start() Cannot start controller.\n");
        return(FALSE);
    }

    // Set various start flags...
    StartedFlag = TRUE;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        Encoder[axis].StartedFlag = TRUE;
        Motor[axis].StartedFlag = TRUE;
    }

    // Reset LoopTask frequency timer...
    LoopTaskTimer->Reset();

    Debugf("Start()\n");

    return(TRUE);
}

/******************************************************************************/

void ROBOT::Stop( void )
{
int axis;

    // Make sure ROBOT object is open...
    if( !OpenFlag )
    {
        Errorf("Stop() Not open.\n");
        return;
    }

    // Print LoopTask frequenc timer results...
    if( StartedFlag )
    {
        LoopTaskTimer->Results(ROBOT_PRN_errorf);
    }

    // Stop controller...
    if( Controller->Started() )
    {
        if( !Controller->Stop() )
        {
            Errorf("Stop() Cannot stop controller.\n");
        }
    }

    // Clear various start flags...
    StartedFlag = FALSE;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        Encoder[axis].StartedFlag = FALSE;
        Motor[axis].StartedFlag = FALSE;
    }

    Debugf("Stop()\n");
}

/******************************************************************************/

BOOL ROBOT::Activated( void )
{
BOOL flag=FALSE;

    if( Controller != NULL )
    {
        flag = Controller->Activated();
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT::JustActivated( void )
{
BOOL flag=FALSE;

    if( Controller != NULL )
    {
        flag = Controller->JustActivated();
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT::JustDeactivated( void )
{
BOOL flag=FALSE;

    if( Controller != NULL )
    {
        flag = Controller->JustDeactivated();
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT::Safe( void )
{
BOOL flag=FALSE;

    if( Controller != NULL )
    {
        flag = Controller->Safe();
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT::ControllerOpen( void )
{
BOOL ok;

    if( Controller == NULL )
    {
        Controller = new CONTROLLER(Hardware.Controller.CardType,Hardware.Controller.Address,Hardware.Controller.SubAddress,Hardware.Controller.ControllerType,ObjectName);
    }

    if( LoopTaskTimer == NULL )
    {
        LoopTaskTimer = new TIMER_Frequency(ObjectName);
    }

    ok = Controller->Open();

    if( !ok )
    {
        ControllerClose();
    }

    Printf(ok,"ControllerOpen() %s.\n",STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

void ROBOT::ControllerClose( void )
{
    if( Controller != NULL )
    {
        Controller->Close();
        delete Controller;
        Controller = NULL;
    }

    if( LoopTaskTimer != NULL )
    {
        delete LoopTaskTimer;
        LoopTaskTimer = NULL;
    }
}

/******************************************************************************/

SENSORAY *ROBOT::Sensoray( void )
{
SENSORAY *S=NULL;

    if( Controller != NULL )
    {
        if( Controller->HostCard == CONTROLLER_HOSTCARD_SENSORAY )
        {
            S = Controller->Sensoray;
        }
    }

    return(S);
}

/******************************************************************************/

void ROBOT::EncoderInit( int axis )
{
    Encoder[axis].Type = ENCODER_TYPE_NONE;
    Encoder[axis].OpenFlag = FALSE;
    Encoder[axis].StartedFlag = FALSE;
    Encoder[axis].Channel = 0;
    Encoder[axis].Sign = 0;
    Encoder[axis].UnitsPerRevolution = 0.0;
    Encoder[axis].Gearing = 0.0;
    Encoder[axis].KF_w = 0.0;
    Encoder[axis].KF_v = 0.0;
    Encoder[axis].KF_dt = 0.0;
    Encoder[axis].Count = 0;
    Encoder[axis].dCount = 0;
    Encoder[axis].KF = NULL;
    Encoder[axis].KF_xraw = 0.0;
    Encoder[axis].KF_x = 0.0;
    Encoder[axis].KF_dx = 0.0;
}

/******************************************************************************/

void ROBOT::EncoderInit( void )
{
int axis;

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        EncoderInit(axis);
    }
}

/******************************************************************************/

void ROBOT::EncoderInit( int axis, int type, int channel, int sign, double gearing, double kf_w, double kf_v, double kf_dt )
{
    EncoderInit(axis);

    Debugf("ROBOT::EncoderInit(axis=%d,...,kf_w=%.3lf,kf_v=%.3lf,kf_dt=%.3lf)\n",axis,kf_w,kf_v,kf_dt);

    Encoder[axis].Type = type;
    Encoder[axis].Channel = channel;
    Encoder[axis].Sign = sign;
    Encoder[axis].UnitsPerRevolution = ENCODER_UnitsPerRevolution[type];
    Encoder[axis].Gearing = gearing;
    Encoder[axis].KF_w = kf_w;
    Encoder[axis].KF_v = kf_v;
    Encoder[axis].KF_dt = kf_dt;
    Encoder[axis].Count = 0;
    Encoder[axis].dCount = 0;
}

/******************************************************************************/

BOOL ROBOT::EncoderOpen( int axis )
{
BOOL ok=TRUE;

    if( Encoder[axis].OpenFlag )
    {
        return(TRUE);
    }

    if( Encoder[axis].Type != ENCODER_TYPE_NONE )
    {
        if( (Encoder[axis].KF_w * Encoder[axis].KF_v * Encoder[axis].KF_dt) != 0.0 )
        {
            if( Encoder[axis].KF == NULL )
            {
                Encoder[axis].KF = new KALMANFILTER(STR_stringf("%s-Encoder[%d]",ObjectName,axis));
            }

            Encoder[axis].KF->Open(Encoder[axis].KF_w,Encoder[axis].KF_v,Encoder[axis].KF_dt,0);
        }

        ok = Controller->EncoderOpen(Encoder[axis].Channel,ENCODER_Multiplier[Encoder[axis].Type]);
    }

    if( ok )
    {
        Encoder[axis].OpenFlag = TRUE;
        Encoder[axis].StartedFlag = TRUE;
    }

    return(ok);
}

/******************************************************************************/

BOOL ROBOT::EncoderOpen( void )
{
int axis;
BOOL ok;

    for( ok=TRUE,axis=0; ((axis < Hardware.AxisCount) && ok); axis++ )
    {
        ok = EncoderOpen(axis);
    }

    return(ok);
}

/******************************************************************************/

void ROBOT::EncoderClose( int axis )
{
    if( !Encoder[axis].OpenFlag )
    {
        return;
    }

    Encoder[axis].StartedFlag = FALSE;
    Encoder[axis].OpenFlag = FALSE;
    
    if( Encoder[axis].KF != NULL )
    {
        Encoder[axis].KF->Close();
        delete Encoder[axis].KF;
        Encoder[axis].KF = NULL;
    }
}

/******************************************************************************/

void ROBOT::EncoderClose( void )
{
int axis;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        EncoderClose(axis);
    }
}

/******************************************************************************/

BOOL ROBOT::EncoderStarted( int axis )
{
BOOL flag=FALSE;

    if( Encoder[axis].OpenFlag )
    {
        flag = Encoder[axis].StartedFlag;
    }

    return(flag);
}

/******************************************************************************/

void ROBOT::EncoderReset( int axis )
{
    if( !EncoderStarted(axis) )
    {
        return;
    }

    if( Encoder[axis].Type != ENCODER_TYPE_NONE )
    {
        Controller->EncoderReset(Encoder[axis].Channel);
    }

    Encoder[axis].Count = 0;
    Encoder[axis].dCount = 0;
}

/******************************************************************************/

void ROBOT::EncoderReset( void )
{
int axis;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        EncoderReset(axis);
    }
}

/******************************************************************************/

void ROBOT::EncoderRead( int axis, double &xraw, double &x, double &dx )
{
long units=0;

    if( !EncoderStarted(axis) )
    {
        return;
    }

    // Get encoder reading using controller-specific function...
    if( Encoder[axis].Type != ENCODER_TYPE_NONE )
    {
        units = Controller->EncoderGet(Encoder[axis].Channel);
    }

    // Adjust sign.
    units *= (long)Encoder[axis].Sign;
    
    // Change since last read...
    Encoder[axis].dCount = units - Encoder[axis].Count;
    Encoder[axis].Count = units;

    // Kalman Filter processing.
    Encoder[axis].KF_xraw = units;

    if( Encoder[axis].KF != NULL )
    {
    	Encoder[axis].KF->Calculate(Encoder[axis].KF_xraw,Encoder[axis].KF_x,Encoder[axis].KF_dx);
    }

    xraw = Encoder[axis].KF_xraw;
    x = Encoder[axis].KF_x;
    dx = Encoder[axis].KF_dx;
}

/******************************************************************************/

void ROBOT::EncoderRead( double xraw[], double x[], double dx[] )
{
int axis;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        EncoderRead(axis,xraw[axis],x[axis],dx[axis]);
    }
}

/******************************************************************************/

void ROBOT::EncoderRevolutions( int axis, double &rev_xraw, double &rev_x, double &rev_dx )
{
double xraw,x,dx;
double k;

    if( !EncoderStarted(axis) )
    {
        return;
    }

    k = Encoder[axis].UnitsPerRevolution * Encoder[axis].Gearing;

    EncoderRead(axis,xraw,x,dx);
    rev_xraw = xraw / k;
    rev_x = x / k;
    rev_dx = dx / k;
}

/******************************************************************************/

void ROBOT::EncoderRevolutions( double rev_xraw[], double rev_x[], double rev_dx[] )
{
int axis;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        EncoderRevolutions(axis,rev_xraw[axis],rev_x[axis],rev_dx[axis]);
    }
}

/******************************************************************************/

void ROBOT::EncoderRadians( int axis, double &rad_xraw, double &rad_x, double &rad_dx )
{
double rev_xraw,rev_x,rev_dx;

    EncoderRevolutions(axis,rev_xraw,rev_x,rev_dx);
    rad_xraw = r360 * rev_xraw;
    rad_x = r360 * rev_x;
    rad_dx = r360 * rev_dx;
}

/******************************************************************************/

void ROBOT::EncoderRadians( double rad_xraw[], double rad_x[], double rad_dx[] )
{
int axis;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        EncoderRadians(axis,rad_xraw[axis],rad_x[axis],rad_dx[axis]);
    }
}

/******************************************************************************/

void ROBOT::EncoderDegrees( int axis, double &deg_xraw, double &deg_x, double &deg_dx )
{
double rad_xraw,rad_x,rad_dx;

    EncoderRadians(axis,rad_xraw,rad_x,rad_dx);
    deg_xraw = R2D(rad_xraw);
    deg_x = R2D(rad_x);
    deg_dx = R2D(rad_dx);
}

/******************************************************************************/

void ROBOT::EncoderDegrees( double deg_xraw[], double deg_x[], double deg_dx[] )
{
int axis;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        EncoderDegrees(axis,deg_xraw[axis],deg_x[axis],deg_dx[axis]);
    }
}

/******************************************************************************/

char *ROBOT::EncoderText( int axis )
{
    return(STR_TextCode(ENCODER_TypeText,Encoder[axis].Type));
}

/******************************************************************************/

void ROBOT::MotorInit( int axis )
{
    Motor[axis].Type = MOTOR_TYPE_NONE;
    Motor[axis].OpenFlag = FALSE;
    Motor[axis].StartedFlag = FALSE;
}

/******************************************************************************/

void ROBOT::MotorInit( void )
{
int axis;

    for( axis=0; (axis < ROBOT_DOFS); axis++ )
    {
        MotorInit(axis);
    }
}

/******************************************************************************/

void ROBOT::MotorInit( int axis, int type, int channel, double gearing )
{
    MotorInit(axis);

    Motor[axis].Type = type;
    Motor[axis].Channel = channel;
    Motor[axis].Gearing = gearing;
    Motor[axis].UnitsPerNm = 0.0;
    Motor[axis].Units = 0;
}

/******************************************************************************/

BOOL ROBOT::MotorOpen( int axis )
{
BOOL ok;

    if( Motor[axis].OpenFlag )
    {
        return(TRUE);
    }

    if( (ok=Controller->MotorOpen(Motor[axis].Channel)) )
    {
        Controller->MotorReset(Motor[axis].Channel);
        Motor[axis].OpenFlag = TRUE;
        Motor[axis].StartedFlag = TRUE;
    }

    return(ok);
}

/******************************************************************************/

BOOL ROBOT::MotorOpen( void )
{
int axis;
BOOL ok;

    for( ok=TRUE,axis=0; ((axis < Hardware.AxisCount) && ok); axis++ )
    {
        ok = MotorOpen(axis);
    }

    return(ok);
}

/******************************************************************************/

void ROBOT::MotorClose( int axis )
{
    if( !Motor[axis].OpenFlag )
    {
        return;
    }

    Controller->MotorReset(Motor[axis].Channel);
    Motor[axis].StartedFlag = FALSE;
    Motor[axis].OpenFlag = FALSE;
}

/******************************************************************************/

void ROBOT::MotorClose( void )
{
int axis;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        MotorClose(axis);
    }
}

/******************************************************************************/

BOOL ROBOT::MotorStarted( int axis )
{
BOOL flag=FALSE;

    if( Motor[axis].OpenFlag )
    {
        flag = Motor[axis].StartedFlag;
    }

    return(flag);
}

/******************************************************************************/

void ROBOT::MotorReset( int axis )
{
    if( !MotorStarted(axis) )
    {
        return;
    }

    Controller->MotorReset(Motor[axis].Channel);
    Motor[axis].Units = 0;
}

/******************************************************************************/

void ROBOT::MotorReset( void )
{
int axis;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        MotorReset(axis);
    }
}

/******************************************************************************/

void ROBOT::MotorSet( int axis, long units )
{
    if( !MotorStarted(axis) )
    {
        return;
    }

    Controller->MotorSet(Motor[axis].Channel,units);
    Motor[axis].Units = units;
}

/******************************************************************************/

void ROBOT::MotorSet( long units[] )
{
int axis;

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        MotorSet(axis,units[axis]);
    }
}

/******************************************************************************/

char *ROBOT::MotorText( int axis )
{
    return(STR_TextCode(MOTOR_TypeText,Motor[axis].Type));
}

/******************************************************************************/

BOOL ROBOT::TempTrakOpen( void )
{
BOOL ok;

    if( Hardware.Controller.ControllerType == CONTROLLER_TYPE_NONE )
    {
        return(TRUE);
    }

    if( TempTrak == NULL )
    {
        TempTrak = new TEMPTRAK(Controller->Name(),Hardware.AxisCount);
    }

    ok = TempTrak->Opened();

    if( !ok )
    {
        TempTrakClose();
    }

    Printf(ok,"TempTrakOpen() %s.\n",STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

void ROBOT::TempTrakClose( void )
{
    if( TempTrak != NULL )
    {
        delete TempTrak;
        TempTrak = NULL;
    }
}

/******************************************************************************/

BOOL ROBOT::TempTrakStart( void )
{
BOOL ok=TRUE;

    TempTrakState = ROBOT_TEMPTRAK_INIT;

    if( TempTrak != NULL )
    {
        ok = TempTrak->Start(Frequency);
        Printf(ok,"TempTrak->Start() %s.\n",STR_OkFailed(ok));
    }

    if( !ok )
    {
        TempTrakStop();
    }

    return(ok);
}

/******************************************************************************/

BOOL ROBOT::TempTrakStarted( void )
{
BOOL flag=FALSE;

    if( TempTrak != NULL )
    {
        flag = TempTrak->Started();
    }

    return(flag);
}

/******************************************************************************/

BOOL ROBOT::TempTrakStop( void )
{
BOOL ok=TRUE;

    if( TempTrakStarted() )
    {
        ok = TempTrak->Stop();
        Printf(ok,"TempTrak->Stop() %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

BOOL ROBOT::TempTrakCooling( void )
{
BOOL flag;

    flag = (TempTrakState == ROBOT_TEMPTRAK_COOLING);

    return(flag);
}

/******************************************************************************/

void ROBOT::TempTrakLoopTask( void )
{
int axis;
long DAC[ROBOT_DOFS];

    if( !TempTrakStarted() )
    {
        return;
    }

    for( axis=0; (axis < Hardware.AxisCount); axis++ )
    {
        DAC[axis] = Motor[axis].Units;
    }

    TempTrak->Update(DAC);

    switch( TempTrakState )
    {
        case ROBOT_TEMPTRAK_INIT :
           TempTrakState = ROBOT_TEMPTRAK_RUNNING;
           break;

        case ROBOT_TEMPTRAK_RUNNING :
           // Check motor temperature...
           if( TempTrak->OverHeated() )
           {
               Errorf("Motors over-heated.\n");
               TempTrakState = ROBOT_TEMPTRAK_COOLING;
           }
           break;

        case ROBOT_TEMPTRAK_COOLING :
           // Wait for motors to cool...
           if( TempTrak->Cooled() )
           {
               Errorf("Motors cool.\n");
               TempTrakState = ROBOT_TEMPTRAK_RESUME;
           }
           break;

        case ROBOT_TEMPTRAK_RESUME :
           TempTrakState = ROBOT_TEMPTRAK_RUNNING;
           break;

        case ROBOT_TEMPTRAK_DISABLED :
           break;
    }
}

/******************************************************************************/

void ROBOT::LoopTask( void )
{
    // Make sure handle is open...
    if( !OpenFlag )
    {
        return;
    }

    // Call controller's LoopTask function.
    Controller->LoopTask();

    // Call robot object's temperature tracking LoopTask function...
    TempTrakLoopTask();

    // Time frequency of LoopTask...
    LoopTaskTimer->Loop();

}

/******************************************************************************/

void ROBOT::LoopTaskFrequency( double frequency )
{
    Frequency = frequency;
}

/******************************************************************************/

