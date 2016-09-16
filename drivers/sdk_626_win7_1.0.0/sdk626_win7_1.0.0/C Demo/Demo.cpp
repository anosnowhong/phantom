//////////////////////////////////////////////////////
// DEMO.CPP : Interrupt-driven frequency counter.
//////////////////////////////////////////////////////

#include <windows.h>
#include <conio.h>
#include <stdio.h>

#include "Win626.h"

// Function prototypes.
VOID AppISR();
VOID CreateFreqCounter( HBD hbd, WORD CounterA, WORD GateTime );

#define COUNTER		CNTR_0A		// Counter A (and implicitly, B) channel to use.
#define GATE_TIME	1000		// Gate time in milliseconds.

//////////////////////////////////////////////////////////////////////
// APPLICATION MAIN FUNCTION.

int main()
{
	printf( "\n\nSENSORAY 626 DEMO PROGRAM\n\n" );

    // Link to S626.DLL.
    S626_DLLOpen();

    // Declare Model 626 board to driver and launch the interrupt thread.
    S626_OpenBoard( 0, 0, AppISR, THREAD_PRIORITY_ABOVE_NORMAL );
	DWORD ErrCode = S626_GetErrors( 0 );
    if ( ErrCode )
	{
        printf( "ONE OR MORE ERRORS DETECTED:\n" );
		
		// List errors.
		if ( ErrCode & 0x00000001 )		printf( " * Failed to open kernel-mode driver\n" );
		if ( ErrCode & 0x00000002 )		printf( " * Can't detect/register board\n" );
		if ( ErrCode & 0x00000004 )		printf( " * Memory allocation error\n" );
		if ( ErrCode & 0x00000008 )		printf( " * Can't lock DMA buffer\n" );
		if ( ErrCode & 0x00000010 )		printf( " * Can't launch interrupt thread\n" );
		if ( ErrCode & 0x00000020 )		printf( " * Can't enable IRQ\n" );
		if ( ErrCode & 0x00000040 )		printf( " * Missed interrupt\n" );
		if ( ErrCode & 0x00000080 )		printf( " * Can't instantiate board object\n" );
		if ( ErrCode & 0x00000100 )		printf( " * Unsupported kernel-mode driver version\n" );
		if ( ErrCode & 0x00010000 )		printf( " * D/A communication timeout\n" );
		if ( ErrCode & 0x00020000 )		printf( " * Illegal counter parameter\n" );

		printf( "\nHit any key to exit ..." );
		while ( !_kbhit() );
	}
    else
    {
        printf( "Initializing frequency counter ...\n" );
		
		// Configure counter A/B pair as a frequency counter.
        CreateFreqCounter( 0, COUNTER, GATE_TIME );

        // Enable interrupts in response to captured counter A overflows.
        S626_CounterIntSourceSet( 0, COUNTER, INTSRC_OVER );

        // Enable board’s master interrupt.
        S626_InterruptEnable( 0, true );

        // Suspend this thread for ten seconds.
        Sleep( 10000 );

	    // Terminate interrupt thread and release resources.
	    S626_CloseBoard( 0 );
    }

    // Disconnect from S626.DLL.
    S626_DLLClose();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ISR CALLBACK FUNCTION.

VOID AppISR()
{
	static WORD seconds = 0;

    // Get interrupt request (IRQ) status flags.
    WORD IntStatus[4];							// Array that receives interrupt status.
    S626_InterruptStatus( 0, IntStatus );       // Fetch IRQ status for all sources.

	if ( IntStatus[3] & OVERMASK(COUNTER) )
	{
		// Clear counter A overflow capture flag to negate the interrupt request.
		S626_CounterCapFlagsReset( 0, COUNTER );

		// Read and display measured frequency from counter B.
		printf( "%2d: Frequency (Hz) = %8d.\r", ++seconds, S626_CounterReadLatch( 0, COUNTER + 3 ) * 1000 / GATE_TIME );
	}
	else
	{
		printf( "UNEXPECTED INTERRUPT!!!\n" );
		Sleep( 250 );
	}

	// Enable board’s master interrupt.
	S626_InterruptEnable( 0, true );
}

////////////////////////////////////////////////////////////////////
// Configure an A/B counter pair to measure frequency.
// The counter pair is specified by the A member of the pair.
// Acquisition gate time is specified in milliseconds.
////////////////////////////////////////////////////////////////////

VOID CreateFreqCounter( HBD hbd, WORD CounterA, WORD GateTime )
{
	// Get logical counter number for counterB.
	WORD CounterB = CounterA + 3;

    // Set operating mode for counterA.
    S626_CounterModeSet( hbd, CounterA,
        ( LOADSRC_INDX   << BF_LOADSRC ) |     // Preload upon index.
        ( INDXSRC_SOFT   << BF_INDXSRC ) |     // Disable hardware index.
        ( CLKSRC_TIMER   << BF_CLKSRC  ) |     // Operating mode is Timer.
        ( CNTDIR_DOWN    << BF_CLKPOL  ) |     // Count direction is Down.
        ( CLKMULT_1X     << BF_CLKMULT ) |     // Clock multiplier is 1x.
        ( CLKENAB_ALWAYS << BF_CLKENAB ) );    // Counting is always enabled.

    // Set operating mode for counterB.
    S626_CounterModeSet( 0, CounterB,
        ( LOADSRCB_OVERA << BF_LOADSRC ) |     // Preload zeros upon leading gate edge.
        ( INDXSRC_SOFT   << BF_INDXSRC ) |     // Hardware index is disabled.
        ( CLKSRC_COUNTER << BF_CLKSRC  ) |     // Operating mode is Counter.
        ( CLKPOL_POS     << BF_CLKPOL  ) |     // Clock is active high.
        ( CLKMULT_1X     << BF_CLKMULT ) |     // Clock multiplier is 1x.
        ( CLKENAB_ALWAYS << BF_CLKENAB ) );    // Clock is always enabled.

    // Set counterB's preload value so that it will be reset upon counterA overflow.
    S626_CounterPreload( 0, CounterB, 0 );

    // Enable latching of counterB's acquired frequency data upon counterA overflow.
    S626_CounterLatchSourceSet( 0, CounterB, LATCHSRC_B_OVERA );

	// Set short interval for first timer period.
    S626_CounterPreload( hbd, CounterA, 50 * 2000 );		// Set timer interval to 50ms.
    S626_CounterSoftIndex( hbd, CounterA );					// Preload counter core.

	// Set counterA preload value to the desired gate time.  Since the counter
    // clock is fixed at 2 MHz, this is computed by multiplying milliseconds by 2,000.
    S626_CounterPreload( hbd, CounterA, GateTime * 2000 );	// Program gate time.

    // Enable preload of counterA in response to overflow.  This causes the timer to
    // restart automatically when its counts reach zero.
    S626_CounterLoadTrigSet( hbd, CounterA, LOADSRC_OVER  );
	S626_CounterCapFlagsReset( 0, CounterA );

	// Wait for end of first (short) timer interval and discard freq. data as it is not valid.
	WORD OverMask = OVERMASK( CounterA );
	while ( !( S626_CounterCapStatus( hbd ) & OverMask ) )
		Sleep( 50 );
	S626_CounterCapFlagsReset( 0, CounterA );
}
