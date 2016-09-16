VERSION 5.00
Begin VB.Form FrmDemo 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Sine Wave Generator Demo"
   ClientHeight    =   600
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4995
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   600
   ScaleWidth      =   4995
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton BtnExit 
      Caption         =   "E&xit"
      Height          =   375
      Left            =   3240
      TabIndex        =   2
      Top             =   120
      Width           =   1215
   End
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Left            =   4500
      Top             =   90
   End
   Begin VB.CommandButton BtnGenSineWave 
      Caption         =   "&Run"
      Height          =   375
      Left            =   540
      TabIndex        =   1
      Top             =   120
      Width           =   1275
   End
   Begin VB.CommandButton BtnHalt 
      Caption         =   "&Halt"
      Enabled         =   0   'False
      Height          =   375
      Left            =   1920
      TabIndex        =   0
      Top             =   120
      Width           =   1215
   End
End
Attribute VB_Name = "FrmDemo"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'*****************************************************
' DEMO.FRM : Generate sine wave on DAC channel 0.
'*****************************************************
Option Explicit

Private IsRunning As Boolean

Private Sub Form_Load()
    ' Declare Model 626 board to driver.  This assumes only one 626 board in the system, otherwise
    ' the target board's address must be explicitly specified.
    
    '****************************************************************************
    ' IMPORTANT NOTE: VB CAN'T RELIABLY HANDLE INTERRUPTS FROM S626.DLL.  IF
    ' INTERRUPTS ARE REQUIRED, USE THE S626.OCX ACTIVE-X CONTROL INSTEAD.
    '****************************************************************************
    
    Call S626_OpenBoard(0, 0, 0, 0)
    If S626_GetErrors(0) Then
        MsgBox "ERROR: problem opening board.", , "Fatal Error"
        BtnGenSineWave.Enabled = False
    End If
End Sub

Private Sub BtnExit_Click()
    Call S626_CloseBoard(0)    ' Do a clean shut-down.
    Unload Me
End Sub

Private Sub EnableButtons()
    BtnHalt.Enabled = IsRunning
    BtnExit.Enabled = Not IsRunning
    BtnGenSineWave.Enabled = Not IsRunning
End Sub

Private Sub BtnGenSineWave_Click()
    Const NUM_STEPS = 200
    Static PhaseStep As Long
    IsRunning = True
    EnableButtons
    Do
        PhaseStep = PhaseStep + 1
        If (PhaseStep >= NUM_STEPS) Then PhaseStep = 0
        Call S626_WriteDAC(0, 0, CLng(Sin(CDbl(PhaseStep) * 2# * 3.14159 / NUM_STEPS) * 8191))
        DoEvents
    Loop While IsRunning
    EnableButtons
End Sub

Private Sub BtnHalt_Click()
    IsRunning = False
End Sub

