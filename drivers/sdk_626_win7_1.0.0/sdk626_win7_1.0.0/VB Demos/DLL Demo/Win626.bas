Attribute VB_Name = "Win626"
Option Explicit

' ADC poll list attributes --------------------------------------------

Public Const EOPL = &H80                ' Marker for end of ADC poll list.
Public Const RANGE_10V = 0              ' ADC +/- 10V range.
Public Const RANGE_5V = &H10            ' ADC +/- 5V range.

' Counter constants ---------------------------------------------------

Public Const BITPOS_INTSRC = &H2000
Public Const BITPOS_LATCHSRC = &H800
Public Const BITPOS_LOADSRC = &H200
Public Const BITPOS_INDXSRC = &H80
Public Const BITPOS_INDXPOL = &H40
Public Const BITPOS_CLKSRC = &H10
Public Const BITPOS_CLKPOL = &H8
Public Const BITPOS_CLKMULT = &H2
Public Const BITPOS_CLKENAB = &H1

Public Const INTSRC_NONE = BITPOS_INTSRC * 0
Public Const INTSRC_OVER = BITPOS_INTSRC * 1
Public Const INTSRC_INDX = BITPOS_INTSRC * 2
Public Const INTSRC_BOTH = BITPOS_INTSRC * 3

Public Const LATCHSRC_AB_READ = BITPOS_LATCHSRC * 0
Public Const LATCHSRC_A_INDXA = BITPOS_LATCHSRC * 1
Public Const LATCHSRC_B_INDXB = BITPOS_LATCHSRC * 2
Public Const LATCHSRC_B_OVERA = BITPOS_LATCHSRC * 3

Public Const LOADSRC_INDX = BITPOS_LOADSRC * 0
Public Const LOADSRC_OVER = BITPOS_LOADSRC * 1
Public Const LOADSRCB_OVERA = BITPOS_LOADSRC * 2
Public Const LOADSRC_NONE = BITPOS_LOADSRC * 3

Public Const INDXSRC_HARD = BITPOS_INDXSRC * 0
Public Const INDXSRC_SOFT = BITPOS_INDXSRC * 1

Public Const INDXPOL_POS = BITPOS_INDXPOL * 0
Public Const INDXPOL_NEG = BITPOS_INDXPOL * 1

Public Const CLKSRC_COUNTER = BITPOS_CLKSRC * 0
Public Const CLKSRC_TIMER = BITPOS_CLKSRC * 2
Public Const CLKSRC_EXTENDER = BITPOS_CLKSRC * 3

Public Const CLKPOL_POS = BITPOS_CLKPOL * 0
Public Const CLKPOL_NEG = BITPOS_CLKPOL * 1
Public Const CNTDIR_UP = BITPOS_CLKPOL * 0
Public Const CNTDIR_DOWN = BITPOS_CLKPOL * 1

Public Const CLKMULT_4X = BITPOS_CLKMULT * 0
Public Const CLKMULT_2X = BITPOS_CLKMULT * 1
Public Const CLKMULT_1X = BITPOS_CLKMULT * 2

Public Const CLKENAB_ALWAYS = BITPOS_CLKENAB * 0
Public Const CLKENAB_INDEX = BITPOS_CLKENAB * 1

Public Const CNTR_0A = 0
Public Const CNTR_1A = 1
Public Const CNTR_2A = 2
Public Const CNTR_0B = 3
Public Const CNTR_1B = 4
Public Const CNTR_2B = 5

' Windows thread priority constants -------------------------

Public Const THREAD_PRIORITY_LOWEST = -2
Public Const THREAD_PRIORITY_BELOW_NORMAL = -1
Public Const THREAD_PRIORITY_NORMAL = 0
Public Const THREAD_PRIORITY_ABOVE_NORMAL = 1
Public Const THREAD_PRIORITY_HIGHEST = 2
Public Const THREAD_PRIORITY_TIME_CRITICAL = 15

' Error codes returned by S626_GetErrors() --------------------------------

' Unrecoverable errors:
Public Const ERR626_OPEN = &H1&
Public Const ERR626_CARDREG = &H2&
Public Const ERR626_ALLOC_MEMORY = &H4&
Public Const ERR626_LOCK_BUFFER = &H8&
Public Const ERR626_THREAD = &H10&
Public Const ERR626_INTERRUPT = &H20&
Public Const ERR626_LOST_IRQ = &H40&
Public Const ERR626_INIT = &H80&
' Recoverable errors:
Public Const ERR626_ILLEGAL_PARM = &H10000
Public Const ERR626_I2C = &H20000
Public Const ERR626_DACTIMEOUT = &H100000
Public Const ERR626_COUNTERSETUP = &H200000

' Exported functions supplied by S626.DLL -------------------------------------------

