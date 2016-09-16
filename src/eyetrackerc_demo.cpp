#include <motor.h>

// forward declaration
bool get();
// define a list to get validation points/fixations respectively
smi_Vec2d * validationPointList;
smi_Vec2d * fixationPointList;

bool apiCallRC(int rc_input, char *description)
{
	char smi_info[10000] = "SMI_INFO: ";
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

void outputHelp(){
}

//
// some information is commented out just because it will spam the command line
//
void sampleToString(smi_SampleHMDStruct * sample){
	if (true) {
			printf("Timestamp: %llu \t PORx: %f PORy: %f\n",sample->timestamp);
		fflush(stdout);
	}
}

void CALLBACK myCallback(smi_CallbackDataStruct * result){
	switch (result->type){
	case SMI_SIMPLE_GAZE_SAMPLE:
	{ // check the type
		smi_SampleHMDStruct * sample = (smi_SampleHMDStruct*)result->result; // cast the result
		sampleToString(sample);
		break;
	}
	case SMI_EYE_IMAGE_LEFT:
	{
		break;
	}
	case SMI_EYE_IMAGE_RIGHT:
	{
		break;
	}
	}
}

int main(int argc, const char* argv[]){


	EYETRACKER_HMD_Init();

	//outputHelp();

	EYETRACKER_HMD_Start();
	EYETRACKER_HMD_ListCal();
	EYETRACKER_HMD_LoadCal("tmp");

	while (1)
	{
		sampleToString(&test_data());
		Sleep(1000);
	}
// start listening for keyboard input
	get();

	return 0;
}

bool get()
{
	char c;
	while (true)
	{
		c = _getch(); // waiting for keyboard input

		if (c == 'q') {
			EYETRACKER_HMD_Close();
			return true; // terminate the application
		}
		if (c == 'v'){
			//apiCallRC(smi_validation());
		}
		if (c == 'b'){
			//apiCallRC(smi_quantitativeValidation(&validationPointList, &fixationPointList, true, true, 0));
		}
		if (c == '1'){
			//cout << endl << "smi_Calibrate (1 point): ";
			EYETRACKER_HMD_Calibration(1);
		}
		if (c == '3'){
			//cout << endl << "smi_Calibrate (3 points): ";
			EYETRACKER_HMD_Calibration(3);
		}
		if (c == '5'){
			//cout << endl << "smi_Calibrate (5 points): ";
			EYETRACKER_HMD_Calibration(5);
		}
		if (c == '9') {
			//cout << endl << "smi_Calibration (9 points): ";
			EYETRACKER_HMD_Calibration(9);
		}
		if (c == 'n'){
			//cout << endl << "smi_ResetCalibration: ";
			//apiCallRC(smi_resetCalibration());
		}
		if (c == 'e'){
			static bool showeyeImages = false;
			showeyeImages = !showeyeImages;
			if (showeyeImages){
				//cout << endl << "smi_ShowEyeImageMonitor: " << endl;
				smi_showEyeImageMonitor();
			}
			else{
				//cout << endl << "smi_HideEyeImageMonitor: " << endl;
				smi_hideEyeImageMonitor();
			}
		}
		if (c == 's'){
			//cout << endl << "smi_startEyeImageStreaming: " << endl;
			//apiCallRC(smi_startEyeImageStreaming());
		}
		if (c == 'u'){
			//cout << endl << "smi_stopEyeImageStreaming: " << endl;
			//apiCallRC(smi_stopEyeImageStreaming());
		}
		if (c == 'h') {
			/*
			streamEyeData = false;
			//getline(cin, name);
			char * _name = new char[name.length() + 1];
			strcpy(_name, name.c_str());
			apiCallRC(smi_saveCalibration(_name));
			delete[] _name;
			streamEyeData = true;
			*/
		}
		if (c == 'j') {
			const char * avCalibrations = smi_getAvailableCalibrations();
		}
		if (c == 'k') {
			/*
			streamEyeData = false;
			char *_name = new char[name.length() + 1];
			strcpy(_name, name.c_str());
			apiCallRC(smi_loadCalibration(_name));
			delete[] _name;
			streamEyeData = true;
			*/
		}
		if (c == 'm') {
			long long ts = smi_getServerTime();
		}
	}
}