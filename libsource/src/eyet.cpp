/******************************************************************************/
/*                                                                            */
/* MODULE  : EYET.cpp                                                         */
/*                                                                            */
/* PURPOSE : EyeLink-1000 high-level API.                                     */
/*                                                                            */
/* DATE    : 18/Jun/2011                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 18/Jun/2011 - Initial development.                               */
/*                                                                            */
/******************************************************************************/

#include <motor.h>
#include <eyet.h>
#include <EyeLink1000.h>

/******************************************************************************/

#define   EYET_PORT_DIRECT   0
int       EYET_PortNumber=EYET_PORT_DIRECT;
double    EYET_SampleRate=1000.0; // Hz
double    EYET_SamplePeriod;      // sec
double    EYET_XRange[2] = { -20.0,20.0 };
double    EYET_YRange[2] = { -15.0,15.0 };
STRING    EYET_CalibrateFile="EYELINK.CAL";
matrix    EYET_CalibrateMatrix(2,3);
int       EYET_Targets[2] = { 3,3 };
BOOL      EYET_TargetCentre = FALSE;
double    EYET_TargetRadius = 0.2;
STRING    EYET_TargetColorText="YELLOW";
int       EYET_TargetColor=YELLOW;
int       EYET_TargetNoDataColor=GREY;
STRING    EYET_TargetNoDataColorText="GREY";
double    EYET_CursorRadius = 0.1;
STRING    EYET_CursorColorText="RED";
int       EYET_CursorColor=RED;
double    EYET_CalibStartDelay=1.0;
double    EYET_TargetCalibDelay=0.5;
double    EYET_FixateTime=0.5;
double    EYET_FixateSD=0.5;
int       EYET_CalibrateWindowPoints=10;
double    EYET_InterTargetDelay=0.0;
double    EYET_TargetTestDelay=2.0;

BOOL      EYET_OpenFlag=FALSE;
void    (*EYET_FuncGraphicsText)( char *text )=NULL;
void    (*EYET_FuncBeepGo)( void )=NULL;
BOOL      EYET_KeyPressFlag=FALSE;
unsigned  char  EYET_KeyPressCode;
int       EYET_TargetCount;
matrix    EYET_TargetList;
matrix    EYET_TargetEyeXY;
int       EYET_TargetIndex;
int       EYET_TargetDone;
BOOL      EYET_TestFlag=FALSE;
BOOL      EYET_NoDataFlag=TRUE;
TIMER     EYET_FixateTimer("EYET_Fixtate");
DATAPROC *EYET_FixateWindow[2] = { NULL,NULL };
DATAPROC *EYET_CalibrateWindow[2] = { NULL,NULL };
int       EYET_FixateWindowPoints;
double    EYET_EyeXY[2];
double    EYET_CalibrateWindowEyeXY_Mean[2];
double    EYET_CalibrateWindowEyeXY_SD[2];
TIMER     EYET_FrameLastTimer("EYET_FrameLastTimer");

PERMUTELIST EYET_TargetPermute;
TIMER_Interval EYET_FrameNextLatency("EYET_FrameNextLatency");

/******************************************************************************/

#define EYET_STATE_INIT               0
#define EYET_STATE_IDLE               1
#define EYET_STATE_CALIB_INIT         2
#define EYET_STATE_CALIB_DELAY        3
#define EYET_STATE_CALIB_GO           4
#define EYET_STATE_CALIB_TARGET       5
#define EYET_STATE_CALIB_FIXATE       6
#define EYET_STATE_CALIB_NEXT         7
#define EYET_STATE_CALIB_INTERTARGET  8
#define EYET_STATE_CALIB_DONE         9
#define EYET_STATE_TEST_INIT         10
#define EYET_STATE_TEST_TARGET       11
#define EYET_STATE_TEST_NEXT         12
#define EYET_STATE_TEST_INTERTARGET  13
#define EYET_STATE_TEST_DONE         14

char *EYET_StateText[] = { "Init","Idle","CalibInit","CalibDelay","CalibGo","CalibTarget","CalibFixate","CalibNext","CalibInterTarget","CalibDone","TestInit","TestTarget","TestNext","TestInterTarget","TestDone" };
BOOL  EYET_StateDisplayTarget[] = { FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,TRUE,FALSE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE };
BOOL  EYET_StateDisplayCursor[] = { FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,TRUE,FALSE };
BOOL  EYET_StateCalibrate[] = { FALSE,FALSE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE };

