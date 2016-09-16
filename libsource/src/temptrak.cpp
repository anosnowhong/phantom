/******************************************************************************/
/*                                                                            */
/* MODULE  : TempTrack.cpp                                                    */
/*                                                                            */
/* PURPOSE : Temperature Tracking API (for motors).                           */
/*                                                                            */
/* DATE    : 15/Feb/2001                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 13/Oct/2001 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "TEMPTRAK"
#define MODULE_TEXT     "Temperature Tracking API"
#define MODULE_DATE     "13/10/2002"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL     3

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

#define TEMPTRAK_errorf ROBOT_errorf
#define TEMPTRAK_messgf ROBOT_messgf
#define TEMPTRAK_debugf ROBOT_debugf

/******************************************************************************/

TEMPTRAK::TEMPTRAK( char *name, int axis, char *config )
{
int item;

    OpenFlag = FALSE;

    // Initialize object...
    OpenFlag = Init(name,axis,config);

    STR_printf(OpenFlag,TEMPTRAK_debugf,TEMPTRAK_errorf,"TEMPTRAK(%s) %s.\n",name,STR_OkFailed(OpenFlag));
}

/******************************************************************************/

TEMPTRAK::TEMPTRAK( char *name, int axis )
{
int item;

    OpenFlag = FALSE;

    // Initialize object...
    OpenFlag = Init(name,axis);

    STR_printf(OpenFlag,TEMPTRAK_debugf,TEMPTRAK_errorf,"TEMPTRAK(%s) %s.\n",name,STR_OkFailed(OpenFlag));
}

/******************************************************************************/

TEMPTRAK::~TEMPTRAK( void )
{
    // Make sure the object is stopped...
    Stop();
}

/******************************************************************************/

void TEMPTRAK::Init( void )
{
int axis;

    memset(ObjectName,0,STRLEN);
    memset(ConfigFile,0,STRLEN);
    memset(ConfigPath,0,STRLEN);
    OpenFlag = FALSE;
    StartedFlag = FALSE;
    Axis = 0;
    dt = 0.0;
    FreqTimer = NULL;
}

/******************************************************************************/

BOOL TEMPTRAK::Init( char *name, int axis, char *config )
{
BOOL ok;

    Init();

    strncpy(ObjectName,name,STRLEN);
    Axis = axis;

    if( config != NULL )
    {
        strncpy(ConfigFile,config,STRLEN);
    }
    else
    {
        strncpy(ConfigFile,STR_stringf("%s.TMP",ObjectName),STRLEN);
    }

    ok = FILE_Calibration(ConfigFile,ConfigPath);

    return(ok);
}

/******************************************************************************/

BOOL TEMPTRAK::Init( char *name, int axis )
{
BOOL ok=FALSE;

    // Initialize object...
    ok = Init(name,axis,NULL);

    return(ok);
}

/******************************************************************************/

void TEMPTRAK::ConfigSet( void )
{
    CONFIG_reset();

    CONFIG_set("T_Air",T_Air);
    CONFIG_set("ER_Cu",ER_Cu,TEMPTRAK_AXIS);
    CONFIG_set("DAC2I",DAC2I,TEMPTRAK_AXIS);
    CONFIG_set("TR_Cu2Fe",TR_Cu2Fe,TEMPTRAK_AXIS);
    CONFIG_set("TR_Fe2Air",TR_Fe2Air,TEMPTRAK_AXIS);
    CONFIG_set("MassCu",MassCu,TEMPTRAK_AXIS);
    CONFIG_set("MassFe",MassFe,TEMPTRAK_AXIS);
    CONFIG_set("T_OverHeat",T_OverHeat);
    printf("overheat value : %G",T_OverHeat);
    CONFIG_set("T_Cooled",T_Cooled);
    CONFIG_set("T_Cu",T_Cu,TEMPTRAK_AXIS);
    CONFIG_set("T_Fe",T_Fe,TEMPTRAK_AXIS);
    CONFIG_set("t_Save",t_Save);
}

/******************************************************************************/

