/*****************************************************************************/
/*                                                                           */
/* PRODUCT : Screen Functions Library                                        */
/*                                                                           */
/* FILE    : SCR.H                                                           */
/*                                                                           */
/* PURPOSE : Prototypes, structures & definitions.                           */
/*                                                                           */
/* AUTHOR  : James Ingram                                                    */
/*                                                                           */
/* DATE    : 15/Oct/92                                                       */
/*                                                                           */
/* CHANGES                                                                   */
/*                                                                           */
/* V1.0 - JNI 15/10/92 - Initial development.                                */
/*                                                                           */
/* V1.1 - JNI 21/01/95 - Minor modifications to the prompt functions.        */
/*                                                                           */
/*        JNI 24/01/95 - Absorbed all menu functionality into "scr_mnky(...)"*/
/*                       and implemented types "MENU_REC" & "MENU_OPT".  The */
/*                       old function "scr_menu(...)" merely calls the new   */
/*                       function for compatibility and ease of implementing */
/*                       simple option lists.                                */
/*                                                                           */
/*                     - Allocated each page copy number to specific purpose */
/*                       so that application and library functions would not */
/*                       compete.                                            */
/*                                                                           */
/*        JNI 25/01/95 - Implemented type "MENU_FNM" in "scr_mnky(...)" for  */
/*                       file name look ups.                                 */
/*                                                                           */
/* V1.2   JNI 11/02/97 - Implemented "FLD_STRUCTURE" in SCR field stuff.     */
/*                                                                           */
/*        JNI 29/04/97 - Cursor position push/pop in "SCR_CURS.C"            */
/*                                                                           */
/*        JNI 10/03/98 - Added Ctrl PGUP/PGDN keys to "scr_mnky(...)".       */
/*                                                                           */
/* V1.3   JNI 24/06/98 - Changes to "SCR_TIME.C" for PLS HCU system.         */
/*                                                                           */
/*****************************************************************************/

#ifndef SCR
#define SCR

#define scr_ver()  "SCR.LIB, V1.3, 24/06/98"

/*****************************************************************************/

#include "scr_defs.h"
#include "ascii.h"
#include "kb.h"

/*****************************************************************************/

#ifdef  MSVC
#include "msvc2bc.h"
#endif

/*****************************************************************************/

typedef struct
{
    short col;
    short row;
    short wid;
    short hgt;
}
BOX;


typedef struct
{
    short X;
    short Y;
}
point;


struct  bios_equipment
{
    USHORT diskdrive   : 1;
    USHORT coprocessor : 1;
    USHORT systemRAM   : 2;

    USHORT videomode   : 2;
#define BIOS_EQUIP_CO80      2
#define BIOS_EQUIP_MO80      3

    USHORT drivecount  : 2;
    USHORT DMApresent  : 1;
    USHORT RS_232count : 3;
    USHORT gameport    : 1;
    USHORT serialPRN   : 1;
    USHORT LPTCount    : 2;
};


#define BIOS_VIDEO_INT            0x10
#define BIOS_VIDEO_SETMODE        0x00
#define BIOS_VIDEO_TC80X25           3
#define BIOS_VIDEO_CURSIZE        0x01
#define BIOS_VIDEO_SETPOSN        0x02
#define BIOS_VIDEO_GETPOSN        0x03
#define BIOS_VIDEO_SCROLUP        0x06
#define BIOS_VIDEO_SCROLDN        0x07
#define BIOS_VIDEO_EXTFUNC        0x10
#define BIOS_VIDEO_PALETTE        0x00
#define BIOS_VIDEO_OVERSCAN       0x01
#define BIOS_VIDEO_BGNDMODE       0x03
#define BIOS_VIDEO_WRITSTR        0x13
#define BIOS_VIDEO_WRITSTR_SUB0   0x00
#define BIOS_VIDEO_WRITSTR_SUB1   0x01
#define BIOS_VIDEO_WRITCHR        0x09

#define DOS_DATE_GET              0x2A00

#define VIDRAM_SEGMENT       0xB800

#define MK_ATTR(F,B)    ((((B) & 0x0f) << 4) | ((F) & 0x0f))
#define FG_ATTR(A)      ((A) & 0x0f)
#define BG_ATTR(A)      (((A) >> 4) & 0x0f)

/**/
/*****************************************************************************/
/* MODULE: SCR_MODE  Screen mode functions.                                  */
/*****************************************************************************/

