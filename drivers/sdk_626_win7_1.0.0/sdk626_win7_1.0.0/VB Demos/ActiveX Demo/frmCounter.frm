VERSION 5.00
Begin VB.Form frmCounter 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Counter"
   ClientHeight    =   6615
   ClientLeft      =   8760
   ClientTop       =   510
   ClientWidth     =   2595
   DrawStyle       =   6  'Inside Solid
   FillStyle       =   0  'Solid
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   6615
   ScaleWidth      =   2595
   ShowInTaskbar   =   0   'False
   Begin VB.Frame Frame 
      Caption         =   "Core"
      Height          =   615
      Index           =   4
      Left            =   0
      TabIndex        =   27
      Top             =   0
      Width           =   2595
      Begin VB.CheckBox chkDisplayHex 
         Caption         =   "Hex"
         Height          =   195
         Left            =   1740
         TabIndex        =   29
         Top             =   300
         Width           =   615
      End
      Begin VB.TextBox txtDec 
         Alignment       =   2  'Center
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
         Left            =   180
         TabIndex        =   28
         Text            =   "16777215"
         Top             =   240
         Width           =   1275
      End
   End
   Begin VB.Frame Frame 
      Caption         =   "Index"
      Height          =   1215
      Index           =   9
      Left            =   0
      TabIndex        =   22
      Top             =   3960
      Width           =   1275
      Begin VB.CheckBox ChkSoftIndexOnly 
         Caption         =   "Soft Only"
         Height          =   195
         Left            =   120
         TabIndex        =   30
         Top             =   480
         Width           =   1035
      End
      Begin VB.CommandButton cmdSoftIdx 
         Caption         =   "Pulse"
         Height          =   315
         Left            =   300
         TabIndex        =   24
         Top             =   780
         Width           =   735
      End
      Begin VB.CheckBox ChkIndexPos 
         Caption         =   "Pos/On"
         Height          =   315
         Left            =   120
         TabIndex        =   23
         Top             =   180
         Width           =   1035
      End
   End
   Begin VB.Frame Frame 
      Caption         =   "Preload"
      Height          =   1035
      Index           =   8
      Left            =   0
      TabIndex        =   17
      Top             =   2100
      Width           =   2595
      Begin VB.ComboBox cmbPreloadTrigger 
         Height          =   315
         Left            =   840
         Style           =   2  'Dropdown List
         TabIndex        =   20
         Top             =   240
         Width           =   1575
      End
      Begin VB.CommandButton cmdLoadPreload 
         Caption         =   "Set"
         Height          =   315
         Left            =   120
         TabIndex        =   19
         Top             =   600
         Width           =   675
      End
      Begin VB.TextBox txtPreLoad 
         Height          =   315
         Left            =   840
         TabIndex        =   18
         Text            =   "?"
         Top             =   600
         Width           =   1455
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Trig :"
         Height          =   195
         Index           =   0
         Left            =   300
         TabIndex        =   21
         Top             =   300
         Width           =   435
      End
   End
   Begin VB.Frame Frame 
      Caption         =   "Clock"
      Height          =   1395
      Index           =   0
      Left            =   0
      TabIndex        =   11
      Top             =   660
      Width           =   2595
      Begin VB.Frame Frame 
         Caption         =   "Enable"
         Height          =   735
         Index           =   5
         Left            =   1380
         TabIndex        =   31
         Top             =   600
         Width           =   1155
         Begin VB.OptionButton OptEnable 
            Caption         =   "Index"
            Height          =   195
            Index           =   1
            Left            =   120
            TabIndex        =   33
            Top             =   480
            Width           =   855
         End
         Begin VB.OptionButton OptEnable 
            Caption         =   "Always"
            Height          =   195
            Index           =   0
            Left            =   120
            TabIndex        =   32
            Top             =   240
            Width           =   855
         End
      End
      Begin VB.ComboBox cmbCountSource 
         Height          =   315
         Left            =   600
         Style           =   2  'Dropdown List
         TabIndex        =   14
         Top             =   180
         Width           =   1275
      End
      Begin VB.ComboBox cmbMult 
         Height          =   315
         Left            =   600
         Style           =   2  'Dropdown List
         TabIndex        =   13
         Top             =   540
         Width           =   675
      End
      Begin VB.CheckBox ChkClkPos 
         Caption         =   "Pos/Up"
         Height          =   315
         Left            =   300
         TabIndex        =   12
         Top             =   960
         Width           =   855
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Mult :"
         Height          =   195
         Index           =   2
         Left            =   120
         TabIndex        =   16
         Top             =   600
         Width           =   435
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Mode :"
         Height          =   195
         Index           =   4
         Left            =   60
         TabIndex        =   15
         Top             =   240
         Width           =   495
      End
   End
   Begin VB.Frame Frame 
      Caption         =   "Capture"
      Height          =   1215
      Index           =   2
      Left            =   1380
      TabIndex        =   7
      Top             =   3960
      Width           =   1215
      Begin VB.CheckBox chkIndex 
         Caption         =   "Index"
         Enabled         =   0   'False
         Height          =   195
         Left            =   120
         TabIndex        =   10
         Top             =   480
         Width           =   795
      End
      Begin VB.CheckBox chkOverFlow 
         Caption         =   "Overflow"
         Enabled         =   0   'False
         Height          =   195
         Left            =   120
         TabIndex        =   9
         Top             =   240
         Width           =   975
      End
      Begin VB.CommandButton cmdClearFlags 
         Caption         =   "Clear"
         Height          =   315
         Left            =   240
         TabIndex        =   8
         Top             =   780
         Width           =   735
      End
   End
   Begin VB.Frame Frame 
      Caption         =   "Interrupt"
      Height          =   1395
      Index           =   1
      Left            =   0
      TabIndex        =   2
      Top             =   5220
      Width           =   2595
      Begin VB.ComboBox cmbInterruptSource 
         Height          =   315
         ItemData        =   "frmCounter.frx":0000
         Left            =   780
         List            =   "frmCounter.frx":0010
         Style           =   2  'Dropdown List
         TabIndex        =   25
         Top             =   240
         Width           =   1695
      End
      Begin VB.CheckBox chkIndexInterrupt 
         Caption         =   "Index"
         Enabled         =   0   'False
         Height          =   255
         Left            =   180
         TabIndex        =   6
         Top             =   720
         Width           =   735
      End
      Begin VB.CheckBox chkOverflowInterrupt 
         Caption         =   "Overflow"
         Enabled         =   0   'False
         Height          =   255
         Left            =   180
         TabIndex        =   5
         Top             =   960
         Width           =   975
      End
      Begin VB.CommandButton cmdClearInt 
         Caption         =   "Clear"
         Height          =   315
         Left            =   1320
         TabIndex        =   4
         Top             =   960
         Width           =   1155
      End
      Begin VB.TextBox txtNoOfInterrupts 
         Alignment       =   1  'Right Justify
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
         Left            =   1320
         Locked          =   -1  'True
         TabIndex        =   3
         Text            =   "0"
         Top             =   660
         Width           =   1155
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Source :"
         Height          =   195
         Index           =   1
         Left            =   120
         TabIndex        =   26
         Top             =   300
         Width           =   615
      End
   End
   Begin VB.Frame Frame 
      Caption         =   "Latch Trig (A/B Shared)"
      Height          =   735
      Index           =   3
      Left            =   0
      TabIndex        =   0
      Top             =   3180
      Width           =   2595
      Begin VB.ComboBox cmbLatchSource 
         Height          =   315
         ItemData        =   "frmCounter.frx":0042
         Left            =   180
         List            =   "frmCounter.frx":0052
         Style           =   2  'Dropdown List
         TabIndex        =   1
         Top             =   300
         Width           =   2235
      End
   End
