/******************************************************************************/
/*                                                                            */
/* MODULE  : COM.cpp                                                          */
/*                                                                            */
/* PURPOSE : RS232 Communications API.                                        */
/*                                                                            */
/* DATE    : 27/Dec/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 27/Dec/2000 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 10/Sep/2001 - Use integer handle rather than Win32 HANDLE.       */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "COM"
#define MODULE_TEXT     "RS232 COMs API"
#define MODULE_DATE     "10/09/2001"
#define MODULE_VERSION  "1.1"
#define MODULE_LEVEL    1

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

BOOL    COM_API_started = FALSE;
struct  COM_HandleItem  COM_Item[COM_MAX];

/******************************************************************************/

struct  STR_TextItem  COM_ModemStatusText[] =
{
    { MS_DSR_ON,"DSR" },
    { MS_CTS_ON,"CTS" },
    { MS_RING_ON,"RING" },
    { MS_RLSD_ON,"RLSD" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem  COM_SignalText[] =
{
    { CLRDTR,"DTR Clear" },
    { SETDTR,"DTR Set"   },
    { CLRRTS,"RTS Clear" },
    { SETRTS,"RTS Set"   },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

PRINTF  COM_PRN_messgf=NULL;                     // General messages printf function.
PRINTF  COM_PRN_errorf=NULL;                     // Error messages printf function.
PRINTF  COM_PRN_debugf=NULL;                     // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     COM_messgf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(COM_PRN_messgf,buff));
}

/******************************************************************************/

int     COM_errorf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(COM_PRN_errorf,buff));
}

/******************************************************************************/

int     COM_debugf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(COM_PRN_debugf,buff));
}

/******************************************************************************/

void    COM_Init( int item )
{
    COM_Item[item].comX = 0;
    COM_Item[item].comH = INVALID_HANDLE_VALUE;
    COM_Item[item].timer = NULL;
}

/******************************************************************************/

