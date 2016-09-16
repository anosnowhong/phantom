/******************************************************************************/
/*                                                                            */ 
/* MODULE  : OPTOdata.h                                                       */ 
/*                                                                            */ 
/* PURPOSE : OptoTrak data conversion routines.                               */ 
/*                                                                            */ 
/* DATE    : 21/Jun/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V2.2  JNI 14/Feb/2001 - Developed with new MOTOR.LIB modules.              */ 
/*                                                                            */ 
/* V2.3  JNI 25/Apr/2001 - Changed to use default file names and buffers.     */ 
/*                                                                            */ 
/******************************************************************************/

extern  ND3D   OPTO_nd3d[];       // Array of marker positions (XYZ).
extern  matrix OPTO_RTMX;         // Coordinate frame R/T matrix.

/******************************************************************************/

BOOL    OPTO_FileOpen( char *file, int &frames, int &markers );
void    OPTO_FileClose( void );

/******************************************************************************/

BOOL    OPTO_FileFrame( int frame, ND3D nd3d[] );

BOOL    OPTO_File2Matrix( char *file, matrix &posn, matrix &seen, char *rtmx );
BOOL    OPTO_File2Matrix( matrix &posn, matrix &seen, char *rtmx );
BOOL    OPTO_File2Matrix( char *file, matrix &posn, char *rtmx );
BOOL    OPTO_File2Matrix( matrix &posn, char *rtmx );

/******************************************************************************/

void    OPTO_BuffFrame( void *data, ND3D nd3d[], int frame, int markers );

BOOL    OPTO_Buff2Matrix( OPTOMEM *buff, matrix &posn, matrix &seen, char *rtmx, BOOL keep );
BOOL    OPTO_Buff2Matrix( OPTOMEM *buff, matrix &posn, matrix &seen, char *rtmx );
BOOL    OPTO_Buff2Matrix( matrix &posn, matrix &seen, char *rtmx );
BOOL    OPTO_Buff2Matrix( OPTOMEM *buff, matrix &posn, char *rtmx );
BOOL    OPTO_Buff2Matrix( matrix &posn, char *rtmx );

/******************************************************************************/

BOOL    OPTO_Buffer2Matrix( int type, matrix &posn, matrix &seen, char *rtmx );

/******************************************************************************/