TIMER EYET_StateTimer("EYET-State");
int   EYET_State=EYET_STATE_INIT;

/******************************************************************************/

void EYET_ConfigSetup( void )
{
    // Reset configuration variable list...
    CONFIG_reset();

    // Set up variable list for configuration ...
    CONFIG_set("PortNumber",EYET_PortNumber);
    CONFIG_set("SampleRate",EYET_SampleRate);
    CONFIG_set("XRange",EYET_XRange,2);
    CONFIG_set("YRange",EYET_YRange,2);
    CONFIG_set("Targets",EYET_Targets,2);
    CONFIG_setBOOL("TargetCentre",EYET_TargetCentre);
    CONFIG_set("TargetRadius",EYET_TargetRadius);
    CONFIG_set("TargetColor",EYET_TargetColorText);
    CONFIG_set("TargetNoDataColor",EYET_TargetNoDataColorText);
    CONFIG_set("CursorRadius",EYET_CursorRadius);
    CONFIG_set("CursorColor",EYET_CursorColorText);
    CONFIG_set("CalibStartDelay",EYET_CalibStartDelay);
    CONFIG_set("TargetCalibDelay",EYET_TargetCalibDelay);
    CONFIG_set("TargetTestDelay",EYET_TargetTestDelay);
    CONFIG_set("InterTargetDelay",EYET_InterTargetDelay);
    CONFIG_set("FixateTime",EYET_FixateTime);
    CONFIG_set("FixateSD",EYET_FixateSD);
    CONFIG_set("CalibratePoints",EYET_CalibrateWindowPoints);
    CONFIG_set("CalibrateFile",EYET_CalibrateFile);
}

/******************************************************************************/

BOOL EYET_ConfigLoad( char *file )
{
int i,j,k;
double xstep=0.0,ystep=0.0;
BOOL ok=TRUE;

    EYET_ConfigSetup();
 
    // Load configuration file...
    if( !CONFIG_read(file) )
    {
        EYET_errorf("EYET_ConfigLoad(%s) Cannot read file.\n",file);
        return(FALSE);
    }

    if( !GRAPHICS_ColorCode(EYET_TargetColor,EYET_TargetColorText) )
    {
        EYET_errorf("EYET_ConfigLoad(%s) Invalid color (%s).\n",file,EYET_TargetColorText);
        ok = FALSE;
    }

    if( !GRAPHICS_ColorCode(EYET_TargetNoDataColor,EYET_TargetNoDataColorText) )
    {
        EYET_errorf("EYET_ConfigLoad(%s) Invalid color (%s).\n",file,EYET_TargetNoDataColorText);
        ok = FALSE;
    }

    if( !GRAPHICS_ColorCode(EYET_CursorColor,EYET_CursorColorText) )
    {
        EYET_errorf("EYET_ConfigLoad(%s) Invalid color (%s).\n",file,EYET_CursorColorText);
        ok = FALSE;
    }

    EYET_SamplePeriod = 1.0 / EYET_SampleRate;
    EYET_FixateWindowPoints = (int)(EYET_FixateTime / EYET_SamplePeriod);

    EYET_TargetCount = EYET_Targets[0]*EYET_Targets[0];

    if( EYET_TargetCentre )
    {
        EYET_TargetCount++;
    }

    EYET_TargetPermute.Init(1,EYET_TargetCount,TRUE);

    EYET_TargetList.dim(EYET_TargetCount,2);
    EYET_TargetEyeXY.dim(EYET_TargetCount,2);

    if( EYET_Targets[0] > 1 )
    {
        xstep = (EYET_XRange[1]-EYET_XRange[0])/(double)(EYET_Targets[0]-1);
    }

    if( EYET_Targets[1] > 1 )
    {
        ystep = (EYET_YRange[1]-EYET_YRange[0])/(double)(EYET_Targets[1]-1);
    }

    for( k=0,i=0; (i < EYET_Targets[0]); i++ )
    {
        for( j=0; (j < EYET_Targets[1]); j++ )
        {
            k++;
            EYET_TargetList(k,1) = EYET_XRange[0] + (xstep * (double)i);
            EYET_TargetList(k,2) = EYET_YRange[0] + (ystep * (double)j);
        }
    }

    if( EYET_TargetCentre )
    {
        k++;
        EYET_TargetList(k,1) = (EYET_XRange[0] + EYET_XRange[1]) / 2.0;
        EYET_TargetList(k,2) = (EYET_YRange[0] + EYET_YRange[1]) / 2.0;
    }

    EYET_messgf("EYET_ConfigLoad(%s)\n",file);
    CONFIG_list(EYET_messgf);
    disp(EYET_TargetList);

    return(ok);
}

