/******************************************************************************/
/*                                                                            */ 
/* MODULE  : GOGGLES.cpp                                                      */ 
/*                                                                            */ 
/* PURPOSE : Plato Goggles Interface API.                                     */ 
/*                                                                            */ 
/* DATE    : 18/Nov/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 18/Nov/2000 - Initial development.                               */ 
/*                                                                            */ 
/* V1.1  JNI 26/Mar/2002 - Save mask rather than try read it back from port.  */ 
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     "GOGGLES"
#define MODULE_TEXT     "Plato Goggles Interface API"
#define MODULE_DATE     "26/03/2002"
#define MODULE_VERSION  "1.1"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

UCHAR   GOGGLES_mask[]={ 0x01,0x02,0x03 };       // Bit masks for left, right & both.
int     GOGGLES_LPT=GOGGLES_LPT_INVALID;         // LPT (parallel) port number.
UCHAR   GOGGLES_current=0x00;                    // Current value of port mask. (V1.1)

/******************************************************************************/

struct  STR_TextItem  GOGGLES_EyeText[] =
{
    { GOGGLES_EYE_LEFT,"Left" },
    { GOGGLES_EYE_RIGHT,"Right" },
    { GOGGLES_EYE_BOTH,"Both" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem  GOGGLES_StateText[] =
{
    { GOGGLES_OPAQUE,"Opaque" },
    { GOGGLES_TRANSPARENT,"Transparent" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

PRINTF  GOGGLES_PRN_messgf=NULL;                 // General messages printf function.
PRINTF  GOGGLES_PRN_errorf=printf;               // Error messages printf function.
PRINTF  GOGGLES_PRN_debugf=NULL;                 // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     GOGGLES_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(GOGGLES_PRN_messgf,buff));
}

/******************************************************************************/

int     GOGGLES_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(GOGGLES_PRN_errorf,buff));
}

/******************************************************************************/

int     GOGGLES_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(GOGGLES_PRN_debugf,buff));
}

/******************************************************************************/

UCHAR   GOGGLES_portlast=0x00;

void    GOGGLES_port( int LPT, UCHAR mask )
{
    if( LPT != LPT_INVALID )
    {
        LPT_byte(LPT,LPT_PORT_DATA,mask);
        GOGGLES_current = mask;   // Save mask value. (V1.1)
    }
}

/******************************************************************************/

UCHAR   GOGGLES_port( int LPT )
{
UCHAR   port=0x00;

    if( LPT != LPT_INVALID )
    {
    //  port = LPT_byte(LPT,LPT_PORT_DATA);
        port = GOGGLES_current;   // Use saved mask value. (V1.1)
    }

    return(port);
}

/******************************************************************************/

BOOL    GOGGLES_open( int LPT )
{
int     ISA;

    if( (ISA=LPT_base(LPT)) == LPT_INVALID )
    {
        GOGGLES_errorf("GOGGLES_open(LPT=%d) Invalid port number.\n",LPT);
        return(FALSE);
    }

    GOGGLES_debugf("GOGGLES_open(LPT=%d) ISA port 0x%04X.\n",LPT,ISA);
    GOGGLES_LPT = LPT;

    GOGGLES_set(GOGGLES_EYE_BOTH,GOGGLES_OPAQUE);

    return(TRUE);
}

/******************************************************************************/

BOOL GOGGLES_open( void )
{
int LPT=1;
BOOL ok;

    ok = GOGGLES_open(LPT);

    return(ok);
}

/******************************************************************************/

void    GOGGLES_close( void )
{
    GOGGLES_LPT = GOGGLES_LPT_INVALID;
}

/******************************************************************************/

void    GOGGLES_set( int eye, UCHAR state )
{
UCHAR   last,data,mask;

    last = GOGGLES_port(GOGGLES_LPT);

    switch( state )
    {
        case GOGGLES_TRANSPARENT :
           data = last & ~GOGGLES_mask[eye];
           break;

        case GOGGLES_OPAQUE :
           data = last | GOGGLES_mask[eye];
           break;
    }

    if( data != last )
    {
        GOGGLES_port(GOGGLES_LPT,data);
    }
}

/******************************************************************************/

UCHAR   GOGGLES_state( int eye )
{
UCHAR   state;

    state = (GOGGLES_port(GOGGLES_LPT) & GOGGLES_mask[eye]) ? GOGGLES_OPAQUE : GOGGLES_TRANSPARENT;

    return(state);
}

/******************************************************************************/

void    GOGGLES_state( UCHAR state[] )
{
int     eye;

    for( eye=GOGGLES_EYE_LEFT; (eye <= GOGGLES_EYE_RIGHT); eye++ )
    {
        state[eye] = GOGGLES_state(eye);
    }
}

/******************************************************************************/

void    GOGGLES_state( UCHAR &left, UCHAR &right )
{
UCHAR   state[GOGGLES_EYE_BOTH];

    GOGGLES_state(state);
    left  = state[GOGGLES_EYE_LEFT];
    right = state[GOGGLES_EYE_RIGHT];
}

/******************************************************************************/

void    GOGGLES_toggle( int eye )
{
UCHAR   state;

    state = GOGGLES_state(eye);
    GOGGLES_set(eye,(state == GOGGLES_OPAQUE) ? GOGGLES_TRANSPARENT : GOGGLES_OPAQUE);
}

/******************************************************************************/

void    GOGGLES_swap( void )
{
int     eye;

    for( eye=GOGGLES_EYE_LEFT; (eye <= GOGGLES_EYE_RIGHT); eye++ )
    {
        GOGGLES_toggle(eye);
    }
}

/******************************************************************************/

void    GOGGLES_left( UCHAR state )
{
    GOGGLES_set(GOGGLES_EYE_LEFT,state);
}

/******************************************************************************/

void    GOGGLES_right( UCHAR state )
{
    GOGGLES_set(GOGGLES_EYE_RIGHT,state);
}

/******************************************************************************/

void    GOGGLES_both( UCHAR state )
{
    GOGGLES_set(GOGGLES_EYE_BOTH,state);
}

/******************************************************************************/

void    GOGGLES_opaque( int eye )
{
    GOGGLES_set(eye,GOGGLES_OPAQUE);
}

/******************************************************************************/

void    GOGGLES_transparent( int eye )
{
    GOGGLES_set(eye,GOGGLES_TRANSPARENT);
}

/******************************************************************************/

void    GOGGLES_left_opaque( void )
{
    GOGGLES_set(GOGGLES_EYE_LEFT,GOGGLES_OPAQUE);
}

/******************************************************************************/

void    GOGGLES_left_transparent( void )
{
    GOGGLES_set(GOGGLES_EYE_LEFT,GOGGLES_TRANSPARENT);
}

/******************************************************************************/

void    GOGGLES_right_opaque( void )
{
    GOGGLES_set(GOGGLES_EYE_RIGHT,GOGGLES_OPAQUE);
}

/******************************************************************************/

void    GOGGLES_right_transparent( void )
{
    GOGGLES_set(GOGGLES_EYE_RIGHT,GOGGLES_TRANSPARENT);
}

/******************************************************************************/

void    GOGGLES_both_oqaque( void )
{
    GOGGLES_set(GOGGLES_EYE_BOTH,GOGGLES_OPAQUE);
}

/******************************************************************************/

void    GOGGLES_both_transparent( void )
{
    GOGGLES_set(GOGGLES_EYE_BOTH,GOGGLES_TRANSPARENT);
}

/******************************************************************************/