End
Attribute VB_Name = "frmCounter"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public nCounter             As Integer      ' Counter channel number associated with this form instance.

Private InitMode            As Boolean      ' Disable controls while updating.
Private PairedCounter       As Integer      ' Other counter channel number of this A/B pair.

Private Sub ChkClkPos_Click()
    If InitMode Then Exit Sub
    bd.CounterClkPol(nCounter) = ChkClkPos.Value Xor 1
    RefreshConfig
End Sub

Private Sub ChkIndexPos_Click()
    If InitMode Then Exit Sub
    bd.CounterIndexPol(nCounter) = ChkIndexPos.Value Xor 1
    RefreshConfig
End Sub

Private Sub ChkSoftIndexOnly_Click()
    If InitMode Then Exit Sub
    bd.CounterIndexSrc(nCounter) = ChkSoftIndexOnly.Value
    RefreshConfig
End Sub

Private Sub cmbInterruptSource_Click()
    If InitMode Then Exit Sub
    bd.CounterIntSource(nCounter) = cmbInterruptSource.ListIndex
    RefreshConfig
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.mnuCounters(nCounter).Checked = False
End Sub

Private Sub Form_Load()
    Dim isB As Integer
    isB = (nCounter > 2) And 1
    
    Me.Caption = "Counter " & Format$(nCounter - isB * 3) & IIf(isB, "B", "A") & " (" & Format$(nCounter) & ")"
    
    With cmbCountSource
        .Clear
        .AddItem "Counter"
        .AddItem "Counter"
        .AddItem "Timer"
        .AddItem IIf(isB, "Extender", "Timer")
    End With
    
    With cmbPreloadTrigger
        .Clear
        .AddItem "My Index"
        .AddItem "My Overflow"
        .AddItem IIf(isB, Format$(nCounter - 3) & "A's Overflow", "Disabled")
        .AddItem "Disabled"
    End With
    
    With cmbMult
        .Clear
        .AddItem "4x"
        .AddItem "2x"
        .AddItem "1x"
    End With
    
    PairedCounter = nCounter + IIf(nCounter < 3, 3, -3)
    
    ' Init interrupt status.
    chkIndexInterrupt.Value = CntrIntIndex(nCounter) And 1
    chkOverflowInterrupt.Value = CntrIntOver(nCounter) And 1
    txtNoOfInterrupts.Text = Format$(CntrIntCount(nCounter))

    RefreshConfig
