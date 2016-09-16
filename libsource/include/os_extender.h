/******************************************************************
**
**             os_extender.h
**
** Header file for nt os extender library
**
** Copyright 1996-1997 SensAble Technologies Inc.
** All rights reserved.
******************************************************************/
#include <windows.h>

/* The function initOSExtender must be the first thing done in your
program.
To start your servo loop call startServoLoop(callback, userData)
where callback is a function of the form:
	SCHEDULER_CALLBACK callback(void *userData)
and data userData is a pointer to user data to be passed to
the callback when it is called.
To stop scheduling call stopServoLoop. */
 
/* return type for callback function */
#define SCHEDULER_CALLBACK void FAR PASCAL

#ifdef __cplusplus
extern "C" {
#endif

 
/* functions */
int initOSExtender();
int startServoLoop(void (FAR PASCAL *userCallback)(void *), void *userData);
void stopServoLoop();

#ifdef __cplusplus
}
#endif
