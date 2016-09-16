/******************************************************************************/
/*                                                                            */ 
/* MODULE  : typedefs.h                                                       */ 
/*                                                                            */ 
/* PURPOSE : General types and definitions.                                   */ 
/*                                                                            */ 
/* DATE    : 12/May/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 12/May/2000 - Initial version.                                   */ 
/*                                                                            */ 
/* V1.1  JNI 17/Jan/2002 - Added typedef for 4-byte (LONGLONG) integers.      */ 
/*                                                                            */ 
/******************************************************************************/

// The usual unsigned integers...
typedef unsigned short       USHORT;
typedef unsigned long        ULONG;
typedef unsigned int         UINT;
typedef unsigned char        UCHAR;

// 8-byte integer (V1.1)...
typedef LONGLONG             INT64;

#define STRLEN 256
typedef char   STRING[STRLEN+1];

typedef int (*PRINTF)( const char *, ... );

/******************************************************************************/

#define BITS_BYTE       8
#define BITS_WORD      16
#define BITS_DWORD     32

/******************************************************************************/

#define MAX_CHAR_SIGNED      127
#define MAX_CHAR_UNSIGNED    256

#define MAX_SHORT_SIGNED     32767
#define MAX_SHORT_UNSIGNED   65535

#define MAX_LONG_SIGNED      2147483647
#define MAX_LONG_UNSIGNED    4294967295

#define MAX_DOUBLE           100000000000000.0

#define INT15_MAX            32768

#ifndef INT16_MAX
#define INT16_MAX            65536
#endif

/******************************************************************************/

#ifndef LO
#define LO    0
#endif

#ifndef LOW
#define LOW   0
#endif

#ifndef HI
#define HI    1
#endif

#ifndef HIGH
#define HIGH  1
#endif

/******************************************************************************/

// ASCII codes...
#define NUL        0x00                          // Null character.
#define SOH        0x01                          // Start Of Header.
#define STX        0x02                          // Start Of Text.
#define ETX        0x03                          // End Of Text.
#define EOH        0x04                          // End Of Transmission.
#define EOT        0x04                          // End Of Transmission.
#define ESC        0x1B                          // ESCape key.
#define TAB        0x09                          // TAB.
#define SPACE      0x20                          // SPACE.
#define LF         0x0A                          // Line Feed.
#define CR         0x0D                          // Carriage Return.
#define ENTER      0x0D                          // Enter.
#define PLUSMINUS  0xF1                          // Plus or Minus character.
#define BACKSPACE  0x08                          // BackSpace.

/******************************************************************************/

#define PRNTBUFF   256                           // Standard print buffer size.

/******************************************************************************/

#define MAXPATH    256                           // Maximum file path.
#define MAXEXTN      4                           // Maximum file extension.

/******************************************************************************/

#define  EXIT_OK        0                        // Applications exit codes...
#define  EXIT_ERROR    -1

/******************************************************************************/
/* Windows NT Environment Variables.                                          */
/******************************************************************************/

#define  WINNT_USER     "USERNAME"               // User name.
#define  WINNT_MACHINE  "USERDOMAIN"             // Machine name.

/******************************************************************************/

