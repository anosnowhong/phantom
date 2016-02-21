/******************************************************************************/
/* MOUSE.                                                                     */
/******************************************************************************/

#include <motor.h>

/******************************************************************************/

BOOL    MOUSE_OpenFlag=FALSE;
BOOL    MOUSE_StartFlag=FALSE;
double  MOUSE_Position[GRAPHICS_2D] = { 0,0 };
double  MOUSE_PositionRaw[GRAPHICS_2D] = { 0,0 };
double  MOUSE_PositionRawLast[GRAPHICS_2D] = { 0,0 };
double  MOUSE_PositionRawStep[GRAPHICS_2D] = { 0,0 };
double  MOUSE_PositionRawStepMin[GRAPHICS_2D] = { 0,0 };
int     MOUSE_DisplayPixels[GRAPHICS_2D];
double  MOUSE_DisplaySize[GRAPHICS_2D];
double  MOUSE_DisplayScale[GRAPHICS_2D];
double  MOUSE_DisplayCentre[GRAPHICS_2D];
BOOL    MOUSE_ButtonState[MOUSE_BUTTONS];

TIMER_Frequency MOUSE_UpdatePositionFrequency("MOUSE_UpdatePositionFrequency");

/******************************************************************************/

BOOL MOUSE_Open( char *config )
{
BOOL ok=TRUE;
int i;

    for( i=0; (i < MOUSE_BUTTONS); i++ )
    {
        MOUSE_ButtonState[i] = FALSE;
    }

    if( MOUSE_OpenFlag )
    {
        return(TRUE);
    }

    MOUSE_OpenFlag = TRUE;

    return(ok);
}

/******************************************************************************/

BOOL MOUSE_Open( void )
{
BOOL ok;

    ok = MOUSE_Open(NULL);

    return(ok);
}

/******************************************************************************/

void MOUSE_Close( void )
{
int i;
BOOL ok;

    if( !MOUSE_OpenFlag )
    {
        return;
    }

    // Make sure mouse is stopped first...
    MOUSE_Stop();

    MOUSE_UpdatePositionFrequency.Results();
    ok = MOUSE_UpdatePositionFrequency.iData()->Save();

    for( i=0; (i < GRAPHICS_2D); i++ )
    {
        printf("MOUSE_PositionRawStepMin[%d]=%.2lf\n",i,MOUSE_PositionRawStepMin[i]);
    }

    MOUSE_OpenFlag = FALSE;
}

/******************************************************************************/

BOOL MOUSE_Start( void )
{
BOOL ok=TRUE;

    if( !MOUSE_OpenFlag )
    {
        if( !MOUSE_Open() )
        {
            return(FALSE);
        }
    }

    if( MOUSE_StartFlag )
    {
        return(TRUE);
    }

    // Make sure GRAPHICS system is running and get dimensions of screen...
    ok = GRAPHICS_Screen(MOUSE_DisplayPixels,MOUSE_DisplaySize);

    if( !ok )
    {
        return(FALSE);
    }

    MOUSE_DisplayScale[GRAPHICS_X] = MOUSE_DisplaySize[GRAPHICS_X] / (double)MOUSE_DisplayPixels[GRAPHICS_X];
    MOUSE_DisplayScale[GRAPHICS_Y] = MOUSE_DisplaySize[GRAPHICS_Y] / (double)MOUSE_DisplayPixels[GRAPHICS_Y];

    MOUSE_DisplayCentre[GRAPHICS_X] = MOUSE_DisplaySize[GRAPHICS_X] / 2.0;
    MOUSE_DisplayCentre[GRAPHICS_Y] = MOUSE_DisplaySize[GRAPHICS_Y] / 2.0;

    // Install mouse motion function...
    glutPassiveMotionFunc(MOUSE_UpdatePosition);
    glutMotionFunc(MOUSE_UpdatePosition);
    glutMouseFunc(MOUSE_UpdateButton);

    MOUSE_StartFlag = TRUE;

    return(TRUE);
}

/******************************************************************************/

void MOUSE_Stop( void )
{
    if( !MOUSE_OpenFlag )
    {
        return;
    }

    if( !MOUSE_StartFlag )
    {
        return;
    }

    MOUSE_StartFlag = FALSE;
}

/******************************************************************************/

BOOL MOUSE_Started( void )
{
BOOL flag=FALSE;

    if( MOUSE_OpenFlag )
    {
        flag = MOUSE_StartFlag;
    }

    return(flag);
}