End Sub

Public Sub RefreshConfig()
    ' Prevent recursive calls.
    If InitMode Then Exit Sub
    InitMode = True
    
    ' Init controls based on the operating mode.
    cmbInterruptSource.ListIndex = bd.CounterIntSource(nCounter)
    cmbLatchSource.ListIndex = bd.CounterLatchSource(nCounter)
    cmbPreloadTrigger.ListIndex = bd.CounterLoadTrig(nCounter)
    ChkSoftIndexOnly.Value = bd.CounterIndexSrc(nCounter)
    ChkIndexPos.Value = bd.CounterIndexPol(nCounter) Xor 1
    cmbCountSource.ListIndex = bd.CounterClkSrc(nCounter)
    ChkClkPos.Value = bd.CounterClkPol(nCounter) Xor 1
    OptEnable(bd.CounterEnable(nCounter)).Value = True
    cmbMult.ListIndex = bd.CounterClkMult(nCounter)
    
    ' ClockMultiplier control is enabled only in Counter mode.
    cmbMult.Enabled = (cmbCountSource.ListIndex < 2)
    
    ' IndexPolarity control is enabled only if hard index is disabled.
    ChkIndexPos.Enabled = (ChkSoftIndexOnly.Value = 0)
    
    ' Refresh the paired A/B counter.
    If frmMain.mnuCounters(PairedCounter).Checked Then frmCounters(PairedCounter).RefreshConfig
    
    InitMode = False
    
    Me.Refresh
    
End Sub

Private Sub cmdClearFlags_Click()
    bd.CounterResetCapFlags nCounter
End Sub

Private Sub cmdSoftIdx_Click()
    bd.CounterSoftIndex nCounter
End Sub

Private Sub cmdClearInt_Click()
    CntrIntCount(nCounter) = 0
    CntrIntOver(nCounter) = False
    CntrIntIndex(nCounter) = False
End Sub

Private Sub cmbMult_Click()
    If InitMode Then Exit Sub
    bd.CounterClkMult(nCounter) = cmbMult.ListIndex
    RefreshConfig
End Sub

Private Sub cmbLatchSource_Click()
    If InitMode Then Exit Sub
    bd.CounterLatchSource(nCounter) = cmbLatchSource.ListIndex
    RefreshConfig
End Sub

Private Sub cmbCountSource_Click()
    If InitMode Then Exit Sub
    bd.CounterClkSrc(nCounter) = cmbCountSource.ListIndex
    RefreshConfig
End Sub

Private Sub cmbPreloadTrigger_Click()
    If InitMode Then Exit Sub
    bd.CounterLoadTrig(nCounter) = cmbPreloadTrigger.ListIndex
    RefreshConfig
End Sub

Private Sub cmdLoadPreload_Click()
    Const PRELOAD_DEFAULT = 5000000
    Dim Value As Long
    If txtPreLoad.Text = "?" Then txtPreLoad.Text = PRELOAD_DEFAULT
    On Error GoTo PreloadValueErr
    Value = CLng(txtPreLoad.Text)
    txtPreLoad.Text = Format$(Value)
    bd.CounterPreloadVal(nCounter) = Value
    RefreshConfig
    Exit Sub
PreloadValueErr:
    Value = PRELOAD_DEFAULT
    Resume Next
End Sub

Private Sub OptEnable_Click(Index As Integer)
    If InitMode Then Exit Sub
    bd.CounterEnable(nCounter) = Index
    RefreshConfig
End Sub

Public Sub FormUpdate()

    If (chkDisplayHex.Value) Then
        txtDec.Text = StrHex(bd.CounterReadLatch(nCounter), 6)
    Else
        txtDec.Text = StrDec(bd.CounterReadLatch(nCounter), 8)
    End If
    
    ' Refresh capture status.
    chkIndex.Value = bd.CounterIndexCaptured(nCounter) And 1
    chkOverFlow.Value = bd.CounterOverflowCaptured(nCounter) And 1

End Sub
