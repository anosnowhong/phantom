VERSION 5.00
Object = "{48D39029-367B-4534-8776-5F456B61C338}#1.0#0"; "S626.ocx"
Begin VB.MDIForm frmMain 
   BackColor       =   &H8000000C&
   Caption         =   "Sensoray 626 Demo"
   ClientHeight    =   6705
   ClientLeft      =   165
   ClientTop       =   735
   ClientWidth     =   8820
   LinkTopic       =   "MDIForm1"
   StartUpPosition =   3  'Windows Default
   WindowState     =   2  'Maximized
   Begin VB.PictureBox Picture1 
      Align           =   1  'Align Top
      Height          =   345
      Left            =   0
      ScaleHeight     =   285
      ScaleWidth      =   8760
      TabIndex        =   0
      Top             =   0
      Width           =   8820
      Begin VB.CheckBox ChkMasterIntEnable 
         Caption         =   "Interrupts Enabled"
         Enabled         =   0   'False
         Height          =   255
         Left            =   5340
         TabIndex        =   8
         Top             =   30
         Width           =   1635
      End
      Begin VB.TextBox TxtPciSlot 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BorderStyle     =   0  'None
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   225
         Left            =   1620
         TabIndex        =   3
         Text            =   "0"
         Top             =   30
         Width           =   495
      End
      Begin VB.TextBox TxtPciBus 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BorderStyle     =   0  'None
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   225
         Left            =   540
         TabIndex        =   2
         Text            =   "0"
         Top             =   30
         Width           =   495
      End
      Begin VB.CheckBox ChkIsOpen 
         Caption         =   "Open"
         Height          =   255
         Left            =   4320
         TabIndex        =   1
         Top             =   30
         Width           =   735
      End
      Begin VB.Label LblErrorFlags 
         Alignment       =   2  'Center
         BackColor       =   &H8000000A&
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   2760
         TabIndex        =   7
         Top             =   30
         Width           =   1215
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Errs :"
         Height          =   255
         Index           =   2
         Left            =   2280
         TabIndex        =   6
         Top             =   30
         Width           =   435
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Slot :"
         Height          =   255
         Index           =   1
         Left            =   1140
         TabIndex        =   5
         Top             =   30
         Width           =   435
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Bus :"
         Height          =   255
         Index           =   0
         Left            =   60
         TabIndex        =   4
         Top             =   30
         Width           =   375
      End
   End
   Begin VB.Timer Timer1 
      Interval        =   50
      Left            =   2820
      Top             =   1020
   End
   Begin S626Lib.S626 S626 
      Left            =   1320
      Top             =   1350
      _Version        =   65536
      _ExtentX        =   741
      _ExtentY        =   741
      _StockProps     =   0
   End
   Begin VB.Menu MnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuMainMenuExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu MnuTopAnalog 
      Caption         =   "Analog"
      Begin VB.Menu mnuADC 
         Caption         =   "ADC"
      End
      Begin VB.Menu MnuAnalogSep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuDAC 
         Caption         =   "DAC"
      End
      Begin VB.Menu MnuAnalogSep2 
         Caption         =   "-"
      End
      Begin VB.Menu MnuTrimDac 
         Caption         =   "Trim DAC"
      End
   End
   Begin VB.Menu MnuTopDio 
      Caption         =   "DIO"
      Begin VB.Menu mnuDIn 
         Caption         =   "Inputs"
      End
      Begin VB.Menu mnuDOut 
         Caption         =   "Outputs"
      End
      Begin VB.Menu mnuSeparator1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuCapEdgesStatus 
         Caption         =   "Capture Flags"
      End
      Begin VB.Menu mnuDioCapEn 
         Caption         =   "Capture Enables"
      End
      Begin VB.Menu mnuEdgSelection 
         Caption         =   "Capture Edges"
      End
      Begin VB.Menu mnuSeparator2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuDioIntEn 
         Caption         =   "Interrupt Enables"
      End
      Begin VB.Menu mnuDioIntEvents 
         Caption         =   "Interrupt Event Log"
      End
      Begin VB.Menu MnuSeparator3 
         Caption         =   "-"
      End
      Begin VB.Menu MnuDioFunction 
         Caption         =   "Special Function"
      End
   End
   Begin VB.Menu MnuTopCounter 
      Caption         =   "Counters"
      Begin VB.Menu mnuCounters 
         Caption         =   "0 A"
         Index           =   0
      End
      Begin VB.Menu mnuCounters 
         Caption         =   "1 A"
         Index           =   1
      End
      Begin VB.Menu mnuCounters 
         Caption         =   "2 A"
         Index           =   2
      End
      Begin VB.Menu mnuCounters 
         Caption         =   "0 B"
         Index           =   3
      End
      Begin VB.Menu mnuCounters 
         Caption         =   "1 B"
         Index           =   4
      End
      Begin VB.Menu mnuCounters 
         Caption         =   "2 B"
         Index           =   5
      End
   End
   Begin VB.Menu MnuTopOther 
      Caption         =   "Other"
      Begin VB.Menu mnuWatchDog 
         Caption         =   "WatchDog"
      End
      Begin VB.Menu MnuOtherSep1 
         Caption         =   "-"
      End
      Begin VB.Menu MnuBattery 
         Caption         =   "Battery"
      End
      Begin VB.Menu MnuOtherSep2 
         Caption         =   "-"
      End
      Begin VB.Menu MnuEeprom 
         Caption         =   "EEPROM"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private InitMode As Boolean