/******************************************************************************/

BOOL MOUSE_GetPosn( matrix &pomx )
{
BOOL ok=TRUE;

    pomx.dim(3,1);

    if( MOUSE_Started() )
    {
        pomx(1,1) = MOUSE_Position[GRAPHICS_X];
        pomx(2,1) = MOUSE_Position[GRAPHICS_Y];
        pomx(3,1) = 0.0;
    }

    return(ok);
}

/******************************************************************************/

void MOUSE_UpdatePosition( int x, int y )
{
static BOOL first=TRUE;
int i;

    MOUSE_PositionRaw[GRAPHICS_X] = (double)x;
    MOUSE_PositionRaw[GRAPHICS_Y] = (double)y;

    MOUSE_Position[GRAPHICS_X] = ((MOUSE_DisplayScale[GRAPHICS_X] * (double)x) - MOUSE_DisplayCentre[GRAPHICS_X]);
    MOUSE_Position[GRAPHICS_Y] = ((MOUSE_DisplayScale[GRAPHICS_Y] * (double)(MOUSE_DisplayPixels[GRAPHICS_Y] - y)) - MOUSE_DisplayCentre[GRAPHICS_Y]);

    if( first )
    {
        first = FALSE;

	MOUSE_UpdatePositionFrequency.Reset();

        for( i=0; (i < GRAPHICS_2D); i++ )
        {
            MOUSE_PositionRawLast[i] = MOUSE_PositionRaw[i];
            MOUSE_PositionRawStepMin[i] = 0.0;
        }
    }
    else
    {
        MOUSE_UpdatePositionFrequency.Loop();
        
        for( i=0; (i < GRAPHICS_2D); i++ )
        {
            MOUSE_PositionRawStep[i] = fabs(MOUSE_PositionRaw[i] - MOUSE_PositionRawLast[i]);
            if( MOUSE_PositionRawStep[i] != 0.0 )
            {
                if( (MOUSE_PositionRawStepMin[i] > MOUSE_PositionRawStep[i]) || (MOUSE_PositionRawStepMin[i] == 0.0) )
                {
                    MOUSE_PositionRawStepMin[i] = MOUSE_PositionRawStep[i];
                }
            }
        }
    }
}

/******************************************************************************/

void MOUSE_UpdateButton( int button, int state, int x, int y )
{
    switch( state )
    {
        case GLUT_UP :
           MOUSE_ButtonState[button] = FALSE;
           break;

        case GLUT_DOWN :
           MOUSE_ButtonState[button] = TRUE;
           break;
    }

    // Only process release of button...
    if( state == GLUT_UP )
    {
        MOUSE_ButtonEvent(button);
    }

    MOUSE_UpdatePosition(x,y);
}

/******************************************************************************/

BOOL MOUSE_ButtonDepressed( int button )
{
BOOL flag;

    flag = MOUSE_ButtonState[button];

    return(flag);
}

/******************************************************************************/

BOOL MOUSE_ButtonDepressed( void )
{
int i=0;
BOOL flag;

    for( flag=FALSE,i=0; (i < MOUSE_BUTTONS); i++ )
    {
        if( MOUSE_ButtonDepressed(i) )
        {
            flag = TRUE;
        }
    }

    return(flag);
}

/******************************************************************************/

int MOUSE_Button[MOUSE_EVENTS];
int MOUSE_ButtonHead=0;
int MOUSE_ButtonTail=0;

/******************************************************************************/

void MOUSE_ButtonIncrement( int &index )
{
    if( ++index >= MOUSE_EVENTS )
    {
        index = 0;
    }
}

/******************************************************************************/

void MOUSE_ButtonEvent( int button )
{
    MOUSE_Button[MOUSE_ButtonHead] = button;
    MOUSE_ButtonIncrement(MOUSE_ButtonHead);
}


/******************************************************************************/

BOOL MOUSE_ButtonGet( int &button )
{
    if( MOUSE_ButtonHead == MOUSE_ButtonTail )
    {
        return(FALSE);
    }

    button = MOUSE_Button[MOUSE_ButtonTail];

    MOUSE_ButtonIncrement(MOUSE_ButtonTail);

    return(TRUE);
}

/******************************************************************************/

void MOUSE_ButtonClear( void )
{
    MOUSE_ButtonHead = 0;
    MOUSE_ButtonTail = 0;
}

/******************************************************************************/

