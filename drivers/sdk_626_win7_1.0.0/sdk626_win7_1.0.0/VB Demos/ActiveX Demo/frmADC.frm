VERSION 5.00
Begin VB.Form frmADC 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "ADC"
   ClientHeight    =   5925
   ClientLeft      =   7620
   ClientTop       =   4005
   ClientWidth     =   3555
   DrawStyle       =   6  'Inside Solid
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   5925
   ScaleWidth      =   3555
   ShowInTaskbar   =   0   'False
   Begin VB.Frame Frame1 
      Caption         =   "Oversample Control"
      Height          =   675
      Left            =   60
      TabIndex        =   84
      Top             =   5190
      Width           =   3435
      Begin VB.HScrollBar ScrSampleCounts 
         Height          =   255
         LargeChange     =   4
         Left            =   720
         Max             =   32
         Min             =   1
         TabIndex        =   86
         Top             =   300
         Value           =   1
         Width           =   2535
      End
      Begin VB.Label LblSampleCounts 
         Alignment       =   2  'Center
         BorderStyle     =   1  'Fixed Single
         Caption         =   "1"
         Height          =   255
         Left            =   180
         TabIndex        =   85
         Top             =   300
         Width           =   435
      End
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
      Height          =   315
      Index           =   15
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   83
      Top             =   4740
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
      Height          =   315
      Index           =   14
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   82
      Top             =   4440
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
      Height          =   315
      Index           =   13
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   81
      Top             =   4140
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
      Height          =   315
      Index           =   12
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   80
      Top             =   3840
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
      Height          =   315
      Index           =   11
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   79
      Top             =   3540
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
      Height          =   315
      Index           =   10
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   78
      Top             =   3240
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
      Height          =   315
      Index           =   9
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   77
      Top             =   2940
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
      Height          =   315
      Index           =   8
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   76
      Top             =   2640
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
      Height          =   315
      Index           =   7
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   75
      Top             =   2340
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
      Height          =   315
      Index           =   6
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   74
      Top             =   2040
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
      Height          =   315
      Index           =   5
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   73
      Top             =   1740
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
      Height          =   315
      Index           =   4
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   72
      Top             =   1440
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
      Height          =   315
      Index           =   3
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   71
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
      Height          =   315
      Index           =   2
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   70
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
      Height          =   315
      Index           =   1
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   69
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
      Height          =   315
      Index           =   0
      Left            =   2520
      MultiLine       =   -1  'True
      TabIndex        =   66
      Top             =   240
      Width           =   975
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   15
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   65
      Top             =   4740
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   14
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   64
      Top             =   4440
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   13
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   63
      Top             =   4140
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   12
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   62
      Top             =   3840
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   11
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   61
      Top             =   3540
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   10
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   60
      Top             =   3240
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   9
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   59
      Top             =   2940
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   8
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   58
      Top             =   2640
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   7
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   57
      Top             =   2340
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   6
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   56
      Top             =   2040
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   5
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   55
      Top             =   1740
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   4
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   54
      Top             =   1440
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   3
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   53
      Top             =   1140
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   2
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   52
      Top             =   840
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   1
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   51
      Top             =   540
      Width           =   735
   End
   Begin VB.TextBox txtADCResult 
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
      Height          =   315
      Index           =   0
      Left            =   1740
      MultiLine       =   -1  'True
      TabIndex        =   50
      Top             =   240
      Width           =   735
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   15
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   47
      Top             =   4740
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   15
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   46
      Top             =   4740
      Width           =   675
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   15
      Left            =   60
      TabIndex        =   45
      Top             =   4800
      Value           =   -1  'True
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   14
      Left            =   60
      TabIndex        =   44
      Top             =   4500
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   13
      Left            =   60
      TabIndex        =   43
      Top             =   4200
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   12
      Left            =   60
      TabIndex        =   42
      Top             =   3900
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   11
      Left            =   60
      TabIndex        =   41
      Top             =   3600
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   10
      Left            =   60
      TabIndex        =   40
      Top             =   3300
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   9
      Left            =   60
      TabIndex        =   39
      Top             =   3000
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   8
      Left            =   60
      TabIndex        =   38
      Top             =   2700
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   7
      Left            =   60
      TabIndex        =   37
      Top             =   2400
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   6
      Left            =   60
      TabIndex        =   36
      Top             =   2100
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   5
      Left            =   60
      TabIndex        =   35
      Top             =   1800
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   4
      Left            =   60
      TabIndex        =   34
      Top             =   1500
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   3
      Left            =   60
      TabIndex        =   33
      Top             =   1200
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   2
      Left            =   60
      TabIndex        =   32
      Top             =   900
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   1
      Left            =   60
      TabIndex        =   31
      Top             =   600
      Width           =   195
   End
   Begin VB.OptionButton optLast 
      Height          =   195
      Index           =   0
      Left            =   60
      TabIndex        =   30
      Top             =   300
      Width           =   195
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   14
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   29
      Top             =   4440
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   13
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   28
      Top             =   4140
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   12
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   27
      Top             =   3840
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   11
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   26
      Top             =   3540
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   10
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   25
      Top             =   3240
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   9
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   24
      Top             =   2940
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   8
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   23
      Top             =   2640
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   7
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   22
      Top             =   2340
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   6
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   21
      Top             =   2040
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   5
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   20
      Top             =   1740
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   4
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   19
      Top             =   1440
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   3
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   18
      Top             =   1140
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   2
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   17
      Top             =   840
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   1
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   16
      Top             =   540
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListRange 
      Height          =   315
      Index           =   0
      Left            =   1020
      Style           =   2  'Dropdown List
      TabIndex        =   15
      Top             =   240
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   14
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   14
      Top             =   4440
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   13
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   13
      Top             =   4140
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   12
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   12
      Top             =   3840
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   11
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   11
      Top             =   3540
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   10
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   10
      Top             =   3240
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   9
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   9
      Top             =   2940
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   8
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   8
      Top             =   2640
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   7
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   7
      Top             =   2340
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   6
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   6
      Top             =   2040
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   5
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   5
      Top             =   1740
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   4
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   4
      Top             =   1440
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   3
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   3
      Top             =   1140
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   2
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   2
      Top             =   840
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   0
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   1
      Top             =   240
      Width           =   675
   End
   Begin VB.ComboBox cmbPollListChan 
      Height          =   315
      Index           =   1
      Left            =   300
      Style           =   2  'Dropdown List
      TabIndex        =   0
      Top             =   540
      Width           =   675
   End
   Begin VB.Label Label4 
      AutoSize        =   -1  'True
      Caption         =   "Volts"
      Height          =   195
      Left            =   2775
      TabIndex        =   68
      Top             =   0
      Width           =   345
   End
   Begin VB.Label lblHexDec 
      Alignment       =   2  'Center
      AutoSize        =   -1  'True
      Caption         =   "Raw"
      Height          =   195
      Left            =   1935
      TabIndex        =   67
      Top             =   0
      Width           =   345
   End
   Begin VB.Label Label3 
      AutoSize        =   -1  'True
      Caption         =   "Range"
      Height          =   195
      Left            =   1110
      TabIndex        =   49
      Top             =   0
      Width           =   480
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      Caption         =   "Channel"
      Height          =   195
      Left            =   345
      TabIndex        =   48
      Top             =   0
      Width           =   585
   End
