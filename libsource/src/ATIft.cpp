/******************************************************************************/
/*                                                                            */
/* MODULE  : ATIFT.cpp                                                        */
/*                                                                            */
/* PURPOSE : Interface functions for ATI F/T Sensor.                          */
/*                                                                            */
/* DATE    : 19/Apr/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V2.0  JNI 19/Apr/2000 - Taken from module "phantom.cpp" & re-developed.    */
/*                                                                            */
/* V2.1  JNI 05/Apr/2001 - Download sensor calibration file.                  */
/*                                                                            */
/*                       - Torque units set on card must be Nm or Nmm. Nm are */
/*                         always returned by API.                            */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "ATIFT"
#define MODULE_TEXT     "ATI F/T Sensor API"
#define MODULE_DATE     "05/05/2001"
#define MODULE_VERSION  "2.1"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

// ***** List of valid ATI ISA F/T card addresses to search...
USHORT   ATIFT_BaseList[] = { 0x0280,0x0288,0x0290,0x0298,0x02A0,0x02A8,0x02B0,0x02B8,ATIFT_BASE_INVALID };

// ***** List of ATI ISA F/T cards installed...
struct   ATIFT_CardItem       ATIFT_CardList[ATIFT_CARD_LIST];
short    ATIFT_SSN=ATIFT_SN_INVALID;            // Default sensor serial number.

/******************************************************************************/

struct  ATIFT_CS_Information ATIFT_CS_Table[] =  // Command Structure information...
{  // Function code   ,Function  ,Full size mode    ,Size of header   ,Size of full structure           
    { ATIFT_FC_SENSOR ,"Sensor"  ,ATIFT_SENSOR_WRITE,ATIFT_CS_FUNCMODE,sizeof(struct ATIFT_CS_Sensor ) / ATIFT_RAM_WORD },
    { ATIFT_FC_CARD   ,"Card"    ,ATIFT_CARD_WRITE  ,ATIFT_CS_FUNCMODE,sizeof(struct ATIFT_CS_Card   ) / ATIFT_RAM_WORD },
    { ATIFT_FC_BIAS   ,"Bias"    ,ATIFT_BIAS_WRITE  ,ATIFT_CS_FUNCMODE,sizeof(struct ATIFT_CS_Bias   ) / ATIFT_RAM_WORD },
    { ATIFT_FC_GETPEAK,"GetPeak" ,ATIFT_CS_MODE_READ,ATIFT_CS_FUNCMODE,sizeof(struct ATIFT_CS_GetPeak) / ATIFT_RAM_WORD },
    { ATIFT_FC_TEST   ,"Test"    ,ATIFT_CS_MODE_READ,ATIFT_CS_FUNCONLY,sizeof(struct ATIFT_CS_Test   ) / ATIFT_RAM_WORD },
    { ATIFT_FC_MATRIX ,"Matrix"  ,ATIFT_MATRIX_WRITE,ATIFT_CS_FUNCMODE,sizeof(struct ATIFT_CS_Matrix ) / ATIFT_RAM_WORD },
    { ATIFT_FC_IOLINE ,"I/O Line",ATIFT_IOLINE_WRITE,ATIFT_CS_FUNCMODE,sizeof(struct ATIFT_CS_IOLine ) / ATIFT_RAM_WORD },
    { ATIFT_FC_OUTPUT ,"Output"  ,ATIFT_OUTPUT_WRITE,ATIFT_CS_FUNCMODE,sizeof(struct ATIFT_CS_Output ) / ATIFT_RAM_WORD },
    { ATIFT_FC_CLEAR,NULL,0xFFFF,-1,-1 },        // End of table...
};

/******************************************************************************/

struct  STR_TextItem  ATIFT_RC_Text[] = 
{
    { ATIFT_RC_NOTSET   ,"Not Set"               },
    { ATIFT_RC_OK       ,"OK"                    },
    { ATIFT_RC_INVALID  ,"Invalid Command"       },
    { ATIFT_RC_SUPPORT  ,"Command Not Supported" },
    { ATIFT_RC_PARAMETER,"Parameter"             },
    { ATIFT_RC_TIMEOUT  ,"Timeout"               },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

int     ATIFT_CS_SizeSend( USHORT func, USHORT mode )
{                                           // Size of command structure to send (Set or Get).
int     indx,size=-1;

    for( indx=0; (ATIFT_CS_Table[indx].func != ATIFT_FC_CLEAR); indx++ )
    {
        if( ATIFT_CS_Table[indx].func != func )  // Not the one we want...
	{
	    continue;
        }

        if( ATIFT_CS_Table[indx].mode == ATIFT_CS_MODE_READ )
        {                                        // Get mode only, so enquiry...
            size = ATIFT_CS_Table[indx].enqr;
        }
	else
        if( ATIFT_CS_Table[indx].mode == mode )  // Set mode, so full structure size.
	{ 
	    size = ATIFT_CS_Table[indx].full;
        }
        else                                     // Get mode, so enquiry only...
	{
	    size = ATIFT_CS_Table[indx].enqr;
	}
    }

    return(size);
}

/******************************************************************************/

int     ATIFT_CS_SizeRecv( USHORT fc )     // Size of command structure to receive (Get only).
{
int      indx,size=-1;

    for( indx=0; (ATIFT_CS_Table[indx].func != ATIFT_FC_CLEAR); indx++ )
    {
        if( ATIFT_CS_Table[indx].func != fc )    // Not the one we want...
	{
	    continue;
        }

	size = ATIFT_CS_Table[indx].full;
    }

    return(size);
}

/******************************************************************************/

char   *ATIFT_CS_Desc( USHORT fc )          // Description of Function Code.
{
int     indx;
char   *desc="";

    for( indx=0; (ATIFT_CS_Table[indx].func != ATIFT_FC_CLEAR); indx++ )
    {
        if( ATIFT_CS_Table[indx].func != fc )    // Not the one we want...
	{
	    continue;
        }

        desc = ATIFT_CS_Table[indx].desc;
    }

    return(desc);
}

/******************************************************************************/

BOOL    ATIFT_API_started = FALSE;

/******************************************************************************/

PRINTF  ATIFT_PRN_messgf=NULL;                   // General messages printf function.
PRINTF  ATIFT_PRN_errorf=NULL;                   // Error messages printf function.
PRINTF  ATIFT_PRN_debugf=NULL;                   // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     ATIFT_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ATIFT_PRN_messgf,buff));
}

/******************************************************************************/

int     ATIFT_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ATIFT_PRN_errorf,buff));
}

/******************************************************************************/

int     ATIFT_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(ATIFT_PRN_debugf,buff));
}

/******************************************************************************/
/* Functions for accessing ATI ISA FT card's registers and dual-port RAM.     */
/******************************************************************************/

void    ATIFT_IndxRAM( USHORT base, USHORT indx )
{                                           // Set dual-port RAM index for read/write.
    ISA_word(ATIFT_IndxReg(base),indx);
}

void    ATIFT_DataPut( USHORT base, USHORT word )
{                                           // Write word to dual-port RAM (data).
    ISA_word(ATIFT_DataReg(base),word);
}

