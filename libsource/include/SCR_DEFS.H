/*****************************************************************************/
/*                                                                           */
/* PRODUCT : Text Display Library                                            */
/*                                                                           */
/* FILE    : TEXDEFS.H                                                       */
/*                                                                           */
/* PURPOSE : Global definitions & type declarations.                         */
/*                                                                           */
/* AUTHOR  : James Ingram                                                    */
/*                                                                           */
/* DATE    : 15/Oct/92                                                       */
/*                                                                           */
/*****************************************************************************/

#ifndef SCR_DEFS
#define SCR_DEFS

// #undef  NUL
// #undef  NIL
// #undef  ERR
// #undef  EOF
// #undef  TRUE
// #undef  BOOL

#define FALSE    0
#define TRUE     1
#define OFF      0
#define ON       1
#define CR       0x0D                       /* Carriage return               */
#define LF       0x0A                       /* Line feed                     */
#define FF       0x0C                       /* Form feed                     */

#define Black    0
#define Blue     1
#define Green    2
#define Cyan     3
#define Red      4
#define Magenta  5
#define Brown    6
#define LGray    7
#define DGray    8
#define LGrey    7
#define DGrey    8
#define LBlue    9
#define LGreen   10
#define LCyan    11
#define LRed     12
#define LMagenta 13
#define Yellow   14
#ifndef White
#define White    15
#endif

#define Flash    8

typedef unsigned short       USBOOL;
typedef unsigned short       USHORT;
typedef unsigned long        ULONG;
typedef unsigned char        UCHAR;

typedef struct  _TIME
{

    signed char  HSecond;
    signed char  Second;
    signed char  Minute;
    signed char  Hour;

}                            BTIME;


typedef struct  _DATE
{

    signed char  Day;
    signed char  Month;
    signed short Year;

}                            BDATE;

#endif
