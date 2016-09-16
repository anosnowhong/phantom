/******************************************************************************/
/*                                                                            */ 
/* MODULE  : ATIFT.h                                                          */ 
/*                                                                            */ 
/* PURPOSE : Interface functions for ATI ISA F/T Sensor.                      */ 
/*                                                                            */ 
/* DATE    : 19/Apr/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V2.0  JNI 19/Apr/2000 - Taken from module "phantom.cpp" & re-developed.    */
/*                                                                            */ 
/* V2.1  JNI 05/Apr/2001 - Download sensor calibration file.                  */ 
/*                                                                            */ 
/******************************************************************************/

#define ATIFT_DEBUG() if( !ATIFT_API_start(printf,printf,printf) ) { printf("Cannot start ATIFT API.\n"); exit(0); }

/******************************************************************************/
/* ATI ISA FT card is accessed using 3 I/O registers starting from the base   */
/* base port address.                                                         */
/* BASE+0 Index Register, pointer to dual-port RAM.                           */
/* BASE+2 Data Register, read/write to dual-port RAM.                         */
/* BASE+4 Status Register, bit mapped (see below).                            */
/******************************************************************************/

#define ATIFT_IndxReg(B)     ((B)+0x0000)
#define ATIFT_DataReg(B)     ((B)+0x0002)
#define ATIFT_StatReg(B)     ((B)+0x0004)

#define ATIFT_STAT_ACK       0x10      // Command complete or Data available.
#define ATIFT_STAT_IRQ       0x80      // Data available.

/******************************************************************************/
/* ATI ISA FT card contains 1K words of dual-port RAM for data exchange with  */
/* host PC. There are 5 areas: command buffer; 2 data buffers; 2 mailboxes.   */
/******************************************************************************/

#define ATIFT_RAM_WORD       sizeof(short)  // Size of ISA card word.

#define ATIFT_RAM_COMMAND    0x0000    // Command buffer from PC to ISA Card.
#define ATIFT_RAM_DATA_0     0x0100    // First FT data buffer.
#define ATIFT_RAM_DATA_1     0x0120    // Second FT data buffer.
#define ATIFT_RAM_MAIL2PC    0x03FE    // MailBox byte from ISA Card to PC.
#define ATIFT_RAM_MAIL2ISA   0x03FF    // MailBox byte from PC to ISA Card.

/******************************************************************************/
/* Command return codes. Returned in command data (LSB of command code).      */
/******************************************************************************/

#define ATIFT_RC_NOTSET      0x0000    // Not set.
#define ATIFT_RC_MASK        0x00FF    // Return code mask (LSB).
#define ATIFT_RC_OK          0x0001    // Successful.
#define ATIFT_RC_INVALID     0x0002    // Invalid Command.
#define ATIFT_RC_SUPPORT     0x0003    // Command not supported.
#define ATIFT_RC_PARAMETER   0x0004    // Invalid Parameter.
#define ATIFT_RC_TIMEOUT     0x000A    // API Error: Timeout.

/******************************************************************************/
/* MailBox codes.                                                             */
/******************************************************************************/

#define ATIFT_MB_NEWDATA     0xFF00    // New data available.
#define ATIFT_MB_BUFFMASK    0x01FF    // Data buffer address mask.

/******************************************************************************/
/* ATI ISA FT card function codes. Each function code is associated with a    */
/* command structure. This structure is written to and read from the card's   */
/* dual-port RAM (the data exchange interface between the PC & the ISA card). */
/******************************************************************************/

// Function codes...                                                          
#define ATIFT_FC_CLEAR       0x0000    // No command.
#define ATIFT_FC_SENSOR      0x0100    // Read sensor calibration values.
#define ATIFT_FC_CARD        0x0200    // Configure the card.
#define ATIFT_FC_BIAS        0x0300    // Read or Set bias (remove loading).
#define ATIFT_FC_GETPEAK     0x0400    // Get Peak values.
#define ATIFT_FC_TEST        0x0500    // Reset card and perform self test.
#define ATIFT_FC_MATRIX      0x0600    // Access calibration matrix.
#define ATIFT_FC_MONITOR     0x0700    // Set card monitoring conditions.
#define ATIFT_FC_IOLINE      0x0800    // Read/Write card I/O lines (9 pin).
#define ATIFT_FC_PATCH       0x0900    // Download Patch (reserved).
#define ATIFT_FC_OUTPUT      0x0A00    // Return output units, etc...

