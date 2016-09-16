/******************************************************************************/

int     FOB_FrameSize[] =              // Size of data frames for each format...
{
    0,
    sizeof(FOB_RawPosition),
    sizeof(FOB_RawAngles),
    sizeof(FOB_RawMatrix),
    sizeof(FOB_RawPosition) + sizeof(FOB_RawAngles),
    sizeof(FOB_RawPosition) + sizeof(FOB_RawMatrix),
    sizeof(FOB_RawQuaternion),
    sizeof(FOB_RawPosition) + sizeof(FOB_RawQuaternion)
};

/******************************************************************************/

BOOL    FOB_FrameByte( BYTE &data )
{
BOOL     frame=FALSE;

    if( (data & FOB_FRAME_BIT) )
    {
        frame = TRUE;
        data &= (~FOB_FRAME_BIT);
    }

    return(frame);
}

/******************************************************************************/

WORD    FOB_FrameWord( WORD data )
{
UCHAR  *dptr;
WORD    word;

    word = data;    
    dptr = (UCHAR *)&word;

//  Strip of FOB frame bit from LSByte;
    dptr[FOB_LSB] = dptr[FOB_LSB] & (~FOB_FRAME_BIT);

//  Left shift LSByte one bit.
    dptr[FOB_LSB] = dptr[FOB_LSB] << 1;

//  Left shift word one bit.
    word = word << 1;

    return(word);
}

/******************************************************************************/

float   FOB_Raw2Inch( short raw, WORD scale )
{
float   inch;

    inch = ((float)raw / (float)INT15_MAX) * (float)scale;

    return(inch);
}

/******************************************************************************/

float   FOB_Inch2cm( float inch )
{
float   cm,inch2cm=2.542;

    cm = inch * inch2cm;

    return(cm);
}

/******************************************************************************/

float   FOB_Raw2cm( short raw, WORD scale )
{
float   inch,cm;

    inch = FOB_Raw2Inch(raw,scale);
    cm = FOB_Inch2cm(inch);

    return(cm);
}

/******************************************************************************/

float   FOB_Raw2Degree( short raw, short scale )
{
float   degree;

    degree = ((float)raw / (float)INT15_MAX) * (float)scale;

    return(degree);
}

/******************************************************************************/

float   FOB_Degree2Radian( float degree )
{
float   radian,degree2radian=PI/180.0;

    radian = degree * degree2radian;

    return(radian);
}

/******************************************************************************/

float   FOB_Raw2Radian( short raw, short scale )
{
float   degree,radian;

    degree = FOB_Raw2Degree(raw,scale);
    radian = FOB_Degree2Radian(degree);

    return(radian);
}

/******************************************************************************/

float   FOB_Raw2Float( short raw, float scale )
{
float   flt;

    flt = ((float)raw / (float)INT15_MAX) * scale;

    return(flt);
}

/******************************************************************************/

void    FOB_Raw2POMX( FOB_RawPosition *raw, WORD scale, matrix &pomx )
{
    matrix_dim(pomx,SPMX_POMX_DIM);

    pomx(SPMX_POMX_XC) = FOB_Raw2cm(raw->nX,scale);
    pomx(SPMX_POMX_YC) = FOB_Raw2cm(raw->nY,scale);
    pomx(SPMX_POMX_ZC) = FOB_Raw2cm(raw->nZ,scale);
}

/******************************************************************************/

void    FOB_Raw2POMX( int ID, FOB_RawPosition *raw, matrix &pomx )
{
    matrix_dim(pomx,SPMX_POMX_DIM);

    pomx(SPMX_POMX_XC) = FOB_Raw2cm(raw->nX,FOB_Port[ID].scale);
    pomx(SPMX_POMX_YC) = FOB_Raw2cm(raw->nY,FOB_Port[ID].scale);
    pomx(SPMX_POMX_ZC) = FOB_Raw2cm(raw->nZ,FOB_Port[ID].scale);
}

/******************************************************************************/