extern  short           scr_crtlines;  /* Rows in current screen mode.       */
extern  short           scr_crtwidth;  /* Columns in current screen mode.    */

void    scr_setbgndmode( UCHAR mode );
void    scr_setoverscan( UCHAR attr );
USBOOL  scr_setcrtlines( short lines );

#define BGNDMODE_INTENSITY   0x00
#define BGNDMODE_BLINKING    0x01

/**/
/*****************************************************************************/
/* MODULE: SCR_CURS  Cursor position/size funtions.                          */
/*****************************************************************************/

#define CURSOR_NORMAL        0x00
#define CURSOR_FULLBLOCK     0x01
#define CURSOR_HALFBLOCK     0x02

void    scr_cursize( UCHAR size );
void    scr_curposn( short col, short row );

extern  short scr_posn_x,scr_posn_y;

void    scr_posnpush( void );
void    scr_posnpop( void );

/**/
/*****************************************************************************/
/* MODULE: SCR_DISP  Display functions.                                      */
/*****************************************************************************/

void    scr_dspc ( short col, short row, UCHAR attr, char chr );
void    scr_dspcn( short col, short row, UCHAR attr, char chr, short count );
void    scr_dsps ( short col, short row, UCHAR attr, char *string );
void    scr_dsps ( short col, short row, UCHAR attr, char *string );
void    scr_dspsn( short col, short row, UCHAR attr, char *string, short length );
void    scr_dspf ( short col, short row, UCHAR attr, char *format, ... );

/**/
/*****************************************************************************/
/* MODULE: SCR_PCPY  Screen save (page copy) functions.                      */
/*****************************************************************************/

#define PCOPY_MAX       4              /* Maximum page number supported.     */

#define PCOPY_DISPLAY   0              /* Display page.                      */
#define PCOPY_SAVE1     1              /* General purpose 1.                 */
#define PCOPY_SAVE2     2              /* General purpose 2.                 */
#define PCOPY_BOXSAVE   3              /* Box save page.                     */
#define PCOPY_LIBSAVE   4              /* Library functions save page.       */

#ifdef BC
extern  UCHAR   far   *scr_pcopybuf;   /* Page copy buffer area.             */
void    scr_pcopy( short spage, short dpage, BOX *box );
#endif

/**/
/*****************************************************************************/
/* MODULE: SCR_BOX   Box setup/draw/scroll functions.                        */
/*****************************************************************************/

void    scr_boxset ( BOX *box, short col, short row, short wid, short hgt );
short   scr_boxptr ( BOX *box, short col, short row );
void    scr_boxfram( BOX *box, UCHAR type, UCHAR attr );
void    scr_boxfill( BOX *box, UCHAR attr, UCHAR fill );
void    scr_boxexpl( BOX *box );
void    scr_boxscrl( BOX *box, short lines );
void    scr_boxdraw( BOX  *out, BOX *in, short col, short row, short wid, short hgt,
                     UCHAR flag, UCHAR type, UCHAR boxattr, UCHAR titattr, char *title );

#define BOX_LEAVE       0x00           /* Leave the box on the screen.       */
#define BOX_SAVE        0x01           /* Save the area underneath box.      */
#define BOX_CLEAR       0x02           /* Clear box area when finished.      */
#define BOX_SHADOW      0x04           /* Draw box with shadow.              */
#define BOX_FRAME       0x08           /* Frame box with line draw chars.    */
#define BOX_EXPLODE     0x10           /* Draw box with "exploding" frame.   */
#define BOX_ORIGIN      0x20           /* Set origin to inner box when done. */

#define BOX_SHADCORN    0xDC           /* Bottom right corner shadow char.   */
#define BOX_SHADVERT    0xDB           /* Verticle (right) shadow char.      */
#define BOX_SHADHORZ    0xDF           /* Horizontal (bottom) shadow char.   */

void    scr_horzln( short col, short row, short len, UCHAR type, UCHAR ends, UCHAR attr );
void    scr_vertln( short col, short row, short len, UCHAR type, UCHAR ends, UCHAR attr );

/**/
/*****************************************************************************/
/* MODULE: SCR_CLS   Screen clear/setup functions.                           */
/*****************************************************************************/

void    scr_clr( short col, short row, short wid, short hgt, UCHAR attr );
void    scr_cls( void );

#define SCR_VERSION     4
extern  char       scr_version[SCR_VERSION+1];

