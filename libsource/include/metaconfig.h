/******************************************************************************/
/*                                                                            */
/* MODULE  : MetaConfig.h                                                     */
/*                                                                            */
/* PURPOSE : Specified arbitrary sequences of configuration files.            */
/*                                                                            */
/* DATE    : 29/Sep/2007                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 29/Sep/2009 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define METACONFIG_UNITS (int)(('Z'-'A')+1)
#define METACONFIG_ITEMS 128

#define METACONFIG_TYPE_NOTUSED     0
#define METACONFIG_TYPE_FILE        1
#define METACONFIG_TYPE_SUBSEQUENCE 2

#define METACONFIG_SEQUENCE_LENGTH  1024

/******************************************************************************/

void METACONFIG_Setup( void );
void METACONFIG_Init( void );
BOOL METACONFIG_Load( char *file );
BOOL METACONFIG_Process( char *file, int &items, STRING list[] );

/******************************************************************************/

