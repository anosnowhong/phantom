#include <motor.h>

//Latency reporter
TIMER_Frequency EYETRACKER_SampleFrequency("EYETRACKER_SampleFrequency");
TIMER_Frequency  EYETRACKER_EventFrequency("EYETRACKER_HMD_EventFrequency");

TIMER_Interval EYETRACKER_StopLatency("EYETRACKER_CheckErcordingLatency");
TIMER_Interval EYETRACKER_CheckRecordingLatency("EYETRACKER_HMD_CheckRecordingLatency");
TIMER_Interval EYETRACKER_GetNextDataLatency("EYETRACKER_HMD_GetNextDataLatency");
TIMER_Interval EYETRACKER_GetFloatDataLatency("EYETRACKER_HMD_GetFloatDataLatency");
TIMER_Interval EYETRACKER_DataStart("EYETRACKER_HMD_DataStart");
TIMER_Interval EYETRACKER_DataStartWaitReady("EYETRACKER_HMD_DataStartWaitReady");
TIMER_Interval EYETRACKER_DataStop("EYETRACKER_HMD_DataStop");
TIMER_Interval EYETRACKER_DataStopWaitReady("EYETRACKER_HMD_DataStopWaitReady");
TIMER_Interval EYETRACKER_OpenLatency("EYETRACKER_HMD_OpenLatency");
TIMER_Interval EYETRACKER_CloseLatency("EYETRACKER__HMD_CloseLatency");
TIMER_Interval EYETRACKER_StartLatency("EYETRACKER_HMD_StartLatency");

BOOL EYETRACKER_HMD_OpenFlag = FALSE;
BOOL EYETRACKER_HMD_StartFlag = FALSE;

smi_SampleHMDStruct frame_sample;
smi_CallbackDataStruct frame_cb;
FSAMPLE transfer2eyelink;

//config parameters
smi_TrackingParameterStruct params;



int EYETRACKER_FrameCount = 0;

void CALLBACK eyetracker_cb(smi_CallbackDataStruct *result){
	switch (result->type){
	case SMI_SIMPLE_GAZE_SAMPLE:
	{
		smi_SampleHMDStruct *sample = (smi_SampleHMDStruct*)result->result;
		frame_sample = *sample;
		frame_cb = *result;
		//printf("=======%G\n", sample->left.por.x);
		break;
	}
	case SMI_EYE_IMAGE_LEFT:
	{
		printf("got left eye image.\n");
		break;
	}
	case SMI_EYE_IMAGE_RIGHT:
	{
		printf("got left eye image.\n");
		break;
	}

	}
}

/*Print out timestamp and eye positon (Terminal use only)*/
void sampleToString(bool streamEyeData, smi_SampleHMDStruct* sample)
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
	/*
	if (streamEyeData){
	//printf("timestamp: %u \n\r PORx: %G \n\r PORy: %G", sample->timestamp, sample->por.x, sample->por.y);
	std::cout << "timestamp: " << sample->timestamp
	<< " PORx:" << sample->por.x
	<< " PORy:" << sample->por.y
	<< "	\r";
	std::cout.flush();
	}
	*/
}

/*An error check function*/
BOOL EYETRACKER_HMD_Status(int rc_input, char *description)
{
	char smi_info[10000] = "SMI_INFO: ";
	strcat(smi_info, smi_rcToString(rc_input));
	strcat(smi_info, description);
	strcat(smi_info, "\n");
	if (rc_input == SMI_RET_SUCCESS)
	{
		printf(smi_info);
		return 1;
	}
	else
	{
		printf(smi_info);
		return 0;
	}
}

void EYETRACKER_HMD_Close()
{
	EYETRACKER_CloseLatency.Before();

	EYETRACKER_HMD_Status(smi_quit(), "(SMI_QUIT)");

	EYETRACKER_CloseLatency.After();

	EYETRACKER_HMD_OpenFlag = FALSE;
}

/*register callback funciton.This device do not need check if it was opened,
as it is a part of Oculus. Just need check if callback registered success or not.

RETURN: TRUE if successfully register callback function.
FALSE when failed to register and print out error info.
*/
BOOL EYETRACKER_HMD_Open()
{
	int rc_hmd;

	if (EYETRACKER_HMD_OpenFlag)
	{
		return TRUE;
	}

	EYETRACKER_OpenLatency.Before();

	if (smi_checkHardware())
	{
		rc_hmd = smi_setCallback(eyetracker_cb);
	}
	else
	{
		printf("Checks if our SDK is installed, our Hardware is available and the correct Oculus Runtime is installed.\n");
		EYETRACKER_HMD_Close();
		return FALSE;
	}

	EYETRACKER_OpenLatency.After();

	EYETRACKER_HMD_OpenFlag = TRUE;

	return EYETRACKER_HMD_Status(rc_hmd, "(Register Callback)");
}

void EYETRACKER_HMD_Results()
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