USHORT  ATIFT_DataGet( USHORT base )
{                                           // Read word from dual-port RAM (data).
    return(ISA_word(ATIFT_DataReg(base)));
}

USHORT  ATIFT_StatGet( USHORT base )
{                                           // Read word from status register.
    return(ISA_word(ATIFT_StatReg(base)));
}

/******************************************************************************/

USHORT  ATIFT_Mail2PC( USHORT base )        
{                                           // Get Card->PC MailBox status word.
    ATIFT_IndxRAM(base,ATIFT_RAM_MAIL2PC);  // Set MailBox address.
    return(ATIFT_DataGet(base));            // Get status word.
}

void    ATIFT_Mail2PC( USHORT base, USHORT word )
{                                           // Set Card->PC MailBox status word.
    ATIFT_IndxRAM(base,ATIFT_RAM_MAIL2PC);  // Set MailBox address.
    ATIFT_DataPut(base,word);               // Set status word.
}

void    ATIFT_Mail2ISA( USHORT base, USHORT word )
{                                           // Set PC->Card MailBox status word.
    ATIFT_IndxRAM(base,ATIFT_RAM_MAIL2ISA); // Set MailBox address.
    ATIFT_DataPut(base,word);               // Set status word.
}

/******************************************************************************/
/* Execute ATI card command.                                                  */
/******************************************************************************/

USHORT  ATIFT_send( USHORT base, void *buff, int size )
{
USHORT  rc;

    rc = ATIFT_send(base,buff,size,ATIFT_TIMEOUT_GENERAL);

    return(rc);
}

/******************************************************************************/

USHORT  ATIFT_send( USHORT base, void *buff, int size, double timeout )
{
USHORT *dp,fc,word,rc,ok;
short   i;
TIMER   timer("ATIFT_send()");

//  Write the command block (buff) to dual-port RAM...

    dp = (USHORT *)buff;
    fc = dp[0];		                    // Function code for command block.

//  1. PC writes command block to dual-port RAM (0x0000)...
    ATIFT_IndxRAM(base,ATIFT_RAM_COMMAND);  // Set RAM index to command buffer.

    for( i=0; (i < size); i++ )             // Copy command block to RAM.
    {
        ATIFT_DataPut(base,dp[i]);
    }

//  Read PC MailBox of dual-port RAM to clear IRQ flag...
    word = ATIFT_Mail2PC(base);             // Read Card->PC MailBox.
    ATIFT_debugf("ATIFT_Mail2PC(base=0x%04X)=0x%04X\n",base,word);
    ATIFT_Mail2PC(base,fc);                 // Clear acknowledgement.

//  2. PC writes command word to dual-port RAM PC->ISA mailbox (0x03FF)...
    ATIFT_Mail2ISA(base,fc);                // Set PC->Card MailBox function code.

//  3. F/T ISA card stops writing data and processes command...
//  4. F/T ISA card acknowledgement is put in dual-port RAM ISA->PC mailbox (0x03FE)...

//  Start timeout timer...
    timer.Reset();

    do                                      // Wait for command to complete...
    {
        ok = FALSE;

//      5. PC tests acknowledgement of command by reading Status Register (base+0x04)...
        word = ATIFT_StatGet(base);         // Read status register.

        if( (word & ATIFT_STAT_ACK) == 0x0000 )   
	{                                   // Command complete, check return code...
            ATIFT_debugf("ATIFT_StatGet(base=0x%04X)=0x%04X\n",base,word);

//          6. PC reads acknowledgement from ISA->PC mailbox (0x03FE), reseting interrupt flag...
            rc = (ATIFT_Mail2PC(base) & ATIFT_RC_MASK);    // Get return code.
            ok = (rc != ATIFT_RC_NOTSET);
         }
         else
         if( timer.Expired(timeout) )
         {                                  // Timeout period has expired...
             rc = ATIFT_RC_TIMEOUT;
             break;
         }
    }
    while( !ok );

    ATIFT_debugf("ATIFT_send(size=%d) fc=0x%04X rc=0x%02X\n",size,fc,rc);
    ATIFT_debugf("Timer %.1lf msec.\n",timer.Elapsed());

//  7. PC writes 0x0000 to dual-port RAM (0x0000) and ISA card resumes writing data...
    ATIFT_IndxRAM(base,ATIFT_RAM_COMMAND);
    ATIFT_DataPut(base,ATIFT_FC_CLEAR);
  
    return(rc);
}

/******************************************************************************/

void    ATIFT_recv( USHORT base, USHORT addr, void *buff, int size )
{
USHORT *dp;
short   i;
 
    ATIFT_IndxRAM(base,addr);               // RAM index address...

    dp = (USHORT *)buff;

    for( i=0; (i < size); i++ )             // Copy block from RAM...
    {
        dp[i] = ATIFT_DataGet(base);
    }
}

/******************************************************************************/

USHORT  ATIFT_func( USHORT base, void *data )
{
USHORT  rc;

    rc = ATIFT_func(base,data,ATIFT_TIMEOUT_GENERAL);

    return(rc);
}

/******************************************************************************/

USHORT  ATIFT_func( USHORT base, void *data, double timeout )
{
USHORT  func,mode,rc;
USHORT *dptr;

    dptr = (USHORT *)data;
    func = dptr[0];                              // 1st word is Function code.
    mode = dptr[1];                              // 2nd word is Mode (usually GET or SET).

    rc = ATIFT_send(base,data,ATIFT_CS_SizeSend(func,mode),timeout);

    if( rc == ATIFT_RC_OK )                      // Command successful...
    {
        ATIFT_recv(base,ATIFT_RAM_COMMAND,data,ATIFT_CS_SizeRecv(func));
        ATIFT_debugf("ATIFT_func(base=0x%04X,func=%s[0x%04X])\n",base,ATIFT_CS_Desc(func),func);
    }

    if( rc != ATIFT_RC_OK )                      // Command failed...
    {
        ATIFT_errorf("ATIFT_func(base=0x%04X,func=%s[0x%04X]) rc=0x%02X\n",base,ATIFT_CS_Desc(func),func,rc);
    }

    return(rc);
}

/******************************************************************************/

USHORT  ATIFT_cmd_sensor( USHORT base, USHORT mode, struct ATIFT_CS_Sensor *data )
{
USHORT  rc;
double  timeout;

    data->func = ATIFT_FC_SENSOR;           // Function code.
    data->mode = mode;                      // Mode (Get,Set).

    timeout = (mode == ATIFT_SENSOR_WRITE) ? ATIFT_TIMEOUT_WRITE : ATIFT_TIMEOUT_GENERAL;

    if( (rc=ATIFT_func(base,data,timeout)) != ATIFT_RC_OK )
    {                                       // Command failed...
        return(rc);
    }

    ATIFT_debugf("ATIFT_cmd_sensor(mode=0x%02X) SGN=%d SSN=%05d\n",
            data->mode,
            data->SGN,
	    data->SSN);

    return(rc);
}

/******************************************************************************/

