#include <string>
#include <iostream>
#include <Eyetracker/iViewHMDAPI.h>
#include <eyetrackercpp_hmd.h>

void CALLBACK EYETRACKER_HMD::eyetracker_cb(smi_CallbackDataStruct *result){
	switch (result->type){
	case SMI_SIMPLE_GAZE_SAMPLE:
		smi_SampleHMDStruct *sample = (smi_SampleHMDStruct*)result->result;
		//can't change streamEyeData in nonstitic function.
		EYETRACKER_HMD::sampleToString(true, sample);		
		//EYETRACKER_HMD *pThis = m_instanceMap[sample];
		//pThis->sampleToString(true, sample);
		break;
	//case SMI_EYE_IMAGE_LEFT:
		//break;
	//case SMI_EYE_IMAGE_RIGHT:
		//break;
	}

}

/*Print out timestamp and eye positon (Terminal use only)*/
void EYETRACKER_HMD::sampleToString(bool streamEyeData,smi_SampleHMDStruct* sample)
{
	/*Avaiable Output
	//<< " iod: " << sample->iod
		//<< " ipd: " << sample->ipd
		//<< " gazeDirection.x: " << sample->gazeDirection.x
		//<< " gazeDirection.y: " << sample->gazeDirection.y 
		//<< " gazeDirection.z: " << sample->gazeDirection.z
		//<< " isValid: "			<< sample->isValid

		//<< " Left PORx: " << sample->left.por.x
		//<< " Left PORy: " << sample->left.por.y
		//<< " left.eyeballCenter.x: " << sample->left.gazeBasePoint.x
		//<< " left.eyeballCenter.y: " << sample->left.gazeBasePoint.y 
		//<< " left.eyeballCenter.z: " << sample->left.gazeBasePoint.z 
		//<< " left.gazeDirection.x: " << sample->left.gazeDirection.x
		//<< " left.gazeDirection.y: " << sample->left.gazeDirection.y
		//<< " left.gazeDirection.z: " << sample->left.gazeDirection.z
		//<< " left.eyeLensDistance: " << sample->left.eyeLensDistance
		//<< " left.eyeScreenDistance: " << sample->left.eyeScreenDistance
		//<< " left.pupilPosition.x: " << sample->left.pupilPosition.x
		//<< " left.pupilPosition.y: " << sample->left.pupilPosition.y
		//<< " left.pupilPosition.z: " << sample->left.pupilPosition.z
		//<< " left.pupilRadius: " << sample->left.pupilRadius

		//<< " Right PORx: " << sample->right.por.x 
		//<< " Right PORy: " << sample->right.por.y
		//<< " right.eyeballCenter.x: " << sample->right.gazeBasePoint.x
		//<< " right.eyeballCenter.y: " << sample->right.gazeBasePoint.y
		//<< " right.eyeballCenter.z: " << sample->right.gazeBasePoint.z
		//<< " right.gazeDirection.x: " << sample->right.gazeDirection.x
		//<< " right.gazeDirection.y: " << sample->right.gazeDirection.y
		//<< " right.gazeDirection.z: " << sample->right.gazeDirection.z
		//<< " right.eyeLensDistance: " << sample->right.eyeLensDistance
		//<< " right.eyeScreenDistance: " << sample->right.eyeScreenDistance
		//<< " right.pupilPosition.x: " << sample->right.pupilPosition.x
		//<< " right.pupilPosition.y: " << sample->right.pupilPosition.y
		//<< " right.pupilPosition.z: " << sample->right.pupilPosition.z
		//<< " right.pupilRadius: " << sample->right.pupilRadius*/
	if (streamEyeData){
		//printf("timestamp: %u \n\r PORx: %G \n\r PORy: %G", sample->timestamp, sample->por.x, sample->por.y);
		std::cout << "timestamp: " << sample->timestamp
			<< " PORx:" << sample->por.x
			<< " PORy:" << sample->por.y
			<< "	\r";
		std::cout.flush();
	}
}

