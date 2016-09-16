VERSION 5.00
Begin VB.Form frmBoardAttr 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Board Attributes"
   ClientHeight    =   2655
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   2055
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2655
   ScaleWidth      =   2055
   ShowInTaskbar   =   0   'False
   Begin VB.Frame Frame2 
      Caption         =   "Select"
      Height          =   1635
      Left            =   60
      TabIndex        =   2
      Top             =   60
      Width           =   1935
      Begin VB.TextBox TxtPciSlot 
         Alignment       =   2  'Center
         Height          =   285
         Left            =   1080
         TabIndex        =   7
         Text            =   "0"
         Top             =   1140
         Width           =   615
      End
      Begin VB.TextBox TxtPciBus 
         Alignment       =   2  'Center
         Height          =   285
         Left            =   1080
         TabIndex        =   6
         Text            =   "0"
         Top             =   720
         Width           =   615
      End
      Begin VB.CheckBox ChkIsOpen 
         Caption         =   "Open"
         Height          =   255
         Left            =   540
         TabIndex        =   5
         Top             =   300
         Width           =   735
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "PCI Slot :"
         Height          =   255
         Index           =   1
         Left            =   240
         TabIndex        =   4
         Top             =   1140
         Width           =   735
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "PCI Bus :"
         Height          =   255
         Index           =   0
         Left            =   240
         TabIndex        =   3
         Top             =   720
         Width           =   735
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Error Flags"
      Height          =   795
      Left            =   60
      TabIndex        =   0
      Top             =   1800
      Width           =   1935
      Begin VB.Label LblErrorFlags 
         Alignment       =   2  'Center
         BackColor       =   &H80000005&
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
         Left            =   360
         TabIndex        =   1
         Top             =   360
         Width           =   1215
      End
   End
End
Attribute VB_Name = "frmBoardAttr"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private InitMode As Boolean

Private Sub ChkIsOpen_Click()
    Dim ErrorCode As Long
    If Not InitMode Then
        InitMode = True
        bd.BoardOpen = (ChkIsOpen.Value <> 0)
        If bd.BoardOpen Then
            TxtPciBus.Text = Format$(bd.PCIBus)
            TxtPciSlot.Text = Format$(bd.PCISlot)
        Else
            If (ChkIsOpen.Value <> 0) Then ChkIsOpen.Value = 0
        End If
        InitMode = False
    End If
    frmMain.MenusEnable bd.BoardOpen
    TxtPciBus.Enabled = Not bd.BoardOpen
    TxtPciSlot.Enabled = Not bd.BoardOpen
End Sub

Private Sub Form_Load()
    InitMode = True
    ChkIsOpen.Value = (bd.BoardOpen = True) And 1
    TxtPciBus.Text = Format$(bd.PCIBus)
    TxtPciSlot.Text = Format$(bd.PCISlot)
    InitMode = False
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.MnuBoardAttr.Checked = False
End Sub

Public Sub FormUpdate()
    LblErrorFlags.Caption = StrHex(bd.GetErrors, 8)
End Sub

Private Sub TxtPciBus_Validate(Cancel As Boolean)
    Dim Value As Integer
    On Error GoTo ErrBus
    If InitMode Then Exit Sub
    Value = CInt(TxtPciBus.Text)
    TxtPciBus.Text = Format$(Value)
    bd.PCIBus = Value
    Exit Sub
ErrBus:
    Value = 0
    Resume Next
End Sub

Private Sub TxtPciSlot_Validate(Cancel As Boolean)
    Dim Value As Integer
    On Error GoTo ErrSlot
    If InitMode Then Exit Sub
    Value = CInt(TxtPciSlot.Text)
    TxtPciSlot.Text = Format$(Value)
    bd.PCISlot = Value
    Exit Sub
ErrSlot:
    Value = 0
    Resume Next
End Sub