/*
Setting parameters and start streaming using this parameter. Read SMI Doc for more info.
*/
BOOL EYETRACKER_HMD_Start()
{
	int rc_hmd;

	if (!EYETRACKER_HMD_OpenFlag)
		return FALSE;

	if (EYETRACKER_HMD_StartFlag)
		return TRUE;

	memset(&params, 0, sizeof(smi_TrackingParameterStruct));
	params.mappingDistance = 1500;//default
	EYETRACKER_StartLatency.Before();
	rc_hmd = smi_startStreaming(false, &params);
	EYETRACKER_StartLatency.After();
	EYETRACKER_SampleFrequency.Reset();
	EYETRACKER_EventFrequency.Reset();

	//load default calibration file (latest calibration call without specify the file name)
	rc_hmd = EYETRACKER_HMD_LoadCal("default");

	EYETRACKER_HMD_StartFlag = EYETRACKER_HMD_Status(rc_hmd, "(Start Streaming)");

	return EYETRACKER_HMD_StartFlag;
}

BOOL EYETRACKER_HMD_Start(char name[])
{	
	if (!EYETRACKER_HMD_OpenFlag)
		return FALSE;

	if (EYETRACKER_HMD_StartFlag)
		return TRUE;

	int rc_hmd;
	//config parameters
	memset(&params, 0, sizeof(smi_TrackingParameterStruct));
	params.mappingDistance = 1500;//default
	EYETRACKER_StartLatency.Before();
	rc_hmd = smi_startStreaming(false, &params);
	EYETRACKER_StartLatency.After();
	EYETRACKER_SampleFrequency.Reset();
	EYETRACKER_EventFrequency.Reset();

	//load default calibration file (latest calibration call without specify the file name)
	rc_hmd = EYETRACKER_HMD_LoadCal(name);

	EYETRACKER_HMD_StartFlag = EYETRACKER_HMD_Status(rc_hmd, "(Start Streaming)");

	return EYETRACKER_HMD_StartFlag;
}

/*This stop function will set callback function to NULL*/
void EYETRACKER_HMD_Stop()
{
	int rc_hmd;

	if (!EYETRACKER_HMD_OpenFlag & EYETRACKER_HMD_StartFlag)
		return;

	EYETRACKER_StopLatency.Before();

	rc_hmd = smi_stopStreaming();

	EYETRACKER_StopLatency.After();

	EYETRACKER_HMD_StartFlag = FALSE;
	EYETRACKER_HMD_Status(rc_hmd, "(Stop Streaming)");
}

/*Start cablibration procedure, need input number of calibration points*/
bool EYETRACKER_HMD_Calibration(int num)
{
	int rc_hmd;
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
	rc_hmd = smi_setupCalibration(calibrationHMDStruct);
	bool tmp1 = EYETRACKER_HMD_Status(rc_hmd, "(Setup Calibration)");
	rc_hmd = smi_calibrate();
	bool tmp2 = EYETRACKER_HMD_Status(rc_hmd, "(Calibration Procedure)");
	return (tmp1&&tmp2);
}

bool EYETRACKER_HMD_SaveCal(char *s)
{
	int rc_hmd;
	//temporarily disable position output to screen
	//streamEyeData = false;

	//Not sure where the calibration is saved.
	rc_hmd = smi_saveCalibration(s);
	//streamEyeData = true;

	return EYETRACKER_HMD_Status(rc_hmd, "(Save Calibration)");
}

bool EYETRACKER_HMD_LoadCal(char *s)
{
	int rc_hmd;

	rc_hmd = smi_loadCalibration(s);

	return EYETRACKER_HMD_Status(rc_hmd, "(Load Calibration)");
}

int EYETRACKER_HMD_ListCal()
{
	const char *cal_list = smi_getAvailableCalibrations();
	int amount = 1;
	for (int i = 0; i < strlen(cal_list); i++)
	{
		if (cal_list[i] == ',')
			amount++;
	}

	printf("%s\n", cal_list);
	return amount;
}

void EYETRACKER_HMD_EYE_STREAMING()
{

}

//extract data that needed, e.g.
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
void EYETRACKER_HMD_FrameExtract_Left(smi_SampleHMDStruct *frame, double &TimeStamp, double EyeXY[], double &PupilSize)
{
	TimeStamp = (double)frame->timestamp;
	EyeXY[0] = (double)frame->left.por.x;
	EyeXY[1] = (double)frame->left.por.y;
	PupilSize = (double)frame->left.pupilRadius;
}

void EYETRACKER_HMD_FrameExtract(smi_SampleHMDStruct *frame, double &TimeStamp, double EyeXY[])
{
	TimeStamp = (double)frame->timestamp;
	EyeXY[0] = (double)frame->left.por.x;
	EyeXY[1] = (double)frame->left.por.y;
	//PupilSize = (double)frame->left.pupilRadius;
}