EYETRACKER_HMD::EYETRACKER_HMD():
	EYETRACKER_StopLatency("EYETRACKER_CheckErcordingLatency"),
	EYETRACKER_SampleFrequency("EYETRACKER_SampleFrequency"),
	EYETRACKER_EventFrequency("EYETRACKER_HMD_EventFrequency"),
	EYETRACKER_CheckRecordingLatency("EYETRACKER_HMD_CheckRecordingLatency"),
	EYETRACKER_GetNextDataLatency("EYETRACKER_HMD_GetNextDataLatency"),
	EYETRACKER_GetFloatDataLatency("EYETRACKER_HMD_GetFloatDataLatency"),
	EYETRACKER_DataStart("EYETRACKER_HMD_DataStart"),
	EYETRACKER_DataStartWaitReady("EYETRACKER_HMD_DataStartWaitReady"),
	EYETRACKER_DataStop("EYETRACKER_HMD_DataStop"),
	EYETRACKER_DataStopWaitReady("EYETRACKER_HMD_DataStopWaitReady"),
	EYETRACKER_OpenLatency("EYETRACKER_HMD_OpenLatency"),
	EYETRACKER_CloseLatency("EYETRACKER__HMD_CloseLatency"),
	EYETRACKER_StartLatency("EYETRACKER_HMD_StartLatency")

{
	//m_instanceMap.insert(std::make_pair(frame_sample, this));
	//Blank
}

EYETRACKER_HMD::EYETRACKER_HMD(bool register_cb):
	EYETRACKER_StopLatency("EYETRACKER_CheckErcordingLatency"),
	EYETRACKER_SampleFrequency("EYETRACKER_SampleFrequency"),
	EYETRACKER_EventFrequency("EYETRACKER_HMD_EventFrequency"),
	EYETRACKER_CheckRecordingLatency("EYETRACKER_HMD_CheckRecordingLatency"),
	EYETRACKER_GetNextDataLatency("EYETRACKER_HMD_GetNextDataLatency"),
	EYETRACKER_GetFloatDataLatency("EYETRACKER_HMD_GetFloatDataLatency"),
	EYETRACKER_DataStart("EYETRACKER_HMD_DataStart"),
	EYETRACKER_DataStartWaitReady("EYETRACKER_HMD_DataStartWaitReady"),
	EYETRACKER_DataStop("EYETRACKER_HMD_DataStop"),
	EYETRACKER_DataStopWaitReady("EYETRACKER_HMD_DataStopWaitReady"),
	EYETRACKER_OpenLatency("EYETRACKER_HMD_OpenLatency"),
	EYETRACKER_CloseLatency("EYETRACKER__HMD_CloseLatency"),
	EYETRACKER_StartLatency("EYETRACKER_HMD_StartLatency")
{
	

	if (register_cb == true)
	{ 
		EYETRACKER_HMD_Init();
	}

}

EYETRACKER_HMD::~EYETRACKER_HMD()
{
	//m_instanceMap.erase(frame_sample);
	smi_quit();
}

/*An error check function*/
bool EYETRACKER_HMD::EYETRACKER_HMD_Status(int rc_input, char *description)
{
	char smi_info[10000]="SMI_INFO: ";
	strcat(smi_info, smi_rcToString(rc_input));
	strcat(smi_info, description);
	strcat(smi_info, "\n");
	if (rc_input == SMI_RET_SUCCESS)
	{ 
		printf(smi_info);
		return true;
	}
	else
	{ 
		printf(smi_info);
		return false;
	}
}

void EYETRACKER_HMD::EYETRACKER_HMD_Close()
{
	EYETRACKER_CloseLatency.Before();
	EYETRACKER_HMD_Status(smi_quit(),"(SMI_QUIT)");
	EYETRACKER_CloseLatency.After();
}

/*register callback funciton.This device do not need check if it was opened,
as it is a part of Oculus. Just need check if callback registered success or not.

RETURN: TRUE if successfully register callback function.
		FALSE when failed to register and print out error info.
*/
bool EYETRACKER_HMD::EYETRACKER_HMD_Init()
{

	EYETRACKER_OpenLatency.Before();
	rc = smi_setCallback(this->eyetracker_cb);
	EYETRACKER_OpenLatency.After();

	return EYETRACKER_HMD_Status(rc, "(Register Callback)");
}

