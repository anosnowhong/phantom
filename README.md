by Hongru Yang 2016

#Phantom System: Overview
Phantom System is used for tracking people's hand and eye movements, all the data is stored during experiments for later analysis.

###Compoments
The whole system is made up of following compoments:

* Robot arm 
* Oculus(with calibration cam) + Eyetracker sensor
* Amp (produce force)
* S626 board (the middleware communicate with robot arm)

If you start developing on a new computer make sure all the comopments are connected well and all the driviers are installed.

Driver for S626 board is not available any more, it can be found on "PhantomSystem/developing/drivers"

Please read **Source Code Compile Guide** below before starting using the software.

### Other Documentations: 
* [SMI](http://www.smivision.com/en/gaze-and-eye-tracking-systems/support/software-download.html)(eyetracker)
* [Oculus SDK 0.8.0](https://developer3.oculus.com/documentation/pcsdk/latest/)


##Software and Tools (these exe files are not included in the repository, they will be generated after you compile the whole project) 
###utility.exe
A command line tool that has lots of options, mainly used to reset robot arm, print out movement data for debugging.

**usage example:**
```
#Print out position, velocity and acceleration 
utility.exe /P:ROBOT_BIG /D:P 
```

###oculus.exe (source code can be found in "PhantomSystem/developing/src/oculus_demo.cpp")
A demo that combine robot arm and oculus vision together, you can move the robot arm and view the movement in oculus.

**usage example:**

```
#start the demo
oculus.exe /R:ROBOT_BIG
```
###eye_tracker.exe (source code can be found in "PhantomSystem/developing/src/oculus_eyetracker.cpp")
This demo is based on oculus.exe but added eye tracker feature, a ball will be generated at the direction you are looking at. 

**usage example:**
```
eye_tracker.exe /R:ROBOT_BIG
```

###eyetracker_calibration.exe
A tool used for calibrate eye tracker. The result of the calibration will be saved internally by SMI api,
 which means that you won't be able to find any file externally. Instead, when you save your calibration info, 
 it will show all the exisiting calibration info that you can use during the experiment.
 
In the experiment, calibration is loaded in DeviceStart(void) in which EYETRACKER_HMD_Open(name) is called where the "name" 
specifices the name of the calibaration you saved. If no argument it will just load "default" calibration info.

###DynamicLearning.exe
Run experiment using this program. For arguements see "Parameters" function.

#Phantom System: FAQ
##How to commit change of codes I made to the VS repository?
See youtube tutorial for VS team service
##If I add a new developer to VS team service, how can I control the access rights?
Creat new user account in Windows and assign folder access rights, 
if he is an experiment developer he can only have access to the cpp file of the experiment.  
##What to do when programming a new experiment?
See **Create New Subproject** below. For libraries, use the same libraries as "DyanmicLearning.cpp"
##What are the limitations in hardwares and softwares
The system runs only in Windows 8 (no guarantee whether it will work on Win 7 or 10). And Phantom+Oculus setup works only in Oculus SDK 0.8.0 and 
SMI+Oculus set has limitation decribed in their manual, but we found that it also works in the current setup.
##What are all the folders in E:/PhantomSystem?
* backups: hard copy of preivous codes and executables (VS team service also provide roll back)
* current: current executable (this folder is what experimenters will be using)
* data: all the output files from the experiment will be stored in this folder
* developing: codes under developement. Once you finish the development, move executables to "current"
    * bin: executables
    * build: project/solution filed made by CMake
    * build_script: batch procedure to run CMake
    * Document: all documentations
    * drivers
    * lib: lib files
    * libsource: source code for libraries
    * src: source for executables
* m-code: Matlab code for data analysis
* rederence: some additional descriptions about coding an experiment

# Source Code Compile Guide 
All the source code in `libsource` folder is to compile a static lib (motor.lib), however this lib is already provided in the lib folder. 
So you don't need to compile that. If you make any change in those libs and compile, the new motor.lib will be used. 

##Software preparation 
* Install visual studio and find 'vcvars32.bat' in the vc folder. There maybe a serveral vs version. Find the one that contains 'vcvars32.bat'), 
open "Developer Command Prompt" (this may have alreay been installed when you installed the VS), and drag 'vcvars32.bat' int the command window to 
setup the compile environment. 
* Install s626 sdk from "PhantomSystem/developing/drivers/sdk_626_win7_1.0.0/sdk_626_win7_1.0.0/wind7_drivers/win7x64/dpinst.exe". You may want to do x32 when your Windows is in 32 bit. 
* The whole project is using CMake (a program that automatically sets all the libraries and additional information required for compile), 
so install CMake (https://cmake.org/) before compiling. 
* Go and run "PhantomSystem/developing/build_script/run.bat", which will do the folowing:
    * Delete all the exisiting files in the "PhantomSystem/developing/build" folder and subfolders (note that this won't delete any code as all the source codes are outside of this "build" folder)
    * run cmake command and generate project/solution files in the "build" folder
* When CMake is done, you will see "phantom_eyetracker.sln". You should be able to compile when VS is open. 
* Dependence(put these file in Debug or Release folder after you have compiled the program, these are needed to execute.): 
```
OAPI.dll, glut32.dll, s626.dll 
iViewHMDAPI_x64.dll  ```
All these dll's can be found in "Developing\bin"

##Hardware preparation 
* Plugin the 626 board, install the driver 
* GO "Computer->properties->Device Manager->Sound, video and game controllers" and in "General" tab check "Location" (e.g. PCI bus 4) 
* Open "developing/bin/calib/ROBOT_BIG.cfg" file and modify the second number in "Contoller", which is from the "Location" information you found (e.g. PCI bus 4 -> 0x40000)

##Create New Subproject
* This project is maintained using CMake, new subproject can be created by adding the following example code in "developing/CMakelists.txt":

```CMake
#Example: when you created demo1.cpp that depends on the library_1 and library_2
add_executable(demo1 ${CMAKE_CURRENT_SOURCE_DIR}/src/demo1.cpp)
target_link_libraries(demo1 dependencies_library_1 library_2)
```

# Phantom Calibration

As encoders in phantom measure only relative angles, we need to define one fixed configuration (let's say it is CalConfig) of the robot for calibration. 
We can simply put the robot to that configuration and run the calibration, which will reset encoder readings.
Once done, all the encoder reading will be 'offseted' so that their reading at home configuration (the home configuration when the angle is $(0,0,90)$ degrees
, let's say it is HomeConfig) will be $(0, -43.5, 51.1)$ (this is pre-measured location of HomeConfig with respect to the base frame of the phantom). The offset is already defined in "angle" in "developing/bin/calib/general/ROBOT_BIG.cal" file, 
which specifes the difference in encoder readings between CalConfig and HomeConfig. It would be unnecessary to change these offset value, 
but if you have to follow the procedure below:

1. Set the "baseXYZ" and "angle" in "ROBOT_BIG.cal" file to all ZERO
2. Reset encoder at home position (for how to reset encoder see below)
3. Move the robot to CalConfig and fix 
4. Run "utility.exe /P:ROBOT_BIG /D:A" and write down first three values (Raw angles in degrees)
5. Compare the angle (degree) in HomeConfig, that is $(0,0,90)$, write down the difference (e.g. if the angle is $(-88,6,96)$, the difference is $(-88,6,6)$).
6. Convert the difference angle to radian and update the "angle" values in "ROBOT_BIG.cal".

## How to reset encoder
It is recommended that you rest encode everytime you start your experiment.

1. Move the robot to the CalConfig and fix
2. Go and run "utility.exe /P:ROBOT_BIG /E" and follow the option displayed

