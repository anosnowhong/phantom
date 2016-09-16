/* stereo.h */

#ifndef STEREO_H
#define STEREO_H

/*
 * These two variables define the height of each stereo field that is
 * displayed, and the offset (from the bottom of the screen) of the
 * top one, both in pixels.
 *
 * They are initialized in stereo_on; this is done this way in
 * anticipation of a future getgdesc()-like call to return their
 * values, to avoid having to re-code all of the stereo demos as soon
 * as differing, incompatible stereo hardware comes out.
 */
extern int YMAXSTEREO;
extern int YOFFSET;

/* void stereopersp(fovy, aspect, near, far, conv, eye)
 *
 * fovy, aspect, near, far - work just like the 'perspective' command
 *
 * conv - the plane at which the left and right image will converge on the 
 *	  screen.  If conv is equal to the near plane, the stereo image will
 *	  appear to be behind the plane of the screen.  If conv is set to 
 *	  the far plane, the stereo image will appear in front of the screen.
 *
 * eye - the distance (in world coordinates) the eye is off-center (half the
 *	 eye separation).
 *
 * For this to work, you must link with the graphics library (-lgl_s)
 * and the math library (-lm).
 */
extern void stereopersp(int, float, float, float, float, float);

/*
 * stereo_on()
 *
 * If the hardware supports it and it hasn't been done before, this
 * routine will:
 *  Switch the monitor into 120 HZ mode
 *  Constrain the mouse to the lower half of the screen
 * It also sets the YMAXSTEREO and YOFFSET variables
 */
extern void stereo_on();

/*
 * stereo_off()
 *
 * Undoes the effects of stereo_on().  If multiple windowed stereo
 * applications are running, the application that turned on stereo
 * will turn it off.  A background program should probably be run
 * first thing, to provide a suitable backdrop for the other windows
 * (not a concern if the application takes over the entire screen, of
 * course).
 */
extern void stereo_off();

/*
 * Some routines to give a little bit of support to multi-windowed
 * stereo applications.
 *
 * This was hacked together very quickly; stereo windows may easily be
 * confused by pushing or popping them, or by stowing them.  Don't do
 * that.
 */
/*
 * long st_winopen(char *title)
 *   Returns a unique identifier for a pair of stereo windows.  This
 *   routine will also call stereo_on for you, if necessary.
 * void st_winclose(long)
 */
extern long st_winopen(char *);
extern void st_winclose(long);

/*
 * void st_right(long st_id)
 * void st_left(long st_id)
 *   Before drawing into the left or right eye view, call one of these
 *   routines.
 */
extern void st_right(long);
extern void st_left(long);

/*
 * void st_redraw(short val)
 *   When your program gets a REDRAW token, this routine must be
 *   called to keep the stereo windows moving together.  Pass it the
 *   value gotten from the qread call.
 */
extern void st_redraw(short);

#endif