/*Start cablibration procedure, need input number of calibration points*/
bool EYETRACKER_HMD::EYETRACKER_HMD_Calibration(int num)
{
	smi_CalibrationHMDStruct *calibrationHMDStruct;
	smi_createCalibrationHMDStruct(&calibrationHMDStruct);
	calibrationHMDStruct->backgroundColor->blue = 0.5;
	calibrationHMDStruct->backgroundColor->green = 0.5;
	calibrationHMDStruct->backgroundColor->red = 0.5;
	calibrationHMDStruct->foregroundColor->blue = 1.0;
	calibrationHMDStruct->foregroundColor->green = 1.0;
	calibrationHMDStruct->foregroundColor->red = 1.0;
	switch (num)
	{
	case 1: 
		calibrationHMDStruct->type = SMI_ONE_POINT_CALIBRATION;
		break;
	case 3:
		calibrationHMDStruct->type = SMI_THREE_POINT_CALIBRATION;
		break;
	case 5:
		calibrationHMDStruct->type = SMI_FIVE_POINT_CALIBRATION;
		break;
	case 9:
		calibrationHMDStruct->type = SMI_NINE_POINT_CALIBRATION;
		break;
	}
	rc = smi_setupCalibration(calibrationHMDStruct);
	bool tmp1 = EYETRACKER_HMD_Status(rc,"(Setup Calibration)");
	rc = smi_calibrate();
	bool tmp2 = EYETRACKER_HMD_Status(rc,"(Calibration Procedure)");
	return (tmp1&&tmp2);
}

bool EYETRACKER_HMD::EYETRACKER_HMD_SaveCal(char *s)
{
	//temporarily disable position output to screen
	streamEyeData = false;
	
	//Not sure where the calibration is saved.
	rc = smi_saveCalibration(s);
	streamEyeData = true;

	return EYETRACKER_HMD_Status(rc,"(Save Calibration)");
}

bool EYETRACKER_HMD::EYETRACKER_HMD_LoadCal(char *s)
{
	streamEyeData = false;

	printf("%s", s);
	rc = smi_loadCalibration(s);
	streamEyeData = true;
	
	return EYETRACKER_HMD_Status(rc, "(Load Calibration)");
}

int EYETRACKER_HMD::EYETRACKER_HMD_ListCal()
{
	const char *cal_list = smi_getAvailableCalibrations();
	int amount = 1;
	for (int i = 0; i < strlen(cal_list); i++)
	{
		if (cal_list[i] == ',')
			amount++;
	}
	
	printf("%s", cal_list);
	return amount;
}

/*
Setting parameters and start streaming using this parameter. Read SMI Doc for more info.

*/
bool EYETRACKER_HMD::EYETRACKER_HMD_Start()
{
	smi_TrackingParameterStruct params;
	memset(&params, 0, sizeof(smi_TrackingParameterStruct));
	params.mappingDistance = 1500;//default

	EYETRACKER_DataStart.Before();
	rc = smi_startStreaming(false, &params);
	EYETRACKER_DataStart.After();

	return EYETRACKER_HMD_Status(rc,"(Start Streaming)");
}

/*This stop function will set callback function to NULL*/
bool EYETRACKER_HMD::EYETRACKER_HMD_Stop()
{

	EYETRACKER_StopLatency.Before();
	rc = smi_stopStreaming();
	EYETRACKER_StopLatency.After();

	return EYETRACKER_HMD_Status(rc, "(Stop Streaming)");
}


void EYETRACKER_HMD::EYETRACKER_HMD_EYE_STREAMING()
{

}

void EYETRACKER_HMD::EYETRACKER_HMD_Results()
{
	EYETRACKER_SampleFrequency.Results();
	EYETRACKER_EventFrequency.Results();

	EYETRACKER_CheckRecordingLatency.Results();
	EYETRACKER_GetNextDataLatency.Results();
	EYETRACKER_GetFloatDataLatency.Results();

	EYETRACKER_DataStart.Results();
	EYETRACKER_DataStartWaitReady.Results();
	EYETRACKER_DataStop.Results();
	EYETRACKER_DataStopWaitReady.Results();

	EYETRACKER_OpenLatency.Results();
	EYETRACKER_CloseLatency.Results();
	EYETRACKER_StartLatency.Results();
	EYETRACKER_StopLatency.Results();
}

void EYETRACKER_HMD::EYETRACKER_HMD_FrameExtract()
{
	/*
	frame->timestamp;
	frame->por.x;
	frame->por.y;
	frame->left.por.x;
	frame->left.por.y;
	frame->right.por.x;
	frame->right.por.y;
	frame->right.pupilRadius;
	frame->right.pupilPosition.x;
	frame->right.pupilPosition.y;
	frame->right.pupilPosition.z;
	*/
}