/******************************************************************************/
/*                                                                            */ 
/* MODULE  : STIM.cpp                                                         */ 
/*                                                                            */ 
/* PURPOSE : Parallel port (LPT) Interface API for Muscle Stimulator.         */ 
/*                                                                            */ 
/* DATE    : 23/Mar/2001                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 23/Mar/2001 - Initial development.                               */ 
/*                                                                            */ 
/******************************************************************************/

#define STIM_DEBUG() if( !STIM_API_start(printf,printf,printf) ) { printf("Cannot start STIMulator API.\n"); exit(0); }

/******************************************************************************/
// Bit masks for LPT data and control port...

#define STIM_DATA_CLOCK   0x01    // Clock signal for DS1267 serial interface.
#define STIM_DATA_XMIT    0x02    // Data transmit for DS1267.
#define STIM_DATA_CHIP_1  0x04    // Select line for DS1267 chip 1.
#define STIM_DATA_CHIP_2  0x08    // Select line for DS1267 chip 2.
#define STIM_DATA_TRIGGER 0x80    // Trigger line for stimulator.

#define STIM_CTRL_ENABLE  0x01    // Enable line for interface.

/******************************************************************************/

#define STIM_LPT_DEFAULT     1    // LPT port 1 is default.

#define STIM_CHANNEL_MAX     4    // Maximum channels.

#define STIM_CHIP_POT        2    // POTs per chip.
#define STIM_CHIP_MIN        1    // Chip number range...
#define STIM_CHIP_MAX        2

#define STIM_LOOPTASK    500.0    // Frequency of trigger LoopTask...

#define STIM_MODE_INIT      -1    // Pulse modes...
#define STIM_MODE_NONE       0
#define STIM_MODE_SINGLE     1
#define STIM_MODE_MULTIPLE   2

#define STIM_CHARGE     3000.0    // Stimulator charge period (msec).
#define STIM_PULSE_MIN     5.0    // Minimum inter-pulse interval (msec).

/******************************************************************************/

int     STIM_messgf( const char *mask, ... );
int     STIM_errorf( const char *mask, ... );
int     STIM_debugf( const char *mask, ... );

BOOL    STIM_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    STIM_API_stop( void );
BOOL    STIM_API_check( void );

BOOL    STIM_check( BOOL charge );

void    STIM_LPT( USHORT port, BYTE data );
BYTE    STIM_LPT( USHORT port );

void    STIM_enable( BOOL enable );
void    STIM_serialbit( BYTE data );
void    STIM_serialbyte( BYTE data );
void    STIM_select( int chip, BOOL enable );
void    STIM_POT( int chip, BYTE potA, BYTE potB );
void    STIM_loop( void );
void    STIM_trigger( int mode, double latency, double frequency, double duration );

/******************************************************************************/

BOOL    STIM_open( int LPTn, BOOL trace );
BOOL    STIM_open( BOOL trace );
BOOL    STIM_open( void );
void    STIM_close( void );

void    STIM_amplitude( int CH1, int CH2, int CH3, int CH4 );
void    STIM_amplitude( int CH[] );

void    STIM_pulse( void );
void    STIM_pulse( double IPI );
void    STIM_multiple( double latency, double frequency, double duration );
void    STIM_single( double latency );

BOOL    STIM_done( void );
void    STIM_wait( void );

/******************************************************************************/

void    STIM_TraceStart( void );
void    STIM_TraceLoop( void );
void    STIM_TraceStop( void );

/******************************************************************************/

