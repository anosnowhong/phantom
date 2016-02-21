/******************************************************************************/

#include <motor.h>
#include <winuser.h>
#include <mmsystem.h>

/******************************************************************************/

BOOL WAVE_Volume( double &volume, BOOL set )
{
static BOOL initialize=FALSE;
static BOOL wavecaps_volume=FALSE;
static BOOL wavecaps_lrvolume=FALSE;
double current=0.0;
WAVEOUTCAPS woc;
MMRESULT rc;
HWAVEOUT hwo=0;
DWORD dwMaxVolume=0xF000;
DWORD dwVolume=0x0000;

    if( !initialize )
    {
        if( (rc=waveOutGetDevCaps(0,&woc,sizeof(woc))) == MMSYSERR_NOERROR )
        {
            wavecaps_volume = ((woc.dwSupport & WAVECAPS_VOLUME) != 0);
            wavecaps_lrvolume = ((woc.dwSupport & WAVECAPS_LRVOLUME) != 0);
        }
        else
        {
            printf("WAVE_Volume() waveOutGetCaps(...) Failed (rc=%ld).\n",rc);
            return(FALSE);
        }

        initialize = TRUE;
    }

    if( !wavecaps_volume )
    {
        return(FALSE);
    }

    if( (rc=waveOutGetVolume(hwo,&dwVolume)) != MMSYSERR_NOERROR )
    {
        printf("WAVE_Volume() waveOutGetVolume(...) Failed (rc=%ld).\n",rc);
        return(FALSE);
    }

    current = (double)(dwVolume & 0xFFFF) / (double)dwMaxVolume;
    printf("WAVE_Volume() Current=%.2lf (0x%08lX)\n",current,dwVolume);

    if( !set )
    {
        volume = current;
        return(TRUE);
    }

    // Clamp between 0 and 1.
    range(volume,0.0,1.0);
    dwVolume = (DWORD)(volume * (double)dwMaxVolume);

    if( wavecaps_lrvolume )
    {
        dwVolume |= (dwVolume << 16);
    }

    if( (rc=waveOutSetVolume(hwo,dwVolume)) != MMSYSERR_NOERROR )
    {
        printf("WAVE_Volume() waveOutSetVolume(...) Failed (rc=%ld).\n",rc);
        return(FALSE);
    }

    printf("WAVE_Volume() Set=%.2lf (0x%08lX)\n",volume,dwVolume);
    return(TRUE);
}

/******************************************************************************/

double WAVE_GetVolume( void )
{
double volume;
BOOL ok;

    ok = WAVE_Volume(volume,FALSE);

    return(volume);
}

/******************************************************************************/

void WAVE_SetVolume( double volume )
{
BOOL ok;

    ok = WAVE_Volume(volume,TRUE);
}

/******************************************************************************/

WAVEPLAY::WAVEPLAY( char *name, char *file )
{
BOOL ok;

    ok = Init(name,file);
}

/******************************************************************************/

WAVEPLAY::WAVEPLAY( char *file )
{
BOOL ok;

    ok = Init(file);
}

/******************************************************************************/

WAVEPLAY::WAVEPLAY( void )
{
BOOL ok;

    ok = Init();
}

/******************************************************************************/

WAVEPLAY::~WAVEPLAY( void )
{
    Free();
}

/******************************************************************************/

BOOL WAVEPLAY::Init( void )
{
BOOL ok=TRUE;

    memset(ObjectName,0,STRLEN);
    memset(FileName,0,STRLEN);
    LoadFlag = FALSE;
    Buffer = NULL;

    return(ok);
}

/******************************************************************************/

BOOL WAVEPLAY::Init( char *name, char *file )
{
BOOL ok=TRUE;

    if( !Init() )
    {
        return(FALSE);
    }

    if( name != NULL )
    {
        strncpy(ObjectName,name,STRLEN);
    }

    if( file != NULL )
    {
        strncpy(FileName,file,STRLEN);
        ok = Load();
    }

    return(ok);
}


/******************************************************************************/

BOOL WAVEPLAY::Init( char *file )
{
BOOL ok;

    ok = Init(NULL,file);

    return(ok);
}

/******************************************************************************/

void WAVEPLAY::Free( void )
{
    if( Buffer != NULL )
    {
        free(Buffer);
        Buffer = NULL;
    }

    LoadFlag = FALSE;
}

/******************************************************************************/

