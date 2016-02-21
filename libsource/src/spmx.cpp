/******************************************************************************/
/*                                                                            */
/* MODULE  : SPMX.cpp                                                         */
/*                                                                            */
/* PURPOSE : SPatial MatriX API.                                              */
/*                                                                            */
/* DATE    : 11/Nov/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 11/Nov/2000 - Initial development of module.                     */
/*                                                                            */
/* V1.2  JNI 14/Jun/2001 - Added functions to give RTMX for X,Y,Z angles &    */
/*                         functions to return Euler angles for RTMX.         */
/*                                                                            */
/* V1.3  JNI 27/Jun/2001 - Added (over-loaded) function to find RTMX without  */
/*                         translation (allow rotation only).                 */
/*                                                                            */
/* V1.4  JNI 22/Jan/2002 - Added functions (to do rotations) to convert XYZ   */
/*                         axis into other axis systems.                      */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "SPMX"
#define MODULE_TEXT     "Spatial MatriX API"
#define MODULE_DATE     "22/01/2002"
#define MODULE_VERSION  "1.4"
#define MODULE_LEVEL    1

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

#define SPMX_errorf printf
#define SPMX_debugf printf
#define SPMX_messgf printf

/******************************************************************************/

// Usefull angles in radians...
double  r180=M_PI;
double  r360=2.0*r180;
double  r90=r180/2.0;
double  r45=r90/2.0;
double  r15=r45/3.0;
double  r30=2.0*r15;
double  r60=2.0*r30;
double  r135=r45+r90;
double  r270=3.0*r90;

/******************************************************************************/

void    SPMX_mtx2xyz( matrix &mtx, double &x, double &y, double &z )
{
    if( mtx.cols() == SPMX_3D )
    {
        x = mtx(1,1);
        y = mtx(1,2);
        z = mtx(1,3);
    }
    else
    if( mtx.rows() == SPMX_3D )
    {
        x = mtx(1,1);
        y = mtx(2,1);
        z = mtx(3,1);
    }
}

/******************************************************************************/

void    SPMX_mtx2xyz( matrix &mtx, double xyz[] )
{
    SPMX_mtx2xyz(mtx,xyz[AXIS_X],xyz[AXIS_Y],xyz[AXIS_Z]);
}

/******************************************************************************/

void    SPMX_mtx2xyz( matrix &xyz, float &x, float &y, float &z )
{
double dx,dy,dz;

    SPMX_mtx2xyz(xyz,dx,dy,dz);

    x = dx;
    y = dy;
    z = dz;
}

/******************************************************************************/

void    SPMX_xyz2mtx( float x, float y, float z, matrix &mtx )
{
double dx,dy,dz;

    dx = x;
    dy = y;
    dz = z;

    SPMX_xyz2mtx(dx,dy,dz,mtx);
}

/******************************************************************************/

void    SPMX_xyz2mtx( double xyz[], matrix &mtx )
{
    SPMX_xyz2mtx(xyz[AXIS_X],xyz[AXIS_Y],xyz[AXIS_Z],mtx);
}

/******************************************************************************/

matrix  SPMX_xyz2mtx( float x, float y, float z )
{
MTXRETN mtx(SPMX_POMX_MTX);

    SPMX_xyz2mtx(x,y,z,mtx);

    return(mtx);
}

/******************************************************************************/

void    SPMX_xyz2mtx( double x, double y, double z, matrix &xyz )
{
    if( xyz.isempty() )
    {
        xyz.dim(SPMX_POMX_MTX);
    }

    if( xyz.cols() == SPMX_3D )
    {
        xyz(1,1) = x;
        xyz(1,2) = y;
        xyz(1,3) = z;
    }
    else
    if( xyz.rows() == SPMX_3D )
    {
        xyz(1,1) = x;
        xyz(2,1) = y;
        xyz(3,1) = z;
    }
}

/******************************************************************************/

matrix SPMX_xyz2mtx( double x, double y, double z )
{
MTXRETN mtx(SPMX_POMX_MTX);

    SPMX_xyz2mtx(x,y,z,mtx);

    return(mtx);
}

/******************************************************************************/

matrix SPMX_xyz2mtx( double xyz[] )
{
MTXRETN mtx(SPMX_POMX_MTX);

    SPMX_xyz2mtx(xyz,mtx);

    return(mtx);
}

/******************************************************************************/

void    SPMX_xyz2pomx( float x, float y, float z, matrix &pomx )
{
    pomx.dim(SPMX_POMX_MTX);

    pomx(SPMX_POMX_XC) = (double)x;
    pomx(SPMX_POMX_YC) = (double)y;
    pomx(SPMX_POMX_ZC) = (double)z;
}

/******************************************************************************/

matrix  SPMX_xyz2pomx( float x, float y, float z )
{
static  matrix  pomx(SPMX_POMX_MTX);

    SPMX_xyz2pomx(x,y,z,pomx);

    return(pomx);
}

/******************************************************************************/

void    SPMX_xyz2pomx( float xyz[], matrix &pomx )
{
int     i=0;

    pomx.dim(SPMX_POMX_MTX);

    pomx(SPMX_POMX_XC) = (double)xyz[i++];
    pomx(SPMX_POMX_YC) = (double)xyz[i++];
    pomx(SPMX_POMX_ZC) = (double)xyz[i++];
}

/******************************************************************************/

void    SPMX_pomx2xyz( matrix &pomx, float &x, float &y, float &z )
{
    x = pomx.F(SPMX_POMX_XC);
    y = pomx.F(SPMX_POMX_YC);
    z = pomx.F(SPMX_POMX_ZC);
}

/******************************************************************************/

void    SPMX_pomx2xyz( matrix &pomx, float xyz[] )
{
int     i=0;

    xyz[i++] = pomx.F(SPMX_POMX_XC);
    xyz[i++] = pomx.F(SPMX_POMX_YC);
    xyz[i++] = pomx.F(SPMX_POMX_ZC);
}

/******************************************************************************/

void    SPMX_xyz2ptmx( float x, float y, float z, matrix &ptmx )
{
    ptmx.dim(SPMX_PTMX_MTX);

    ptmx(SPMX_PTMX_XC) = (double)x;
    ptmx(SPMX_PTMX_YC) = (double)y;
    ptmx(SPMX_PTMX_ZC) = (double)z;
    ptmx(SPMX_PTMX_TC) = 1.0;
}

/******************************************************************************/

void    SPMX_ptmx2xyz( matrix &ptmx, float &x, float &y, float &z )
{
    x = ptmx.F(SPMX_PTMX_XC);
    y = ptmx.F(SPMX_PTMX_YC);
    z = ptmx.F(SPMX_PTMX_ZC);
}

/******************************************************************************/

void    SPMX_xyz2eamx( float x, float y, float z, matrix &eamx )
{
    eamx.dim(SPMX_EAMX_MTX);

    eamx(SPMX_EAMX_XC) = (double)x;
    eamx(SPMX_EAMX_YC) = (double)y;
    eamx(SPMX_EAMX_ZC) = (double)z;
}

/******************************************************************************/

void    SPMX_xyz2eamx( float xyz[], matrix &eamx )
{
int     i=0;

    eamx.dim(SPMX_EAMX_MTX);

    eamx(SPMX_EAMX_XC) = (double)xyz[i++];
    eamx(SPMX_EAMX_YC) = (double)xyz[i++];
    eamx(SPMX_EAMX_ZC) = (double)xyz[i++];
}

/******************************************************************************/

void    SPMX_eamx2xyz( matrix &eamx, float &x, float &y, float &z )
{
    x = eamx.F(SPMX_EAMX_XC);
    y = eamx.F(SPMX_EAMX_YC);
    z = eamx.F(SPMX_EAMX_ZC);
}

/******************************************************************************/

