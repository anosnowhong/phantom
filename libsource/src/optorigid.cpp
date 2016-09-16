/******************************************************************************/
/*                                                                            */ 
/* MODULE  : OPTORIGID.cpp                                                    */ 
/*                                                                            */ 
/* PURPOSE : Rigid Body functions for use with OptoTrak Motion Sensor System. */ 
/*                                                                            */ 
/* DATE    : 21/Jun/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V2.0  JNI 21/Jun/2000 - Re-developed from "mylib.lib" OPTO module.         */ 
/*                                                                            */ 
/* V2.1  JNI 18/Jan/2002 - Someone actually wanted to use it so had to get it */ 
/*                         working (only 2 years after initial coding).       */ 
/*                                                                            */ 
/* V2.2  JNI 28/Feb/2002 - Fixed Save / Load functions for Antonia's TMS coil.*/ 
/*                                                                            */ 
/* V2.3  JNI 11/Aug/2006 - Renamed from RIGID to OPTORIGID.                   */ 
/*                                                                            */ 
/* V2.4  JNI 08/Apr/2010 - Cleaning up after the Rigid Body mystery.          */ 
/*                                                                            */ 
/******************************************************************************/

#define MODULE_NAME     "OPTORIGID"
#define MODULE_TEXT     "Rigid Body API"
#define MODULE_DATE     "08/Apr/2010"
#define MODULE_VERSION  "2.4"
#define MODULE_LEVEL    4

/******************************************************************************/

#include <motor.h>                               // Includes everything we need.

/******************************************************************************/

BOOL    OPTORIGID_API_started=FALSE;

/******************************************************************************/

PRINTF  OPTORIGID_PRN_messgf=NULL;                   // General messages printf function.
PRINTF  OPTORIGID_PRN_errorf=NULL;                   // Error messages printf function.
PRINTF  OPTORIGID_PRN_debugf=NULL;                   // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int     OPTORIGID_messgf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(OPTORIGID_PRN_messgf,buff));
}

/******************************************************************************/

int     OPTORIGID_errorf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(OPTORIGID_PRN_errorf,buff));
}

/******************************************************************************/

int     OPTORIGID_debugf( const char *mask, ... )
{
va_list args;
static  char    buff[PRNTBUFF];

    va_start(args,mask);
    vsprintf(buff,mask,args);
    va_end(args);

    return(STR_printf(OPTORIGID_PRN_debugf,buff));
}

/******************************************************************************/

BOOL OPTORIGID_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf )
{
BOOL ok=FALSE;
int m,n;

    OPTORIGID_PRN_messgf = messgf;         // General API message print function.
    OPTORIGID_PRN_errorf = errorf;         // API error message print function.
    OPTORIGID_PRN_debugf = debugf;         // Debug information print function.

//  Make sure API is not already running...
    if( OPTORIGID_API_started )
    {
        return(TRUE);
    }

    ok = TRUE;

    if( ok )
    {
        ATEXIT_API(OPTORIGID_API_stop);         // Install stop function.
        OPTORIGID_API_started = TRUE;           // Set started flag.

        MODULE_start(OPTORIGID_PRN_messgf);     // Register module.
    }
    else
    {
        OPTORIGID_errorf("OPTORIGID_API_start(...) Failed.\n");
    }

    return(ok);
}

/******************************************************************************/

void    OPTORIGID_API_stop( void )
{
//  Make sure API is running in the first place...
    if( !OPTORIGID_API_started )
    {
         return;
    }

//  Register module stop, etc...
    MODULE_stop();
    OPTORIGID_API_started = FALSE;
}

/******************************************************************************/

BOOL    OPTORIGID_API_check( void )
{
BOOL    ok=TRUE;

    if( !OPTORIGID_API_started )           // API not started...
    {                                  // Start module automatically...
        ok = OPTORIGID_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
        OPTORIGID_debugf("OPTORIGID_API_check() Start %s.\n",ok ? "OK" : "Failed");
    }

    return(ok);
}

/******************************************************************************/