// Command Structure information...

#define ATIFT_CS_FUNCMODE    2         // Command Structure header (func + mode).
#define ATIFT_CS_FUNCONLY    1         // Command Structure header (func only).

struct  ATIFT_CS_Information           // Command Structure information.
{
    USHORT func;                       // Function code.

    char  *desc;                       // Description of function.

    USHORT mode;                       // Write mode (whole structure required).
#define ATIFT_CS_MODE_READ   0xFFFF    // Command only has read mode.

    int    enqr;                       // Size of enquiry only.
    int    full;                       // Full size (whole structure).
};

extern  struct ATIFT_CS_Information    ATIFT_CS_Table[];

/******************************************************************************/
/* Command structures...                                                      */
/******************************************************************************/

struct  ATIFT_CS_Sensor                // Command Structure for SENSOR...
{
    USHORT func;                       // Function code (See list above).

    USHORT mode;
#define ATIFT_SENSOR_READ    0x0000    // Read SENSOR information.
#define ATIFT_SENSOR_WRITE   0x000F    // Write SENSOR information to EEPROM.

#define ATIFT_SENSOR_GAGE    8         // Maximum number of strain gages.

#define ATIFT_SENSOR_CM      6         // Sensor Calibration Matrix (rows).
    long   SCM[ATIFT_SENSOR_CM][ATIFT_SENSOR_GAGE];

#define ATIFT_HB_ROW         1
    USHORT SHB[ATIFT_SENSOR_GAGE];     // Sensor Hardware Bias Array.

#define ATIFT_TC_MmA         1
#define ATIFT_TC_BmA         2
#define ATIFT_TC_MbA         3
#define ATIFT_TC_BbA         4
#define ATIFT_TC_ROW         4
    short  MmA[ATIFT_SENSOR_GAGE];     // Mm Array (temperature compensation).
    short  BmA[ATIFT_SENSOR_GAGE];     // Bm Array (temperature compensation).
    short  MbA[ATIFT_SENSOR_GAGE];     // Mb Array (temperature compensation).
    short  BbA[ATIFT_SENSOR_GAGE];     // Bb Array (temperature compensation).

    short  SGN;                        // Strain Gauge Number (6 or 8).

    short  CPF;                        // Counts Per Force (x10).
    short  CPT;                        // Counts Per Torque (x10).

    USHORT MaxF;                       // Maximum Force.
    USHORT MaxT;                       // Maximum Torque.
    USHORT MaxTemp;                    // Maximum Temperature.
    USHORT MinTemp;                    // Minimum Temperature.

    USHORT units;                      // Units...
#define ATIFT_SENSOR_UNITS_LBF    0x01 // Lft, Lbf-in.
#define ATIFT_SENSOR_UNITS_NMM    0x02 // N, N-mm.
#define ATIFT_SENSOR_UNITS_NMF    0x04 // N, N-m.
#define ATIFT_SENSOR_UNITS_KG     0x08 // Kg, Kg-cm.
#define ATIFT_SENSOR_UNITS_KLBF   0x10 // Klbf, Klbf-in.

    USHORT SSN;                        // Sensor Serial Number.
};


struct  ATIFT_CS_Card                  // Command Structure for CARD...
{
    USHORT func;                       // Function code (See list above).

    USHORT mode;
#define ATIFT_CARD_READ      0x0000    // Read CARD information.
#define ATIFT_CARD_WRITE     0x000F    // Write CARD information.

    USHORT DCF;                        // Data Conditioning Flags...
#define ATIFT_CARD_DCF_MODE  0x0007    // Filter mode.
#define ATIFT_CARD_DCF_BIAS  0x0008    // Enable software bias.
#define ATIFT_CARD_DCF_TEMP  0x0010    // Enable temperature compensation.
#define ATIFT_CARD_DCF_CALIB 0x0020    // Enable calibration.
#define ATIFT_CARD_DCF_PEAK  0x0040    // Enable peak holding.
#define ATIFT_CARD_DCF_COND  0x0080    // Enable monitoring conditions.
#define ATIFT_CARD_DCF_SAT   0x0100    // Enable saturation checking.
#define ATIFT_CARD_DCF_DPRAM 0x0200    // Reserve dual-port RAM.