void    SPMX_pomx2ptmx( matrix &pomx, matrix &ptmx )
{
int     c;

    ptmx.dim(SPMX_PTMX_ROW,pomx.cols());

    for( c=1; (c <= pomx.cols()); c++ )
    {
        ptmx(SPMX_PTMX_X,c) = pomx(SPMX_POMX_X,c);
        ptmx(SPMX_PTMX_Y,c) = pomx(SPMX_POMX_Y,c);
        ptmx(SPMX_PTMX_Z,c) = pomx(SPMX_POMX_Z,c);
        ptmx(SPMX_PTMX_T,c) = 1.0;
    }
}

/******************************************************************************/

void    SPMX_ptmx2pomx( matrix &ptmx, matrix &pomx )
{
int     c;

    pomx.dim(SPMX_POMX_ROW,ptmx.cols());

    for( c=1; (c <= ptmx.cols()); c++ )
    {
        pomx(SPMX_POMX_X,c) = ptmx(SPMX_PTMX_X,c);
        pomx(SPMX_POMX_Y,c) = ptmx(SPMX_PTMX_Y,c);
        pomx(SPMX_POMX_Z,c) = ptmx(SPMX_PTMX_Z,c);
    }
}

/******************************************************************************/

void    SPMX_poea2pemx( matrix &pomx, matrix &eamx, matrix &pemx )
{
    pemx.dim(SPMX_PEMX_MTX);

    pemx(SPMX_PEMX_PXC) = pomx(SPMX_POMX_XC);
    pemx(SPMX_PEMX_PYC) = pomx(SPMX_POMX_YC);
    pemx(SPMX_PEMX_PZC) = pomx(SPMX_POMX_ZC);

    pemx(SPMX_PEMX_EXC) = eamx(SPMX_EAMX_XC);
    pemx(SPMX_PEMX_EYC) = eamx(SPMX_EAMX_YC);
    pemx(SPMX_PEMX_EZC) = eamx(SPMX_EAMX_ZC);
}

/******************************************************************************/

void    SPMX_pemx2poea( matrix &pemx, matrix &pomx, matrix &eamx )
{
    pomx.dim(SPMX_POMX_MTX);
    eamx.dim(SPMX_EAMX_MTX);

    pomx(SPMX_POMX_XC) = pemx(SPMX_PEMX_PXC);
    pomx(SPMX_POMX_YC) = pemx(SPMX_PEMX_PYC);
    pomx(SPMX_POMX_ZC) = pemx(SPMX_PEMX_PZC);

    eamx(SPMX_EAMX_XC) = pemx(SPMX_PEMX_EXC);
    eamx(SPMX_EAMX_YC) = pemx(SPMX_PEMX_EYC);
    eamx(SPMX_EAMX_ZC) = pemx(SPMX_PEMX_EZC);
}

/******************************************************************************/

void    SPMX_xyz2romx( double x, double y, double z, matrix &romx )
{
double  sX,sY,sZ;
double  cX,cY,cZ;

    sX = sin(x);
    sY = sin(y);
    sZ = sin(z);

    cX = cos(x);
    cY = cos(y);
    cZ = cos(z);

    romx.dim(SPMX_ROMX_MTX);

//  FOB Manual...
//  romx(1,1) = cY*cZ;            romx(1,2) = cY*sZ;            romx(1,3) =-sZ;
//  romx(2,1) =-cX*sZ+sX*sY*cZ;   romx(2,2) = cX*cZ+sX*sY*sZ;   romx(2,3) = sX*cY;
//  romx(3,1) = sX*sZ+cX*sY*cZ;   romx(3,2) =-sX*cZ+cX*sY*sZ;   romx(3,3) = cX*cY;

//  Euler Angle Formulas...
    romx(1,1) = cY*cZ;            romx(1,2) =-cY*sZ;            romx(1,3) = sZ;
    romx(2,1) = cZ*sX*sY+cX*sZ;   romx(2,2) = cX*cZ-sX*sY*sZ;   romx(2,3) =-cY*sX;
    romx(3,1) =-cX*cZ*sY*sX*sZ;   romx(3,2) = cZ*sX+cX*sY*sZ;   romx(3,3) = cX*cY;
}

/******************************************************************************/

void    SPMX_xyz2romx( matrix &romx, double &x, double &y, double &z )
{
double  eps=0.00001;
double  sX,cX;

    if( (fabs(romx(1,3)) > eps) && (fabs(romx(2,3)) > eps) )
    {
        x = atan2(romx(2,3),romx(1,3));
        sX = sin(x);
        cX = cos(x);
        y = atan2(cX*romx(1,3)+sX*romx(2,3),romx(3,3));
        z = atan2(-sX*romx(1,1)+cX*romx(2,1),-sX*romx(1,2)+cX*romx(2,2));
    }
    else
    {
        x = 0.0;
        y = atan2(romx(1,3),romx(3,3));
        z = atan2(romx(2,1),romx(2,2));
    }
}

/******************************************************************************/

void    SPMX_eamx2romx( matrix &eamx, matrix &romx )
{
    SPMX_xyz2romx(eamx(SPMX_EAMX_XC),eamx(SPMX_EAMX_YC),eamx(SPMX_EAMX_ZC),romx);
}

/******************************************************************************/

void    SPMX_romx2eamx( matrix &romx, matrix &eamx )
{
    eamx.dim(SPMX_EAMX_MTX);
    SPMX_xyz2romx(romx,eamx(SPMX_EAMX_XC),eamx(SPMX_EAMX_YC),eamx(SPMX_EAMX_ZC));
}

/******************************************************************************/

void    SPMX_pemx2rtmx( matrix &pemx, matrix &rtmx )
{
matrix  romx(SPMX_ROMX_MTX);
matrix  pomx(SPMX_POMX_MTX);
matrix  eamx(SPMX_EAMX_MTX);

    SPMX_pemx2poea(pemx,pomx,eamx);
    SPMX_eamx2romx(eamx,romx);
    SPMX_poea2rtmx(pomx,eamx,rtmx);
}

/******************************************************************************/

void    SPMX_rtmx2pemx( matrix &rtmx, matrix &pemx )
{
matrix  pomx(SPMX_POMX_MTX);
matrix  eamx(SPMX_EAMX_MTX);

    SPMX_rtmx2poea(rtmx,pomx,eamx);
    SPMX_poea2pemx(pomx,eamx,pemx);
}

/******************************************************************************/

void    SPMX_poea2rtmx( matrix &pomx, matrix &eamx, matrix &rtmx )
{
matrix  romx(SPMX_ROMX_MTX);

    SPMX_eamx2romx(eamx,romx);
    SPMX_poro2rtmx(pomx,romx,rtmx);
}

/******************************************************************************/

void    SPMX_rtmx2poea( matrix &rtmx, matrix &pomx, matrix &eamx )
{
matrix  romx(SPMX_ROMX_MTX);

    SPMX_rtmx2poro(rtmx,pomx,romx);
    SPMX_romx2eamx(romx,eamx);
}

/******************************************************************************/

void    SPMX_poro2rtmx( matrix &pomx, matrix &romx, matrix &rtmx )
{
matrix  rTmx(SPMX_RTMX_T,0.0,0.0,0.0,1.0),temp;

    matrix_concat_col(temp,romx,pomx);
    matrix_concat_row(rtmx,temp,rTmx);
}

/******************************************************************************/

void    SPMX_romx2rtmx( matrix &romx, matrix &rtmx )
{
matrix  pomx(SPMX_POMX_MTX);

    SPMX_poro2rtmx(pomx,romx,rtmx);
}

/******************************************************************************/

void    SPMX_pomx2rtmx( matrix &pomx, matrix &rtmx )
{
matrix  romx(SPMX_ROMX_MTX);

    romx = SPMX_ROMX_NULL;
    SPMX_poro2rtmx(pomx,romx,rtmx);
}
/******************************************************************************/

void    SPMX_ptmx2rtmx( matrix &ptmx, matrix &rtmx )
{
matrix  romx(SPMX_ROMX_MTX);

    romx = SPMX_ROMX_NULL;
    SPMX_poro2rtmx(SPMX_ptmx2pomx(ptmx),romx,rtmx);
}