/******************************************************************************/

BOOL EYET_Open( char *cnfg, void (*grphtext)( char *text ), void (*beepgo)( void ) )
{
BOOL ok=FALSE;
int i;

    if( EYET_OpenFlag )
    {
        return(TRUE);
    }

    if( !EYET_ConfigLoad(cnfg) )
    {
        return(FALSE);
    }

    if( !matrix_read(EYET_CalibrateFile,EYET_CalibrateMatrix) )
    {
        EYET_errorf("EYET_Open(%s) Cannot read calibration file: %s\n",cnfg,EYET_CalibrateFile);
        return(FALSE);
    }

    EYET_messgf("matrix_read(%s)\n",EYET_CalibrateFile);
    disp(EYET_CalibrateMatrix);

    for( i=0; (i < 2); i++ )
    {
        EYET_FixateWindow[i] = new DATAPROC(STR_stringf("EYET_FixateWindow[%d]",i),EYET_FixateWindowPoints,DATAPROC_FLAG_CIRCULAR);
        EYET_CalibrateWindow[i] = new DATAPROC(STR_stringf("EYET_CalibrateWindow[%d]",i),EYET_CalibrateWindowPoints,DATAPROC_FLAG_CIRCULAR);
    }

    if( EYET_PortNumber == EYET_PORT_DIRECT )
    {
        if( EYELINK_Open() )
        {
            ok = EYELINK_Start();
        }
    }
    else
    {
        ok = EYECOM_Open(EYET_PortNumber);
    }

    if( ok )
    {
        EYET_OpenFlag = TRUE;
        EYET_FuncGraphicsText = grphtext;
        EYET_FuncBeepGo = beepgo;
    }

    return(ok);
}

/******************************************************************************/

BOOL EYET_Open( char *cnfg, void (*grphtext)( char *text ) )
{
BOOL ok;

    ok = EYET_Open(cnfg,grphtext,NULL);

    return(ok);
}

/******************************************************************************/

void EYET_Close( void )
{
int i;

    if( EYET_OpenFlag )
    {
        if( EYET_PortNumber == EYET_PORT_DIRECT )
        {
            EYELINK_Stop();
            EYELINK_Close();
        }
        else
        {
            EYECOM_Close();
        }

        EYET_OpenFlag = FALSE;

        EYET_FrameNextLatency.Results();
    }

    for( i=0; (i < 2); i++ )
    {
        if( EYET_FixateWindow[i] != NULL )
        {
            delete EYET_FixateWindow[i];
            EYET_FixateWindow[i] = NULL;
        }

        if( EYET_CalibrateWindow[i] != NULL )
        {
            delete EYET_CalibrateWindow[i];
            EYET_CalibrateWindow[i] = NULL;
        }
    }
}

/******************************************************************************/

double EYET_GetSampleRate( void )
{
double rate=0.0;

    if( EYET_OpenFlag )
    {
        rate = EYET_SampleRate;
    }

    return(rate);
}

/******************************************************************************/


void EYET_GraphicsText( char *text )
{
    if( EYET_FuncGraphicsText != NULL )
    {
        (*EYET_FuncGraphicsText)(text);
    }
}

/******************************************************************************/

void EYET_BeepGo( void )
{
    if( EYET_FuncBeepGo != NULL )
    {
        (*EYET_FuncBeepGo)();
    }
}

/******************************************************************************/

void EYET_KeyEvent( unsigned char key )
{
    EYET_KeyPressFlag = TRUE;
    EYET_KeyPressCode = key;
}

/******************************************************************************/

void EYET_KeyReset( void )
{
    EYET_KeyPressFlag = FALSE;
}

/******************************************************************************/