    USHORT DM;                         // Data Mask...

    USHORT EHI;                        // Enable Hardware Interrupts...
#define ATIFT_CARD_EHI_ON    0x01
#define ATIFT_CARD_EHI_OFF   0x00

    USHORT SRHz;                       // Sample rate. (Hz)
#define ATIFT_RATE_MAX       7800      // Maximum sample rate (Hz).
#define ATIFT_RATE_FAILED    0         // Invalid rate (function failed).

    USHORT CM;                         // Card Model (100 for ISA FT).
    USHORT CUL;                        // Card Ucode Level.
    USHORT CSN;                        // Card Serial Number.
};


struct  ATIFT_CS_Bias                  // Command Structure for BIAS...
{
    USHORT func;                       // Function code (See list above).

    USHORT mode;
#define ATIFT_BIAS_READ      0x0000    // Read BIAS information.
#define ATIFT_BIAS_BIAS      0x0001    // Calculate new BIAS.
#define ATIFT_BIAS_WRITE     0x0002    // Write BIAS information.

#define ATIFT_BIAS_SBA       6         // Software Bias Array.
    short  SBA[ATIFT_BIAS_SBA];        // Force[x,y,z]; Torque[x,y,z].

#define ATIFT_BIAS_RES       2         // Reserved.
  short  res[ATIFT_BIAS_RES];
};


struct  ATIFT_CS_GetPeak               // Command Structure for GETPEAK...
{
    USHORT func;                       // Function code (See list above).

    USHORT mode;
#define ATIFT_GETPEAK_RESET   0x0001   // Reset peak registers.
#define ATIFT_GETPEAK_NORESET 0x0000   // Don't reset peak registers.

#define ATIFT_GETPEAK_AXIS   6         // Force[x,y,z]; Torque[x,y,z].

    long   min[ATIFT_GETPEAK_AXIS];    // Minimum peak array for each axis.
    long   max[ATIFT_GETPEAK_AXIS];    // Maximum peak array for each axis.
};


struct  ATIFT_CS_Test                  // Command Structure for TEST... 
{
    USHORT func;                       // Function code (See list above).

    USHORT test;                       // Bit mapped, see below...
#define ATIFT_TEST_NOTATTACH 0x0001    // Sensor not attached. 
#define ATIFT_TEST_RAMERROR  0x0002    // Dual-port RAM error. 
#define ATIFT_TEST_BIASERROR 0x0004    // Bias test error.    
#define ATIFT_TEST_DSPERROR  0x0008    // DSP internal error.  
#define ATIFT_TEST_OTHER     0x0010    // Some other error.   
#define ATIFT_TEST_MASK      0x001F    // Mask for bit map.
#define ATIFT_TEST_FAILED    0xFFFF    // TEST function failed (API only).
#define ATIFT_TEST_OK        0x0000    // No problems.

    USHORT dpRAM;                      // Bit mapped, error position.

    USHORT gage;                       // Bit mapped, gauge saturated.
};


struct  ATIFT_CS_Matrix                // Command Structure for MATRIX...
{
    USHORT func;                       // Function code (See list above).

    USHORT mode;
#define ATIFT_MATRIX_READ    0x0000    // Read current values.
#define ATIFT_MATRIX_WRITE   0x0001    // Write current values.

#define ATIFT_MATRIX_ROW     ATIFT_SENSOR_CM
#define ATIFT_MATRIX_COL     ATIFT_SENSOR_GAGE
#define ATIFT_MATRIX_FACTOR  1000000.0
    long   matrix[ATIFT_MATRIX_ROW][ATIFT_MATRIX_COL];
};

 
struct  ATIFT_CS_Monitor               // Command Structure for MONITOR...
{
    USHORT func;                       // Function code (See list above).

    USHORT mode;
#define ATIFT_MONITOR_READ   0x0000    // Read condition table entry.
#define ATIFT_MONITOR_SET    0x0001    // Set condition table entry.
#define ATIFT_MONITOR_CLEAR  0x0002    // Clear condition table entry.
#define ATIFT_MONITOR_RESET  0x0003    // Clear condition table.

