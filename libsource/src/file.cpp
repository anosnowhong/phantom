/******************************************************************************/
/*                                                                            */
/* MODULE  : FILE.cpp                                                         */
/*                                                                            */
/* PURPOSE : File API functions.                                              */
/*                                                                            */
/* DATE    : 08/Feb/2001                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 08/Feb/2001 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 16/Jan/2002 - Added special-case function for calibration files. */
/*                                                                            */
/*                       - Added "MotorCalib" environment variable to over-   */
/*                         ride hard-coded default.                           */
/*                                                                            */
/*                       - Moved default from H:\calib to S:\calib.           */
/*                                                                            */
/* V1.2  JNI 23/Jan/2002 - Added machine-specific to search by computer name. */
/*                                                                            */
/* V1.3  JNI 26/Apr/2007 - Added disk-free-space function.                    */
/*                                                                            */
/* V1.4  JNI 18/May/2015 - Silent function to turn error messages, etc, off.  */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "FILE"
#define MODULE_TEXT     "File API"
#define MODULE_DATE     "18/05/2015"
#define MODULE_VERSION  "1.4"
#define MODULE_LEVEL    2

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

PRINTF  FILE_PRN_messgf=NULL;               // General messages printf function.
PRINTF  FILE_PRN_errorf=NULL;               // Error messages printf function.
PRINTF  FILE_PRN_debugf=NULL;               // Debug information printf function.

PRINTF  FILE_SilentMessgf=NULL;
PRINTF  FILE_SilentErrorf=NULL;
PRINTF  FILE_SilentDebugf=NULL;

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int FILE_messgf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(FILE_PRN_messgf,buff));
}

/******************************************************************************/

int FILE_errorf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(FILE_PRN_errorf,buff));
}

/******************************************************************************/

int FILE_debugf( const char *mask, ... )
{
va_list args;
static  char  buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(FILE_PRN_debugf,buff));
}

/******************************************************************************/

BOOL    FILE_API_started=FALSE;                       // API started flag.

/******************************************************************************/

STRING  FILE_CalibrationPath=FILE_CALIBRATION_SEARCH;   // Calibration path.
STRING  FILE_MachineName="";                          // Machine name.          

/******************************************************************************/

struct STR_TextItem FILE_FuncText[] = { { FILE_LOAD,"Load" },{ FILE_SAVE,"Save" },{ STR_TEXT_ENDOFTABLE } };

/******************************************************************************/

char *FILE_Func( int func )
{
char *text;

    text = STR_TextCode(FILE_FuncText,func);

    return(text);
}

/******************************************************************************/

void FILE_API_stop( void )
{
    if( !FILE_API_started  )          // API not started in the first place...
    {
         return;
    }

    FILE_API_started = FALSE;         // Clear started flag.
    MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL FILE_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
char *buff;
DWORD size;

    if( FILE_API_started )                 // Start the API once...
    {
        return(TRUE);
    }

    FILE_PRN_messgf = messgf;              // General API message print function.
    FILE_PRN_errorf = errorf;              // API error message print function.
    FILE_PRN_debugf = debugf;              // Debug information print function.

    FILE_SilentMessgf = FILE_PRN_messgf;
    FILE_SilentErrorf = FILE_PRN_errorf;
    FILE_SilentDebugf = FILE_PRN_debugf;

    // Default calibration path...
    strncpy(FILE_CalibrationPath,FILE_CALIBRATION_SEARCH,STRLEN);
    memset(FILE_MachineName,0,STRLEN);

    if( !STR_null(buff=CONFIG_CalibrationPath()) )
    {
        strncpy(FILE_CalibrationPath,buff,STRLEN);
    }

    if( !STR_null(buff=CONFIG_ComputerName()) )
    {
        strncpy(FILE_MachineName,buff,STRLEN);
    }

    ATEXIT_API(FILE_API_stop);             // Install stop function.
    FILE_API_started = TRUE;               // Set started flag.
    MODULE_start(FILE_PRN_messgf);         // Register module.

    FILE_messgf("FILE_CalibrationPath[%s]\n",FILE_CalibrationPath);

    return(TRUE);
}

/******************************************************************************/

BOOL FILE_API_check( void )
{
BOOL ok=TRUE;

    if( FILE_API_started )             // API started...
    {                                  // Start module automatically...
        return(TRUE);
    }

    // Start API...
    ok = FILE_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
    FILE_debugf("FILE_API_check() Start %s.\n",ok ? "OK" : "Failed");

    return(ok);
}

/******************************************************************************/