Private Sub CloseAllForms()
    Dim i As Integer
    ChildClose mnuADC, frmADC
    ChildClose MnuBattery, frmBattery
    ChildClose mnuCapEdgesStatus, frmCapEdgesStatus
    ChildClose mnuDAC, frmDAC
    ChildClose mnuDIn, frmDin
    ChildClose mnuDioCapEn, frmDioCapEn
    ChildClose MnuDioFunction, frmDioFunction
    ChildClose mnuDioIntEn, frmDioIntEn
    ChildClose mnuDioIntEvents, frmDioInterrupt
    ChildClose mnuDOut, frmDout
    ChildClose mnuEdgSelection, frmEdgSelection
    ChildClose MnuEeprom, frmEeprom
    ChildClose MnuTrimDac, frmTrimDac
    ChildClose mnuWatchDog, frmWatchdog
    For i = 0 To 5
        ChildClose mnuCounters(i), frmCounters(i)
    Next i
End Sub

Public Sub MenusEnable(ByVal Enable As Boolean)
    MnuTopAnalog.Enabled = Enable
    MnuTopCounter.Enabled = Enable
    MnuTopDio.Enabled = Enable
    MnuTopOther.Enabled = Enable
    ' If menus are disabled, close all open windows except the BoardAttributes window.
    If Not Enable Then CloseAllForms
End Sub

Private Sub ChildClose(m As Menu, f As Form)
    If m.Checked Then Unload f
End Sub

Private Sub ChildProc(m As Menu, f As Form)
    m.Checked = Not m.Checked
    If m.Checked Then f.Show Else Unload f
End Sub

Private Sub ChkMasterIntEnable_Click()
    If Not InitMode Then bd.InterruptEnable = ChkMasterIntEnable.Value
End Sub

Private Sub MDIForm_Load()
    Set bd = S626
    DioCaptureChan = -1
    MenusEnable False
    Me.Show
    
    InitMode = True
    ChkIsOpen.Value = (bd.BoardOpen = True) And 1
    TxtPciBus.Text = Format$(bd.PCIBus)
    TxtPciSlot.Text = Format$(bd.PCISlot)
    InitMode = False
    
    Timer1.Interval = 50
    Timer1.Enabled = True
End Sub

Private Sub MDIForm_Unload(Cancel As Integer)
    bd.InterruptEnable = False
    bd.BoardOpen = False
    CloseAllForms
End Sub

Private Sub MnuBattery_Click()
    ChildProc MnuBattery, frmBattery
End Sub

Private Sub mnuCounters_Click(Index As Integer)
    mnuCounters(Index).Checked = Not mnuCounters(Index).Checked
    If mnuCounters(Index).Checked = False Then
        Unload frmCounters(Index)
    Else
        frmCounters(Index).nCounter = Index
        frmCounters(Index).Show
    End If
End Sub

Private Sub mnuADC_Click()
    ChildProc mnuADC, frmADC
End Sub

Private Sub mnuCapEdgesStatus_Click()
    ChildProc mnuCapEdgesStatus, frmCapEdgesStatus
End Sub

Private Sub mnuDAC_Click()
    ChildProc mnuDAC, frmDAC
End Sub

Private Sub mnuDIn_Click()
    ChildProc mnuDIn, frmDin
End Sub

Private Sub MnuDioFunction_Click()
    ChildProc MnuDioFunction, frmDioFunction
End Sub

Private Sub mnuDioIntEn_Click()
    ChildProc mnuDioIntEn, frmDioIntEn
End Sub

Private Sub mnuDioIntEvents_Click()
    ChildProc mnuDioIntEvents, frmDioInterrupt
End Sub

Private Sub mnuDOut_Click()
    ChildProc mnuDOut, frmDout
End Sub