/******************************************************************************/

void    SPMX_rtmx2poro( matrix &rtmx, matrix &pomx, matrix &romx )
{
    pomx.dim(SPMX_POMX_MTX);
    matrix_extract_sub(pomx,rtmx,1,4,3,4);

    romx.dim(SPMX_ROMX_MTX);
    matrix_extract_sub(romx,rtmx,1,1,3,3);
}

/******************************************************************************/

void    SPMX_rtmx2pomx( matrix &rtmx, matrix &pomx )
{
    pomx.dim(SPMX_POMX_MTX);
    matrix_extract_sub(pomx,rtmx,1,4,3,4);
}
/******************************************************************************/

void    SPMX_rtmx2ptmx( matrix &rtmx, matrix &ptmx )
{
    ptmx.dim(SPMX_PTMX_MTX);
    matrix_extract_sub(ptmx,rtmx,1,4,4,4);
    ptmx(4,1) = 1.0;
}

/******************************************************************************/

void    SPMX_rtmx2romx( matrix &rtmx, matrix &romx )
{
    romx.dim(SPMX_ROMX_MTX);
    matrix_extract_sub(romx,rtmx,1,1,3,3);
}

/*****************************************************************************/

BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, BOOL translation )
{
matrix  p1,p2;
matrix  c1,c2;
matrix  d1,d2;
matrix  c,u,s,v;
matrix  R,D,trans;
matrix  rTmx(SPMX_RTMX_T,0.0,0.0,0.0,1.0);
int i;

    // Check that columns match between sets of XYZ co-ordinates...
    if( xyz1.cols() != xyz2.cols() )
    {
        SPMX_errorf("SPMX_rtmx(...) Columns must match xyz2 & xyz1.\n");
        return(FALSE);
    }

    // Check for correct number of rows...
    if( xyz1.rows() == SPMX_PTMX_ROW )      // Remove "Translation" row...
    {
        p1 = xyz1(1,1,3,xyz1.cols());
    }
    else
    if( xyz1.rows() == SPMX_POMX_ROW )      // Fine just as is (so copy)...
    {
        p1 = xyz1;
    }
    else
    {
        SPMX_errorf("SPMX_rtmx(...) Invalid number of rows in xyz1.\n");
        return(FALSE);
    }

    // Check for correct number of rows...
    if( xyz2.rows() == SPMX_PTMX_ROW )      // Remove "Translation" row...
    {
        p2 = xyz2(1,1,3,xyz1.cols());
    }
    else
    if( xyz2.rows() == SPMX_POMX_ROW )      // Fine just as is (so copy)...
    {
        p2 = xyz2;
    }
    else
    {
        SPMX_errorf("SPMX_rtmx(...) Invalid number of rows in xyz2.\n");
        return(FALSE);
    }

    // If translation allowed...
    if( translation )        // ...calculate centre (mean) for x,y,z points...
    {
        matrix_mean_row(c1,p1);
        matrix_mean_row(c2,p2);
    }
    else                     // ...otherwise set means to zero...
    {
        c1.dim(p1.rows(),1);
        c2.dim(p2.rows(),1);
    }

    // Calculate difference matrix for x,y,z points from centre...

    // Matrix addition/subtraction was changed so that the operation below stopped working...
    // d1 = p1 - c1;
    // d2 = p2 - c2;
    d1.dim(p1);
    d2.dim(p2);

    // Must do each element separately...
    for( i=1; (i <= p1.cols()); i++ )
    {
        d1(1,i) = p1(1,i) - c1(1,1);
        d1(2,i) = p1(2,i) - c1(2,1);
        d1(3,i) = p1(3,i) - c1(3,1);

        d2(1,i) = p2(1,i) - c2(1,1);
        d2(2,i) = p2(2,i) - c2(2,1);
        d2(3,i) = p2(3,i) - c2(3,1);
    }

    c = d1 * T(d2);

    svd(c,u,s,v);
    R = v * T(u);

    if( det(R) < 0.0 )
    {
        s = diag(s);
        D = diag(((abs(s) == mmin(abs(s))) * -2.0) + 1.0);
        R = v * D * T(u);
    }

    trans = c2 - R * c1;
    rtmx = R | trans;
    rtmx = rtmx % rTmx;

    return(TRUE);
}

/******************************************************************************/

BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, matrix &xyz3, double &err, BOOL translation )
{
matrix  pomx1(SPMX_POMX_MTX),pomx2(SPMX_POMX_MTX),pomx3(SPMX_POMX_MTX);
matrix  ptmx1(SPMX_PTMX_MTX),ptmx2(SPMX_PTMX_MTX),ptmx3(SPMX_PTMX_MTX);
int     indx;
double  TE,PE;

//  Calculate the Rotation / Translation MatriX (RTMX)...
    if( !SPMX_rtmx(rtmx,xyz1,xyz2,translation) )
    {
        return(FALSE);
    }

//  Dimension destinate matrix to save R/T points...
    xyz3.dim(SPMX_POMX_ROW,xyz1.cols());

//  Check each point in the original position matrix...
    for( TE=0.0,indx=1; (indx <= xyz1.cols()); indx++ )
    {
//      Pull out single positions...
        matrix_array_get(pomx1,xyz1,indx);
        matrix_array_get(pomx2,xyz2,indx);

//      Convert from POMX to PTMX for R/T test...
        SPMX_pomx2ptmx(pomx1,ptmx1);

//      Do the R/T...
        ptmx3 = rtmx * ptmx1;
        SPMX_ptmx2pomx(ptmx3,pomx3);

//      Check how much the R/T position differs from original...
        PE = norm(pomx3-pomx2);
        TE+= PE;

//      Save R/T point...
        matrix_array_put(xyz3,pomx3,indx);        
    }

    err = TE / (double)xyz1.cols();

    return(TRUE);
}

/******************************************************************************/

BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, double &err, BOOL translation )
{
static  matrix  xyz3;

    return(SPMX_rtmx(rtmx,xyz1,xyz2,xyz3,err,translation));
}

/******************************************************************************/

BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2 )
{
    return(SPMX_rtmx(rtmx,xyz1,xyz2,TRUE));
}

/******************************************************************************/

BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, matrix &xyz3, double &err )
{
    return(SPMX_rtmx(rtmx,xyz1,xyz2,xyz3,err,TRUE));
}

/******************************************************************************/

BOOL    SPMX_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2, double &err )
{
matrix  xyz3;

    return(SPMX_rtmx(rtmx,xyz1,xyz2,xyz3,err,TRUE));
}

/******************************************************************************/

void    SPMX_doRT( matrix &rtmx, matrix &pomx )
{
matrix  ptmx(SPMX_PTMX_MTX);

    SPMX_pomx2ptmx(pomx,ptmx);
    ptmx = rtmx * ptmx;
    SPMX_ptmx2pomx(ptmx,pomx);
}

/******************************************************************************/

void    SPMX_doRT( matrix &rtmx, matrix &xyz1, matrix &xyz2 )
{
matrix  pomx(SPMX_POMX_MTX);
int     p;

    xyz2.dim(xyz1);

    for( p=1; (p <= xyz1.cols()); p++ )
    {
        matrix_array_get(pomx,xyz1,p);
        SPMX_doRT(rtmx,pomx);
        matrix_array_put(xyz2,pomx,p);
    }
}

/******************************************************************************/

void    SPMX_doRT( matrix &rtmx, double &x, double &y, double &z )
{
matrix  pomx(SPMX_POMX_MTX);

    SPMX_xyz2mtx(x,y,z,pomx);
    SPMX_doRT(rtmx,pomx);
    SPMX_mtx2xyz(pomx,x,y,z);
}

/******************************************************************************/

void    SPMX_doRT( matrix &rtmx, double xyz[] )
{
matrix  pomx(SPMX_POMX_MTX);

    SPMX_xyz2mtx(xyz,pomx);
    SPMX_doRT(rtmx,pomx);
    SPMX_mtx2xyz(pomx,xyz);
}

