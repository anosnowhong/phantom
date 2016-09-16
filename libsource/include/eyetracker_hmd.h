#ifndef EYETRACKER_HMD_
#define EYETRACKER_HMD_
#include <motor.h>

BOOL EYETRACKER_HMD_Open();
//start with default calibration file
BOOL EYETRACKER_HMD_Start();
//start with specified calibration file
BOOL EYETRACKER_HMD_Start(char *name);
void EYETRACKER_HMD_Stop();

bool EYETRACKER_HMD_Calibration(int);
bool EYETRACKER_HMD_SaveCal(char *s);
bool EYETRACKER_HMD_LoadCal(char *s);
int  EYETRACKER_HMD_ListCal();

void EYETRACKER_HMD_EYE_STREAMING();
void EYETRACKER_HMD_Position2D();

void EYETRACKER_HMD_Close();
void sampleToString(bool, smi_SampleHMDStruct* sample);

BOOL EYETRACKER_HMD_Status(int, char*);
void EYETRACKER_HMD_Results();
void CALLBACK eyetracker_cb(smi_CallbackDataStruct *result);

BOOL EYETRACKER_HMD_FrameNext(double &TimeStamp, double EyeXY[], BOOL &ready);
void EYETRACKER_HMD_FrameExtract_Left(smi_SampleHMDStruct *frame, double &TimeStamp, double EyeXY[], double &PupilSize);
void EYETRACKER_HMD_FrameExtract(smi_SampleHMDStruct *frame, double &TimeStamp, double EyeXY[]);
//void EYETRACKER_HMD_FrameNext(double, double, BOOL);
//smi_SampleHMDStruct frame_sample;

void EYETRACKER_HMD_TF2Oculus(smi_SampleHMDStruct *data);

BOOL EYETRACKER_HMD_DATA(smi_SampleHMDStruct *);
BOOL EYETRACKER_HMD_RAW_DATA(smi_SampleHMDStruct *);
FSAMPLE hmd2eyelink_data();
#endif