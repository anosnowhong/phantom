#include <motor.h>
//#include "include/eyetrackercpp_hmd.h"
int main()
{
	//register callback
	EYETRACKER_HMD eye_demo(true);
	eye_demo.EYETRACKER_HMD_Start();
	//start streaming
	int rc;
	
	while (true)
	{
		while (true)
		{
			printf("Press Any Key to Start!\n");
			char init;
			init = _getch();
				
			//stop incase spam command line
			//eye_demo.EYETRACKER_HMD_Close();
			eye_demo.EYETRACKER_HMD_Stop();
			eye_demo.EYETRACKER_HMD_Init();
			printf("Avaiable calibration file:\n");
			if (eye_demo.EYETRACKER_HMD_ListCal() == 0)
			{
				printf("No Calibration file found, starting calbration procedure\n");
				//calibrate with 5 points
				eye_demo.EYETRACKER_HMD_Start();
				rc = eye_demo.EYETRACKER_HMD_Calibration(5);
				char name[20] = "tmp";
				rc = eye_demo.EYETRACKER_HMD_SaveCal(name);
				rc = eye_demo.EYETRACKER_HMD_LoadCal(name);
			}
			else
			{
				//Load cal file with name or start a new calibration
				printf("\nSpecify your choice of calibration file with its name or start NEW calibration by inputting 'new':\n");
				char s[1024];
				scanf("%s", s);
				 
				if (!strcmp(s, "new"))
				{
					eye_demo.EYETRACKER_HMD_Start();
					rc = eye_demo.EYETRACKER_HMD_Calibration(5);
					char name[20] = "tmp";
					rc = eye_demo.EYETRACKER_HMD_SaveCal(name);
					rc = eye_demo.EYETRACKER_HMD_LoadCal(name);
				}
				else
					rc = eye_demo.EYETRACKER_HMD_LoadCal(s);


			}
			if (rc == false)
			{
				printf("Failed need restart the procedure...\n");
				break;
			}
			else
			{ 
				rc = eye_demo.EYETRACKER_HMD_Start();
				if (rc == false)
					return false;
			}
			//output eye postion using current calibration file.


		}
	}
	//calibration procedure or load calibration file

	//output eye position.
	return 0;
}