/******************************************************************************/

void    SPMX_undoRT( matrix &rtmx, matrix &pomx )
{
matrix  iR;

    iR = inv(rtmx);
    SPMX_doRT(iR,pomx);
}


/******************************************************************************/

void    SPMX_undoRT( matrix &rtmx, matrix &xyz1, matrix &xyz2 )
{
matrix  iR;

    iR = inv(rtmx);
    SPMX_doRT(iR,xyz1,xyz2);
}

/******************************************************************************/

void    SPMX_undoRT( matrix &rtmx, double &x, double &y, double &z )
{
matrix  iR;

    iR = inv(rtmx);
    SPMX_doRT(iR,x,y,z);
}


/******************************************************************************/

void    SPMX_undoRT( matrix &rtmx, double xyz[] )
{
matrix  iR;

    iR = inv(rtmx);
    SPMX_doRT(iR,xyz);
}

/******************************************************************************/

void    SPMX_undoRT( matrix &rtmx, float &x, float &y, float &z )
{
matrix  iR;

    iR = inv(rtmx);
    SPMX_doRT(iR,x,y,z);
}

/******************************************************************************/

void    SPMX_doRT( matrix &rtmx, float &x, float &y, float &z )
{
matrix  pomx(SPMX_POMX_MTX);

    SPMX_xyz2pomx(x,y,z,pomx);
    SPMX_doRT(rtmx,pomx);
    SPMX_pomx2xyz(pomx,x,y,z);
}

/******************************************************************************/

void    SPMX_doR( matrix &romx, matrix &pomx )
{
    pomx = romx * pomx;
}

/******************************************************************************/

void    SPMX_doR( matrix &romx, matrix &xyz1, matrix &xyz2 )
{
matrix  pomx(SPMX_POMX_MTX);
int     p;

    xyz2.dim(xyz1);

    for( p=1; (p <= xyz1.cols()); p++ )
    {
        matrix_array_get(pomx,xyz1,p);
        SPMX_doR(romx,pomx);
        matrix_array_put(xyz2,pomx,p);
    }
}

/******************************************************************************/

void    SPMX_doR( matrix &romx, double &x, double &y, double &z )
{
matrix  pomx(SPMX_POMX_MTX);

    SPMX_xyz2mtx(x,y,z,pomx);
    SPMX_doR(romx,pomx);
    SPMX_mtx2xyz(pomx,x,y,z);
}

/******************************************************************************/

void    SPMX_doR( matrix &romx, double xyz[] )
{
matrix  pomx(SPMX_POMX_MTX);

    SPMX_xyz2mtx(xyz,pomx);
    SPMX_doR(romx,pomx);
    SPMX_mtx2xyz(pomx,xyz);
}

/******************************************************************************/

void    SPMX_doR( matrix &romx, float &x, float &y, float &z )
{
matrix  pomx(SPMX_POMX_MTX);

    SPMX_xyz2mtx(x,y,z,pomx);
    SPMX_doR(romx,pomx);
    SPMX_mtx2xyz(pomx,x,y,z);
}

/******************************************************************************/

void SPMX_romxX( double angle, matrix &R )
{
double c,s;

    R.dim(SPMX_ROMX_MTX);

    c = cos(angle);
    s = sin(angle);

    R(1,1) = 1.0;
    R(2,2) = c;
    R(2,3) =-s;
    R(3,2) = s;
    R(3,3) = c;
}

/******************************************************************************/

matrix  SPMX_romxX( double angle )
{
matrix R(SPMX_ROMX_MTX);

    SPMX_romxX(angle,R);

    return(R);
}

/******************************************************************************/


void SPMX_romxY( double angle, matrix &R )
{
double c,s;

    R.dim(SPMX_ROMX_MTX);

    c = cos(angle);
    s = sin(angle);

    R(1,1) = c;
    R(1,3) = s;
    R(2,2) = 1.0;
    R(3,1) =-s;
    R(3,3) = c;
}

/******************************************************************************/

matrix  SPMX_romxY( double angle )
{
matrix R(SPMX_ROMX_MTX);

    SPMX_romxY(angle,R);

    return(R);
}

/******************************************************************************/

void SPMX_romxZ( double angle, matrix &R )
{
double c,s;

    R.dim(SPMX_ROMX_MTX);

    c = cos(angle);
    s = sin(angle);

    R(1,1) = c;
    R(1,2) =-s;
    R(2,1) = s;
    R(2,2) = c;
    R(3,3) = 1.0;
}

/******************************************************************************/

matrix  SPMX_romxZ( double angle )
{
matrix R(SPMX_ROMX_MTX);

    SPMX_romxZ(angle,R);

    return(R);
}

/******************************************************************************/

matrix  SPMX_rtmxX( double angle )
{
double	c,s;
matrix  rtmx(SPMX_RTMX_MTX);
matrix  pomx(SPMX_POMX_MTX);

    SPMX_poro2rtmx(pomx,SPMX_romxX(angle),rtmx);
    return(rtmx);
}

/******************************************************************************/

matrix  SPMX_rtmxY( double angle )
{
double	c,s;
matrix  rtmx(SPMX_RTMX_MTX);
matrix  pomx(SPMX_POMX_MTX);

    SPMX_poro2rtmx(pomx,SPMX_romxY(angle),rtmx);
    return(rtmx);
}


/******************************************************************************/

matrix  SPMX_rtmxZ( double angle )
{
double	c,s;
matrix  rtmx(SPMX_RTMX_MTX);
matrix  pomx(SPMX_POMX_MTX);

    SPMX_poro2rtmx(pomx,SPMX_romxZ(angle),rtmx);
    return(rtmx);
}

/******************************************************************************/

matrix  SPMX_rtmx2pomx( matrix &rtmx )
{
matrix  pomx;

    SPMX_rtmx2pomx(rtmx,pomx);
    return(pomx);
}

/******************************************************************************/

matrix  SPMX_rtmx2ptmx( matrix &rtmx )
{
matrix  ptmx;

    SPMX_rtmx2ptmx(rtmx,ptmx);
    return(ptmx);
}

/******************************************************************************/

matrix  SPMX_rtmx2romx( matrix &rtmx )
{
matrix  romx;

    SPMX_rtmx2romx(rtmx,romx);
    return(romx);
}

/******************************************************************************/

matrix  SPMX_romx2rtmx( matrix &romx )
{
matrix  rtmx;

    SPMX_romx2rtmx(romx,rtmx);
    return(rtmx);
}

/******************************************************************************/

matrix  SPMX_ptmx2pomx( matrix &ptmx )
{
MTXRETN pomx;

    SPMX_ptmx2pomx(ptmx,pomx);
    return(pomx);
}

/******************************************************************************/

matrix  SPMX_pomx2ptmx( matrix &pomx )
{
MTXRETN ptmx;

    SPMX_pomx2ptmx(pomx,ptmx);
    return(ptmx);
}

/******************************************************************************/

matrix  SPMX_pomx2rtmx( matrix &pomx )
{
MTXRETN rtmx;

    SPMX_pomx2rtmx(pomx,rtmx);
    return(rtmx);
}

/******************************************************************************/

matrix  SPMX_ptmx2rtmx( matrix &ptmx )
{
MTXRETN rtmx;

    SPMX_ptmx2rtmx(ptmx,rtmx);
    return(rtmx);
}

/******************************************************************************/
/* Euler Angles (3x1) from Rotation Matrix (3x3)                              */
/******************************************************************************/