Private Sub mnuDioCapEn_Click()
    ChildProc mnuDioCapEn, frmDioCapEn
End Sub

Private Sub mnuEdgSelection_Click()
    ChildProc mnuEdgSelection, frmEdgSelection
End Sub

Private Sub MnuEeprom_Click()
    ChildProc MnuEeprom, frmEeprom
End Sub

Private Sub mnuMainMenuExit_Click()
    Unload Me
End Sub

Private Sub MnuTrimDac_Click()
    ChildProc MnuTrimDac, frmTrimDac
End Sub

Private Sub mnuWatchDog_Click()
    ChildProc mnuWatchDog, frmWatchdog
End Sub

Private Sub ChkIsOpen_Click()
    ' Open or close board for I/O processing.
    Dim ErrorCode As Long
    If Not InitMode Then
        InitMode = True
        bd.BoardOpen = (ChkIsOpen.Value <> 0)
        If bd.BoardOpen Then
            TxtPciBus.Text = Format$(bd.PCIBus)
            TxtPciSlot.Text = Format$(bd.PCISlot)
        Else
            If (ChkIsOpen.Value <> 0) Then ChkIsOpen.Value = 0
            ChkMasterIntEnable.Value = False
        End If
        ChkMasterIntEnable.Enabled = bd.BoardOpen
        InitMode = False
    End If
    MenusEnable bd.BoardOpen
    TxtPciBus.Enabled = Not bd.BoardOpen
    TxtPciSlot.Enabled = Not bd.BoardOpen
End Sub

Private Sub TxtPciBus_Validate(Cancel As Boolean)
    ' Declare PCI bus number.
    Dim Value As Integer
    On Error GoTo ErrBus
    If InitMode Then Exit Sub
    Value = CInt(TxtPciBus.Text)
    TxtPciBus.Text = Format$(Value)
    bd.PCIBus = Value
    Exit Sub
ErrBus:
    Value = 0
    Resume Next
End Sub

Private Sub TxtPciSlot_Validate(Cancel As Boolean)
    ' Declare PCI slot number.
    Dim Value As Integer
    On Error GoTo ErrSlot
    If InitMode Then Exit Sub
    Value = CInt(TxtPciSlot.Text)
    TxtPciSlot.Text = Format$(Value)
    bd.PCISlot = Value
    Exit Sub
ErrSlot:
    Value = 0
    Resume Next
End Sub

' Asynchronous event handlers ============================================================

Private Sub S626_DIOEdgeCapture(ByVal Chan As Integer)
    ' Process DIO interrupt.
    DioCaptureChan = Chan
    If (mnuDioIntEvents.Checked) Then frmDioInterrupt.LblLastDioCaptured.Caption = Format$(DioCaptureChan)
End Sub

Private Sub S626_IndexCapture(ByVal Chan As Integer)
    ' Process Counter Index interrupt.
    CntrIntIndex(Chan) = True
    CntrIntCount(Chan) = CntrIntCount(Chan) + 1
    ' Refresh interrupt status display.
    If (mnuCounters(Chan).Checked) Then
        With frmCounters(Chan)
            .chkIndexInterrupt.Value = CntrIntIndex(Chan) And 1
            .txtNoOfInterrupts.Text = Format$(CntrIntCount(Chan))
        End With
    End If
End Sub

Private Sub S626_OverflowCapture(ByVal Chan As Integer)
    ' Process Counter Overflow interrupt.
    CntrIntOver(Chan) = True
    CntrIntCount(Chan) = CntrIntCount(Chan) + 1
    ' Refresh interrupt status display.
    If (mnuCounters(Chan).Checked) Then
        With frmCounters(Chan)
            .chkOverflowInterrupt.Value = CntrIntOver(Chan) And 1
            .txtNoOfInterrupts.Text = Format$(CntrIntCount(Chan))
        End With
    End If
End Sub

Private Sub Timer1_Timer()
    ' Refresh all visible controls to reflect board's current state.
    Dim i As Integer
    Dim errs As Long
    For i = 0 To 5
        If (mnuCounters(i).Checked) Then frmCounters(i).FormUpdate
    Next i
    If mnuDIn.Checked Then frmDin.FormUpdate
    If mnuADC.Checked Then frmADC.FormUpdate
    If mnuWatchDog.Checked Then frmWatchdog.FormUpdate
    If mnuCapEdgesStatus.Checked Then frmCapEdgesStatus.FormUpdate
    ' Local status update.
    errs = bd.GetErrors
    LblErrorFlags.Caption = StrHex(errs, 8)
    LblErrorFlags.ForeColor = IIf(errs = 0, vbBlack, vbRed)
End Sub

