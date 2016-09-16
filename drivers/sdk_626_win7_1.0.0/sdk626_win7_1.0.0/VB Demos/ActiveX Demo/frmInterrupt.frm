VERSION 5.00
Begin VB.Form frmInterrupt 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Master Interrupt"
   ClientHeight    =   360
   ClientLeft      =   1185
   ClientTop       =   4335
   ClientWidth     =   2325
   DrawStyle       =   6  'Inside Solid
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   360
   ScaleWidth      =   2325
   ShowInTaskbar   =   0   'False
   Begin VB.CheckBox ChkMasterIntEnable 
      Caption         =   "Master Interrupt Enabled"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   60
      Width           =   2055
   End
End
Attribute VB_Name = "frmInterrupt"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private InitMode As Integer

Private Sub Form_Load()
    InitMode = True
    ChkMasterIntEnable.Value = (bd.InterruptEnable <> 0) And 1
    InitMode = False
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.MnuMasterInterrupt.Checked = False
End Sub

Private Sub ChkMasterIntEnable_Click()
    If InitMode Then Exit Sub
    bd.InterruptEnable = ChkMasterIntEnable.Value
End Sub