End
Attribute VB_Name = "frmADC"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Const EOPL = &H80

Private InitMode            As Boolean              ' Deactivate controls.
Private LastSlot            As Integer              ' Last active poll list slot.
Private ADCData(0 To 15)    As Integer              ' Digitized values from ADC.
Private AccData(0 To 15)    As Long                 ' Accumulated digitized values, used for averaging.
Private AvgCounts           As Integer              ' Number of samples accumulated in AccData[].

Private Sub Form_Load()
    Dim Chan As Integer
    Dim Slot As Integer
    ' Disable hardware state changes.
    InitMode = True
    ' For each SlotList position ...
    For Slot = 0 To 15
        ' Populate ChanNumber combo box.
        cmbPollListChan(Slot).Clear
        For Chan = 0 To 15
            cmbPollListChan(Slot).AddItem Format$(Chan), Chan
        Next Chan
        cmbPollListChan(Slot).ListIndex = Slot
        ' Populate Range combo box.
        cmbPollListRange(Slot).Clear
        cmbPollListRange(Slot).AddItem "10V", 0
        cmbPollListRange(Slot).AddItem "5V", 1
        cmbPollListRange(Slot).ListIndex = 0
    Next Slot
    ' Enable all slots.
    LastSlot = 15
    Me.Show
    optLast(LastSlot).SetFocus
    SetupPollList
    ' Enable hardware state changes.
    InitMode = False
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.mnuADC.Checked = False
End Sub

Private Sub cmbPollListChan_Click(Index As Integer)
    If Not InitMode Then SetupPollList
End Sub

Private Sub cmbPollListRange_Click(Index As Integer)
    If Not InitMode Then SetupPollList
End Sub

Private Sub optLast_Click(Index As Integer)
    If InitMode Then Exit Sub
    LastSlot = Index
    SetupPollList
End Sub

Private Sub SetupPollList()
    Dim Slot As Integer
    ' Insert items into pollList, remove EOPL (End Of Poll List) markers and render unused slots invisible.
    For Slot = 0 To 15
        bd.ADCPollList(Slot) = cmbPollListChan(Slot).ListIndex Or (cmbPollListRange(Slot).ListIndex * &H10)
        txtADCResult(Slot).Visible = (Slot <= LastSlot)
        txtVolts(Slot).Visible = (Slot <= LastSlot)
    Next Slot
    bd.ADCPollList(LastSlot) = bd.ADCPollList(LastSlot) Or EOPL     ' Insert EOPL marker.
End Sub

Private Sub ResetAccumulators()
    Dim Slot As Integer
    For Slot = 0 To 15
        AccData(Slot) = 0
    Next Slot
    AvgCounts = 0
End Sub

Private Sub ScrSampleCounts_Change()
    ResetAccumulators
    LblSampleCounts.Caption = Format$(ScrSampleCounts.Value)
End Sub

Private Sub ScrSampleCounts_Scroll()
    ResetAccumulators
    LblSampleCounts.Caption = Format$(ScrSampleCounts.Value)
End Sub

Public Sub FormUpdate()
    Dim Slot        As Integer
    Dim AvgData     As Integer
    Dim Volts      As String
    ' Digitize and accumulate.
    bd.ADCExecutePollList ADCData(0)
    For Slot = 0 To LastSlot
        AccData(Slot) = AccData(Slot) + ADCData(Slot)
    Next Slot
    AvgCounts = AvgCounts + 1
    ' If sufficient samples have been accumulated ...
    If AvgCounts >= ScrSampleCounts.Value Then
        ' Update displayed values and reset accumulators.
        For Slot = 0 To LastSlot
            AvgData = AccData(Slot) / AvgCounts
            Volts = Format$(AvgData / 32768 * IIf(cmbPollListRange(Slot).ListIndex = 0, 10, 5), "#0.0000")
            txtVolts(Slot).Text = String(4 - InStr(2, Volts, "."), " ") & Volts
            txtADCResult(Slot).Text = StrHex(AvgData, 4)
        Next Slot
        ResetAccumulators
    End If
End Sub