USHORT  ATIFT_cmd_card( USHORT base, USHORT mode, struct ATIFT_CS_Card *data )
{
USHORT  rc;

    data->func = ATIFT_FC_CARD;             // Function code.
    data->mode = mode;                      // Mode (Get,Set).

    if( (rc=ATIFT_func(base,data)) != ATIFT_RC_OK )
    {                                       // Command failed...
        return(rc);
    }

    ATIFT_debugf("ATIFT_cmd_card(mode=0x%02X) DCF=0x%04X DM=0x%02X EHI=%1d SR=%u CM=%d CUL=%d CSN=%05d\n",
            data->mode,
            data->DCF,
            data->DM,
            data->EHI,
            data->SRHz,
            data->CM,
            data->CUL,
	    data->CSN);

    return(rc);
}

/******************************************************************************/

USHORT  ATIFT_cmd_bias( USHORT base, USHORT mode, struct ATIFT_CS_Bias *data )
{
USHORT  rc;

    data->func = ATIFT_FC_BIAS;             // Function code.
    data->mode = mode;                      // Mode (Get,Bias,Set).

    if( (rc=ATIFT_func(base,data)) != ATIFT_RC_OK )
    {                                       // Command failed...
        return(rc);
    }

    ATIFT_debugf("ATIFT_cmd_bias(mode=0x%02X)\n",data->mode);

    return(rc);
}

/******************************************************************************/

USHORT  ATIFT_cmd_getpeak( USHORT base, USHORT mode, struct ATIFT_CS_GetPeak *data )
{
USHORT  rc;

    data->func = ATIFT_FC_GETPEAK;          // Function code.
    data->mode = mode;                      // Reset peak registers?

    if( (rc=ATIFT_func(base,data)) != ATIFT_RC_OK )
    {                                       // Command failed...
        return(rc);
    }

    ATIFT_debugf("ATIFT_cmd_getpeak(mode=0x%02X)\n",data->mode);

    return(rc);
}

/******************************************************************************/

USHORT  ATIFT_cmd_test( USHORT base, struct ATIFT_CS_Test *data )
{
USHORT  rc;

    data->func = ATIFT_FC_TEST;             // Function code.

    if( (rc=ATIFT_func(base,data)) != ATIFT_RC_OK )
    {                                       // Command failed...
        return(rc);
    }

    ATIFT_debugf("ATIFT_cmd_test() test=0x%02X RAM=0x%04X gage=0x%02X\n",
                data->test,
                data->dpRAM,
                data->gage);

    return(rc);
}

/******************************************************************************/

USHORT  ATIFT_cmd_matrix( USHORT base, USHORT mode, struct ATIFT_CS_Matrix *data )
{
USHORT  rc;

    data->func = ATIFT_FC_MATRIX;           // Function code.
    data->mode = mode;                      // Mode (Get or Set).

    if( (rc=ATIFT_func(base,data)) != ATIFT_RC_OK )
    {                                       // Command failed...
        return(rc);
    }

    ATIFT_debugf("ATIFT_cmd_matrix(mode=0x%02X)\n",data->mode);

    return(rc);
}

/******************************************************************************/

USHORT  ATIFT_cmd_IOline( USHORT base, USHORT mode, struct ATIFT_CS_IOLine *data )
{
USHORT  rc;

    data->func = ATIFT_FC_IOLINE;           // Function code.
    data->mode = mode;                      // Mode (Get or Set).

    if( (rc=ATIFT_func(base,data)) != ATIFT_RC_OK )
    {                                       // Command failed...
        return(rc);
    }

    ATIFT_debugf("ATIFT_cmd_IOline(mode=0x%02X) I=0x%02X O=0x%02X\n",
                data->mode,
                data->input,
                data->output);

    return(rc);
}

/******************************************************************************/

char   *ATIFT_units[] = { "N/A","Lbf,Lbf-in","N,N-mm","N,N-m","Kg,Kg-cm",NULL };

/******************************************************************************/

USHORT  ATIFT_cmd_output( USHORT base, USHORT mode, struct ATIFT_CS_Output *data )
{
USHORT  rc;
float   cpf,cpt;
double  timeout;

    data->func = ATIFT_FC_OUTPUT;
    data->mode = mode;                      // Mode (Get only).

    timeout = (mode == ATIFT_OUTPUT_WRITE) ? ATIFT_TIMEOUT_WRITE : ATIFT_TIMEOUT_GENERAL;

    if( (rc=ATIFT_func(base,data,timeout)) != ATIFT_RC_OK )
      {                                     // Command failed...
        return(rc);
    }

    ATIFT_debugf("ATIFT_cmd_output(mode=0x%02X) %s bits=%u cpf=%u cpt=%u\n",
            data->mode,
            ATIFT_units[data->units],
            data->bits,
            data->CPF,
            data->CPT);

    return(rc);
}

/******************************************************************************/

struct  ATIFT_CS_Test   ATIFT_Test;
struct  ATIFT_CS_Sensor ATIFT_Sensor;
struct  ATIFT_CS_Card   ATIFT_Card;
struct  ATIFT_CS_Output ATIFT_Output;
STRING                  ATIFT_Output_Units;

/******************************************************************************/

#define ATIFT_INFO_NOTEST   -1
#define ATIFT_INFO_SENSOR    0
#define ATIFT_INFO_OUTPUT    1
#define ATIFT_INFO_CARD      2

char   *ATIFT_INFO[] = { "SENSOR","OUTPUT","CARD",NULL };

/******************************************************************************/

BOOL    ATIFT_info( USHORT base, int mode )
{
    return(ATIFT_info(base,mode,ATIFT_INFO_NOTEST));
}

/******************************************************************************/

BOOL    ATIFT_info( USHORT base, int mode, int test )
{
USHORT  rc;
BOOL    ok=FALSE;
int     info;

    if( mode == ATIFT_INFO_READ )
    {
        memset(&ATIFT_Sensor,0x00,sizeof(struct ATIFT_CS_Sensor));
        memset(&ATIFT_Card  ,0x00,sizeof(struct ATIFT_CS_Card  ));
        memset(&ATIFT_Output,0x00,sizeof(struct ATIFT_CS_Output));
    }

    for( info=ATIFT_INFO_SENSOR,ok=TRUE; ((info <= ATIFT_INFO_CARD) && ok); info++ )
    {
        if( (test != ATIFT_INFO_NOTEST) && (test == info) )
        {
            if( ATIFT_cmd_test(base) != ATIFT_TEST_OK )
            {
                ATIFT_errorf("ATIFT_info(base=0x%04X...) Failed test.\n",base);
                ok = FALSE;
                continue;
            }
        }

	switch( info )
	{
	   case ATIFT_INFO_SENSOR :
               rc = ATIFT_cmd_sensor(base,(mode == ATIFT_INFO_WRITE) ? ATIFT_SENSOR_WRITE : ATIFT_SENSOR_READ,&ATIFT_Sensor);
               break;

           case ATIFT_INFO_OUTPUT :
              rc = ATIFT_cmd_output(base,(mode == ATIFT_INFO_WRITE) ? ATIFT_OUTPUT_WRITE : ATIFT_OUTPUT_READ,&ATIFT_Output);
              break;

	   case ATIFT_INFO_CARD :
	       rc = ATIFT_cmd_card(base,(mode == ATIFT_INFO_WRITE) ? ATIFT_CARD_WRITE : ATIFT_CARD_READ,&ATIFT_Card);
	       break;
        }

        ok = (rc == ATIFT_RC_OK);

        STR_printf(ok,ATIFT_debugf,ATIFT_errorf,"ATIFT_info(base=0x%04X,write=%s) %s %s (rc=0x%02X).\n",
                   base,STR_YesNo(mode),ATIFT_INFO[info],STR_OkFailed(ok),rc);
    }

    return(ok);
}