BOOL EYETRACKER_HMD_FrameNext(double &TimeStamp, double EyeXY[], BOOL &ready)
{
	int rc_hmd;
	BOOL ok;

	ready = FALSE;
	if (!EYETRACKER_HMD_OpenFlag & EYETRACKER_HMD_StartFlag)
		return FALSE;

	EYETRACKER_GetFloatDataLatency.Before();
	smi_SampleHMDStruct get_data;
	ok = EYETRACKER_HMD_DATA(&get_data);
	EYETRACKER_GetFloatDataLatency.After();
	EYETRACKER_HMD_FrameExtract(&frame_sample, TimeStamp, EyeXY);
	ready = TRUE;

	return ok;
}

//transform to oculus hmd coordinate system by using the info provided by oculus.
void EYETRACKER_HMD_TF2Oculus(smi_SampleHMDStruct *data)
{
	int rc;
	ovrPosef oculus_pose;
	//transform to hmd oculus (inverse x and z coordinate)
	data->gazeDirection.x = -data->gazeDirection.x;
	data->gazeDirection.z = -data->gazeDirection.z;
	rc = OCULUS_GetPose(oculus_pose);
	if (rc)
	{
		//printf("SMI_INFO: GetPose Success! Start transforming...\n");
		Matrix4f tf;
		Matrix4f invert_tf;
		float scale = 0.0;
		Vector4f gaze_direction(data->gazeDirection.x, data->gazeDirection.y, data->gazeDirection.z, 1.0);
		Vector4f new_gaze_direction;
		OCULUS_GetViewProjection(tf, scale);
		/*
		for (int i = 0; i < 4; i++)
		{
		for (int j = 0; j < 4; j++)
		{
		printf("%G ", tf.M[i][j]);
		}
		printf("\n");
		}
		*/
		//printf("gaze raw: %G,%G,%G\n", gaze_direction.x,gaze_direction.y,gaze_direction.z);

		//use view matrix get orentation
		//use inverted matrix transform (0,0,0) in view space to world space
		invert_tf = tf.Inverted();
		new_gaze_direction = invert_tf.Transform(gaze_direction);
		new_gaze_direction = new_gaze_direction / new_gaze_direction.w;
		//printf("transformed %G,%G,%G,%G\n", new_gaze_direction.x, new_gaze_direction.y, new_gaze_direction.z, new_gaze_direction.w);

		//scale
		new_gaze_direction.x /= scale;// gaze_direction.x / scale;
		new_gaze_direction.y /= scale;// gaze_direction.y / scale;
		new_gaze_direction.z /= scale;// gaze_direction.z / scale;
		//printf("gaze scale :%G,%G,%G\n", new_gaze_direction.x,new_gaze_direction.y,new_gaze_direction.z);
		//rotation
		new_gaze_direction.x = -new_gaze_direction.x;
		new_gaze_direction.y = -new_gaze_direction.y;
		//rotation x 90
		Vector4f final_direction;
		final_direction.x = new_gaze_direction.x;
		final_direction.y = new_gaze_direction.z;
		final_direction.z = -new_gaze_direction.y;

		//update the data
		data->gazeDirection.x = final_direction.x;
		data->gazeDirection.y = final_direction.y;
		data->gazeDirection.z = final_direction.z;


	}

	else
		printf("SMI_INFO: Failed to Get Tracking Data.(OCULUS get Pose return FALSE!)\n");
}

BOOL EYETRACKER_HMD_DATA(smi_SampleHMDStruct *data)
{
	//copy whole data incase any change 
	*data = frame_sample;
	//transform data
	EYETRACKER_HMD_TF2Oculus(data);

	return TRUE;
}

//raw data directly get form sensor
BOOL EYETRACKER_HMD_RAW_DATA(smi_SampleHMDStruct *data)
{
	*data = frame_sample;
	return TRUE;
}

FSAMPLE hmd2eyelink_data(){
	FSAMPLE elink;
	/*
	smi_SampleHMDStruct eye_cp_data;
	//smi_CallbackDataStruct hmdcb_cp_data;

	//copy whole data incase changing
	eye_cp_data = frame_sample;
	//hmdcb_cp_data = frame_cb;

	elink.time = eye_cp_data.timestamp;//[ns]
	//elink.type = ;
	//elink.flags

	//elink.px[0] = ;
	//elink.px[1] = ;
	//elink.py[0] =
	//elink.py[1] =
	//elink.hx[0] =
	//elink.hx[1] =
	//elink.hy[0] =
	//elink.hy[1] =
	//elink.pa[0]
	//elink.pa[1]

	//this for left eye(ref from eyelink API), eyelink can only track one eye at one time
	elink.gx[0] = eye_cp_data.por.x;
	elink.gy[0] = eye_cp_data.por.y;
	//this for right eye(ref from eyelink API), eyelink can only track one eye at one time
	elink.gx[1] = eye_cp_data.por.x;
	elink.gy[1] = eye_cp_data.por.y;

	//elink.rx =
	//elink.ry =
	//elink.status
	//elink.input
	*/
	return elink;
}