BOOL WAVEPLAY::Load( void )
{
BOOL ok=FALSE;
char *path;
FILE *FP=NULL;
double volume;

    Free();

    if( (path=FILE_Path(FileName)) == NULL )
    {
        WAVEPLAY_errorf("WAVEPLAY::Load(%s) File not found.\n",FileName);
        return(FALSE);
    }

    strncpy(FilePath,path,STRLEN);

    if( (Size=FILE_Size(FilePath)) == FILE_SIZE_INVALID )
    {
        WAVEPLAY_errorf("WAVEPLAY::Load(%s) File not found.\n",FilePath);
        return(FALSE);
    }

    if( (Buffer=(char *)malloc(Size)) == NULL )
    {
        WAVEPLAY_errorf("WAVEPLAY::Load(%s) Not enough memory.\n",FileName);
        return(FALSE);
    }

    if( (FP=fopen(FilePath,"rb")) != NULL )
    {
        ok = (fread(Buffer,Size,1,FP) == 1);
    }

    if( !ok )
    {
        WAVEPLAY_errorf("WAVEPLAY::Load(%s) Cannot read file.\n",FilePath);
        Free();
        return(FALSE);
    }

    STR_printf(ok,WAVEPLAY_debugf,WAVEPLAY_errorf,"WAVEPLAY::Load(%s) %s.\n",FilePath,STR_OkFailed(ok));

    // WAV files are around 10MB per minute, 167KB per second.
    Seconds = ((double)Size / (167.0*1024.0)) * 5.0; // Factor of 4 to be safe.

    LoadFlag = ok;

    if( ok )
    {
        volume = WAVE_GetVolume();
        WAVE_SetVolume(0.0);
        ok = Play();
        TIMER_delay(seconds2milliseconds(Seconds));
        WAVE_SetVolume(volume);
    }

    return(ok);
}

/******************************************************************************/

BOOL WAVEPLAY::Load( char *file )
{
BOOL ok;

    strncpy(FileName,file,STRLEN);
    ok = Load();

    return(ok);
}

/******************************************************************************/

BOOL WAVEPLAY::Play( BOOL wait, BOOL loop )
{
DWORD flags=SND_MEMORY | SND_NODEFAULT;
BOOL ok;

    if( !Loaded() )
    {
        return(FALSE);
    }

    if( wait )
    {
        flags |= SND_SYNC;
    }
    else
    {
        flags |= SND_ASYNC;
    }

    if( loop )
    {
        flags |= SND_LOOP | SND_NOSTOP;
    }

    ok = PlaySound(Buffer,NULL,flags);
   
    STR_printf(ok,WAVEPLAY_debugf,WAVEPLAY_errorf,"WAVEPLAY::Play(%s) %s.\n",FileName,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL WAVEPLAY::Play( void  )
{
BOOL ok;
BOOL wait=FALSE,loop=FALSE;

    ok = Play(wait,loop);

    return(ok);
}

/******************************************************************************/

BOOL WAVEPLAY::PlayWait( void  )
{
BOOL ok;
BOOL wait=TRUE,loop=FALSE;

    ok = Play(wait,loop);

    return(ok);
}

/******************************************************************************/

void WAVEPLAY::Stop( void )
{
BOOL ok;

    ok = PlaySound(NULL,NULL,SND_PURGE);
}

/******************************************************************************/

BOOL WAVEPLAY::Loaded( void )
{
    return(LoadFlag);
}

/******************************************************************************/

BOOL WAVELIST_Load( WAVELIST *list )
{
BOOL ok;

    if( list->Object != NULL )
    {
        return(TRUE);
    }

    list->Object = new WAVEPLAY(list->ObjectName,list->FileName);
    ok = (list->Object)->Loaded();

    if( !ok )
    {
        delete list->Object;
        list->Object = NULL;
    }

    printf("WAVELIST_Load(%s) %s.\n",list->FileName,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL WAVELIST_Open( WAVELIST list[] )
{
int item;
BOOL ok;

    for( ok=TRUE,item=0; !STR_null(list[item].ObjectName); item++ )
    {
        if( !WAVELIST_Load(&list[item]) )
        {
            ok = FALSE;
        }
    }

    return(ok);
}

/******************************************************************************/

void WAVELIST_Close( WAVELIST list[] )
{
int item;

    for( item=0; !STR_null(list[item].ObjectName); item++ )
    {
        delete list[item].Object;
        list[item].Object = NULL;
    }
}

/******************************************************************************/

WAVEPLAY *WAVELIST_Object( WAVELIST list[], char *name )
{
WAVEPLAY *object;
int item;

    for( object=NULL,item=0; !STR_null(list[item].ObjectName); item++ )
    {
        if( strncmp(list[item].ObjectName,name,STRLEN) == 0 )
        {
            object = list[item].Object;
            break;
        }
    }

    return(object);
}

/******************************************************************************/

void WAVELIST_Play( WAVELIST list[], char *name, BOOL wait )
{
WAVEPLAY *object;

    if( (object=WAVELIST_Object(list,name)) != NULL )
    {
        object->Play(wait,FALSE);
    }
}

/******************************************************************************/

void WAVELIST_Play( WAVELIST list[], char *name )
{
    WAVELIST_Play(list,name,FALSE);
}

/******************************************************************************/

void WAVELIST_PlayWait( WAVELIST list[], char *name )
{
    WAVELIST_Play(list,name,TRUE);
}

/******************************************************************************/