void    FOB_Raw2DegreeEAMX( FOB_RawAngles *raw, matrix &eamx )
{
    matrix_dim(eamx,SPMX_EAMX_DIM);

    eamx(FOB_ANGLE_AZIM_C) = FOB_Raw2Degree(raw->nAzimuth  ,FOB_ANGLE_SCALE);
    eamx(FOB_ANGLE_ELEV_C) = FOB_Raw2Degree(raw->nElevation,FOB_ANGLE_SCALE);
    eamx(FOB_ANGLE_ROLL_C) = FOB_Raw2Degree(raw->nRoll     ,FOB_ANGLE_SCALE);
}

/******************************************************************************/

void    FOB_Raw2RadianEAMX( FOB_RawAngles *raw, matrix &eamx )
{
    matrix_dim(eamx,SPMX_EAMX_DIM);

    eamx(FOB_ANGLE_AZIM_C) = FOB_Raw2Radian(raw->nAzimuth  ,FOB_ANGLE_SCALE);
    eamx(FOB_ANGLE_ELEV_C) = FOB_Raw2Radian(raw->nElevation,FOB_ANGLE_SCALE);
    eamx(FOB_ANGLE_ROLL_C) = FOB_Raw2Radian(raw->nRoll     ,FOB_ANGLE_SCALE);
}

/******************************************************************************/

void    FOB_Raw2EAMX( int ID, FOB_RawAngles *raw, matrix &eamx )
{
    switch( FOB_AngleUnit() )
    {
        case FOB_ANGLEUNIT_DEGREES :
           FOB_Raw2DegreeEAMX(raw,eamx);
           break;

        case FOB_ANGLEUNIT_RADIANS :
           FOB_Raw2RadianEAMX(raw,eamx);
           break;
    }
}

/******************************************************************************/

void    FOB_Raw2ROMX( FOB_RawMatrix *raw, matrix &romx )
{
int     k,r,c;

    matrix_dim(romx,SPMX_ROMX_DIM);    // Dimension ROMX...

//  Put linear array of FOB values into a 3 x 3 ROMX...
//  FOB manual says row changes most frequently but all our experiments suggest that this is wrong.
//  The following is coded so that column changes most frequently...
    for( k=0,r=1; (r <= SPMX_ROMX_ROW); r++ )
    {
        for( c=1; (c <= SPMX_ROMX_ROW); c++ )
        {
            romx(r,c) = (double)FOB_Raw2Float(raw->n[k++],FOB_MATRIX_SCALE);
        }
    }
}

/******************************************************************************/

void    FOB_Raw2QTMX( FOB_RawQuaternion *raw, matrix &qtmx )
{
int     n;

    matrix_dim(qtmx,SPMX_QTMX_DIM);

    for( n=1; (n <= SPMX_QTMX_ROW); n++ )
    {
        qtmx(n,1) = (double)FOB_Raw2Float(raw->nQ[n-1],FOB_QUATERNION_SCALE);
    }
}

/******************************************************************************/

int     FOB_Data2Angles( WORD data[], FOB_RawAngles *angles )
{
int     d=0;

    angles->nAzimuth   = FOB_FrameWord(data[d++]);
    angles->nElevation = FOB_FrameWord(data[d++]);
    angles->nRoll      = FOB_FrameWord(data[d++]);

    return(d);
}

/******************************************************************************/

int     FOB_Data2Position( WORD data[], FOB_RawPosition *position )
{
int     word=0;

    position->nX = FOB_FrameWord(data[word++]);
    position->nY = FOB_FrameWord(data[word++]);
    position->nZ = FOB_FrameWord(data[word++]);

    return(word);
}

/******************************************************************************/

int     FOB_Data2Matrix( WORD data[], FOB_RawMatrix *mtx )
{
int     n,word=0;

    for( n=0; (n < FOB_MATRIX_N); n++ )
    {
        mtx->n[n] = FOB_FrameWord(data[word++]);
    }

    return(word);
}

/******************************************************************************/

int     FOB_Data2Quaternion( WORD data[], FOB_RawQuaternion *quaternion )
{
int     n,word=0;

    for( n=0; (n < FOB_QUATERNION_N); n++ )
    {
        quaternion->nQ[n] = FOB_FrameWord(data[word++]);
    }

    return(word);
}