/******************************************************************************/

BOOL    ATIFT_init( USHORT base, struct ATIFT_CardItem *item )
{
    if( !ATIFT_info(base,ATIFT_INFO_READ) )
    {
        return(FALSE);
    }

    item->SSN = ATIFT_Sensor.SSN;           // Sensor Serial Number.
    item->SGN = ATIFT_Sensor.SGN;           // Strain Gage Number.
    item->CSN = ATIFT_Card.CSN;             // Card Serial Number.
    item->SRHz = ATIFT_Card.SRHz;           // Card Sample Rate.

    switch( ATIFT_Output.units )            // Check torque units. (V2.1)
    {
        case ATIFT_OUTPUT_UNITS_Nmm :
        case ATIFT_OUTPUT_UNITS_Nm :
           break;

        default :
           ATIFT_errorf("ATIFT_init(base=0x%04X,S/N=%u) Invalid torque units (must be Nm or Nmm).\n",base,ATIFT_Sensor.SSN);
           break;
    }

    item->fc = ATIFT_CNTS_SCALE / (float)ATIFT_Output.CPF; // Force conversion (fc)...
    item->tc = ATIFT_CNTS_SCALE / (float)ATIFT_Output.CPT; // Torque conversion (tc)...

    if( ATIFT_Output.units == ATIFT_OUTPUT_UNITS_Nmm )     // Convert to Nm...
    {
        item->tc /= 100.0;
    }

    return(TRUE);
}

/******************************************************************************/

short   ATIFT_find( void )
{
short   base,card;

    for( base=0,card=0; ((ATIFT_BaseList[base] != ATIFT_BASE_INVALID) && (card < ATIFT_CARD_LIST)); base++ )
    {                                  // Check each ISA port address for an ATI F/T card...
        if( ATIFT_init(ATIFT_BaseList[base],&ATIFT_CardList[card]) )
        {                              // Yes, ATI F/T card responded...
            ATIFT_CardList[card].base = ATIFT_BaseList[base];
            card++;
        }
    }

    return(card);
}

/******************************************************************************/

void    ATIFT_list( PRINTF prnf )
{
int     card;

    for( card=0; (card < ATIFT_CARD_LIST); card++ )
    {
        if( ATIFT_CardList[card].base == ATIFT_BASE_INVALID )
        {
            continue;
        }

      (*prnf)("%d 0x%04X FT%05u\n",card,ATIFT_CardList[card].base,ATIFT_CardList[card].SSN);
    }
}

/******************************************************************************/

void    AITFT_list( void )
{
    ATIFT_list(printf);
}

/******************************************************************************/

short   ATIFT_findbase( USHORT base )
{
short   item,card=ATIFT_CARD_INVALID;

    for( item=0; (item < ATIFT_CARD_LIST); item++ )
    {
         if( ATIFT_CardList[item].base == ATIFT_BASE_INVALID )
         {
             continue;
         }

         if( ATIFT_CardList[item].base == base )
         {
             card = item;
             break;
         }
    }

    return(card);
}

/******************************************************************************/

short   ATIFT_findSSN( USHORT SSN )
{
short   item,card=ATIFT_CARD_INVALID;

    for( item=0; (item < ATIFT_CARD_LIST); item++ )
    {
         if( ATIFT_CardList[item].base == ATIFT_BASE_INVALID )
         {
             continue;
         }

         if( ATIFT_CardList[item].SSN == SSN )
         {
             card = item;
             break;
         }
    }

    return(card);
}

/******************************************************************************/

short   ATIFT_findCSN( USHORT CSN )
{
short   item,card=ATIFT_CARD_INVALID;

    for( item=0; (item < ATIFT_CARD_LIST); item++ )
    {
         if( ATIFT_CardList[item].base == ATIFT_BASE_INVALID )
         {
             continue;
         }

         if( ATIFT_CardList[item].CSN == CSN )
         {
             card = item;
             break;
         }
    }

    return(card);
}

/******************************************************************************/

int     ATIFT_getcard( int func )
{
static  int  posn=0;
int     find=ATIFT_CARD_INVALID,card,cards;

    for( cards=0,card=0; (card < ATIFT_CARD_LIST); card++ )
    {
        if( ATIFT_CardList[card].base == ATIFT_BASE_INVALID )
        {
            continue;
        }

        if( (func == ATIFT_GET_NEXT) && (card <= posn) )
        {
            continue;
        }

        if( find == ATIFT_CARD_INVALID )
        {
            find = posn = card;
        }

        cards++;
    }

    if( (func == ATIFT_GET_ONLY) && (cards > 1) )
    {
        ATIFT_errorf("ATIFT_getcard(...) Multiple F/T cards present.\n");
        find = ATIFT_CARD_INVALID;
    }

    if( find == ATIFT_SN_INVALID )
    {
        ATIFT_debugf("ATIFT_getcard(func=%d) Failed.\n",func);
    }

    return(find);
}

/******************************************************************************/

USHORT  ATIFT_getbase( int func )
{
int     card;
USHORT  base=ATIFT_BASE_INVALID;

    if( (card=ATIFT_getcard(func)) != ATIFT_CARD_INVALID )
    {
        base = ATIFT_CardList[card].base;
    }

    return(base);
}

/******************************************************************************/

USHORT  ATIFT_getSSN( int func )
{
int     card;
USHORT  SSN=ATIFT_SN_INVALID;

    if( (card=ATIFT_getcard(func)) != ATIFT_CARD_INVALID )
    {
        SSN = ATIFT_CardList[card].SSN;
    }

    return(SSN);
}

/******************************************************************************/

USHORT _ATIFT_getSSN( int func )
{
static  int  posn=0;
USHORT  SSN=ATIFT_SN_INVALID;
int     card,cards;

    for( cards=0,card=0; (card < ATIFT_CARD_LIST); card++ )
    {
        if( ATIFT_CardList[card].base == ATIFT_BASE_INVALID )
        {
            continue;
        }

        if( (func == ATIFT_GET_NEXT) && (card <= posn) )
        {
            continue;
        }

        if( SSN == ATIFT_SN_INVALID )
        {
            SSN = ATIFT_CardList[card].SSN;
            posn = card;
        }

        cards++;
    }

    if( (func == ATIFT_GET_ONLY) && (cards > 1) )
    {
        ATIFT_errorf("ATIFT_getSSN(...) Multiple F/T cards present.\n");
        SSN = ATIFT_SN_INVALID;
    }

    if( SSN == ATIFT_SN_INVALID )
    {
        ATIFT_debugf("ATIFT_getSSN(func=%d) Failed.\n",func);
    }

    return(SSN);
}

