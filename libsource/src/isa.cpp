/******************************************************************************/
/*                                                                            */ 
/* MODULE  : ISA.cpp                                                          */ 
/*                                                                            */ 
/* PURPOSE : PC ISA Bus Interface functions.                                  */ 
/*                                                                            */ 
/* DATE    : 11/May/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 11/May/2000 - Initial development.                               */ 
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     "ISA"
#define MODULE_TEXT     "PC ISA Bus Interface API"
#define MODULE_DATE     "28/06/2000"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL    0

/******************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

PRINTF  ISA_PRN_messgf=NULL;                     // General messages printf function.
PRINTF  ISA_PRN_errorf=NULL;                     // Error messages printf function.
PRINTF  ISA_PRN_debugf=NULL;                     // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     ISA_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ISA_PRN_messgf,buff));
}

/******************************************************************************/

int     ISA_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ISA_PRN_errorf,buff));
}

/******************************************************************************/

int     ISA_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ISA_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    ISA_API_started=FALSE;

/******************************************************************************/

BOOL    ISA_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL     ok;

    ISA_PRN_messgf = messgf;           // General API message print function.
    ISA_PRN_errorf = errorf;           // API error message print function.
    ISA_PRN_debugf = debugf;           // Debug information print function.

    if( ISA_API_started )              // Start the API once...
    {
        return(TRUE);
    }

    ok = ISA_patch();                  // Install WindowsNT ISA patch.

    if( ok )
    {
        ATEXIT_API(ISA_API_stop);      // Install stop function.
        ISA_API_started = TRUE;        // Set started flag.

        MODULE_start(ISA_PRN_messgf);  // Register module.
    }
    else
    {
        ISA_errorf("ISA_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    ISA_API_stop( void )
{
    if( !ISA_API_started )             // API not started in the first place...
    {
        return;
    }

    ISA_API_started = FALSE;           // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    ISA_API_check( void )
{
BOOL    ok=TRUE;

    if( !ISA_API_started )             // API not started...
    {                                  // Start module automatically...
        ok = ISA_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        STR_printf(ok,ISA_debugf,ISA_errorf,"ISA_API_check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/******************************************************************************/

BOOL    ISA_patch( void )              // Install WindowsNT ISA patch.
{
BOOL    ok=FALSE;
HANDLE  fh=INVALID_HANDLE_VALUE;

    fh = CreateFile("\\\\.\\giveio",   // Open giveIO patch device...
                    GENERIC_READ,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);

    if( fh != INVALID_HANDLE_VALUE )
    {
        ISA_debugf("ISA_patch() GIVEIO device opened.\n");

        CloseHandle(fh);               // Device does not need to stay open.
        ok = TRUE;
    }
    else
    {
        ISA_errorf("ISA_patch() GIVEIO device could not be opened.\n");
    }

    return(ok);
}

/******************************************************************************/

void    ISA_word( USHORT addr, USHORT word )
{
    if( !ISA_API_check() )        // Make sure API is started.
    {
        return;
    }

    (void)_outpw(addr,word);      // Write word to ISA port.
}

/******************************************************************************/

USHORT  ISA_word( USHORT addr )                  
{
    if( !ISA_API_check() )        // Make sure API is started.
    {
        return(0);
    }

    return(_inpw(addr));          // Read word from ISA port.
}

/******************************************************************************/

void    ISA_byte( USHORT addr, BYTE byte )
{
    if( !ISA_API_check() )        // Make sure API is started.
    {
        return;
    }

    (void)_outp(addr,byte);       // Write byte to ISA port.
}

/******************************************************************************/

BYTE    ISA_byte( USHORT addr )                  
{
    if( !ISA_API_check() )        // Make sure API is started.
    {
        return(0);
    }

    return(_inp(addr));           // Read byte from ISA port.
}

/******************************************************************************/
/* LPT (Parallel port) port I/O functions...                                  */
/******************************************************************************/

BYTE    LPT_mode[] = { LPT_INVALID,LPT_DIR_OUT,LPT_DIR_OUT };
USHORT  LPT_addr[] = { 0x0378,0x0278 };

/******************************************************************************/

USHORT  LPT_base( int LPTn )
{
USHORT  base=LPT_INVALID;

    if( (LPTn >= 1) && (LPTn <= 2) )
    {
        base = LPT_addr[LPTn-1];
    }

    return(base);
}

/******************************************************************************/

void    LPT_direction( int LPTn, BYTE direction )
{
     LPT_bit(LPTn,LPT_PORT_CTRL,LPT_CTRL_DIR,direction);
     LPT_mode[LPTn] = direction;
}

/******************************************************************************/

void    LPT_byte( int LPTn, USHORT port, BYTE data )
{
USHORT  base;

    if( (base=LPT_base(LPTn)) == LPT_INVALID )
    {
        return;
    }

    if( (port == LPT_PORT_DATA) && (LPT_mode[LPTn] != LPT_DIR_OUT) )
    {
        LPT_direction(LPTn,LPT_DIR_OUT);
    }
    
    ISA_byte(base+port,data);
}

/******************************************************************************/

BYTE    LPT_byte( int LPTn, USHORT port )
{
USHORT  base;
BYTE    data=0;
BOOL    change=FALSE;

    if( (base=LPT_base(LPTn)) == LPT_INVALID )
    {
        return(0);
    }

    if( (port == LPT_PORT_DATA) && (LPT_mode[LPTn] != LPT_DIR_IN) )
    {
        LPT_direction(LPTn,LPT_DIR_IN);
        change = TRUE;
    }

    data = ISA_byte(base + port);

    if( change )
    {
        LPT_direction(LPTn,LPT_DIR_OUT);
    }

    return(data);
}

/******************************************************************************/

void    LPT_bits( int LPTn, USHORT port, BYTE bits[] )
{
BYTE    data;

    data = BIT_byte(bits);
    LPT_byte(LPTn,port,data);
}

/******************************************************************************/

void    LPT_bit( int LPTn, USHORT port, BYTE bit, BYTE value )
{
BYTE    data,mask;

    data = LPT_byte(LPTn,port);
    mask = (1 << bit);

    if( value )
    {
        data |= mask;
    }
    else
    {
        data &= ~mask;
    }

    LPT_byte(LPTn,port,data);
}

/******************************************************************************/

void LPT_data( int LPTn, BYTE byte )
{
    LPT_byte(LPTn,LPT_PORT_DATA,byte);
}

/******************************************************************************/

BYTE LPT_data( int LPTn )
{
BYTE data;

    data = LPT_byte(LPTn,LPT_PORT_DATA);

    return(data);
}

/******************************************************************************/

void LPT_stat( int LPTn, BYTE byte )
{
    LPT_byte(LPTn,LPT_PORT_STAT,byte);
}

/******************************************************************************/

BYTE LPT_stat( int LPTn )
{
BYTE data;

    data = LPT_byte(LPTn,LPT_PORT_STAT);

    return(data);
}

/******************************************************************************/

void LPT_ctrl( int LPTn, BYTE byte )
{
    LPT_byte(LPTn,LPT_PORT_CTRL,byte);
}

/******************************************************************************/

BYTE LPT_ctrl( int LPTn )
{
BYTE data;

    data = LPT_byte(LPTn,LPT_PORT_CTRL);

    return(data);
}

/******************************************************************************/
/* BIT conversions...                                                         */
/******************************************************************************/

void    BIT_byte( BYTE value, BYTE bits[] )
{
int     b,k;

    for( k=1,b=0; (b < BIT_BYTE); b++ )
    {
        bits[b] = (value & k) ? 1 : 0;
        k += k;
    }
}

/******************************************************************************/

BYTE    BIT_byte( BYTE bits[] )
{
int     b,k;
BYTE    value;

    for( value=0,k=1,b=0; (b < BIT_BYTE); b++ )
    {
        value += bits[b] * k;
        k += k;
    }

    return(value);
}

/******************************************************************************/

int BIT_count( DWORD value )
{
int count,i;

    for( count=0,i=0; (i < BIT_DWORD); i++ )
    {
        if( (value & (1 << i)) != 0 )
        {
            count++;
        }
    }

    return(count);
}

/******************************************************************************/

void    BIT_word( WORD value, BYTE bits[] )
{
int     b,k;

    for( k=1,b=0; (b < BIT_WORD); b++ )
    {
        bits[b] = (value & k) ? 1 : 0;
        k += k;
    }
}

/******************************************************************************/

WORD    BIT_word( BYTE bits[] )
{
int     b,k;
WORD    value;

    for( value=0,k=1,b=0; (b < BIT_WORD); b++ )
    {
        value += bits[b] * k;
        k += k;
    }

    return(value);
}

/******************************************************************************/

char   *BIT_text( int value, int size )
{
char   *buff;
BYTE    bits[BIT_MAX];
int     b;

    if( (buff=STR_Buff(size+1)) == NULL )
    {
        return(NULL);
    }

    switch( size )
    {
        case BIT_BYTE :
           BIT_byte(value,bits);
           break;

        case BIT_WORD :
           BIT_word(value,bits);
           break;
    }

    for( b=0; (b < size); b++ )
    {
        buff[size-(b+1)] = 0x30 + bits[b];
    }

    return(buff);
}

/******************************************************************************/

char   *BIT_text( BYTE value )
{
char   *buff;

    buff = BIT_text(value,BIT_BYTE);

    return(buff);
}

/******************************************************************************/

BOOL  VGA_VerticalRetraceFlag=FALSE;
TIMER VGA_VerticalRetraceTimer("VGA_VerticalRetraceTimer");

/******************************************************************************/

BOOL VGA_VerticalRetrace( void )
{
BOOL flag;
BYTE status;

    status = ISA_byte(VGA_PORT_STATUS);
    flag = ((status & VGA_STATUS_RETRACE) != 0);

    return(flag);
}

/******************************************************************************/

BOOL VGA_VerticalRetraceOnset( void )
{
BOOL flag,changed=FALSE;

    flag = VGA_VerticalRetrace();

    if( flag && !VGA_VerticalRetraceFlag )
    {
        VGA_VerticalRetraceFlag = TRUE;
        changed = TRUE;
    }

    return(changed);
}

/******************************************************************************/

BOOL VGA_VerticalRetraceOffset( void )
{
BOOL flag,changed=FALSE;

    flag = VGA_VerticalRetrace();

    if( !flag && VGA_VerticalRetraceFlag )
    {
        VGA_VerticalRetraceFlag = FALSE;
        changed = TRUE;
    }

    return(changed);
}

/******************************************************************************/

BOOL VGA_VerticalRetraceOnsetOffset( TIMER *timer, double *onset_sec, double *offset_sec  )
{
    if( !VGA_VerticalRetrace() )
    {
        return(FALSE);
    }

    if( timer != NULL )
    {
       *onset_sec = timer->ElapsedSeconds();
    }

    while( VGA_VerticalRetrace() );

    if( timer != NULL )
    {
       *offset_sec = timer->ElapsedSeconds();
    }

    return(TRUE);
}

/******************************************************************************/

BOOL VGA_VerticalRetraceOnsetOffset( double &latency_sec )
{
BOOL flag=FALSE;
double t0,t1;

    if( VGA_VerticalRetraceOnsetOffset(&VGA_VerticalRetraceTimer,&t0,&t1) )
    {
        flag = TRUE;
        latency_sec = t0-t1;
    }

    return(flag);
}

/******************************************************************************/

