/******************************************************************************/
/*                                                                            */ 
/* MODULE  : OPTOdata.cpp                                                     */ 
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

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

BOOL    OPTO_FileOpen( char *file, int &frames, int &markers )
{
BOOL    ok=FALSE;
int     rc;
UINT    uFileID=0;
int     nItems;
int     nSubItems;
long    lnFrames;
float   fFrequency;
STRING  szComments;
void   *pFileHeader;

    if( (rc=FileOpen(file,             // File name.
                     uFileID,          // Unique file ID.
                     OPEN_READ,        // Mode of access.
                    &nItems,           // Items in file (markers).
                    &nSubItems,        // Sub-items in file.
                    &lnFrames,         // Frames of data.
                    &fFrequency,       // Frequency of data collection.
                     szComments,       // User comments.
                    &pFileHeader       // Pointer to ND file header.
                     )) == OPTO_RC_OK )
    {
        markers = nItems;
        frames = (int)lnFrames;

        OPTO_debugf("OPTO_FileOpen(file=%s) %d frames, %d markers.\n",file,frames,markers);
        ok = TRUE;
    }
    else
    {        
        OPTO_Fail("FileOpen",rc);
        OPTO_errorf("OPTO_FileOpen(file=%s) Failed.\n",file);
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_FileFrame( int frame, ND3D nd3d[] )
{
BOOL    ok=TRUE;
int     rc;
UINT    uFileID=0;

    if( (rc=FileRead(uFileID,          // File ID.
                     frame,            // Frame offset number.
                     1,                // Frames to read.
                     nd3d              // Data.
                     )) != OPTO_RC_OK )
    {
        OPTO_Fail("FileRead",rc);
        OPTO_errorf("OPTO_FileFrame() Failed.\n");
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/

void    OPTO_FileClose( void )
{
int     rc;
UINT    uFileID=0;

    if( (rc=FileClose(uFileID)) != OPTO_RC_OK )
    {
        OPTO_Fail("FileClose",rc);
        OPTO_errorf("OPTO_FileClose() Failed.\n");
    }
}

/******************************************************************************/

void    OPTO_Frame2Matrix( ND3D nd3d[], matrix &posn, matrix &seen, int frame, int markers )
{
matrix  fposn,fseen;
int     marker;

    // Dimension temporary matrices to hold each frame...
    matrix_dim(fposn,SPMX_PTMX_ROW,markers);
    matrix_dim(fseen,OPTO_SEEN_ROW,markers);

    // Convert single frame from ND format to our format...
    OPTO_ND3d2Posn(nd3d,fposn,fseen,markers,OPTO_RTMX.isnotempty());

    // Put each marker's XYZ position and SEEN flag into the correct frame...
    for( marker=0; (marker < markers); marker++ )
    {
        posn(frame+1,(marker*SPMX_POMX_ROW) + SPMX_POMX_X) = fposn(SPMX_POMX_X,marker+1);
        posn(frame+1,(marker*SPMX_POMX_ROW) + SPMX_POMX_Y) = fposn(SPMX_POMX_Y,marker+1);
        posn(frame+1,(marker*SPMX_POMX_ROW) + SPMX_POMX_Z) = fposn(SPMX_POMX_Z,marker+1);

        seen(frame+1,marker+1) = fseen(OPTO_SEEN_ROW,marker+1);
    }
}

/******************************************************************************/

BOOL    OPTO_File2Matrix( char *file, matrix &posn, matrix &seen, char *rtmx )
{
BOOL    ok;
int     markers,frames,frame;

    // Set default file name... (V2.3)
    file = OPTO_FileDefault(file);
 
    // Set RTMX for rotation / translation...
    if( !OPTO_SetRTMX(rtmx) )
    {
        OPTO_errorf("OPTO_File2Matrix(file=%s,rtmx=%s) Cannot load RTMX.\n",file,rtmx);
        return(FALSE);
    }

    // Open OptoTrak data file...
    if( !OPTO_FileOpen(file,frames,markers) )
    {
        OPTO_errorf("OPTO_File2Matrix(file=%s) Cannot open file.\n",file);
        return(FALSE);
    }

    OPTO_debugf("OPTO_File2Matrix(file=%s) frames=%d, markers=%d.\n",file,frames,markers);

    // Dimension application's matrices to hold data...
    matrix_dim(posn,frames,markers*SPMX_POMX_ROW);
    matrix_dim(seen,frames,markers);

    // Loop through file, one frame at a time...
    for( ok=TRUE,frame=0; ((frame < frames) && ok); frame++ )
    {
        // Get a single frame of marker positiosn from the data file...
        if( !(ok=OPTO_FileFrame(frame,OPTO_nd3d)) )
        {
            OPTO_errorf("OPTO_File2Matrix(file=%s) Cannot read file.\n",file);
            continue;
        }

        // Convert single frame of data from ND OptoTrak position to our format...
        OPTO_Frame2Matrix(OPTO_nd3d,posn,seen,frame,markers);
    }

    // Close file...
    OPTO_FileClose();

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_File2Matrix( matrix &posn, matrix &seen, char *rtmx )
{
BOOL    ok;

    ok = OPTO_File2Matrix(OPTO_FILE_NULL,posn,seen,rtmx);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_File2Matrix( char *file, matrix &posn, char *rtmx )
{
static  matrix  seen;
BOOL    ok;

    ok = OPTO_File2Matrix(file,posn,seen,rtmx);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_File2Matrix( matrix &posn, char *rtmx )
{
static  matrix  seen;
BOOL    ok;

    ok = OPTO_File2Matrix(OPTO_FILE_NULL,posn,seen,rtmx);

    return(ok);
}

/******************************************************************************/

void    OPTO_BuffFrame( void *data, ND3D nd3d[], int frame, int markers )
{
NDreal *rp;
int     fs,fo,m;

    rp = (NDreal *)data;                         // Pointer to ND Real Types in data.
    fs = OPTO_BufferFloatsPerFrame(markers);     // Frame size (values per frame).
    fo = frame * fs;                             // Frame offset.

    for( m=0; (m < markers); m++ )
    {
        nd3d[m].x = rp[fo+(m*3)+0];
        nd3d[m].y = rp[fo+(m*3)+1];
        nd3d[m].z = rp[fo+(m*3)+2];
    }
}

/******************************************************************************/

BOOL    OPTO_Buff2Matrix( OPTOMEM *buff, matrix &posn, matrix &seen, char *rtmx, BOOL keep )
{
BOOL    ok;
int     frame;

    // Set default memory buffer... (V2.3)
    buff = OPTO_MemoryDefault(buff);

    // Set RTMX for rotation / translation...
    if( !OPTO_SetRTMX(rtmx) )
    {
        OPTO_errorf("OPTO_Buff2Matrix(rtmx=%s) Cannot load RTMX.\n",rtmx);
        return(FALSE);
    }

    OPTO_debugf("OPTO_Buff2Matrix(...) freq=%.1f time=%.1f frames=%d(%d) markers=%d.\n",
                buff->freq,
                buff->ct,
                buff->collected,
                buff->frames,
                buff->markers);

    // Dimension application's matrices to hold data...
    matrix_dim(posn,buff->collected,buff->markers*SPMX_POMX_ROW);
    matrix_dim(seen,buff->collected,buff->markers);

    // Loop through data, one frame at a time...
    for( ok=TRUE,frame=0; ((frame < buff->collected) && ok); frame++ )
    {
        // Get a single frame of data from the buffer...
        OPTO_BuffFrame(buff->data,OPTO_nd3d,frame,buff->markers);

        // Convert frame to our matrix format...
        OPTO_Frame2Matrix(OPTO_nd3d,posn,seen,frame,buff->markers);
    }

    if( !keep )
    {
        OPTO_BufferFree(buff);
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Buff2Matrix( OPTOMEM *buff, matrix &posn, matrix &seen, char *rtmx )
{
BOOL    ok=TRUE;

    // Convert memory-buffered data to our matrices...
    ok = OPTO_Buff2Matrix(buff,posn,seen,rtmx,FALSE);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Buff2Matrix( matrix &posn, matrix &seen, char *rtmx )
{
BOOL    ok=TRUE;

    // Convert memory-buffered data to our matrices...
    ok = OPTO_Buff2Matrix(OPTO_Buffer,posn,seen,rtmx,FALSE);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Buff2Matrix( OPTOMEM *buff, matrix &posn, char *rtmx )
{
static  matrix  seen;
BOOL    ok;

    // Convert memory-buffered data to our matrices...
    ok = OPTO_Buff2Matrix(buff,posn,seen,rtmx,FALSE);

    return(ok);
}

/******************************************************************************/

BOOL    OPTO_Buff2Matrix( matrix &posn, char *rtmx )
{
static  matrix  seen;
BOOL    ok;

    // Convert memory-buffered data to our matrices...
    ok = OPTO_Buff2Matrix(OPTO_Buffer,posn,seen,rtmx,FALSE);

    return(ok);
}

/******************************************************************************/

BOOL (*OPTO_Buffer2MatrixFunc[])( matrix &posn, matrix &seen, char *rtmx ) =
{
    OPTO_File2Matrix,
    OPTO_Buff2Matrix,
};

/******************************************************************************/

BOOL OPTO_Buffer2Matrix( int type, matrix &posn, matrix &seen, char *rtmx )
{
BOOL ok;

    ok = (*OPTO_Buffer2MatrixFunc[type])(posn,seen,rtmx);

    return(ok);
}

/******************************************************************************/

