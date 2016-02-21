/*****************************************************************************/
/*                                                                           */
/* PRODUCT : Configuration File                                              */
/*                                                                           */
/* MODULE  : CFG.H                                                           */
/*                                                                           */
/* PURPOSE : System Configuration Variable prototypes & structures.          */
/*                                                                           */
/* AUTHOR  : James Ingram                                                    */
/*                                                                           */
/* DATE    : 21/Jul/94                                                       */
/*                                                                           */
/* CHANGES                                                                   */
/*                                                                           */
/* V1.0 - JNI 21/07/94 - Initial version.                                    */
/*                                                                           */
/* V1.2 - JNI 11/02/97 - Changed variable structure ("cfg_variable") to use  */
/*                       the SCR field rather that duplicate the same details*/
/*                       twice.  Implemented "FLD_TABLE" and "FLD_STRUCTURE" */
/*                       variable types.                                     */
/*                                                                           */
/* V1.3 - JNI 11/02/97 - Fixed bug in "CFG_EDIT.C" that caused a problem if  */
/*                       more than 16 fields were edited.                    */
/*                                                                           */
/*****************************************************************************/

#ifndef CFG
#define CFG

#define cfg_ver()  "CFG.LIB, V1.3, 09/07/98"

/*****************************************************************************/

#define CFG_INIT        0              /* Stages in CFG file processing...   */
#define CFG_VARN        1
#define CFG_DATA        2
#define CFG_PROC        3
#define CFG_INDX        4

#define CFG_MAX_VAR    32              /* Maximum length of variable name.   */
#define CFG_MAX_DAT   256              /* Maximum length of variable data.   */
#define CFG_MAX_IDX     4              /* Maximum length of array index.     */
#define CFG_MAX_DIM     2              /* Maximum number of array dimensions.*/
#define CFG_MAX_BUF  1024              /* Maximum configuration buffer size. */

#define CFG_FILE_GET    0
#define CFG_FILE_PUT    1

/*****************************************************************************/

struct  cfg_variable
{
    char                    *var;
    struct field_information fld;

    short                    idx[CFG_MAX_DIM];
};

/*****************************************************************************/
/* MODULE: CFG_PROC  Configuration processing functions.                     */
/*****************************************************************************/

USBOOL  cfg_file_get( char *file, struct cfg_variable table[] );
USBOOL  cfg_file_put( char *file, struct cfg_variable table[] );

void    cfg_file( short func, char *filename, struct cfg_variable table[] );
char   *cfg_text( struct cfg_variable *varptr, char *format );

/*****************************************************************************/
/* MODULE: CFG_EDIT  Configuration editing functions.                        */
/*****************************************************************************/

void    cfg_init( void );
short   cfg_edit( short col, short row, char *pal, struct cfg_variable table[] );

/*****************************************************************************/
/* MODULE: CFG_BUFF  Configuration buffering functions.                      */
/*****************************************************************************/

short   cfg_buff( short func, struct cfg_variable *tabl, char *buff, short size );

/*****************************************************************************/
#endif
