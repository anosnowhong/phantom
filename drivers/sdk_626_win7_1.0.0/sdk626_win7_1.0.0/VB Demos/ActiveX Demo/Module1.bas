Attribute VB_Name = "Module1"
Option Explicit

Public frmCounters(0 To 5)  As New frmCounter       ' Counter forms.
Public DioCaptureChan       As Integer              ' Chan number of last detected DIO interrupt (-1 if none detected).
Public CntrIntIndex(0 To 5) As Boolean
Public CntrIntOver(0 To 5)  As Boolean
Public CntrIntCount(0 To 5) As Long
Public bd                   As S626

Public Function StrHex(ByVal Value As Long, ByVal nDigits As Long) As String
    StrHex = "0x" & Right$(String(nDigits - 1, "0") & Hex$(Value), nDigits)
End Function

Public Function StrDec(ByVal Value As Long, ByVal nDigits As Long) As String
    StrDec = Right$(String(nDigits - 1, " ") & Format$(Value), nDigits)
End Function