/******************************************************************************/

int     FOB_Data2Frame( int ID, int bird, WORD data[], FOB_RawFrame *frame )
{
int     word;

    word = FOB_Data2Frame(FOB_Port[ID].dataformat[bird],data,&frame->reading[bird]);

    return(word);
}

/******************************************************************************/

int     FOB_Data2Frame( BYTE dataformat, WORD data[], FOB_RawReading *reading )
{
int     word=0;
BOOL    ok=TRUE;

    switch( dataformat )
    {
        case FOB_DATAFORMAT_POSITION :
           word += FOB_Data2Position(&data[word],&reading->position);
           break;

        case FOB_DATAFORMAT_ANGLES :
           word += FOB_Data2Angles(&data[word],&reading->angles);
           break;

        case FOB_DATAFORMAT_MATRIX :
           word += FOB_Data2Matrix(&data[word],&reading->matrix);
           break;

        case FOB_DATAFORMAT_POSNANGLES :
           word += FOB_Data2Position(&data[word],&reading->position);
           word += FOB_Data2Angles(&data[word],&reading->angles);
           break;

        case FOB_DATAFORMAT_POSNMATRIX :
           word += FOB_Data2Position(&data[word],&reading->position);
           word += FOB_Data2Matrix(&data[word],&reading->matrix);
           break;

        case FOB_DATAFORMAT_QUATERNION :
           word += FOB_Data2Quaternion(&data[word],&reading->quaternion);
           break;

        case FOB_DATAFORMAT_POSNQUATERNION :
           word += FOB_Data2Position(&data[word],&reading->position);
           word += FOB_Data2Quaternion(&data[word],&reading->quaternion);
           break;

        default :
           FOB_errorf("FOB_Data2Frame(...) Unknown data format (%d).\n",dataformat);
           ok = FALSE;
           break;
    }

    return(ok ? word : 0);
}

/******************************************************************************/

void    FOB_AxisPOMX( matrix &pomx )
{
static  matrix  ptmx,temp;

    SPMX_axispomx(FOB_axis,pomx);

    if( FOB_RTMX.isnotempty() )
    {
        SPMX_pomx2ptmx(pomx,ptmx);
        temp = FOB_RTMX * ptmx;
        SPMX_ptmx2pomx(temp,pomx);
    }
}

/******************************************************************************/

void    FOB_AxisROMX( matrix &romx )
{
static  matrix  ROMX,temp;

    SPMX_axisromx(FOB_axis,romx);

    if( FOB_RTMX.isnotempty() )
    {
        SPMX_rtmx2romx(FOB_RTMX,ROMX);
        temp = ROMX * romx;
        romx = temp;
    }
}

/******************************************************************************/

void    FOB_AxisRTMX( matrix &rtmx )
{
static  matrix  temp;

    SPMX_axisrtmx(FOB_axis,rtmx);

    if( FOB_RTMX.isnotempty() )
    {
        temp = FOB_RTMX * rtmx;
        rtmx = temp;
    }
}

/******************************************************************************/

void    FOB_PutPOMX( int ID, int bird, FOB_RawReading *data, matrix &pomx )
{
    FOB_Raw2POMX(ID,&data->position,pomx);
    FOB_TrackHemisphere(ID,bird,pomx);
    FOB_AxisPOMX(pomx);
}

/******************************************************************************/

void    FOB_PutPOMX( int ID, FOB_RawReading data[], matrix pomx[] )
{
int     bird;

    for( bird=0; (bird < FOB_BirdsOnPort(ID)); bird++ )
    {
        FOB_PutPOMX(ID,bird,&data[bird],pomx[bird]);
    }
}

/******************************************************************************/

void    FOB_PutEAMX( int ID, FOB_RawReading data[], matrix eamx[] )
{
int     bird;

    for( bird=0; (bird < FOB_BirdsOnPort(ID)); bird++ )
    {
        switch( FOB_AngleUnit() )
        {
            case FOB_ANGLEUNIT_DEGREES :
               FOB_Raw2DegreeEAMX(&data[bird].angles,eamx[bird]);
               break;

            case FOB_ANGLEUNIT_RADIANS :
               FOB_Raw2RadianEAMX(&data[bird].angles,eamx[bird]);
               break;
        }
    }
}

