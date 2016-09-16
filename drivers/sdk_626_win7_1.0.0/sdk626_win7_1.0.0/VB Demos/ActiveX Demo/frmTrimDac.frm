VERSION 5.00
Begin VB.Form frmTrimDac 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Trim DACs"
   ClientHeight    =   2745
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   3615
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   2745
   ScaleWidth      =   3615
   ShowInTaskbar   =   0   'False
   Begin VB.Frame Frame 
      Caption         =   "Address"
      Height          =   1455
      Left            =   60
      TabIndex        =   5
      Top             =   30
      Width           =   3495
      Begin VB.HScrollBar ScrAdrs 
         Height          =   255
         Left            =   120
         Max             =   10
         TabIndex        =   6
         Top             =   1020
         Width           =   3255
      End
      Begin VB.Label Lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "Function :"
         Height          =   225
         Index           =   2
         Left            =   120
         TabIndex        =   12
         Top             =   660
         Width           =   735
      End
      Begin VB.Label Lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "Physical :"
         Height          =   225
         Index           =   1
         Left            =   1740
         TabIndex        =   11
         Top             =   330
         Width           =   735
      End
      Begin VB.Label Lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "Logical :"
         Height          =   225
         Index           =   0
         Left            =   60
         TabIndex        =   10
         Top             =   330
         Width           =   795
      End
      Begin VB.Label LblPhysicalAdrs 
         Alignment       =   2  'Center
         BorderStyle     =   1  'Fixed Single
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
         Left            =   2520
         TabIndex        =   9
         Top             =   300
         Width           =   615
      End
      Begin VB.Label LblLogicalAdrs 
         Alignment       =   2  'Center
         BorderStyle     =   1  'Fixed Single
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
         Left            =   900
         TabIndex        =   8
         Top             =   300
         Width           =   615
      End
      Begin VB.Label LblFunction 
         Alignment       =   2  'Center
         BorderStyle     =   1  'Fixed Single
         Height          =   255
         Left            =   900
         TabIndex        =   7
         Top             =   630
         Width           =   2295
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Data"
      Height          =   1095
      Left            =   60
      TabIndex        =   0
      Top             =   1560
      Width           =   3495
      Begin VB.CommandButton BtnWrite 
         Caption         =   "Save"
         Height          =   345
         Left            =   2400
         TabIndex        =   2
         Top             =   210
         Width           =   735
      End
      Begin VB.HScrollBar ScrData 
         Height          =   255
         Left            =   120
         Max             =   255
         TabIndex        =   1
         Top             =   690
         Width           =   3255
      End
      Begin VB.Label LblData 
         Alignment       =   2  'Center
         BorderStyle     =   1  'Fixed Single
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
         Index           =   0
         Left            =   120
         TabIndex        =   4
         Top             =   270
         Width           =   615
      End
      Begin VB.Label LblData 
         Alignment       =   2  'Center
         BorderStyle     =   1  'Fixed Single
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
         Index           =   1
         Left            =   900
         TabIndex        =   3
         Top             =   270
         Width           =   555
      End
   End
End
Attribute VB_Name = "frmTrimDac"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private InitMode As Boolean

Private Sub Form_Load()
    SelectAdrs
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.MnuTrimDac.Checked = False
End Sub

Private Sub SelectAdrs()
    Dim t As String
    Dim p As Integer
    ' Set Data Value scroll bar without writing to the trimdac.
    InitMode = True
    ScrData.Value = bd.TrimDacSetpoint(ScrAdrs.Value)
    InitMode = False
    ' Map logical trimdac channel number into trimdac device's physical channel number
    ' and symbolic function name.
    Select Case ScrAdrs.Value
        Case 0:  p = 10: t = "ADC offset"
        Case 1:  p = 9:  t = "ADC 10V gain"
        Case 2:  p = 8:  t = "ADC 5V gain"
        Case 3:  p = 3:  t = "DAC0 offset"
        Case 4:  p = 2:  t = "DAC1 offset"
        Case 5:  p = 7:  t = "DAC2 offset"
        Case 6:  p = 6:  t = "DAC3 offset"
        Case 7:  p = 1:  t = "DAC0 gain"
        Case 8:  p = 0:  t = "DAC1 gain"
        Case 9:  p = 5:  t = "DAC2 gain"
        Case 10: p = 4:  t = "DAC3 gain"
    End Select
    LblLogicalAdrs.Caption = Format$(ScrAdrs.Value)
    LblPhysicalAdrs.Caption = Format$(p)
    LblFunction.Caption = t
End Sub

Private Sub BtnWrite_Click()
    Dim a As Integer
    ' Map logical trimdac channel number into affiliated EEPROM address.
    Select Case ScrAdrs.Value
        Case 0: a = &H40
        Case 1: a = &H41
        Case 2: a = &H42
        Case 3: a = &H50
        Case 4: a = &H51
        Case 5: a = &H52
        Case 6: a = &H53
        Case 7: a = &H60
        Case 8: a = &H61
        Case 9: a = &H62
        Case 10: a = &H63
    End Select
    ' Write trimdac's new default value to EEPROM.
    bd.EEPROM(a) = ScrData.Value
End Sub

Private Sub ScrAdrs_Change()
    SelectAdrs
End Sub

Private Sub ScrAdrs_Scroll()
    SelectAdrs
End Sub

Private Sub ChangeData()
    LblData(0).Caption = "0x" & Right$("0" & Hex$(ScrData.Value), 2)
    LblData(1).Caption = Right$("  " & Format$(ScrData.Value), 3)
    If Not InitMode Then bd.TrimDacSetpoint(ScrAdrs.Value) = ScrData.Value
End Sub

Private Sub ScrData_Change()
    ChangeData
End Sub

Private Sub ScrData_Scroll()
    ChangeData
End Sub
