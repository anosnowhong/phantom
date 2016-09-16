#ifndef EYETRACKERCPP_HMD_
#define EYETRACKERCPP_HMD_

#ifdef __cplusplus
extern "C" {
#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#include <string.h>
#include <windows.h>
#include <sys\timeb.h>
#include <winioctl.h>
#include <sys\types.h>
#include <conio.h> 
#endif
#ifdef __cplusplus
}
#endif

//#include <motor.h>


#include <typedefs.h>
#include <matrix.h>
#include <dataproc.h>
#include <timer.h>

//#include <map>
class EYETRACKER_HMD
{
public:
	EYETRACKER_HMD();
	~EYETRACKER_HMD();
	EYETRACKER_HMD(bool);
	bool EYETRACKER_HMD_Init();
	bool EYETRACKER_HMD_Start();
	bool EYETRACKER_HMD_Stop();

	bool EYETRACKER_HMD_Calibration(int);
	bool EYETRACKER_HMD_SaveCal(char *s);
	bool EYETRACKER_HMD_LoadCal(char *s);
	int  EYETRACKER_HMD_ListCal();

	void EYETRACKER_HMD_EYE_STREAMING();
	void EYETRACKER_HMD_Position2D();

	void EYETRACKER_HMD_Close();

	static void sampleToString(bool, smi_SampleHMDStruct* sample);
private:
	int rc;
	bool streamEyeData = true;
	smi_CallbackDataStruct *frame_cb;
	smi_SampleHMDStruct *frame_sample;

	static bool EYETRACKER_HMD_Status(int, char*);
	//Latency reporter
	TIMER_Frequency EYETRACKER_SampleFrequency;
	TIMER_Frequency EYETRACKER_EventFrequency;

	TIMER_Interval EYETRACKER_CheckRecordingLatency;
	TIMER_Interval EYETRACKER_GetNextDataLatency;
	TIMER_Interval EYETRACKER_GetFloatDataLatency;
	TIMER_Interval EYETRACKER_DataStart;
	TIMER_Interval EYETRACKER_DataStartWaitReady;
	TIMER_Interval EYETRACKER_DataStop;
	TIMER_Interval EYETRACKER_DataStopWaitReady;
	TIMER_Interval EYETRACKER_OpenLatency;
	TIMER_Interval EYETRACKER_CloseLatency;
	TIMER_Interval EYETRACKER_StartLatency;
	TIMER_Interval EYETRACKER_StopLatency;
	void EYETRACKER_HMD_Results();
	void EYETRACKER_HMD_FrameExtract();
	static void CALLBACK eyetracker_cb(smi_CallbackDataStruct *result);
	//static std::map<smi_CallbackDataStruct*, EYETRACKER_HMD*> m_instanceMap;
	//static std::map<smi_SampleHMDStruct*, EYETRACKER_HMD*> m_instanceMap;
};

#endif