void    scr_verset( char *version );

void    scr_prmptrgt( UCHAR attr, char *text );
void    scr_prmptlne( UCHAR attr, char *format, ... );
void    scr_prmptl_r( char *left, char *right );
void    scr_prmpterr( UCHAR beep, UCHAR wait, char *format, ... );
#define BEEP_NONE       0              /* No beep at all.                    */
#define BEEP_ERROR      1              /* Sound the error beep.              */
#define BEEP_WARNING    2              /* Sound the warning beep.            */
#define BEEP_TIME       3              /* Sound the time beep.               */
#define BEEP_SECURITY   4              /* Sound the security alarm beep.     */

#define WAIT_KEY        0xFF           /* Wait for a key to be pressed.      */
#define WAIT_NONE       0x00           /* Don't wait for at all.             */

void    scr_pagehead( char *name, char *head, char *subh, void (*func)( void ) );

/**/
/*****************************************************************************/
/* MODULE: SCR_PAL   Screen information (palette) functions.                 */
/*****************************************************************************/

USBOOL  scr_palinit( char *name );
void    scr_palunit( void );
USBOOL  scr_palset ( char *name );
void    scr_palpush( char *name );
void    scr_palpop ( void );
void    scr_palEGA ( UCHAR color, UCHAR value );

#define PAL_EXTENSION   "PAL"          /* Palette file name extenstion.      */
#define PAL_DEFAULT     "DEFAULT"      /* Default initialize palette name.   */
#define PAL_GLOBATTR    "GLOBATTR"     /* Global attribute pallete name.     */
#define PAL_MENU        "MENU"         /* Menu selection pallete name.       */
#define PAL_PAGEHEAD    "PAGEHEAD"     /* Page heading pallete name.         */
#define PAL_ERRORBOX    "ERRORBOX"     /* Page heading pallete name.         */
#define PAL_STACK       20             /* Size of palette push/pop stack.    */

struct  scr_paletteheader
{
    USHORT     palcount;
    UCHAR      overscan;
    UCHAR      bgndmode;
    UCHAR      direct;

    UCHAR      adaptor;
#define PAL_EGA         0
#define PAL_VGA         1
#define PAL_CGA         2
#define PAL_MGA         3

    UCHAR      mode;
    UCHAR      spare[7];
    UCHAR      map[16];
};


struct  scr_palette
{
#define PAL_ATTR        8

    UCHAR     attr[PAL_ATTR];
#define PAL_BORD        0
#define PAL_TITL        1
#define PAL_HEAD        2
#define PAL_TEXT        3
#define PAL_DATA        4
#define PAL_HLGT        5
#define PAL_PRMT        6
#define PAL_ALTN        7

#define PAL_NAME        8
    char      name[PAL_NAME];
};


extern  struct  scr_palette            scr_palcur;
extern  struct  scr_paletteheader      scr_palhead;

#define pBORD   scr_palcur.attr[PAL_BORD]
#define pTITL   scr_palcur.attr[PAL_TITL]
#define pHEAD   scr_palcur.attr[PAL_HEAD]
#define pTEXT   scr_palcur.attr[PAL_TEXT]
#define pDATA   scr_palcur.attr[PAL_DATA]
#define pHLGT   scr_palcur.attr[PAL_HLGT]
#define pPRMT   scr_palcur.attr[PAL_PRMT]
#define pALTN   scr_palcur.attr[PAL_ALTN]
#define pNAME   scr_palcur.name

#define pBOX    scr_palcur.attr[PAL_BORD]
#define pTITLE  scr_palcur.attr[PAL_TITL]
#define pPROMPT scr_palcur.attr[PAL_PRMT]
#define pALT    scr_palcur.attr[PAL_ALTN]

#define PAL_BACK        pBOX           /* Display attribute for back ground. */
#define PAL_SHAD        pTITLE         /* Display attribute for shadow box.  */
#define PAL_EXPL        pHEAD          /* Display attribute for explode box. */
#define PAL_INP         pTEXT          /* Display attribute for input.       */
#define PAL_ERR         pDATA          /* Display attribute for errors.      */
#define PAL_VER         pHLGT          /* Display attribute for version.     */
#define PAL_KEY         pALT           /* Display attribute for key panels.  */

