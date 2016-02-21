/******************************************************************************/
/*                                                                            */
/* MODULE  : DEVICES.cpp                                                      */
/*                                                                            */
/* PURPOSE : Configuration file for hardware devices on each PC.              */
/*                                                                            */
/* DATE    : 24/Jul/2006                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 24/Jul/2006 - Initial Development of module.                     */
/*                                                                            */
/******************************************************************************/

#include <motor.h>

/******************************************************************************/

STRING DEVICES_RobotText[DEVICES_ITEMS]={ "","","","" };
STRING DEVICES_OptoTrakText="No";
STRING DEVICES_CyberGloveText[DEVICES_ITEMS]={ "","","","" };
STRING DEVICES_NIDAQText[DEVICES_ITEMS]={ "","","","" };
STRING DEVICES_ATIFTText[DEVICES_ITEMS]={ "","","","" };
STRING DEVICES_DAQFTText[DEVICES_ITEMS]={ "","","","" };
STRING DEVICES_Sensoray626Text[DEVICES_ITEMS]={ "","","","" };
STRING DEVICES_SNMSText="No";
STRING DEVICES_FOBText="No";
STRING DEVICES_LibertyText="No";

int DEVICES_Robot=0;
int DEVICES_OptoTrak=0;
int DEVICES_CyberGlove=0;
int DEVICES_NIDAQ=0;
int DEVICES_ATIFT=0;
int DEVICES_DAQFT=0;
int DEVICES_Sensoray626=0;
int DEVICES_SNMS=0;
int DEVICES_FOB=0;
int DEVICES_Liberty=0;

/******************************************************************************/

void DEVICES_ConfigSetup( void )
{
    CONFIG_reset();

    CONFIG_set("Robot",DEVICES_RobotText,DEVICES_ITEMS);
    CONFIG_set("OptoTrak",DEVICES_OptoTrakText);
    CONFIG_set("CyberGlove",DEVICES_CyberGloveText,DEVICES_ITEMS);
    CONFIG_set("NIDAQ",DEVICES_NIDAQText,DEVICES_ITEMS);
    CONFIG_set("ATIFT",DEVICES_ATIFTText,DEVICES_ITEMS);
    CONFIG_set("DAQFT",DEVICES_DAQFTText,DEVICES_ITEMS);
    CONFIG_set("Sensoray626",DEVICES_Sensoray626Text,DEVICES_ITEMS);
    CONFIG_set("SNMS",DEVICES_SNMSText);
    CONFIG_set("FOB",DEVICES_FOBText);
    CONFIG_set("Liberty",DEVICES_LibertyText);
}

/******************************************************************************/

BOOL DEVICES_ConfigRead( void )
{
char *path;

    DEVICES_ConfigSetup();

    if( (path=FILE_Path(DEVICES_FILE)) == NULL )
    {
        return(FALSE);
    }

    if( !CONFIG_read(path) )
    {
        return(FALSE);
    }

    DEVICES_Robot = STR_count(DEVICES_RobotText,DEVICES_ITEMS);
    DEVICES_OptoTrak = STR_Bool(DEVICES_OptoTrakText);
    DEVICES_CyberGlove = STR_count(DEVICES_CyberGloveText,DEVICES_ITEMS);
    DEVICES_NIDAQ = STR_count(DEVICES_NIDAQText,DEVICES_ITEMS);
    DEVICES_ATIFT = STR_count(DEVICES_ATIFTText,DEVICES_ITEMS);
    DEVICES_DAQFT = STR_count(DEVICES_DAQFTText,DEVICES_ITEMS);
    DEVICES_Sensoray626 = STR_count(DEVICES_Sensoray626Text,DEVICES_ITEMS);
    DEVICES_SNMS = STR_Bool(DEVICES_SNMSText);
    DEVICES_FOB = STR_Bool(DEVICES_FOBText);
    DEVICES_Liberty = STR_Bool(DEVICES_LibertyText);

    return(TRUE);
}

/******************************************************************************/

BOOL DEVICES( void )
{
BOOL ok;

    ok = DEVICES_ConfigRead();

    printf("DEVICES() %s.\n",STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