' Diagnostics:
Public Declare Sub S626_I2CWrite Lib "s626.dll" (ByVal hbd As Long, ByVal Adrs As Byte, ByVal Value As Byte)
Public Declare Sub S626_RegWrite Lib "s626.dll" (ByVal hbd As Long, ByVal Adrs As Integer, ByVal Data As Integer)
Public Declare Sub S626_WriteTrimDAC Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Byte, ByVal Value As Byte)
Public Declare Function S626_I2CRead Lib "s626.dll" (ByVal hbd As Long, ByVal Adrs As Byte) As Byte
Public Declare Function S626_RegRead Lib "s626.dll" (ByVal hbd As Long, ByVal Adrs As Integer) As Integer

' Analog I/O:
Public Declare Sub S626_ReadADC Lib "s626.dll" (ByVal hbd As Long, ByRef DataArray As Integer)
Public Declare Sub S626_ResetADC Lib "s626.dll" (ByVal hbd As Long, ByRef PollList As Byte)
Public Declare Sub S626_WriteDAC Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer, ByVal Value As Long)

' Battery:
Public Declare Sub S626_BackupEnableSet Lib "s626.dll" (ByVal hbd As Long, ByVal Enab As Integer)
Public Declare Sub S626_ChargeEnableSet Lib "s626.dll" (ByVal hbd As Long, ByVal Enab As Integer)
Public Declare Function S626_BackupEnableGet Lib "s626.dll" (ByVal hbd As Long) As Integer
Public Declare Function S626_ChargeEnableGet Lib "s626.dll" (ByVal hbd As Long) As Integer

' Watchdog:
Public Declare Sub S626_WatchdogEnableSet Lib "s626.dll" (ByVal hbd As Long, ByVal Enab As Integer)
Public Declare Sub S626_WatchdogPeriodSet Lib "s626.dll" (ByVal hbd As Long, ByVal Period As Integer)
Public Declare Sub S626_WatchdogReset Lib "s626.dll" (ByVal hbd As Long)
Public Declare Function S626_WatchdogEnableGet Lib "s626.dll" (ByVal hbd As Long) As Integer
Public Declare Function S626_WatchdogPeriodGet Lib "s626.dll" (ByVal hbd As Long) As Integer
Public Declare Function S626_WatchdogTimeout Lib "s626.dll" (ByVal hbd As Long) As Integer

' Digital I/O:
Public Declare Sub S626_DIOWriteBankSet Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer, ByVal Values As Integer)
Public Declare Sub S626_DIOEdgeSet Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer, ByVal Values As Integer)
Public Declare Sub S626_DIOCapReset Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer, ByVal Values As Integer)
Public Declare Sub S626_DIOCapEnableSet Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer, ByVal Chans As Integer, ByVal Enab As Integer)
Public Declare Sub S626_DIOIntEnableSet Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer, ByVal Values As Integer)
Public Declare Sub S626_DIOModeSet Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer, ByVal Overflow As Integer)
Public Declare Function S626_DIOReadBank Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer) As Integer
Public Declare Function S626_DIOWriteBankGet Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer) As Integer
Public Declare Function S626_DIOEdgeGet Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer) As Integer
Public Declare Function S626_DIOCapEnableGet Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer) As Integer
Public Declare Function S626_DIOCapStatus Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer) As Integer
Public Declare Function S626_DIOIntEnableGet Lib "s626.dll" (ByVal hbd As Long, ByVal Group As Integer) As Integer
Public Declare Function S626_DIOModeGet Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer) As Integer

' Counters:
Public Declare Sub S626_CounterCapFlagsReset Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer)
Public Declare Sub S626_CounterLatchSourceSet Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer, ByVal Value As Integer)
Public Declare Sub S626_CounterModeSet Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer, ByVal Mode As Integer)
Public Declare Sub S626_CounterEnableSet Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer, ByVal Enab As Integer)
Public Declare Sub S626_CounterLoadTrigSet Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer, ByVal Trig As Integer)
Public Declare Sub S626_CounterSoftIndex Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer)
Public Declare Sub S626_CounterPreload Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer, ByVal Value As Long)
Public Declare Sub S626_CounterIntSourceSet Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer, ByVal Source As Integer)
Public Declare Function S626_CounterReadLatch Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer) As Long
Public Declare Function S626_CounterCapStatus Lib "s626.dll" (ByVal hbd As Long) As Integer
Public Declare Function S626_CounterModeGet Lib "s626.dll" (ByVal hbd As Long, ByVal Chan As Integer) As Integer

' Status and Control:
Public Declare Sub S626_InterruptEnable Lib "s626.dll" (ByVal hbd As Long, ByVal Enable As Integer)
Public Declare Sub S626_InterruptStatus Lib "s626.dll" (ByVal hbd As Long, ByRef IntFlags As Integer)
Public Declare Sub S626_OpenBoard Lib "s626.dll" (ByVal hbd As Long, ByVal BoardAdrs As Long, ByVal IntCallback As Long, ByVal Priority As Long)
Public Declare Sub S626_CloseBoard Lib "s626.dll" (ByVal hbd As Long)
Public Declare Function S626_GetAddress Lib "s626.dll" (ByVal hbd As Long) As Long
Public Declare Function S626_GetErrors Lib "s626.dll" (ByVal hbd As Long) As Long

