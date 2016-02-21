/******************************************************************************/
/*                                                                            */ 
/* MODULE  : MMTimer.h                                                        */ 
/*                                                                            */ 
/* PURPOSE : Windows MultiMedia Timer functions.                              */ 
/*                                                                            */ 
/* DATE    : 14/Nov/2002                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 14/Nov/2002 - Separate module created for MultiMedia Timer.      */ 
/*                                                                            */ 
/******************************************************************************/

#define MMTIMER_DEBUG() if( !MMTIMER_API_start(printf,printf,printf) ) { printf("Cannot start MMTIMER API.\n"); exit(0); }

/******************************************************************************/

BOOL    MMTIMER_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    MMTIMER_API_stop( void );
BOOL    MMTIMER_API_check( void );

/******************************************************************************/

int     MMTIMER_messgf( const char *mask, ... );
int     MMTIMER_errorf( const char *mask, ... );
int     MMTIMER_debugf( const char *mask, ... );

/*****************************************************************************/

BOOL    MMTIMER_Start( UINT period, void (*func)( void ) );
BOOL    MMTIMER_Start( UINT period );
BOOL    MMTIMER_Stop( void );

/******************************************************************************/

void __stdcall MMTIMER_Task( unsigned int uID, unsigned int uMsg, unsigned long dwUser, unsigned long dw1, unsigned long dw2 );

/*****************************************************************************/

void    MMTIMER_PriorityLow( void );
void    MMTIMER_PriorityResume( void );

/******************************************************************************/