BOOL TEMPTRAK::ConfigLoad( void )
{
BOOL ok;

    ConfigSet();

    if( !CONFIG_read(ConfigPath) )
    {
        return(FALSE);
    }

    TEMPTRAK_debugf("TEMPTRAK::ConfigLoad(%s) OK.\n",ConfigPath);
    CONFIG_list(TEMPTRAK_debugf);

    return(TRUE);
}

/******************************************************************************/

BOOL TEMPTRAK::ConfigSave( void )
{
BOOL ok;

    ConfigSet();

    if( !CONFIG_write(ConfigPath) )
    {
        return(FALSE);
    }

    TEMPTRAK_debugf("TEMPTRAK::ConfigSave(%s) OK.\n",ConfigPath);
    CONFIG_list(TEMPTRAK_debugf);

    return(TRUE);
}

/******************************************************************************/

BOOL TEMPTRAK::Opened( void )
{
BOOL flag;

    flag = OpenFlag;

    return(flag);
}

/******************************************************************************/

BOOL TEMPTRAK::Started( void )
{
BOOL flag=FALSE;

    if( Opened() )
    {
        flag = StartedFlag;
    }

    return(flag);
}

/******************************************************************************/

BOOL TEMPTRAK::Resume( int func )
{
BOOL ok=FALSE;
int axis;
double elapsed,t,step;
STRING text;

    // Make sure object has been opened...
    if( !Opened() )
    {
        TEMPTRAK_errorf("TEMPTRAK::Resume(%s) Not Opened.\n",FILE_Func(func));
        return(FALSE);
    }

    strncpy(text,STR_stringf("TEMPTRAK::Resume(%s=%s)",FILE_Func(func),ConfigPath),STRLEN);

    switch( func )
    {
        case FILE_LOAD :
           if( !(ok=ConfigLoad()) )
           {
               break;
           }

           elapsed = TIMER_seconds() - t_Save;

           // Set to room temperature...
           if( seconds2minutes(elapsed) > 30.0 )
           {
               TEMPTRAK_debugf("%s Setting to air temperature.\n",text);

               for( axis=0; (axis < Axis); axis++ )
               {
                   T_Cu[axis] = T_Air;
                   T_Fe[axis] = T_Air;
               }

               break;
           }

           // Simulate elapsed time...
           TEMPTRAK_debugf("%s Simulate %.1lf minutes elapsed time.\n",text,seconds2minutes(elapsed));
           step = 0.01;

           for( t=0; (t < elapsed); t += step )
           {
               for( axis=0; (axis < Axis); axis++ )
               {
                   dTdt(axis,0,step);
               }
           }
           break;

        case FILE_SAVE :
           t_Save = TIMER_seconds();
           ok = ConfigSave();
           break;
    }

    STR_printf(ok,TEMPTRAK_debugf,TEMPTRAK_errorf,"%s %s.\n",text,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL TEMPTRAK::Start( double frequency )
{
BOOL ok;
int axis;

    // Object not opened...
    if( !Opened() )
    {
        return(FALSE);
    }

    // Already started?
    if( Started() )
    {
        return(TRUE);
    }

    // Set time-step based on LoopTask frequency...
    dt = 1.0 / frequency;

    // Assume motors equalibriated...
    for( axis=0; (axis < Axis); axis++ )
    {
        T_Cu[axis] = T_Air;
        T_Fe[axis] = T_Air;
    }

    // Time frequency of LoopTask...
    FreqTimer = new TIMER_Frequency(STR_stringf("TEMPTRAK %s",ObjectName));

    // Resume temperature tracking...
    ok = Resume(FILE_LOAD);

    StartedFlag = TRUE;

    TEMPTRAK_debugf("TEMPTRAK::Start() dt=%0.04lf\n",dt);

    FreqTimer->Reset();

    return(TRUE);
}

/******************************************************************************/

BOOL TEMPTRAK::Stop( void )
{
BOOL ok=FALSE;

    // Object not opened...
    if( !Opened() )
    {
        return(FALSE);
    }

    // Already stopped?
    if( !Started() )
    {
        return(TRUE);
    }

    StartedFlag = FALSE;

    // Frequency timer...
    if( FreqTimer != NULL )
    {
        //FreqTimer->Calculate();
        //FreqTimer->Results(TEMPTRAK_messgf);
        delete FreqTimer;
        FreqTimer = NULL;
    }

    // Save temperature tracking...
    ok = Resume(FILE_SAVE);

    return(ok);
}

/******************************************************************************/

void TEMPTRAK::Update( long DAC[] )
{
int axis;

    if( FreqTimer != NULL )
    {
        FreqTimer->Loop();
    }

    for( axis=0; (axis < Axis); axis++ )
    {
        Update(axis,DAC[axis]);
    }
}

/******************************************************************************/

void TEMPTRAK::Update( int axis, long DAC )
{
    if( !Started() )
    {
        return;
    }

    // Calculation for this time step...
    dTdt(axis,DAC);
}

/******************************************************************************/

void TEMPTRAK::dTdt( int axis, long DAC )
{
    dTdt(axis,DAC,dt);
}

/******************************************************************************/

void TEMPTRAK::dTdt( int axis, long DAC, double step )
{
double I;
double Q_I2Cu;
double dT_Cu2Fe;
double dT_Fe2Air;
double Q_Cu2Fe;
double Q_Fe2Air;

//  Heat flow model for motor...
//              (T_Cu)    (T_Fe)    (T_Air)
//  I(amps) ---> [Cu] ---> [Fe] ---> [Air]
//        (Q_I2Cu)  (Q_Cu2Fe)  (Q_Fe2Air)

    // Calculate current (I) based on DAC units...
    I = Current(axis,DAC);

    // Heat input (Q = I^2.R)...
    Q_I2Cu = (I * I) * ER_Cu[axis];

    // Temperature differentials...
    dT_Cu2Fe  = T_Cu[axis] - T_Fe[axis];    // Copper core to iron case.
    dT_Fe2Air = T_Fe[axis] - T_Air;         // Iron case to air.

    // Head flow (Q = dT/TR or I = V/R)...
    Q_Cu2Fe  = dT_Cu2Fe  / TR_Cu2Fe[axis];
    Q_Fe2Air = dT_Fe2Air / TR_Fe2Air[axis];

    // Change in heat...
    dT_Cu[axis] = ((Q_I2Cu  - Q_Cu2Fe ) * step) / (MassCu[axis] * SPECIFIC_HEAT_Cu);
    dT_Fe[axis] = ((Q_Cu2Fe - Q_Fe2Air) * step) / (MassFe[axis] * SPECIFIC_HEAT_Fe);

    // Apply changes...
    T_Cu[axis] += dT_Cu[axis];
    T_Fe[axis] += dT_Fe[axis];	
}

/******************************************************************************/

double TEMPTRAK::T_Core( int axis )
{
    return(T_Cu[axis]);
}

/******************************************************************************/

double TEMPTRAK::T_Case( int axis )
{
    return(T_Fe[axis]);
}

/******************************************************************************/

BOOL gFlag = FALSE;

BOOL TEMPTRAK::CoreExceed( double T )
{
int axis;
BOOL flag;

    for( flag=FALSE,axis=0; (axis < Axis); axis++ )
    {
        if( T_Core(axis) >= T )
        {	
			if (!gFlag)
			{
				printf("====T_Cu: %G %G %G  %G %G %G\n", T_Cu[0], T_Cu[1], T_Cu[2],T_Cu[3],T_Cu[4],T_Cu[5]);
				gFlag = TRUE;
			}
            flag = TRUE;
        }
    }

    return(flag);
}

/******************************************************************************/

BOOL TEMPTRAK::OverHeated( void )
{
BOOL flag;

    flag = CoreExceed(T_OverHeat);


    return(flag);
}

/******************************************************************************/

BOOL TEMPTRAK::Cooled( void )
{
BOOL flag;

    flag = !CoreExceed(T_Cooled);

    return(flag);
}

/******************************************************************************/

double TEMPTRAK::Current( int axis, long DAC )
{
double I;

    I = DAC2I[axis] * (double)DAC;

    return(I);
}

/******************************************************************************/

long TEMPTRAK::DAC( int axis, double Current )
{
long D;

    D = (long)(Current / DAC2I[axis]);

    return(D);
}

/******************************************************************************/

