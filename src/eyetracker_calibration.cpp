#include <motor.h>

int main(int argc, char** argv)
{
	//start device
	EYETRACKER_HMD_Open();
	printf("Starting calibration tool using 'default' parameter...");
	EYETRACKER_HMD_Start();
	//call calibration service
	printf("Input the calibration points number: (Options are 1, 3, 5, 9)\n");
	int num;
	scanf("%d", &num);
	EYETRACKER_HMD_Calibration(num);
	//save calibration file
	printf("Please enter the name for new calibration file, or update default calibration file: \n");
	char name[100];
	scanf("%99s", name);
	EYETRACKER_HMD_SaveCal(name);

	printf("List all the calibration file?(y/n)\n");
	char list;
	list = _getch();
	if (list == 'y')
		EYETRACKER_HMD_ListCal();

	printf("Closing Program...\n");
	EYETRACKER_HMD_Close();
	return 0;
}