    USHORT posn;                       // Table entry position (0...size-1).
    USHORT size;                       // Table size.
    USHORT cond;                       // Number of conditions monitors.

    USHORT stat;                       // Bit mapped monitor status...
#define ATIFT_MONITOR_NOT     0x0001   // Condition not monitored.    
#define ATIFT_MONITOR_EXPIRED 0x0002   // Condition has expired.         
#define ATIFT_MONITOR_GATED   0x0004   // Condition is gated off.           
#define ATIFT_MONITOR_NOALARM 0x0008   // Monitoring and not in alarm state.
#define ATIFT_MONITOR_ALARM   0x0010   // Monitoring and in alarm state.
#define ATIFT_MONITOR_LATCH   0x0080   // Alarm latch bit.                

    USHORT ctrl;                       // Bit mapped control word...
#define ATIFT_MONITOR_AXIS    0x0007   // Axis monitored.
#define ATIFT_MONITOR_COMPARE 0x0008   // See below...                   
#define ATIFT_MONITOR_LE      0x0000   // Less than or equal to.        
#define ATIFT_MONITOR_GE      0x0008   // Greater than or equal to.
#define ATIFT_MONITOR_PCBAO   0x0100   // PC Bus Alarm Output.
#define ATIFT_MONITOR_PCOAO   0x0200   // PC Bus Out-of-Alarm Output.
#define ATIFT_MONITOR_PCBEB   0x0400   // PC Bus Expired Before alarm output.
#define ATIFT_MONITOR_IGE     0x0800   // Input Gate Enabled.           
#define ATIFT_MONITOR_DOAO    0x0000   // Discrete Output Alarm Output.
#define ATIFT_MONITOR_DOOA    0x0000   // Discrete Output Out-of-Alarm.
#define ATIFT_MONITOR_DOE     0x0000   // Discrete Output Expired.        

    short  threshold;                  // Monitored Axis is compared to this value.

    USHORT tt;                         // Threshold time (msec).
#define ATIFT_MONITOR_NOTIME  0x0000   // No threshold time.           

    USHORT et;                         // Expiration time (msec).
#define ATIFT_MONITOR_NOEXP   0x0000   // Static condition does not expire.

    USHORT diocw;                      // Discrete IO Control Word, bit mapped...
#define ATIFT_MONITOR_OCA     0x0003   // Output Code for Alarm.       
#define ATIFT_MONITOR_OCOA    0x000C   // Output Code for Out-of-Alarm.
#define ATIFT_MONITOR_OCE     0x0030   // Output Code for Expiration only.
#define ATIFT_MONITOR_IGC     0x00C0   // Input Gate Code.              
};


struct  ATIFT_CS_IOLine                // Command Structure for IOLINE... 
{
    USHORT func;                       // Function code (See list above).

    USHORT mode;
#define ATIFT_IOLINE_READ     0x0000   // Read discrete I/O.           
#define ATIFT_IOLINE_WRITE    0x0001   // Write discrete I/O.           

#define ATIFT_IOLINE_LINE1    0x0001   // Bit mapped, Line 1.
#define ATIFT_IOLINE_LINE2    0x0002   // Bit mapped, Line 2.
#define ATIFT_IOLINE_FAILED   0xFFFF   // IOLINE function failed (API only).

    USHORT input;                      // Discrete input (green LEDs).
    USHORT output;                     // Discrete output (yellow LEDs).
};


struct  ATIFT_CS_Output                // Command Structure for OUTPUT...
{
    USHORT func;                       // Function code (See list above).

    USHORT mode;
#define ATIFT_OUTPUT_READ     0x0000   // Read OUTPUT information.
#define ATIFT_OUTPUT_WRITE    0x000F   // Write (reserved mode).
 
    USHORT units;                      // Units for force and torque...
#define ATIFT_OUTPUT_UNITS_Lbin   1    // Llb, Lbf-in.
#define ATIFT_OUTPUT_UNITS_Nmm    2    // N, N-mm.
#define ATIFT_OUTPUT_UNITS_Nm     3    // N, N-m.
#define ATIFT_OUTPUT_UNITS_Kgcm   4    // Kg, Kg-cm.

