/******************************************************************************/
/*                                                                            */
/* MODULE  : ROBOT-PHANTOM.h                                                  */
/*                                                                            */
/* PURPOSE : Map old PHANTOM function names to new ROBOT module.              */
/*                                                                            */
/* DATE    : 13/Oct/2004                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 13/Oct/2004 - Initial development.                               */
/*                                                                            */
/******************************************************************************/

#ifndef ROBOT_PHANTOM_H
#define ROBOT_PHANTOM_H

/******************************************************************************/

#define PHANTOM_INVALID       ROBOT_INVALID

#define PHANTOM_DOF           ROBOT_DOF
#define PHANTOM_3D            ROBOT_3D
#define PHANTOM_2D            ROBOT_2D

#define PHANTOM_DEBUG         ROBOT_DEBUG

#define PHANTOM_Open          ROBOT_Open
#define PHANTOM_Close         ROBOT_Close
#define PHANTOM_CloseAll      ROBOT_CloseAll
#define PHANTOM_Start         ROBOT_Start
#define PHANTOM_Stop          ROBOT_Stop

#define PHANTOM_Info          ROBOT_Info
#define PHANTOM_PanicOn2Off   ROBOT_PanicOn2Off
#define PHANTOM_PanicOff2On   ROBOT_PanicOff2On
#define PHANTOM_PanicNow      ROBOT_PanicNow

#define PHANTOM_Cooling       ROBOT_Cooling
#define PHANTOM_Started       ROBOT_Started
#define PHANTOM_Safe          ROBOT_Safe
#define PHANTOM_Enabled       ROBOT_Activated
#define PHANTOM_Panic         ROBOT_Panic
#define PHANTOM_Ramped        ROBOT_Ramped
#define PHANTOM_Robot         ROBOT_Robot
#define PHANTOM_Ping          ROBOT_Ping

#define PHANTOM_EnabledNo2Yes PHANTOM_JustActivated
#define PHANTOM_EnabledYes2No ROBOT_JustDeactivated

#define PHANTOM_EncoderReset  ROBOT_EncoderReset  
#define PHANTOM_MotorReset    ROBOT_MotorReset

#define PHANTOM_RampValue     ROBOT_RampValue
#define PHANTOM_RampZero      ROBOT_RampZero
#define PHANTOM_RampFlag      ROBOT_RampFlag

#define PHANTOM_AnglesRaw     ROBOT_AnglesRaw
#define PHANTOM_Angles        ROBOT_Angles

#define PHANTOM_PosnRaw       ROBOT_PosnRaw
#define PHANTOM_Posn          ROBOT_Posn
#define PHANTOM_Marker        ROBOT_Marker

/******************************************************************************/

#define PHANTOM_FT_Installed  ROBOT_FT_Installed
#define PHANTOM_FT_Opened     ROBOT_FT_Opened
#define PHANTOM_FT_Open       ROBOT_FT_Open
#define PHANTOM_FT_Close      ROBOT_FT_Close
#define PHANTOM_FT_Read       ROBOT_FT_Read               
#define PHANTOM_FT_BiasReset  ROBOT_FT_BiasReset
#define PHANTOM_FT_BiasWait   ROBOT_FT_BiasWait

/******************************************************************************/

#endif
