/******************************************************************************/
/*                                                                            */
/* MODULE  : SENSORAY.cpp                                                     */
/*                                                                            */
/* PURPOSE : Sensoray 626 MultiFunction I/O card - API                        */
/*                                                                            */

/* DATE    : 01/Aug/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 01/Aug/2002 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 11/Mar/2004 - Allow multiple cards to be addressed.              */
/*                                                                            */
/* V2.0  JNI 26/Apr/2007 - Encoder support (moved from CONTROLLER module).    */
/*                                                                            */
/*       JNI 26/Apr/2007 - Map two virtual cards onto a single card.          */
/*                                                                            */
/* V2.1  JNI 22/May/2009 - Catch undefined double values for DAC volts.       */
/*                                                                            */
/* V3.0  JNI 17/Jul/2013 - Added support for Sensoray 826 (S826) card.        */
/*                                                                            */
/* V3.1  JNI 15/Apr/2015 - Ongoing development of S826 interface for 3BOT.    */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "SENSORAY"
#define MODULE_TEXT     "Sensoray Multi-I/O Card API"
#define MODULE_DATE     "15/04/2015"
#define MODULE_VERSION  "3.3.8"
#define MODULE_LEVEL    2

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

BOOL SENSORAY_API_started = FALSE;

/******************************************************************************/

struct  STR_TextItem  S626_ErrorText[] =
{
    { ERR_OPEN        ,"OPEN DRIVER"       },
    { ERR_CARDREG     ,"REGISTER BOARD"    },
    { ERR_ALLOC_MEMORY,"MEMORY"            },
    { ERR_LOCK_BUFFER ,"DMA BUFFER"        },
    { ERR_THREAD      ,"INTERRUPT THREAD"  },
    { ERR_LOST_IRQ    ,"MISSED INTERRUPT"  },
    { ERR_INIT        ,"BOARD OBJECT"      },
    { ERR_ILLEGAL_PARM,"INVALID PARAMETER" },
    { ERR_DACTIMEOUT  ,"DAC TIME-OUT"      },
    { ERR_COUNTERSETUP,"INVALID COUNTER"   },
    { STR_TEXT_ENDOFTABLE },
};



/******************************************************************************/

char  *SENSORAY_CardTypeText[] = { "None","S626","S826",NULL };
struct SENSORAY_Handle SENSORAY_HandleList[SENSORAY_HANDLE_MAX];



WORD   S626_EncoderCounter[] = { CNTR_0A,CNTR_1A,CNTR_2A,CNTR_0B,CNTR_1B,CNTR_2B };
WORD   S626_EncoderIndexMask[] = { 0x0010,0x0040,0x0100,0x0020,0x0080,0x0200 };


char  *SENSORAY_ChannelType[] = { "DAC","ADC","DIO","ENC" };

double SENSORAY_ADC_GainVolts[] = { 5.0,10.0 };
int    S626_GainList[] = { ADC_RANGE_5V,ADC_RANGE_10V };


/******************************************************************************/

PRINTF SENSORAY_PRN_messgf=NULL;           // General messages printf function.
PRINTF SENSORAY_PRN_errorf=NULL;           // Error messages printf function.
PRINTF SENSORAY_PRN_debugf=NULL;           // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int SENSORAY_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    if( SENSORAY_PRN_messgf == NULL )
    {
        return(0);
    }

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(SENSORAY_PRN_messgf,buff));
}

/******************************************************************************/

int SENSORAY_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    if( SENSORAY_PRN_errorf == NULL )
    {
        return(0);
    }

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(SENSORAY_PRN_errorf,buff));
}

/******************************************************************************/

int SENSORAY_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    if( SENSORAY_PRN_debugf == NULL )
    {
        return(0);
    }

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(SENSORAY_PRN_debugf,buff));
}

/******************************************************************************/

void SENSORAY_HandleClear( int handle )
{
int subaddress;

    SENSORAY_HandleList[handle].InUse = FALSE;
    SENSORAY_HandleList[handle].CardType = SENSORAY_CARDTYPE_NONE;
    SENSORAY_HandleList[handle].CardAddress = 0;
    SENSORAY_HandleList[handle].CardHandle = 0;

    for( subaddress=0; (subaddress < SENSORAY_SUBADDRESS_MAX); subaddress++ )
    {
        SENSORAY_HandleList[handle].SubAddress[subaddress].InUse = FALSE;
        SENSORAY_HandleList[handle].SubAddress[subaddress].ADC.Started = FALSE;
        SENSORAY_HandleList[handle].SubAddress[subaddress].ADC.Count = 0;
    }
}

/******************************************************************************/

void SENSORAY_HandleInit( void )
{
int handle;

    for( handle=0; (handle < SENSORAY_HANDLE_MAX); handle++ )
    {
        SENSORAY_HandleClear(handle);
    }
}

/******************************************************************************/

