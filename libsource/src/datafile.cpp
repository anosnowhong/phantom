/******************************************************************************/
/*                                                                            */
/* MODULE  : DATAFILE.cpp                                                     */
/*                                                                            */
/* PURPOSE : Functions to write experimental data to file.                    */
/*                                                                            */
/* DATE    : 21/Jul/2006                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 21/Jul/2006 - Initial Development of module.                     */
/*                                                                            */
/* V1.1  JNI 26/Apr/2007 - Added functions for starting API, etc.             */
/*                                                                            */
/*                       - Check for disk space at time of file open.         */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "DATAFILE"
#define MODULE_TEXT     "DataFile API"
#define MODULE_DATE     "26/04/2007"
#define MODULE_VERSION  "1.1"
#define MODULE_LEVEL    2

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

PRINTF  DATAFILE_PRN_messgf=NULL;               // General messages printf function.
PRINTF  DATAFILE_PRN_errorf=NULL;               // Error messages printf function.
PRINTF  DATAFILE_PRN_debugf=NULL;               // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int DATAFILE_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(DATAFILE_PRN_messgf,buff));
}

/******************************************************************************/

int DATAFILE_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(DATAFILE_PRN_errorf,buff));
}

/******************************************************************************/

int DATAFILE_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(DATAFILE_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    DATAFILE_API_started=FALSE;                       // API started flag.

/******************************************************************************/

void DATAFILE_API_stop( void )
{
    if( !DATAFILE_API_started  )       // API not started in the first place...
    {
         return;
    }

    DATAFILE_API_started = FALSE;     // Clear started flag.
    MODULE_stop();                    // Register module stop.
}

/******************************************************************************/

BOOL DATAFILE_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
char *buff;
DWORD size;

    if( DATAFILE_API_started )         // Start the API once...
    {
        return(TRUE);
    }

    DATAFILE_PRN_messgf = messgf;      // General API message print function.
    DATAFILE_PRN_errorf = errorf;      // API error message print function.
    DATAFILE_PRN_debugf = debugf;      // Debug information print function.

    DATAFILE_ListInitialize();

    ATEXIT_API(DATAFILE_API_stop);             // Install stop function.
    DATAFILE_API_started = TRUE;               // Set started flag.
    MODULE_start(DATAFILE_PRN_messgf);         // Register module.

    return(TRUE);
}

/******************************************************************************/

BOOL DATAFILE_API_check( void )
{
BOOL ok=TRUE;

    if( DATAFILE_API_started )             // API started...
    {                                  // Start module automatically...
        return(TRUE);
    }

    // Start API...
    ok = DATAFILE_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
    DATAFILE_debugf("DATAFILE_API_check() Start %s.\n",ok ? "OK" : "Failed");

    return(ok);
}

/*****************************************************************************/

BYTE DATAFILE_Buffer[DATAFILE_BUFFER];
int DATAFILE_BufferPointer=0;

FILE *DATAFILE_FP=NULL;

MATDAT *DATAFILE_Trial;
MATDAT *DATAFILE_Frame[DATAFILE_FRAMES];
int DATAFILE_FrameBlocks=0;

/*****************************************************************************/

void DATAFILE_BufferReset( void )
{
    DATAFILE_BufferPointer = 0;
}

/*****************************************************************************/

void DATAFILE_BufferData( BYTE *data )
{
    if( DATAFILE_BufferPointer < DATAFILE_BUFFER )
    {
        DATAFILE_Buffer[DATAFILE_BufferPointer++] = *data;
    }
}

/*****************************************************************************/

void DATAFILE_BufferData( BYTE *data, int length )
{
int i;

    for( i=0; (i < length); i++ )
    {
        DATAFILE_BufferData(&data[i]);
    }
}

/*****************************************************************************/

void DATAFILE_BufferData( int *data )
{
    DATAFILE_BufferData((BYTE *)data,sizeof(int));
}