DWORD FILE_Size( char *file )
{
WIN32_FIND_DATA dirent;
HANDLE fh;
DWORD size=FILE_SIZE_INVALID;

    if( (fh=FindFirstFile(file,&dirent)) != INVALID_HANDLE_VALUE )
    {
        FindClose(fh);
        size = dirent.nFileSizeLow;
    }

    return(size);
}

/******************************************************************************/

BOOL FILE_Load( char *file, BYTE **buff, int &size )
{
BOOL ok=FALSE;
DWORD fsize;
FILE *FP=NULL;
char *mode="rb";

    if( !FILE_API_check() )
    {
        return(FALSE);
    }

    if( (fsize=FILE_Size(file)) != FILE_SIZE_INVALID )
    {
        size = fsize;

        if( (FP=fopen(file,mode)) != NULL )
        {
            if( ((*buff)=(BYTE *)malloc(size)) != NULL )
            {
                if( fread(*buff,1,size,FP) == size )
                {
                    ok = TRUE;
                }
                else
                {
                    FILE_errorf("FILE_Load(file=%s) fread() Error.\n",file);

                    free(*buff);
                    *buff = NULL;
                    size = 0;
                }
            }
            else
            {
                FILE_errorf("FILE_Load(file=%s) malloc() Error.\n",file);
            }

            fclose(FP);
        }
        else
        {
            FILE_errorf("FILE_Load(file=%s) fopen() Error.\n",file);
        }
    }
    else
    {
        FILE_errorf("FILE_Load(file=%s) Cannot find file.\n",file);
    }

    return(ok);
}

/******************************************************************************/

BOOL FILE_Exist( char *file )
{
BOOL exist;

    exist = (FILE_Size(file) != FILE_SIZE_INVALID);

    return(exist);
}

/******************************************************************************/

char *FILE_SearchPaths( char *path, char *subpath, char *name )
{
static  STRING  buff;
char **list;
int paths,item,subitem;
char *file=NULL;
BOOL found;
STRING func;

    // Make sure FILE API is started...
    if( !FILE_API_check() )
    {
        return(NULL);
    }

    // First check if local file exists...
    if( FILE_Exist(name) )
    {
        return(name);
    }

    // Now do more complicated search path...

    strncpy(func,STR_stringf("FILE_SearchsPath(path=%s,file=%s)",path,name),sizeof(STRING));

    // Extract list of paths...
    list = STR_tokens(path,";",paths);

    // Make sure paths were parsed...
    if( paths == 0 )
    {
        FILE_errorf("%s Cannot parse paths.\n",func);
        return(NULL);
    }

    for( found=FALSE,item=0; ((item < paths) && !found); item++ )
    {
        for( subitem=0; ((subitem < 2) && !found); subitem++ )
        {
            file = NULL;

            switch( subitem )
            {
                case 0 : // Search for file in path + subpath...
                   if( !STR_null(subpath,STRLEN) )
                   {
                       strncpy(file=buff,STR_stringf("%s\\%s\\%s",list[item],subpath,name),STRLEN);
                   }
                   break;

                case 1 : // Search for file in path...
                   strncpy(file=buff,STR_stringf("%s\\%s",list[item],name),STRLEN);
                   break;
            }

            if( file != NULL )
            {
                found = FILE_Exist(file);
                FILE_debugf("FILE_SearchPath(...) [%02d] %s %s\n",item,file,STR_YesNo(found));
            }
        }
    }

    if( !found )
    {
        FILE_errorf("%s Cannot find file.\n",func);
        return(NULL);
    }

    FILE_messgf("%s OK.\n[%s]\n",func,file);

    return(file);
}


/******************************************************************************/