BOOL EYET_KeyPress( unsigned char &key )
{
BOOL flag=FALSE;

    if( EYET_KeyPressFlag )
    {
        flag = TRUE;
        key = EYET_KeyPressCode;
        EYET_KeyPressFlag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL EYET_KeyPress( void )
{
unsigned char key;
BOOL flag;

    flag = EYET_KeyPress(key);

    return(flag);
}

/******************************************************************************/

BOOL EYET_CalibrateStart( BOOL testflag )
{
BOOL ok=FALSE;

    if( EYET_StateIdle() )
    {
        EYET_KeyReset();
        EYET_StateNext(EYET_STATE_CALIB_INIT);
        EYET_TestFlag = testflag;
        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

void EYET_CalibrateDone( void )
{
int i,j,n;
matrix X,Y,XT,M[2];

    EYET_messgf("EYET_TargetList\n");
    disp(EYET_TargetList);

    EYET_messgf("EYET_TargetEyeXY\n");
    disp(EYET_TargetEyeXY);

    n = EYET_TargetCount;
    Y.dim(n,1);
    X.dim(n,3);

    for( i=1; (i <= 2); i++ )
    {
        for( j=1; (j <= n); j++ )
        {
            Y(j,1) = EYET_TargetList(j,i);

            X(j,1) = EYET_TargetEyeXY(j,1);
            X(j,2) = EYET_TargetEyeXY(j,2);
            X(j,3) = 1.0;
        }

        matrix_transpose(XT,X);
        M[i-1] = inv(XT * X) * XT * Y;
    }

    EYET_CalibrateMatrix = M[0] | M[1];

    EYET_messgf("EYET_CalibrateMatrix\n");
    disp(EYET_CalibrateMatrix);

    if( !matrix_write(EYET_CalibrateFile,EYET_CalibrateMatrix) )
    {
        EYET_errorf("EYET_CalibrateDone() Cannot write calibration file: %s\n",EYET_CalibrateFile);
    }
}

/******************************************************************************/

BOOL EYET_TestStart( void )
{
BOOL ok=FALSE;

    if( EYET_StateIdle() )
    {
        EYET_KeyReset();
        EYET_StateNext(EYET_STATE_TEST_INIT);
        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

void EYET_Abort( void )
{
    EYET_StateNext(EYET_STATE_IDLE);
}

/******************************************************************************/

BOOL EYET_StateIdle( void )
{
BOOL flag;

    flag = (EYET_State == EYET_STATE_IDLE);

    return(flag);
}

/******************************************************************************/

void EYET_StateNext( int state )
{
    EYET_messgf("EYET_StateNext: %s[%d] > %s[%d] (%.0lf msec).\n",EYET_StateText[EYET_State],EYET_State,EYET_StateText[state],state,EYET_StateTimer.Elapsed());
    EYET_StateTimer.Reset();
    EYET_State = state;
}

/******************************************************************************/

void EYET_StateProcess( void )
{
unsigned char key;

    switch( EYET_State )
    {
        case EYET_STATE_INIT :
            EYET_StateNext(EYET_STATE_IDLE);
            break;

        case EYET_STATE_IDLE :
            break;

        case EYET_STATE_CALIB_INIT :
            EYET_GraphicsText("Eye tracker calibrate");

            if( EYET_KeyPress(key) )
            {
                (*EYET_GraphicsText)("");

                if( key != SPACE )
                {
                    EYET_StateNext(EYET_STATE_IDLE);
                    break;
                }

                EYET_TargetPermute.Reset();
                EYET_TargetDone = 0;
                EYET_StateNext(EYET_STATE_CALIB_DELAY);
                break;
            }
            break;

        case EYET_STATE_CALIB_DELAY :
            if( EYET_StateTimer.ExpiredSeconds(EYET_CalibStartDelay) )
            {
                EYET_StateNext(EYET_STATE_CALIB_NEXT);
            }
            break;

        case EYET_STATE_CALIB_GO :
            EYET_BeepGo();
            EYET_StateNext(EYET_STATE_CALIB_TARGET);
            break;

        case EYET_STATE_CALIB_TARGET :
            if( EYET_StateTimer.ExpiredSeconds(EYET_TargetCalibDelay) )
            {
                EYET_FixateTimer.Reset();
                EYET_StateNext(EYET_STATE_CALIB_FIXATE);
                break;
            }
            break;

        case EYET_STATE_CALIB_FIXATE :
            if( EYET_FixateTimer.ExpiredSeconds(EYET_FixateTime) )
            {
                EYET_TargetEyeXY(EYET_TargetIndex,1) = EYET_CalibrateWindowEyeXY_Mean[0];
                EYET_TargetEyeXY(EYET_TargetIndex,2) = EYET_CalibrateWindowEyeXY_Mean[1];

                EYET_debugf("EYET_TargetEyeXY: Target=%d X=%.2lf (SD=%.2lf), Y=%.2lf (SD=%.2lf)\n",EYET_TargetIndex,EYET_CalibrateWindowEyeXY_Mean[0],EYET_CalibrateWindowEyeXY_SD[0],EYET_CalibrateWindowEyeXY_Mean[1],EYET_CalibrateWindowEyeXY_SD[1]);

                EYET_StateNext(EYET_STATE_CALIB_NEXT);
                break;
            }

            if( TIMER_EveryHz(8.0) )
            {
                EYET_debugf("EYET_FixateSD: X=%.2lf Y=%.2lf\n",EYET_FixateWindow[0]->SD(),EYET_FixateWindow[1]->SD());
            }
            break;

        case EYET_STATE_CALIB_NEXT :
            if( ++EYET_TargetDone > EYET_TargetCount )
            {
                EYET_StateNext(EYET_STATE_CALIB_DONE);
                break;
            }

            EYET_TargetIndex = EYET_TargetPermute.GetNext();
            EYET_StateNext(EYET_STATE_CALIB_INTERTARGET);
            break;

        case EYET_STATE_CALIB_INTERTARGET :
            if( EYET_StateTimer.ExpiredSeconds(EYET_InterTargetDelay) )
            {
                EYET_StateNext(EYET_STATE_CALIB_GO);
                break;
            }
            break;

        case EYET_STATE_CALIB_DONE :
            EYET_CalibrateDone();

            if( EYET_TestFlag )
            {
                EYET_StateNext(EYET_STATE_TEST_INIT);
                break;
            }

            EYET_StateNext(EYET_STATE_IDLE);
            break;

        case EYET_STATE_TEST_INIT :
            (*EYET_GraphicsText)("Eye tracker test");

            if( EYET_KeyPress() )
            {
                (*EYET_GraphicsText)("");
                EYET_TargetPermute.Reset();
                EYET_StateNext(EYET_STATE_TEST_NEXT);
                break;
            }
            break;

        case EYET_STATE_TEST_TARGET :
            if( EYET_StateTimer.ExpiredSeconds(EYET_TargetTestDelay) )
            {
                EYET_StateNext(EYET_STATE_TEST_NEXT);
                break;
            }
            break;

        case EYET_STATE_TEST_NEXT :
            if( EYET_KeyPress() )
            {
                EYET_StateNext(EYET_STATE_TEST_DONE);
                break;
            }

            EYET_TargetIndex = EYET_TargetPermute.GetNext();
            EYET_StateNext(EYET_STATE_TEST_INTERTARGET);
            break;

        case EYET_STATE_TEST_INTERTARGET :
            if( EYET_StateTimer.ExpiredSeconds(0.0) )
            {
                EYET_StateNext(EYET_STATE_TEST_TARGET);
                break;
            }
            break;

        case EYET_STATE_TEST_DONE :
            EYET_StateNext(EYET_STATE_IDLE);
            break;

    }
}

/******************************************************************************/

void EYET_LoopTask( void )
{
    if( !EYET_OpenFlag )
    {
        return;
    }

    if( EYET_PortNumber != EYET_PORT_DIRECT )
    {
        EYECOM_LoopTask();
    }
}

/******************************************************************************/

BOOL EYET_NoData( double EyeXY[] )
{
BOOL flag;

    flag = ((EyeXY[0] < -30000) || (EyeXY[1] < -30000));

    return(flag);
}

/******************************************************************************/

BOOL EYET_FrameNext( double &TimeStamp, double EyeXY[], double &PupilSize, BOOL &NoDataFlag, double &FixateSeconds, BOOL &ReadyFlag )
{
static matrix P1(3,1),P2(2,1);
BOOL ok,ready;
int i;

    ReadyFlag = FALSE;
    NoDataFlag = FALSE;
    FixateSeconds = 0.0;

    if( !EYET_OpenFlag )
    {
        return(TRUE);
    }

    // LoopTask processing, if required.
    EYET_LoopTask();

    EYET_FrameNextLatency.Before();

    // Get next frame in a loop so we're sure to have most recent.
    do
    {
        if( EYET_PortNumber == EYET_PORT_DIRECT )
        {
            ok = EYELINK_FrameNext(TimeStamp,EyeXY,PupilSize,ready);
        }
        else
        {
            ok = EYECOM_FrameNext(TimeStamp,EyeXY,ready);
        }

        if( ok && ready )
        {
            ReadyFlag = TRUE;
        }
    }
    while( ok && ready );

    EYET_FrameNextLatency.After();

    if( ReadyFlag )
    {
        EYET_FrameLastTimer.Reset();

        // Check for "no data" from eye tracker.
        NoDataFlag = EYET_NoData(EyeXY);

        // Apply calibration if data is available and we're not in the calibrate state.
        if(  !EYET_StateCalibrate[EYET_State] )
        {
            P1(1,1) = EyeXY[0];
            P1(2,1) = EyeXY[1];
            P1(3,1) = 1.0;

            P2 = T(EYET_CalibrateMatrix) * P1;

            EyeXY[0] = P2(1,1);
            EyeXY[1] = P2(2,1);
        }

        for( i=0; (i < 2); i++ )
        {
            EYET_EyeXY[i] = EyeXY[i];
            EYET_FixateWindow[i]->Data(EyeXY[i]);
            EYET_CalibrateWindow[i]->Data(EyeXY[i]);
            EYET_CalibrateWindowEyeXY_Mean[i] = EYET_CalibrateWindow[i]->Mean();
            EYET_CalibrateWindowEyeXY_SD[i] = EYET_CalibrateWindow[i]->SD();

            if( NoDataFlag || (EYET_FixateWindow[i]->SD() >= EYET_FixateSD) )
            {
                EYET_FixateTimer.Reset();
            }
        }

        FixateSeconds = EYET_FixateTimer.ElapsedSeconds();
        EYET_NoDataFlag = NoDataFlag;
    }
    else
    if( EYET_FrameLastTimer.ExpiredSeconds(10.0 * EYET_SamplePeriod) )
    {
        ok = FALSE;
        NoDataFlag = TRUE;
        EYET_NoDataFlag = TRUE;
    }

    return(ok);
}

/******************************************************************************/

BOOL EYET_FrameNext( double &TimeStamp, double EyeXY[], BOOL &NoDataFlag, double &FixateSeconds, BOOL &ReadyFlag )
{
BOOL ok;
double PupilSize;

    ok = EYET_FrameNext(TimeStamp,EyeXY,PupilSize,NoDataFlag,FixateSeconds,ReadyFlag);

    return(ok);
}

/******************************************************************************/

BOOL EYET_FrameNext( double &TimeStamp, double EyeXY[], double &PupilSize, BOOL &ReadyFlag )
{
BOOL ok,NoDataFlag;
double FixateSeconds;

    ok = EYET_FrameNext(TimeStamp,EyeXY,PupilSize,NoDataFlag,FixateSeconds,ReadyFlag);

    return(ok);
}

/******************************************************************************/

BOOL EYET_FrameNext( double &TimeStamp, double EyeXY[], BOOL &ReadyFlag )
{
BOOL ok;
double PupilSize;

    ok = EYET_FrameNext(TimeStamp,EyeXY,PupilSize,ReadyFlag);

    return(ok);
}

/******************************************************************************/

void EYET_GraphicsDisplay( void )
{
matrix posn(3,1);
int i;

    if( EYET_State == EYET_STATE_CALIB_INIT )
    {
        for( i=1; (i <= EYET_TargetCount); i++ )
        {
            posn(1,1) = EYET_TargetList(i,1);
            posn(2,1) = EYET_TargetList(i,2);
            posn(3,1) = -EYET_TargetRadius;
            GRAPHICS_Circle(&posn,EYET_TargetRadius,EYET_NoDataFlag ? EYET_TargetNoDataColor : EYET_TargetColor);
        }
    }

    if( EYET_StateDisplayTarget[EYET_State] )
    {
        posn(1,1) = EYET_TargetList(EYET_TargetIndex,1);
        posn(2,1) = EYET_TargetList(EYET_TargetIndex,2);
        posn(3,1) = -EYET_TargetRadius;
        GRAPHICS_Circle(&posn,EYET_TargetRadius,EYET_NoDataFlag ? EYET_TargetNoDataColor : EYET_TargetColor);
    }

    if( EYET_StateDisplayCursor[EYET_State] )
    {
        posn(1,1) = EYET_EyeXY[0];
        posn(2,1) = EYET_EyeXY[1];
        posn(3,1) = 0.0;
        GRAPHICS_Circle(&posn,EYET_CursorRadius,EYET_CursorColor);
    }
}

/******************************************************************************/
/* EYECOM - EyeLink1000 RS232 streaming functions.                           */
/******************************************************************************/

#define EYECOM_STATE_INIT   0
#define EYECOM_STATE_SOH    1
#define EYECOM_STATE_STX    2
#define EYECOM_STATE_DATA   3
#define EYECOM_STATE_ETX    4
#define EYECOM_STATE_EOH    5

#define EYECOM_FRAME_BUFF   256
#define EYECOM_FRAME_DATA   3

int   EYECOM_PortHandle=COM_INVALID;
int   EYECOM_PortNumber=1;
int   EYECOM_PortBaud=115200;
BOOL  EYECOM_ErrorFlag=FALSE;
int   EYECOM_FrameSize=2+sizeof(float)+sizeof(float)+2;
int   EYECOM_FrameWaiting=1;
int   EYECOM_State=EYECOM_STATE_INIT;
int   EYECOM_Field=0;
int   EYECOM_Byte=0;
BYTE  EYECOM_Buffer[EYECOM_FRAME_BUFF];
BOOL  EYECOM_FrameReady=FALSE;
int   EYECOM_SyncErrors=0;
int   EYECOM_FrameOverRunErrors=0;
int   EYECOM_FrameCount=0;
float EYECOM_BufferData[EYECOM_FRAME_DATA] = { 0.0,0.0,0.0 };
float EYECOM_FrameData[EYECOM_FRAME_DATA] = { 0.0,0.0,0.0 };

TIMER_Frequency EYECOM_LoopTaskFrequency("EYECOM_LoopTaskFrequency");
TIMER_Frequency EYECOM_FrameFrequency("EYECOM_FrameFrequency");
TIMER_Interval  EYECOM_LoopTaskLatency("EYECOM_LoopTaskLatency");
TIMER_Interval  EYECOM_ReadLatency("EYECOM_ReadLatency");

/******************************************************************************/

void EYECOM_ProcessByte( BOOL dataflag, BYTE databyte )
{
int i;

    switch( EYECOM_State )
    {
        case EYECOM_STATE_INIT :
           EYECOM_LoopTaskFrequency.Reset();
           EYECOM_FrameFrequency.Reset();
           EYECOM_ErrorFlag = FALSE;
           EYECOM_FrameReady = FALSE;
           EYECOM_FrameCount = 0;
           EYECOM_SyncErrors = 0;
           EYECOM_FrameOverRunErrors = 0;
           EYECOM_FrameWaiting = 1;
           EYECOM_State++;
           break;

        case EYECOM_STATE_SOH :
           if( !dataflag ) break;
           
           if( databyte != SOH )
           {
               EYECOM_SyncErrors++;
               EYECOM_FrameWaiting = 1;
               EYECOM_State = EYECOM_STATE_SOH;
               break;
           }

           EYECOM_State++;
           EYECOM_Field = 0;
           EYECOM_Byte = 0;
           break;

        case EYECOM_STATE_STX :
           if( !dataflag ) break;

           if( databyte != STX )
           {
               EYECOM_SyncErrors++;
               EYECOM_FrameWaiting = 1;
               EYECOM_State = EYECOM_STATE_SOH;
               break;
           }

           EYECOM_State++;
           EYECOM_FrameWaiting = EYECOM_FrameSize - 2;
           break;

        case EYECOM_STATE_DATA :
           if( !dataflag ) break;

           EYECOM_Buffer[EYECOM_Byte++] = databyte;
           EYECOM_FrameWaiting--;

           if( EYECOM_Byte == sizeof(float) )
           {
               EYECOM_BufferData[EYECOM_Field++] = *((float *)EYECOM_Buffer);
               EYECOM_Byte = 0;

               if( EYECOM_Field == EYECOM_FRAME_DATA )
               {
                   EYECOM_State++;
                   break;
               }
           }
           break;

        case EYECOM_STATE_ETX :
           if( !dataflag ) break;

           if( databyte != ETX )
           {
               EYECOM_SyncErrors++;
               EYECOM_FrameWaiting = 1;
               EYECOM_State = EYECOM_STATE_SOH;
               break;
           }

           EYECOM_FrameWaiting--;
           EYECOM_State++;
           break;

        case EYECOM_STATE_EOH :
           if( !dataflag ) break;

           if( databyte != EOH )
           {
               EYECOM_SyncErrors++;
               EYECOM_FrameWaiting = 1;
               EYECOM_State = EYECOM_STATE_SOH;
               break;
           }

           EYECOM_FrameWaiting = 1;
           EYECOM_State = EYECOM_STATE_SOH;

           if( EYECOM_FrameReady )
           {
               EYECOM_FrameOverRunErrors++;
           }

           EYECOM_FrameFrequency.Loop();
           EYECOM_FrameCount++;
           EYECOM_FrameReady = TRUE;
           for( i=0; (i < EYECOM_FRAME_DATA); i++ )
           {
               EYECOM_FrameData[i] = EYECOM_BufferData[i];
           }
           break;
    }
}

/******************************************************************************/

void EYECOM_LoopTask( void )
{
BOOL flag=FALSE;
static BYTE buff[EYECOM_FRAME_BUFF];
double wait_msec=0.01;
int count=0;
int i;

    if( EYECOM_ErrorFlag )
    {
        return;
    }

    EYECOM_LoopTaskFrequency.Loop();

    EYECOM_ProcessByte(FALSE,0);

    EYECOM_LoopTaskLatency.Before();

    EYECOM_ReadLatency.Before();

    if( !COM_Read(EYECOM_PortHandle,buff,EYECOM_FrameWaiting,count,flag,wait_msec) )
    {
        EYECOM_errorf("EYECOM: RS232 read error.\n");
        EYECOM_ErrorFlag = TRUE;
    }

    EYECOM_ReadLatency.After();

    if( flag )
    {
        for( i=0; (i < count); i++ )
        {
            EYECOM_ProcessByte(TRUE,buff[i]);
        }
    }

    EYECOM_LoopTaskLatency.After();
}

/******************************************************************************/

BOOL EYECOM_FrameNext( double &TimeStamp, double EyeXY[], BOOL &ready )
{
int i,j;

    ready = FALSE;

    if( EYECOM_ErrorFlag )
    {
        return(FALSE);
    }

    if( EYECOM_FrameReady )
    {
        ready = TRUE;
        EYECOM_FrameReady = FALSE;

        j = 0;

        if( EYECOM_FRAME_DATA == 3 )
        {
            TimeStamp = EYECOM_FrameData[j++];
        }

        for( i=0; (i < 2); i++ )
        {
            EyeXY[i] = EYECOM_FrameData[j++];
        }
    }

    return(TRUE);
}

/******************************************************************************/

void EYECOM_PortDCB( DCB *dcb )
{
    // Set various parameters...
    dcb->ByteSize = 8;
    dcb->Parity = NOPARITY;
    dcb->StopBits = ONESTOPBIT;
    dcb->fAbortOnError = TRUE;
    dcb->fDtrControl = DTR_CONTROL_DISABLE;
    dcb->fDsrSensitivity = FALSE;
    dcb->fOutxDsrFlow = FALSE;
    dcb->fOutxCtsFlow = FALSE;
    dcb->fRtsControl = RTS_CONTROL_DISABLE;
}

/******************************************************************************/

BOOL EYECOM_Open( int port )
{
BOOL ok=TRUE;

    // Open RS232 port...
    if( (EYECOM_PortHandle=COM_Open(port,EYECOM_PortBaud,EYECOM_PortDCB)) == COM_INVALID )
    {
        EYECOM_errorf("EYECOM_Open: Cannot open RS232 port %d.\n",port);
        ok = FALSE;
    }

    if( ok )
    {
        ok = COM_Reset(EYECOM_PortHandle);
    }

    return(ok);
}

/******************************************************************************/

void EYECOM_Close( void )
{
BOOL ok;

    // Close RS232 port...
    if( EYECOM_PortHandle != COM_INVALID )
    {
        ok = COM_Close(EYECOM_PortHandle);
        EYECOM_PortHandle = COM_INVALID;
    }

    printf("EYECOM: Frames=%d SyncErrors=%d FrameOverRunErrors=%d\n",EYECOM_FrameCount,EYECOM_SyncErrors,EYECOM_FrameOverRunErrors);
    EYECOM_LoopTaskFrequency.Results();
    EYECOM_FrameFrequency.Results();
    EYECOM_LoopTaskLatency.Results();
    EYECOM_ReadLatency.Results();
}

/******************************************************************************/

