VERSION 5.00
Begin VB.Form frmDioIntEn 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "DIO Interrupt Enables"
   ClientHeight    =   675
   ClientLeft      =   3765
   ClientTop       =   2085
   ClientWidth     =   9915
   DrawStyle       =   6  'Inside Solid
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   675
   ScaleWidth      =   9915
   ShowInTaskbar   =   0   'False
   Begin VB.Frame Frame1 
      Height          =   675
      Index           =   0
      Left            =   6000
      TabIndex        =   43
      Top             =   0
      Width           =   3915
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   15
         Left            =   60
         TabIndex        =   0
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   14
         Left            =   300
         TabIndex        =   1
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   13
         Left            =   540
         TabIndex        =   2
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   12
         Left            =   780
         TabIndex        =   3
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   11
         Left            =   1020
         TabIndex        =   4
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   10
         Left            =   1260
         TabIndex        =   5
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   9
         Left            =   1500
         TabIndex        =   6
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   8
         Left            =   1740
         TabIndex        =   7
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   7
         Left            =   1980
         TabIndex        =   8
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   6
         Left            =   2220
         TabIndex        =   9
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   5
         Left            =   2460
         TabIndex        =   10
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   4
         Left            =   2700
         TabIndex        =   11
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   3
         Left            =   2940
         TabIndex        =   12
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   2
         Left            =   3180
         TabIndex        =   13
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   1
         Left            =   3420
         TabIndex        =   14
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   0
         Left            =   3660
         TabIndex        =   15
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.Line Line3 
         X1              =   2910
         X2              =   2910
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Line Line2 
         X1              =   1950
         X2              =   1950
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Line Line1 
         X1              =   990
         X2              =   990
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "15"
         Height          =   195
         Index           =   2
         Left            =   60
         TabIndex        =   45
         Top             =   420
         Width           =   195
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "0"
         Height          =   195
         Index           =   1
         Left            =   3660
         TabIndex        =   44
         Top             =   420
         Width           =   195
      End
   End
   Begin VB.Frame Frame1 
      Height          =   675
      Index           =   1
      Left            =   2040
      TabIndex        =   46
      Top             =   0
      Width           =   3915
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   31
         Left            =   60
         TabIndex        =   16
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   30
         Left            =   300
         TabIndex        =   17
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   29
         Left            =   540
         TabIndex        =   18
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   28
         Left            =   780
         TabIndex        =   19
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   27
         Left            =   1020
         TabIndex        =   20
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   26
         Left            =   1260
         TabIndex        =   21
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   25
         Left            =   1500
         TabIndex        =   22
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   24
         Left            =   1740
         TabIndex        =   23
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   23
         Left            =   1980
         TabIndex        =   24
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   22
         Left            =   2220
         TabIndex        =   25
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   21
         Left            =   2460
         TabIndex        =   26
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   20
         Left            =   2700
         TabIndex        =   27
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   19
         Left            =   2940
         TabIndex        =   28
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   18
         Left            =   3180
         TabIndex        =   29
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   17
         Left            =   3420
         TabIndex        =   30
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   16
         Left            =   3660
         TabIndex        =   31
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.Line Line6 
         X1              =   2910
         X2              =   2910
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Line Line5 
         X1              =   1950
         X2              =   1950
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Line Line4 
         X1              =   990
         X2              =   990
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "31"
         Height          =   195
         Index           =   5
         Left            =   60
         TabIndex        =   48
         Top             =   420
         Width           =   195
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "16"
         Height          =   195
         Index           =   4
         Left            =   3660
         TabIndex        =   47
         Top             =   420
         Width           =   195
      End
   End
   Begin VB.Frame Frame1 
      Height          =   675
      Index           =   3
      Left            =   0
      TabIndex        =   40
      Top             =   0
      Width           =   1995
      Begin VB.CheckBox chkEdg 
         Caption         =   "X"
         Height          =   195
         Index           =   39
         Left            =   60
         TabIndex        =   32
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Caption         =   "X"
         Height          =   195
         Index           =   38
         Left            =   300
         TabIndex        =   33
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Caption         =   "X"
         Height          =   195
         Index           =   37
         Left            =   540
         TabIndex        =   34
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Caption         =   "X"
         Height          =   195
         Index           =   36
         Left            =   780
         TabIndex        =   35
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Caption         =   "X"
         Height          =   195
         Index           =   35
         Left            =   1020
         TabIndex        =   36
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Caption         =   "X"
         Height          =   195
         Index           =   34
         Left            =   1260
         TabIndex        =   37
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Caption         =   "X"
         Height          =   195
         Index           =   33
         Left            =   1500
         TabIndex        =   38
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkEdg 
         Height          =   195
         Index           =   32
         Left            =   1740
         TabIndex        =   39
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.Line Line9 
         X1              =   990
         X2              =   990
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "39"
         Height          =   195
         Index           =   8
         Left            =   60
         TabIndex        =   42
         Top             =   420
         Width           =   195
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "32"
         Height          =   195
         Index           =   7
         Left            =   1740
         TabIndex        =   41
         Top             =   420
         Width           =   195
      End
   End
End
Attribute VB_Name = "frmDioIntEn"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private InitMode As Boolean

Private Sub Form_Load()
    FormUpdate
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.mnuDioIntEn.Checked = False
End Sub

Private Sub chkEdg_Click(Index As Integer)
    If Not InitMode Then bd.DIOChanIntEnable(Index) = chkEdg(Index).Value
End Sub

Public Sub FormUpdate()
    Dim i As Integer
    InitMode = True
    For i = 0 To 39
        chkEdg(i).Value = bd.DIOChanIntEnable(i) And 1
    Next i
    InitMode = False
End Sub