/******************************************************************************/

void    FOB_PutROMX( int ID, int bird, FOB_RawReading *data, matrix &romx )
{
    FOB_Raw2ROMX(&data->matrix,romx);
    FOB_AxisROMX(romx);
}

/******************************************************************************/

void    FOB_PutROMX( int ID, FOB_RawReading data[], matrix romx[] )
{
int     bird;

    for( bird=0; (bird < FOB_BirdsOnPort(ID)); bird++ )
    {
        FOB_PutROMX(ID,bird,&data[bird],romx[bird]);
    }
}

/******************************************************************************/

void    FOB_PutQTMX( int ID, FOB_RawReading data[], matrix qtmx[] )
{
int     bird;

    for( bird=0; (bird < FOB_BirdsOnPort(ID)); bird++ )
    {
        FOB_Raw2QTMX(&data[bird].quaternion,qtmx[bird]);
    }
}

/******************************************************************************/

void    FOB_PutRTMX( int ID, int bird, FOB_RawReading *data, matrix &rtmx )
{
static  matrix  pomx,romx;

    matrix_dim(rtmx,SPMX_RTMX_MTX);
    FOB_Raw2POMX(ID,&data->position,pomx);
    FOB_TrackHemisphere(ID,bird,pomx);
    FOB_Raw2ROMX(&data->matrix,romx);
    SPMX_poro2rtmx(pomx,romx,rtmx);
    FOB_AxisRTMX(rtmx);
}

/******************************************************************************/

void    FOB_PutRTMX( int ID, FOB_RawReading data[], matrix rtmx[] )
{
int     bird;

    for( bird=0; (bird < FOB_BirdsOnPort(ID)); bird++ )
    {
        FOB_PutRTMX(ID,bird,&data[bird],rtmx[bird]);
    }
}

/******************************************************************************/

void    FOB_PutSPMX( int ID, int bird, BYTE dataformat, FOB_RawReading *data, matrix *pomx, matrix *eamx, matrix *romx, matrix *qtmx, matrix *rtmx )
{
    switch( dataformat )
    {
        case FOB_DATAFORMAT_POSITION :
           if( pomx == NULL )
           {
               break;
           }

           FOB_PutPOMX(ID,bird,data,*pomx);
           break;

        case FOB_DATAFORMAT_ANGLES :
           if( eamx == NULL )
           {
               break;
           }

           FOB_Raw2EAMX(ID,&data->angles,*eamx);
           break;

        case FOB_DATAFORMAT_MATRIX :
           if( romx == NULL )
           {
               break;
           }

           FOB_PutROMX(ID,bird,data,*romx);
           break;

        case FOB_DATAFORMAT_POSNANGLES :
           if( pomx != NULL )
           {
               FOB_PutPOMX(ID,bird,data,*pomx);
           }

           if( eamx != NULL )
           {
               FOB_Raw2EAMX(ID,&data->angles,*eamx);
           }
           break;

        case FOB_DATAFORMAT_POSNMATRIX :
           if( pomx != NULL )
           {
               FOB_PutPOMX(ID,bird,data,*pomx);
           }

           if( romx != NULL )
           {
               FOB_PutROMX(ID,bird,data,*romx);
           }

           if( rtmx != NULL )
           {
               FOB_PutRTMX(ID,bird,data,*rtmx);
           }
           break;

        case FOB_DATAFORMAT_QUATERNION :
           if( qtmx == NULL )
           {
               break;
           }

           FOB_Raw2QTMX(&data->quaternion,*qtmx);
           break;

        case FOB_DATAFORMAT_POSNQUATERNION :
           if( pomx != NULL )
           {
               FOB_PutPOMX(ID,bird,data,*pomx);
           }

           if( qtmx != NULL )
           {
               FOB_Raw2QTMX(&data->quaternion,*qtmx);
           }
           break;
    }
}

/******************************************************************************/

