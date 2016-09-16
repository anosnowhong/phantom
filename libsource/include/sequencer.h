/******************************************************************************/
/* SEQUENCER - Execute a prescribed sequence of TRIGGER pulses...             */
/******************************************************************************/
#include <motor.h>

BOOL SEQUENCER_Open( int port, int channel, double pulsewidth );
void SEQUENCER_Close();

void SEQUENCER_Task( void );

BOOL SEQUENCER_Pulse( char *sequence, int index );

BOOL SEQUENCER_Load( double frequency, char *sequence, int repeats, int cycles, double period );
BOOL SEQUENCER_Load( double frequency, char *sequence, int repeats );
void SEQUENCER_Unload( void );

BOOL SEQUENCER_Loaded( void );
BOOL SEQUENCER_Running( void );

void SEQUENCER_Details( PRINTF prnf );

void SEQUENCER_Start( void );
void SEQUENCER_Stop( void );

BOOL SEQUENCER_Finished( int &pulsecount, double &elapsedseconds );
BOOL SEQUENCER_Finished( void );

/******************************************************************************/