void    OPTORIGID_Init( OPTORIGID &rigid )
{
int     i;

    memset(rigid.name,0x00,STRLEN);

    rigid.m1 = OPTO_MARKER_NULL;
    rigid.m2 = OPTO_MARKER_NULL;
    rigid.mc = 0;

    matrix_empty(rigid.rbmx);
    matrix_empty(rigid.mdmx);
}

/******************************************************************************/

BOOL    OPTORIGID_Make( OPTORIGID &rigid, int m1, int m2, char *name )
{
int     mc;

//  Make sure API is running...
    if( !OPTORIGID_API_check() )
    {
        return(FALSE);
    }

//  Marker count...
    mc = (m2-m1)+1;

//  Check marker count...
    if( mc > OPTORIGID_MARKERS )
    {
        OPTORIGID_errorf("OPTORIGID_Make() %d markers are too many (max %d).\n",mc,OPTORIGID_MARKERS);
        return(FALSE);
    }

//  Optional name for rigid body...
    if( STR_null(name) )
    {
        OPTORIGID_errorf("OPTORIGID_Make() Must name rigid body.\n");
        return(FALSE);
    }

//  Clear rigid body variable and set values...
    OPTORIGID_Init(rigid);

    rigid.m1 = m1;
    rigid.m2 = m2;
    rigid.mc = mc;

    strncpy(rigid.name,name,STRLEN);

//  Allocate the Rigid body R/T MatriX...
    OPTORIGID_debugf("OPTORIGID_Make(%s) Allocating RBMX.\n",rigid.name);
    rigid.rbmx.dim(SPMX_PTMX_ROW,rigid.mc);

//  Allocate the Marker Distance MatriX...
    OPTORIGID_debugf("OPTORIGID_Make(%s) Allocating MDMX.\n",rigid.name);
    rigid.mdmx.dim(rigid.mc,rigid.mc);

    OPTORIGID_debugf("OPTORIGID_Make(%s) Done.\n",rigid.name);

    return(TRUE);
}

/******************************************************************************/

void    OPTORIGID_Cnfg( OPTORIGID &rigid )
{
    CONFIG_reset();

    CONFIG_set("name",rigid.name);
    CONFIG_set("m1",rigid.m1);
    CONFIG_set("m2",rigid.m2);
    CONFIG_set("markers",rigid.mc);
}

/******************************************************************************/

#define OPTORIGID_FILE_CFG  0
#define OPTORIGID_FILE_RBX  1
#define OPTORIGID_FILE_MDX  2

char   *OPTORIGID_FileExtn[] = { "CFG","RBX","MDX" };

/******************************************************************************/

