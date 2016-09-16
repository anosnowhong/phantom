#Phantom System
Phantom System is used for tracking people's hand and eye movements, all the data is stored during experiments for later analysis.

###Compoments
The whole system is made up of following compoments:

* Robot arm 
* Oculus(with calibration cam) + Eyetracker sensor
* Amp (produce force)
* S626 board (the middleware communicate with robot arm)

If you start developing on a new computer make sure all the comopments are connected well and all the driviers are installed.


Please read Source Code Compile Guide before starting using the software.

### Other Documentations: 
* [SMI](http://www.smivision.com/en/gaze-and-eye-tracking-systems/support/software-download.html)(eyetracker)
* [Oculus SDK 0.8.0](https://developer3.oculus.com/documentation/pcsdk/latest/)
* [Compile Guide]()(in Documents folder)
* [Calibration Procedure]()(If you screw up the config file!!!)

##Software and Tools
###utility.exe
A command line tool that has lots of options, mainly used for reset robot arm, print out movement data for debuging.

**usage example:**
```
#Print out position, velocity and acceleration 
utility.exe /P:ROBOT_BIG /D:P 
#Reset 
utility.exe /P:ROBOT_BIG /E
```

###oculus.exe
A demo that combine robot arm and oculus vision together, you can move the robot arm and view the movement in oculus.

**usage example:**

```
#start the demo
oculus.exe /R:ROBOT_BIG
```
###eye_tracker.exe
This demo is based on oculus.exe but added eye tracker feature, a new ball will be generate at the direction you are looking at. 

**usage example:**
```
eye_tracker.exe /R:ROBOT_BIG
```

###eyetracker_calibration.exe
A tool used for calibrate eye tracker, you can save the calibration and list available calibration files.

###DynamicLearning.exe
Run experiment using this program.

**usage example:**
```

```

##For Developer

###motor library
This library contains all the api that you may need during developing, for convenient all the source code are compiled and generate only this library.
Here are some instructions if you want to modify the source code.


* eyetracker_hmd.cpp 
> A higher level api for eye tracker in oculus.

* Oculus.cpp
> start device, define scene, view options and so on, usually don't need call the functions in this file directly.

* graphics.cpp
> define lots of primitive shape, more parameters to rendering a scene. A higher level wapper for oculus.


















