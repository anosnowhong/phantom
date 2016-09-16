VERSION 5.00
Begin VB.Form frmDioFunction 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "DIO Function"
   ClientHeight    =   2595
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   2505
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   2595
   ScaleWidth      =   2505
   ShowInTaskbar   =   0   'False
   Begin VB.CheckBox ChkDio 
      Caption         =   "DIO 5"
      Height          =   255
      Index           =   5
      Left            =   360
      TabIndex        =   6
      Top             =   1320
      Width           =   855
   End
   Begin VB.CheckBox ChkDio 
      Caption         =   "DIO 4"
      Height          =   255
      Index           =   4
      Left            =   360
      TabIndex        =   5
      Top             =   1080
      Width           =   855
   End
   Begin VB.CheckBox ChkDio 
      Caption         =   "DIO 3"
      Height          =   255
      Index           =   3
      Left            =   360
      TabIndex        =   4
      Top             =   840
      Width           =   855
   End
   Begin VB.CheckBox ChkDio 
      Caption         =   "DIO 2"
      Height          =   255
      Index           =   2
      Left            =   360
      TabIndex        =   3
      Top             =   600
      Width           =   855
   End
   Begin VB.CheckBox ChkDio 
      Caption         =   "DIO 1"
      Height          =   255
      Index           =   1
      Left            =   360
      TabIndex        =   2
      Top             =   360
      Width           =   855
   End
   Begin VB.CheckBox ChkDio 
      Caption         =   "DIO 0"
      Height          =   255
      Index           =   0
      Left            =   360
      TabIndex        =   0
      Top             =   120
      Width           =   855
   End
   Begin VB.Label Label1 
      Caption         =   "Check box to enable DIO to behave as output for counter channel"
      Height          =   735
      Left            =   120
      TabIndex        =   1
      Top             =   1800
      Width           =   2295
   End
End
Attribute VB_Name = "frmDioFunction"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private InitMode As Boolean

Private Sub Form_Load()
    Dim i As Integer
    InitMode = True
    For i = 0 To 5
        ChkDio(i).Value = (bd.DIOMode(i) <> 0) And 1
    Next i
    InitMode = False
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.MnuDioFunction.Checked = False
End Sub

Private Sub ChkDio_Click(Index As Integer)
    If Not InitMode Then bd.DIOMode(Index) = (ChkDio(Index).Value <> 0)
End Sub

