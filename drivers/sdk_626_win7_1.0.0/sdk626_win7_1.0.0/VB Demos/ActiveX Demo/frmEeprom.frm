VERSION 5.00
Begin VB.Form frmEeprom 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Serial EEPROM"
   ClientHeight    =   2280
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   3615
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   2280
   ScaleWidth      =   3615
   ShowInTaskbar   =   0   'False
   Begin VB.Frame Frame1 
      Caption         =   "Data"
      Height          =   1095
      Left            =   60
      TabIndex        =   1
      Top             =   1110
      Width           =   3495
      Begin VB.HScrollBar ScrData 
         Height          =   255
         Left            =   120
         Max             =   255
         TabIndex        =   3
         Top             =   690
         Width           =   3255
      End
      Begin VB.CommandButton BtnWrite 
         Caption         =   "Store"
         Height          =   345
         Left            =   2340
         TabIndex        =   2
         Top             =   210
         Width           =   915
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
         TabIndex        =   8
         Top             =   270
         Width           =   555
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
   End
   Begin VB.Frame Frame 
      Caption         =   "Address"
      Height          =   1035
      Left            =   60
      TabIndex        =   0
      Top             =   60
      Width           =   3495
      Begin VB.HScrollBar ScrAdrs 
         Height          =   255
         Left            =   120
         Max             =   127
         TabIndex        =   5
         Top             =   630
         Width           =   3255
      End
      Begin VB.Label LblFunction 
         Alignment       =   2  'Center
         BorderStyle     =   1  'Fixed Single
         Height          =   255
         Left            =   840
         TabIndex        =   7
         Top             =   300
         Width           =   2535
      End
      Begin VB.Label LblAdrs 
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
         Left            =   120
         TabIndex        =   6
         Top             =   300
         Width           =   615
      End
   End
End
Attribute VB_Name = "frmEeprom"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub SelectAdrs()
    LblAdrs.Caption = "0x" & Right$("0" & Hex$(ScrAdrs.Value), 2)
    ScrData.Value = bd.EEPROM(ScrAdrs.Value)
    Dim t As String
    Select Case ScrAdrs.Value
        Case 0: t = "PCI SubDeviceID, LSB"
        Case 1: t = "PCI SubDeviceID, MSB"
        Case 2: t = "PCI SubVendorID, LSB"
        Case 3: t = "PCI SubVendorID, MSB"
        Case &H40: t = "ADC offset"
        Case &H41: t = "ADC 10V gain"
        Case &H42: t = "ADC 5V gain"
        Case &H50: t = "DAC0 offset"
        Case &H51: t = "DAC1 offset"
        Case &H52: t = "DAC2 offset"
        Case &H53: t = "DAC3 offset"
        Case &H60: t = "DAC0 gain"
        Case &H61: t = "DAC1 gain"
        Case &H62: t = "DAC2 gain"
        Case &H63: t = "DAC3 gain"
        Case Default: t = ""
    End Select
    LblFunction.Caption = t
    BtnWrite.Enabled = (ScrAdrs.Value > 3)  ' Don't change PCI attributes!!!
    ScrData.Enabled = (ScrAdrs.Value > 3)
End Sub

Private Sub BtnWrite_Click()
    bd.EEPROM(ScrAdrs.Value) = ScrData.Value
End Sub

Private Sub Form_Load()
    SelectAdrs
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.MnuEeprom.Checked = False
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
End Sub

Private Sub ScrData_Change()
    ChangeData
End Sub

Private Sub ScrData_Scroll()
    ChangeData
End Sub
