VERSION 5.00
Begin VB.Form frmBattery 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Battery"
   ClientHeight    =   690
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   1695
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   690
   ScaleWidth      =   1695
   ShowInTaskbar   =   0   'False
   Begin VB.CheckBox ChkChargeEnable 
      Caption         =   "Charge Enable"
      Height          =   255
      Left            =   120
      TabIndex        =   1
      Top             =   360
      Width           =   1695
   End
   Begin VB.CheckBox ChkBackupEnable 
      Caption         =   "Backup Enable"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1695
   End
End
Attribute VB_Name = "frmBattery"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private InitMode As Boolean

Private Sub Form_Load()
    InitMode = True
    ChkBackupEnable.Value = (bd.BackupEnable <> 0) And 1
    ChkChargeEnable.Value = (bd.ChargeEnable <> 0) And 1
    InitMode = False
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.MnuBattery.Checked = False
End Sub

Private Sub ChkBackupEnable_Click()
    If Not InitMode Then bd.BackupEnable = (ChkBackupEnable.Value <> 0)
End Sub

Private Sub ChkChargeEnable_Click()
    If Not InitMode Then bd.ChargeEnable = (ChkChargeEnable.Value <> 0)
End Sub