/*****************************************************************************/

void DATAFILE_BufferData( long *data )
{
    DATAFILE_BufferData((BYTE *)data,sizeof(long));
}

/*****************************************************************************/

void DATAFILE_BufferData( double *data )
{
    DATAFILE_BufferData((BYTE *)data,sizeof(double));
}

/*****************************************************************************/

void DATAFILE_BufferFill( BYTE data, int length )
{
int i;

    for( i=0; (i < length); i++ )
    {
        DATAFILE_BufferData(&data);
    }
}

/*****************************************************************************/

void DATAFILE_BufferType( BYTE type )
{
    DATAFILE_BufferData(&type);
}

/*****************************************************************************/

BOOL DATAFILE_BufferWrite( FILE *FP )
{
BOOL ok=TRUE;

    if( DATAFILE_BufferPointer > 0 )
    {
        ok = DATAFILE_Write(FP,DATAFILE_Buffer,DATAFILE_BufferPointer);
    }

    return(ok);
}

/*****************************************************************************/

void DATAFILE_BufferMATDATVariables( MATDAT *MatDat )
{
int v;
char name[MATDAT_STRLEN];
int items,rows,cols;

    DATAFILE_BufferData((BYTE *)MatDat->ObjectName,MATDAT_STRLEN);
    v = MatDat->GetVariables();
    DATAFILE_BufferData(&v);

    for( v=0; (v < MatDat->GetVariables()); v++ )
    {
        MatDat->GetVariable(v,name,items,rows,cols);
        DATAFILE_BufferData(&items);
        DATAFILE_BufferData(&rows);
        DATAFILE_BufferData(&cols);
        DATAFILE_BufferData((BYTE *)name,MATDAT_STRLEN);
    }
}

/*****************************************************************************/

