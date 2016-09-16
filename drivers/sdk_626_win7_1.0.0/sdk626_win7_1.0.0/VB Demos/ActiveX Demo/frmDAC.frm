VERSION 5.00
Begin VB.Form frmDAC 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "DAC"
   ClientHeight    =   1440
   ClientLeft      =   3945
   ClientTop       =   10890
   ClientWidth     =   7650
   DrawStyle       =   6  'Inside Solid
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   1440
   ScaleWidth      =   7650
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton cmdZero 
      Caption         =   "Zero"
      Height          =   255
      Index           =   3
      Left            =   4620
      TabIndex        =   20
      Top             =   1140
      Width           =   495
   End
   Begin VB.CommandButton cmdZero 
      Caption         =   "Zero"
      Height          =   255
      Index           =   2
      Left            =   4620
      TabIndex        =   19
      Top             =   840
      Width           =   495
   End
   Begin VB.CommandButton cmdZero 
      Caption         =   "Zero"
      Height          =   255
      Index           =   1
      Left            =   4620
      TabIndex        =   18
      Top             =   540
      Width           =   495
   End
   Begin VB.CommandButton cmdZero 
      Caption         =   "Zero"
      Height          =   255
      Index           =   0
      Left            =   4620
      TabIndex        =   17
      Top             =   240
      Width           =   495
   End
   Begin VB.TextBox txtVolts 
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
      Height          =   285
      Index           =   3
      Left            =   6180
      Locked          =   -1  'True
      TabIndex        =   15
      Top             =   1140
      Width           =   975
   End
   Begin VB.TextBox txtValue 
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
      Height          =   285
      Index           =   3
      Left            =   5160
      Locked          =   -1  'True
      TabIndex        =   14
      Top             =   1140
      Width           =   975
   End
   Begin VB.TextBox txtVolts 
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
      Height          =   285
      Index           =   2
      Left            =   6180
      Locked          =   -1  'True
      TabIndex        =   13
      Top             =   840
      Width           =   975
   End
   Begin VB.TextBox txtValue 
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
      Height          =   285
      Index           =   2
      Left            =   5160
      Locked          =   -1  'True
      TabIndex        =   12
      Top             =   840
      Width           =   975
   End
   Begin VB.TextBox txtVolts 
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
      Height          =   285
      Index           =   1
      Left            =   6180
      Locked          =   -1  'True
      TabIndex        =   11
      Top             =   540
      Width           =   975
   End
   Begin VB.TextBox txtValue 
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
      Height          =   285
      Index           =   1
      Left            =   5160
      Locked          =   -1  'True
      TabIndex        =   10
      Top             =   540
      Width           =   975
   End
   Begin VB.TextBox txtVolts 
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
      Height          =   285
      Index           =   0
      Left            =   6180
      Locked          =   -1  'True
      TabIndex        =   9
      Top             =   240
      Width           =   975
   End
   Begin VB.TextBox txtValue 
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
      Height          =   285
      Index           =   0
      Left            =   5160
      Locked          =   -1  'True
      TabIndex        =   8
      Top             =   240
      Width           =   975
   End
   Begin VB.HScrollBar hscrDac 
      Height          =   255
      Index           =   3
      LargeChange     =   250
      Left            =   360
      Max             =   8191
      Min             =   -8191
      TabIndex        =   7
      Top             =   1140
      Width           =   4155
   End
   Begin VB.HScrollBar hscrDac 
      Height          =   255
      Index           =   2
      LargeChange     =   250
      Left            =   360
      Max             =   8191
      Min             =   -8191
      TabIndex        =   6
      Top             =   840
      Width           =   4155
   End
   Begin VB.HScrollBar hscrDac 
      Height          =   255
      Index           =   1
      LargeChange     =   250
      Left            =   360
      Max             =   8191
      Min             =   -8191
      TabIndex        =   5
      Top             =   540
      Width           =   4155
   End
   Begin VB.HScrollBar hscrDac 
      Height          =   255
      Index           =   0
      LargeChange     =   250
      Left            =   360
      Max             =   8191
      Min             =   -8191
      TabIndex        =   0
      Top             =   240
      Width           =   4155
   End
   Begin VB.Label Label 
      AutoSize        =   -1  'True
      Caption         =   "Raw"
      Height          =   195
      Index           =   3
      Left            =   5520
      TabIndex        =   25
      Top             =   0
      Width           =   330
   End
   Begin VB.Label Label 
      AutoSize        =   -1  'True
      Caption         =   "+10"
      Height          =   195
      Index           =   6
      Left            =   3960
      TabIndex        =   24
      Top             =   0
      Width           =   270
   End
   Begin VB.Label Label 
      AutoSize        =   -1  'True
      Caption         =   "-10"
      Height          =   195
      Index           =   2
      Left            =   660
      TabIndex        =   23
      Top             =   0
      Width           =   225
   End
   Begin VB.Label Label 
      AutoSize        =   -1  'True
      Caption         =   "0"
      Height          =   195
      Index           =   4
      Left            =   2385
      TabIndex        =   22
      Top             =   0
      Width           =   90
   End
   Begin VB.Label Label 
      AutoSize        =   -1  'True
      Caption         =   "Volts"
      Height          =   195
      Index           =   1
      Left            =   6540
      TabIndex        =   21
      Top             =   0
      Width           =   345
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      AutoSize        =   -1  'True
      Caption         =   "Ch"
      Height          =   195
      Index           =   0
      Left            =   120
      TabIndex        =   16
      Top             =   0
      Width           =   225
   End
   Begin VB.Label Label4 
      AutoSize        =   -1  'True
      Caption         =   "1"
      Height          =   195
      Left            =   180
      TabIndex        =   4
      Top             =   570
      Width           =   90
   End
   Begin VB.Label Label3 
      AutoSize        =   -1  'True
      Caption         =   "2"
      Height          =   195
      Left            =   180
      TabIndex        =   3
      Top             =   870
      Width           =   90
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      Caption         =   "3"
      Height          =   195
      Left            =   180
      TabIndex        =   2
      Top             =   1170
      Width           =   90
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "0"
      Height          =   195
      Left            =   180
      TabIndex        =   1
      Top             =   240
      Width           =   90
   End
End
Attribute VB_Name = "frmDAC"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private InitMode As Boolean

Private Sub cmdZero_Click(Index As Integer)
    hscrDac(Index).Value = 0
End Sub

Private Sub Form_Load()
    Dim Chan As Integer
    InitMode = True
    For Chan = 0 To 3
        hscrDac(Chan).Value = bd.DACSetpoint(Chan)
        ShowDacValue Chan
    Next Chan
    InitMode = False
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.mnuDAC.Checked = False
End Sub

Private Sub ShowDacValue(ByVal Chan As Integer)
    txtVolts(Chan) = Format((hscrDac(Chan).Value / 8191 * 10), "#0.00000")
    txtValue(Chan) = StrHex(hscrDac(Chan).Value, 4)
End Sub

Private Sub SetDac(Index As Integer)
    bd.DACSetpoint(Index) = hscrDac(Index).Value
    ShowDacValue Index
End Sub

Private Sub hscrDac_Change(Index As Integer)
    If Not InitMode Then SetDac Index
End Sub

Private Sub hscrDac_Scroll(Index As Integer)
    If Not InitMode Then SetDac Index
End Sub