    USHORT bits;                       // Bit resolution (12-16).

    USHORT CPF;                        // Counts Per Force (scaled).
    USHORT CPT;                        // Counts Per Torque (scaled).
#define ATIFT_CNTS_SCALE   10.0        // Scale factor.

    USHORT CPF_card;                   // CPF field reserved by card.
    USHORT CPT_card;                   // CPT field reserved by card.
};

/******************************************************************************/
/* ATI FT card data structure.                                                */
/******************************************************************************/

struct  ATIFT_Data
{
    USHORT seqno;
    USHORT status;

#define ATIFT_DATA_FORCE      3
    long   force[ATIFT_DATA_FORCE];

#define ATIFT_DATA_TORQUE     3
    long   torque[ATIFT_DATA_TORQUE];
};

#define ATIFT_DATA_ARRAY      (ATIFT_DATA_FORCE + ATIFT_DATA_TORQUE)

#define ATIFT_DATA_Fx         0
#define ATIFT_DATA_Fy         1
#define ATIFT_DATA_Fz         2
#define ATIFT_DATA_Tx         3
#define ATIFT_DATA_Ty         4
#define ATIFT_DATA_Tz         5

/******************************************************************************/
/* ATI FT card identification structures.                                     */
/******************************************************************************/

extern  USHORT   ATIFT_baselist[];     // List of valid ISA addresses.

struct  ATIFT_CardItem                 // Card identification item.
{
    USHORT base;                       // Base address of card.
#define ATIFT_BASE_INVALID   0xFFFF    // Invalid Base address.

    short  SGN;                        // Number of gages.

    USHORT CSN;                        // Card Serial Number.
    USHORT SSN;                        // Sensor Serial Number.
#define ATIFT_SN_INVALID     0         // Invalid Serial Number.
#define ATIFT_SN_ONLYCARD    0         // Open the only card in the system.

    USHORT SRHz;                       // Sample rate (Hz).

    float  fc;                         // Force Conversion factor.
    float  tc;                         // Torque Conversion factor.

    BOOL   used;                       // Card is currently open.
};

#define ATIFT_CARD_LIST           4    // Maximum number of cards...
extern  struct  ATIFT_CardItem    ATIFT_CardList[ATIFT_CARD_LIST];

#define ATIFT_CARD_INVALID -1

/******************************************************************************/

#define ATIFT_TIMEOUT_GENERAL     50.0      // General timeout period (msec).
#define ATIFT_TIMEOUT_DATA        50.0      // Read data timeout (msec).
#define ATIFT_TIMEOUT_WRITE     2500.0      // Timeout period for write.

/******************************************************************************/
/* ATI FT card interface functions: Internal functions.                       */
/******************************************************************************/

// **** Print functions...
int     ATIFT_messgf( const char *mask, ... );
int     ATIFT_errorf( const char *mask, ... );
int     ATIFT_debugf( const char *mask, ... );

// **** General interface functions...
USHORT  ATIFT_send( USHORT base, void *buff, int size, double timeout );
void    ATIFT_recv( USHORT base, USHORT addr, void *buff, int size );
USHORT  ATIFT_func( USHORT base, void *data, double timeout );
USHORT  ATIFT_func( USHORT base, void *data );

// **** Specific command functions...
USHORT  ATIFT_cmd_sensor( USHORT base, USHORT mode, struct ATIFT_CS_Sensor *data );
USHORT  ATIFT_cmd_card( USHORT base, USHORT mode, struct ATIFT_CS_Card *data );
USHORT  ATIFT_cmd_bias( USHORT base, USHORT mode, struct ATIFT_CS_Bias *data );
USHORT  ATIFT_cmd_getpeak( USHORT base, USHORT mode, struct ATIFT_CS_GetPeak *data );
USHORT  ATIFT_cmd_test( USHORT base, struct ATIFT_CS_Test *data );
USHORT  ATIFT_cmd_test( USHORT base );
USHORT  ATIFT_cmd_matrix( USHORT base, USHORT mode, struct ATIFT_CS_Matrix *data );
USHORT  ATIFT_cmd_IOline( USHORT base, USHORT mode, struct ATIFT_CS_IOLine *data );
USHORT  ATIFT_cmd_output( USHORT base, USHORT mode, struct ATIFT_CS_Output *data );