void    SPMX_romx2exyz( matrix &romx, matrix &exyz )
{
double  tX,tY,tZ;

    exyz.dim(SPMX_EAMX_MTX);

    tY = asin(romx(1,3));

    if( tY < r90 )
    {
        if( tY > -r90 )
        {
            tX = atan2(-romx(2,3),romx(3,3));
            tZ = atan2(-romx(1,2),romx(1,1));
        }
        else
        {     // Solution not unique...
            tX = -atan2(romx(2,1),romx(2,2));
            tZ = 0.0;
        }
    }
    else
    {     // Solution not unique...
        tX = atan2(romx(2,1),romx(2,2));
        tZ = 0.0;
    }

    exyz(SPMX_EAMX_XC) = tX;
    exyz(SPMX_EAMX_YC) = tY;
    exyz(SPMX_EAMX_ZC) = tZ;
}

/******************************************************************************/

matrix  SPMX_romx2exyz( matrix &romx )
{
static  matrix  exyz;

    SPMX_romx2exyz(romx,exyz);
    return(exyz);
}

/******************************************************************************/

void    SPMX_romx2exzy( matrix &romx, matrix &exyz )
{
double  tX,tY,tZ;

    exyz.dim(SPMX_EAMX_MTX);

    tZ = asin(-romx(1,2));

    if( tZ < r90 )
    {
        if( tZ > -r90 )
        {
            tX = atan2(romx(3,2),romx(2,2));
            tY = atan2(romx(1,3),romx(1,1));
        }
        else
        {     // Solution not unique...
            tX = -atan2(-romx(3,1),romx(3,3));
            tY = 0.0;
        }
    }
    else
    {     // Solution not unique...
        tX = atan2(-romx(3,1),romx(3,3));
        tY = 0.0;
    }

    exyz(SPMX_EAMX_XC) = tX;
    exyz(SPMX_EAMX_YC) = tY;
    exyz(SPMX_EAMX_ZC) = tZ;
}

/******************************************************************************/

matrix  SPMX_romx2exzy( matrix &romx )
{
static  matrix  exyz;

    SPMX_romx2exzy(romx,exyz);
    return(exyz);
}

/******************************************************************************/

void    SPMX_romx2eyxz( matrix &romx, matrix &exyz )
{
double  tX,tY,tZ;

    exyz.dim(SPMX_EAMX_MTX);

    tX = asin(-romx(2,3));

    if( tX < r90 )
    {
        if( tX > -r90 )
        {
            tY = atan2(romx(1,3),romx(3,3));
            tZ = atan2(romx(2,1),romx(2,2));
        }
        else
        {     // Solution not unique...
            tY = -atan2(-romx(1,2),romx(1,1));
            tZ = 0.0;
        }
    }
    else
    {     // Solution not unique...
        tY = atan2(-romx(1,2),romx(1,1));
        tZ = 0.0;
    }

    exyz(SPMX_EAMX_XC) = tX;
    exyz(SPMX_EAMX_YC) = tY;
    exyz(SPMX_EAMX_ZC) = tZ;
}

/******************************************************************************/

matrix  SPMX_romx2eyxz( matrix &romx )
{
static  matrix  exyz;

    SPMX_romx2eyxz(romx,exyz);
    return(exyz);
}

/******************************************************************************/

void    SPMX_romx2eyzx( matrix &romx, matrix &exyz )
{
double  tX,tY,tZ;

    exyz.dim(SPMX_EAMX_MTX);

    tZ = asin(romx(2,1));

    if( tZ < r90 )
    {
        if( tZ > -r90 )
        {
            tY = atan2(-romx(3,1),romx(1,1));
            tX = atan2(-romx(2,3),romx(2,2));
        }
        else
        {     // Solution not unique...
            tY = -atan2(romx(3,2),romx(3,3));
            tX = 0.0;
        }
    }
    else
    {     // Solution not unique...
        tY = atan2(romx(3,2),romx(3,3));
        tX = 0.0;
    }

    exyz(SPMX_EAMX_XC) = tX;
    exyz(SPMX_EAMX_YC) = tY;
    exyz(SPMX_EAMX_ZC) = tZ;
}

/******************************************************************************/

matrix  SPMX_romx2eyzx( matrix &romx )
{
static  matrix  exyz;

    SPMX_romx2eyzx(romx,exyz);
    return(exyz);
}

/******************************************************************************/

void    SPMX_romx2ezxy( matrix &romx, matrix &exyz )
{
double  tX,tY,tZ;

    exyz.dim(SPMX_EAMX_MTX);

    tX = asin(romx(3,2));

    if( tX < r90 )
    {
        if( tX > -r90 )
        {
            tZ = atan2(-romx(1,2),romx(2,2));
            tY = atan2(-romx(3,1),romx(3,3));
        }
        else
        {     // Solution not unique...
            tZ = -atan2(romx(1,3),romx(1,1));
            tY = 0.0;
        }
    }
    else
    {     // Solution not unique...
        tZ = atan2(romx(1,3),romx(1,1));
        tY = 0.0;
    }

    exyz(SPMX_EAMX_XC) = tX;
    exyz(SPMX_EAMX_YC) = tY;
    exyz(SPMX_EAMX_ZC) = tZ;
}

/******************************************************************************/

matrix  SPMX_romx2ezxy( matrix &romx )
{
static  matrix  exyz;

    SPMX_romx2ezxy(romx,exyz);
    return(exyz);
}


/******************************************************************************/

void    SPMX_romx2ezyx( matrix &romx, matrix &exyz )
{
double  tX,tY,tZ;

    exyz.dim(SPMX_EAMX_MTX);

    tY = asin(romx(3,1));

    if( tY < r90 )
    {
        if( tY > -r90 )
        {
            tZ = atan2(romx(2,1),romx(1,1));
            tX = atan2(romx(3,2),romx(3,3));
        }
        else
        {     // Solution not unique...
            tZ = -atan2(romx(1,2),romx(1,3));
            tX = 0.0;
        }
    }
    else
    {     // Solution not unique...
        tZ = atan2(-romx(1,2),romx(1,3));
        tX = 0.0;
    }

    exyz(SPMX_EAMX_XC) = tX;
    exyz(SPMX_EAMX_YC) = tY;
    exyz(SPMX_EAMX_ZC) = tZ;
}

/******************************************************************************/

matrix  SPMX_romx2ezyx( matrix &romx )
{
static  matrix  exyz;

    SPMX_romx2ezyx(romx,exyz);
    return(exyz);
}

/******************************************************************************/

void  (*SPMX_romx2eamxfunc[])( matrix &romx, matrix &eamx ) =
{
    SPMX_romx2exyz,
    SPMX_romx2exzy,
    SPMX_romx2eyxz,
    SPMX_romx2eyzx,
    SPMX_romx2ezxy,
    SPMX_romx2ezyx,
    NULL
};

/******************************************************************************/

void    SPMX_exyz2romx( matrix &eamx, matrix &romx )
{
    romx = SPMX_romxX(eamx(SPMX_EAMX_XC)) * SPMX_romxY(eamx(SPMX_EAMX_YC)) * SPMX_romxZ(eamx(SPMX_EAMX_ZC));
}

/******************************************************************************/

matrix  SPMX_exyz2romx( matrix &eamx )
{
static  matrix  romx;

    SPMX_exyz2romx(eamx,romx);
    return(romx);
}

/******************************************************************************/

void    SPMX_exzy2romx( matrix &eamx, matrix &romx )
{
    romx = SPMX_romxX(eamx(SPMX_EAMX_XC)) * SPMX_romxZ(eamx(SPMX_EAMX_ZC)) * SPMX_romxY(eamx(SPMX_EAMX_YC));
}

/******************************************************************************/

matrix  SPMX_exzy2romx( matrix &eamx )
{
static  matrix  romx;

    SPMX_exzy2romx(eamx,romx);
    return(romx);
}

/******************************************************************************/

void    SPMX_eyxz2romx( matrix &eamx, matrix &romx )
{
    romx = SPMX_romxY(eamx(SPMX_EAMX_YC)) * SPMX_romxX(eamx(SPMX_EAMX_XC)) * SPMX_romxZ(eamx(SPMX_EAMX_ZC));
}

/******************************************************************************/