extern  UCHAR           scr_attrback;  /* Display attribute for back ground. */
extern  UCHAR           scr_attrshad;  /* Display attribute for shadow box.  */
extern  UCHAR           scr_attrexpl;  /* Display attribute for explode box. */
extern  UCHAR           scr_attrinp;   /* Display attribute for input.       */
extern  UCHAR           scr_attrerr;   /* Display attribute for errors.      */
extern  UCHAR           scr_attrver;   /* Display attribute for version.     */
extern  UCHAR           scr_attrkey;   /* Display attribute for key panels.  */

/**/
/*****************************************************************************/
/* MODULE: SCR_SCRL  Screen scroll up/down functions.                        */
/*****************************************************************************/

void    scr_scrdn( short col, short row, short wid, short hgt, short lines, UCHAR attr );
void    scr_scrup( short col, short row, short wid, short hgt, short lines, UCHAR attr );

/**/
/*****************************************************************************/
/* MODULE: SCR_BEEP  Beep functions.                                         */
/*****************************************************************************/

void    scr_beeptime ( void );
void    scr_beepwarn ( void );
void    scr_beeperror( void );
void    scr_beepscrty( void );

/**/
/*****************************************************************************/
/* MODULE: SCR_DATE  Date manipulation functions.                            */
/*****************************************************************************/

#define DATE_CURRENT    0x01           /* Get the curret date                */
#define DATE_MONTH      0x02           /* Use month abbrieviation vs. number */
#define DATE_YEAR       0x04           /* Display full 4 digit year          */

#define DATE_BUFFER     9              /* DD:MM:YY = 8 + NULL                */

#define DATE_FLD_DAY    0
#define DATE_FLD_MONTH  1
#define DATE_FLD_YEAR   2

extern  char      *scr_monthstr[];
extern  short      scr_monthday[];

void    scr_dateget( BDATE *d );
char   *scr_datestr( BDATE *d , UCHAR flag );
void    scr_dateset( BDATE *d , UCHAR day, UCHAR month, short year );
short   scr_datecmp( BDATE *d1, BDATE *d2 );
short   scr_datedif( BDATE *d1, BDATE *d2 );
void    scr_dateage( BDATE *d1, short days );
USBOOL  scr_datebuf( BDATE *d, UCHAR flag, char *buf );
short   scr_dateinp( BDATE *d , short col, short row, UCHAR attr, UCHAR flag, USBOOL (*kbfunc)( short *c, short *s ) );
USBOOL  scr_datenul( BDATE *d );
short   scr_datedow( void );

/**/
/*****************************************************************************/
/* MODULE: SCR_TIME  Time manipulation functions.                            */
/*****************************************************************************/

#define TIME_12_HOUR    0x01           /* 12 hour format vs. 24 hour.        */
#define TIME_AM_PM      0x03           /* Use AM/PM format                   */
#define TIME_SECONDS    0x04           /* Include seconds in time            */
#define TIME_CURRENT    0x08           /* Get the current time               */

#define TIME_BUFFER     12             /* HH:MM:SS AM = 11 + NULL            */

#define TIME_FLD_HOUR   0
#define TIME_FLD_MINUTE 1
#define TIME_FLD_SECOND 2
#define TIME_FLD_PERIOD 3

void    scr_timeget( BTIME *t );
char   *scr_timestr( BTIME *t , UCHAR flag );
void    scr_timeset( BTIME *t , UCHAR hsecond, UCHAR second, UCHAR minute, UCHAR hour );
short   scr_timecmp( BTIME *t1, BTIME *t2 );
void    scr_timesec( BTIME *t, long hsec );
void    scr_timedif( BTIME *t1, BTIME *t2, BTIME *dif );
void    scr_timeadd( BTIME *t1, BTIME *t2, BTIME *add );
void    scr_timeage( BTIME *t1, short minutes );
USBOOL  scr_timebuf( BTIME *t, UCHAR flag, char *str );
short   scr_timeinp( BTIME *t , short col, short row, UCHAR attr, UCHAR flag, USBOOL (*kbfunc)( short *c, short *s ) );
USBOOL  scr_timenul( BTIME *t );

/**/
/*****************************************************************************/
/* MODULE: SCR_INP   General purpose input functions.                        */
/*****************************************************************************/

