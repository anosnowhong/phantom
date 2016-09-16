/******************************************************************************/

#include <sequencer.h>

/******************************************************************************/

#define TRIGGER_PORT_INVALID     -1         // Invalid port number.

#define TRIGGER_EVENTS           10         // Maxmum number of trigger events.
#define TRIGGER_INVALID          -1         // Invalid trigger handle.

struct  TRIGGER_EventItem
{
    BOOL      used;                         // Event used flag.
    BOOL      running;                      // Event currently running flag.
    BOOL      state;                        // Current logic state flag.
    BOOL      kill;                         // Kill event when finished.
    int       channel;                      // Channel number for event.

    int       pulse_type;                   // Pulse type (see below)...
#define TRIGGER_PULSE_LO2HI       0 
#define TRIGGER_PULSE_HI2LO       1

    int       reset_type;                   // Event reset type (see below)...
#define TRIGGER_RESET_SINGLE      0
#define TRIGGER_RESET_MULTIPLE    1

    double    tmark;
    double    width;
    double    period;

    int       total;
    int       pulses;

    int       loop;
};

/******************************************************************************/

int     TRIGGER_messgf( const char *mask, ... );
int     TRIGGER_errorf( const char *mask, ... );
int     TRIGGER_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    TRIGGER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    TRIGGER_API_stop( void );
BOOL    TRIGGER_API_check( void );

/******************************************************************************/

void    TRIGGER_init( void );
int     TRIGGER_free( void );

BOOL    TRIGGER_ID( int ID );

// Internal port I/O functions...
void    TRIGGER_put( UCHAR mask );
UCHAR   TRIGGER_get( void );

void    TRIGGER_1( int channel );
void    TRIGGER_0( int channel );

BOOL    TRIGGER_state( int channel );

void    TRIGGER_on( int ID );
void    TRIGGER_off( int ID );

BOOL    TRIGGER_status( int ID );

void    TRIGGER_task( void );

BOOL    TRIGGER_running( int ID );

/******************************************************************************/
/* Application level functions...                                             */
/******************************************************************************/

// Open and Close parallel port...
BOOL    TRIGGER_open( int port );
void    TRIGGER_close( void );

// Set-up a trigger event...
int     TRIGGER_setup( int channel, int pulse_type, int reset_type, double width_msec, double period_msec, int pulses );
int     TRIGGER_setup( int port, int channel, int pulse_type, int reset_type, double width_msec, double period_msec, int pulses );
int     TRIGGER_setup( int channel, int pulse_type, int reset_type, double width_msec );
int     TRIGGER_setup( int port, int channel, int pulse_type, int reset_type, double width_msec );

// Set-up and start a trigger event...
int     TRIGGER_start( int channel, int reset_type, double width_msec, double period_msec, int pulses );
int     TRIGGER_start( int channel, double width_msec );

//  Start an existing trigger event...
void    TRIGGER_start( int ID );

// Stop trigger events...
void    TRIGGER_stop( int ID );
void    TRIGGER_stopall( void );

//  Generate an immediate pulse...
void    TRIGGER_pulse( int channel, double width_msec );
void    TRIGGER_pulse( int port, int channel, double width_msec );

/******************************************************************************/