// **** Special information function (does SENSOR, CARD and OUTPUT)...
BOOL    ATIFT_info( USHORT base, int mode, int test );
BOOL    ATIFT_info( USHORT base, int mode );
#define ATIFT_INFO_READ      0
#define ATIFT_INFO_WRITE     1

// **** Card interface setup functions...
BOOL    ATIFT_init( USHORT base, struct ATIFT_CardItem *item );
short   ATIFT_find( void );            // Find all ATI ISA FT cards...

// **** Print list of ISA cards...
void    ATIFT_list( PRINTF prnf );
void    ATIFT_list( void );

// **** Handle management functions...
short   ATIFT_findbase( USHORT base );
short   ATIFT_findSSN( USHORT SSN );
short   ATIFT_findCSN( USHORT CSN );

int     ATIFT_getcard( int func );
USHORT  ATIFT_getbase( int func );
USHORT  ATIFT_getSSN( int func );
#define ATIFT_GET_ONLY  0
#define ATIFT_GET_FIRST 1
#define ATIFT_GET_NEXT  2

USHORT  ATIFT_base( short card, BOOL used );
USHORT  ATIFT_base( short card );

USHORT  ATIFT_send( USHORT base, void *buff, int size );
void    ATIFT_recv( USHORT base, USHORT addr, void *buff, int size );

// **** Read sensor data function...
BOOL    ATIFT_data( USHORT base, long wait, struct ATIFT_Data *data );

// **** Sensor configuration functions...
void    ATIFT_sensor_cfg( void );
BOOL    ATIFT_cfg2sensor( USHORT base, char *file );
BOOL    ATIFT_sensor2cfg( USHORT base, char *file );

/******************************************************************************/
/* ATI FT card interface functions: API functions.                            */
/******************************************************************************/

BOOL    ATIFT_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    ATIFT_API_stop( void );
BOOL    ATIFT_API_check( void );

BOOL    ATIFT_open( USHORT &SSN, BOOL bias );
BOOL    ATIFT_open( USHORT &SSN );
BOOL    ATIFT_open( void );

void    ATIFT_close( USHORT SSN );
void    ATIFT_closeall( void );

BOOL    ATIFT_read( USHORT SSN, USHORT *SeqN, float *ft );
BOOL    ATIFT_read( USHORT SSN, float *ft );
BOOL    ATIFT_bias( USHORT SSN );
USHORT  ATIFT_rateset( USHORT SSN, USHORT SRHz );
USHORT  ATIFT_rateget( USHORT SSN );
USHORT  ATIFT_test( USHORT SSN );
BOOL    ATIFT_lineout( USHORT SSN, USHORT output );
USHORT  ATIFT_linein( USHORT SSN );
BOOL    ATIFT_matrix( USHORT SSN, matrix &mtx );
BOOL    ATIFT_sensor( USHORT SSN, USHORT mode, struct ATIFT_CS_Sensor *sensor );

BOOL    ATIFT_load( USHORT base, USHORT SSN );
BOOL    ATIFT_load( USHORT SSN );

/******************************************************************************/
/* These functions are just stubs that use the default card handle...         */
/******************************************************************************/

void    ATIFT_use( USHORT SSN );            // Set default card handle.

/******************************************************************************/

class ATIFT
{
private:
    STRING ObjectName;

    BOOL OpenFlag;

    USHORT SerialNumber;

public:
    ATIFT( );
    ATIFT( char *name, int serialno );
    ATIFT( int serialno );

   ~ATIFT( );

    void Init( void );
    void Init( int serialno );
    void Init( char *name, int serialno );

    BOOL Open( int serialno, BOOL reset );
    BOOL Open( BOOL reset );
    BOOL Open( void );

    BOOL Opened( void );

    void Close( void );

    BOOL Bias( void );

    BOOL Read( int &sequenceno, double data[] );
    BOOL Read( double data[] );

    BOOL RateSet( double frequency );
    BOOL RateGet( double &frequency );

    BOOL Test( int &test );
    BOOL Test( void );

    BOOL LineSet( int value );
    BOOL LineGet( int &value );
};

/******************************************************************************/