BOOL FILE_SearchPaths( char *path, char *subpath, char *name, char *file, int size )
{
BOOL ok=FALSE;
char *buff;

    if( (buff=FILE_SearchPaths(path,subpath,name)) != NULL )
    {
        strncpy(file,buff,size);
        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

BOOL FILE_SearchPaths( char *path, char *subpath, char *name, char *file )
{
BOOL ok=FALSE;
char *buff;

    if( (buff=FILE_SearchPaths(path,subpath,name)) != NULL )
    {
        strncpy(file,buff,STRLEN);
        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

char *FILE_SearchPath( char *path, char *name )
{
char *file;

    file = FILE_SearchPaths(path,NULL,name);

    return(file);
}

/******************************************************************************/

BOOL FILE_SearchPath( char *path, char *name, char *file )
{
BOOL ok;

    ok = FILE_SearchPaths(path,NULL,name,file);

    return(ok);
}

/******************************************************************************/

char *FILE_MachinePath( char *machine )
{
static STRING path;

    memset(path,0,STRLEN);

    if( FILE_API_check() )
    {
        strncpy(path,STR_stringf("%s\\%s",FILE_CALIBRATION_PATH,machine),STRLEN);
    }

    return(path);
}

/******************************************************************************/

char *FILE_MachinePath( void )
{
char *path;

    path = FILE_MachinePath(FILE_MachineName);

    return(path);
}

/******************************************************************************/

char *FILE_MachinePath( char *path, char *name )
{
char *file;

    file = FILE_SearchPaths(path,FILE_MachineName,name);

    return(file);
}

/******************************************************************************/

BOOL FILE_MachinePath( char *path, char *name, char *file )
{
BOOL ok;

    ok = FILE_SearchPaths(path,FILE_MachineName,name,file);

    return(ok);
}

/******************************************************************************/

char *FILE_Calibration( char *name )
{
char *file;

    file = FILE_MachinePath(FILE_CalibrationPath,name);

    return(file);
}

/******************************************************************************/

char *FILE_CalibrationSilent( char *name )
{
char *file;

    FILE_Silent(TRUE);  // Stop printing error messages, etc...

    file = FILE_MachinePath(FILE_CalibrationPath,name);

    FILE_Silent(FALSE); // Resume printing error messages, etc...

    return(file);
}

/******************************************************************************/

BOOL FILE_Calibration( char *name, char *file )
{
BOOL ok=FALSE;

    ok = FILE_MachinePath(FILE_CalibrationPath,name,file);

    return(ok);
}

/******************************************************************************/

char *FILE_Path( char *file )
{
char *path;

    path = FILE_Calibration(file);

    return(path);
}

/******************************************************************************/

double FILE_DiskSpace( void )
{
char DriveLetter=0x00;
double space;

    space = FILE_DiskSpace(DriveLetter);

    return(space);
}

/******************************************************************************/

double FILE_DiskSpace( char DriveLetter )
{
struct _diskfree_t driveinfo;
int drive;
unsigned int rc=0;
double space=-1.0;
STRING DriveString;

    // Make sure FILE API is started...
    if( !FILE_API_check() )
    {
        return(space);
    }

    if( DriveLetter == 0x00 )
    {
       drive = 0;
       memset(DriveString,0,STRLEN);
    }
    else
    {
        drive = 1 + (unsigned int)toupper(DriveLetter) - 'A';
        strncpy(DriveString,STR_stringf("%c:",DriveLetter),STRLEN);
    }

    if( (drive < 0) || (drive > 26) )
    {
        FILE_errorf("FILE_DiskSpace(%c:) Invalid drive specified (drive=%d).\n",DriveLetter,drive);
        return(space);
    }

    rc = _getdiskfree(drive,&driveinfo);

    if( rc == 0 )
    {
        space = (double)driveinfo.avail_clusters * (double)driveinfo.sectors_per_cluster * (double)driveinfo.bytes_per_sector;
    }
    else
    {
        FILE_errorf("FILE_DiskSpace(%s) Failed (%s[%d]).\n",DriveString,strerror(errno),errno);
    }

    return(space);
}

/******************************************************************************/

void FILE_DiskSpace( double bytes, double *KB, double *MB, double *GB, double *TB )
{
double kb,mb,gb,tb;
double k=1000;

    kb = bytes / k;
    mb = kb / k;
    gb = mb / k;
    tb = gb / k;

    if( KB != NULL ) *KB = kb;
    if( MB != NULL ) *MB = mb;
    if( GB != NULL ) *GB = gb;
    if( TB != NULL ) *TB = tb;
}

/******************************************************************************/

double FILE_DiskSpaceKB( double bytes )
{
double kb;

    FILE_DiskSpace(bytes,&kb,NULL,NULL,NULL);

    return(kb);

}

/******************************************************************************/

double FILE_DiskSpaceMB( double bytes )
{
double mb;

    FILE_DiskSpace(bytes,NULL,&mb,NULL,NULL);

    return(mb);
}

/******************************************************************************/

double FILE_DiskSpaceGB( double bytes )
{
double gb;

    FILE_DiskSpace(bytes,NULL,NULL,&gb,NULL);

    return(gb);
}

/******************************************************************************/

double FILE_DiskSpaceTB( double bytes )
{
double tb;

    FILE_DiskSpace(bytes,NULL,NULL,NULL,&tb);

    return(tb);
}

/******************************************************************************/

void FILE_Silent( BOOL flag )
{
    if( flag )
    {
        FILE_PRN_messgf = NULL;
        FILE_PRN_errorf = NULL;
        FILE_PRN_debugf = NULL;
    }
    else
    {
        FILE_PRN_messgf = FILE_SilentMessgf;
        FILE_PRN_errorf = FILE_SilentErrorf;
        FILE_PRN_debugf = FILE_SilentDebugf;
    }
}

/******************************************************************************/