BOOL    COM_Check( int item )
{
BOOL    ok=TRUE;

    if( !COM_API_check() )
    {
        ok = FALSE;
    }
    else
    if( (item < 0) || (item > COM_MAX) )
    {
        ok = FALSE;
    }
    if( COM_Item[item].comH == INVALID_HANDLE_VALUE )
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

int     COM_Find( HANDLE comH )
{
int     item,find;

    if( !COM_API_check() )                      // Check if API started okay...
    {
        return(COM_INVALID);
    }

    for( find=COM_INVALID,item=0; (item < COM_MAX); item++ )
    {
        if( COM_Item[item].comH == comH )
        {
            find = item;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

HANDLE  COM_Handle( int item )
{
HANDLE  comH=INVALID_HANDLE_VALUE;

    if( COM_Check(item) )                   // Make sure handle is valid...
    {
        comH = COM_Item[item].comH;
    }

    return(comH);
}

/******************************************************************************/

BOOL    COM_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL    ok;
int     item;

    if( COM_API_started )                   // Start the API once...
    {
        return(TRUE);
    }

    COM_PRN_messgf = messgf;                // General API message print function.
    COM_PRN_errorf = errorf;                // API error message print function.
    COM_PRN_debugf = debugf;                // Debug information print function.

    for( item=0; (item < COM_MAX); item++ )
    {
        COM_Init(item);
    }

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(COM_API_stop);           // Install stop function.
        COM_API_started = TRUE;             // Set started flag.

        MODULE_start(COM_PRN_messgf);       // Register module.
    }
    else
    {
        COM_errorf("COM_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    COM_API_stop( void )
{
    if( !COM_API_started )                  // API not started in the first place...
    {
         return;
    }

    COM_CloseAll();

    COM_API_started = FALSE;           // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    COM_API_check( void )
{
BOOL    ok=TRUE;

    if( !COM_API_started )             // API not started...
    {                                  // Start module automatically...
        ok = COM_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        STR_printf(ok,COM_debugf,COM_errorf,"COM_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

char   *COM_Error( void )
{
DWORD   rc;

    rc = GetLastError();
    return(STR_stringf("%ld",rc));
}

/******************************************************************************/

int     COM_Open( int comX, long baud, void (*SetDCB)( DCB *dcb ) )
{
int     item;
HANDLE  comH;

//  Find a free handle...
    if( (item=COM_Find(INVALID_HANDLE_VALUE)) == COM_INVALID )
    {
        COM_errorf("COM_Open(COM%d) No free handles.\n",comX);
        return(COM_INVALID);
    }


//  Open COM port...
    comH = CreateFile(STR_stringf("\\\\.\\COM%d",comX),    // File: comX
		      GENERIC_READ | GENERIC_WRITE,        // Access: Input / Output.
		      0,                                   // Shared: Exclusive Access.
		      NULL,                                // Security: None
		      OPEN_EXISTING,                       // Create: Open existing.
		      0,                                   // Flags: Non-overlapped (synchronous).
		      NULL);                               // Template: None. 

//  Did we open it?
    if( comH == INVALID_HANDLE_VALUE )
    {
        COM_errorf("COM_Open(COM%d) CreateFile() Failed (%s).\n",comX,COM_Error());
        return(COM_INVALID);
    }

    COM_Item[item].comX = comX;
    COM_Item[item].comH = comH;
    COM_Item[item].timer = new TIMER(STR_stringf("COM%d",comX));

//  Do we want to set up COM port now?
    if( (baud != COM_BAUD_NULL) || (SetDCB != NULL) )
    {
        if( !COM_Setup(item,baud,SetDCB) )
        {
            COM_errorf("COM_Open(COM%d) Cannot setup COM port.\n",comX);
        }
    }

    return(item);
}

/******************************************************************************/

int     COM_Open( int comX, long baud )
{
int     item;

    item = COM_Open(comX,baud,NULL);

    return(item);
}

/******************************************************************************/

int     COM_Open( int comX )
{
int     item;

    item = COM_Open(comX,COM_BAUD_NULL,NULL);

    return(item);
}

/******************************************************************************/

BOOL    COM_Close( HANDLE comH )
{
BOOL    ok=TRUE;

    if( !CloseHandle(comH) )
    {
        COM_errorf("COM_Close(...) CloseHandle() Failed (%s).\n",COM_Error());
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    COM_Close( int item )
{
BOOL    ok;

    if( !COM_Check(item) )
    {
        return(FALSE);
    }

    ok = COM_Close(COM_Item[item].comH);

    if( COM_Item[item].timer != NULL )
    {
        delete COM_Item[item].timer;
        COM_Item[item].timer = NULL;
    }

    COM_Init(item);

    return(ok);
}

/******************************************************************************/

void    COM_CloseAll( void )
{
int     item;
BOOL    ok;

    for( item=0; (item < COM_MAX); item++ )
    {
        ok = COM_Close(item);
    }
}

/******************************************************************************/

BOOL    COM_Setup( int item, long baud, void (*SetDCB)( DCB *dcb ) )
{
DCB           COM_DCB;
COMMTIMEOUTS  COM_TimeOuts;

    if( !COM_Check(item) )
    {
        return(FALSE);
    }

//  Set size of buffers...
    if( !SetupComm(COM_Item[item].comH,COM_BUFF,COM_BUFF) )
    {
        COM_errorf("COM_Setup(COM%d) SetupComm() Failed (%s).\n",COM_Item[item].comX,COM_Error());
        return(FALSE);
    }

//  Get and then set RS232 characteristics...
    if( !GetCommState(COM_Item[item].comH,&COM_DCB) )
    {
        COM_errorf("COM_Setup(COM%d) GetCommState() Failed (%s).\n",COM_Item[item].comX,COM_Error());
        return(FALSE);
    }

//  Set RS232 characteristics...
    COM_DCB.DCBlength = sizeof(DCB);
    COM_DCB.BaudRate = baud;

//  User-defined DCB setup...
    if( SetDCB != NULL )
    {
       (*SetDCB)(&COM_DCB);
    }

    if( !SetCommState(COM_Item[item].comH,&COM_DCB) )
    {
        COM_errorf("COM_Setup(COM%d) SetCommState() Failed (%s).\n",COM_Item[item].comX,COM_Error());
        return(FALSE);
    }

//  Get and then set RS232 timeout values...
    if( !GetCommTimeouts(COM_Item[item].comH,&COM_TimeOuts) )
    {
        COM_errorf("COM_Setup(COM%d) GetCommTimeouts() Failed (%s).\n",COM_Item[item].comX,COM_Error());
        return(FALSE);
    }

    COM_debugf("GetCommTimeouts() RdIT=%ld RdTTM=%ld RdTTC=%ld WrTTM=%ld WrTTC=%ld (msec).\n",
               COM_TimeOuts.ReadIntervalTimeout,
               COM_TimeOuts.ReadTotalTimeoutMultiplier,
               COM_TimeOuts.ReadTotalTimeoutConstant,
               COM_TimeOuts.WriteTotalTimeoutMultiplier,
               COM_TimeOuts.WriteTotalTimeoutConstant);

//  Set RS232 timeout (values = no wait)...
    COM_TimeOuts.ReadIntervalTimeout = COM_RdIT;
    COM_TimeOuts.ReadTotalTimeoutMultiplier = COM_RdTTM;
    COM_TimeOuts.ReadTotalTimeoutConstant = COM_RdTTC;

    if( !SetCommTimeouts(COM_Item[item].comH,&COM_TimeOuts) )
    {
        COM_errorf("COM_Setup(COM%d) SetCommTimeouts() Failed (%s).\n",COM_Item[item].comX,COM_Error());
        return(FALSE);
    }

    COM_debugf("SetCommTimeouts() RdIT=%ld RdTTM=%ld RdTTC=%ld WrTTM=%ld WrTTC=%ld (msec).\n",
               COM_TimeOuts.ReadIntervalTimeout,
               COM_TimeOuts.ReadTotalTimeoutMultiplier,
               COM_TimeOuts.ReadTotalTimeoutConstant,
               COM_TimeOuts.WriteTotalTimeoutMultiplier,
               COM_TimeOuts.WriteTotalTimeoutConstant);

    return(TRUE);
} 

/******************************************************************************/

BOOL    COM_ModemStatus( int item, DWORD *stat )
{
BOOL    ok=TRUE;

    if( !GetCommModemStatus(COM_Item[item].comH,stat) )
    {
        COM_errorf("COM_ModemStatus(COM%d) GetCommModemStatus() Failed (%s).\n",COM_Item[item].comX,COM_Error());
        ok = FALSE;
    }
    else
    {
        COM_debugf("COM_ModemStatus(COM%d) [%08lX] %s.\n",COM_Item[item].comX,*stat,STR_TextFlag(COM_ModemStatusText,*stat));
    }

    return(ok);
}

/******************************************************************************/

BOOL    COM_ModemStatus( int item, BOOL *CTS, BOOL *DSR, BOOL *Ring, BOOL *RLSD )
{
BOOL    ok;
DWORD   stat;

    if( (ok=COM_ModemStatus(item,&stat)) )
    {
        if( CTS != NULL )
        {
            *CTS = ((stat & MS_CTS_ON) != 0);
        }

        if( DSR != NULL )
        {
            *DSR = ((stat & MS_DSR_ON) != 0);
        }

        if( Ring != NULL )
        {
            *Ring = ((stat & MS_RING_ON) != 0);
        }

        if( RLSD != NULL )
        {
            *RLSD = ((stat & MS_RLSD_ON) != 0);
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    COM_Signal( int item, DWORD signal )
{
BOOL    ok=TRUE;

    if( !EscapeCommFunction(COM_Item[item].comH,signal) )
    {
        COM_errorf("COM_Signal(COM%d,%s) EscapeCommFunction() Failed (%s).\n",COM_Item[item].comX,STR_TextCode(COM_SignalText,signal),COM_Error());
        ok = FALSE;
    }
    else
    {
        COM_debugf("COM_Signal(COM%d,%s) OK.\n",COM_Item[item].comX,STR_TextCode(COM_SignalText,signal));
    }

    return(ok);
}

/******************************************************************************/

BOOL    COM_RTS( int item, BOOL flag )
{
BOOL    ok=TRUE;

    ok = COM_Signal(item,flag ? SETRTS : CLRRTS);

    return(ok);
}

/******************************************************************************/

BOOL    COM_DTR( int item, BOOL flag )
{
BOOL    ok=TRUE;

    ok = COM_Signal(item,flag ? SETDTR : CLRDTR);

    return(ok);
}

/******************************************************************************/

BOOL    COM_Write( int item, void *buff, int size )
{
BOOL    ok=TRUE;
DWORD   done;

    if( COM_Handle(item) == INVALID_HANDLE_VALUE )
    {
        COM_errorf("COM_Write(...) Invalid handle.\n");
        return(FALSE);
    }

    if( !WriteFile(COM_Item[item].comH,buff,size,&done,NULL) )
    {
        COM_errorf("COM_Write(COM%d) WriteFile() Failed (%s).\n",COM_Item[item].comX,COM_Error());
        ok = FALSE;
    }
    else
    if( size != (int)done )
    {
        COM_errorf("COM_Write(COM%d) WriteFile() Incomplete (size=%d, done=%ld).\n",COM_Item[item].comX,size,done);
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    COM_Read( int item, void *buff, int size, int &got, BOOL &complete, double wait_msec )
{
BOOL    ok=TRUE,expired=TRUE;
DWORD   done=0;
BYTE   *bptr=(BYTE *)buff;

    complete = FALSE;
    got = 0;

    if( COM_Handle(item) == INVALID_HANDLE_VALUE )
    {
        COM_errorf("COM_Read(...) Invalid handle.\n");
        return(FALSE);
    }

    COM_Item[item].timer->Reset();

    do
    {
        if( !ReadFile(COM_Item[item].comH,&bptr[got],size-got,&done,NULL) )
        {
            COM_errorf("COM_Read(COM%d) ReadFile() Failed (%s).\n",COM_Item[item].comX,COM_Error());
            ok = FALSE;
            continue;
        }

        got += (int)done;
        complete = (size == got);
        expired = COM_Item[item].timer->Expired(wait_msec);
    }
    while( ok && !complete && !expired );

    //if( ok && !complete && (wait_msec != 0L) )
    //{
    //    COM_debugf("COM_Read(COM%d) ReadFile() Incomplete (size=%d, got=%d).\n",COM_Item[item].comX,size,got);
    //}

    return(ok);
}

/******************************************************************************/

BOOL    COM_Read( int item, void *buff, int size, BOOL &complete, double wait_msec )
{
BOOL    ok;
int     got;

    ok = COM_Read(item,buff,size,got,complete,wait_msec);

    return(ok);
}

/******************************************************************************/

BOOL    COM_Read( int item, void *buff, int size, double wait_msec )
{
BOOL    ok,complete;

    ok = COM_Read(item,buff,size,complete,wait_msec);

    return(ok && complete);
}

/******************************************************************************/

BOOL    COM_Read( int item, void *buff, int size )
{
BOOL    ok;

    ok = COM_Read(item,buff,size,COM_RdTO);

    return(ok);
}

/******************************************************************************/

BOOL    COM_Reset( int item )
{
BOOL    ok,complete;
BYTE    buff[COM_BUFF];

    do
    {
        ok = COM_Read(item,buff,sizeof(buff),complete,0.0);
    }
    while( ok && complete );

    return(ok);
}

/******************************************************************************/

