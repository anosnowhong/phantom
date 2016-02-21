/******************************************************************************/
/*                                                                            */ 
/* MODULE  : ISA.h                                                            */ 
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

BOOL    ISA_API_start(  PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    ISA_API_stop( void );
BOOL    ISA_API_check( void );

/******************************************************************************/

BOOL    ISA_patch( void );                       // Install WindowsNT ISA patch.

void    ISA_word( USHORT addr, USHORT word );    // Write word to ISA port.
USHORT  ISA_word( USHORT addr );                 // Read word from ISA port.

void    ISA_byte( USHORT addr, BYTE byte );      // Write byte to ISA port.
BYTE    ISA_byte( USHORT addr );                 // Read byte from ISA port.

/******************************************************************************/

// VGA status port
#define VGA_PORT_STATUS 0x3DA

// Bit 3 signals vertical retrace
#define VGA_STATUS_RETRACE 0x08

BOOL VGA_VerticalRetrace( void );
BOOL VGA_VerticalRetraceOnset( void );
BOOL VGA_VerticalRetraceOffset( void );
BOOL VGA_VerticalRetraceOnsetOffset( TIMER *timer, double *onset_sec, double *offset_sec ); 
BOOL VGA_VerticalRetraceOnsetOffset( double &latency_sec );

/******************************************************************************/

// Base address for a particular LPT device number...
USHORT  LPT_base( int LPTn );                    // LPT (pallarel) port ISA address.
#define LPT_INVALID          0

// LPT device is 3 ISA ports (base + port)...
#define LPT_PORT_MAX         3                   // Number of ports in an LPT device...
#define LPT_PORT_DATA        0                   // Data port on LPT.
#define LPT_PORT_STAT        1                   // Status port on LPT.
#define LPT_PORT_CTRL        2                   // Control port on LPT.

// Data port bits...
#define LPT_DATA_D0          0                   // Data bit 0 (bi-direction, D25[2]).
#define LPT_DATA_D1          1                   // Data bit 1 (bi-direction, D25[3]).
#define LPT_DATA_D2          2                   // Data bit 2 (bi-direction, D25[4]).
#define LPT_DATA_D3          3                   // Data bit 3 (bi-direction, D25[5]).
#define LPT_DATA_D4          4                   // Data bit 4 (bi-direction, D25[6]).
#define LPT_DATA_D5          5                   // Data bit 5 (bi-direction, D25[7]).
#define LPT_DATA_D6          6                   // Data bit 6 (bi-direction, D25[8]).
#define LPT_DATA_D7          7                   // Data bit 7 (bi-direction, D25[9]).

// Status port bits...
#define LPT_STAT_ERROR       3                   // Printer Error (input, D25[15]).
#define LPT_STAT_SELECT      4                   // Printer Selected (input, D25[13]).
#define LPT_STAT_PAPEROUT    5                   // Paper Out (input, D25[12]).
#define LPT_STAT_ACK         6                   // Acknowledge (?, D25[10]).
#define LPT_STAT_BUSY        7                   // Busy (inverted, ?, D25[11]).

// Control port bits...
#define LPT_CTRL_STROBE      0                   // Data Strobe (inverted, output, D25[1]).
#define LPT_CTRL_AUTOLF      1                   // Auto LineFeed (inverted, ?, D25[14]).
#define LPT_CTRL_INIT        2                   // Initiallize (output, D25[16]).
#define LPT_CTRL_SELECT      3                   // Select (inverted, ?, D25[17]).

// Data port is bi-directional on most (not all) PCs...
#define LPT_CTRL_DIR         5                   // Control bit for data port direction.
#define LPT_DIR_OUT          0                   // Output (default).
#define LPT_DIR_IN           1                   // Input

// Functions to read and write bytes to LPT ports...
void    LPT_byte( int LPTn, USHORT port, BYTE byte );
BYTE    LPT_byte( int LPTn, USHORT port );
void    LPT_bits( int LPTn, USHORT port, BYTE bits[] );
void    LPT_bit( int LPTn, USHORT port, BYTE bit, BYTE value );

void    LPT_data( int LPTn, BYTE byte );
BYTE    LPT_data( int LPTn );

void    LPT_stat( int LPTn, BYTE byte );
BYTE    LPT_stat( int LPTn );

void    LPT_ctrl( int LPTn, BYTE byte );
BYTE    LPT_ctrl( int LPTn );

/******************************************************************************/

#define BIT_BYTE   8
#define BIT_WORD  16
#define BIT_DWORD 32
#define BIT_MAX   32

char   *BIT_text( int value, int size );
char   *BIT_text( BYTE value );

void    BIT_byte( BYTE value, BYTE bits[] );
BYTE    BIT_byte( BYTE bits[] );

void    BIT_word( WORD value, BYTE bits[] );
WORD    BIT_word( BYTE bits[] );

int     BIT_count( DWORD value );

/******************************************************************************/