BOOL DATAFILE_Write( FILE *FP, BYTE *buff, int size )
{
int w;
BOOL ok=TRUE;

    w = fwrite(buff,1,size,FP);
    ok = (w == DATAFILE_BufferPointer);

    if( ok )
    {
        ok = (fflush(FP) == 0);
    }

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( MATDAT &Trial )
{
BOOL ok;

    ok = DATAFILE_Header(DATAFILE_FP,Trial);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( MATDAT &Trial, MATDAT &Frame1 )
{
BOOL ok;

    ok = DATAFILE_Header(DATAFILE_FP,Trial,Frame1);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2 )
{
BOOL ok;

    ok = DATAFILE_Header(DATAFILE_FP,Trial,Frame1,Frame2);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3 )
{
BOOL ok;

    ok = DATAFILE_Header(DATAFILE_FP,Trial,Frame1,Frame2,Frame3);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3, MATDAT &Frame4 )
{
BOOL ok;

    ok = DATAFILE_Header(DATAFILE_FP,Trial,Frame1,Frame2,Frame3,Frame4);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial )
{
BOOL ok;

    ok = DATAFILE_Header(FP,&Trial,NULL,NULL,NULL,NULL);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial, MATDAT &Frame1 )
{
BOOL ok;

    ok = DATAFILE_Header(FP,&Trial,&Frame1,NULL,NULL,NULL);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2 )
{
BOOL ok;

    ok = DATAFILE_Header(FP,&Trial,&Frame1,&Frame2,NULL,NULL);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3 )
{
BOOL ok;

    ok = DATAFILE_Header(FP,&Trial,&Frame1,&Frame2,&Frame3,NULL);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( FILE *FP, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3, MATDAT &Frame4 )
{
BOOL ok;

    ok = DATAFILE_Header(FP,&Trial,&Frame1,&Frame2,&Frame3,&Frame4);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( FILE *FP, MATDAT *Trial, MATDAT *Frame1, MATDAT *Frame2, MATDAT *Frame3, MATDAT *Frame4 )
{
MATDAT *f[DATAFILE_FRAMES];
BOOL ok;

    f[0] = Frame1;
    f[1] = Frame2;
    f[2] = Frame3;
    f[3] = Frame4;

    ok = DATAFILE_Header(FP,Trial,f);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( FILE *FP, MATDAT *Trial, MATDAT *F[] )
{
int f,i;
BYTE b;
BOOL ok;

    for( f=0; (f < DATAFILE_FRAMES); f++ )
    {
        DATAFILE_Frame[f] = NULL;
    }

    for( DATAFILE_FrameBlocks=0; ((F[DATAFILE_FrameBlocks] != NULL) && (DATAFILE_FrameBlocks < DATAFILE_FRAMES)); DATAFILE_FrameBlocks++ );

    DATAFILE_BufferReset();
    DATAFILE_BufferType(DATAFILE_TYPE_HEADER);
    b = DATAFILE_VERSION;
    DATAFILE_BufferData(&b);

    i = Trial->GetColumns();
    DATAFILE_BufferData(&i);
    DATAFILE_BufferMATDATVariables(Trial);

    DATAFILE_Trial = Trial;

    i = DATAFILE_FrameBlocks;
    DATAFILE_BufferData(&i);

    for( f=0; (f < DATAFILE_FrameBlocks); f++ )
    {
        DATAFILE_Frame[f] = F[f];

        i = F[f]->GetRows();
        DATAFILE_BufferData(&i);
        i = F[f]->GetColumns();
        DATAFILE_BufferData(&i);
        DATAFILE_BufferMATDATVariables(F[f]);
    }

    ok = DATAFILE_BufferWrite(FP);

    return(ok);
}

/******************************************************************************/

BOOL DATAFILE_Header( FILE *FP, int TrialColumns, int FrameBlocks, int FrameRows[], int FrameCols[] )
{
int f,r,c;
BOOL ok;

    DATAFILE_BufferReset();
    DATAFILE_BufferType(DATAFILE_TYPE_HEADER);
    DATAFILE_BufferData(&TrialColumns);
    DATAFILE_BufferData(&FrameBlocks);

    for( f=0; (f < FrameBlocks); f++ )
    {
        DATAFILE_BufferData(&FrameRows[f]);
        DATAFILE_BufferData(&FrameCols[f]);
    }

    ok = DATAFILE_BufferWrite(FP);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Header( FILE *FP, int TrialColumns, int FrameBlocks, ... )
{
int f,r[DATAFILE_FRAMES],c[DATAFILE_FRAMES];
va_list argp;
BOOL ok;

    va_start(argp,FrameBlocks);

    for( f=0; (f < FrameBlocks); f++ )
    {
        r[f] = va_arg(argp,int);
        c[f] = va_arg(argp,int);
    }

    va_end(argp);

    ok = DATAFILE_Header(FP,TrialColumns,FrameBlocks,r,c);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Header( int TrialColumns, int FrameBlocks, ... )
{
int f,r[DATAFILE_FRAMES],c[DATAFILE_FRAMES];
va_list argp;
BOOL ok;

    va_start(argp,FrameBlocks);

    for( f=0; (f < FrameBlocks); f++ )
    {
        r[f] = va_arg(argp,int);
        c[f] = va_arg(argp,int);
    }

    va_end(argp);

    ok = DATAFILE_Header(DATAFILE_FP,TrialColumns,FrameBlocks,r,c);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Header( int TrialColumns, int FrameBlocks, int FrameRows[], int FrameCols[] )
{
BOOL ok;

    ok = DATAFILE_Header(DATAFILE_FP,TrialColumns,FrameBlocks,FrameRows,FrameCols);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_SaveTrial( FILE *FP, int TrialNumber, matrix &TrialData )
{
BOOL ok;
int c;
double data;

    DATAFILE_BufferReset();
    DATAFILE_BufferType(DATAFILE_TYPE_TRIAL);
    DATAFILE_BufferData(&TrialNumber);

    for( c=1; (c <= TrialData.cols()); c++ )
    {
        data = TrialData(TrialNumber,c);
        DATAFILE_BufferData(&data);
    }

    ok = DATAFILE_BufferWrite(FP);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_SaveTrial( int TrialNumber, matrix &TrialData )
{
BOOL ok;

    ok = DATAFILE_SaveTrial(DATAFILE_FP,TrialNumber,TrialData);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_SaveTrial( int TrialNumber )
{
BOOL ok;

    ok = DATAFILE_SaveTrial(TrialNumber,*DATAFILE_Trial->Matrix());

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_SaveFrame( FILE *FP, matrix &FrameData, int FrameRows )
{
BOOL ok;
int r,c;
double data;

    DATAFILE_BufferReset();
    DATAFILE_BufferType(DATAFILE_TYPE_FRAME);

    r = FrameRows;
    c = FrameData.cols();

    DATAFILE_BufferData(&r);
    DATAFILE_BufferData(&c);

    ok = DATAFILE_BufferWrite(FP);

    for( r=1; ((r <= FrameRows) && ok); r++ )
    {
        DATAFILE_BufferReset();

        for( c=1; (c <= FrameData.cols()); c++ )
        {
            data = FrameData(r,c);
            DATAFILE_BufferData(&data);
        }

        ok = DATAFILE_BufferWrite(FP);
    }

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_SaveFrame( matrix &FrameData, int FrameRows )
{
BOOL ok;

    ok = DATAFILE_SaveFrame(DATAFILE_FP,FrameData,FrameRows);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_SaveFrame( int FrameBlock )
{
BOOL ok;

    ok = DATAFILE_SaveFrame(*DATAFILE_Frame[FrameBlock]->Matrix(),DATAFILE_Frame[FrameBlock]->GetRow());

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_SaveFrame( void )
{
BOOL ok;
int f;

    for( ok=TRUE,f=0; ((f < DATAFILE_FrameBlocks) && ok); f++ )
    {
        ok = DATAFILE_SaveFrame(*DATAFILE_Frame[f]->Matrix(),DATAFILE_Frame[f]->GetRow());
    }

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_TrialSave( int TrialNumber )
{
BOOL ok;

    if( (ok=DATAFILE_SaveTrial(TrialNumber)) )
    {
        ok = DATAFILE_SaveFrame();
    }

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_DiskSpace( char *filename )
{
char drive=0x00;
double bytes,GB;
BOOL flag=TRUE;

    if( filename[1] == ':' )
    {
        drive = filename[0];
    }

    bytes = FILE_DiskSpace(drive);

    if( bytes >= 0.0 )
    {
        GB = FILE_DiskSpaceGB(bytes);
        flag = (GB >= DATAFILE_DISKSPACE_GB_LIMIT);
        STR_printf(flag,DATAFILE_errorf,printf,"DATAFILE_DiskSpace(%s) %.2lf GB free (Sufficient=%s).\n",filename,GB,STR_YesNo(flag));
    }

    return(flag);
}

/*****************************************************************************/

BOOL DATAFILE_Open( char *filename, char *mode, FILE **FP )
{
BOOL ok=FALSE;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    if( (*FP=fopen(filename,mode)) != NULL )
    {
        ok = TRUE;
    }

    STR_printf(ok,DATAFILE_errorf,DATAFILE_messgf,"DATAFILE_Open(%s) %s.\n",filename,STR_OkFailed(ok));

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Open( char *filename, FILE **FP )
{
BOOL ok;

    ok = DATAFILE_Open(filename,"wb",FP);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Open( char *filename, char *mode )
{
BOOL ok;

    ok = DATAFILE_Open(filename,mode,&DATAFILE_FP);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Open( char *filename )
{
BOOL ok=FALSE;

    ok = DATAFILE_Open(filename,&DATAFILE_FP);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Open( char *filename, MATDAT &Trial )
{
BOOL ok=FALSE;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    if( DATAFILE_Open(filename) )
    {
        if( DATAFILE_Header(Trial) )
        {
            ok = DATAFILE_ListAdd(filename);
        }
    }

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Open( char *filename, MATDAT &Trial, MATDAT &Frame1 )
{
BOOL ok=FALSE;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    if( DATAFILE_Open(filename) )
    {
        if( DATAFILE_Header(Trial,Frame1) )
        {
            ok = DATAFILE_ListAdd(filename);
        }
    }

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Open( char *filename, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2 )
{
BOOL ok=FALSE;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    if( DATAFILE_Open(filename) )
    {
        if( DATAFILE_Header(Trial,Frame1,Frame2) )
        {
            ok = DATAFILE_ListAdd(filename);
        }
    }

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Open( char *filename, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3 )
{
BOOL ok=FALSE;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    if( DATAFILE_Open(filename) )
    {
        if( DATAFILE_Header(Trial,Frame1,Frame2,Frame3) )
        {
            ok = DATAFILE_ListAdd(filename);
        }
    }
    
    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Open( char *filename, MATDAT &Trial, MATDAT &Frame1, MATDAT &Frame2, MATDAT &Frame3, MATDAT &Frame4 )
{
BOOL ok=FALSE;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    if( DATAFILE_Open(filename) )
    {
        if( DATAFILE_Header(Trial,Frame1,Frame2,Frame3,Frame4) )
        {
            ok = DATAFILE_ListAdd(filename);
        }
    }

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Opened( void )
{
BOOL flag;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    flag = (DATAFILE_FP != NULL);

    return(flag);
}

/*****************************************************************************/

BOOL DATAFILE_EOF( void )
{
BOOL flag=FALSE;

    if( DATAFILE_Opened() )
    {
        flag = (feof(DATAFILE_FP) != 0);
    }

    return(flag);
}

/*****************************************************************************/

void DATAFILE_Close( FILE **FP )
{
    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return;
    }

    if( *FP != NULL )
    {
        fclose(*FP);
        *FP = NULL;
    }
}

/*****************************************************************************/

void DATAFILE_Close( void )
{
    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return;
    }

    DATAFILE_ListDelete(DATAFILE_FP);
    DATAFILE_Close(&DATAFILE_FP);
}

/*****************************************************************************/

BOOL DATAFILE_AppendFile( FILE *FP, char *file )
{
BOOL ok=FALSE;
long size;
char *buffer=NULL;
FILE *fptr=NULL;
STRING name;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    if( (size=FILE_Size(file)) == FILE_SIZE_INVALID )
    {
        DATAFILE_errorf("DATAFILE_AppendFile(%d) Cannot find file.\n",file);
        return(FALSE);
    }

    if( (buffer=(char *)malloc(size)) == NULL )
    {
        DATAFILE_errorf("DATAFILE_AppendFile(%d) Cannot allocate memory.\n",file);
        return(FALSE);
    }

    if( (fptr=fopen(file,"rb")) != NULL )
    {
        ok = (fread(buffer,size,1,fptr) == 1);
   }

    fclose(fptr);

    if( !ok )
    {
        DATAFILE_errorf("DATAFILE_AppendFile(%d) Cannot read file.\n",file);
        free(buffer);
        return(FALSE);
    }

    DATAFILE_BufferReset();
    DATAFILE_BufferType(DATAFILE_TYPE_FILE);
    DATAFILE_BufferData(&size);

    memset(name,0,STRLEN);
    strncpy(name,file,STRLEN);
    DATAFILE_BufferData((BYTE *)name,STRLEN);

    if( DATAFILE_BufferWrite(FP) )
    {
        ok = DATAFILE_Write(FP,(BYTE *)buffer,(int)size);
    }

    free(buffer);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_AppendFile( char *file )
{
BOOL ok;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    ok = DATAFILE_AppendFile(DATAFILE_FP,file);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( FILE *FP, double *data, int items )
{
BOOL ok;

    ok = (fread((char *)data,sizeof(double),items,FP) == items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( double *data, int items )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( double *data )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,1);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( FILE *FP, int *data, int items )
{
BOOL ok;

    ok = (fread((char *)data,sizeof(int),items,FP) == items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( int *data, int items )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( int *data )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,1);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( FILE *FP, long *data, int items )
{
BOOL ok;

    ok = (fread((char *)data,sizeof(long),items,FP) == items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( long *data, int items )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( long *data )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,1);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( FILE *FP, BYTE *data, int items )
{
BOOL ok;

    ok = (fread((char *)data,sizeof(BYTE),items,FP) == items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( BYTE *data, int items )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( BYTE *data )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,1);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( FILE *FP, char *data, int items )
{
BOOL ok;

    ok = (fread(data,sizeof(char),items,FP) == items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( char *data, int items )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,items);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ReadData( char *data )
{
BOOL ok;

    ok = DATAFILE_ReadData(DATAFILE_FP,data,1);

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_Check( char *filename )
{
BOOL flag=TRUE;

    // Make sure API is running...
    if( !DATAFILE_API_check() )
    {
        return(FALSE);
    }

    // Check available disk space.
    if( !DATAFILE_DiskSpace(filename) )
    {
        flag = FALSE;
    }
    else
    if( FILE_Exist(filename) )
    {
        printf("%s: Data already exists (ESCape to Exit).\n",filename);
        flag = !KB_ESC(KB_WAIT);
    }

    return(flag);
}

/*****************************************************************************/

BOOL    DATAFILE_ListInitialized=FALSE;
#define DATAFILE_LIST  8
struct  DATAFILE_Handle DATAFILE_ListHandle[DATAFILE_LIST];

/*****************************************************************************/

void DATAFILE_ListInitialize( void )
{
int f;

    if( DATAFILE_ListInitialized )
    {
        return;
    }

    for( f=0; (f < DATAFILE_LIST); f++ )
    {
        DATAFILE_ListHandle[f].FP = NULL;
    }

    DATAFILE_ListInitialized = TRUE;
}

/*****************************************************************************/

int DATAFILE_ListFind( char *filename )
{
int f;
BOOL item;

    DATAFILE_ListInitialize();

    for( item=-1,f=0; (f < DATAFILE_LIST); f++ )
    {
        if( DATAFILE_ListHandle[f].FP != NULL )
        {
            if( strncmp(DATAFILE_ListHandle[f].FileName,filename,STRLEN) == 0 )
            {
                item = f;
                break;
            }
        }
    }

    return(item);
}

/*****************************************************************************/

int DATAFILE_ListFind( FILE *FP )
{
int f;
BOOL item;

    DATAFILE_ListInitialize();

    for( item=-1,f=0; (f < DATAFILE_LIST); f++ )
    {
        if( DATAFILE_ListHandle[f].FP == FP )
        {
            item = f;
            break;
        }
    }

    return(item);
}

/*****************************************************************************/

int DATAFILE_ListFree( void )
{
int item;

    item = DATAFILE_ListFind((FILE *)NULL);

    return(item);
}

/*****************************************************************************/

void DATAFILE_ListGet( int item )
{
int frame;

    DATAFILE_FP = DATAFILE_ListHandle[item].FP;
    DATAFILE_Trial = DATAFILE_ListHandle[item].Trial;
    DATAFILE_FrameBlocks = DATAFILE_ListHandle[item].FrameBlocks;

    for( frame=0; (frame < DATAFILE_FrameBlocks); frame++ )
    {
        DATAFILE_Frame[frame] = DATAFILE_ListHandle[item].Frame[frame];
    }
}

/*****************************************************************************/

void DATAFILE_ListSet( int item, char *filename )
{
int frame;

    memcpy(DATAFILE_ListHandle[item].FileName,filename,STRLEN);

    DATAFILE_ListHandle[item].FP = DATAFILE_FP;
    DATAFILE_ListHandle[item].Trial = DATAFILE_Trial;
    DATAFILE_ListHandle[item].FrameBlocks = DATAFILE_FrameBlocks;

    for( frame=0; (frame < DATAFILE_FrameBlocks); frame++ )
    {
        DATAFILE_ListHandle[item].Frame[frame] = DATAFILE_Frame[frame];
    }
}

/*****************************************************************************/

BOOL DATAFILE_ListSelect( char *filename )
{
int item;
BOOL ok=FALSE;

    if( (item=DATAFILE_ListFind(filename)) != -1 )
    {
        DATAFILE_ListGet(item);
        ok = TRUE;
    }

    return(ok);
}

/*****************************************************************************/

BOOL DATAFILE_ListAdd( char *filename )
{
int item;
BOOL ok=FALSE;

    if( (item=DATAFILE_ListFree()) != -1 )
    {
        DATAFILE_ListSet(item,filename);
        ok = TRUE;
    }

    return(ok);
}

/*****************************************************************************/

void DATAFILE_ListDelete( char *filename )
{
int item;

    if( (item=DATAFILE_ListFind(filename)) != -1 )
    {
        DATAFILE_ListDelete(DATAFILE_ListHandle[item].FP);
    }
}

/*****************************************************************************/

void DATAFILE_ListDelete( FILE *FP )
{
int item;

    if( (item=DATAFILE_ListFind(FP)) != -1 )
    {
        DATAFILE_ListHandle[item].FP = NULL;
        memset(DATAFILE_ListHandle[item].FileName,0,STRLEN);
    }
}

/*****************************************************************************/

BOOL DATAFILE_Save( char *filename, MATDAT &data )
{
BOOL ok;
int row;

    ok = DATAFILE_Open(filename,data);

    for( row=1; ((row <= data.GetRows()) && ok); row++ )
    {
        ok = DATAFILE_TrialSave(row);
    }

    if( DATAFILE_Opened() )
    {
        DATAFILE_Close();
    }

    return(ok);
}

/*****************************************************************************/

void DateString( char *buff )
{
static STRING temp;
static int index[] = { 3,4,2,0,1,5,6,7,-1 };
int i;

    memset(temp,0,STRLEN);
    _strdate(temp);

    for( i=0; (index[i] != -1); i++ )
    {
        buff[i] = temp[index[i]];
    }
}

/*****************************************************************************/

BOOL DATAFILE_DateTime( char *filename )
{
BOOL flag=FALSE;
STRING buff;
int i,j,k,l;
char *p;

    if( (p=strstr(filename,"DDMMYY")) != NULL )
    {
        memset(buff,0,STRLEN);
        DateString(buff);

        for( j=0,i=0; (i < 6); j++,i++ )
        {
            if( buff[j] == '/' )
            {
                j++;
            }

            p[i] = buff[j];
        }   

        flag = TRUE; 
    }

    if( (p=strstr(filename,"YYMMDD")) != NULL )
    {
        memset(buff,0,STRLEN);
        DateString(buff);

        for( l=0,k=2; (k >= 0); k-- )
        {
            for( j=(k*3),i=0; (i < 2); j++,i++,l++ )
            {
                p[l] = buff[j];
            }
        }   

        flag = TRUE; 
    }

    if( (p=strstr(filename,"HHMM")) != NULL )
    {
    	memset(buff,0,STRLEN);
        _strtime(buff);

        for( j=0,i=0; (i < 4); j++,i++ )
        {
            if( buff[j] == ':' )
            {
                j++;
            }

            p[i] = buff[j];
        }   

        flag = TRUE; 
    }

    return(flag);
}

/*****************************************************************************/

