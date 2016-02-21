/******************************************************************************/
/*                                                                            */
/* MODULE  : DATAFILE.h                                                       */
/*                                                                            */
/* PURPOSE : Functions to write experimental data to file.                    */
/*                                                                            */
/* DATE    : 21/Jul/2006                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 21/Jul/2006 - Initial Development of module.                     */
/*                                                                            */
/******************************************************************************/

#ifndef DATAFILE_H
#define DATAFILE_H

/*****************************************************************************/

#define DATAFILE_VERSION 1

/*****************************************************************************/

int     DATAFILE_messgf( const char *mask, ... );
int     DATAFILE_errorf( const char *mask, ... );
int     DATAFILE_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    DATAFILE_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    DATAFILE_API_stop( void );
BOOL    DATAFILE_API_check( void );

/******************************************************************************/

#define DATAFILE_TYPE_HEADER 'H'
#define DATAFILE_TYPE_TRIAL  'T'
#define DATAFILE_TYPE_FRAME  'F'
#define DATAFILE_TYPE_FILE   'X'

#define DATAFILE_BUFFER 10240

#define DATAFILE_FRAMES 4
#define DATAFILE_FILES 16

/*****************************************************************************/

void DATAFILE_BufferReset( void );
void DATAFILE_BufferData( BYTE *data );
void DATAFILE_BufferData( BYTE *data, int length );
void DATAFILE_BufferData( int *data );
void DATAFILE_BufferData( long *data );
void DATAFILE_BufferData( double *data );
void DATAFILE_BufferFill( BYTE data, int length );
void DATAFILE_BufferType( BYTE type );
BOOL DATAFILE_BufferWrite( FILE *FP );

void DATAFILE_BufferMATDATVariables( MATDAT *MatDat );

BOOL DATAFILE_Write( FILE *FP, BYTE *buff, int size );

BOOL DATAFILE_Header( FILE *FP, MATDAT *Trial, MATDAT *F[] );
BOOL DATAFILE_Header( FILE *FP, MATDAT *Trial, MATDAT *Frame1, MATDAT *Frame2, MATDAT *Frame3, MATDAT *Frame4 );

BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial );
BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial, MATDAT &Frame1 );
BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2 );
BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3 );
BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3, MATDAT &Frame4 );

BOOL DATAFILE_Header( MATDAT &Trial );
BOOL DATAFILE_Header( MATDAT &Trial, MATDAT &Frame1 );
BOOL DATAFILE_Header( MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2 );
BOOL DATAFILE_Header( MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3 );
BOOL DATAFILE_Header( MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3, MATDAT &Frame4 );

BOOL DATAFILE_Header( FILE *FP, int TrialColumns, int FrameBlocks, int FrameRows[], int FrameCols[] );
BOOL DATAFILE_Header( FILE *FP, int TrialColumns, int FrameBlocks, ... );
BOOL DATAFILE_Header( int TrialColumns, int FrameBlocks, ... );
BOOL DATAFILE_Header( int TrialColumns, int FrameBlocks, int FrameRows[], int FrameCols[] );

BOOL DATAFILE_SaveTrial( FILE *FP, int TrialNumber, matrix &TrialData );
BOOL DATAFILE_SaveTrial( int TrialNumber, matrix &TrialData );
BOOL DATAFILE_SaveTrial( int TrialNumber );

BOOL DATAFILE_SaveFrame( FILE *FP, matrix &FrameData, int FrameRows );
BOOL DATAFILE_SaveFrame( matrix &FrameData, int FrameRows );
BOOL DATAFILE_SaveFrame( int FrameBlock );
BOOL DATAFILE_SaveFrame( void );

BOOL DATAFILE_TrialSave( int TrialNumber );

BOOL DATAFILE_AppendFile( FILE *FP, char *file );
BOOL DATAFILE_AppendFile( char *file );

BOOL DATAFILE_ReadData( FILE *FP, double *data, int items );
BOOL DATAFILE_ReadData( double *data, int items );
BOOL DATAFILE_ReadData( double *data );
BOOL DATAFILE_ReadData( FILE *FP, int *data, int items );
BOOL DATAFILE_ReadData( int *data, int items );
BOOL DATAFILE_ReadData( int *data );
BOOL DATAFILE_ReadData( FILE *FP, long *data, int items );
BOOL DATAFILE_ReadData( long *data, int items );
BOOL DATAFILE_ReadData( long *data );
BOOL DATAFILE_ReadData( FILE *FP, BYTE *data, int items );
BOOL DATAFILE_ReadData( BYTE *data, int items );
BOOL DATAFILE_ReadData( BYTE *data );
BOOL DATAFILE_ReadData( FILE *FP, char *data, int items );
BOOL DATAFILE_ReadData( char *data, int items );
BOOL DATAFILE_ReadData( char *data );

BOOL DATAFILE_Open( char *filename, char *mode, FILE **FP );
BOOL DATAFILE_Open( char *filename, FILE **FP );
BOOL DATAFILE_Open( char *filename, char *mode );
BOOL DATAFILE_Open( char *filename );

BOOL DATAFILE_DiskSpace( char *filename );
#define DATAFILE_DISKSPACE_GB_LIMIT 2.0

BOOL DATAFILE_Check( char *filename );
BOOL DATAFILE_DateTime( char *filename );

BOOL DATAFILE_Open( char *filename, MATDAT &Trial );
BOOL DATAFILE_Open( char *filename, MATDAT &Trial, MATDAT &Frame1 );
BOOL DATAFILE_Open( char *filename, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2 );
BOOL DATAFILE_Open( char *filename, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3 );
BOOL DATAFILE_Open( char *filename, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3, MATDAT &Frame4 );

BOOL DATAFILE_Opened( void );
BOOL DATAFILE_EOF( void );

void DATAFILE_Close( FILE **FP );
void DATAFILE_Close( void );

extern FILE *DATAFILE_FP;

/*****************************************************************************/

struct DATAFILE_Handle
{
    FILE *FP;
    STRING FileName;

    MATDAT *Trial;
    MATDAT *Frame[DATAFILE_FRAMES];
    int FrameBlocks;
};

/*****************************************************************************/

void DATAFILE_ListInitialize( void );
int DATAFILE_ListFind( char *filename );
int DATAFILE_ListFind( FILE *FP );
int DATAFILE_ListFree( void );
void DATAFILE_ListGet( int item );
void DATAFILE_ListSet( int item, char *filename );
BOOL DATAFILE_ListSelect( char *filename );
BOOL DATAFILE_ListAdd( char *filename );
void DATAFILE_ListDelete( char *filename );
void DATAFILE_ListDelete( FILE *FP );

/*****************************************************************************/

BOOL DATAFILE_Save( char *filename, MATDAT &data );

/*****************************************************************************/

#endif