/******************************************************************************/

USHORT  ATIFT_base( short card, BOOL used )
{
USHORT  base=ATIFT_BASE_INVALID;

    if( !ATIFT_API_check() )                // API started?
    {
        return(base);
    }

    if( (ATIFT_CardList[card].base != ATIFT_BASE_INVALID) && (!used || ATIFT_CardList[card].used) )
    {
        base = ATIFT_CardList[card].base;
    }
    else
    {
        ATIFT_errorf("ATIFT_base(card=%d) Invalid card handle.\n",card);
    }

    return(base);
}

/******************************************************************************/

USHORT  ATIFT_base( short card )
{
USHORT  base;

    base = ATIFT_base(card,TRUE);

    return(base);
}

/******************************************************************************/

BOOL     ATIFT_data( USHORT base, struct ATIFT_Data *data, long timeout )
{
BOOL     ok,TO;
USHORT   stat,buff;
TIMER    timer("ATIFT_data()");

//  Start timout timer...
    timer.Reset();

//  Status get timeout loop...
    do
    {
        stat = ATIFT_StatGet(base);                   // Status...
        ok = ((stat & ATIFT_STAT_ACK) == 0x0000);     // Data ready?
        TO = timer.Expired(timeout);                  // Timeout?
    }
    while( !ok && !TO );                              // Loop until data ready or timeout...

//  Did we get some data?
    if( !ok )
    {
        return(ok);
    }

//  Get Card->PC MailBox status word...
    stat = ATIFT_Mail2PC(base);             

    if( (stat & ATIFT_MB_NEWDATA) == ATIFT_MB_NEWDATA )
    {                                       // New data is available...
        buff = (stat & ATIFT_MB_BUFFMASK);  // Which buffer?
    }
    else
    {                                       // No new data...
        ok = FALSE;                         // This should never happen...
    }

    if( !ok )
    {
        ATIFT_errorf("ATIFT_data(base=0x%04X,...) Data expected.\n",base);
        return(ok);
    }

    memset(data,0x00,sizeof(struct ATIFT_Data));
    ATIFT_recv(base,buff,data,sizeof(struct ATIFT_Data) / ATIFT_RAM_WORD);

    return(ok);
}

/******************************************************************************/
/* API functions...                                                           */
/******************************************************************************/

BOOL     ATIFT_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
short    card,cards;
BOOL     ok;

    if( ATIFT_API_started )            // Start the API once...
    {
        return(TRUE);
    }

    ATIFT_PRN_messgf = messgf;         // General API message print function.
    ATIFT_PRN_errorf = errorf;         // API error message print function.
    ATIFT_PRN_debugf = debugf;         // Debug information print function.

    for( card=0; (card < ATIFT_CARD_LIST); card++ )
    {
        memset(&ATIFT_CardList[card],0x00,sizeof(struct ATIFT_CardItem));
        ATIFT_CardList[card].base = ATIFT_BASE_INVALID;
        ATIFT_CardList[card].used = FALSE;
    }

    ATIFT_API_started = TRUE;          // Set started flag.

    if( (cards=ATIFT_find()) == 0 )
    {
        ATIFT_errorf("ATIFT_API_start(...) No ATI F/T card(s) found.\n");
    }

    for( card=0; (card < ATIFT_CARD_LIST); card++ )
    {
        if( ATIFT_CardList[card].base == ATIFT_BASE_INVALID )
        {
            continue;
        }

        ATIFT_debugf("card[%1d] base=0x%04X gage=%1d SSN=FT%05u CSN=FT%05u rate=%uHz\n",
                 card,
                 ATIFT_CardList[card].base,
                 ATIFT_CardList[card].SGN,
                 ATIFT_CardList[card].SSN,
                 ATIFT_CardList[card].CSN,
                 ATIFT_CardList[card].SRHz);
    }

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(ATIFT_API_stop);         // Install stop function.
        MODULE_start(ATIFT_PRN_messgf);     // Register module.
        ATIFT_messgf("%d ATI F/T card(s) found.\n",cards);
    }
    else
    {
        ATIFT_errorf("ATIFT_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void     ATIFT_API_stop( void )
{
short    card;

    if( !ATIFT_API_started )           // API not started in the first place...
    {
         return;
    }

    ATIFT_closeall();                  // Close all ATI F/T...

    ATIFT_API_started = FALSE;         // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL    ATIFT_API_check( void )
{
BOOL    ok=TRUE;

    if( !ATIFT_API_started )           // API not started...
    {                                  // Start module automatically...
        ok = ATIFT_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        ATIFT_debugf("ATIFT_API_check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/*****************************************************************************/

BOOL ATIFT_open( USHORT &SSN, BOOL bias )
{
USHORT  base;
short   card=ATIFT_CARD_INVALID;
BOOL    ok=FALSE;

    // API started?
    if( !ATIFT_API_check() )                
    {
        return(ATIFT_CARD_INVALID);
    }

    if( SSN == ATIFT_SN_ONLYCARD )
    {
        // Get SSN of only card in system.
        if( (SSN=ATIFT_getSSN(ATIFT_GET_ONLY)) == ATIFT_SN_INVALID )
        {
            return(FALSE);
        }
    }

    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(FALSE);
    }

    ATIFT_CardList[card].used = TRUE;
    ATIFT_messgf("OPEN[card=%1d] FT%05u.\n",card,ATIFT_CardList[card].SSN);

    if( bias )
    {
        ok = ATIFT_bias(SSN);
    }

    ATIFT_use(SSN);

    return(TRUE);
}

/******************************************************************************/

BOOL ATIFT_open( USHORT &SSN )
{
BOOL ok;

    ok = ATIFT_open(SSN,TRUE);

    return(ok);
}

/******************************************************************************/

BOOL ATIFT_open( void )
{
USHORT SSN=ATIFT_SN_INVALID;
BOOL ok;

    ok = ATIFT_open(SSN);

    return(ok);
}

/******************************************************************************/

void ATIFT_close( USHORT SSN )
{
USHORT base;
short card;

    // API started?
    if( !ATIFT_API_check() )
    {
        return;
    }

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) != ATIFT_CARD_INVALID )
    {
        // Card is currently open?
        if( !ATIFT_CardList[card].used )
        {
            ATIFT_CardList[card].used = FALSE;
            ATIFT_messgf("SHUT[card=%1d] FT%05u.\n",card,ATIFT_CardList[card].SSN);
        }
    }
}

/******************************************************************************/

void ATIFT_closeall( void )
{
short card;

    // API started?
    if( !ATIFT_API_check() )
    {
        return;
    }

    // Go through list of ATIFT cards.
    for( card=0; (card < ATIFT_CARD_LIST); card++ )
    {
        // Close card if it's currently open.
        if( !ATIFT_CardList[card].used )
        {
            ATIFT_close(card);
        }
    }
}

/******************************************************************************/

BOOL ATIFT_read( USHORT SSN, USHORT *seqno, float *ft )
{
USHORT base;
int r,i;
short card;
struct ATIFT_Data DATA;

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(FALSE);
    }

    // Find base address for card number.
    if( (base=ATIFT_base(card)) == ATIFT_BASE_INVALID )
    {
        return(FALSE);
    }

    // Get F/T data.
    if( !ATIFT_data(base,&DATA,ATIFT_TIMEOUT_DATA) )
    {
        ATIFT_errorf("ATIFT_read(card=%d,...) No data.\n",card);
        return(FALSE);
    }

    // Put sequence in variable.
    if( seqno != NULL )                     
    {  
       *seqno = (float)DATA.seqno;
    }

    r=0;

    // Put forces in variables.
    for( i=0; (i < ATIFT_DATA_FORCE); i++ )
    {
        ft[r++] = (float)DATA.force[i] * ATIFT_CardList[card].fc; 
    }

    // Put torques in variables.
    for( i=0; (i < ATIFT_DATA_TORQUE); i++ )
    {
        ft[r++] = (float)DATA.torque[i] * ATIFT_CardList[card].tc;
    }

    return(TRUE);
}

/******************************************************************************/

BOOL ATIFT_read( USHORT SSN, float *ft )
{
BOOL ok;

    ok = ATIFT_read(SSN,NULL,ft);

    return(ok);
}

/******************************************************************************/

BOOL ATIFT_bias( USHORT SSN )
{
USHORT base;
struct ATIFT_CS_Bias BIAS;
struct ATIFT_Data DATA;
BOOL ok=FALSE;
short card;

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(FALSE);
    }

    if( (base=ATIFT_base(card)) == ATIFT_BASE_INVALID )
    {
        return(FALSE);
    }

    // Must do a read before setting bias...
    if( ATIFT_data(base,&DATA,ATIFT_TIMEOUT_DATA) )
    {
        ok = (ATIFT_cmd_bias(base,ATIFT_BIAS_BIAS,&BIAS) == ATIFT_RC_OK);
    }

    return(ok);
}

/******************************************************************************/

USHORT ATIFT_rateset( USHORT SSN, USHORT SRHz )
{
USHORT base;
struct ATIFT_CS_Card CARD;
short card;

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(ATIFT_RATE_FAILED);
    }

    if( (base=ATIFT_base(card)) == ATIFT_BASE_INVALID )
    {
        return(ATIFT_RATE_FAILED);
    }

    memset(&CARD,0x00,sizeof(struct ATIFT_CS_Card));

    // Get current CARD information...
    if( ATIFT_cmd_card(base,ATIFT_CARD_READ,&CARD) != ATIFT_RC_OK ) 
    {                                         
        return(ATIFT_RATE_FAILED);
    }

    CARD.SRHz = SRHz;        // Put requested sample rate in CARD structure.

//  Now set CARD information with new sample rate...
    if( ATIFT_cmd_card(base,ATIFT_CARD_WRITE,&CARD) != ATIFT_RC_OK )
    {
        return(ATIFT_RATE_FAILED);
    }

    return(ATIFT_rateget(SSN));     // Return the actual rate.
}
/******************************************************************************/

