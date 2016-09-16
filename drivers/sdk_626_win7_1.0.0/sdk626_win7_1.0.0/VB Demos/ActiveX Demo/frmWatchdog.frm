VERSION 5.00
Begin VB.Form frmWatchdog 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Watchdog"
   ClientHeight    =   2340
   ClientLeft      =   10395
   ClientTop       =   525
   ClientWidth     =   1560
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   2340
   ScaleWidth      =   1560
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton cmdWatchdogClear 
      Caption         =   "&Clear"
      Height          =   315
      Left            =   420
      TabIndex        =   7
      Top             =   1980
      Width           =   855
   End
   Begin VB.CheckBox chkTimedOut 
      Caption         =   "Timed Out"
      Enabled         =   0   'False
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   1680
      Width           =   1095
   End
   Begin VB.CheckBox chkEnable 
      Caption         =   "Enable"
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   0
      Width           =   855
   End
   Begin VB.Frame Frame1 
      Caption         =   "Period"
      Height          =   1335
      Left            =   0
      TabIndex        =   0
      Top             =   240
      Width           =   1515
      Begin VB.OptionButton optPeriod 
         Caption         =   "10 seconds"
         Height          =   255
         Index           =   3
         Left            =   120
         TabIndex        =   4
         Top             =   960
         Width           =   1170
      End
      Begin VB.OptionButton optPeriod 
         Caption         =   "1 second"
         Height          =   255
         Index           =   2
         Left            =   120
         TabIndex        =   3
         Top             =   720
         Width           =   1170
      End
      Begin VB.OptionButton optPeriod 
         Caption         =   "1/2 second"
         Height          =   255
         Index           =   1
         Left            =   120
         TabIndex        =   2
         Top             =   480
         Width           =   1230
      End
      Begin VB.OptionButton optPeriod 
         Caption         =   "1/8th second"
         Height          =   255
         Index           =   0
         Left            =   120
         TabIndex        =   1
         Top             =   240
         Width           =   1290
      End
   End
End
Attribute VB_Name = "frmWatchdog"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub chkEnable_Click()
    bd.WatchdogEnable = chkEnable.Value
End Sub

Private Sub cmdWatchdogClear_Click()
    bd.WatchdogReset
End Sub

Private Sub Form_Load()
    optPeriod(bd.WatchdogPeriod) = True
    chkEnable.Value = 0                    'make sure the watchdog is disabled
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.mnuWatchDog.Checked = False
End Sub

Private Sub optPeriod_Click(Index As Integer)
    bd.WatchdogPeriod = Index
End Sub

Public Sub FormUpdate()
    chkTimedOut.Value = (bd.WatchdogTimedOut <> 0) And 1
End Sub
