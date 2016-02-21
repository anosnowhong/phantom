/*****************************************************************************
 * PROJECT:  Mouse routines with 'real' graphic cursor in text mode.
 *****************************************************************************
 * MODULE:  MOU.H
 *****************************************************************************
 * DESCRIPTION:
 *   Header file for the mouse routines.
 *
 *****************************************************************************
 * MODIFICATION NOTES:
 *    Date     Author Comment
 * 07-Jan-1991   dk   Fixed bugs and set up for release to Usenet.
 * 26-Oct-1990   dk   Initial file.
 *****************************************************************************
 *
 * DISCLAIMER:
 *
 * Programmers may incorporate any or all code into their programs,
 * giving proper credit within the source. Publication of the
 * source routines is permitted so long as proper credit is given
 * to Dave Kirsch.
 *
 * Copyright (C) 1990, 1991 by Dave Kirsch.  You may use this program, or
 * code or tables extracted from it, as desired without restriction.
 * I can not and will not be held responsible for any damage caused from
 * the use of this software.
 *
 *****************************************************************************
 * This source works with Turbo C 2.0 and MSC 6.0 and above.
 *****************************************************************************/

/********************************************************/
/* 26-Oct-1990 - dk                                     */
/*                                                      */
/*  Standard types and constants I use in my programs.  */
/*                                                      */
/********************************************************/

typedef unsigned char       byte;
typedef unsigned short int  word;
typedef unsigned short int  boole;
typedef unsigned long  int  dword;

#define FALSE 0
#define TRUE 1

#ifdef __TURBOC__
  #define FAST pascal     /* for fast calling of functions -- Turbo C */
#else
  #define FAST _fastcall  /* for fast calling of functions -- MicroSoft C 6.0 */
  #define asm _asm
#endif
#define LOCAL   near   /* function can not be called outside of this module */
#define PRIVATE static /* function is private */
#define STATIC  static /* private variables */

#ifndef MK_FP
  #define MK_FP(seg,ofs)  ((void far *) \
                             (((unsigned long)(seg) << 16) | (unsigned)(ofs)))
#endif

#ifndef poke
#define poke(a,b,c)     (*((int  far*)MK_FP((a),(b))) = (int)(c))
#define pokeb(a,b,c)    (*((char far*)MK_FP((a),(b))) = (char)(c))
#define peek(a,b)       (*((int  far*)MK_FP((a),(b))))
#define peekb(a,b)      (*((char far*)MK_FP((a),(b))))
#endif

/***************************************************/
/* Mon 07-Jan-1991 - dk                            */
/*                                                 */
/*  Variables and defines for the mouse routines.  */
/*                                                 */
/***************************************************/

/* Size of mouse "click" ahead buffer. */
#define MOUSEBUFFERSIZE 16

/* Bit defines for mouse driver function 12 -- define handler. */
#define MOUSEMOVE      1
#define LEFTBPRESS     2
#define LEFTBRELEASE   4
#define RIGHTBPRESS    8
#define RIGHTBRELEASE 16
#define MIDBPRESS     32
#define MIDBRELEASE   64

#define LEFTBDOWN  1
#define RIGHTBDOWN 2
#define MIDBDOWN   4

/* Shift states for byte a 0:417h
   bit 7 =1 INSert active
   bit 6 =1 Caps Lock active
   bit 5 =1 Num Lock active
   bit 4 =1 Scroll Lock active
   bit 3 =1 either Alt pressed
   bit 2 =1 either Ctrl pressed
   bit 1 =1 Left Shift pressed
   bit 0 =1 Right Shift pressed
*/

#define SHIFT_RIGHTSHIFT 0x01
#define SHIFT_LEFTSHIFT  0x02
#define SHIFT_SHIFT      0x03 /* Either shift key. */
#define SHIFT_CTRL       0x04
#define SHIFT_ALT        0x08
#define SHIFT_SCROLLLOCK 0x10
#define SHIFT_NUMLOCK    0x20
#define SHIFT_CAPSLOCK   0x40
#define SHIFT_INS        0x80

/* Mouse information record */
struct minforectype {
  word buttonstat;
  word	cx, cy;
  byte shiftstate;
};

#define MOUINFOREC struct minforectype

extern word mousehidden;           /* Is the mouse on? Additive flag */
extern boole mouseinstalled;     /* Is the mouse installed? */

extern volatile word mousex, mousey; /* Mouse coordinates in characters. */
extern volatile word moupx, moupy; /* Mouse coordinates in characters. */

extern "C"
{
/* Initialize the mouse routines -- must be called. */
void    FAST MOUinit(void);

/* Deinitialize the mouse routines -- must be called on shutdown.
   Failure to call it will most likely result in a system crash if the mouse
   is moved. */
void    FAST MOUdeinit(void);

/* Hide the mouse cursor */
void    FAST MOUhide(void);

/* Hide the mouse cursor if it moves or is in a specific rectangular region
   of the screen. */
void    FAST MOUconditionalhide(int x1, int y1, int x2, int y2);

/* Show the mouse cursor */
void    FAST MOUshow(void);

/* return TRUE if there are events waiting in the buffer. */
boole FAST MOUcheck(void);

/* look at the next event in the buffer, but don't pull it out. */
void    FAST MOUpreview(MOUINFOREC *mouinforec);

/* get and remove next event from the buffer. */
void    FAST MOUget(MOUINFOREC *mouinforec);

/* return the current status of the mouse buttons (see defines above). */
word	FAST MOUbuttonstatus(void);

/* Set the mouse cursor to a specific screen position. */
void FAST MOUsetpos(int x, int y);

}
