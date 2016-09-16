/******************************************************************************/

void    FOB_Close( void )
{
    FOB_Close(FOB_ID);
}

/******************************************************************************/

BOOL    FOB_Start( void )
{
    return(FOB_Start(FOB_ID));
}

/******************************************************************************/

void    FOB_Stop( void )
{
    FOB_Stop(FOB_ID);
}

/******************************************************************************/

BOOL    FOB_GetPosn( matrix pomx[], BOOL &fresh )
{
    return(FOB_GetPosn(FOB_ID,pomx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetPosn( matrix pomx[] )
{
    return(FOB_GetPosn(FOB_ID,pomx));
}

/******************************************************************************/

BOOL    FOB_GetAngles( matrix aomx[], BOOL &fresh )
{
    return(FOB_GetAngles(FOB_ID,aomx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetAngles( matrix aomx[] )
{
    return(FOB_GetAngles(FOB_ID,aomx));
}

/******************************************************************************/

BOOL    FOB_GetPosnAngles( matrix pomx[], matrix aomx[], BOOL &fresh )
{
    return(FOB_GetPosnAngles(FOB_ID,pomx,aomx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetPosnAngles( matrix pomx[], matrix aomx[] )
{
    return(FOB_GetPosnAngles(FOB_ID,pomx,aomx));
}

/******************************************************************************/

BOOL    FOB_GetROMX( matrix romx[], BOOL &fresh )
{
    return(FOB_GetROMX(FOB_ID,romx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetROMX( matrix romx[] )
{
    return(FOB_GetROMX(FOB_ID,romx));
}

/******************************************************************************/

BOOL    FOB_GetPosnROMX( matrix pomx[], matrix romx[], BOOL &fresh )
{
    return(FOB_GetPosnROMX(FOB_ID,pomx,romx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetPosnROMX( matrix pomx[], matrix romx[] )
{
    return(FOB_GetPosnROMX(FOB_ID,pomx,romx));
}

/******************************************************************************/

BOOL    FOB_GetRTMX( matrix pomx[], matrix romx[], matrix rtmx[], BOOL &fresh )
{
    return(FOB_GetRTMX(FOB_ID,pomx,romx,rtmx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetRTMX( matrix pomx[], matrix romx[], matrix rtmx[] )
{
    return(FOB_GetRTMX(FOB_ID,pomx,romx,rtmx));
}

/******************************************************************************/

BOOL    FOB_GetRTMX( matrix rtmx[], BOOL &fresh )
{
    return(FOB_GetRTMX(FOB_ID,rtmx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetRTMX( matrix rtmx[] )
{
    return(FOB_GetRTMX(FOB_ID,rtmx));
}

/******************************************************************************/

BOOL    FOB_GetQTMX( matrix qtmx[], BOOL &fresh )
{
    return(FOB_GetQTMX(FOB_ID,qtmx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetQTMX( matrix qtmx[] )
{
    return(FOB_GetQTMX(FOB_ID,qtmx));
}

/******************************************************************************/

BOOL    FOB_GetPosnQTMX( matrix pomx[], matrix qtmx[], BOOL &fresh )
{
    return(FOB_GetPosnQTMX(FOB_ID,pomx,qtmx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetPosnQTMX( matrix pomx[], matrix qtmx[] )
{
    return(FOB_GetPosnQTMX(FOB_ID,pomx,qtmx));
}

/******************************************************************************/

BOOL    FOB_GetData( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], matrix rtmx[], BOOL &fresh )
{
    return(FOB_GetData(FOB_ID,pomx,aomx,romx,qtmx,rtmx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetData( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], BOOL &fresh )
{
    return(FOB_GetData(FOB_ID,pomx,aomx,romx,qtmx,fresh));
}

/******************************************************************************/

BOOL    FOB_GetData( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[], matrix rtmx[] )
{
    return(FOB_GetData(FOB_ID,pomx,aomx,romx,qtmx,rtmx));
}

/******************************************************************************/

BOOL    FOB_GetData( matrix pomx[], matrix aomx[], matrix romx[], matrix qtmx[] )
{
    return(FOB_GetData(FOB_ID,pomx,aomx,romx,qtmx));
}

/******************************************************************************/

BOOL    FOB_Flying( void )
{
    return(FOB_Flying(FOB_ID));
}

/******************************************************************************/

WORD    FOB_PosnScale( void )
{
    return(FOB_PosnScale(FOB_ID));
}

/******************************************************************************/