#define INPUT_CLEAR     0x0001         /* Clear buffer at start.             */
#define INPUT_UCASE     0x0002         /* Convert input chars to upper case. */
#define INPUT_INITCLR   0x0004         /* Clear buffer when initial key hit. */
#define INPUT_AUTOENT   0x0008         /* Auto ENTER at end of field.        */
#define INPUT_STRIP     0x0010         /* Strip trailing spaces from input.  */
#define INPUT_POSNSAVE  0x0020         /* Save position in input buffer.     */
#define INPUT_POSNSTRIP 0x0040         /* Strip spaces from current position.*/
#define INPUT_REFRESH   0x0080         /* Refresh display with each key.     */
#define INPUT_SECURE    0x0100         /* Secure input (don't display)       */
#define INPUT_DELETE    0x0200         /* DELETE key terminates input.       */
#define INPUT_UNSIGNED  0x0400         /* Unsigned input.                    */
#define INPUT_SCINOT    0x0800         /* Scientific Notation (float/double) */
#define INPUT_DECIMAL   0x2000         /* Decimal input valid only.          */
#define INPUT_HEXDECI   0x4000         /* Hexidecimal input valid only.      */
#define INPUT_INSERT    0x8000         /* Insert key terminates input.       */

#define INPUT_MAXSTR    80             /* Maximum input for string.          */
#define INPUT_MAXINT    20             /* Maximum input for integer.         */
#define INPUT_MAXFLT    20             /* Maximum input for float.           */

extern  short      scr_inputposn;      /* Position in input buffer.          */
extern  USBOOL     scr_inputins;       /* Insert mode TRUE/FALSE toggle.     */
extern  USBOOL     scr_inputuppr;      /* Force upper case TRUE/FALSE.       */

short   scr_inpstr( char *inp, short col, short row, short wid, UCHAR attr, USHORT flag, USBOOL (*kbfunc)( short *c, short *s ) );
short   scr_inpint( void *inp, short col, short row, short wid, UCHAR attr, USHORT flag, USBOOL (*kbfunc)( short *c, short *s ), short size );
short   scr_inpflt( void *inp, short col, short row, short wid, UCHAR attr, USHORT flag, USBOOL (*kbfunc)( short *c, short *s ), short size, short prec );

/**/
/*****************************************************************************/
/* MODULE: SCR_KP    Key Panel functions.                                    */
/*****************************************************************************/

struct  scr_kp_info
{
    char  *label;
    char  *text;
    UCHAR  attr;
    short  scanc;
};

#define SCR_KP_WID      8
#define SCR_KP_HGT      2
#define SCR_KP_MAX      10
#define SCR_KP_STACK    5

void    scr_kp_top ( USBOOL hlgt, BOX *box, short col, short row, struct scr_kp_info * );
void    scr_kp_draw( short col, short row, struct scr_kp_info * );
USBOOL  scr_kp_scan( short *charc, short *scanc );
void    scr_kp_clr ( void );
void    scr_kp_push( void );
void    scr_kp_pop ( void );

/**/
/*****************************************************************************/
/* MODULE: SCR_KB    Special keyboard functions.                             */
/*****************************************************************************/

void    kb_stuffset ( USBOOL (*kbfunc)( short *, short * ) );
void    kb_stuffkey ( short charc, short scanc );
void    kb_stuffstr ( char *str );
USBOOL  kb_stuffscan( short *charc, short *scanc );

void    kb_ignoreset( BOX *box, USHORT mask );

extern  point   kb_boxposn;

void    kb_boxset ( USBOOL (*kbfunc)( short *, short * ), BOX *box, short refno, short scanc );
void    kb_boxclr ( short refno );
USBOOL  kb_boxscan( short *charc, short *scanc );

void    kb_timeoutset ( USBOOL (*kbfunc)( short *, short * ), short seconds, short scanc );
USBOOL  kb_timeoutscan( short *charc, short *scanc );

void    kb_bgndset ( USBOOL (*kbfunc)( short *, short * ), void (*proc)( void ) );
USBOOL  kb_bgndscan( short *charc, short *scanc );

USBOOL  kb_escape( void );

/**/
/*****************************************************************************/
/* MODULE: SCR_PNT   Pointer (mouse) manipulation functions.                 */
/*****************************************************************************/

extern  USBOOL scr_pointinst;

void    scr_pointinit( void );
void    scr_pointstop( void );
void    scr_pointshow( void );
void    scr_pointhide( void );
void    scr_pointposn( short col, short row );
void    scr_pointvert( short max, short min );
void    scr_pointread( short *col, short *row, short *but );
USBOOL  scr_pointpres( void );
USBOOL  scr_pointmove( void );
USBOOL  scr_pointbox ( short col, short row, BOX *box );

