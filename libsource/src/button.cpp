/******************************************************************************/

#include <motor.h>

/******************************************************************************/

BYTE BUTTON_LeftRight[] = { 'L','R' };
BYTE BUTTON_NoYes[] = { 'N','Y' };
BYTE BUTTON_QWER[] = { 'Q','W','E','R' };
BYTE BUTTON_UIOP[] = { 'U','I','O','P' };

int  BUTTON_Port=1;
BYTE BUTTON_Mask=0xFF;
BYTE BUTTON_Data;
BYTE BUTTON_Last[BUTTON_MAX];
BYTE BUTTON_Bits[BUTTON_MAX];
BOOL BUTTON_Flag[BUTTON_MAX];
BOOL BUTTON_Opened=FALSE;
BOOL BUTTON_Event=FALSE;

/******************************************************************************/

void BUTTON_Open( int port, BYTE mask )
{
    BUTTON_Port = port;
    BUTTON_Mask = mask;

    BUTTON_Reset();
    BUTTON_Opened = TRUE;
}

/******************************************************************************/

void BUTTON_Open( BYTE mask )
{
    // Open using default port...
    BUTTON_Open(1,mask);
}

/******************************************************************************/

void BUTTON_Open( void )
{
    // Open using default port and mask...
    BUTTON_Open(1,0xFF);
}

/******************************************************************************/

void BUTTON_Reset( void )
{
int b;

    for( b=0; (b < BUTTON_MAX); b++ )
    {
        BUTTON_Last[b] = BUTTON_LAST_RESET;
        BUTTON_Flag[b] = FALSE;
    }
}

/******************************************************************************/

void BUTTON_LoopTask( void )
{
int b;

    if( !BUTTON_Opened )
    {
        return;
    }

    BUTTON_Data = LPT_byte(BUTTON_Port,LPT_PORT_DATA) & BUTTON_Mask;
    BIT_byte(BUTTON_Data,BUTTON_Bits);

    for( b=0; (b < BUTTON_MAX); b++ )
    {
        if( BUTTON_Bits[b] == BUTTON_Last[b] )
        {
            continue;
        }

        if( BUTTON_Last[b] != BUTTON_LAST_RESET )
        {
            if( BUTTON_Bits[b] )
            {
                BUTTON_PutEvent(b);
            }

            BUTTON_Event = TRUE;
        }

        BUTTON_Last[b] = BUTTON_Bits[b];
    }
}

/******************************************************************************/

BOOL BUTTON_Button( int &button )
{
int b;
BOOL flag;

    for( flag=FALSE,b=0; ((b < BUTTON_MAX) && !flag); b++ )
    {
        if( BUTTON_Flag[b] )
        {
            BUTTON_Flag[b] = FALSE;

            flag = TRUE;
            button = b;
        }
    }

    return(flag);
}

/******************************************************************************/

void BUTTON_PutEvent( int button )
{
    BUTTON_Flag[button] = TRUE;
}

/******************************************************************************/

BOOL BUTTON_GetEvent( BYTE bits[] )
{
BOOL flag=FALSE;
int b;

    flag = BUTTON_Event;
    BUTTON_Event = FALSE;

    for( b=0; (b < BUTTON_MAX); b++ )
    {
        bits[b] = BUTTON_Bits[b];
    }

    return(flag);
}

/******************************************************************************/

BOOL BUTTON_Key( BYTE &code, BYTE xlat[] )
{
BOOL flag=FALSE;
int button;

    if( KB_key(code,KB_UPPER) )   // Regular KeyBoard input...
    {
        flag = TRUE;
    }
    else
    if( BUTTON_Button(button) )  // Button-box input via parallel port...
    {
        flag = TRUE;

        if( xlat != NULL )
        {
            code = xlat[button];
        }
        else
        {
            code = button;
        }
    }
    
    return(flag);
}

/******************************************************************************/

BOOL BUTTON_Key( BYTE &code )
{
BOOL ok;

    ok = BUTTON_Key(code,NULL);

    return(ok);
}

/******************************************************************************/

BOOL BUTTON_KeyLeftRight( BYTE &code )
{
BOOL ok;

    ok = BUTTON_Key(code,BUTTON_LeftRight);

    return(ok);
}

/******************************************************************************/

BOOL BUTTON_KeyNoYes( BYTE &code )
{
BOOL ok;

    ok = BUTTON_Key(code,BUTTON_NoYes);

    return(ok);
}

/******************************************************************************/

BOOL BUTTON_KeyQWER( BYTE &code )
{
BOOL ok;

    ok = BUTTON_Key(code,BUTTON_QWER);

    return(ok);
}

/******************************************************************************/

BOOL BUTTON_KeyUIOP( BYTE &code )
{
BOOL ok;

    ok = BUTTON_Key(code,BUTTON_UIOP);

    return(ok);
}

/******************************************************************************/

BOOL BUTTON_ESC( void )
{
BOOL flag;
BYTE code;

    BUTTON_Reset();

    while( !BUTTON_Key(code) );

    flag = (code == ESC);

    return(flag);
}

/******************************************************************************/

BOOL BUTTON_TaskKey( BYTE &code, BYTE xlat[] )
{
BOOL flag;

    BUTTON_LoopTask();

    flag = BUTTON_Key(code,xlat);

    return(flag);
}

/******************************************************************************/

BOOL BUTTON_TaskKey( BYTE &code )
{
BOOL flag;

    flag = BUTTON_TaskKey(code,NULL);

    return(flag);
}

/******************************************************************************/

BOOL BUTTON_TaskGetEvent( BYTE buttons[] )
{
BOOL flag;

    BUTTON_LoopTask();

    flag = BUTTON_GetEvent(buttons);

    return(flag);
}

/******************************************************************************/

BOOL BUTTON_TaskButton( int &button )
{
BOOL flag;

    BUTTON_LoopTask();

    flag = BUTTON_Button(button);

    return(flag);
}

/******************************************************************************/

