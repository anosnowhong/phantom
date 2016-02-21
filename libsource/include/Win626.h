////////////////////////////////////////////////////////////////////////////////
// Module    : WIN626.H
// Function  : Declarations for pointers to DLL functions.
// Usage     : Included by all Windows apps that access S626.DLL functions.
// Author    : Jim Lamberson
// Copyright : (C) 2000 Sensoray
////////////////////////////////////////////////////////////////////////////////

#include "App626.h"

// Error codes returned by S626_DLLOpen().
#define ERR_LOAD_DLL	1		// Failed to open S626.DLL.
#define ERR_FUNCADDR	2		// Failed to find function name in S626.DLL.

#ifndef XFUNC626	// If compiling an app module, declare externals in WIN626.C ------------------

// Pointers to DLL functions:
#define XFUNC626(RTNTYPE,FUNCNAME)		extern "C" RTNTYPE (__stdcall *FUNCNAME)

// S626.DLL Startup and Shutdown functions: 
extern "C" DWORD S626_DLLOpen();	// Open DLL and initialize function pointers.
extern "C" VOID  S626_DLLClose();	// Close DLL.

#endif	// ----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////
// Pointers to functions that are exported by S626.DLL.
///////////////////////////////////////////////////////////////////////////////////

// Status and control functions.
XFUNC626( DWORD, S626_GetAddress			)( HBD hbd );
XFUNC626( DWORD, S626_GetErrors				)( HBD hbd );
XFUNC626( VOID,  S626_OpenBoard				)( HBD hbd, DWORD PhysLoc, FPTR_ISR IntFunc, DWORD Priority );
XFUNC626( VOID,  S626_CloseBoard			)( HBD hbd );
XFUNC626( VOID,  S626_InterruptEnable		)( HBD hbd, WORD enable );
XFUNC626( VOID,  S626_InterruptStatus		)( HBD hbd, WORD *status );

// Diagnostics.
XFUNC626( BYTE,  S626_I2CRead				)( HBD hbd, BYTE addr );
XFUNC626( VOID,  S626_I2CWrite				)( HBD hbd, BYTE addr, BYTE value );
XFUNC626( WORD,  S626_RegRead				)( HBD hbd, WORD addr );
XFUNC626( VOID,  S626_RegWrite				)( HBD hbd, WORD addr, WORD value );

// Analog I/O functions.
XFUNC626( VOID,  S626_ReadADC				)( HBD hbd, WORD *databuf );
XFUNC626( VOID,  S626_ResetADC				)( HBD hbd, BYTE *pollist );
XFUNC626( VOID,  S626_WriteDAC				)( HBD hbd, WORD chan, DWORD value );
XFUNC626( VOID,  S626_WriteTrimDAC			)( HBD hbd, BYTE chan, BYTE value );

// Digital I/O functions.
XFUNC626( WORD,  S626_DIOReadBank			)( HBD hbd, WORD group );
XFUNC626( WORD,  S626_DIOWriteBankGet		)( HBD hbd, WORD group );
XFUNC626( VOID,  S626_DIOWriteBankSet		)( HBD hbd, WORD group, WORD value );
XFUNC626( WORD,  S626_DIOEdgeGet			)( HBD hbd, WORD group );
XFUNC626( VOID,  S626_DIOEdgeSet			)( HBD hbd, WORD group, WORD value );
XFUNC626( VOID,  S626_DIOCapEnableSet		)( HBD hbd, WORD group, WORD chanmask, WORD enable );
XFUNC626( WORD,  S626_DIOCapEnableGet		)( HBD hbd, WORD group );
XFUNC626( VOID,  S626_DIOCapStatus			)( HBD hbd, WORD group );
XFUNC626( VOID,  S626_DIOCapReset			)( HBD hbd, WORD group, WORD value );
XFUNC626( WORD,  S626_DIOIntEnableGet		)( HBD hbd, WORD group );
XFUNC626( VOID,  S626_DIOIntEnableSet		)( HBD hbd, WORD group, WORD value );
XFUNC626( WORD,  S626_DIOModeGet			)( HBD hbd, WORD group );
XFUNC626( VOID,  S626_DIOModeSet			)( HBD hbd, WORD group, WORD value );

// Counter functions.
XFUNC626( VOID,  S626_CounterModeSet		)( HBD hbd, WORD chan, WORD mode );
XFUNC626( WORD,  S626_CounterModeGet		)( HBD hbd, WORD chan );
XFUNC626( VOID,  S626_CounterEnableSet		)( HBD hbd, WORD chan, WORD enable );
XFUNC626( VOID,  S626_CounterPreload		)( HBD hbd, WORD chan, DWORD value );
XFUNC626( VOID,  S626_CounterLoadTrigSet	)( HBD hbd, WORD chan, WORD value );
XFUNC626( VOID,  S626_CounterLatchSourceSet	)( HBD hbd, WORD chan, WORD value );
XFUNC626( DWORD, S626_CounterReadLatch		)( HBD hbd, WORD chan );
XFUNC626( WORD,  S626_CounterCapStatus		)( HBD hbd );
XFUNC626( VOID,  S626_CounterCapFlagsReset	)( HBD hbd, WORD chan );
XFUNC626( VOID,  S626_CounterSoftIndex		)( HBD hbd, WORD chan );
XFUNC626( VOID,  S626_CounterIntSourceSet	)( HBD hbd, WORD chan, WORD value );

// Battery functions:
XFUNC626( WORD,  S626_BackupEnableGet		)( HBD hbd );
XFUNC626( VOID,  S626_BackupEnableSet		)( HBD hbd, WORD en );
XFUNC626( WORD,  S626_ChargeEnableGet		)( HBD hbd );
XFUNC626( VOID,  S626_ChargeEnableSet		)( HBD hbd, WORD en );

// Watchdog functions:
XFUNC626( WORD,  S626_WatchdogTimeout		)( HBD hbd );
XFUNC626( WORD,  S626_WatchdogEnableGet		)( HBD hbd );
XFUNC626( VOID,  S626_WatchdogEnableSet		)( HBD hbd, WORD en );
XFUNC626( WORD,  S626_WatchdogPeriodGet		)( HBD hbd );
XFUNC626( VOID,  S626_WatchdogPeriodSet		)( HBD hbd, WORD val );
XFUNC626( VOID,  S626_WatchdogReset			)( HBD hbd );


