Attribute VB_Name = "ModDemo"
Option Explicit

' Kernel functions:
Public Declare Sub Sleep Lib "kernel32" (ByVal milliseconds As Long)

Public Sub InterruptCallback()
    ' ISR CALLBACK FUNCTION.
    FrmDemo.InterruptHandler    ' Call application interrupt handler.
End Sub