matrix  SPMX_eyxz2romx( matrix &eamx )
{
static  matrix  romx;

    SPMX_eyxz2romx(eamx,romx);
    return(romx);
}

/******************************************************************************/

void    SPMX_eyzx2romx( matrix &eamx, matrix &romx )
{
    romx = SPMX_romxY(eamx(SPMX_EAMX_YC)) * SPMX_romxZ(eamx(SPMX_EAMX_ZC)) * SPMX_romxX(eamx(SPMX_EAMX_XC));
}

/******************************************************************************/

matrix  SPMX_eyzx2romx( matrix &eamx )
{
static  matrix  romx;

    SPMX_eyzx2romx(eamx,romx);
    return(romx);
}

/******************************************************************************/

void    SPMX_ezxy2romx( matrix &eamx, matrix &romx )
{
    romx = SPMX_romxZ(eamx(SPMX_EAMX_ZC)) * SPMX_romxX(eamx(SPMX_EAMX_XC)) * SPMX_romxY(eamx(SPMX_EAMX_YC));
}

/******************************************************************************/

matrix  SPMX_ezxy2romx( matrix &eamx )
{
static  matrix  romx;

    SPMX_ezxy2romx(eamx,romx);
    return(romx);
}

/******************************************************************************/

void    SPMX_ezyx2romx( matrix &eamx, matrix &romx )
{
    romx = SPMX_romxZ(eamx(SPMX_EAMX_ZC)) * SPMX_romxY(eamx(SPMX_EAMX_YC)) * SPMX_romxX(eamx(SPMX_EAMX_XC));
}

/******************************************************************************/

matrix  SPMX_ezyx2romx( matrix &eamx )
{
static  matrix  romx;

    SPMX_ezyx2romx(eamx,romx);
    return(romx);
}

/******************************************************************************/

void  (*SPMX_eamx2romxfunc[])( matrix &eamx, matrix &romx ) =
{
    SPMX_exyz2romx,
    SPMX_exzy2romx,
    SPMX_eyxz2romx,
    SPMX_eyzx2romx,
    SPMX_ezxy2romx,
    SPMX_ezyx2romx,
    NULL
};

/******************************************************************************/

struct  STR_TextItem    SPMX_eamx[] =
{
    { SPMX_EAMS_XYZ,"XYZ" },
    { SPMX_EAMS_XZY,"XZY" },
    { SPMX_EAMS_YXZ,"YXZ" },
    { SPMX_EAMS_YZX,"YZX" },
    { SPMX_EAMS_ZXY,"ZXY" },
    { SPMX_EAMS_ZYX,"ZYX" },
    { STR_TEXT_EOT },
};

/******************************************************************************/

int     SPMX_eamxtype( char *text )
{
int     axis;

    if( (axis=STR_TextCode(SPMX_eamx,text)) == STR_TEXT_EOT )
    {
        axis = SPMX_AXIS_NONE;
    }

    return(axis);
}

/******************************************************************************/

struct  STR_TextItem    SPMX_axis[] =
{
    // List of supported axis systems...
    { SPMX_AXIS_XYZ   ,"XYZ"    },
    { SPMX_AXIS_YnXZ  ,"Y-XZ"   },
    { SPMX_AXIS_nZYX  ,"-ZYX"   },
    { SPMX_AXIS_XZnY  ,"XZ-Y"   },
    { SPMX_AXIS_nXnYZ ,"-X-YZ"  },
    { SPMX_AXIS_nXYnZ ,"-XY-Z"  },
    { SPMX_AXIS_XnYnZ ,"X-Y-Z"  },
    { SPMX_AXIS_nYXZ  ,"-YXZ"   },
    { SPMX_AXIS_ZYnX  ,"ZY-X"   },
    { SPMX_AXIS_XnZY  ,"X-ZY"   },
    { SPMX_AXIS_YZX   ,"YZX"    },
    { SPMX_AXIS_nZnXY ,"-Z-XY"  },
    { SPMX_AXIS_ZnXnY ,"Z-X-Y"  },
    { SPMX_AXIS_nZXnY ,"-ZX-Y"  },
    { SPMX_AXIS_ZnYX  ,"Z-YX"   },
    { SPMX_AXIS_nYnXnZ,"-Y-X-Z" },
    { SPMX_AXIS_nXnZnY,"-X-Z-Y" },
    { SPMX_AXIS_YXnZ  ,"YX-Z"   },
    { SPMX_AXIS_nXZY  ,"-XZY"   },
    { SPMX_AXIS_nZnYnX,"-Z-Y-X" },
    { SPMX_AXIS_nYnZX ,"-Y-ZX"  },
    { SPMX_AXIS_YnZnX ,"Y-Z-X"  },
    { SPMX_AXIS_nYZnX ,"-YZ-X"  },
    { SPMX_AXIS_ZXY   ,"ZXY"    },

    // List repeated with 'n' instead of '-'...
    { SPMX_AXIS_XYZ   ,"XYZ" },
    { SPMX_AXIS_YnXZ  ,"YnXZ" },
    { SPMX_AXIS_nZYX  ,"nZYX" },
    { SPMX_AXIS_XZnY  ,"XZnY" },
    { SPMX_AXIS_nXnYZ ,"nXnYZ" },
    { SPMX_AXIS_nXYnZ ,"nXYnZ" },
    { SPMX_AXIS_XnYnZ ,"XnYnZ" },
    { SPMX_AXIS_nYXZ  ,"nYXZ" },
    { SPMX_AXIS_ZYnX  ,"ZYnX" },
    { SPMX_AXIS_XnZY  ,"XnZY" },
    { SPMX_AXIS_YZX   ,"YZX" },
    { SPMX_AXIS_nZnXY ,"nZnXY" },
    { SPMX_AXIS_ZnXnY ,"ZnXnY" },
    { SPMX_AXIS_nZXnY ,"nZXnY" },
    { SPMX_AXIS_ZnYX  ,"ZnYX" },
    { SPMX_AXIS_nYnXnZ,"nYnXnZ" },
    { SPMX_AXIS_nXnZnY,"nXnZnY" },
    { SPMX_AXIS_YXnZ  ,"YXnZ" },
    { SPMX_AXIS_nXZY  ,"nXZY" },
    { SPMX_AXIS_nZnYnX,"nZnYnX" },
    { SPMX_AXIS_nYnZX ,"nYnZX" },
    { SPMX_AXIS_YnZnX ,"YnZnX" },
    { SPMX_AXIS_nYZnX ,"nYZnX" },
    { SPMX_AXIS_ZXY   ,"ZXY" },
    { STR_TEXT_EOT },
//  End code-fragment...
};

/******************************************************************************/

void    SPMX_romx2eamx( matrix &romx, matrix &eamx, int type )
{
    if( type != SPMX_AXIS_NONE )
    {
        (*SPMX_romx2eamxfunc[type])(romx,eamx);
    }
}

/******************************************************************************/

void    SPMX_eamx2romx( matrix &eamx, matrix &romx, int type )
{
    if( type != SPMX_AXIS_NONE )
    {
        (*SPMX_eamx2romxfunc[type])(eamx,romx);
    }
}

/******************************************************************************/

int     SPMX_axistype( char *text )
{
int     axis;

    if( (axis=STR_TextCode(SPMX_axis,text)) == STR_TEXT_EOT )
    {
        axis = SPMX_AXIS_NONE;
    }

    return(axis);
}

/******************************************************************************/

matrix  SPMX_RTMXaxis[SPMX_AXIS_MAX];
matrix  SPMX_ROMXaxis[SPMX_AXIS_MAX];

/******************************************************************************/