void    FOB_PutSPMX( int ID, int bird, BYTE dataformat, FOB_RawReading *data, matrix *pomx, matrix *eamx, matrix *romx, matrix *qtmx )
{
    FOB_PutSPMX(ID,bird,dataformat,data,pomx,eamx,romx,qtmx,NULL);
}

/******************************************************************************/

void    FOB_PutSPMX( int ID, FOB_RawReading data[], matrix pomx[], matrix eamx[], matrix romx[], matrix qtmx[], matrix rtmx[] )
{
int     bird;

    for( bird=0; (bird < FOB_BirdsOnPort(ID)); bird++ )
    {
        FOB_PutSPMX(ID,bird,FOB_Port[ID].dataformat[bird],&data[bird],M(pomx,bird),M(eamx,bird),M(romx,bird),M(qtmx,bird),M(rtmx,bird));
    }
}

/******************************************************************************/

BOOL    signflip( double d1, double d2 )
{
BOOL    flip=FALSE;

    if( d1 > 0.0 && d2 < 0.0 )
    {
        flip = TRUE;
    }
    else
    if( d2 > 0.0 && d1 < 0.0 )
    {
        flip = TRUE;
    }

    return(flip);
}

/******************************************************************************/

void    FOB_TrackHemisphere( int ID, int bird, matrix &pomx )
{
int     fixed,flip1,flip2;

    // Tracking not enabled, so do nothing...
    if( !FOB_tracking )
    {
        return;
    }

    // Initialize tracking...
    if( FOB_Port[ID].tracking[bird] == FOB_HEMISPHERE_INIT )
    {
        FOB_Port[ID].tracking[bird] = FOB_Port[ID].hemisphere[bird];
        FOB_Port[ID].pomx[bird] = pomx;

        FOB_debugf("FOB_TrackHemisphere(ID=%d,bird=%d) INIT -> %s.\n",ID,bird,
                    STR_TextCode(FOB_HemisphereText,FOB_Port[ID].tracking[bird]));
        return;
    }

    // If we aren't in home hemisphere, flip signs of axis...
    if( FOB_Port[ID].tracking[bird] != FOB_Port[ID].hemisphere[bird] )
    {
        pomx = -pomx;
    }

    // Set which axis has fixed-sign and which will flip (depends on home hemisphere)...
    switch( FOB_HemisphereAxis[FOB_Port[ID].hemisphere[bird]] )
    {
        case FOB_HEMI_AXIS_X :         // X axis defines hemisphere changes (front / rear)...
           fixed = SPMX_POMX_X;
           flip1 = SPMX_POMX_Y;
           flip2 = SPMX_POMX_Z;
           break;

        case FOB_HEMI_AXIS_Y :         // Y axis defines hemisphere changes (right / left)...
           fixed = SPMX_POMX_Y;
           flip1 = SPMX_POMX_X;
           flip2 = SPMX_POMX_Z;
           break;

        case FOB_HEMI_AXIS_Z :         // Z axis defines hemisphere changes (upper / lower)...
           fixed = SPMX_POMX_Z;
           flip1 = SPMX_POMX_X;
           flip2 = SPMX_POMX_Y;
           break;
    }

    // When bird crosses hemisphere boundary, signs of other 2 axis flip...
    // And the fixed axis will be close to zero...
    if( signflip(FOB_Port[ID].pomx[bird](flip1,1),pomx(flip1,1)) && signflip(FOB_Port[ID].pomx[bird](flip2,1),pomx(flip2,1)) )
    {
        FOB_debugf("FOB_TrackHemisphere(ID=%d,bird=%d) %s -> %s.\n",ID,bird,
                    STR_TextCode(FOB_HemisphereText,FOB_Port[ID].tracking[bird]),
                    STR_TextCode(FOB_HemisphereText,FOB_HemisphereFlip[FOB_Port[ID].tracking[bird]]));

        // Flip back...
        FOB_Port[ID].tracking[bird] = FOB_HemisphereFlip[FOB_Port[ID].tracking[bird]];
        pomx = -pomx;
    }

    // Save current POMX so we can detect sign changes...
    FOB_Port[ID].pomx[bird] = pomx;
}

/******************************************************************************/