BOOL    OPTORIGID_FileLoad( OPTORIGID &rigid, char *name )
{
BOOL    ok=FALSE;
STRING  file;
int     step;

    // Make sure API is running...
    if( !OPTORIGID_API_check() )
    {
        return(FALSE);
    }

    // Clear it before we start...
    OPTORIGID_Init(rigid);

    // Perform each step to load rigid body...
    for( ok=TRUE,step=OPTORIGID_FILE_CFG; ((step <= OPTORIGID_FILE_MDX) && ok); step++ )
    {
        strncpy(file,STR_stringf("%s.%s",name,OPTORIGID_FileExtn[step]),STRLEN);

        switch( step )
        {
            case OPTORIGID_FILE_CFG :
               OPTORIGID_Cnfg(rigid);
               ok = CONFIG_read(file);
               break;

            case OPTORIGID_FILE_RBX :
               ok = matrix_read(file,rigid.rbmx);
               break;

            case OPTORIGID_FILE_MDX :
               ok = matrix_read(file,rigid.mdmx);
               break;
        }

        STR_printf(ok,OPTORIGID_debugf,OPTORIGID_errorf,"OPTORIGID_FileLoad(%s) %s %s.\n",rigid.name,file,STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTORIGID_FileSave( OPTORIGID &rigid, char *name )
{
BOOL    ok=FALSE;
STRING  file;
int     step;

    // Make sure API is running...
    if( !OPTORIGID_API_check() )
    {
        return(FALSE);
    }

    // Perform each step to save rigid body...
    for( ok=TRUE,step=OPTORIGID_FILE_CFG; ((step <= OPTORIGID_FILE_MDX) && ok); step++ )
    {
        strncpy(file,STR_stringf("%s.%s",name,OPTORIGID_FileExtn[step]),STRLEN);

        switch( step )
        {
            case OPTORIGID_FILE_CFG :
               OPTORIGID_Cnfg(rigid);
               ok = CONFIG_write(file);
               break;

            case OPTORIGID_FILE_RBX :
               ok = matrix_write(file,rigid.rbmx);
               break;

            case OPTORIGID_FILE_MDX :
               ok = matrix_write(file,rigid.mdmx);
               break;
        }

        STR_printf(ok,OPTORIGID_debugf,OPTORIGID_errorf,"OPTORIGID_FileSave(%s) %s %s.\n",rigid.name,file,STR_OkFailed(ok));
    }

    return(ok);
}

/******************************************************************************/

BOOL    OPTORIGID_FileSave( OPTORIGID &rigid )
{
BOOL    ok;

    ok = OPTORIGID_FileSave(rigid,rigid.name);

    return(ok);
};

/******************************************************************************/

BOOL    OPTORIGID_GetBody( OPTORIGID &rigid, int m1, int m2, char *name )
{
BOOL    ok=FALSE;

    if( OPTORIGID_Make(rigid,m1,m2,name) )
    {
        ok = OPTORIGID_GetBody(rigid);
    }

    return(ok);
}

/******************************************************************************/

BOOL    _OPTORIGID_GetBody( OPTORIGID &rigid )
{
BOOL    ok=TRUE;
int     m,n,i,k,f,fmax=2000;
UINT    frame,last=0;
double  d;
matrix  posn;
matrix  seen;
matrix  done;
matrix  dcount;
matrix  dindex;
matrix  dmtx;

    if( !OPTORIGID_API_check() )                     // Make sure the API is running.
    {
        return(FALSE);
    }

    OPTORIGID_debugf("OPTORIGID_GetBody(...) Start.\n");

//  Dimension matrices to the required sizes...
    posn.dim(SPMX_PTMX_ROW,rigid.mc);
    seen.dim(OPTO_SEEN_ROW,rigid.mc);
  
    OPTORIGID_debugf("OPTORIGID_GetBody(...) Matrices dimensioned, entering loop.\n");

    dindex.dim(rigid.mc,rigid.mc);
    
    for( k=0,m=1; (m <= rigid.mc); m++ )
    {
        for( n=m; (n <= rigid.mc); n++ )
        {
            k = k + 1;
            dindex(m,n) = (double)k;
        }
    }

    printf("%d markers gives %d pairs.\n",rigid.mc,k);
    disp(dindex);

    dcount.dim(1,k);
    done.dim(1,k);
    dmtx.dim(fmax,k);
    f = 0;

    do
    {
        if( KB_ESC(KB_NOWAIT) )
        {
            ok = FALSE;
            continue;
        }

//      Get position of rigid body's markers...
        frame = OPTO_UsrPosn(posn,seen,rigid.m1,rigid.m2);
        if( frame == last )
        {
            continue;
        }

        //printf("frame=%d\n",frame);
        last = frame;

        //disp(seen);
        if( sum(seen) < 2 )
        {
            continue;
        }

        f = f + 1;
        if( TIMER_EveryHz(8.0) )
        {
            printf("f=%d\n",f);
        }

        /*if( f > dmtx.rows() )
        {
            //OPTORIGID_errorf("OPTORIGID_GetBody(...) Too many frames of distance data.\n");
            printf("OPTORIGID_GetBody(...) Too many frames of distance data.\n");
            ok = FALSE;
            continue;
        }*/

        for( m=1; (m <= rigid.mc); m++ )         // Loop for each marker.
        {
            for( n=m; (n <= rigid.mc); n++ )             // Loop for each pair.
            {
                i = dindex.I(m,n);

                if( seen.B(1,m) && seen.B(1,n) )
                {
                    d = norm(SPMX_ptmx2pomx(posn[m])-SPMX_ptmx2pomx(posn[n]));
                    dcount(1,i) = dcount(1,i) + 1;
                    dmtx(f,i) = d;
                }
                else
                {
                    dmtx(f,i) = -1.0;
                }
            }
        }

        for( i=1; (i <= k); i++ )
        {
            done(1,i) = (double)(dcount(1,i) >= 100);
        }

        if( TIMER_EveryHz(8.0) )
        {
            disp(dcount);
        }
    }
    while( ok && (f < fmax) ); 
    //while( ok && (sum(done) != k) ); 

    if( !ok )                                         // Something wrong...
    {
        OPTORIGID_errorf("OPTORIGID_GetBody(%s) Failed.\n",rigid.name);
        return(ok);
    }

    ok = matrix_write(STR_stringf("%s.DAT",rigid.name),dmtx,f);

    if( !ok )                                         // Something wrong...
    {
        OPTORIGID_errorf("OPTORIGID_GetBody(%s) Cannot wrist distance matrix.\n",rigid.name);
        return(ok);
    }

    return(ok);
}

/******************************************************************************/

int OPTSIM_Frame=0;
int OPTSIM_Frames=0;
int OPTSIM_Markers=0;
matrix OPTSIM_Data;

BOOL OPTSIM_Load( char *file )
{
BOOL ok;

    OPTSIM_Frame = 0;
    OPTSIM_Frames = 0;

    ok = matrix_read(file,OPTSIM_Data);
    if( ok )
    {
        OPTSIM_Frames = OPTSIM_Data.rows();
        OPTSIM_Markers = OPTSIM_Data.cols() / 4;
 
        printf("OPTSIM_Load(%s) frames=%d markers=%d\n",file,OPTSIM_Frames,OPTSIM_Markers);
    }

    return(ok);

}

UINT OPTSIM_GetPosn( matrix &P, matrix &S )
{
int frame=0;
int f,k,m,i;

    P.dim(4,OPTSIM_Markers);
    S.dim(1,OPTSIM_Markers);
    
    if( OPTSIM_Frames > 0 )
    {
        if( (OPTSIM_Frame < OPTSIM_Frames) && TIMER_EveryHz(100.0) )
        {
            f = ++OPTSIM_Frame;

            for( k=0,m=1; (m <= OPTSIM_Markers); m++ )
            {
                for( i=1; (i <= 3); i++ )
                {
                    k = k + 1;
                    P(i,m) = OPTSIM_Data(f,k);
                }

                P(4,m) = 1.0;

                k = k + 1;
                S(1,m) = OPTSIM_Data(f,k);
            }
        }

        frame = OPTSIM_Frame;
    }

    return(frame);
}

/******************************************************************************/

BOOL    OPTORIGID_GetBody( OPTORIGID &rigid )
{
BOOL    ok=TRUE;
int     vc,m,n,i;
UINT    frame,last=0;
matrix  posn;           // POSN: Current position of rigid body's markers. 
matrix  seen;           // SEEN: The markers we can see in the rigid body.
matrix  done;           // DONE: Markers we've done in the rigid body's coordinate frame.
matrix  body;           // POSN: Position in the new frame.
matrix  csbd;           // CSBD: The markers we C)an currently S)ee and that have B)een D)one.
matrix  rtmx;           // Rotation/Translation matrix for rigid body.
matrix  err;            // Error in R/T matrix.
matrix  psd;            // Position SD.
matrix  rerr;           // Error in RTMX matrix for each marker during definition.
double  e;

    if( !OPTORIGID_API_check() )                     // Make sure the API is running.
    {
        return(FALSE);
    }

    OPTORIGID_debugf("OPTORIGID_GetBody(...) Start.\n");

//  Dimension matrices to the required sizes...
    posn.dim(SPMX_PTMX_ROW,rigid.mc);
    seen.dim(OPTO_SEEN_ROW,rigid.mc);
    done.dim(OPTO_SEEN_ROW,rigid.mc);
    body.dim(SPMX_PTMX_ROW,rigid.mc);
    psd.dim(3,rigid.mc);
    rerr.dim(1,rigid.mc);

    OPTORIGID_debugf("OPTORIGID_GetBody(...) Matrices dimensioned, entering loop.\n");

    do
    {
        if( KB_ESC(KB_NOWAIT) )
        {
            ok = FALSE;
            continue;
        }

//      Get position of rigid body's markers...
        frame = OPTO_UsrPosn(posn,seen,rigid.m1,rigid.m2);
        //frame = OPTSIM_GetPosn(posn,seen);
        if( frame == last )
        {
            continue;
        }

        last = frame;

        if( TIMER_EveryHz(8.0) )
        {
            printf("OptoTrakFrame=%d\n",frame);
            disp(seen);                                  
            disp(done);                                  
        }

//      Must see the minimum number of markers before proceeding...
        if( (vc=sum(seen)) < 3 )    
        {               
            continue;
        }

        OPTORIGID_debugf("OPTORIGID_GetBody(%s) %d/%d markers visible.\n",rigid.name,rigid.mc,vc);

//      Flag markers that we Can See and have Been Done...
        csbd = seen && done;

//      This is the first time we've seen them. They're our first-seen markers...
        if( sum(done) == 0 )          
        {
            for( m=1; (m <= rigid.mc); m++)
            {           // Save the position of our first-seen markers...
                if( (BOOL)seen(1,m) )
                {
                    colassign(body,m,posn[m]);   // Save position columns.
                    done(1,m) = (double)TRUE;    // Yes! We've got this one.
                    rerr(1,m) = 0.0;
                    printf("Done initial marker %d (P=%.1lf,%.1lf,%.1lf).\n",m,posn(1,m),posn(2,m),posn(3,m));

                    OPTORIGID_debugf("OPTORIGID_GetBody(%s) Marker %d done (first-seen).\n",rigid.name,m);
                }
            } 
        }
        else            // Not the first time we've seen them...
        if( sum(csbd) >= 3 )        // Can see and have been done...
        {               // Must see the minimum of those we can see and have done...
//          Find R/T matrix from current positions to first-seen positions.
            e = 0.0;
            if( (ok=SPMX_rtmx(rtmx,posn(csbd),body(csbd))) )
            {
                err = (rtmx * posn(csbd)) - body(csbd);     // Calculate the error. Should be zero if "SPMX_rtmx(...)"is working.
                e = sqrt(sum(err^err)) / sum(csbd);
                ok = (e < 0.4); // Check error tolerance.
            }

            if( !ok )
            {
                OPTORIGID_errorf("OPTORIGID_GetBody(%s) Cannot find RTMX (error=%.1lf cm).\n",rigid.name,e);
                continue;
            }
                  
            for( m=1; (m <= rigid.mc); m++ )
            {
                if( (BOOL)seen(1,m) && !(BOOL)done(1,m) )
                {       // A marker we haven't had before! R/T it based on our first-seen positions.
                    colassign(body,m,rtmx*posn[m]);   // Save (first-seen relative) position columns.
                    done(1,m) = (float)TRUE;          // Yes! We're done with this marker.
                    rerr(1,m) = e;
                    printf("Done additional marker %d (P=%.1lf,%.1lf,%.1lf; B=%.1lf,%.1lf,%.1lf; rerror=%.3lf).\n",m,posn(1,m),posn(2,m),posn(3,m),body(1,m),body(2,m),body(3,m),e);
                    disp(rtmx);

                    OPTORIGID_debugf("OPTORIGID_GetBody(%s) Marker %d done (relative).\n",rigid.name,m);
                }
            }
        }
    }
    while( ok && (sum(done) < rigid.mc) );       // Loop until we've done all markers (or we have an error).

    if( !ok )                                         // Something wrong...
    {
        OPTORIGID_errorf("OPTORIGID_GetBody(%s) Failed.\n",rigid.name);
        return(ok);
    }

    disp(rerr);

//  Distances from each marker to every other marker...
    OPTORIGID_debugf("OPTORIGID_GetBody(%s) Calculating distance matrix.\n",rigid.name);
  
    for( m=1; (m <= rigid.mc); m++ )         // Loop for each marker.
    {
        for( n=1; (n < m); n++ )             // Loop for each pair.
        {
            rigid.mdmx(m,n) = rigid.mdmx(n,m) = norm(body[m]-body[n]);
        }
    }
  
 // Now we find the center of all markers and make coodrinates relative to it...

    OPTORIGID_debugf("OPTORIGID_GetBody(%s) Calculating body-centred co-ordinates matrix.\n",rigid.name);

    body = body - rowmean(body) * ones(1,body.cols());                      // Removes the row mean (so relative to centre).
    rowassign(body,4,ones(1,cols(body)));             // Puts the ones back in the 4th row.

    rigid.rbmx = body;

    OPTORIGID_debugf("OPTORIGID_GetBody(%s) Done.\n",rigid.name);

    return(ok);
}

/******************************************************************************/

BOOL OPTORIGID_RTMX( OPTORIGID &rigid, matrix &rtmx )
{
static matrix posn;
static matrix seen;
UINT frame;
BOOL ok;

    // Make sure the API is running.
    if( !OPTORIGID_API_check() )
    {
        return(FALSE);
    }

    // Dimension matrices to the required sizes.
    posn.dim(SPMX_PTMX_ROW,rigid.mc);
    seen.dim(OPTO_SEEN_ROW,rigid.mc);

    // Get current position of markers (in user-defined frame) on the rigid body.
    frame = OPTO_UsrPosn(posn,seen,rigid.m1,rigid.m2);

    // Find R/T matrix to get from current position to zero position.
    ok = OPTORIGID_RTMX(rigid,rtmx,posn,seen);

    return(ok);
}

/******************************************************************************/

BOOL OPTORIGID_RTMX( OPTORIGID &rigid, matrix &rtmx, matrix &posn, matrix &seen )
{
matrix err;
int m,n,i;
double derror,rerror,k;

    // Make sure the API is running...
    if( !OPTORIGID_API_check() )
    {
        return(FALSE);
    }

    // Check if minimum number of markers are visible...  
    if( sum(seen) < 3 )
    {
        // OPTORIGID_debugf("OPTORIGID_RTMX(%s) Markers not visible.\n",rigid.name);
        return(FALSE);
    }

    // Calculate error in paired marker distances.
    derror = 0.0;
    k = 0.0;

    for( m=1; (m <= rigid.mc); m++ )        // Loop for each marker.
    {
        for( n=m+1; (n < rigid.mc); n++ )   // Loop for each pair.
        {
            if( seen.B(1,m) && seen.B(1,n) )
            {
                k = k + 1.0;
                derror += fabs(norm(rigid.rbmx[m]-rigid.rbmx[n]) - norm(posn[m]-posn[n]));
            }
        }
    }

    derror /= k;

    // Find R/T matrix to get from current position to original position...
    if( !SPMX_rtmx(rtmx,rigid.rbmx(seen),posn(seen)) )
    {
        OPTORIGID_errorf("OPTORIGID_RTMX(%s) Cannot find RTMX.\n",rigid.name);
        return(FALSE);
    }

    // Calculate error in RTMX.
    err = (rtmx * rigid.rbmx(seen)) - posn(seen);
    rerror = sqrt(sum(err^err)) / sum(seen);

    if( TIMER_EveryHz(8.0) )
    {
        //printf("OPTORIGID_RTMX(%s) derror=%.3lf(cm) rerror=%.3lf(cm)\n",rigid.name,derror,rerror);
    }

    return(TRUE);
}

/******************************************************************************/

BOOL OPTORIGID_RTMX( OPTORIGID &rigid, matrix &rtmx, matrix posn[], BOOL seen[] )
{
matrix P;
matrix S;
BOOL ok;
int m;

    // Make sure the API is running...
    if( !OPTORIGID_API_check() )
    {
        return(FALSE);
    }

    // Dimension matrices to the required sizes.
    P.dim(SPMX_PTMX_ROW,rigid.mc);
    S.dim(OPTO_SEEN_ROW,rigid.mc);

    for( m=1; (m <= rigid.mc); m++ )
    {
        P(1,m) = posn[m+rigid.m1-2](1,1);
        P(2,m) = posn[m+rigid.m1-2](2,1);
        P(3,m) = posn[m+rigid.m1-2](3,1);

        S(1,m) = (double)seen[m+rigid.m1-2];
    }

    // Find R/T matrix to get from current position to zero position.
    ok = OPTORIGID_RTMX(rigid,rtmx,P,S);

    return(ok);
}

/******************************************************************************/