void    SPMX_axisinit( matrix *romx )
{
int     axis;
static  BOOL  init=FALSE;

    if( init )
    {
        return;
    }

//  Initialize only once...
    init = TRUE;

//  Set NULL (identity) rotation for defauly XYZ...
    SPMX_ROMXaxis[SPMX_AXIS_XYZ] = SPMX_ROMX_NULL;

//  ROMX versions of rotations to get from XYZ to specific axis systems...
//  The following code-fragment is generated by a program...
    SPMX_ROMXaxis[SPMX_AXIS_YnXZ  ] = SPMX_romxZ(r90 );
    SPMX_ROMXaxis[SPMX_AXIS_nZYX  ] = SPMX_romxY(r90 );
    SPMX_ROMXaxis[SPMX_AXIS_XZnY  ] = SPMX_romxX(r90 );
    SPMX_ROMXaxis[SPMX_AXIS_nXnYZ ] = SPMX_romxZ(r180);
    SPMX_ROMXaxis[SPMX_AXIS_nXYnZ ] = SPMX_romxY(r180);
    SPMX_ROMXaxis[SPMX_AXIS_XnYnZ ] = SPMX_romxX(r180);
    SPMX_ROMXaxis[SPMX_AXIS_nYXZ  ] = SPMX_romxZ(r270);
    SPMX_ROMXaxis[SPMX_AXIS_ZYnX  ] = SPMX_romxY(r270);
    SPMX_ROMXaxis[SPMX_AXIS_XnZY  ] = SPMX_romxX(r270);
    SPMX_ROMXaxis[SPMX_AXIS_YZX   ] = SPMX_romxY(r90 ) * SPMX_romxZ(r90 );
    SPMX_ROMXaxis[SPMX_AXIS_nZnXY ] = SPMX_romxZ(r90 ) * SPMX_romxY(r90 );
    SPMX_ROMXaxis[SPMX_AXIS_ZnXnY ] = SPMX_romxX(r90 ) * SPMX_romxZ(r90 );
    SPMX_ROMXaxis[SPMX_AXIS_nZXnY ] = SPMX_romxY(r90 ) * SPMX_romxX(r90 );
    SPMX_ROMXaxis[SPMX_AXIS_ZnYX  ] = SPMX_romxY(r90 ) * SPMX_romxZ(r180);
    SPMX_ROMXaxis[SPMX_AXIS_nYnXnZ] = SPMX_romxZ(r90 ) * SPMX_romxY(r180);
    SPMX_ROMXaxis[SPMX_AXIS_nXnZnY] = SPMX_romxX(r90 ) * SPMX_romxZ(r180);
    SPMX_ROMXaxis[SPMX_AXIS_YXnZ  ] = SPMX_romxZ(r90 ) * SPMX_romxX(r180);
    SPMX_ROMXaxis[SPMX_AXIS_nXZY  ] = SPMX_romxX(r90 ) * SPMX_romxY(r180);
    SPMX_ROMXaxis[SPMX_AXIS_nZnYnX] = SPMX_romxY(r90 ) * SPMX_romxX(r180);
    SPMX_ROMXaxis[SPMX_AXIS_nYnZX ] = SPMX_romxY(r90 ) * SPMX_romxZ(r270);
    SPMX_ROMXaxis[SPMX_AXIS_YnZnX ] = SPMX_romxZ(r90 ) * SPMX_romxX(r270);
    SPMX_ROMXaxis[SPMX_AXIS_nYZnX ] = SPMX_romxX(r90 ) * SPMX_romxY(r270);
    SPMX_ROMXaxis[SPMX_AXIS_ZXY   ] = SPMX_romxZ(r270) * SPMX_romxY(r270);
//  End code-fragment...

//  Make RTMX versions of ROMX...
    for( axis=0; (axis < SPMX_AXIS_MAX); axis++ )
    {
        SPMX_romx2rtmx(SPMX_ROMXaxis[axis],SPMX_RTMXaxis[axis]);
    }
}

/******************************************************************************/

void    SPMX_axisinit( void )
{
    SPMX_axisinit(NULL);
}

/******************************************************************************/

void    SPMX_axispomx( int axis, matrix &pomx )
{
    SPMX_axisinit();

    if( axis != SPMX_AXIS_NONE )
    {
        pomx = SPMX_ptmx2pomx(SPMX_RTMXaxis[axis] * SPMX_pomx2ptmx(pomx));
    }
}

/******************************************************************************/

void    SPMX_axisromx( int axis, matrix &romx )
{
    SPMX_axisinit();

    if( axis != SPMX_AXIS_NONE )
    {
        romx = SPMX_ROMXaxis[axis] * romx;
    }
}

/******************************************************************************/

void    SPMX_axisrtmx( int axis, matrix &rtmx )
{
    SPMX_axisinit();

    if( axis != SPMX_AXIS_NONE )
    {
        rtmx = SPMX_RTMXaxis[axis] * rtmx;
    }
}

/******************************************************************************/

GLdouble *OpenGL_rtmx2double( matrix &rtmx, BOOL translate )
{
static  GLdouble  m[SPMX_RTMX_VEC];
double *d;
int     i;

    if( translate )
    {
        d = matrix_double(rtmx,MATRIX_BY_COL);
    }
    else
    {
        d = matrix_double(SPMX_romx2rtmx(SPMX_rtmx2romx(rtmx)),MATRIX_BY_COL);
    }

    for( i=0; (i < SPMX_RTMX_VEC); i++ )
    {
        m[i] = d[i];
    }

    return(m);
}

/******************************************************************************/

void    SPMX_qtmx2xyzw( matrix &qtmx, double &x, double &y, double &z, double &w );
void    SPMX_xyzw2qtmx( double x, double y, double z, double w, matrix &qtmx );

double  SPMX_qtmx_magnitude( matrix &qtmx );
void    SPMX_qtmx_normalise( matrix &qtmx, matrix &norm );

void    SPMX_ramx2xyza( matrix &ramx, double &x, double &y, double &z, double &a );
void    SPMX_xyza2ramx( double x, double y, double z, double a, matrix &ramx );

void    SPMX_ramx_normalise( matrix &ramx, matrix &norm );

/******************************************************************************/

void    SPMX_qtmx2xyzw( matrix &qtmx, double &x, double &y, double &z, double &w )
{
    x = qtmx(1,1);
    y = qtmx(2,1);
    z = qtmx(3,1);
    w = qtmx(4,1);
}

/******************************************************************************/

void    SPMX_xyzw2qtmx( double x, double y, double z, double w, matrix &qtmx )
{
    SPMX_DimQTMX(qtmx);

    qtmx(1,1) = x;
    qtmx(2,1) = y;
    qtmx(3,1) = z;
    qtmx(4,1) = w;
}

/******************************************************************************/

double  SPMX_qtmx_magnitude( matrix &qtmx )
{
double  x,y,z,w;
double  magnitude;

    SPMX_qtmx2xyzw(qtmx,x,y,z,w);

    magnitude = sqrt( (w*w) + (x*x) + (y*y) + (z*z) );

    return(magnitude);
}

/******************************************************************************/

void    SPMX_qtmx_normalise( matrix &qtmx, matrix &norm )
{
double  magnitude;

    magnitude = SPMX_qtmx_magnitude(qtmx);
    norm = qtmx / magnitude;
}


/******************************************************************************/

void    SPMX_qtmx2romx( matrix &qtmx, matrix &romx )
{
double  x,y,z,w;
double  xx,xy,xz,xw;
double  yy,yz,yw;
double  zz,zw;

    SPMX_DimROMX(romx);

    SPMX_qtmx2xyzw(qtmx,x,y,z,w);

    xx = x * x;
    xy = x * y;
    xz = x * z;
    xw = x * w;

    yy = y * y;
    yz = y * z;
    yw = y * w;

    zz = z * z;
    zw = z * w;

    romx(1,1) = 1.0 - 2.0 * (yy + zz);
    romx(2,1) =       2.0 * (xy - zw);
    romx(3,1) =       2.0 * (xz + yw);

    romx(1,2) =       2.0 * (xy + zw);
    romx(2,2) = 1.0 - 2.0 * (xx + zz);
    romx(3,2) =       2.0 * (yz - xw);

    romx(1,3) =       2.0 * (xz - yw);
    romx(2,3) =       2.0 * (yz + xw);
    romx(3,3) = 1.0 - 2.0 * (xx + yy);
}