/**/
/*****************************************************************************/
/* MODULE: SCR_ORIG  Screen origin functions.                                */
/*****************************************************************************/

extern  short      scr_origcol,scr_origrow;

#define ORIG_STACK      20
#define ORIG_COL        0
#define ORIG_ROW        1
#define ORIG_POSN       2

void    scr_origreset( void );
void    scr_origset  ( short col, short row );
void    scr_origpush ( BOX *box );
void    scr_origpop  ( void );
void    scr_origbox  ( BOX *box );

/**/
/*****************************************************************************/
/* MODULE: SCR_FLD   Field manipulation functions.                           */
/*****************************************************************************/

void    scr_int2buf( UCHAR len, void *buf, ULONG val );

/*****************************************************************************/

#define FLD_MAX_TYPE    20             /* Maximum number of field types.     */
#define FLD_MAX_ITEM    16             /* Maximum fields in a FLD_STRUCTURE. */
#define FLD_MAX_FRMT    16             /* Maximum size of format string.     */

#define FLD_SEPARATOR   ','            /* FLD_STRUCTURE field separator.     */

#define FLD_LEN_INTEGER  8             /* Default display lengths...         */
#define FLD_LEN_FLOAT    8
#define FLD_LEN_LOGICAL  5

struct  field_information
{
    UCHAR    type;
#define FLD_STRING       0             /* BTR_VarString  */
#define FLD_INTEGER      1             /* BTR_VarInteger */
#define FLD_FLOAT        2             /* BTR_VarFloat   */
#define FLD_DATE         3             /* BTR_VarDate    */
#define FLD_TIME         4             /* BTR_VarTime    */
#define FLD_LOGICAL      7             /* BTR_VarLogical */
#define FLD_STRUCTURE   16
#define FLD_TABLE       17
#define FLD_PROCEDURE   18
#define FLD_CONST       19
#define FLD_LIST        20
#define FLD_END       0xFF

    short     length;

    UCHAR     precision;
#define FLD_BYTE        sizeof(UCHAR)
#define FLD_SHORT       sizeof(USHORT)
#define FLD_LONG        sizeof(ULONG)

    UCHAR     flags;
#define FLD_DEC         0x00
#define FLD_HEX         0x01

#define FLD_JUSTSPACE   0x00
#define FLD_JUSTZERO    0x02

#define FLD_SIGNED      0x00
#define FLD_UNSIGNED    0x04

#define FLD_JUSTRIGHT   0x00
#define FLD_JUSTLEFT    0x08

#define FLD_TRUEFALSE   0x00
#define FLD_YESNO       0x01

#define FLD_SPACEPACK   0x10
#define FLD_ARRAY       0x20

    void     *pdata;
    char   *(*pproc)(void *);
    void     *pmisc;
};

short   scr_fldmem( struct field_information *fld );
short   scr_fldlen( struct field_information *fld );
char   *scr_fldstr( struct field_information *fld );
USBOOL  scr_fldbuf( struct field_information *fld, char *buf );

/**/
/*****************************************************************************/
/* MODULE: SCR_STR   String manipulation functions.                          */
/*****************************************************************************/

short   scr_strlen ( char *str, short len );
short   scr_strtrim( char *str, short len );
short   scr_strstrip( char *str, short len );
short   scr_strpack( char *str, short len );
char  **scr_strvarg( short argc, ... );
USBOOL  scr_strnull( char *str, short len );
void    scr_strpoolcheck( char *label );
char   *scr_strpool( short len );
char   *scr_stringf( char *format, ... );
char   *scr_strchar( char chr, short count );
char   *scr_strprnt( char *str, short len );
char   *scr_strhex ( void *buf, short len );
USBOOL  scr_memnull( char *str, short len );

#define STR_NULLTERM    -1

#define STRVARG_MAX     20
#define STRPOOL_SIZE    3072
#define STRINGF_SIZE    132
#define STRLIST_SIZE    132

struct  strlist_element
{
    char  *strptr;
    short  length;
};

char   *scr_strlist( struct strlist_element list[], char *next, char *last, char *term );

/**/
/*****************************************************************************/
/* MODULE: SCR_FILE  File (DOS flat file) access functions.                  */
/*****************************************************************************/

struct  scr_fileinformation
{
    short  FD;
    short  access;
    short  mode;

#define SCR_FILENAME     40
    char   filename[SCR_FILENAME];
};

