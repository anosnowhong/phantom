VERSION 5.00
Begin VB.Form frmDin 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "DIO Inputs"
   ClientHeight    =   675
   ClientLeft      =   90
   ClientTop       =   8880
   ClientWidth     =   11835
   DrawStyle       =   6  'Inside Solid
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   675
   ScaleWidth      =   11835
   ShowInTaskbar   =   0   'False
   Begin VB.Frame Frame1 
      Height          =   675
      Index           =   0
      Left            =   7920
      TabIndex        =   3
      Top             =   0
      Width           =   3915
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   9
         Left            =   1500
         TabIndex        =   24
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   8
         Left            =   1740
         TabIndex        =   23
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   7
         Left            =   1980
         TabIndex        =   22
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   6
         Left            =   2220
         TabIndex        =   21
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   5
         Left            =   2460
         TabIndex        =   20
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   4
         Left            =   2700
         TabIndex        =   19
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   3
         Left            =   2940
         TabIndex        =   18
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   2
         Left            =   3180
         TabIndex        =   17
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   1
         Left            =   3420
         TabIndex        =   16
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   0
         Left            =   3660
         TabIndex        =   15
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   15
         Left            =   60
         TabIndex        =   14
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   14
         Left            =   300
         TabIndex        =   13
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   13
         Left            =   540
         TabIndex        =   12
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   12
         Left            =   780
         TabIndex        =   11
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   11
         Left            =   1020
         TabIndex        =   10
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   10
         Left            =   1260
         TabIndex        =   9
         TabStop         =   0   'False
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
         TabIndex        =   5
         Top             =   420
         Width           =   195
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "0"
         Height          =   195
         Index           =   1
         Left            =   3660
         TabIndex        =   4
         Top             =   420
         Width           =   195
      End
   End
   Begin VB.Frame Frame1 
      Height          =   675
      Index           =   1
      Left            =   3960
      TabIndex        =   6
      Top             =   0
      Width           =   3915
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   21
         Left            =   2460
         TabIndex        =   40
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   20
         Left            =   2700
         TabIndex        =   39
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   19
         Left            =   2940
         TabIndex        =   38
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   18
         Left            =   3180
         TabIndex        =   37
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   17
         Left            =   3420
         TabIndex        =   36
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   16
         Left            =   3660
         TabIndex        =   35
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   31
         Left            =   60
         TabIndex        =   34
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   30
         Left            =   300
         TabIndex        =   33
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   29
         Left            =   540
         TabIndex        =   32
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   28
         Left            =   780
         TabIndex        =   31
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   27
         Left            =   1020
         TabIndex        =   30
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   26
         Left            =   1260
         TabIndex        =   29
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   25
         Left            =   1500
         TabIndex        =   28
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   24
         Left            =   1740
         TabIndex        =   27
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   23
         Left            =   1980
         TabIndex        =   26
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Caption         =   "X"
         Enabled         =   0   'False
         Height          =   195
         Index           =   22
         Left            =   2220
         TabIndex        =   25
         TabStop         =   0   'False
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
         TabIndex        =   8
         Top             =   420
         Width           =   195
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "16"
         Height          =   195
         Index           =   4
         Left            =   3660
         TabIndex        =   7
         Top             =   420
         Width           =   195
      End
   End
   Begin VB.Frame Frame1 
      Height          =   675
      Index           =   3
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   3915
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   41
         Left            =   1500
         TabIndex        =   56
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   40
         Left            =   1740
         TabIndex        =   55
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   39
         Left            =   1980
         TabIndex        =   54
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   38
         Left            =   2220
         TabIndex        =   53
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   37
         Left            =   2460
         TabIndex        =   52
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   36
         Left            =   2700
         TabIndex        =   51
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   35
         Left            =   2940
         TabIndex        =   50
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   34
         Left            =   3180
         TabIndex        =   49
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   33
         Left            =   3420
         TabIndex        =   48
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   32
         Left            =   3660
         TabIndex        =   47
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   47
         Left            =   60
         TabIndex        =   46
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   46
         Left            =   300
         TabIndex        =   45
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   45
         Left            =   540
         TabIndex        =   44
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   44
         Left            =   780
         TabIndex        =   43
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   43
         Left            =   1020
         TabIndex        =   42
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.CheckBox chkDIN 
         Enabled         =   0   'False
         Height          =   195
         Index           =   42
         Left            =   1260
         TabIndex        =   41
         TabStop         =   0   'False
         Top             =   180
         Value           =   2  'Grayed
         Width           =   195
      End
      Begin VB.Line Line9 
         X1              =   2910
         X2              =   2910
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Line Line8 
         X1              =   1950
         X2              =   1950
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Line Line7 
         X1              =   990
         X2              =   990
         Y1              =   600
         Y2              =   120
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "32"
         Height          =   195
         Index           =   7
         Left            =   3660
         TabIndex        =   2
         Top             =   420
         Width           =   195
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "47"
         Height          =   195
         Index           =   6
         Left            =   60
         TabIndex        =   1
         Top             =   420
         Width           =   195
      End
   End
End
Attribute VB_Name = "frmDin"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Load()
    FormUpdate
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.mnuDIn.Checked = False
End Sub

Public Sub FormUpdate()
    Dim i As Integer
    For i = 0 To 47
        chkDIN(i).Value = bd.DIOReadChan(i)
    Next i
End Sub