/******************************************************************************/

void    SPMX_qtmx2rtmx( matrix &qtmx, matrix &rtmx )
{
static  matrix  romx;

    SPMX_qtmx2romx(qtmx,romx);
    SPMX_romx2rtmx(romx,rtmx);
}

/******************************************************************************/

void   _SPMX_romx2qtmx( matrix &romx, matrix &qtmx )
{
double  t,s,x=0.0,y=0.0,z=0.0,w=0.0;
double  r11,r22,r33;

    SPMX_DimQTMX(qtmx);

    r11 = romx(1,1);
    r22 = romx(2,2);
    r33 = romx(3,3);

    t = 1.0 + r11 + r22 + r33;

    if( t > 0.00000001 )
    {
        s = sqrt(t) * 2.0;
        x = (romx(2,3) - romx(3,2)) / s;
        y = (romx(3,1) - romx(1,3)) / s;
        z = (romx(1,2) - romx(2,1)) / s;
        w = 0.25 * s;
    }
    else
    if( (r11 > r22) && (r11 > r33) )
    {
        s = sqrt(1.0 + r11 - r22 - r33) * 2.0;
        x = 0.25 * s;
        y = (romx(1,2) + romx(2,1)) / s;
        z = (romx(3,1) + romx(1,3)) / s;
        w = (romx(2,3) - romx(2,2)) / s;
    }
    else
    if( r22 > r33 )
    {
        s = sqrt(1.0 + r22 - r11 - r33) * 2.0;
        x = (romx(1,2) + romx(2,1)) / s;
        y = 0.25 * s;
        z = (romx(2,3) + romx(3,2)) / s;
        w = (romx(3,1) - romx(1,3)) / s;
    }
    else
    {
        s = sqrt(1.0 + r33 - r11 - r22) * 2.0;
        x = (romx(3,1) + romx(1,3)) / s;
        y = (romx(2,3) + romx(3,2)) / s;
        z = 0.25 * s;
        w = (romx(1,2) - romx(2,1)) / s;
    }

    qtmx(1,1) = x;
    qtmx(2,1) = y;
    qtmx(3,1) = z;
    qtmx(4,1) = w;
}

/******************************************************************************/

void    SPMX_romx2qtmx( matrix &romx, matrix &qtmx )
{
static  matrix  rtmx;

    SPMX_romx2rtmx(romx,rtmx);
    SPMX_rtmx2qtmx(rtmx,qtmx);
}

/******************************************************************************/

void    SPMX_rtmx2qtmx( matrix &rtmx, matrix &qtmx )
{
double  t,s,x=0.0,y=0.0,z=0.0,w=0.0;
double *mat;

    mat = matrix_double(rtmx);

    t = 1.0 + mat[0] + mat[5] + mat[10];

    if( t > 0.00000001 )
    {
        s = sqrt(t) * 2.0;
        x = (mat[9] - mat[6]) / s;
        y = (mat[2] - mat[8]) / s;
        z = (mat[4] - mat[1]) / s;
        w = 0.25 * s;
    }
    else
    if( (mat[0] > mat[5]) && (mat[0] > mat[10]) )
    {
        s = sqrt(1.0 + mat[0] - mat[5] - mat[10]) * 2.0;
        x = 0.25 * s;
        y = (mat[4] + mat[1]) / s;
        z = (mat[2] + mat[8]) / s;
        w = (mat[9] - mat[6]) / s;
    }
    else
    if( mat[5] > mat[10] )
    {
        s = sqrt(1.0 + mat[5] - mat[0] - mat[10]) * 2.0;
        x = (mat[4] + mat[1]) / s;
        y = 0.25 * s;
        z = (mat[9] + mat[6]) / s;
        w = (mat[2] - mat[8]) / s;
    }
    else
    {
        s = sqrt(1.0 + mat[10] - mat[0] - mat[5]) * 2.0;
        x = (mat[2] + mat[8]) / s;
        y = (mat[9] + mat[6]) / s;
        z = 0.25 * s;
        w = (mat[4] - mat[1]) / s;
    }

    SPMX_xyzw2qtmx(x,y,z,w,qtmx);
}

/******************************************************************************/

void   _SPMX_rtmx2qtmx( matrix &rtmx, matrix &qtmx )
{
static  matrix  romx;

    SPMX_rtmx2romx(rtmx,romx);
    SPMX_romx2qtmx(romx,qtmx);
}

/******************************************************************************/

void    SPMX_ramx2xyza( matrix &ramx, double &x, double &y, double &z, double &a )
{
    x = ramx(1,1);
    y = ramx(2,1);
    z = ramx(3,1);
    a = ramx(4,1);
}

/******************************************************************************/

void    SPMX_xyza2ramx( double x, double y, double z, double a, matrix &ramx )
{
    SPMX_DimRAMX(ramx);

    ramx(1,1) = x;
    ramx(2,1) = y;
    ramx(3,1) = z;
    ramx(4,1) = a;
}

/******************************************************************************/

void    SPMX_ramx_normalise( matrix &ramx, matrix &norm )
{
    norm = ramx;
}

/******************************************************************************/

void    SPMX_qtmx2ramx( matrix &qtmx, matrix &ramx )
{
static  matrix  norm;
double  x,y,z,w;
double  ca,sa,angle;

    SPMX_qtmx_normalise(qtmx,norm);
    SPMX_qtmx2xyzw(norm,x,y,z,w);

    ca = w;
    angle = acos(ca) * 2.0;
    sa = sqrt(1.0 - ca * ca);

    if( fabs(sa) < 0.0005) sa = 1.0;

    x /= sa;
    y /= sa;
    z /= sa;

    SPMX_xyza2ramx(x,y,z,angle,ramx);
}

/******************************************************************************/

void    SPMX_ramx2qtmx( matrix &ramx, matrix &qtmx )
{
static  matrix  norm;
double  angle,x,y,z,w;
double  sa,ca;

    SPMX_ramx_normalise(ramx,norm);
    SPMX_ramx2xyza(norm,x,y,z,angle);

    sa = sin(angle / 2.0);
    ca = cos(angle / 2.0);

    x *= sa;
    y *= sa;
    z *= sa;
    w = ca;

    SPMX_xyzw2qtmx(x,y,z,w,qtmx);
}

/******************************************************************************/

void    SPMX_ramx2romx( matrix &ramx, matrix &romx )
{
static  matrix  qtmx;

    SPMX_ramx2qtmx(ramx,qtmx);
    SPMX_qtmx2romx(qtmx,romx);
}

/******************************************************************************/

void    SPMX_ramx2rtmx( matrix &ramx, matrix &rtmx )
{
static  matrix  qtmx;

    SPMX_ramx2qtmx(ramx,qtmx);
    SPMX_qtmx2rtmx(qtmx,rtmx);
}

/******************************************************************************/

void    SPMX_romx2ramx( matrix &romx, matrix &ramx )
{
static  matrix  qtmx;

    SPMX_romx2qtmx(romx,qtmx);
    SPMX_qtmx2ramx(qtmx,ramx);
}

/******************************************************************************/

void    SPMX_rtmx2ramx( matrix &rtmx, matrix &ramx )
{
static  matrix  qtmx;

    SPMX_rtmx2qtmx(rtmx,qtmx);
    SPMX_qtmx2ramx(qtmx,ramx);
}

/******************************************************************************/

matrix  SPMX_xyz2rtmx( double x, double y, double z )
{
static  matrix  rtmx,romx;

    SPMX_xyz2romx(x,y,z,romx);
    SPMX_romx2rtmx(romx,rtmx);

    return(rtmx);
}

/******************************************************************************/

double deg0to360( double angle )
{
    while( angle > 360.0 )
    {
        angle -= 360.0;
    }

    while( angle < 0.0 )
    {
        angle += 360.0;
    }
    
    return(angle);
}

/******************************************************************************/