int SENSORAY_HandleFind( DWORD address )
{
int handle,find;

    for( find=SENSORAY_HANDLE_INVALID,handle=0; (handle < SENSORAY_HANDLE_MAX); handle++ )
    {
        if( SENSORAY_HandleList[handle].InUse && (SENSORAY_HandleList[handle].CardAddress == address) )
        {
            find = handle;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

int SENSORAY_HandleAvailable( void )
{
int handle,find;

    for( find=SENSORAY_HANDLE_INVALID,handle=0; (handle < SENSORAY_HANDLE_MAX); handle++ )
    {
        if( SENSORAY_HandleList[handle].InUse == FALSE )
        {
            find = handle;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

void SENSORAY_HandleClose( void )
{
int handle;

    for( handle=0; (handle < SENSORAY_HANDLE_MAX); handle++ )
    {
        if( SENSORAY_HandleList[handle].InUse )
        {
            SENSORAY_HandleClose(handle);
        }
    }
}

/******************************************************************************/

void SENSORAY_HandleClose( int handle )
{
    if( SENSORAY_HandleList[handle].InUse )
    {
        switch( SENSORAY_HandleList[handle].CardType )
        {
            case SENSORAY_CARDTYPE_S626 :
                S626_InterruptEnable(SENSORAY_HandleList[handle].CardHandle,FALSE);
                S626_CloseBoard(SENSORAY_HandleList[handle].CardHandle);
                break;


        }

        // Clear handle.
        SENSORAY_HandleClear(handle);
    }
}

/******************************************************************************/

BOOL SENSORAY_HandleInUse( int handle )
{
BOOL flag=FALSE;
int subaddress;

    if( SENSORAY_HandleList[handle].InUse )
    {
        for( subaddress=0; (subaddress < SENSORAY_SUBADDRESS_MAX); subaddress++ )
        {
            if( SENSORAY_HandleList[handle].SubAddress[subaddress].InUse )
            {
                flag = TRUE;
                break;
            }
        }
    }

    return(flag);
}

/******************************************************************************/

BOOL SENSORAY_HandleValid( int handle )
{
BOOL flag;

    // Handle is valid for use only if it is "in use".
    flag = SENSORAY_HandleInUse(handle);

    return(flag);
}

/******************************************************************************/

void SENSORAY_HandleClose( DWORD CardAddress, BYTE SubAddress )
{
int handle;

    // Find handle for this card address.
    if( (handle=SENSORAY_HandleFind(CardAddress)) == SENSORAY_HANDLE_INVALID )
    {
        SENSORAY_errorf("SENSORAY_HandleClose(CardAddress=0x%05X,SubAddress=0x%02X) Address not found.\n",CardAddress,SubAddress);
        return;
    }

    // Close particular subaddress for this handle.
    if( SENSORAY_HandleList[handle].SubAddress[SubAddress].InUse )
    {
        if( SENSORAY_HandleList[handle].SubAddress[SubAddress].ADC.Started )
        {
            // Stop ADC.
            SENSORAY_HandleList[handle].SubAddress[SubAddress].ADC.Started = FALSE;
        }

        SENSORAY_HandleList[handle].SubAddress[SubAddress].InUse = FALSE;
    }

    // Close handle completely if it's no longer being used.
    if( !SENSORAY_HandleInUse(handle) )
    {
        SENSORAY_HandleClose(handle);
    }
}

/******************************************************************************/

BOOL S626_HandleOpen( int handle, DWORD &CardAddress, BYTE SubAddress, void (*ISR)( void ) )
{
DWORD rc;
BOOL ok;

    // Open a S626 card.
    S626_OpenBoard(handle,        // Logical board number.
                   CardAddress,   // Physical address (0 = auto-detect).
                   ISR,           // Interrupt handling function.
                   THREAD_PRIORITY_ABOVE_NORMAL);

    ok = ((rc=S626_GetErrors(handle)) == 0);

    if( !ok )
    {
        SENSORAY_errorf("S626_OpenBoard(CardAddress=0x%05X) %s",CardAddress,STR_TextFlag(S626_ErrorText,rc));
    }
    else
    {
        // Get card address if none given.
        if( CardAddress == 0 )
        {
            CardAddress = S626_GetAddress(handle);
        }

        // Enable hardware interrupts if an ISR specified.
        if( ISR != NULL )
        {
            S626_InterruptEnable(handle,TRUE);
        }
    }

    return(ok);
}

/******************************************************************************/



/******************************************************************************/

int SENSORAY_HandleOpen( DWORD &CardAddress, BYTE SubAddress, void (*ISR)( void ) )
{
int handle;
BOOL ok;

    // See if card address is already open.
    if( (handle=SENSORAY_HandleFind(CardAddress)) == SENSORAY_HANDLE_INVALID )
    {
        // Find an available handle and open the card.
        if( (handle=SENSORAY_HandleAvailable()) != SENSORAY_HANDLE_INVALID )
        {
            // Shall we open an S626 or S826 card?

                if( (ok=S626_HandleOpen(handle,CardAddress,SubAddress,ISR)) )
                {
                    SENSORAY_HandleList[handle].CardType = SENSORAY_CARDTYPE_S626;
                }


            if( ok )
            {
                SENSORAY_HandleList[handle].InUse = TRUE;
                SENSORAY_HandleList[handle].CardHandle = handle;
                SENSORAY_HandleList[handle].CardAddress = CardAddress;

                SENSORAY_debugf("SENSORAY_HandleOpen(CardAddress=0x%05X,SubAddress=0x%02X) %s card physically opened (handle=%d).\n",CardAddress,SubAddress,SENSORAY_CardTypeText[SENSORAY_HandleList[handle].CardType],handle);
            }
        }
    }
    else
    {
        SENSORAY_debugf("SENSORAY_HandleOpen(CardAddress=0x%05X,SubAddress=0x%02X) %s card already open (handle=%d).\n",CardAddress,SubAddress,SENSORAY_CardTypeText[SENSORAY_HandleList[handle].CardType],handle);
    }

    // Did we get a valid handle for the S626 card?
    if( handle == SENSORAY_HANDLE_INVALID )
    {
        return(SENSORAY_HANDLE_INVALID);
    }

    // Is subaddress already in use?
    if( SENSORAY_HandleList[handle].SubAddress[SubAddress].InUse )
    {
        SENSORAY_errorf("SENSORAY_HandleOpen(CardAddress=0x%05X,SubAddress=0x%02X) %s SubAddress in use.\n",CardAddress,SubAddress,SENSORAY_CardTypeText[SENSORAY_HandleList[handle].CardType]);
        return(SENSORAY_HANDLE_INVALID);
    }

    // Success, set subaddress in-use flag.
    SENSORAY_HandleList[handle].SubAddress[SubAddress].InUse = TRUE;

    return(handle);
}

/******************************************************************************/

void SENSORAY_HandlePrint( int handle )
{
int subaddress;

    printf("%02d: InUse=%s CardType=%s CardAddress=0x%05X CardHandle=%d\n",handle,STR_YesNo(SENSORAY_HandleList[handle].InUse),SENSORAY_CardTypeText[SENSORAY_HandleList[handle].CardType],SENSORAY_HandleList[handle].CardAddress,SENSORAY_HandleList[handle].CardHandle);
    for( subaddress=0; (subaddress < SENSORAY_SUBADDRESS_MAX); subaddress++ )
    {
        printf("SubAddress[%d]: InUse=%s\n",subaddress,STR_YesNo(SENSORAY_HandleList[handle].SubAddress[subaddress].InUse));
    }
}

/******************************************************************************/

void SENSORAY_HandlePrint( void )
{
int handle;

    for( handle=0; (handle < SENSORAY_HANDLE_MAX); handle++ )
    {
        SENSORAY_HandlePrint(handle);
    }
}

/******************************************************************************/

SENSORAY *SENSORAY_ObjectList[SENSORAY_OBJECT_MAX];
void    (*SENSORAY_ObjectISR[])( void ) = { SENSORAY_ISR_Object0,SENSORAY_ISR_Object1,SENSORAY_ISR_Object2,SENSORAY_ISR_Object3,SENSORAY_ISR_Object4,SENSORAY_ISR_Object5,SENSORAY_ISR_Object6,SENSORAY_ISR_Object7 };

/******************************************************************************/

void SENSORAY_ObjectInit( void )
{
static BOOL init=FALSE;
int object;

    if( !init )
    {
        for( object=0; (object < SENSORAY_OBJECT_MAX); object++ )
        {
            SENSORAY_ObjectList[object] = NULL;
        }

        init = TRUE;
    }
}

/******************************************************************************/

int SENSORAY_ObjectFind( SENSORAY *SX26 )
{
int i,object;

    SENSORAY_ObjectInit();

    for( object=SENSORAY_OBJECT_INVALID,i=0; (i < SENSORAY_OBJECT_MAX); i++ )
    {
        if( SENSORAY_ObjectList[i] == SX26 )
        {
            object = i;
            break;
        }
    }

    return(object);
}

/******************************************************************************/

int SENSORAY_ObjectOpen( SENSORAY *SX26 )
{
int object=0;

    SENSORAY_ObjectInit();

    if( (object=SENSORAY_ObjectFind(NULL)) == SENSORAY_OBJECT_INVALID )
    {
        SENSORAY_errorf("SENSORAY_ObjectOpen() No free object handles.\n");
    }
    else
    {
        SENSORAY_ObjectList[object] = SX26;
    }

    return(object);
}

/******************************************************************************/

void SENSORAY_ObjectClose( SENSORAY *SX26 )
{
int object;

    SENSORAY_ObjectInit();

    if( (object=SENSORAY_ObjectFind(SX26)) == SENSORAY_OBJECT_INVALID )
    {
        SENSORAY_errorf("SENSORAY_ObjectClose() Cannot find object handle.\n");
    }
    else
    {
        SENSORAY_ObjectList[object] = NULL;
    }
}

/******************************************************************************/

SENSORAY *SENSORAY_ObjectSX26( int object )
{
SENSORAY *SX26;

    SX26 = SENSORAY_ObjectList[object];

    return(SX26);
}

/******************************************************************************/

BOOL SENSORAY_Error( int handle, char *text, int &rc )
{
BOOL ok=TRUE;
STRING error="";

    if( !SENSORAY_HandleValid(handle) )
    {
        SENSORAY_errorf("SENSORAY_Error(handle=%d,text=%s) Invalid handle.\n",handle,text);
        return(FALSE);
    }

    switch( SENSORAY_HandleList[handle].CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            ok = ((rc=S626_GetErrors(SENSORAY_HandleList[handle].CardHandle)) == 0);
            if( !ok )
            {
                strncpy(error,STR_TextFlag(S626_ErrorText,rc),STRLEN);
            }
            break;


    }

    if( (ok && (SENSORAY_debugf != NULL)) || (!ok && (SENSORAY_errorf != NULL)) )
    {
        STR_printf(ok,SENSORAY_debugf,SENSORAY_errorf,"%s %s.\n",text,STR_OkFailed(ok));
    }

    if( !ok )
    {
        SENSORAY_errorf("rc=0x%04X %s\n",rc,error);
    }

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY_Error( int handle, char *text )
{
BOOL ok;
int rc;

    ok = SENSORAY_Error(handle,text,rc);

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL ok=TRUE;

    SENSORAY_PRN_messgf = messgf;      // General API message print function.
    SENSORAY_PRN_errorf = errorf;      // API error message print function.
    SENSORAY_PRN_debugf = debugf;      // Debug information print function.

    // Start the API once...
    if( SENSORAY_API_started )
    {
        return(TRUE);
    }

    // Initialize S626 library.
    S626_DLLOpen();

    // Initialize S826 library and get number of S826 boards installed in system.


    // Set API started flag...
    SENSORAY_API_started = TRUE;

    // Initialize handle list...
    SENSORAY_HandleInit();

    if( ok )
    {
        ATEXIT_API(SENSORAY_API_stop);         // Install stop function.
        MODULE_start(SENSORAY_PRN_messgf);     // Register module.
    }
    else
    {
        SENSORAY_errorf("SENSORAY_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void SENSORAY_API_stop( void )
{
int rc;

    // Make sure API was started in the first place...
    if( !SENSORAY_API_started )
    {
         return;
    }

    // Close all SENSORAY cards...
    SENSORAY_HandleClose();

    // Terminate session with S626 library.
    S626_DLLClose();

    // Terminate session with S826 library.


    // Clear API started flag...
    SENSORAY_API_started = FALSE;

    // Register module stop...
    MODULE_stop();
}

/******************************************************************************/

BOOL SENSORAY_API_check( void )
{
BOOL ok=TRUE;

    // Check if API started. Start it if required..
    if( !SENSORAY_API_started )
    {
        ok = SENSORAY_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        SENSORAY_debugf("SENSORAY_API_check() Start %s.\n",STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

SENSORAY::SENSORAY( DWORD address, BYTE subaddress, char *name )
{
    // Initialize variables...
    Init(address,subaddress,name);
}

/******************************************************************************/

SENSORAY::SENSORAY( DWORD address, BYTE subaddress )
{
    // Initialize variables...
    Init(address,subaddress,NULL);
}

/******************************************************************************/

SENSORAY::SENSORAY( DWORD address, char *name )
{
    // Initialize variables...
    Init(address,0,name);
}

/******************************************************************************/

SENSORAY::SENSORAY( DWORD address )
{
    // Initialize variables...
    Init(address,0,NULL);
}

/******************************************************************************/

SENSORAY::SENSORAY( void )
{
    // Initialize variables...
    Init(0,0,NULL);
}

/******************************************************************************/

SENSORAY::~SENSORAY( void )
{
    // Close card if it's currently open...
    if( Opened() )
    {
        Close();
    }
}

/******************************************************************************/

char *SENSORAY::Name( void )
{
    if( STR_null(NameString) )
    {
        strncat(NameString,STR_stringf("%s[%d]",SENSORAY_CardTypeText[CardType],Handle),STRLEN);

        switch( CardType )
        {
            case SENSORAY_CARDTYPE_S626 :
                strncat(NameString,STR_stringf("-0x%05lX",CardAddress),STRLEN);
                break;


        }

        if( SubAddress != 0 )
        {
            strncat(NameString,STR_stringf("-%d",SubAddress),STRLEN);
        }

        if( !STR_null(ObjectName,STRLEN) )
        {
            strncat(NameString,STR_stringf("-%s",ObjectName),STRLEN);
        }
    }

    return(NameString);
}

/******************************************************************************/

BOOL SENSORAY::Error( char *text, int &rc )
{
BOOL ok;

    ok = SENSORAY_Error(Handle,text,rc);

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::Error( char *text )
{
BOOL ok;

    ok = SENSORAY_Error(Handle,text);

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::CheckOpen( char *text )
{
BOOL flag=TRUE;

    // Check open flag to make sure card had been opened...
    if( !Opened() )
    {
        SENSORAY_errorf("%s %s Not Open.\n",Name(),text);
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL SENSORAY::Opened( void )
{
BOOL flag=TRUE;

    if( !SENSORAY_API_check() )
    {
        flag = FALSE;
    }
    else
    if( Handle == SENSORAY_HANDLE_INVALID )
    {
        flag = FALSE;
    }
    else
    if( !OpenFlag )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

void SENSORAY::Init( DWORD address, BYTE subaddress, char *name )
{
int channel;

    // Make sure API started...
    if( !SENSORAY_API_check() )
    {
        return;
    }

    SENSORAY_debugf("SENSORAY::Init(address=0x%05X,subaddress=0x%02X)\n",address,subaddress);

    // Set other variables...
    CardAddress = address;
    SubAddress = subaddress;

    OpenFlag = FALSE;

    ENC_IndexCallback = NULL;

    for( channel=0; (channel < SENSORAY_ENC_CHANNELS); channel++ )
    {
        ENC_OpenFlag[channel] = FALSE;
    }

    memset(ObjectName,0,STRLEN);
    if( name != NULL )
    {
        strncpy(ObjectName,name,STRLEN);
    }

    memset(NameString,0,STRLEN);
}

/******************************************************************************/

BOOL SENSORAY::Open( void )
{
BOOL ok;
int divider,offset;

    // Make sure API started...
    if( !SENSORAY_API_check() )
    {
        return(FALSE);
    }

    // Check if already opened...
    if( Opened() )
    {
        return(TRUE);
    }

    if( (ObjectHandle=SENSORAY_ObjectOpen(this)) == SENSORAY_OBJECT_INVALID )
    {
        SENSORAY_errorf("SENSORAY::Open(Address=0x%05X,SubAddress=0x%02X) No free object handles.\n",CardAddress,SubAddress);
        return(FALSE);
    }

    // Physically open card and get a handle...
    if( (Handle=SENSORAY_HandleOpen(CardAddress,SubAddress,SENSORAY_ObjectISR[ObjectHandle])) == SENSORAY_HANDLE_INVALID )
    {
        SENSORAY_errorf("SENSORAY::Open(Address=0x%05X,SubAddress=0x%02X) Cannot open handle.\n",CardAddress,SubAddress);
        return(FALSE);
    }

    CardHandle = SENSORAY_HandleList[Handle].CardHandle;
    CardType = SENSORAY_HandleList[Handle].CardType;

    SENSORAY_debugf("SENSORAY::Open(Address=0x%05X,SubAddress=0x%02X) %s Handle=%d.\n",CardAddress,SubAddress,SENSORAY_CardTypeText[CardType],Handle);

    // SubAddress
    if( (SubAddress > 0) && (SubAddress < SENSORAY_SUBADDRESS_MAX) )
    {
        divider = SENSORAY_SUBADDRESS_MAX-1;
        offset = (SubAddress-1);
    }
    else
    {
        SubAddress = 0;
        divider = 1;
        offset = 0;
    }

    switch( SENSORAY_HandleList[Handle].CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            DAC_ChannelMax = S626_DAC_CHANNELS / divider;
            ADC_ChannelMax = S626_ADC_CHANNELS / divider;
            DIO_ChannelMax = S626_DIO_CHANNELS / divider;
            ENC_ChannelMax = S626_ENC_CHANNELS / divider;

            DAC_ResolutionBits = S626_DAC_BITS;
            ADC_ResolutionBits = S626_ADC_BITS;
            ENC_ResolutionBits = S626_ENC_BITS;
            break;


    }

    DAC_ChannelOffset = DAC_ChannelMax * offset;
    ADC_ChannelOffset = ADC_ChannelMax * offset;
    DIO_ChannelOffset = DIO_ChannelMax * offset;
    ENC_ChannelOffset = ENC_ChannelMax * offset;

    // Encoder pre-load value is middle of range given by counter resolution.
    ENC_PreLoadValue = (1 << (ENC_ResolutionBits-1));

    // Set open flag...
    OpenFlag = TRUE;

    // Initialize variables for specific sub-systems...
    DIO_Init();
    DAC_Init();
    ADC_Init();

    // Set card outputs in default states...
    DefaultOutput();

    SENSORAY_debugf("%s\n",Name());
    SENSORAY_debugf("ADC: Channels=%d Resolution=%d(bits)\n",ADC_ChannelMax,ADC_ResolutionBits);
    SENSORAY_debugf("DAC: Channels=%d Resolution=%d(bits)\n",DAC_ChannelMax,DAC_ResolutionBits);
    SENSORAY_debugf("ENC: Channels=%d Resolution=%d(bits) PreLoad=%lu\n",ENC_ChannelMax,ENC_ResolutionBits,ENC_PreLoadValue);
    SENSORAY_debugf("DIO: Channels=%d\n",DAC_ChannelMax);

    return(TRUE);
}

/******************************************************************************/

void SENSORAY::Close( void )
{
int channel;

    // Make sure API started...
    if( !SENSORAY_API_check() )
    {
        return;
    }

    // Make sure card is open...
    if( !Opened() )
    {
        return;
    }

    // Set card outputs in default states...
    DefaultOutput();

    for( channel=0; (channel < SENSORAY_ENC_CHANNELS); channel++ )
    {
        EncoderClose(channel);
    }

    // Physically close card release clear handles...
    SENSORAY_HandleClose(CardAddress,SubAddress);
    SENSORAY_ObjectClose(this);

    OpenFlag = FALSE;
}

/******************************************************************************/

void SENSORAY::DefaultOutput( void )
{
int DAC,DIO;

    // Set analog outputs to zero...
    for( DAC=0; (DAC < DAC_ChannelMax); DAC++ )
    {
        DAC_Write(DAC,0.0);
    }

    // Set digital outputs to zero...
    for( DIO=0; (DIO < DIO_ChannelMax); DIO++ )
    {
        DO_Set(DIO,0);
    }
}

/******************************************************************************/

int SENSORAY::GetCardType( void )
{
int value=SENSORAY_CARDTYPE_NONE;

    // Make sure card is open...
    if( CheckOpen("GetCardType") )
    {
        value = CardType;
    }

    return(value);
}

/******************************************************************************/

int SENSORAY::GetCardHandle( void )
{
int value=SENSORAY_HANDLE_INVALID;

    // Make sure card is open...
    if( CheckOpen("GetCardHandle") )
    {
        value = CardHandle;
    }

    return(value);
}

/******************************************************************************/

WORD SENSORAY::RegRead( WORD addr )
{
WORD value=0;

    // Make sure card is open...
    if( !CheckOpen(STR_stringf("RegRead(addr=0x%04X)",addr)) )
    {
        return(0);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    value = S626_RegRead(CardHandle,addr);

    CriticalSectionExit();   // Terminate exclusive access.

    return(value);
}

/******************************************************************************/

void SENSORAY::RegWrite( WORD addr, WORD value )
{
    // Make sure card is open...
    if( !CheckOpen(STR_stringf("RegWrite(addr=0x%04X,value=0x%04X",addr,value)) )
    {
        return;
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    S626_RegWrite(CardHandle,addr,value);

    CriticalSectionExit();   // Terminate exclusive access.
}

/******************************************************************************/

WORD SENSORAY::DIOWriteBankGet( WORD group )
{
WORD states=0;

    // Make sure card is open...
    if( !CheckOpen("DIOWriteBankGet()") )
    {
        return(0);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    states = S626_DIOWriteBankGet(CardHandle,group);

    CriticalSectionExit();   // Terminate exclusive access.

    return(states);
}

/******************************************************************************/

void SENSORAY::DIOWriteBankSet( WORD group, WORD states )
{
    // Make sure card is open...
    if( !CheckOpen("DIOWriteBankSet()") )
    {
        return;
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    S626_DIOWriteBankSet(CardHandle,group,states);

    CriticalSectionExit();   // Terminate exclusive access.
}

/******************************************************************************/

WORD SENSORAY::DIOReadBank( WORD group )
{
WORD states=0;

    // Make sure card is open...
    if( !CheckOpen("DIOReadBank()") )
    {
        return(0);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    states = S626_DIOReadBank(CardHandle,group);

    CriticalSectionExit();   // Terminate exclusive access.

    return(states);
}

/******************************************************************************/

void SENSORAY::DIO_Init( void  )
{
int channel;

    for( channel=0; (channel < DIO_ChannelMax); channel++ )
    {
        DO_Set(channel,0);
    }
}

/******************************************************************************/

BOOL SENSORAY::S626_DIO_Channel( int &channel, WORD &group, WORD &mask )
{
WORD bit;

    if( !DIO_ChannelIndex(channel) )
    {
        return(FALSE);
    }

    group = channel / 16;
    bit = channel % 16;
    mask = 1 << bit;

    SENSORAY_debugf("SENSORAY::S626_DIO_Channel(channel=%d) group=%d mask=0x%06X\n",channel,group,mask);

    return(TRUE);
}

/******************************************************************************/


/******************************************************************************/

void SENSORAY::S626_DO_Set( int channel, BOOL state )
{
WORD group,mask,value,next;

    if( !S626_DIO_Channel(channel,group,mask) )
    {
        return;
    }

    value = DIOWriteBankGet(group);

    if( state )
    {
        next = value | mask;
    }
    else
    {
        next = value & ~mask;
    }

    if( value != next )
    {
        DIOWriteBankSet(group,next);
    }

    SENSORAY_debugf("SENSORAY::S626_DO_Set(channel=%d,state=%d) group=%d mask=0x%06X value=%d next=%d\n",channel,state,group,mask,value,next);
}

/******************************************************************************/

/******************************************************************************/

void SENSORAY::DO_Set( int channel, BOOL state )
{
    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            S626_DO_Set(channel,state);
            break;


    }
}

/******************************************************************************/

BOOL SENSORAY::S626_DIO_Get( int channel, int dio )
{
WORD group,mask,value=0;
BOOL state;

    if( !S626_DIO_Channel(channel,group,mask) )
    {
        return(FALSE);
    }

    switch( dio )
    {
        case SENSORAY_DIO_OUTPUT :
           value = DIOWriteBankGet(group);
           break;

        case SENSORAY_DIO_INPUT :
           value = DIOReadBank(group);
           break;
    }

    state = ((value & mask) != 0);

    return(state);
}

/******************************************************************************/


/******************************************************************************/

BOOL SENSORAY::DIO_Get( int channel, int dio )
{
BOOL state;

    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            state = S626_DIO_Get(channel,dio);
            break;


    }

    return(state);
}

/******************************************************************************/

BOOL SENSORAY::DO_Get( int channel )
{
BOOL state;

    state = DIO_Get(channel,SENSORAY_DIO_OUTPUT);

    return(state);
}

/******************************************************************************/

BOOL SENSORAY::DI_Get( int channel )
{
BOOL state;

    // The digital-out state of the channel must be zero to allow digital input.
    /*if( DO_Get(channel) )
    {
        DO_Set(channel,0);
    }*/

    state = DIO_Get(channel,SENSORAY_DIO_INPUT);

    return(state);
}

/******************************************************************************/



/******************************************************************************/


/******************************************************************************/

BOOL SENSORAY::ResetADC( BYTE list[], BYTE gain[], int count )
{
BOOL ok=TRUE;

    // Make sure card is open...
    if( !CheckOpen("ResetADC") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            S626_ResetADC(CardHandle,list);
            break;


    }

    CriticalSectionExit();   // Terminate exclusive access.

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::ReadADC( WORD data[] )
{
BOOL ok=TRUE;
int rc;

    // Make sure card is open...
    if( !CheckOpen("ReadADC()") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            S626_ReadADC(CardHandle,data);

            break;


    }

    CriticalSectionExit();   // Terminate exclusive access.

    return(ok);
}

/******************************************************************************/

void SENSORAY::WriteDAC( int channel, int units )
{
int rc;
BOOL ok;
SHORT S626_SetPoint;

double a,b;

    // Make sure card is open...
    if( !CheckOpen("WriteDAC()") )
    {
        return;
    }

    if( !DAC_ChannelIndex(channel) )
    {
        return;
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            S626_SetPoint = (SHORT)units;
            S626_WriteDAC(CardHandle,channel,S626_SetPoint);
            break;


    }

    CriticalSectionExit();   // Terminate exclusive access.
}

/******************************************************************************/

void SENSORAY::DAC_Init( void )
{
int rc,channel;
BOOL ok;
uint r_range,r_setpoint;

    for( channel=0; (channel < DAC_ChannelMax); channel++ )
    {
        switch( CardType )
        {
            case SENSORAY_CARDTYPE_S626 :
                DAC_UnitsPerVolt[channel] = (double)S626_DAC_URANGE / S626_DAC_VRANGE;
                DAC_UnitsZeroOffset[channel] = 0;
                break;


        }

        //SENSORAY_messgf("%s: DAC[%d] UnitsPerVolt=%.1lf ZeroOffset=%d(0x%04X)\n",Name(),channel,DAC_UnitsPerVolt[channel],DAC_UnitsZeroOffset[channel],DAC_UnitsZeroOffset[channel]);
    }
}

/******************************************************************************/

int SENSORAY::DAC_Units( int channel, double volts )
{
int units=0;

    if( !UndefinedDouble(volts) )
    {
        units = (int)(DAC_UnitsPerVolt[channel] * volts);
    }

    units += DAC_UnitsZeroOffset[channel];

    return(units);
}

/******************************************************************************/

void SENSORAY::DAC_Write( int channel, double volts )
{
uint units;

    units = DAC_Units(channel,volts);
    WriteDAC(channel,units);
}

/******************************************************************************/

void SENSORAY::ADC_Init( void )
{
int c;

    for( c=0; (c < SENSORAY_ADC_CHANNELS); c++ )
    {
        ADC_Channels[c] = SENSORAY_ADC_EOL;
        ADC_Gains[c] = 0;
        ADC_VoltsPerUnit[c] = 0.0;
    }

    ADC_Count = 0;
    ADC_Started = FALSE;
}

/******************************************************************************/

void SENSORAY::ADC_Stop( void )
{
int rc;
BOOL ok;

    // Close ADC session if one has been started.
    if( ADC_Started )
    {
        switch( CardType )
        {
            case SENSORAY_CARDTYPE_S626 :
                // No explicit ADC stop required for S626.
                break;


        }

        ADC_Init();
    }
}

/******************************************************************************/

BOOL SENSORAY::ADC_Setup( int channels[], BYTE gains[], int count )
{
int c;
BYTE clist[SENSORAY_ADC_CHANNELS],glist[SENSORAY_ADC_CHANNELS];
BOOL ok;

    // Make sure card is open...
    if( !CheckOpen("ADC_Setup") )
    {
        return(FALSE);
    }

    // If ADC session already started, stop it before starting another.
    if( ADC_Started )
    {
        ADC_Stop();
    }

    // Create list of channels in Sensoray626 format...
    for( ok=TRUE,c=0; ((c < ADC_ChannelMax) && (c < count) && ok); c++ )
    {
        if( !ADC_ChannelIndex(channels[c]) )
        {
            ok = FALSE;
            continue;
        }

        ADC_Channels[c] = channels[c];
        ADC_Gains[c] = gains[c];
        ADC_VoltsPerUnit[c] = (2.0*SENSORAY_ADC_GainVolts[gains[c]]) / (double)(1 << ADC_ResolutionBits);

        switch( CardType )
        {
            case SENSORAY_CARDTYPE_S626 :
                ADC_CardGains[c] = S626_GainList[gains[c]];
                clist[c] = (ADC_CHANMASK & channels[c]) | ADC_CardGains[c];
                glist[c] = ADC_CardGains[c];
                break;


        }

        SENSORAY_debugf("%s: ADC_Setup(...) c=%d channel=%d MVoltsPerUnit=%.3lf\n",Name(),c,ADC_Channels[c],1000.0 * ADC_VoltsPerUnit[c]);
    }

    // Check channels specified...
    if( (c == 0) || !ok )
    {
        SENSORAY_errorf("%s: ADC_Setup(...) Invalid channels (c=%d, ok=%d).\n",Name(),c,ok);
        return(FALSE);
    }

    // Set end of list flag...
    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            clist[c-1] |= ADC_EOPL;
            break;


    }

    // Set number of channels...
    ADC_Count = c;

    // Set the ADC channel list...
    if( (ok=ResetADC(clist,glist,c)) )
    {
        ADC_Started = TRUE;
    }

    STR_printf(ok,SENSORAY_debugf,SENSORAY_errorf,"%s: ADC_Setup(...) Channels=%d %s\n",Name(),ADC_Count,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::ADC_Setup( int channels[], BYTE gains[] )
{
BOOL ok;
int count;

    for( count=0; ((count < SENSORAY_ADC_CHANNELS) && (channels[count] != SENSORAY_ADC_EOL)); count++ );
    ok = ADC_Setup(channels,gains,count);

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::ADC_Setup( int channels[], BYTE gain, int count )
{
BOOL ok;
int c;
BYTE gains[SENSORAY_ADC_CHANNELS];

    // Create a list of gains...
    for( c=0; (c < SENSORAY_ADC_CHANNELS); c++ )
    {
        gains[c] = gain;
    }

    // Call base function...
    ok = ADC_Setup(channels,gains,count);

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::ADC_Setup( int channels[], BYTE gain )
{
BOOL ok;
int c;
BYTE gains[SENSORAY_ADC_CHANNELS];

    // Create a list of gains...
    for( c=0; (c < SENSORAY_ADC_CHANNELS); c++ )
    {
        gains[c] = gain;
    }

    // Call base function...
    ok = ADC_Setup(channels,gains);

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::ADC_Setup5V( int channels[], int count )
{
BOOL ok;

    ok = ADC_Setup(channels,(BYTE)SENSORAY_ADC_GAIN_5VOLT,count);

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::ADC_Setup5V( int channels[] )
{
BOOL ok;

    ok = ADC_Setup(channels,(BYTE)SENSORAY_ADC_GAIN_5VOLT);

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::ADC_Setup10V( int channels[], int count )
{
BOOL ok;

    ok = ADC_Setup(channels,(BYTE)SENSORAY_ADC_GAIN_10VOLT,count);

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::ADC_Setup10V( int channels[] )
{
BOOL ok;

    ok = ADC_Setup(channels,(BYTE)SENSORAY_ADC_GAIN_10VOLT);

    return(ok);
}

/******************************************************************************/

int SENSORAY::ADC_SignedInteger( WORD raw )
{
int value;

    value = (int)raw;

    if( raw & 0x8000 )
    {
        value = value - 0x10000;
    }

    return(value);
}

/******************************************************************************/

double SENSORAY::ADC_SignedDouble( WORD raw )
{
double value;

    value = (double)ADC_SignedInteger(raw);

    return(value);
}

/******************************************************************************/

void SENSORAY::ADC_SampleUnits( int units[] )
{
BOOL ok;
WORD raw[SENSORAY_ADC_CHANNELS];
int c;

    // Make sure card is open...
    if( !CheckOpen("ADC_SampleUnits()") )
    {
        return;
    }

    // Sample the analog channels...
    ok = ReadADC(raw);

    // Convert raw data to voltages...
    for( c=0; (c < ADC_Count); c++ )
    {
        units[c] = ADC_SignedInteger(raw[c]);
    }
}

/******************************************************************************/

void SENSORAY::ADC_SampleVolts( double volts[] )
{
BOOL ok;
WORD raw[SENSORAY_ADC_CHANNELS];
int c;

    // Make sure card is open...
    if( !CheckOpen("ADC_SampleVolts()") )
    {
        return;
    }

    // Sample the analog channels...
    ok = ReadADC(raw);

    // Convert raw data to voltages...
    for( c=0; (c < ADC_Count); c++ )
    {
        volts[c] = ADC_SignedDouble(raw[c]) * ADC_VoltsPerUnit[c];
    }
}

/******************************************************************************/

BOOL SENSORAY::CounterModeSet( WORD chan, WORD options )
{
BOOL ok;

    // Make sure card is open...
    if( !CheckOpen("CounterModeSet()") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    // Call appropriate Sensoray626 function...
    S626_CounterModeSet(CardHandle,chan,options);

    CriticalSectionExit();   // Terminate exclusive access.

    // Check for errors...
    ok = Error(STR_stringf("S626_CounterModeSet(chan=%d,options=0x%04X)",chan,options));

    return(ok);
}

/******************************************************************************/

WORD SENSORAY::CounterModeGet( WORD chan )
{
WORD value;

    // Make sure card is open...
    if( !CheckOpen("CounterModeGet") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    // Call appropriate Sensoray626 function...
    value = S626_CounterModeGet(CardHandle,chan);

    CriticalSectionExit();   // Terminate exclusive access.

    return(value);
}

/******************************************************************************/

BOOL SENSORAY::CounterEnableSet( WORD chan, WORD cond )
{
BOOL ok;

    // Make sure card is open...
    if( !CheckOpen("CounterEnableSet") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    // Call appropriate Sensoray626 function...
    S626_CounterEnableSet(CardHandle,chan,cond);

    CriticalSectionExit();   // Terminate exclusive access.

    // Check for errors...
    ok = Error(STR_stringf("S626_CounterEnableSet(chan=%d,cond=%d)",chan,cond));

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::CounterPreload( WORD chan, DWORD value )
{
BOOL ok;

    // Make sure card is open...
    if( !CheckOpen("CounterPreload") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    // Call appropriate Sensoray626 function...
    S626_CounterPreload(CardHandle,chan,value);

    CriticalSectionExit();   // Terminate exclusive access.

    // Check for errors...
    ok = Error("S626_CounterPreload");

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::CounterLoadTrigSet( WORD chan, WORD events )
{
BOOL ok;

    // Make sure card is open...
    if( !CheckOpen("CounterLoadTrigSet") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    // Call appropriate Sensoray626 function...
    S626_CounterLoadTrigSet(CardHandle,chan,events);

    CriticalSectionExit();   // Terminate exclusive access.

    // Check for errors...
    ok = Error(STR_stringf("S626_CounterLoadTrigSet(chan=%d,events=%d)",chan,events));

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::CounterLatchSourceSet( WORD chan, WORD events )
{
BOOL ok;

    // Make sure card is open...
    if( !CheckOpen("CounterLatchSourceSet") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    // Call appropriate Sensoray626 function...
    S626_CounterLatchSourceSet(CardHandle,chan,events);

    CriticalSectionExit();   // Terminate exclusive access.

    // Check for errors...
    ok = Error(STR_stringf("S626_CounterLatchSourceSet(chan=%d,events=%d)",chan,events));

    return(ok);
}

/******************************************************************************/

BOOL SENSORAY::CounterIntSourceSet( WORD chan, WORD events )
{
BOOL ok;

    // Make sure card is open...
    if( !CheckOpen("CounterIntSourceSet") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    // Call appropriate Sensoray626 function...
    S626_CounterIntSourceSet(CardHandle,chan,events);

    CriticalSectionExit();   // Terminate exclusive access.

    // Check for errors...
    ok = Error(STR_stringf("S626_CounterIntSourceSet(chan=%d,events=%d)",chan,events));

    return(ok);
}

/******************************************************************************/

DWORD SENSORAY::CounterReadLatch( WORD counter )
{
DWORD value;

    // Make sure card is open...
    if( !CheckOpen("CounterReadLatch") )
    {
        return(0);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    // Call appropriate Sensoray626 function...
    value = S626_CounterReadLatch(CardHandle,counter);

    CriticalSectionExit();   // Terminate exclusive access.

    return(value);
}

/******************************************************************************/

BOOL SENSORAY::CounterSoftIndex( WORD chan )
{
BOOL ok;

    // Make sure card is open...
    if( !CheckOpen("CounterSoftIndex") )
    {
        return(FALSE);
    }

    CriticalSectionEnter();  // Wait for exclusive access to the card.

    // Call appropriate Sensoray626 function...
    S626_CounterSoftIndex(CardHandle,chan);

    CriticalSectionExit();   // Terminate exclusive access.

    // Check for errors...
    ok = Error(STR_stringf("S626_CounterSoftIndex(chan=%d)",chan));

    return(ok);
}

/******************************************************************************/

int SENSORAY::ChannelIndex( int type, int channel )
{
int index=SENSORAY_CHANNEL_INVALID;
int max=0,offset=0;

    switch( type )
    {
        case SENSORAY_DAC :
           max = DAC_ChannelMax;
           offset = DAC_ChannelOffset;
           break;

        case SENSORAY_ADC :
           max = ADC_ChannelMax;
           offset = ADC_ChannelOffset;
           break;

        case SENSORAY_DIO :
           max = DIO_ChannelMax;
           offset = DIO_ChannelOffset;
           break;

        case SENSORAY_ENC :
           max = ENC_ChannelMax;
           offset = ENC_ChannelOffset;
           break;
    }

    if( (channel < 0) || (channel >= max) )
    {
        SENSORAY_errorf("%s: ChannelIndex(type=%s,channel=%d) Invalid channel.\n",Name(),SENSORAY_ChannelType[type],channel);
    }
    else
    {
        index = offset + channel;
    }

    //printf("SENSORAY::ChannelIndex(type=%s,channel=%d) index=%d max=%d offset=%d\n",SENSORAY_ChannelType[type],channel,index,max,offset);
    return(index);
}

/******************************************************************************/

BOOL SENSORAY::DAC_ChannelValid( int channel )
{
BOOL flag;

    flag = ((channel >= 0) && (channel < DAC_ChannelMax));

    return(flag);
}

/******************************************************************************/

BOOL SENSORAY::ADC_ChannelValid( int channel )
{
BOOL flag;

    flag = ((channel >= 0) && (channel < ADC_ChannelMax));

    return(flag);
}

/******************************************************************************/

BOOL SENSORAY::DIO_ChannelValid( int channel )
{
BOOL flag;

    flag = ((channel >= 0) && (channel < DIO_ChannelMax));

    return(flag);
}

/******************************************************************************/

BOOL SENSORAY::ENC_ChannelValid( int channel )
{
BOOL flag;

    flag = ((channel >= 0) && (channel < ENC_ChannelMax));

    return(flag);
}

/******************************************************************************/

BOOL SENSORAY::DAC_ChannelIndex( int &channel )
{
int index;
BOOL flag=FALSE;

    if( (index=ChannelIndex(SENSORAY_DAC,channel)) != SENSORAY_CHANNEL_INVALID )
    {
        channel = index;
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

BOOL SENSORAY::ADC_ChannelIndex( int &channel )
{
int index;
BOOL flag=FALSE;

    if( (index=ChannelIndex(SENSORAY_ADC,channel)) != SENSORAY_CHANNEL_INVALID )
    {
        channel = index;
        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

BOOL SENSORAY::DIO_ChannelIndex( int &channel )
{
int index;
BOOL flag=FALSE;

    if( (index=ChannelIndex(SENSORAY_DIO,channel)) != SENSORAY_CHANNEL_INVALID )
    {
        channel = index;
        flag = TRUE;
    }


    return(flag);
}

/******************************************************************************/

BOOL SENSORAY::ENC_ChannelIndex( int &channel, int &counter )
{
int index;
BOOL flag=FALSE;

    if( (index=ChannelIndex(SENSORAY_ENC,channel)) != SENSORAY_CHANNEL_INVALID )
    {
        channel = index;
        flag = TRUE;

        switch( CardType )
        {
            case SENSORAY_CARDTYPE_S626 :
                counter = S626_EncoderCounter[channel];
                break;


        }
    }

    return(flag);
}

/******************************************************************************/

BOOL SENSORAY::S626_EncoderOpen( int counter, int multiplier )
{
BOOL ok;
WORD ModeSet,ModeGet,clkmult=0;
int step;
#define SENSORAY_S626_ENCODEROPEN_MODESET    0
#define SENSORAY_S626_ENCODEROPEN_MODEGET    1
#define SENSORAY_S626_ENCODEROPEN_TRIGGER    2
#define SENSORAY_S626_ENCODEROPEN_INTSOURCE  3
#define SENSORAY_S626_ENCODEROPEN_SOURCE     4

    switch( multiplier )
    {
        case 1 :
           clkmult = CLKMULT_1X;
           break;

        case 2 :
           clkmult = CLKMULT_2X;
           break;

        case 4 :
           clkmult = CLKMULT_4X;
           break;
    }

    ModeSet = (LOADSRC_INDX   << BF_LOADSRC) |   //
              (INDXSRC_HARD   << BF_INDXSRC) |   //
              (INDXPOL_POS    << BF_INDXPOL) |   //
              (CLKSRC_COUNTER << BF_CLKSRC ) |   //
              (CLKPOL_POS     << BF_CLKPOL ) |   //
              (clkmult        << BF_CLKMULT) |   //
              (CLKENAB_ALWAYS << BF_CLKENAB);    //

    for( ok=TRUE,step=SENSORAY_S626_ENCODEROPEN_MODESET; ((step <= SENSORAY_S626_ENCODEROPEN_SOURCE) && ok); step++ )
    {
        switch( step )
        {
            case SENSORAY_S626_ENCODEROPEN_MODESET :
               ok = CounterModeSet(counter,ModeSet);
               break;

            case SENSORAY_S626_ENCODEROPEN_MODEGET :
               ModeGet = CounterModeGet(counter);
               SENSORAY_debugf("%s EncoderOpen(counter=%d) ModeSet=0x%08X ModeSet=0x%08X\n",Name(),counter,ModeSet,ModeGet);
               break;

            case SENSORAY_S626_ENCODEROPEN_TRIGGER :
               ok = CounterLoadTrigSet(counter,LOADSRC_NONE);
               break;

            case SENSORAY_S626_ENCODEROPEN_INTSOURCE :
               ok = CounterIntSourceSet(counter,INTSRC_INDX);
               break;

            case SENSORAY_S626_ENCODEROPEN_SOURCE :
               ok = CounterLatchSourceSet(counter,LATCHSRC_AB_READ);
               break;
        }
    }

    return(ok);
}

/******************************************************************************/



/******************************************************************************/

BOOL SENSORAY::EncoderOpen( int channel, int multiplier )
{
int counter;
BOOL ok=TRUE;

    if( ENC_OpenFlag[channel] )
    {
        SENSORAY_errorf("%s EncoderOpen(channel=%d,...) Already open.\n",Name(),channel);
        return(TRUE);
    }

    if( !ENC_ChannelIndex(channel,counter) )
    {
        return(FALSE);
    }

    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            S626_EncoderOpen(counter,multiplier);
            break;


    }

    if( ok )
    {
        ENC_OpenFlag[channel] = TRUE;
    }

    STR_printf(ok,SENSORAY_debugf,SENSORAY_errorf,"%s EncoderOpen(channel=%d,multiplier=%d) %s.\n",Name(),counter,multiplier,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

void SENSORAY::S626_EncoderClose( int counter )
{
}

/******************************************************************************/



/******************************************************************************/

void SENSORAY::EncoderClose( int channel )
{
int counter;

    if( !ENC_OpenFlag[channel] )
    {
        return;
    }

    if( !ENC_ChannelIndex(channel,counter) )
    {
        return;
    }

    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            S626_EncoderClose(counter);
            break;


    }

    ENC_OpenFlag[channel] = FALSE;
}

/******************************************************************************/

long SENSORAY::S626_EncoderCount( int counter )
{
long value=0;

    value = CounterReadLatch((WORD)counter) - ENC_PreLoadValue;

    return(value);
}

/******************************************************************************/


/******************************************************************************/



/******************************************************************************/


/******************************************************************************/

long SENSORAY::EncoderCount( int channel )
{
int counter;
long value=0;

    if( !ENC_ChannelIndex(channel,counter) )
    {
        return(0);
    }

    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            value = S626_EncoderCount(counter);
            break;


    }

    return(value);
}

/******************************************************************************/

void SENSORAY::S626_EncoderReset( int counter, long value )
{
    // Set the counter pre-load value.
    CounterPreload(counter,ENC_PreLoadValue + value);

    // Disable counter interrupts.
    CounterIntSourceSet(counter,INTSRC_NONE);

    // Enable pre-load on encoder index.
    CounterLoadTrigSet(counter,LOADSRC_INDX);

    // Trigger pre-load by software index.
    CounterSoftIndex(counter);

    // Disable pre-load on encoder index.
    CounterLoadTrigSet(counter,LOADSRC_NONE);

    // Enable counter interrupts.
    CounterIntSourceSet(counter,INTSRC_INDX);
}

/******************************************************************************/



/******************************************************************************/

void SENSORAY::EncoderReset( int channel, long value )
{
int counter;

    if( !ENC_ChannelIndex(channel,counter) )
    {
        return;
    }

    switch( CardType )
    {
        case SENSORAY_CARDTYPE_S626 :
            S626_EncoderReset(channel,value);
            break;


    }
}

/******************************************************************************/

void SENSORAY::EncoderReset( int channel )
{
    EncoderReset(channel,0L);
}

/******************************************************************************/

void SENSORAY::EncoderIndexSetCallback( void (*func)( int channel, long counts ) )
{
    ENC_IndexCallback = func;
}

/******************************************************************************/

void SENSORAY::EncoderIndexEvent( int channel, long counts )
{
    if( ENC_IndexCallback != NULL )
    {
      (*ENC_IndexCallback)(channel,counts);
    }
}

/******************************************************************************/

void SENSORAY_ISR_Object0( void )
{
    SENSORAY_ISR_Object(0);
}

/******************************************************************************/

void SENSORAY_ISR_Object1( void )
{
    SENSORAY_ISR_Object(1);
}

/******************************************************************************/

void SENSORAY_ISR_Object2( void )
{
    SENSORAY_ISR_Object(2);
}

/******************************************************************************/

void SENSORAY_ISR_Object3( void )
{
    SENSORAY_ISR_Object(3);
}

/******************************************************************************/

void SENSORAY_ISR_Object4( void )
{
    SENSORAY_ISR_Object(4);
}

/******************************************************************************/

void SENSORAY_ISR_Object5( void )
{
    SENSORAY_ISR_Object(5);
}

/******************************************************************************/

void SENSORAY_ISR_Object6( void )
{
    SENSORAY_ISR_Object(6);
}

/******************************************************************************/

void SENSORAY_ISR_Object7( void )
{
    SENSORAY_ISR_Object(7);
}

/******************************************************************************/

void SENSORAY_ISR_Object( int object )
{
SENSORAY *SX26;

    if( (SX26=SENSORAY_ObjectSX26(object)) != NULL )
    {
        SENSORAY_ISR(SX26);
    }
}

/******************************************************************************/

void SENSORAY_ISR( SENSORAY *SX26 )
{
WORD flags;
int counter,channel;
long counts;

    if( SX26->CardType != SENSORAY_CARDTYPE_S626 )
    {
        return;
    }

    flags = S626_CounterCapStatus(SX26->CardHandle);

    for( counter=0; (counter < SENSORAY_ENC_CHANNELS); counter++ )
    {
        if( (flags & S626_EncoderIndexMask[counter]) != 0 )
        {
            S626_CounterCapFlagsReset(SX26->CardHandle,S626_EncoderCounter[counter]);

            channel = counter; // Convert counter to channel.
            counts = SX26->S626_EncoderCount(counter);
            SX26->EncoderIndexEvent(channel,counts);
        }
    }

    // Enable board�s master interrupt.
    S626_InterruptEnable(SX26->CardHandle,true);
}

/******************************************************************************/

void SENSORAY::CriticalSectionEnter( void )
{
}

/******************************************************************************/

void SENSORAY::CriticalSectionExit( void )
{
}

/******************************************************************************/


/******************************************************************************/



/******************************************************************************/



/******************************************************************************/



/******************************************************************************/


/******************************************************************************/



/******************************************************************************/
