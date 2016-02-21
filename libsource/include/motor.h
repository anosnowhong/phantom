/******************************************************************************/
/*                                                                            */ 
/* MODULE  : motor.h                                                          */ 
/*                                                                            */ 
/* PURPOSE : General unclude file for C applications.                         */ 
/*                                                                            */ 
/* DATE    : 26/Apr/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V2.0  JNI 26/Apr/2000 - Development taken over.                            */ 
/*                                                                            */ 
/******************************************************************************/

#ifndef MOTOR_H
#define MOTOR_H

// Added this for Visual Studio 8
#define _CRT_SECURE_NO_DEPRECATE 1

#define _WIN32_WINNT 0x400

/***********************************/
/* Standard Visual C++ Includes... */
/***********************************/
#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#include <string.h>
#include <windows.h>
#include <sys\timeb.h>
#include <winioctl.h>
#include <sys\types.h>
#include <conio.h> 
#include <io.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>
#include <direct.h>
#include <time.h>
#include <process.h>

/***********************************/
/* Nortern Digital OptoTrak...     */
/***********************************/
#include <ndhost.h>           // LIBRARY: oapi.lib
#include <ndtypes.h>          // LIBRARY: oapi.lib
#include <ndopto.h>           // LIBRARY: oapi.lib

/***********************************/
/* National Instruments DAQ...     */
/***********************************/
#include <nidaq.h>            // LIBRARY: nidaq32.lib
#include <nidaqerr.h>
#include <nidaqcns.h>

/***********************************/
/* SpaceWare input library...      */
/***********************************/
#include <si.h>               // LIBRARY: siapp{s,m}t.lib (s=single-thead; m=multi-thread)

/***********************************/
/* Cambridge Electronic Design...  */
/***********************************/
#include <use1401.h>          // LIBRARY: use1432.lib

/***********************************/
/* GLEW...                         */
/***********************************/
#ifndef WIN_XP

#include <assert.h>
#include <glew.h>
#undef main

/***********************************/
/* Oculus Rift HDM...              */
/***********************************/

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <Extras/OVR_CAPI_Util.h>
#include <Extras/OVR_Math.h>
#include <Extras/OVR_StereoProjection.h>

extern "C" void ovrhmd_EnableHSWDisplaySDKRender( ovrHmd hmd, ovrBool enabled );

#endif

/***********************************/
/* Open GL / GLUT...               */
/***********************************/
#include <glut.h>             // LIBRARY: glut32.lib

/***********************************/
/* Polhemus Liberty...             */
/***********************************/
#include <PDI.h>

/***********************************/
/* EyeLink-1000...                 */
/***********************************/
#include <gdi_expt.h>