#define SCR_FILEMAX     20
extern  struct          scr_fileinformation  scr_files[];
extern  USBOOL          scr_fileinit;

#ifdef  BC
short   readfar( short filehand, void far *datbuf, USHORT datlen );
USBOOL  scr_filereadf( char *filename, void far *datbuf, USHORT datlen );
#endif

void    scr_error( char *file, char *func, char *text );

char   *scr_filepath ( char *environ, char *filename );
char   *scr_srchpath ( char *filepath, char *srchpath );
long    scr_filesize ( char *filename );
short   scr_filefind ( char *filename );
USBOOL  scr_fileopen ( char *filename, short access, USHORT mode );
USBOOL  scr_fileread ( char *filename, void *datbuf, USHORT datlen );
USBOOL  scr_filewrite( char *filename, void *datbuf, USHORT datlen );
USBOOL  scr_fileseek ( char *filename, long posn );
USBOOL  scr_fileclose( char *filename );

/**/
/*****************************************************************************/
/* MODULE: SCR_MENU  Menu functions.                                         */
/*****************************************************************************/

short   scr_menu( USBOOL (*kbdfunc)( short *, short * ), void (*optfunc)( void *, char * ),
                  void *ptr, short col, short row, short wid, UCHAR flag, char *head, short hgt, char **opts );

USBOOL  scr_cnfm( short col, short row, USHORT flag, char *head, char *prom );


struct  scr_mnky_info
{
     short   mnutype;
#define MENU_OPT        0
#define MENU_REC        1
#define MENU_KEY        2
#define MENU_FNM        3

     USBOOL  (*kbdfunc)( short *, short * );
     void  (*optfunc)( void *, char * );

     union
     {
         struct  scr_mnky_opt
         {
              short  optlen;
              char **optptr;
         }
         opt;

         struct  scr_mnky_rec
         {
              void  *recbuf;
              short  reclen;
              short  recmax;
              short  optoff;
              short  optlen;
         }
         rec;

         struct  scr_mnky_key
         {
             char *(*keyget)( void *key );
             short   keylen;
             void   *keybuf;
             void   *keynul;
         }
         key;

         struct  scr_mnky_fnm
         {
             struct  scr_mnky_key      key;

             char   *fnmmsk;
         }
         fnm;

         UCHAR   dat[32];
     }
     data;
};

#define MENU_MAXFNM     128
#define MENU_MAXKEY     128

void    scr_mnky_fmt( short row, char *fmt, char *opt, USBOOL bar, USBOOL set );
char   *scr_mnky_opt( struct scr_mnky_info *info, short optn );
char   *scr_mnky_key( struct scr_mnky_info *info, short optn );
short   scr_mnky_len( struct scr_mnky_info *info );
USBOOL  scr_mnky_eof( struct scr_mnky_info *info, short optn );
USBOOL  scr_mnky_fnm( struct scr_mnky_info *info );
void    scr_mnky_end( struct scr_mnky_info *info );

short   scr_mnky( struct scr_mnky_info *info, void *optbuf, short col, short row, short wid, UCHAR flag, char *head, short nopts );

#define MENU_SCREENSAVE      0x01      /* Save/Restore screen under menu.    */
#define MENU_OPTIONSAVE      0x02      /* Position at current option value.  */
#define MENU_LETTERSELECT    0x04      /* First letter of option selects.    */
#define MENU_FLAG            0x08      /* Flag toggle using space bar.       */
#define MENU_NULLKEY         0x10      /* Null key entry as first item.      */
#define MENU_SORTKEY         0x20      /* Sort entries (KEY & REC only).     */
#define MENU_PAGE            0x40      /* Allow PgUp & PgDn through list.    */
#define MENU_DISPONLY        0x80      /* Display the menu & exit function.  */

#define MENU_YESNO           scr_strvarg(2,"No","Yes")
#define MENU_TRUEFALSE       scr_strvarg(2,"False","True")
#define MENU_ONOFF           scr_strvarg(2,"Off","On")

/**/
/*****************************************************************************/
/* MODULE: SCR_TUNE  Tune playing functions.                                 */
/*****************************************************************************/

USBOOL  scr_tuneinit( char *file, USBOOL loop );
void    scr_tuneplay( void );
void    scr_tunestop( void );

#endif

/*****************************************************************************/
/**/
