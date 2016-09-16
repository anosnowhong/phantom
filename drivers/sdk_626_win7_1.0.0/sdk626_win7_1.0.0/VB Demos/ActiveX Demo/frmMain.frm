VERSION 5.00
Object = "{48D39029-367B-4534-8776-5F456B61C338}#1.0#0"; "S626.ocx"
Begin VB.Form frmMain 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Sensoray 626 Demo"
   ClientHeight    =   1185
   ClientLeft      =   150
   ClientTop       =   720
   ClientWidth     =   3855
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1185
   ScaleWidth      =   3855
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer Timer1 
      Interval        =   50
      Left            =   3240
      Top             =   360
   End
   Begin VB.Frame Frame 
      Caption         =   "Error Flags"
      Height          =   1155
      Index           =   1
      Left            =   1980
      TabIndex        =   6
      Top             =   0
      Width           =   1815
      Begin VB.Label LblErrorFlags 
         Alignment       =   2  'Center
         BackColor       =   &H80000005&
         BorderStyle     =   1  'Fixed Single
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   240
         TabIndex        =   7
         Top             =   540
         Width           =   1215
      End
   End
   Begin VB.Frame Frame 
      Caption         =   "Select"
      Height          =   1155
      Index           =   2
      Left            =   60
      TabIndex        =   3
      Top             =   0
      Width           =   1815
      Begin VB.CheckBox ChkIsOpen 
         Caption         =   "Open"
         Height          =   255
         Left            =   720
         TabIndex        =   2
         Top             =   180
         Width           =   735
      End
      Begin VB.TextBox TxtPciBus 
         Alignment       =   2  'Center
         Height          =   285
         Left            =   960
         TabIndex        =   0
         Text            =   "0"
         Top             =   480
         Width           =   615
      End
      Begin VB.TextBox TxtPciSlot 
         Alignment       =   2  'Center
         Height          =   285
         Left            =   960
         TabIndex        =   1
         Text            =   "0"
         Top             =   780
         Width           =   615
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "PCI Bus :"
         Height          =   255
         Index           =   0
         Left            =   120
         TabIndex        =   5
         Top             =   480
         Width           =   735
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "PCI Slot :"
         Height          =   255
         Index           =   1
         Left            =   120
         TabIndex        =   4
         Top             =   780
         Width           =   735
      End
   End
   Begin S626Lib.S626 s626 
      Left            =   3300
      Top             =   840
      _Version        =   65536
      _ExtentX        =   979
      _ExtentY        =   661
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
      Begin VB.Menu MnuAnalogSep 
         Caption         =   "-"
      End
      Begin VB.Menu mnuDAC 
         Caption         =   "DAC"
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
      Begin VB.Menu MnuMasterInterrupt 
         Caption         =   "Master Interrupt"
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

Private Sub Form_Load()
    Set bd = s626
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

Private Sub Form_Unload(Cancel As Integer)
    bd.InterruptEnable = False
    bd.BoardOpen = False
    CloseAllForms
End Sub

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
    ChildClose MnuMasterInterrupt, frmInterrupt
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

Private Sub mnuMainMenuExit_Click()
    Unload Me
End Sub

Private Sub MnuMasterInterrupt_Click()
    ChildProc MnuMasterInterrupt, frmInterrupt
End Sub

Private Sub mnuWatchDog_Click()
    ChildProc mnuWatchDog, frmWatchdog
End Sub

Private Sub ChkIsOpen_Click()
    Dim ErrorCode As Long
    If Not InitMode Then
        InitMode = True
        bd.BoardOpen = (ChkIsOpen.Value <> 0)
        If bd.BoardOpen Then
            TxtPciBus.Text = Format$(bd.PCIBus)
            TxtPciSlot.Text = Format$(bd.PCISlot)
        Else
            If (ChkIsOpen.Value <> 0) Then ChkIsOpen.Value = 0
        End If
        InitMode = False
    End If
    MenusEnable bd.BoardOpen
    TxtPciBus.Enabled = Not bd.BoardOpen
    TxtPciSlot.Enabled = Not bd.BoardOpen
End Sub

Private Sub TxtPciBus_Validate(Cancel As Boolean)
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
    DioCaptureChan = Chan
End Sub

Private Sub S626_IndexCapture(ByVal Chan As Integer)
    CntrIntIndex(Chan) = True
    CntrIntCount(Chan) = CntrIntCount(Chan) + 1
End Sub

Private Sub S626_OverflowCapture(ByVal Chan As Integer)
    CntrIntOver(Chan) = True
    CntrIntCount(Chan) = CntrIntCount(Chan) + 1
End Sub

Private Sub Timer1_Timer()
    Dim i As Integer
    For i = 0 To 5
        If mnuCounters(i).Checked Then frmCounters(i).FormUpdate
    Next i
    If mnuDIn.Checked Then frmDin.FormUpdate
    If mnuADC.Checked Then frmADC.FormUpdate
    If mnuWatchDog.Checked Then frmWatchdog.FormUpdate
    If mnuCapEdgesStatus.Checked Then frmCapEdgesStatus.FormUpdate
    If mnuDioIntEvents.Checked Then frmDioInterrupt.FormUpdate
    ' Local status update.
    LblErrorFlags.Caption = StrHex(bd.GetErrors, 8)
End Sub

