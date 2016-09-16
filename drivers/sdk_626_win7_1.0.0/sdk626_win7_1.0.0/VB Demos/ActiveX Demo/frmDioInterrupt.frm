VERSION 5.00
Begin VB.Form frmDioInterrupt 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "DIO Interrupts"
   ClientHeight    =   465
   ClientLeft      =   1185
   ClientTop       =   4335
   ClientWidth     =   1980
   DrawStyle       =   6  'Inside Solid
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   465
   ScaleWidth      =   1980
   ShowInTaskbar   =   0   'False
   Begin VB.Label LblLastDioCaptured 
      Alignment       =   2  'Center
      BorderStyle     =   1  'Fixed Single
      Caption         =   "-"
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
      Left            =   1260
      TabIndex        =   1
      Top             =   90
      Width           =   375
   End
   Begin VB.Label Label 
      Alignment       =   1  'Right Justify
      Caption         =   "Last Channel :"
      Height          =   255
      Index           =   0
      Left            =   180
      TabIndex        =   0
      Top             =   120
      Width           =   1035
   End
End
Attribute VB_Name = "frmDioInterrupt"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Load()
    LblLastDioCaptured.Caption = IIf(DioCaptureChan = -1, "-", Format$(DioCaptureChan))
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.mnuDioIntEvents.Checked = False
End Sub