USHORT ATIFT_rateget( USHORT SSN )
{
USHORT base,SRHz=0;
struct ATIFT_CS_Card   CARD;
short card;

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(ATIFT_RATE_FAILED);
    }

    if( (base=ATIFT_base(card)) == ATIFT_BASE_INVALID )
    {
        return(ATIFT_RATE_FAILED);
    }

    memset(&CARD,0x00,sizeof(struct ATIFT_CS_Card));

    // Get current CARD information...
    if( ATIFT_cmd_card(base,ATIFT_CARD_READ,&CARD) != ATIFT_RC_OK ) 
    {                                         
        return(ATIFT_RATE_FAILED);
    }

    ATIFT_CardList[card].SRHz = CARD.SRHz;

    return(CARD.SRHz);       // Sample rate from CARD structure.
}

/******************************************************************************/

USHORT   ATIFT_cmd_test( USHORT base )
{
USHORT   rc,test;

    memset(&ATIFT_Test,0x00,sizeof(struct ATIFT_CS_Test));

//  Perform card Reset and Self-Test...
    if( (rc=ATIFT_cmd_test(base,&ATIFT_Test)) != ATIFT_RC_OK ) 
    {
        ATIFT_errorf("ATIFT_cmd_test(base=0x%04X) Failed (rc=0x%02X).\n",base,rc);
        return(ATIFT_TEST_FAILED);
    }

//  Valid part of test bit map...
    test = ATIFT_Test.test & ATIFT_TEST_MASK;     

    ATIFT_debugf("ATIFT_cmd_test(base=0x%04X) test=0x%04X.\n",base,test);

    return(test);
}

/******************************************************************************/

USHORT ATIFT_test( USHORT SSN )
{
USHORT base,test;
short card;

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(ATIFT_TEST_FAILED);
    }

    if( (base=ATIFT_base(card)) == ATIFT_BASE_INVALID )
    {
        return(ATIFT_TEST_FAILED);
    }

    test = ATIFT_cmd_test(base);

    return(test);
}

/******************************************************************************/

BOOL  ATIFT_matrix( USHORT SSN, matrix &mtx )
{
USHORT base,rc;
struct ATIFT_CS_Matrix M;
int x,y;
short card;

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(FALSE);
    }

    if( (base=ATIFT_base(card)) == ATIFT_BASE_INVALID )
    {
        return(FALSE);
    }

    memset(&M,0x00,sizeof(struct ATIFT_CS_Matrix));

    if( (rc=ATIFT_cmd_matrix(base,ATIFT_MATRIX_READ,&M)) != ATIFT_RC_OK ) 
    {                                         
        return(FALSE);
    }

    matrix_dim(mtx,ATIFT_MATRIX_ROW,ATIFT_MATRIX_COL);

    for( x=0; (x < ATIFT_MATRIX_ROW); x++ )
    {
        for( y=0; (y < ATIFT_MATRIX_COL); y++ )
        {
            mtx(x+1,y+1) = (double)M.matrix[x][y] / ATIFT_MATRIX_FACTOR;
        }
    }

    return(TRUE);
}

/******************************************************************************/

BOOL ATIFT_sensor( USHORT SSN, USHORT mode, struct ATIFT_CS_Sensor *sensor )
{
USHORT base,rc;
struct ATIFT_CS_Sensor SENSOR;
int x,y;
matrix CM,HB,TC;
short card;

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(FALSE);
    }

    if( (base=ATIFT_base(card)) == ATIFT_BASE_INVALID )
    {
        return(FALSE);
    }

    if( (rc=ATIFT_cmd_sensor(base,mode,sensor)) != ATIFT_RC_OK ) 
    {                                         
        return(FALSE);
    }

    return(TRUE);
}

/******************************************************************************/
/* V2.1 - Read / Write sensor configuration information.                      */
/* Both SENSOR and OUTPUT information structures are used. A known bug in the */
/* SENSOR command requires some fields to be written with the OUTPUT command. */
/******************************************************************************/