/***********************************/
/* In-House Includes...            */
/***********************************/
#include <typedefs.h>         // General types and definitions.
#include <version.h>          // LIBRARY: motor.lib MODULE: version.obj
#include <mem.h>              // LIBRARY: motor.lib MODULE: mem.obj
#include <matrix.h>           // LIBRARY: motor.lib MODULE: matrix.obj
#include <dataproc.h>         // LIBRARY: motor.lib MODULE: dataproc.obj
#include <timer.h>            // LIBRARY: motor.lib MODULE: timer.obj
#include <isa.h>              // LIBRARY: motor.lib MODULE: isa.obj
#include <spmx.h>             // LIBRARY: motor.lib MODULE: spmx.obj
#include <opto.h>             // LIBRARY: motor.lib MODULE: opto.obj,optobuff.obj,optofile.obj
#include <general.h>          // LIBRARY: motor.lib MODULE: general.obj
#include <winfit.h>           // LIBRARY: motor.lib MODULE: winfit.obj        
#include <winfit2.h>          // LIBRARY: motor.lib MODULE: winfit2.obj        
#include <kalmanfilter.h>     // LIBRARY: motor.lib MODULE: kalmanfilter.obj
#include <ekf.h>              // LIBRARY: motor.lib MODULE: ekf.obj
#include <sensoray.h>         // LIBRARY: motor.lib MODULE: sensoray.obj
#include <phantomisa.h>       // LIBRARY: motor.lib MODULE: phantomisa.obj
#include <controller.h>       // LIBRARY: motor.lib MODULE: controller.obj
#include <temptrak.h>         // LIBRARY: motor.lib MODULE: temptrak.obj
#include <asensor.h>          // LIBRARY: motor.lib MODULE: asensor.obj
#include <ramper.h>           // LIBRARY: motor.lib MODULE: ramper.obj        
#include <robot-object.h>     // LIBRARY: motor.lib MODULE: robot-object.obj
#include <robot.h>            // LIBRARY: motor.lib MODULE: robot.obj
#include <atift.h>            // LIBRARY: motor.lib MODULE: atift.obj
#include <ced.h>              // LIBRARY: motor.lib MODULE: ced.obj
#include <config.h>           // LIBRARY: motor.lib MODULE: config.obj
#include <metaconfig.h>       // LIBRARY: motor.lib MODULE: metaconfig.obj
#include <matdat.h>           // LIBRARY: motor.lib MODULE: matdat.obj
#include <datafile.h>         // LIBRARY: motor.lib MODULE: datafile.obj
#include <graphics.h>         // LIBRARY: motor.lib MODULE: graphics.obj
#include <oculus.h>           // LIBRARY: motor.lib MODULE: oculus.obj
#include <mmtimer.h>          // LIBRARY: motor.lib MODULE: mmtimer.obj
#include <looptask.h>         // LIBRARY: motor.lib MODULE: looptask.obj
#include <fob.h>              // LIBRARY: motor.lib MODULE: fob.obj
#include <cyberglove.h>       // LIBRARY: motor.lib MODULE: cyberglove.obj
#include <str.h>              // LIBRARY: motor.lib MODULE: str.obj
#include <optorigid.h>        // LIBRARY: motor.lib MODULE: optorigid.obj
#include <goggles.h>          // LIBRARY: motor.lib MODULE: goggles.obj
#include <trigger.h>          // LIBRARY: motor.lib MODULE: trigger.obj
#include <cmdarg.h>           // LIBRARY: motor.lib MODULE: cmdarg.obj
#include <com.h>              // LIBRARY: motor.lib MODULE: com.obj
#include <kb.h>               // LIBRARY: motor.lib MODULE: kb.obj
#include <atexit.h>           // LIBRARY: motor.lib MODULE: atexit.obj
#include <file.h>             // LIBRARY: motor.lib MODULE: file.obj        
#include <stim.h>             // LIBRARY: motor.lib MODULE: stim.obj        
#include <snms.h>             // LIBRARY: motor.lib MODULE: snms.obj        
#include <ni-daq.h>           // LIBRARY: motor.lib MODULE: ni-daq.obj        
#include <mouse.h>            // LIBRARY: motor.lib MODULE: mouse.obj        
#include <liberty.h>          // LIBRARY: motor.lib MODULE: liberty.obj        
#include <tracking.h>         // LIBRARY: motor.lib MODULE: tracking.obj        
#include <beeper.h>           // LIBRARY: motor.lib MODULE: beeper.obj        
#include <waveplay.h>         // LIBRARY: motor.lib MODULE: waveplay.obj        
#include <button.h>           // LIBRARY: motor.lib MODULE: button.obj        
#include <devices.h>          // LIBRARY: motor.lib MODULE: devices.obj        
#include <permutelist.h>      // LIBRARY: motor.lib MODULE: permutelist.obj        
#include <pmove.h>            // LIBRARY: motor.lib MODULE: pmove.obj        
#include <EyeLink1000.h>      // LIBRARY: motor.lib MODULE: EyeLink1000.obj        
#include <EyeT.h>             // LIBRARY: motor.lib MODULE: EyeT.obj        

/***********************************/
/* GLM library (Graphical OBJs)... */
/***********************************/
#include <glm.h>

/******************************************************************************/

#endif  MOTOR_H

