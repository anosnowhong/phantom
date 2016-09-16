/******************************************************************************/
/*                                                                            */
/* MODULE  : DEVICES.h                                                        */
/*                                                                            */
/* PURPOSE : Configuration file for hardware devices on each PC.              */
/*                                                                            */
/* DATE    : 24/Jul/2006                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 20/Jul/2006 - Initial Development of module.                     */
/*                                                                            */
/******************************************************************************/

#ifndef DEVICES_H
#define DEVICES_H

/******************************************************************************/

#define DEVICES_FILE "DEVICES.CFG"
#define DEVICES_ITEMS 4

/******************************************************************************/

extern STRING DEVICES_RobotText[DEVICES_ITEMS];
extern STRING DEVICES_OptoTrakText;
extern STRING DEVICES_CyberGloveText[DEVICES_ITEMS];
extern STRING DEVICES_NIDAQText[DEVICES_ITEMS];
extern STRING DEVICES_ATIFTText[DEVICES_ITEMS];
extern STRING DEVICES_DAQFTText[DEVICES_ITEMS];
extern STRING DEVICES_Sensoray626Text[DEVICES_ITEMS];
extern STRING DEVICES_SNMSText;
extern STRING DEVICES_FOBText;
extern STRING DEVICES_LibertyText;

extern int DEVICES_Robot;
extern int DEVICES_OptoTrak;
extern int DEVICES_CyberGlove;
extern int DEVICES_NIDAQ;
extern int DEVICES_ATIFT;
extern int DEVICES_DAQFT;
extern int DEVICES_Sensoray626;
extern int DEVICES_SNMS;
extern int DEVICES_FOB;
extern int DEVICES_Liberty;

/******************************************************************************/

void DEVICES_ConfigSetup( void );
BOOL DEVICES_ConfigRead( void );

BOOL DEVICES( void );

/******************************************************************************/

#endif