void    ATIFT_sensor_cfg( void )
{
    CONFIG_reset();

    CONFIG_set("SerialNumber"   , ATIFT_Sensor.SSN);
    CONFIG_set("NumberOfGanges" , ATIFT_Sensor.SGN);
    CONFIG_set("MaxForce"       , ATIFT_Sensor.MaxF);
    CONFIG_set("MaxTorque"      , ATIFT_Sensor.MaxT);
    CONFIG_set("MaxTemperature" , ATIFT_Sensor.MaxTemp);
    CONFIG_set("MinTemperature" , ATIFT_Sensor.MinTemp);
    CONFIG_set("Calibration[1]" , ATIFT_Sensor.SCM[0],ATIFT_SENSOR_GAGE);
    CONFIG_set("Calibration[2]" , ATIFT_Sensor.SCM[1],ATIFT_SENSOR_GAGE);
    CONFIG_set("Calibration[3]" , ATIFT_Sensor.SCM[2],ATIFT_SENSOR_GAGE);
    CONFIG_set("Calibration[4]" , ATIFT_Sensor.SCM[3],ATIFT_SENSOR_GAGE);
    CONFIG_set("Calibration[5]" , ATIFT_Sensor.SCM[4],ATIFT_SENSOR_GAGE);
    CONFIG_set("Calibration[6]" , ATIFT_Sensor.SCM[5],ATIFT_SENSOR_GAGE);
    CONFIG_set("HardwareBias"   , ATIFT_Sensor.SHB,ATIFT_SENSOR_GAGE);
    CONFIG_set("TempCompMm"     , ATIFT_Sensor.MmA,ATIFT_SENSOR_GAGE);
    CONFIG_set("TempCompBm"     , ATIFT_Sensor.BmA,ATIFT_SENSOR_GAGE);
    CONFIG_set("TempCompMb"     , ATIFT_Sensor.MbA,ATIFT_SENSOR_GAGE);
    CONFIG_set("TempCompBb"     , ATIFT_Sensor.BbA,ATIFT_SENSOR_GAGE);
    CONFIG_set("DataCondition"  , ATIFT_Card.DCF);
    CONFIG_set("SampleRate"     , ATIFT_Card.SRHz);
    CONFIG_set("FT-Units"       , ATIFT_Output_Units);
    CONFIG_set("Resolution"     , ATIFT_Output.bits);
    CONFIG_set("CountsPerForce" , ATIFT_Output.CPF);
    CONFIG_set("CountsPerTorque", ATIFT_Output.CPT);
    CONFIG_set("CPF-Reserved"   , ATIFT_Output.CPF_card);
    CONFIG_set("CPT-Reserved"   , ATIFT_Output.CPT_card);
}

/******************************************************************************/

BOOL    ATIFT_cfg2sensor( USHORT base, char *file )
{
BOOL    ok=TRUE;
short   units;
USHORT  rc;
struct  ATIFT_CS_Test  TEST;

//  Set up configuration variables...
    ATIFT_sensor_cfg();

//  First read values, because we don't set everything...
    if( !ATIFT_info(base,ATIFT_INFO_READ) )
    {
        return(FALSE);
    }

//  Read configuration file...
    if( !CONFIG_read(file) )
    {
        return(FALSE);
    }

    for( units=1; ((ATIFT_units[units] != NULL) && (_stricmp(ATIFT_units[units],ATIFT_Output_Units) != 0)); units++ );

    if( ATIFT_units[units] != NULL )
    {
        ATIFT_Output.units = units;
    }

//  Ensure values common to both SENSOR & OUTPUT are consistent...
    ATIFT_Sensor.units = ATIFT_Output.units;
    ATIFT_Sensor.CPF = ATIFT_Output.CPF;
    ATIFT_Sensor.CPT = ATIFT_Output.CPT;

//  Write SENSOR, CARD & OUTPUT information...
    if( !ATIFT_info(base,ATIFT_INFO_WRITE,ATIFT_INFO_CARD) )
    {
        return(FALSE);
    }
/*
//  Reset card...
    if( (rc=ATIFT_cmd_test(base,&TEST)) != ATIFT_RC_OK )
    {
        ATIFT_errorf("ATIFT_cfg2sensor(base=0x%04X,file=%s) Card failed test (rc=0x%02X).\n",base,file,rc);
        return(FALSE);
    }

//  Write SENSOR, CARD & OUTPUT information...
    if( !ATIFT_info(base,ATIFT_INFO_WRITE) )
    {
        return(FALSE);
    }
*/
    return(ok);
}

/******************************************************************************/

BOOL    ATIFT_sensor2cfg( USHORT base, char *file )
{
BOOL    ok=TRUE;

//  Set up configuration variables...
    ATIFT_sensor_cfg();      

//  Read SENSOR, CARD & OUTPUT information...
    if( !ATIFT_info(base,ATIFT_INFO_READ) )
    {
        return(FALSE);
    }

    strncpy(ATIFT_Output_Units,ATIFT_units[ATIFT_Output.units],STRLEN);

//  Write configuration file...
    ok = CONFIG_write(file);

    return(ok);
}

/******************************************************************************/

BOOL    ATIFT_load( USHORT base, USHORT SSN )
{
char    file[MAXPATH],*path;
BOOL    ok;

    if( base == ATIFT_BASE_INVALID )        // Only one card to choose from...
    {
        base = ATIFT_getbase(ATIFT_GET_ONLY);
    }
    else                                    // Otherwise, find specific card base address...
    if( ATIFT_findbase(base) == ATIFT_CARD_INVALID )
    {
        base = ATIFT_BASE_INVALID;
    }

    if( base == ATIFT_BASE_INVALID )        // No valid base address...
    {
        ATIFT_errorf("ATIFT_load(base=0x%04X,S/N=%05u) Invalid base address.\n",base,SSN);
        return(FALSE);
    }

//  File name for sensor configuration...
    strncpy(file,STR_stringf("ATIFT_%u.CFG",SSN),MAXPATH);

//  Find the file in the search path...
    if( (path=FILE_Calibration(file)) == NULL )
    {
        ATIFT_errorf("ATIFT_load(base=0x%04X,S/N=%05u) Cannot find file (%s).\n",base,SSN,file);
        return(FALSE);
    }

//  Load configuration details from file into the sensor card...
    strncpy(file,path,MAXPATH);
    ok = ATIFT_cfg2sensor(base,file);

    STR_printf(ok,ATIFT_debugf,ATIFT_errorf,"ATIFT_load(base=0x%04X,S/N=%05u) (%s) %s.\n",base,SSN,file,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    ATIFT_load( USHORT SSN )
{
BOOL    ok;

    ok = ATIFT_load(ATIFT_BASE_INVALID,SSN);

    return(ok);
}

/******************************************************************************/

USHORT ATIFT_linein( USHORT SSN )
{
struct ATIFT_CS_IOLine IOLINE;
short card;
USHORT base;

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(ATIFT_IOLINE_FAILED);
    }

    if( (base=ATIFT_base(card)) == ATIFT_BASE_INVALID )
    {
        return(ATIFT_IOLINE_FAILED);
    }

    memset(&IOLINE,0x00,sizeof(struct ATIFT_CS_IOLine));

    // Read I/O LINE to get input states...
    if( ATIFT_cmd_IOline(base,ATIFT_IOLINE_READ,&IOLINE) != ATIFT_RC_OK ) 
    {                                         
        return(ATIFT_IOLINE_FAILED);
    }

    return(IOLINE.input);                   // Return bit map of input lines.
}

/******************************************************************************/

BOOL ATIFT_lineout( USHORT SSN, USHORT output )
{
struct ATIFT_CS_IOLine IOLINE;
short card;
USHORT base;

    // Find card number for serial number.
    if( (card=ATIFT_findSSN(SSN)) == ATIFT_CARD_INVALID )
    {
        return(ATIFT_IOLINE_FAILED);
    }

    if( (base=ATIFT_base(card)) == ATIFT_BASE_INVALID )
    {
        return(FALSE);
    }

    memset(&IOLINE,0x00,sizeof(struct ATIFT_CS_IOLine));

    // Read I/O LINE first before setting output...
    if( ATIFT_cmd_IOline(base,ATIFT_IOLINE_READ,&IOLINE) != ATIFT_RC_OK ) 
    {                                         
        return(ATIFT_IOLINE_FAILED);
    }

    IOLINE.output = output;                 // Set output field of I/O Line structure.

//  Write I/O Line to set output...
    if( ATIFT_cmd_IOline(base,ATIFT_IOLINE_WRITE,&IOLINE) != ATIFT_RC_OK ) 
    {                                         
        return(FALSE);
    }

    return(TRUE);            // If we're here, it must have worked.
}

/******************************************************************************/
/* These functions use default card handle...                                 */
/******************************************************************************/

void ATIFT_use( USHORT SSN )
{
    ATIFT_SSN = SSN;
}

/******************************************************************************/

void    ATIFT_close( void )
{
    ATIFT_close(ATIFT_SSN);
}

/******************************************************************************/

BOOL    ATIFT_read( USHORT *SeqN, float *ft )
{
BOOL ok;

    ok = ATIFT_read(ATIFT_SSN,SeqN,ft);

    return(ok);
}

/******************************************************************************/

BOOL    ATIFT_read( float *ft )
{
BOOL ok;

    ok = ATIFT_read(ATIFT_SSN,NULL,ft);

    return(ok);
}

/******************************************************************************/

BOOL    ATIFT_bias( void )
{
    return(ATIFT_bias(ATIFT_SSN));
}

/******************************************************************************/

USHORT  ATIFT_rateset( USHORT SRHz )
{
    return(ATIFT_rateset(ATIFT_SSN,SRHz));
}

/******************************************************************************/

USHORT  ATIFT_rateget( void )
{
    return(ATIFT_rateget(ATIFT_SSN));
}

/******************************************************************************/

USHORT  ATIFT_test( void )
{
    return(ATIFT_test(ATIFT_SSN));
}

/******************************************************************************/

BOOL    ATIFT_lineout( USHORT output )
{
    return(ATIFT_lineout(ATIFT_SSN,output));
}

/******************************************************************************/

USHORT  ATIFT_linein( void )
{
    return(ATIFT_linein(ATIFT_SSN));
}

/******************************************************************************/

ATIFT::ATIFT( )
{
    Init();
}

/******************************************************************************/

ATIFT::ATIFT( char *name, int serialno )
{
    Init(name,serialno);
}

/******************************************************************************/

ATIFT::ATIFT( int serialno )
{
    Init(serialno);
}

/******************************************************************************/

ATIFT::~ATIFT( )
{
    Close();
}

/******************************************************************************/

void ATIFT::Init( void )
{
    // Make sure API started...
    if( !ATIFT_API_check() )
    {
        return;
    }

    memset(ObjectName,0,STRLEN);

    OpenFlag = FALSE;
    SerialNumber = ATIFT_SN_INVALID;
}

/******************************************************************************/

void ATIFT::Init( int serialno )
{
    Init(STR_stringf("FT%05d",serialno),serialno);
}

/******************************************************************************/

void ATIFT::Init( char *name, int serialno )
{
    Init();

    strncpy(ObjectName,name,STRLEN);
    SerialNumber = serialno;
}

/******************************************************************************/

BOOL ATIFT::Open( int serialno, BOOL reset )
{
BOOL ok;

    Init(serialno);

    ok = Open(reset);

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::Open( BOOL reset )
{
BOOL ok=FALSE;

    if( ATIFT_open(SerialNumber,reset) )
    {
        ok = TRUE;
        OpenFlag = TRUE;
    }

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::Open( void )
{
BOOL ok;

    ok = Open(FALSE);

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::Opened( void )
{
    return(OpenFlag);
}

/******************************************************************************/

void ATIFT::Close( void )
{
    if( OpenFlag )
    {
        ATIFT_close(SerialNumber);
        OpenFlag = FALSE;
    }
}

/******************************************************************************/

BOOL ATIFT::Bias( void )
{
BOOL ok=FALSE;

    if( ATIFT_bias(SerialNumber) )
    {
        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::Read( int &sequenceno, double data[] )
{
BOOL ok=FALSE;
USHORT seqno;
float fdata[ATIFT_DATA_ARRAY];
int i;

    if( ATIFT_read(SerialNumber,&seqno,fdata) )
    {
        ok = TRUE;

        sequenceno = (int)seqno;

        for( i=0; (i < ATIFT_DATA_ARRAY); i++ )
        {
            data[i] = (double)fdata[i];
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::Read( double data[] )
{
BOOL ok;
int sequenceno;

    ok = Read(sequenceno,data);

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::RateSet( double frequency )
{
BOOL ok;

    if( ATIFT_rateset(SerialNumber,(USHORT)frequency) != ATIFT_RATE_FAILED )
    {
        ok = TRUE;
    }

    return(ok);    
}

/******************************************************************************/

BOOL ATIFT::RateGet( double &frequency )
{
USHORT rate;
BOOL ok=FALSE;

    if( (rate=ATIFT_rateget(SerialNumber)) != ATIFT_RATE_FAILED )
    {
        ok = TRUE;
        frequency = (double)rate;
    }

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::Test( int &test )
{
USHORT rc;
BOOL ok=FALSE;

    if( (rc=ATIFT_test(SerialNumber)) != ATIFT_TEST_FAILED )
    {
        ok = TRUE;
        test = (int)rc;
    }

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::Test( void )
{
int test;
BOOL ok;

    ok = Test(test);

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::LineSet( int value )
{
BOOL ok;

    ok = ATIFT_lineout(SerialNumber,value);

    return(ok);
}

/******************************************************************************/

BOOL ATIFT::LineGet( int &value )
{
BOOL ok;
USHORT rc;

    if( (rc=ATIFT_linein(SerialNumber)) != ATIFT_IOLINE_FAILED )
    {
        ok = TRUE;
        value = (int)rc;
    }

    return(ok);
}

/******************************************************************************/


