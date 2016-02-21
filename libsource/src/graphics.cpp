/******************************************************************************/
/*                                                                            */
/* MODULE  : Graphics.cpp                                                     */
/*                                                                            */
/* PURPOSE : OpenGL / GLUT Graphics module.                                   */
/*                                                                            */
/* DATE    : 15/Feb/2001                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V2.0  JNI 16/Jul/2003 - Re-developed from old MyLib module.                */
/*                                                                            */
/* V2.1  JNI 10/Sep/2008 - Added display rotation to GRAPHICS.CFG file.       */
/*                                                                            */
/* V2.2  JNI 27/Apr/2010 - Added lighting parameters to GRAPHICS.CFG file.    */
/*                                                                            */
/* V2.3  JNI 09/Jan/2015 - Support for Oculus Rift HMD.                       */
/*                                                                            */
/*       JNI 28/Jan/2015 - Graphics monitor window for desktop display.       */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "GRAPHICS"
#define MODULE_TEXT     "Graphics (OpenGL/GLUT) Module"
#define MODULE_DATE     "28/01/2015"
#define MODULE_VERSION  "2.3"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

struct  STR_TextItem  GRAPHICS_DisplayText[] =
{
    { GRAPHICS_DISPLAY_MONO   ,"3D-MONO"   },
    { GRAPHICS_DISPLAY_STEREO ,"3D-STEREO" },
    { GRAPHICS_DISPLAY_2D     ,"2D"        },
    { GRAPHICS_DISPLAY_DEFAULT,"DEFAULT"   },
    { GRAPHICS_DISPLAY_MONO   ,"MONO"      },
    { GRAPHICS_DISPLAY_STEREO ,"STEREO"    },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem  GRAPHICS_StereoText[] =
{
    { GRAPHICS_STEREO_NONE          ,""                 },
    { GRAPHICS_STEREO_NONE          ,"None"             },
    { GRAPHICS_STEREO_FRAMEALTERNATE,"FrameAlternation" },
    { GRAPHICS_STEREO_DUALSCREEN    ,"DualScreen"       },
    { GRAPHICS_STEREO_OCULUS        ,"OculusHMD"        },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

BOOL GRAPHICS_DisplayModeParse( int &code, char *text )
{
BOOL ok=FALSE;

    ok = STR_TextCode(GRAPHICS_DisplayText,code,text);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_StereoModeParse( int &code, char *text )
{
BOOL ok=FALSE;

    ok = STR_TextCode(GRAPHICS_StereoText,code,text);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_StereoModeOculus( void )
{
BOOL flag;

    flag = (GRAPHICS_StereoMode == GRAPHICS_STEREO_OCULUS);

    return(flag);
}

/******************************************************************************/

STRING   GRAPHICS_Config=GRAPHICS_CONFIG;
int      GRAPHICS_WindowID=GRAPHICS_WINDOW_INVALID;

STRING   GRAPHICS_Description="Graphics Window";
int      GRAPHICS_DisplayPixels[GRAPHICS_2D] = { 0,0 };
int      GRAPHICS_DisplayFrequency = 0.0;
float    GRAPHICS_DisplayRotation[GRAPHICS_3D] = { 0.0,0.0,0.0 };
STRING   GRAPHICS_StereoModeString="NONE";
int      GRAPHICS_StereoMode=GRAPHICS_STEREO_NONE;
STRING   GRAPHICS_DefaultModeString="2D";
int      GRAPHICS_DefaultMode=GRAPHICS_DISPLAY_2D;
float    GRAPHICS_DisplaySize[GRAPHICS_3D][GRAPHICS_RANGE];
float    GRAPHICS_FocalPlane=0;
float    GRAPHICS_PupilToCentre=3.0;
float    GRAPHICS_EyeCentre[GRAPHICS_3D]= { 0.0,0.0,50.0 };
double   GRAPHICS_LightPosition[GRAPHICS_3D] = { 0.0,0.0,0.0 };
double   GRAPHICS_LightColor[GRAPHICS_RGB] = { 1.0,1.0,1.0 };
STRING   GRAPHICS_CalibrationFile=GRAPHICS_CALIBRATION;
BOOL     GRAPHICS_SpaceBall=FALSE;
BOOL     GRAPHICS_StereoDepth=TRUE;
int      GRAPHICS_ClearColorDefault=BLACK;
STRING   GRAPHICS_ClearColorString="BLACK";
double   GRAPHICS_DisplayDelayMinimum=0.0;
int      GRAPHICS_SphereSegments=100;
int      GRAPHICS_WireSphereSegments=20;
matrix   GRAPHICS_EyeCentrePOMX;
BOOL     GRAPHICS_DoubleBufferedFlag=TRUE;
BYTE     GRAPHICS_FlagOpenGL=0x00;

int      GRAPHICS_DefaultPixels[GRAPHICS_2D];
int      GRAPHICS_DefaultFrequency;

double   GRAPHICS_VerticalRetraceFrequency;
double   GRAPHICS_VerticalRetracePeriod;
TIMER    GRAPHICS_VerticalRetraceOnsetTimer("GRAPHICS_VerticalRetraceOnsetTimer");
TIMER    GRAPHICS_VerticalRetraceOffsetTimer("GRAPHICS_VerticalRetraceOffsetTimer");
TIMER    GRAPHICS_VerticalRetraceCatchTimer("GRAPHICS_VerticalRetraceCatchTimer");
double   GRAPHICS_VerticalRetraceCatchTime = 0.050; // msec
TIMER    GRAPHICS_VerticalRetraceTimer("GRAPHICS_VerticalRetraceTimer");
BOOL     GRAPHICS_VerticalRetraceOnsetSyncFlag = FALSE;
BOOL     GRAPHICS_VerticalRetraceOffsetSyncFlag = FALSE;

DATAPROC GRAPHICS_VerticalRetraceOnsetTimeData("GRAPHICS_VerticalRetraceOnsetTime");
DATAPROC GRAPHICS_VerticalRetraceOnsetErrorData("GRAPHICS_VerticalRetraceOnsetError");
DATAPROC GRAPHICS_VerticalRetraceOffsetTimeData("GRAPHICS_VerticalRetraceOffsetTime");
DATAPROC GRAPHICS_VerticalRetraceOffsetErrorData("GRAPHICS_VerticalRetraceOffsetError");
DATAPROC GRAPHICS_SwapBuffersToVerticalRetraceTimeData("GRAPHICS_SwapBuffersToVerticalRetraceTime",200000);

long     GRAPHICS_SwapBuffersCount=0;
double   GRAPHICS_SwapBuffersToVerticalRetraceTime;
double   GRAPHICS_SwapBuffersLastOnsetTime;
double   GRAPHICS_SwapBuffersLastOffsetTime;
double   GRAPHICS_VerticalRetraceNextOnsetTime;
double   GRAPHICS_VerticalRetraceNextOffsetTime;

/******************************************************************************/

TIMER_Interval   GRAPHICS_ClearStereoLatencyTimer("GRAPHICS_ClearStereoLatency");
TIMER_Interval   GRAPHICS_ClearMonoLatencyTimer("GRAPHICS_ClearMonoLatency");
TIMER_Frequency  GRAPHICS_DisplayFrequencyTimer("GRAPHICS_DisplayFrequency");
TIMER_Interval   GRAPHICS_DisplayLatencyTimer("GRAPHICS_DisplayLatency");
TIMER_Interval   GRAPHICS_TextLatencyTimer("GRAPHICS_TextLatency");
TIMER_Interval   GRAPHICS_DrawLatencyTimer("GRAPHICS_DrawLatency");
TIMER_Interval   GRAPHICS_SwapBufferLatencyTimer("GRAPHICS_SwapBufferLatency");
TIMER            GRAPHICS_SwapBufferAbsoluteTimer("GRAPHICS_SwapBufferAbsoluteTime");
TIMER_Frequency  GRAPHICS_GlutKeyboardFrequencyTimer("GRAPHICS_GlutKeyboardFrequency");
TIMER_Frequency  GRAPHICS_GlutDisplayFrequencyTimer("GRAPHICS_GlutDisplayFrequency");
TIMER_Frequency  GRAPHICS_GlutIdleFrequencyTimer("GRAPHICS_GlutIdleFrequency");

double GRAPHICS_ClearStereoLatency;
double GRAPHICS_ClearMonoLatency;
double GRAPHICS_DisplayLatency;
double GRAPHICS_DrawLatency;
double GRAPHICS_DisplayFrequencyPeriod;
double GRAPHICS_SwapBufferLatency;
double GRAPHICS_SwapBufferAbsoluteTime;

/******************************************************************************/

void (*GRAPHICS_KeyboardFunction)( BYTE key, int x, int y )=NULL;
void (*GRAPHICS_DrawFunction)( void )=NULL;
void (*GRAPHICS_IdleFunction)( void )=NULL;

/******************************************************************************/

void GRAPHICS_WindowDestroy( void )
{
    if( GRAPHICS_WindowID != GRAPHICS_WINDOW_INVALID )
    {
        glutDestroyWindow(GRAPHICS_WindowID);
        GRAPHICS_WindowID = GRAPHICS_WINDOW_INVALID;
    }

    GRAPHICS_MonitorWindowDestroy();
}

/******************************************************************************/

void GRAPHICS_ConfigVariables( void )
{
    // Hide the follow configuration variables...
    CONFIG_defaultflags(CONFIG_FLAG_HIDDEN);

    // GRAPHICS module configuration variables..,
    CONFIG_set("GRAPHICS",GRAPHICS_Config);

    // Resume normal configuration variables...
    CONFIG_defaultflags(CONFIG_FLAG_NONE);
}

/******************************************************************************/

BOOL GRAPHICS_Cnfg( char *file )
{
BOOL ok;

    CONFIG_reset();

    // Set up variable table for configuration...
    CONFIG_set("Description",GRAPHICS_Description);
    CONFIG_set("StereoMode",GRAPHICS_StereoModeString);
    CONFIG_set("DefaultMode",GRAPHICS_DefaultModeString);
    CONFIG_set("ClearColor",GRAPHICS_ClearColorString);
    CONFIG_set("Pixels",GRAPHICS_DisplayPixels,GRAPHICS_2D);
    CONFIG_set("Frequency",GRAPHICS_DisplayFrequency);
    CONFIG_set("Rotation",GRAPHICS_DisplayRotation,GRAPHICS_3D);
    CONFIG_set("Xmin,Xmax",GRAPHICS_DisplaySize[GRAPHICS_X],GRAPHICS_RANGE);
    CONFIG_set("Ymin,Ymax",GRAPHICS_DisplaySize[GRAPHICS_Y],GRAPHICS_RANGE);
    CONFIG_set("Znear,Zfar",GRAPHICS_DisplaySize[GRAPHICS_Z],GRAPHICS_RANGE);
    CONFIG_set("FocalPlane",GRAPHICS_FocalPlane);
    CONFIG_set("PupilToCentre",GRAPHICS_PupilToCentre);
    CONFIG_set("EyeCentre",GRAPHICS_EyeCentre,GRAPHICS_3D);
    CONFIG_set("LightPosition",GRAPHICS_LightPosition,GRAPHICS_3D);
    CONFIG_set("LightColor",GRAPHICS_LightColor,GRAPHICS_RGB);
    CONFIG_set("Calibration",GRAPHICS_CalibrationFile);
    CONFIG_setBOOL("SpaceBall",GRAPHICS_SpaceBall);
    CONFIG_setBOOL("StereoDepth",GRAPHICS_StereoDepth);
    CONFIG_set("DisplayDelay",GRAPHICS_DisplayDelayMinimum);

    // Load configuration file...
    if( !CONFIG_read(file) )
    {
        GRAPHICS_errorf("GRAPHICS_Cnfg(%s) Cannot read file.\n",file);
        return(FALSE);
    }

    if( GRAPHICS_DisplaySize[GRAPHICS_Z][GRAPHICS_MIN] == 0.0 )
    {
        GRAPHICS_DisplaySize[GRAPHICS_Z][GRAPHICS_MIN] = 1.0;
    }

    if( GRAPHICS_DisplaySize[GRAPHICS_Z][GRAPHICS_MAX] == 0.0 )
    {
        GRAPHICS_DisplaySize[GRAPHICS_Z][GRAPHICS_MAX] = 1000.0;
    }

    // Process display  mode...
    if( CONFIG_readflag("DefaultMode") )
    {
        if( !GRAPHICS_DisplayModeParse(GRAPHICS_DefaultMode,GRAPHICS_DefaultModeString) )
        {
            GRAPHICS_errorf("GRAHPICS_Cnfg(%s) Invalid DefaultMode: %s.\n",file,GRAPHICS_DefaultModeString);
            return(FALSE);
        }
    }

    // Process stereo mode...
    if( CONFIG_readflag("StereoMode") )
    {
        if( !GRAPHICS_StereoModeParse(GRAPHICS_StereoMode,GRAPHICS_StereoModeString) )
        {
            GRAPHICS_errorf("GRAHPICS_Cnfg(%s) Invalid StereoMode: %s.\n",file,GRAPHICS_StereoModeString);
            return(FALSE);
        }
    }

    if( GRAPHICS_StereoMode == GRAPHICS_STEREO_OCULUS )
    {
        GRAPHICS_DefaultMode = GRAPHICS_DISPLAY_STEREO;
    }

    // Process clear color...
    if( CONFIG_readflag("ClearColor") )
    {
        if( !GRAPHICS_ColorCode(GRAPHICS_ClearColorDefault,GRAPHICS_ClearColorString) )
        {
            GRAPHICS_errorf("GRAHPICS_Cnfg(%s) Invalid ClearColor: %s.\n",file,GRAPHICS_ClearColorString);
            return(FALSE);
        }
    }

    // Default light position if not specified...
    if( !CONFIG_readflag("LightPosition") )
    {
        GRAPHICS_LightPosition[0] = GRAPHICS_CalibCentre[0];
        GRAPHICS_LightPosition[1] = GRAPHICS_CalibCentre[1];
        GRAPHICS_LightPosition[2] = GRAPHICS_EyeCentre[2];
    }

    GRAPHICS_debugf("GRAPHICS_Cnfg(%s) Loaded.\n",file);
    CONFIG_list(GRAPHICS_debugf);

    // Convert eye-centre to a matrix (more useful)...
    SPMX_xyz2pomx(GRAPHICS_EyeCentre,GRAPHICS_EyeCentrePOMX);

    return(TRUE);
}

/******************************************************************************/

BOOL    GRAPHICS_InitializedFlag=FALSE;
BOOL    GRAPHICS_StartedFlag=FALSE;

HWND    GRAPHICS_WindowParent=NULL;
HWND    GRAPHICS_WindowGLUT=NULL;

/******************************************************************************/

int     GRAPHICS_CapabilityLevel[GRAPHICS_CAPABILITY_INDEX];
BOOL    GRAPHICS_CapabilityStack[GRAPHICS_CAPABILITY_INDEX][GRAPHICS_CAPABILITY_STACK];

int     GRAPHICS_CapabilityTable[GRAPHICS_CAPABILITY_INDEX] =
{
    GL_ALPHA_TEST,
    GL_AUTO_NORMAL,
    GL_MAP2_VERTEX_4,
    GL_BLEND,
    GL_CLIP_PLANE0,
    GL_COLOR_MATERIAL,
    GL_CULL_FACE,
    GL_DEPTH_TEST, 
    GL_DITHER,
    GL_FOG,
    GL_LIGHT0,
    GL_LIGHTING,
    GL_LINE_SMOOTH,
    GL_LINE_STIPPLE,
    GL_LOGIC_OP, 
    GL_MAP1_COLOR_4,
    GL_MAP1_INDEX,
    GL_MAP1_NORMAL,
    GL_MAP1_TEXTURE_COORD_1,
    GL_MAP1_TEXTURE_COORD_2,
    GL_MAP1_TEXTURE_COORD_3,
    GL_MAP1_TEXTURE_COORD_4,
    GL_MAP1_VERTEX_3,
    GL_MAP1_VERTEX_4,
    GL_MAP2_COLOR_4,
    GL_MAP2_INDEX,
    GL_MAP2_NORMAL, 
    GL_MAP2_TEXTURE_COORD_1,
    GL_MAP2_TEXTURE_COORD_2,
    GL_MAP2_TEXTURE_COORD_3,
    GL_MAP2_TEXTURE_COORD_4,
    GL_MAP2_VERTEX_3,
    GL_MAP2_VERTEX_4,
    GL_NORMALIZE,
    GL_POINT_SMOOTH,
    GL_POLYGON_SMOOTH,
    GL_POLYGON_STIPPLE,
    GL_SCISSOR_TEST,
    GL_STENCIL_TEST,
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_GEN_Q,
    GL_TEXTURE_GEN_R,
    GL_TEXTURE_GEN_S,
    GL_TEXTURE_GEN_T,    
};

/******************************************************************************/

BOOL GRAPHICS_Init( char *cnfg )
{
int i,j;
char *file;

    // Check if graphics system already initialized.
    if( GRAPHICS_Initialized() )
    {
        return(TRUE);
    }

    ATEXIT_API(GRAPHICS_Stop);

    // Initialize OpenGL capability stack...
    for( i=0; (i < GRAPHICS_CAPABILITY_INDEX); i++ )
    {
        GRAPHICS_CapabilityLevel[i] = 0;
    }

    // Get handle for parent window...
    if( GRAPHICS_WindowParent == NULL )
    {
        if( (GRAPHICS_WindowParent=GetForegroundWindow()) == NULL )
        {
            GRAPHICS_errorf("GRAPHICS_Init() Cannot get foreground window.\n");
        }
    }

    for( i=0; (i < GRAPHICS_3D); i++ )
    {
        for( j=0; (j < GRAPHICS_RANGE); j++ )
        {
            GRAPHICS_DisplaySize[i][j] = 0.0;
        }
    }

    for( i=0; (i < GRAPHICS_2D); i++ )
    {
        GRAPHICS_DisplayPixels[i] = 0;
    }

    GRAPHICS_DisplayFrequency = 0;

    // Use default configuration file...
    if( STR_null(cnfg) )
    {
        cnfg = GRAPHICS_Config;
    }

    if( (file=FILE_Calibration(cnfg)) == NULL )
    {
        GRAPHICS_errorf("GRAPHICS_Init() %s file not found.\n",cnfg);
        return(FALSE);
    }

    // Load configuration file...
    if( !GRAPHICS_Cnfg(file) )
    {
        return(FALSE);
    }

    if( !DISPLAY_ModeGet(GRAPHICS_DefaultPixels[GRAPHICS_X],GRAPHICS_DefaultPixels[GRAPHICS_Y],GRAPHICS_DefaultFrequency) )
    {
        GRAPHICS_errorf("GRAPHICS_Init() Cannot get display mode.\n");
        return(FALSE);
    }

    if( (GRAPHICS_DisplayPixels[GRAPHICS_X] * GRAPHICS_DisplayPixels[GRAPHICS_Y]) == 0 ) 
    {
        GRAPHICS_DisplayPixels[GRAPHICS_X] = GRAPHICS_DefaultPixels[GRAPHICS_X];  
        GRAPHICS_DisplayPixels[GRAPHICS_Y] = GRAPHICS_DefaultPixels[GRAPHICS_Y];  
    }

    if( GRAPHICS_DisplayFrequency == 0 )
    {
        GRAPHICS_DisplayFrequency = GRAPHICS_DefaultFrequency;
    }

    if( !STR_null(GRAPHICS_Description,STRLEN) )
    {
        GRAPHICS_debugf("%s.\n",GRAPHICS_Description);
    }

    GRAPHICS_TextInit();

    GRAPHICS_InitializedFlag = TRUE;

    return(TRUE);
}

/******************************************************************************/

BOOL    GRAPHICS_Init( void )
{
BOOL    ok;

    // Default GRAPHICS initialization... 
    ok = GRAPHICS_Init(NULL);

    return(ok);
}

/******************************************************************************/

float GRAPHICS_ColorTable[GRAPHICS_COLOR_ROWS][GRAPHICS_COLOR_COLUMNS] = 
{
    { GRAPHICS_COLOR_RED        }, // 00
    { GRAPHICS_COLOR_YELLOW     }, // 01
    { GRAPHICS_COLOR_BLUE       }, // 02
    { GRAPHICS_COLOR_WHITE      }, // 03
    { GRAPHICS_COLOR_GREEN      }, // 04
    { GRAPHICS_COLOR_TURQUOISE  }, // 05
    { GRAPHICS_COLOR_PURPLE     }, // 06
    { GRAPHICS_COLOR_BLACK      }, // 07
    { GRAPHICS_COLOR_LIGHTBLUE  }, // 08
    { GRAPHICS_COLOR_DARKBLUE   }, // 09
    { GRAPHICS_COLOR_GREY       }, // 10
    { GRAPHICS_COLOR_DARKGREY   }, // 11
    { GRAPHICS_COLOR_LIGHTBLACK }, // 12
    { GRAPHICS_COLOR_LIGHTGREY  }, // 13
    { GRAPHICS_COLOR_UNDEFINED  }, // 14
    { GRAPHICS_COLOR_UNDEFINED  }, // 15
    { GRAPHICS_COLOR_UNDEFINED  }, // 16
    { GRAPHICS_COLOR_UNDEFINED  }, // 17
    { GRAPHICS_COLOR_UNDEFINED  }, // 18
    { GRAPHICS_COLOR_UNDEFINED  }, // 19
    { GRAPHICS_COLOR_UNDEFINED  }, // 20
    { GRAPHICS_COLOR_UNDEFINED  }, // 21
    { GRAPHICS_COLOR_UNDEFINED  }, // 22
    { GRAPHICS_COLOR_UNDEFINED  }, // 23
    { GRAPHICS_COLOR_UNDEFINED  }, // 24
    { GRAPHICS_COLOR_UNDEFINED  }, // 25
    { GRAPHICS_COLOR_UNDEFINED  }, // 26
    { GRAPHICS_COLOR_UNDEFINED  }, // 27
    { GRAPHICS_COLOR_UNDEFINED  }, // 28
    { GRAPHICS_COLOR_UNDEFINED  }, // 29
    { GRAPHICS_COLOR_UNDEFINED  }, // 30
    { GRAPHICS_COLOR_UNDEFINED  }, // 31
};

/******************************************************************************/

struct STR_TextItem GRAPHICS_ColorText[] = 
{
    { RED       ,"RED"        },
    { YELLOW    ,"YELLOW"     },
    { BLUE      ,"BLUE"       },
    { WHITE     ,"WHITE"      },
    { GREEN     ,"GREEN"      },
    { TURQUOISE ,"TURQUOISE"  },
    { PURPLE    ,"PURPLE"     },
    { BLACK     ,"BLACK"      },
    { LIGHTBLUE ,"LIGHTBLUE"  },
    { DARKBLUE  ,"DARKBLUE"   },
    { GREY      ,"GREY"       },
    { DARKGREY  ,"DARKGREY"   },
    { LIGHTBLACK,"LIGHTBLACK" },
    { LIGHTGREY ,"LIGHTGREY"  },
    { STR_TEXT_ENDOFTABLE     }
};

/******************************************************************************/

int GRAPHICS_ColorCurrent=NOCOLOR;

/******************************************************************************/

BOOL GRAPHICS_ColorCode( int &code, char *text )
{
BOOL ok=FALSE;

    ok = STR_TextCode(GRAPHICS_ColorText,code,text);

    return(ok);
}

/******************************************************************************/

int GRAPHICS_ColorCode( char *text )
{
BOOL ok;
int code=WHITE;

    ok = GRAPHICS_ColorCode(code,text);

    return(code);
}

/******************************************************************************/

BOOL GRAPHICS_ColorValid( float entry[] )
{
int i;
BOOL flag=FALSE;

    for( flag=TRUE,i=0; ((i < GRAPHICS_COLOR_COLUMNS) && flag); i++ )
    {
        flag = (entry[i] >= 0.0);
    }

    return(flag);
}

/******************************************************************************/

BOOL GRAPHICS_ColorValid( int code )
{
BOOL flag=FALSE;

    if( (code >= 0) && (code < GRAPHICS_COLOR_ROWS) )
    {
        flag = GRAPHICS_ColorValid(GRAPHICS_ColorTable[code]);
    }

    return(flag);
}

/******************************************************************************/

void GRAPHICS_ColorSet( int code, float alpha )
{
int pname;

    if( code == NOCOLOR )
    {
        return;
    }

    if( !GRAPHICS_ColorValid(code) )
    {
        GRAPHICS_errorf("GRAPHICS_ColorSet(code=%d,alpha=%.1f) Invalid color.\n",code,alpha);
        return;
    }

    GRAPHICS_ColorCurrent = code;

    GRAPHICS_ColorTable[code][GRAPHICS_COLOR_ALPHA] = alpha;
    pname = GL_AMBIENT_AND_DIFFUSE;//(int)GRAPHICS_ColorTable[code][GRAPHICS_COLOR_PNAME];

    glColor4fv(GRAPHICS_ColorTable[code]);
    glMaterialfv(GL_FRONT_AND_BACK,pname,GRAPHICS_ColorTable[code]);
}

/******************************************************************************/

void GRAPHICS_ColorSet( char *text, float alpha )
{
int code;

    if( GRAPHICS_ColorCode(code,text) )
    {
        GRAPHICS_ColorSet(code,alpha);
    }
}

/******************************************************************************/

void GRAPHICS_ColorSet( int code )
{
float alpha=1.0;

    GRAPHICS_ColorSet(code,alpha);
}

/******************************************************************************/

void GRAPHICS_ColorSet( char *text )
{
float alpha=1.0;

    GRAPHICS_ColorSet(text,alpha);
}

/******************************************************************************/

int GRAPHICS_ColorGet( void )
{
    return(GRAPHICS_ColorCurrent);
}

/******************************************************************************/

char *GRAPHICS_ColorName( int code )
{
char *name;

    name = STR_TextCode(GRAPHICS_ColorText,code);

    return(name);
}

/******************************************************************************/

int GRAPHICS_ColorDefine( float entry[] )
{
int color;
float r,b,g,alpha,pname;

    r = entry[GRAPHICS_COLOR_R];
    g = entry[GRAPHICS_COLOR_G];
    b = entry[GRAPHICS_COLOR_B];
    alpha = entry[GRAPHICS_COLOR_ALPHA];
    pname = (int)entry[GRAPHICS_COLOR_PNAME];

    color = GRAPHICS_ColorDefine(r,b,g,alpha,pname);

    return(color);
}

/******************************************************************************/

int GRAPHICS_ColorDefine( float r, float b, float g, int pname )
{
int color;
float alpha=1.0;

    color = GRAPHICS_ColorDefine(r,b,g,alpha,pname);

    return(color);
}

/******************************************************************************/

int GRAPHICS_ColorDefine( float r, float b, float g, float alpha, int pname )
{
int i,color=NOCOLOR;

    for( i=0; (i < GRAPHICS_COLOR_ROWS); i++ )
    {
        if( !GRAPHICS_ColorValid(GRAPHICS_ColorTable[i]) )
        {
            color = i;

            GRAPHICS_ColorTable[color][GRAPHICS_COLOR_R] = r;
            GRAPHICS_ColorTable[color][GRAPHICS_COLOR_G] = g;
            GRAPHICS_ColorTable[color][GRAPHICS_COLOR_B] = b;
            GRAPHICS_ColorTable[color][GRAPHICS_COLOR_ALPHA] = alpha;
            GRAPHICS_ColorTable[color][GRAPHICS_COLOR_PNAME] = (float)pname;

            break;
        }
    }

    return(color);
}

/******************************************************************************/

void GRAPHICS_ColorEntry( float entry[], float r, float b, float g, float alpha, int pname )
{
    entry[GRAPHICS_COLOR_R] = r;
    entry[GRAPHICS_COLOR_G] = g;
    entry[GRAPHICS_COLOR_B] = b;
    entry[GRAPHICS_COLOR_ALPHA] = alpha;
    entry[GRAPHICS_COLOR_PNAME] = (float)pname;
}

/******************************************************************************/

void GRAPHICS_ColorUndefine( float entry[] )
{
    GRAPHICS_ColorEntry(entry,-1.0,-1.0,-1.0,1.0,GL_AMBIENT_AND_DIFFUSE);
}

/******************************************************************************/

int GRAPHICS_ClearColorValue;
int GRAPHICS_ClearColorStack;

/******************************************************************************/

void GRAPHICS_ClearColor( int color )
{
GLclampf r,g,b,a=1.0;

    if( !GRAPHICS_ColorValid(color) )
    {
        return;
    }

    r = GRAPHICS_ColorTable[color][GRAPHICS_COLOR_R];
    g = GRAPHICS_ColorTable[color][GRAPHICS_COLOR_G];
    b = GRAPHICS_ColorTable[color][GRAPHICS_COLOR_B];

    GRAPHICS_ClearColorValue = color;

    GRAPHICS_debugf("glClearColor(RGB=%.2f,%.2f,%.2f); color=%d\n",r,g,b,color);
    glClearColor(r,g,b,a);
}

/******************************************************************************/

void GRAPHICS_ClearColor( void )
{
    GRAPHICS_ClearColor(GRAPHICS_ClearColorDefault);
}

/******************************************************************************/

void GRAPHICS_ClearColorPush( void )
{
    GRAPHICS_ClearColorStack = GRAPHICS_ClearColorValue;
}

/******************************************************************************/

void GRAPHICS_ClearColorPop( void )
{
    GRAPHICS_ClearColorValue = GRAPHICS_ClearColorStack;
    GRAPHICS_ClearColor(GRAPHICS_ClearColorValue);
}

/******************************************************************************/

void GRAPHICS_Capability( int capability, BOOL flag )
{
    if( flag )
    {
        glEnable(capability);
    }
    else
    {
        glDisable(capability);
    }
}

/******************************************************************************/

int GRAPHICS_CapabilityIndex( int capability )
{
int i,index;

    for( index=GRAPHICS_CAPABILITY_INVALID,i=0; (i < GRAPHICS_CAPABILITY_INDEX); i++ )
    {
        if( capability == GRAPHICS_CapabilityTable[i] )
        {
            index = i;
            break;
        }
    }

    if( index == GRAPHICS_CAPABILITY_INVALID )
    {
        GRAPHICS_errorf("Invalid OpenGL capability %d.\n",capability);
    }

    return(index);
}

/******************************************************************************/

void GRAPHICS_CapabilityPush( int capability, BOOL flag )
{
int index,level;

    if( (index=GRAPHICS_CapabilityIndex(capability)) == GRAPHICS_CAPABILITY_INVALID )
    {
        return;
    }

    if( GRAPHICS_CapabilityLevel[index] < GRAPHICS_CAPABILITY_STACK )
    {
        level = GRAPHICS_CapabilityLevel[index]++;
        GRAPHICS_CapabilityStack[index][level] = glIsEnabled(capability);
    }

    GRAPHICS_Capability(capability,flag);
}

/******************************************************************************/

void GRAPHICS_CapabilityPop( int capability )
{
BOOL flag;
int index,level;

    if( (index=GRAPHICS_CapabilityIndex(capability)) == GRAPHICS_CAPABILITY_INVALID )
    {
        return;
    }

    if( GRAPHICS_CapabilityLevel[index] > 0 )
    {
        level = --GRAPHICS_CapabilityLevel[index];
        flag = GRAPHICS_CapabilityStack[index][level];
        GRAPHICS_Capability(capability,flag);
    }
}

/******************************************************************************/

void GRAPHICS_CapabilityEnable( int capability )
{
    GRAPHICS_CapabilityPush(capability,TRUE);
}
/******************************************************************************/

void GRAPHICS_CapabilityDisable( int capability )
{
    GRAPHICS_CapabilityPush(capability,FALSE);
}

/******************************************************************************/

void GRAPHICS_StandardOpenGL( BYTE flag, int clearcolor )
{
GLfloat mat_ambient[] = { 1.0,1.0,1.0,1.0 };
GLfloat mat_specular[] = { 1.0,1.0,1.0,1.0 };
GLfloat light_ambient[] = { 0.5,0.5,0.5,1.0 };
GLfloat light_position[] = { 0.0,0.0,0.0,1.0 };
GLfloat light_color[] = { 1.0,1.0,1.0,1.0 };
  
    GRAPHICS_messgf("GRAPHICS_StandardOpenGL(flag=0x%02X,clearcolor=%d)\n",flag,clearcolor);

    // Set up various OpenGL materials.
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_specular);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,50.0);

    // Save OpenGL flags to global variable.
    GRAPHICS_FlagOpenGL = flag;

    // Optional lighting.
    if( flag & GRAPHICS_FLAG_LIGHTING )
    {
        light_position[0] = GRAPHICS_LightPosition[0];
        light_position[1] = GRAPHICS_LightPosition[1];
        light_position[2] = GRAPHICS_LightPosition[2];

        light_color[0] = GRAPHICS_LightColor[0];
        light_color[1] = GRAPHICS_LightColor[1];
        light_color[2] = GRAPHICS_LightColor[2];

        glLightfv(GL_LIGHT0,GL_POSITION,light_position);
        glLightfv(GL_LIGHT0,GL_SPECULAR,light_color);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light_ambient);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);  
    }

    glShadeModel(GL_SMOOTH);

     // Depth testing.
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    // Overwrite default clear color if a specific one given.
    if( clearcolor != NOCOLOR )
    {
        GRAPHICS_ClearColorDefault = clearcolor;
    }

    // This will set clear color to the default value.
    GRAPHICS_ClearColor();

    // Transparency implemented as alpha blending.
    if( flag & GRAPHICS_FLAG_TRANSPARENCY )
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }

    if( flag & GRAPHICS_FLAG_ANTIALIASING )
    {
        glEnable(GL_LINE_SMOOTH);
    }

}

/******************************************************************************/

void GRAPHICS_OpenGL( BYTE flag, int clearcolor )
{
    if( GRAPHICS_StereoMode == GRAPHICS_STEREO_OCULUS )
    {
        GRAPHICS_OculusOpenGL(clearcolor);
    }
    else
    {
        GRAPHICS_StandardOpenGL(flag,clearcolor);
    }

    GRAPHICS_messgf("OpenGL version: %s\n",glGetString(GL_VERSION));
}

/******************************************************************************/

void GRAPHICS_OpenGL( BYTE flag )
{
    GRAPHICS_OpenGL(flag,NOCOLOR);
}

/******************************************************************************/

void GRAPHICS_OpenGL( void )
{
    GRAPHICS_OpenGL(GRAPHICS_FLAG_LIGHTING,NOCOLOR);
}

/******************************************************************************/

BOOL GRAPHICS_Lighting( void )
{
BOOL flag;

    flag = ((GRAPHICS_FlagOpenGL & GRAPHICS_FLAG_LIGHTING) > 0);

    return(flag);
}

/******************************************************************************/

void GRAPHICS_LightingDisable( void )
{
    glDisable(GL_LIGHTING);
}

/******************************************************************************/

void GRAPHICS_LightingEnable( void )
{
    if( GRAPHICS_Lighting() )
    {
        glEnable(GL_LIGHTING);
    }
}

/******************************************************************************/

void GRAPHICS_GlutInit( int glut, int wid, int hgt )
{
int argc=1;
char *argv[2] = { "",NULL };

    // General initialization for GLUT.
    glutInit(&argc,argv);

    // Initialize GLUT display mode with required settings...
    glutInitDisplayMode(glut);

    GRAPHICS_DoubleBufferedFlag = ((glut & GLUT_DOUBLE) > 0);

    // Request window size if width and height specified...
    if( (wid*hgt) != 0 )
    {
        glutInitWindowSize(wid,hgt);
    }
}

/******************************************************************************/

int     GRAPHICS_Display=GRAPHICS_DISPLAY_MONO;
int     GRAPHICS_Dimensions=GRAPHICS_3D;

/******************************************************************************/

int     GRAPHICS_Left[] = { EYE_MONO,EYE_LEFT };
int     GRAPHICS_Right[] = { EYE_MONO,EYE_RIGHT };

/******************************************************************************/

int     GRAPHICS_EyeLeft( void )
{
    return(GRAPHICS_Left[GRAPHICS_Display]);
}

/******************************************************************************/

int     GRAPHICS_EyeRight( void )
{
    return(GRAPHICS_Right[GRAPHICS_Display]);
}

/******************************************************************************/

BOOL    GRAPHICS_DisplayMono( void )
{
BOOL    flag;

    flag = (GRAPHICS_Display == GRAPHICS_DISPLAY_MONO);

    return(flag);
}

/******************************************************************************/

BOOL    GRAPHICS_DisplayStereo( void )
{
BOOL    flag;

    flag = (GRAPHICS_Display == GRAPHICS_DISPLAY_STEREO);

    return(flag);
}

/******************************************************************************/

BOOL    GRAPHICS_Display3D( void )
{
BOOL    flag;

    flag = (GRAPHICS_Dimensions == GRAPHICS_3D);

    return(flag);
}

/******************************************************************************/

BOOL    GRAPHICS_Display2D( void )
{
BOOL    flag;

    flag = (GRAPHICS_Dimensions == GRAPHICS_2D);

    return(flag);
}

/******************************************************************************/

int     GRAPHICS_Dimension( void )
{
int     d;

    d = GRAPHICS_Dimensions;

    return(d);
}

/******************************************************************************/

float   GRAPHICS_CalibRange[GRAPHICS_3D][GRAPHICS_RANGE];
float   GRAPHICS_CalibCentre[GRAPHICS_3D]= { 0,0,0 };
float   GRAPHICS_CalibValidationError=0.0;
float   GRAPHICS_CalibMatrix[OPENGL_VECTOR]= { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

matrix  GRAPHICS_CalibCentrePOMX;

/******************************************************************************/

BOOL    GRAPHICS_Calib3D( void )
{
BOOL flag;

    flag = (GRAPHICS_CalibRange[GRAPHICS_Z][0] != GRAPHICS_CalibRange[GRAPHICS_Z][1]);

    return(flag);
}

/******************************************************************************/

BOOL    GRAPHICS_Calib2D( void )
{
BOOL flag;

    flag = (GRAPHICS_CalibRange[GRAPHICS_Z][0] == GRAPHICS_CalibRange[GRAPHICS_Z][1]);

    return(flag);
}

/******************************************************************************/

void    GRAPHICS_CalibCnfg( void )
{
    CONFIG_reset();

    // Set up variable table for calibration ...
    CONFIG_set("Xmin,Xmax",GRAPHICS_CalibRange[GRAPHICS_X],GRAPHICS_RANGE);
    CONFIG_set("Ymin,Ymax",GRAPHICS_CalibRange[GRAPHICS_Y],GRAPHICS_RANGE);
    CONFIG_set("Zmin,Zmax",GRAPHICS_CalibRange[GRAPHICS_Z],GRAPHICS_RANGE);
    CONFIG_set("Centre",GRAPHICS_CalibCentre,GRAPHICS_3D);
    CONFIG_set("ValidationError",GRAPHICS_CalibValidationError);
    CONFIG_set("Matrix",GRAPHICS_CalibMatrix,OPENGL_VECTOR);
}

/******************************************************************************/

BOOL    GRAPHICS_CalibCnfg( int func, char *file )
{
BOOL    ok=FALSE;

    // Set calibration configuration variables...
    GRAPHICS_CalibCnfg();

    switch( func )
    {
        case FILE_SAVE :     // Save configuration file...
            ok = CONFIG_write(file);
            break;

        case FILE_LOAD :     // Load configuration file...
            ok = CONFIG_read(file);
            break;	
    }

    if( !ok )
    {
        GRAPHICS_errorf("GRAPHICS_Calib%s(%s) Failed.\n",FILE_Func(func),file);
        return(FALSE);
    }

    GRAPHICS_messgf("GRAPHICS_Calib%s(%s) OK.\n",FILE_Func(func),file);
    CONFIG_list(GRAPHICS_messgf);

    // Convert calibration centre to a matrix (more useful)...
    SPMX_xyz2pomx(GRAPHICS_CalibCentre,GRAPHICS_CalibCentrePOMX);

    return(TRUE);
}

/******************************************************************************/

BOOL    GRAPHICS_CalibSave( char *path )
{
BOOL    ok;

    ok = GRAPHICS_CalibCnfg(FILE_SAVE,path);

    return(ok);
}

/******************************************************************************/

BOOL    GRAPHICS_CalibLoad( char *path )
{
int     type;
BOOL    ok=FALSE;

    ok = GRAPHICS_CalibCnfg(FILE_LOAD,path);

    return(ok);
}

/******************************************************************************/

char *GRAPHICS_CalibFile( char *name, int dimensions )
{
static STRING file;
char *path=NULL;

    // Check for user-defined name...
    if( name == NULL )
    {
        name = GRAPHICS_CalibrationFile;
    }

    // Check for user-defined dimensions...
    if( dimensions == 0 )
    {
        dimensions = GRAPHICS_Dimensions;
    }

    // Calibration file name...
    strncpy(file,STR_stringf(name,dimensions),STRLEN);

    if( (path=FILE_Calibration(file)) == NULL )
    {
        GRAPHICS_errorf("GRAPHICS_CalibFile(file=%s) Cannot find file.\n",file);
        return(NULL);
    }

    GRAPHICS_debugf("GRAPHICS_CalibFile(name=%s,dimensions=%d) path=%s\n",name,dimensions,path);
    strncpy(file,path,STRLEN);

    return(file);
}

/******************************************************************************/

char *GRAPHICS_CalibFile( int dimensions )
{
char *path;

    path = GRAPHICS_CalibFile(NULL,dimensions);

    return(path);
}

/******************************************************************************/

char *GRAPHICS_CalibFile( char *name )
{
char *path;

    path = GRAPHICS_CalibFile(name,0);

    return(path);
}

/******************************************************************************/

char *GRAPHICS_CalibFile( void )
{
char *path;

    path = GRAPHICS_CalibFile(0);

    return(path);
}

/******************************************************************************/

//      Frustum matrix (linear RTMX) for LEFT,RIGHT and MONO...
double  GRAPHICS_Frustum[EYE_MAX][OPENGL_VECTOR];

double  GRAPHICS_FrustumLeft[EYE_MAX];
double  GRAPHICS_FrustumRight[EYE_MAX];
double  GRAPHICS_FrustumTop[EYE_MAX];
double  GRAPHICS_FrustumBottom[EYE_MAX];

/******************************************************************************/

void GRAPHICS_Reshape( int w, int h )
{
    GRAPHICS_FrustumInit();  // Not sure we have to do this each time?
    GRAPHICS_SetWindow();
    glViewport(0,0,w,h);     // Define the viewing port...
}

/******************************************************************************/

void GRAPHICS_FocusWindow( HWND window )
{
BOOL ok=FALSE;

    // Make sure GRAPHICS system is running...
    if( !GRAPHICS_Started() )
    {
        return;
    }

    if( window != NULL )
    {
        ok = SetForegroundWindow(window);
    }

    if( !ok )
    {
        GRAPHICS_errorf("GRAPHICS_FocusWindow() Cannot set focus.\n");
    }
}

/******************************************************************************/

void GRAPHICS_FocusParent( void )
{
    GRAPHICS_FocusWindow(GRAPHICS_WindowParent);
}

/******************************************************************************/

void GRAPHICS_FocusGLUT( void )
{
    GRAPHICS_FocusWindow(GRAPHICS_WindowGLUT);
}

/******************************************************************************/

BOOL GRAPHICS_Start( char *cnfg, int mode, char *name, int &ID )
{
BOOL ok;

    ok = GRAPHICS_Start(cnfg,mode,0,0,name,ID);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_Start( char *cnfg, int mode, int wid, int hgt, char *name )
{
BOOL ok;
int ID;

    ok = GRAPHICS_Start(cnfg,mode,wid,hgt,name,ID);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_Initialized( void )
{
BOOL flag;

    flag = GRAPHICS_InitializedFlag;

    return(flag);
}

/******************************************************************************/

BOOL GRAPHICS_Started( void )
{
BOOL flag;

    flag = GRAPHICS_Initialized() && GRAPHICS_StartedFlag;

    return(flag);
}

/******************************************************************************/

void GRAPHICS_Stop( void )
{
    if( GRAPHICS_Started() )
    {
        return;
    }

    if( GRAPHICS_StereoMode == GRAPHICS_STEREO_OCULUS )
    {
        GRAPHICS_OculusStop();
    }

    // Destroy GLUT/OpenGL window.
    GRAPHICS_WindowDestroy();

    if( GRAPHICS_StereoMode != GRAPHICS_STEREO_OCULUS )
    {
        // Restore default display mode if required.
        GRAPHICS_DisplayConfigure(OFF);
    }

    GRAPHICS_StartedFlag = FALSE;
}

/******************************************************************************/

BOOL GRAPHICS_Start( char *cnfg, int mode, int wid, int hgt, char *name, int &ID )
{
int glut=GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE;
BOOL ok=TRUE;
int display;

    GRAPHICS_messgf("GRAPHICS_Start(cnfg=%s,mode=0x%02X,wid=%d,hgt=%d,name=%s)\n",cnfg,mode,wid,hgt,name);

    // Make sure GRAPHICS system not already running...
    if( GRAPHICS_Started() )
    {
        GRAPHICS_debugf("GRAPHICS_Start() Already running.\n");
        return(TRUE);
    }

    // Initialize GRAPHICS system...
    if( !GRAPHICS_Init(cnfg) )
    {
        GRAPHICS_errorf("GRAPHICS_Start() Cannot load configuration.\n");
        return(FALSE);
    }

    // Default display name from GRAPHICS configuration file...
    if( name == NULL )
    {
        name = GRAPHICS_Description;
    }

    GRAPHICS_VerticalRetraceFrequency = (double)GRAPHICS_DisplayFrequency;
    GRAPHICS_VerticalRetracePeriod = 1.0 / GRAPHICS_VerticalRetraceFrequency;

    // Mask off display mode...
    display = (mode & GRAPHICS_DISPLAY);

    // Use default mode if specified.
    if( display == GRAPHICS_DISPLAY_DEFAULT )
    {
        display = GRAPHICS_DefaultMode;
    }

    // Graphics display mode...    
    switch( display )
    {
        case GRAPHICS_DISPLAY_STEREO :      // Stereoscopic 3D...
           if( GRAPHICS_StereoMode == GRAPHICS_STEREO_NONE )
           {
               GRAPHICS_errorf("GRAPHICS_Start() Machine is not stereo-capable.\n");
               ok = FALSE;
               break;
           }

           if(GRAPHICS_StereoMode == GRAPHICS_STEREO_OCULUS )
           {
               if( !(ok=GRAPHICS_OculusStart(name)) )
               {
                   break;
               }
           }

           if( GRAPHICS_StereoMode == GRAPHICS_STEREO_FRAMEALTERNATE )
           {
               glut |= GLUT_STEREO;
           }

           GRAPHICS_Display = GRAPHICS_DISPLAY_STEREO;
           GRAPHICS_Dimensions = GRAPHICS_3D;
           break;

        case GRAPHICS_DISPLAY_MONO :        // Regular 3D...
           GRAPHICS_Display = GRAPHICS_DISPLAY_MONO;
           GRAPHICS_Dimensions = GRAPHICS_3D;
           break;

        case GRAPHICS_DISPLAY_2D :          // 2D...
           GRAPHICS_Display = GRAPHICS_DISPLAY_MONO;
           GRAPHICS_Dimensions = GRAPHICS_2D;
           break;

        default :
           GRAPHICS_errorf("GRAPHICS_Start() Invalid display mode (0x%02X).\n",display);
           ok = FALSE;
           break;
    }

    if( !ok )
    {
        return(FALSE);
    }

    // Some stuff either already done or not applicable to Oculus HMD...
    if( GRAPHICS_StereoMode != GRAPHICS_STEREO_OCULUS )
    {
        // Configure display if required.
        if( !GRAPHICS_DisplayConfigure(ON) )
        {
            GRAPHICS_errorf("GRAPHICS_Start() Invalid display configuration.\n");
        }

        // Initialize GLUT...
        GRAPHICS_GlutInit(glut,wid,hgt);

        // Create default window...
        ID = glutCreateWindow(name);
        GRAPHICS_WindowID = ID;

        // Register default window re-shape function...
        glutReshapeFunc(GRAPHICS_Reshape);

        // Make full screen if width and height not specified...
        if( (wid*hgt) == 0 )
        {
            glutFullScreen();
        }

        // Hide mouse unless otherwise specified...
        if( (mode & GRAPHICS_SHOWMOUSE) == 0 )
        {
            glutSetCursor(GLUT_CURSOR_NONE);
        }

        // Find handle of GLUT window using name...
        if( (GRAPHICS_WindowGLUT=FindWindow(NULL,name)) == NULL )
        {
            GRAPHICS_errorf("GRAPHICS_Start() Cannot find window (%s).\n",name);
        }

        // Set window focus to GLUT window...
        if( (mode & GRAPHICS_DONTFOCUS) == 0 )
        {
            GRAPHICS_FocusGLUT();
        }

        // Initialize frustrum (depending on display mode)...
        GRAPHICS_FrustumInit();

        // Clear graphics buffers...
        GRAPHICS_ClearBuffers();

        // Load calibration file...
        ok = GRAPHICS_CalibUse();
    }

    // Set GRAPHICS system started flag...
    GRAPHICS_StartedFlag = TRUE;

    // Start mouse processing...
    if( !MOUSE_Start() )
    {
        GRAPHICS_errorf("GRAPHICS_Start() Cannot start mouse processing.\n");
    }

    return(ok);
}

/******************************************************************************/

BOOL    GRAPHICS_Start( char *cnfg, int mode, char *name )
{
BOOL    ok;
int     ID;

    ok = GRAPHICS_Start(cnfg,mode,name,ID);

    return(ok);
}

/******************************************************************************/

BOOL    GRAPHICS_Start( int mode, char *name )
{
char   *cnfg=NULL;
BOOL    ok;

    ok = GRAPHICS_Start(cnfg,mode,name);

    return(ok);
}

/******************************************************************************/

BOOL    GRAPHICS_Start( char *cnfg, int mode )
{
char   *name=NULL;
BOOL    ok;

    ok = GRAPHICS_Start(cnfg,mode,name);

    return(ok);
}

/******************************************************************************/

BOOL    GRAPHICS_Start( int mode )
{
char   *cnfg=NULL;
char   *name=NULL;
BOOL    ok;

    ok = GRAPHICS_Start(cnfg,mode,name);

    return(ok);
}
/******************************************************************************/

BOOL    GRAPHICS_Start( int mode, int wid, int hgt, char *name, int &ID )
{
BOOL    ok;

    ok = GRAPHICS_Start(NULL,mode,wid,hgt,name,ID);

    return(ok);
}

/******************************************************************************/

BOOL    GRAPHICS_Start( int mode, int wid, int hgt, char *name )
{
BOOL    ok;

    ok = GRAPHICS_Start(NULL,mode,wid,hgt,name);

    return(ok);
}

/******************************************************************************/

BOOL    GRAPHICS_Start( int mode, char *name, int &ID )
{
char   *cnfg=NULL;
BOOL    ok;

    ok = GRAPHICS_Start(cnfg,mode,name,ID);

    return(ok);
}

/******************************************************************************/

int GRAPHICS_BufferEye[EYE_MAX] = { GL_BACK_LEFT,GL_BACK_RIGHT,GL_BACK };

void GRAPHICS_BufferSet( int eye )
{
int     buffer=eye;
float   x,y;

    x = GRAPHICS_DisplayPixels[GRAPHICS_X];
    y = GRAPHICS_DisplayPixels[GRAPHICS_Y];

    // If dual screen stereo, set view port for left or right screen.
    if( GRAPHICS_StereoMode == GRAPHICS_STEREO_DUALSCREEN )
    {
        buffer = EYE_MONO;

        switch( eye )
        {
            case EYE_LEFT :
            case EYE_MONO :
               glViewport(0,0,(x/2)-1,y-1);
               break;

            case EYE_RIGHT :
               glViewport(x/2,0,(x/2)-1,y-1);
               break;
        }
    }

    // Set draw buffer appropriate for eye and stereo mode.
    glDrawBuffer(GRAPHICS_BufferEye[buffer]);
}

/******************************************************************************/

void GRAPHICS_View( BOOL calib, BOOL centre, float x, float y, float z, int eye )
{
float ex;
float esep[EYE_MAX]={ -ESEP,+ESEP,0.0 };

    // Set X value for this eye...
    ex = EX + esep[eye];

    // Load frustum for this eye...
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(GRAPHICS_Frustum[eye]);

    // Set draw buffer for this eye...
    GRAPHICS_BufferSet(eye);
  
    // Translate view to eye position...
    glMatrixMode(GL_MODELVIEW);	
    glLoadIdentity();
    glTranslatef(-ex,-EY,-EZ);

    // Rotation for orientation of monitor (for portrait vBOT rig).
    glRotatef(GRAPHICS_DisplayRotation[GRAPHICS_X],GL_ROTATE_X);
    glRotatef(GRAPHICS_DisplayRotation[GRAPHICS_Y],GL_ROTATE_Y);
    glRotatef(GRAPHICS_DisplayRotation[GRAPHICS_Z],GL_ROTATE_Z);

    // Use calibration matrix...
    if( calib )
    {
        glMultMatrixf(GRAPHICS_CalibMatrix);
    }

    // Discard Z-axis (depth) if using 2D graphics...
    if( GRAPHICS_Dimensions == GRAPHICS_2D )
    {
        z = GRAPHICS_FocalPlane;
    }

    if( centre )
    {
        x += GRAPHICS_CalibCentre[GRAPHICS_X];
        y += GRAPHICS_CalibCentre[GRAPHICS_Y];
        z += GRAPHICS_CalibCentre[GRAPHICS_Z];
    }

    // Translate to required position...
    glTranslatef(x,y,z);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib, BOOL centre, float x, float y, int eye )
{
float z=0;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib, BOOL centre, matrix &xyz, int eye )
{
float   x=0,y=0,z=0;

    SPMX_mtx2xyz(xyz,x,y,z);
    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib, BOOL centre, int eye )
{
float x=0,y=0,z=0;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib, float x, float y, float z, int eye )
{
BOOL centre=FALSE;

    GRAPHICS_View(calib,FALSE,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib, float x, float y, int eye )
{
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib, matrix &xyz, int eye )
{
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,xyz,eye);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib, int eye )
{
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/
/******************************************************************************/

void    GRAPHICS_View( BOOL calib, float x, float y, float z )
{
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib, float x, float y )
{
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib, matrix &xyz )
{
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,xyz,eye);
}

/******************************************************************************/

void    GRAPHICS_View( BOOL calib )
{
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/
/******************************************************************************/

void    GRAPHICS_ViewNoCalib( float x, float y, float z, int eye )
{
BOOL calib=FALSE;
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalib( float x, float y, int eye )
{
BOOL calib=FALSE;
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalib( matrix &xyz, int eye )
{
BOOL calib=FALSE;
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,xyz,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalib( int eye )
{
BOOL calib=FALSE;
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/
/******************************************************************************/

void    GRAPHICS_ViewNoCalib( float x, float y, float z )
{
BOOL calib=FALSE;
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalib( float x, float y )
{
BOOL calib=FALSE;
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalib( matrix &xyz )
{
BOOL calib=FALSE;
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,xyz,eye);
}


/******************************************************************************/

void    GRAPHICS_ViewNoCalib( void )
{
BOOL calib=FALSE;
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/
/******************************************************************************/

void    GRAPHICS_ViewCalib( float x, float y, float z, int eye )
{
BOOL calib=TRUE;
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalib( float x, float y, int eye )
{
BOOL calib=TRUE;
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalib( matrix &xyz, int eye )
{
BOOL calib=TRUE;
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,xyz,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalib( int eye )
{
BOOL calib=TRUE;
BOOL centre=FALSE;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/
/******************************************************************************/

void    GRAPHICS_ViewCalib( float x, float y, float z )
{
BOOL calib=TRUE;
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalib( float x, float y )
{
BOOL calib=TRUE;
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalib( matrix &xyz )
{
BOOL calib=TRUE;
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,xyz,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalib( void )
{
BOOL calib=TRUE;
BOOL centre=FALSE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/
/******************************************************************************/

void    GRAPHICS_ViewNoCalibCentre( float x, float y, float z, int eye )
{
BOOL calib=FALSE;
BOOL centre=TRUE;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalibCentre( float x, float y, int eye )
{
BOOL calib=FALSE;
BOOL centre=TRUE;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalibCentre( matrix &xyz, int eye )
{
BOOL calib=FALSE;
BOOL centre=TRUE;

    GRAPHICS_View(calib,centre,xyz,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalibCentre( int eye )
{
BOOL calib=FALSE;
BOOL centre=TRUE;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/
/******************************************************************************/

void    GRAPHICS_ViewNoCalibCentre( float x, float y, float z )
{
BOOL calib=FALSE;
BOOL centre=TRUE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalibCentre( float x, float y )
{
BOOL calib=FALSE;
BOOL centre=TRUE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalibCentre( matrix &xyz )
{
BOOL calib=FALSE;
BOOL centre=TRUE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,xyz,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewNoCalibCentre( void )
{
BOOL calib=FALSE;
BOOL centre=TRUE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/
/******************************************************************************/

void    GRAPHICS_ViewCalibCentre( float x, float y, float z, int eye )
{
BOOL calib=TRUE;
BOOL centre=TRUE;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalibCentre( float x, float y, int eye )
{
BOOL calib=TRUE;
BOOL centre=TRUE;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalibCentre( matrix &xyz, int eye )
{
BOOL calib=TRUE;
BOOL centre=TRUE;

    GRAPHICS_View(calib,centre,xyz,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalibCentre( int eye )
{
BOOL calib=TRUE;
BOOL centre=TRUE;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalibCentre( float x, float y, float z )
{
BOOL calib=TRUE;
BOOL centre=TRUE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,z,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalibCentre( float x, float y )
{
BOOL calib=TRUE;
BOOL centre=TRUE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,x,y,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalibCentre( matrix &xyz )
{
BOOL calib=TRUE;
BOOL centre=TRUE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,xyz,eye);
}

/******************************************************************************/

void    GRAPHICS_ViewCalibCentre( void )
{
BOOL calib=TRUE;
BOOL centre=TRUE;
int eye=EYE_MONO;

    GRAPHICS_View(calib,centre,eye);
}

/******************************************************************************/

void    GRAPHICS_FrustumSet( int eye, BOOL orthographic, float scale, float Xeye )
{
float   L,R,T,B;
float   Xmin,Xmax,Ymin,Ymax,Zmin,Zmax;
float   Yeye,Zeye;
float   zNear,zFar;

    Xmin = GRAPHICS_DisplaySize[GRAPHICS_X][GRAPHICS_MIN];
    Xmax = GRAPHICS_DisplaySize[GRAPHICS_X][GRAPHICS_MAX];
    Ymin = GRAPHICS_DisplaySize[GRAPHICS_Y][GRAPHICS_MIN];
    Ymax = GRAPHICS_DisplaySize[GRAPHICS_Y][GRAPHICS_MAX];

    zNear = GRAPHICS_DisplaySize[GRAPHICS_Z][GRAPHICS_MIN];
    zFar = GRAPHICS_DisplaySize[GRAPHICS_Z][GRAPHICS_MAX];

    Yeye = GRAPHICS_EyeCentre[GRAPHICS_Y];
    Zeye = GRAPHICS_EyeCentre[GRAPHICS_Z];

    // Switch to projection matrix and clear...
    glMatrixMode(GL_PROJECTION);	
    glLoadIdentity();

    // Define screen...
    L = scale*(Xmin-Xeye);   // Left side of screen.
    R = scale*(Xmax-Xeye);   // Right side of screen.
    T = scale*(Ymax-Yeye);   // Top of screen.
    B = scale*(Ymin-Yeye);   // Bottom of screen.

    if( orthographic )
    {
        glOrtho(L,R,B,T,zNear,zFar);
    }
    else
    {
        glFrustum(L,R,B,T,zNear,zFar);
    }

    GRAPHICS_debugf("glFrustum(x=%.2f,%.2f,y=%.2f,%.2f,z=%.2f,%.2f)\n",L,R,B,T,zNear,zFar);

    glGetDoublev(GL_PROJECTION_MATRIX,GRAPHICS_Frustum[eye]);

    GRAPHICS_FrustumLeft[eye]   = L;
    GRAPHICS_FrustumRight[eye]  = R;
    GRAPHICS_FrustumTop[eye]    = T;
    GRAPHICS_FrustumBottom[eye] = B;
}

/******************************************************************************/

void    GRAPHICS_FrustumInit( BOOL orthographic )
{
float   Zrange,scale,Xeye;

    // Range is based on distance between Z position of eye and focal plane...
    Zrange = fabs(GRAPHICS_EyeCentre[GRAPHICS_Z]-GRAPHICS_FocalPlane);

    // Default scale...
    scale = 1.0;

    // Calculate scale for 3D view if range is non-zero...
    if( (GRAPHICS_Dimensions == GRAPHICS_3D) && (Zrange != 0.0) )
    {
        scale = 1.0 / Zrange;
    }

    // Left eye...
    Xeye = GRAPHICS_EyeCentre[GRAPHICS_X] - GRAPHICS_PupilToCentre;
    GRAPHICS_FrustumSet(EYE_LEFT,orthographic,scale,Xeye);

    // Right eye...
    Xeye = GRAPHICS_EyeCentre[GRAPHICS_X] + GRAPHICS_PupilToCentre;
    GRAPHICS_FrustumSet(EYE_RIGHT,orthographic,scale,Xeye);

    // Monocular..
    Xeye = GRAPHICS_EyeCentre[GRAPHICS_X];
    GRAPHICS_FrustumSet(EYE_MONO,orthographic,scale,Xeye);
}

/******************************************************************************/

void    GRAPHICS_FrustumInit( void )
{
BOOL    ortho;

    // Use orthographic projection matrix for 2D...
    ortho = (GRAPHICS_Dimensions == GRAPHICS_2D);

    // Set projection matrix...
    GRAPHICS_FrustumInit(ortho);
}

/******************************************************************************/

BOOL GRAPHICS_CalibUse( char *name, int dimensions )
{
char *path;
BOOL ok=FALSE;

    if( (path=GRAPHICS_CalibFile(name,dimensions)) != NULL )
    {
        GRAPHICS_debugf("GRAPHICS_CalibUse(name=%s,dimensions=%d) path=%s\n",name,dimensions,path);
        ok = GRAPHICS_CalibLoad(path);
    }

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_CalibUse( int dimensions )
{
BOOL ok;

    ok = GRAPHICS_CalibUse(NULL,dimensions);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_CalibUse( char *name )
{
BOOL ok;

    ok = GRAPHICS_CalibUse(name,0);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_CalibUse( void )
{
BOOL ok;

    ok = GRAPHICS_CalibUse(NULL,0);

    return(ok);
}

/******************************************************************************/

void GRAPHICS_Clear( GLbitfield mask )
{
    glDrawBuffer(GL_BACK);
    glClear(mask);
}

/******************************************************************************/

void GRAPHICS_Clear( void )
{
    GRAPHICS_Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/******************************************************************************/

void GRAPHICS_ClearBuffers( void )
{
int i;

    for( i=0; (i < 2); i++ )
    {
        GRAPHICS_Clear();
        glutSwapBuffers();
    }
}

/******************************************************************************/

TIMER GRAPHICS_SwapBuffersOnsetTimer("GRAPHICS_SwapBuffersOnset");
TIMER GRAPHICS_SwapBuffersOffsetTimer("GRAPHICS_SwapBuffersOffset");

void GRAPHICS_SwapBuffers( void )
{
    GRAPHICS_SwapBuffersOnsetTimer.Reset();
    glutSwapBuffers();
    GRAPHICS_SwapBuffersOffsetTimer.Reset();
    GRAPHICS_SwapBuffersCount++;
    GRAPHICS_SwapBuffersToVerticalRetraceTime = GRAPHICS_VerticalRetraceOnsetTimeUntilNext();
    GRAPHICS_SwapBuffersToVerticalRetraceTimeData.Data(GRAPHICS_SwapBuffersToVerticalRetraceTime);
}

/******************************************************************************/
/* Some graphics cards (eg. 3DLabs WildCat VP) do not support stereo depth    */
/* buffers. In such cases the depth buffer must be cleared after drawing the  */
/* view for each eye. The clear functions below take this into account.       */
/******************************************************************************/

void GRAPHICS_ClearStereo( void )
{
GLbitfield mask;
BOOL flag;

    // In stereo mode there are buffers for both eyes.
    // These "stereo buffers" can be cleared here...

    // Always clear color ("pixel") buffer...
    mask = GL_COLOR_BUFFER_BIT;

    // Do we clear depth buffer here?
    if( GRAPHICS_Display2D() )
    {
        // Yes, if 2D display.
        flag = TRUE;
    }
    else
    if( GRAPHICS_Display3D() )
    {
        // Yes, if 3D Mono display or if graphics card supports stereo depth buffers.
        flag = GRAPHICS_DisplayMono() || GRAPHICS_StereoDepth;
    }

    if( flag  )
    {
        mask |= GL_DEPTH_BUFFER_BIT;
    }

    // Make sure we're clearing the back buffers...
    glDrawBuffer(GL_BACK);
    glClear(mask);
}

/******************************************************************************/

void GRAPHICS_ClearMono( void )
{
    // In stereo mode there can be buffers that only work for a single eye.
    // These "mono buffers" must be cleared here...

    // If stereo display and stereo depth buffers not supported...
    if( GRAPHICS_DisplayStereo() && !GRAPHICS_StereoDepth )
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

/******************************************************************************/

BOOL GRAPHICS_Screen( int p[], double c[] )
{
BOOL ok;

    ok = GRAPHICS_Screen(p[GRAPHICS_X],p[GRAPHICS_Y],c[GRAPHICS_X],c[GRAPHICS_Y]);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_Screen( int &px, int &py, double &cx, double &cy )
{
    // Make sure GRAPHICS system initialized...
    if( !GRAPHICS_Initialized() )
    {
        return(FALSE);
    }

    // Dimensions of screen in pixels...
    px = GRAPHICS_DisplayPixels[GRAPHICS_X];
    py = GRAPHICS_DisplayPixels[GRAPHICS_Y];

    // Dimensions of screen in centimetres...
    cx = fabs(GRAPHICS_DisplaySize[GRAPHICS_X][GRAPHICS_MAX] - GRAPHICS_DisplaySize[GRAPHICS_X][GRAPHICS_MIN]);
    cy = fabs(GRAPHICS_DisplaySize[GRAPHICS_Y][GRAPHICS_MAX] - GRAPHICS_DisplaySize[GRAPHICS_Y][GRAPHICS_MIN]);

    return(TRUE);
}

/******************************************************************************/

BOOL GRAPHICS_Screen( double &L, double &R, double &T, double &B, double &W, double &H )
{
    // Make sure GRAPHICS system initialized...
    if( !GRAPHICS_Initialized() )
    {
        return(FALSE);
    }

    GRAPHICS_Screen(L,R,B,T);
    GRAPHICS_Screen(W,H);

    return(FALSE);
}

/******************************************************************************/

BOOL GRAPHICS_Screen( double &L, double &R, double &T, double &B )
{
    // Make sure GRAPHICS system initialized...
    if( !GRAPHICS_Initialized() )
    {
        return(FALSE);
    }

    //L = GRAPHICS_FrustumLeft[EYE_MONO];
    //R = GRAPHICS_FrustumRight[EYE_MONO];
    //T = GRAPHICS_FrustumTop[EYE_MONO];
    //B = GRAPHICS_FrustumBottom[EYE_MONO];

    L = GRAPHICS_DisplaySize[GRAPHICS_X][GRAPHICS_MIN];
    R = GRAPHICS_DisplaySize[GRAPHICS_X][GRAPHICS_MAX];
    T = GRAPHICS_DisplaySize[GRAPHICS_Y][GRAPHICS_MIN];
    B = GRAPHICS_DisplaySize[GRAPHICS_Y][GRAPHICS_MAX];

    return(TRUE);
}

/******************************************************************************/

BOOL GRAPHICS_Screen( double &W, double &H )
{
double L,R,T,B;
BOOL ok;

    if( (ok=GRAPHICS_Screen(L,R,T,B)) )
    {
        W = fabs(R - L);
        H = fabs(T - B);
    }

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_WorkSpace( double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax )
{
    // Make sure GRAPHICS system initialized...
    if( !GRAPHICS_Initialized() )
    {
        return(FALSE);
    }

    xmin = GRAPHICS_CalibRange[GRAPHICS_X][GRAPHICS_MIN];
    xmax = GRAPHICS_CalibRange[GRAPHICS_X][GRAPHICS_MAX];

    ymin = GRAPHICS_CalibRange[GRAPHICS_Y][GRAPHICS_MIN];
    ymax = GRAPHICS_CalibRange[GRAPHICS_Y][GRAPHICS_MAX];

    zmin = GRAPHICS_CalibRange[GRAPHICS_Z][GRAPHICS_MIN];
    zmax = GRAPHICS_CalibRange[GRAPHICS_Z][GRAPHICS_MAX];

    return(TRUE);
}

/******************************************************************************/

BOOL GRAPHICS_WorkSpace( double &xmin, double &xmax, double &ymin, double &ymax )
{
double zmin,zmax;
BOOL ok;

    ok = GRAPHICS_WorkSpace(xmin,xmax,ymin,ymax,zmin,zmax);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_WorkSpace( double &xlen, double &ylen )
{
double xmin,xmax,ymin,ymax;
BOOL ok=FALSE;

    if( GRAPHICS_WorkSpace(xmin,xmax,ymin,ymax) )
    {
        ok = TRUE;
        xlen = xmax-xmin;
        ylen = ymax-ymin;
    }

    return(ok);
}

/******************************************************************************/

void GRAPHICS_TranslateCentre( matrix &position )
{
    position += GRAPHICS_CalibCentrePOMX;
}

/******************************************************************************/

void GRAPHICS_Translate( matrix &position )
{
    glTranslated(position(1,1),position(2,1),position(3,1));
}

/******************************************************************************/

void GRAPHICS_Rotate( matrix &rotation )
{
static matrix vector(3,1);
int i;

    for( i=1; (i <= 3); i++ )
    {
        if( rotation(i,1) == 0.0 )
        {
            continue;
        }

        vector.zeros();
        vector(i,1) = 1.0;
        glRotated(rotation(i,1),vector(1,1),vector(2,1),vector(3,1));
    }
}

/******************************************************************************/

BOOL GRAPHICS_DisplayConfigureDefault( void )
{
BOOL flag=TRUE;

    if( GRAPHICS_DisplayPixels[GRAPHICS_X] != GRAPHICS_DefaultPixels[GRAPHICS_X] )
    {
        flag = FALSE;
    }
    else
    if( GRAPHICS_DisplayPixels[GRAPHICS_Y] != GRAPHICS_DefaultPixels[GRAPHICS_Y] )
    {
        flag = FALSE;
    }
    else
    if( GRAPHICS_DisplayFrequency != GRAPHICS_DefaultFrequency )
    {
        flag = FALSE;
    }

    return(flag);
}

/******************************************************************************/

BOOL GRAPHICS_DisplayConfigure( BOOL flag )
{
BOOL ok;

    if( GRAPHICS_DisplayConfigureDefault() )
    {
        return(TRUE);
    }

    if( flag )
    {
        ok = DISPLAY_ModeSet(GRAPHICS_DisplayPixels[GRAPHICS_X],GRAPHICS_DisplayPixels[GRAPHICS_Y],GRAPHICS_DisplayFrequency);
    }
    else
    {
        ok = DISPLAY_ModeSet(GRAPHICS_DefaultPixels[GRAPHICS_X],GRAPHICS_DefaultPixels[GRAPHICS_Y],GRAPHICS_DefaultFrequency);
    }

    return(ok);
}

/******************************************************************************/

struct  DISPLAY_ModeItem  DISPLAY_Mode[DISPLAY_MODES];
int     DISPLAY_Modes=-1;

/******************************************************************************/

struct  STR_TextItem  DISPLAY_ChangeDisplaySettings[] =
{
    { DISP_CHANGE_SUCCESSFUL,"The settings change was successful." },
    { DISP_CHANGE_RESTART,"The computer must be restarted in order for the graphics mode to work." },
    { DISP_CHANGE_BADFLAGS,"An invalid set of flags was passed in." },
    { DISP_CHANGE_BADPARAM,"An invalid parameter was passed in. This can include an invalid flag or combination of flags." },
    { DISP_CHANGE_FAILED,"The display driver failed the specified graphics mode." },
    { DISP_CHANGE_BADMODE,"The graphics mode is not supported." },
    { DISP_CHANGE_NOTUPDATED,"Unable to write settings to the registry." },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

BOOL DISPLAY_ModeGet( int item, DEVMODE *mode )
{
BOOL ok;

    // Get current display settings...
    ok = EnumDisplaySettings(NULL,item,mode);

    return(ok);
}

/******************************************************************************/

BOOL DISPLAY_ModeGet( DEVMODE *mode )
{
BOOL ok;

    // Get current display settings...
    ok = DISPLAY_ModeGet(ENUM_CURRENT_SETTINGS,mode);

    STR_printf(ok,DISPLAY_debugf,DISPLAY_errorf,"DISPLAY_ModeGet(...) %s.\n",STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL DISPLAY_ModeList( void )
{
BOOL ok;
DEVMODE mode;

    for( ok=TRUE,DISPLAY_Modes=0; ((DISPLAY_Modes < DISPLAY_MODES) && ok); )
    {
        if( (ok=DISPLAY_ModeGet(DISPLAY_Modes,&mode)) )
        {
            DISPLAY_Mode[DISPLAY_Modes].Width = mode.dmPelsWidth;
            DISPLAY_Mode[DISPLAY_Modes].Height = mode.dmPelsHeight;
            DISPLAY_Mode[DISPLAY_Modes].Frequency = mode.dmDisplayFrequency;
            DISPLAY_Mode[DISPLAY_Modes].ColorBits = mode.dmBitsPerPel;

            DISPLAY_Modes++;
        }
    }

    ok = (DISPLAY_Modes != 0);

    STR_printf(ok,DISPLAY_debugf,DISPLAY_errorf,"DISPLAY_ModeList() [%d] %s.\n",DISPLAY_Modes,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL DISPLAY_ModeSupported( int width, int height, int frequency )
{
BOOL flag;
int mode;

    // Get list of modes if required...
    if( DISPLAY_Modes == -1 )
    {
        if( !DISPLAY_ModeList() )
        {
            return(FALSE);
        }
    }

    // Go through list to find the specified mode...
    for( flag=FALSE,mode=0; (mode < DISPLAY_Modes); mode++ )
    {
        if( DISPLAY_Mode[mode].Width != width )
        {
            continue;
        }

        if( DISPLAY_Mode[mode].Height != height )
        {
            continue;
        }

        if( DISPLAY_Mode[mode].Frequency != frequency )
        {
            continue;
        }

        flag = TRUE;
        break;
    }

    return(flag);
}

/******************************************************************************/

BOOL DISPLAY_ModeGet( int &width, int &height, int &frequency )
{
BOOL ok;
DEVMODE mode;

    // Get current display settings...
    if( (ok=DISPLAY_ModeGet(&mode)) )
    {
        width = mode.dmPelsWidth;
        height = mode.dmPelsHeight;
        frequency = mode.dmDisplayFrequency;
    }

    return(ok);
}

/******************************************************************************/

BOOL DISPLAY_ModeGet( int &width, int &height )
{
BOOL ok;
int frequency;

    ok = DISPLAY_ModeGet(width,height,frequency);

    return(ok);
}

/******************************************************************************/

BOOL DISPLAY_ModeSet( int width, int height, int frequency )
{
BOOL ok;
DEVMODE mode;
LONG rc;

    // Make sure requested mode is supported...
    if( !DISPLAY_ModeSupported(width,height,frequency) )
    {
        DISPLAY_errorf("DISPLAY_ModeSet(width=%d,height=%d,frequency=%d) Mode not supported.\n",width,height,frequency);
        return(FALSE);
    }

    // Get current settings...
    if( !DISPLAY_ModeGet(&mode) )
    {
        return(FALSE);
    }

    mode.dmFields = 0;
    mode.dmPelsWidth = width;              mode.dmFields |= DM_PELSWIDTH;
    mode.dmPelsHeight = height;            mode.dmFields |= DM_PELSHEIGHT;
    mode.dmDisplayFrequency = frequency;   mode.dmFields |= DM_DISPLAYFREQUENCY;

    // Set display mode...
    rc = ChangeDisplaySettings(&mode,0);
    ok = (rc == DISP_CHANGE_SUCCESSFUL);

    STR_printf(ok,DISPLAY_debugf,DISPLAY_errorf,"DISPLAY_ModeSet(width=%d,height=%d,frequency=%d) %s.\n",width,height,frequency,STR_OkFailed(ok));
    if( !ok )
    {
        DISPLAY_errorf("%ld %s.\n",rc,STR_TextCode(DISPLAY_ChangeDisplaySettings,rc));
    }

    return(ok);
}

/******************************************************************************/

BOOL DISPLAY_Frequency( int &frequency )
{
BOOL ok;
int width,height;

    ok = DISPLAY_ModeGet(width,height,frequency);

    return(ok);
}

/******************************************************************************/

int DISPLAY_StackWidth=0;
int DISPLAY_StackHeight=0;
int DISPLAY_StackFrequency=0;

/******************************************************************************/

BOOL DISPLAY_ModePush( int width, int height, int frequency )
{
    if( !DISPLAY_ModeGet(DISPLAY_StackWidth,DISPLAY_StackHeight,DISPLAY_StackFrequency) )
    {
        return(FALSE);
    }

    if( frequency == 0 )
    {
        frequency = DISPLAY_StackFrequency;
    }

    if( !DISPLAY_ModeSet(width,height,frequency) )
    {
        return(FALSE);
    }

    ATEXIT_Appl(DISPLAY_ModePop);

    return(TRUE);
}

/******************************************************************************/

BOOL DISPLAY_ModePush( int width, int height )
{
BOOL ok;

    ok = DISPLAY_ModePush(width,height,0);

    return(ok);
}

/******************************************************************************/

void DISPLAY_ModePop( void )
{
BOOL ok;

    if( (DISPLAY_StackWidth != 0) && (DISPLAY_StackHeight != 0) )
    {
        ok = DISPLAY_ModeSet(DISPLAY_StackWidth,DISPLAY_StackHeight,DISPLAY_StackFrequency);

        DISPLAY_StackWidth = 0;
        DISPLAY_StackHeight = 0;
        DISPLAY_StackFrequency = 0;
    }
}

/******************************************************************************/

void GRAPHICS_Sphere( matrix *posn, float radius, int color, float alpha )
{
    glPushMatrix();

    if( posn != NULL )
    {
        GRAPHICS_Translate(*posn);
    }

    if( color != NOCOLOR )
    {
        GRAPHICS_ColorSet(color,alpha);
    }

    glutSolidSphere(radius,GRAPHICS_SphereSegments,GRAPHICS_SphereSegments);

    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_Sphere( matrix *posn, float radius, int color )
{
float alpha=1.0;

    GRAPHICS_Sphere(posn,radius,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Sphere( matrix *posn, float radius )
{
int color=NOCOLOR;

    GRAPHICS_Sphere(posn,radius,color);
}

/******************************************************************************/

void GRAPHICS_WireSphere( matrix *posn, float radius, int color, float alpha, int segments )
{
    GRAPHICS_LightingDisable();
    glLineWidth(1.0);

    glPushMatrix();

    if( posn != NULL )
    {
        GRAPHICS_Translate(*posn);
    }

    if( color != NOCOLOR )
    {
        GRAPHICS_ColorSet(color,alpha);
    }

    if( segments == 0 )
    {
        segments = GRAPHICS_WireSphereSegments;
    }

    //glRotatef(90.0,1.0,0.0,0.0);
    glutWireSphere(radius,segments,segments);

    glPopMatrix();
    GRAPHICS_LightingEnable();
}

/******************************************************************************/

void GRAPHICS_WireSphere( matrix *posn, float radius, int color, float alpha )
{
int segments=0;

    GRAPHICS_WireSphere(posn,radius,color,alpha,segments);
}

/******************************************************************************/

void GRAPHICS_WireSphere( matrix *posn, float radius, int color )
{
float alpha=1.0;

    GRAPHICS_WireSphere(posn,radius,color,alpha);
}

/******************************************************************************/

void GRAPHICS_WireSphere( matrix *posn, float radius )
{
int color=NOCOLOR;

    GRAPHICS_Sphere(posn,radius,color);
}

/******************************************************************************/

void GRAPHICS_Circle( matrix *posn, float radius, float width, int color, float alpha )
{
static float z=0.0;
float astep,a;
int i,j;
static matrix V;

    V.dim(GRAPHICS_SphereSegments,2);
    astep = (2.0*PI)/(double)GRAPHICS_SphereSegments;
 
    for( a=0.0,i=1; (i <= GRAPHICS_SphereSegments); i++ )
    {
        V(i,1) = radius * sin(a);
        V(i,2) = radius * cos(a);
        a += astep;
    }

    glPushMatrix();

    if( posn != NULL )
    {
        GRAPHICS_Translate(*posn);
    }

    if( width == 0.0 )
    {
        glBegin(GL_TRIANGLES);
    }
    else
    {
        glLineWidth(width);
        glBegin(GL_LINE_STRIP);
    }

    GRAPHICS_ColorSet(color,alpha);

    for( i=1; (i <= GRAPHICS_SphereSegments); i++ )
    {
        glVertex3d(V(i,1),V(i,2),z);

        if( width == 0.0 )
        {
            j = i+1;
            if( j > GRAPHICS_SphereSegments )
            {
                j = 1;
            }

            glVertex3d(V(j,1),V(j,2),z);
            glVertex3d(0,0,z);
        }
    }

    // Link back to first vertex to complete ring.
    if( width > 0.0 )
    {
        glVertex3d(V(1,1),V(1,2),z);
    }

    glEnd();

    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_Circle( matrix *posn, float radius, int color, float alpha )
{
float width=0.0; // Solid circle.

    GRAPHICS_Circle(posn,radius,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Circle( matrix *posn, float radius, int color  )
{
float alpha=1.0;

    GRAPHICS_Circle(posn,radius,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Circle( matrix *posn, float radius  )
{
int color=NOCOLOR;

    GRAPHICS_Circle(posn,radius,color);
}

/******************************************************************************/

void GRAPHICS_Ring( matrix *posn, float radius, float width, int color, float alpha )
{
    GRAPHICS_Circle(posn,radius,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Ring( matrix *posn, float radius, float width, int color )
{
float alpha=1.0;

    GRAPHICS_Ring(posn,radius,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Ring( matrix *posn, float radius, float width )
{
int color=NOCOLOR;

    GRAPHICS_Ring(posn,radius,width,color);
}

/******************************************************************************/

void GRAPHICS_Sector( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha )
{
static float z=0.0;
float astep,a;
int i,j,segments;
static matrix V;

    segments = 1 + (int)((double)GRAPHICS_SphereSegments * (arcrad/(2.0*PI)));
    astep = arcrad/(double)segments;

    segments++;
    V.dim(segments,2);
 
    for( a=(rotrad-(arcrad/2.0)),i=1; (i <= segments); i++ )
    {
        V(i,1) = radius * sin(a);
        V(i,2) = radius * cos(a);
        a += astep;
    }

    glPushMatrix();

    if( posn != NULL )
    {
        GRAPHICS_Translate(*posn);
    }

    if( width == 0.0 )
    {
        glBegin(GL_TRIANGLES);
    }
    else
    {
        glLineWidth(width);
        glBegin(GL_LINE_STRIP);
    }

    GRAPHICS_ColorSet(color,alpha);

    for( i=1; (i < segments); i++ )
    {
        glVertex3d(V(i,1),V(i,2),z);

        if( width == 0.0 )
        {
            j = i+1;
            glVertex3d(V(j,1),V(j,2),z);
            glVertex3d(0,0,z);
        }
    }

    // Set final vertex in line strip.
    if( width > 0.0 )
    {
        glVertex3d(V(i,1),V(i,2),z);
    }

    glEnd();

    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_Sector( matrix *posn, float radius, float arcrad, float rotrad, int color, float alpha )
{
float width=0.0; // Solid sector.

    GRAPHICS_Sector(posn,radius,arcrad,rotrad,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Sector( matrix *posn, float radius, float arcrad, float rotrad, int color )
{
float alpha=1.0;

    GRAPHICS_Sector(posn,radius,arcrad,rotrad,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Sector( matrix *posn, float radius, float arcrad, float rotrad )
{
int color=NOCOLOR;

    GRAPHICS_Sector(posn,radius,arcrad,rotrad,color);
}

/******************************************************************************/

void GRAPHICS_Arc( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha )
{
    GRAPHICS_Sector(posn,radius,arcrad,rotrad,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Arc( matrix *posn, float radius, float arcrad, float rotrad, float width, int color )
{
float alpha=1.0;

    GRAPHICS_Arc(posn,radius,arcrad,rotrad,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Arc( matrix *posn, float radius, float arcrad, float rotrad, float width )
{
int color=NOCOLOR;

    GRAPHICS_Arc(posn,radius,arcrad,rotrad,width,color);
}

/******************************************************************************/

void GRAPHICS_ArcAt( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha )
{
static matrix P(3,1);

    P.zeros();

    if( posn != NULL )
    {
        P = *posn;
    }

    P(1,1) -= radius * sin(rotrad);
    P(2,1) -= radius * cos(rotrad);

    GRAPHICS_Arc(&P,radius,arcrad,rotrad,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_ArcAt( matrix *posn, float radius, float arcrad, float rotrad, float width, int color )
{
float alpha=1.0;

    GRAPHICS_ArcAt(posn,radius,arcrad,rotrad,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_ArcAt( matrix *posn, float radius, float arcrad, float rotrad, float width )
{
int color=NOCOLOR;

    GRAPHICS_ArcAt(posn,radius,arcrad,rotrad,width,color);
}

/******************************************************************************/

void GRAPHICS_ArcPolygon( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha )
{
static float z=0.0;
float astep,a,r1,r2;
int i,j,segments;
static matrix V;

    r1 = radius-(width/2.0); // Inner radius
    r2 = radius+(width/2.0); // Outer radius

    segments = 1 + (int)((double)GRAPHICS_SphereSegments * (arcrad/(2.0*PI)));
    astep = arcrad/(double)segments;

    V.dim((4*segments),2);
 
    j = 0;

    // Vertices associated with inner edge of arc.
    for( i=1; (i <= segments); i++ )
    {
        a = (rotrad-(arcrad/2.0)) + (astep*(double)(i-1));

        j++;
        V(j,1) = r1 * sin(a);
        V(j,2) = r1 * cos(a);

        a += astep;

        j++;
        V(j,1) = r1 * sin(a);
        V(j,2) = r1 * cos(a);

        j++;
        V(j,1) = r2 * sin(a);
        V(j,2) = r2 * cos(a);

        a -= astep;

        j++;
        V(j,1) = r2 * sin(a);
        V(j,2) = r2 * cos(a);
    }

    glPushMatrix();

    if( posn != NULL )
    {
        GRAPHICS_Translate(*posn);
    }

    glLineWidth(1.0);
    glBegin(GL_QUADS);

    GRAPHICS_ColorSet(color,alpha);

    for( i=1; (i <= j); i++ )
    {
        glVertex3d(V(i,1),V(i,2),z);
    }

    glEnd();

    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_ArcPolygon( matrix *posn, float radius, float arcrad, float rotrad, float width, int color )
{
float alpha=1.0;

    GRAPHICS_ArcPolygon(posn,radius,arcrad,rotrad,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_ArcPolygon( matrix *posn, float radius, float arcrad, float rotrad, float width )
{
int color=NOCOLOR;

    GRAPHICS_ArcPolygon(posn,radius,arcrad,rotrad,width,color);
}

/******************************************************************************/

void GRAPHICS_ArcPolygonAt( matrix *posn, float radius, float arcrad, float rotrad, float width, int color, float alpha )
{
static matrix P(3,1);

    P.zeros();

    if( posn != NULL )
    {
        P = *posn;
    }

    P(1,1) -= radius * sin(rotrad);
    P(2,1) -= radius * cos(rotrad);

    GRAPHICS_ArcPolygon(&P,radius,arcrad,rotrad,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_ArcPolygonAt( matrix *posn, float radius, float arcrad, float rotrad, float width, int color )
{
float alpha=1.0;

    GRAPHICS_ArcPolygonAt(posn,radius,arcrad,rotrad,width,color,alpha);
}

/******************************************************************************/

void GRAPHICS_ArcPolygonAt( matrix *posn, float radius, float arcrad, float rotrad, float width )
{
int color=NOCOLOR;

    GRAPHICS_ArcPolygonAt(posn,radius,arcrad,rotrad,width,color);
}

/******************************************************************************/

void GRAPHICS_Rectangle( matrix *posn, float xwidth, float yheight, int color, float alpha )
{
static matrix V(4,2);
static int sides=4;
static float xsign[4] = { -1.0,-1.0,+1.0,+1.0 };
static float ysign[4] = { -1.0,+1.0,+1.0,-1.0 };
static float z=0.0;
int i,j;

    for( i=0; (i < sides); i++ )
    {
        V(i+1,1) = xsign[i]*xwidth/2.0;
        V(i+1,2) = ysign[i]*yheight/2.0;
    }

    glPushMatrix();

    if( posn != NULL )
    {
        GRAPHICS_Translate(*posn);
    }

    glBegin(GL_TRIANGLES);

    GRAPHICS_ColorSet(color,alpha);

    for( i=1; (i <= sides); i++ )
    {
        j = i+1;
        if( j > sides )
        {
            j = 1;
        }

        glVertex3d(V(i,1),V(i,2),z);
        glVertex3d(V(j,1),V(j,2),z);
        glVertex3d(0,0,z);
    }

    glEnd();

    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_Rectangle( matrix *posn, float xwidth, float yheight, int color )
{
float alpha=1.0;

    GRAPHICS_Rectangle(posn,xwidth,yheight,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Rectangle( matrix *posn, float xwidth, float yheight )
{
int color=NOCOLOR;

    GRAPHICS_Rectangle(posn,xwidth,yheight,color);
}

/******************************************************************************/

void GRAPHICS_Square( matrix *posn, float length, int color, float alpha )
{
    GRAPHICS_Rectangle(posn,length,length,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Square( matrix *posn, float length, int color )
{
    GRAPHICS_Rectangle(posn,length,length,color);
}

/******************************************************************************/

void GRAPHICS_Square( matrix *posn, float length )
{
    GRAPHICS_Rectangle(posn,length,length);
}

/******************************************************************************/

void GRAPHICS_Cuboid( matrix *posn, float xlength, float ywidth, float zheight, int color, float alpha )
{
float xscale,zscale;

    glPushMatrix();

    if( posn != NULL )
    {
        GRAPHICS_Translate(*posn);
    }

    xscale = ywidth / xlength;
    zscale = ywidth / zheight;
    glScalef(xscale,1.0,zscale);

    if( color != NOCOLOR )
    {
        GRAPHICS_ColorSet(color,alpha);
    }

    glutSolidCube(ywidth);

    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_Cuboid( matrix *posn, float xlength, float ywidth, float zheight, int color )
{
float alpha=1.0;

    GRAPHICS_Cuboid(posn,xlength,ywidth,zheight,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Cuboid( matrix *posn, float xlength, float ywidth, float zheight )
{
int color=NOCOLOR;

    GRAPHICS_Cuboid(posn,xlength,ywidth,zheight,color);
}

/******************************************************************************/

void GRAPHICS_Cube( matrix *posn, float length, int color, float alpha )
{
    GRAPHICS_Cuboid(posn,length,length,length,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Cube( matrix *posn, float length, int color )
{
    GRAPHICS_Cuboid(posn,length,length,length,color);
}

/******************************************************************************/

void GRAPHICS_Cube( matrix *posn, float length )
{
    GRAPHICS_Cuboid(posn,length,length,length);
}

/******************************************************************************/

void GRAPHICS_Triangle( matrix *posn, double Zangle, double sides, double base, int color, float alpha )
{
double x=0.0,y=0.0,z=0.0;
double a,o,h,s;

    glPushMatrix();

    if( posn != NULL )
    {
        GRAPHICS_Translate(*posn);
    }

    glRotated(Zangle,0.0,0.0,1.0);

    glBegin(GL_TRIANGLES);

    GRAPHICS_ColorSet(color,alpha);

    // Right-angle triangle.
    h = sides;                 // Hypotenuse
    o = base/2.0;              // Opposite
    a = sqrt((h*h)-(o*o));     // Adjacent

    glVertex3d(x,y+(a/2),z); 	// Top vertex of triangle
    glVertex3d(x-o,y-(a/2),z);	// Left vertex
    glVertex3d(x+o,y-(a/2),z);  // Right vertix

    glEnd();

    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_Triangle( matrix *posn, double Zangle, double sides, double base, int color )
{
float alpha=1.0;

    GRAPHICS_Triangle(posn,Zangle,sides,base,color,alpha);
}

/******************************************************************************/

void GRAPHICS_Triangle( matrix *posn, double Zangle, double sides, double base )
{
int color=NOCOLOR;

    GRAPHICS_Triangle(posn,Zangle,sides,base,color);
}

/******************************************************************************/

void GRAPHICS_Line( matrix &s, matrix &e, double width, int color )
{
    glPushMatrix();
    glLineWidth(width);
    glBegin(GL_LINES);
    GRAPHICS_ColorSet(color);

    glVertex3d(s(1,1),s(2,1),s(3,1));
    glVertex3d(e(1,1),e(2,1),e(3,1));

    glEnd();
    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_Line( matrix &s, matrix &e, double width )
{
int color=NOCOLOR;

    glPushMatrix();
    glLineWidth(width);
    glBegin(GL_LINES);
    GRAPHICS_ColorSet(color);

    glVertex3d(s(1,1),s(2,1),s(3,1));
    glVertex3d(e(1,1),e(2,1),e(3,1));

    glEnd();
    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_FixationCross( matrix &posn, float length, float width, int color )
{
static matrix s(3,1),e(3,1);
int i;

    glPushMatrix();

    for( i=1; (i <= 2); i++ )
    {
        s = posn;
        s(i,1) -= (length/2.0);

        e = posn;
        e(i,1) += (length/2.0);

        GRAPHICS_Line(s,e,width,color);
    }

    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_MassOnAStick( matrix &HandlePosition, double HandleAngle, int ObjectColor, double HandleRadius, double RodLength, double RodWidth, int HeadSize )
{
static matrix V(3,1);
static matrix P(3,1);
static matrix E(3,1);
static matrix R(3,3);
static double a1,a2;

    glPushMatrix();

    SPMX_romxZ(D2R(-HandleAngle),R);
    V.zeros();
    V(2,1) = RodLength;
    E = HandlePosition + (R * V);

    // Draw the object stick.
    glPushMatrix();
    set_color(ObjectColor);
    glLineWidth(RodWidth);
    glBegin(GL_LINES);    
    P = HandlePosition;
    P(3,1) += HandleRadius;
    glVertex3f(P(1,1),P(2,1),P(3,1));
    P = E;
    P(3,1) += HandleRadius;
    glVertex3f(P(1,1),P(2,1),P(3,1));
    glEnd(); 
    glPopMatrix();

    // Draw the circular handle at the base of the stick.
    P = HandlePosition;
    P(3,1) += HandleRadius;
    GRAPHICS_Circle(&P,HandleRadius,ObjectColor);

    // Draw the square mass at the end of the stick.
    P = E;
    P(3,1) += HandleRadius;
    glPushMatrix();
    GRAPHICS_Translate(P);
    glRotated(-HandleAngle,GL_ROTATE_Z);
    GRAPHICS_Translate(-P);
    GRAPHICS_Square(&P,HeadSize,ObjectColor);
    glPopMatrix();

    glPopMatrix();
}

/******************************************************************************/

double GRAPHICS_VerticalRetraceOnsetTimeSinceLast( void )
{
double t=0.0;

    if( GRAPHICS_VerticalRetraceOnsetSyncFlag )
    {
        t = fmod(GRAPHICS_VerticalRetraceOnsetTimer.ElapsedSeconds(),GRAPHICS_VerticalRetracePeriod);
    }

    return(seconds2milliseconds(t));
}

/******************************************************************************/

double GRAPHICS_VerticalRetraceOffsetTimeSinceLast( void )
{
double t=0.0;

    if( GRAPHICS_VerticalRetraceOffsetSyncFlag )
    {
        t = fmod(GRAPHICS_VerticalRetraceOffsetTimer.ElapsedSeconds(),GRAPHICS_VerticalRetracePeriod);
    }

    return(seconds2milliseconds(t));
}

/******************************************************************************/

double GRAPHICS_VerticalRetraceOnsetTimeUntilNext( void )
{
double t=0.0;

    if( GRAPHICS_VerticalRetraceOnsetSyncFlag )
    {
        t = seconds2milliseconds(GRAPHICS_VerticalRetracePeriod) - GRAPHICS_VerticalRetraceOnsetTimeSinceLast();
    }

    return(t);
}

/******************************************************************************/

double GRAPHICS_VerticalRetraceOffsetTimeUntilNext( void )
{
double t=0.0;

    if( GRAPHICS_VerticalRetraceOffsetSyncFlag )
    {
        t = seconds2milliseconds(GRAPHICS_VerticalRetracePeriod) - GRAPHICS_VerticalRetraceOffsetTimeSinceLast();
    }

    return(t);
}

/******************************************************************************/

void GRAPHICS_VerticalRetraceCatchOnset( void )
{
double t0,t1;

    t0 = GRAPHICS_VerticalRetraceOnsetTimeUntilNext();
    t1 = GRAPHICS_VerticalRetraceOnsetTimeSinceLast();
    if( t1 < t0 )
    {
        t0 = t1;
    }

    GRAPHICS_VerticalRetraceOnsetTimer.Reset();
    GRAPHICS_VerticalRetraceOnsetTimeData.Data(GRAPHICS_VerticalRetraceTimer.ElapsedSeconds());

    if( GRAPHICS_VerticalRetraceOffsetSyncFlag )
    {
        GRAPHICS_VerticalRetraceOnsetErrorData.Data(t0);
    }

    GRAPHICS_VerticalRetraceOnsetSyncFlag = TRUE;
}

/******************************************************************************/

void GRAPHICS_VerticalRetraceCatchOffset( void )
{
double t0,t1;

    t0 = GRAPHICS_VerticalRetraceOffsetTimeUntilNext();
    t1 = GRAPHICS_VerticalRetraceOffsetTimeSinceLast();

    if( t1 < t0 )
    {
        t0 = t1;
    }

    GRAPHICS_VerticalRetraceOffsetTimer.Reset();
    GRAPHICS_VerticalRetraceOffsetTimeData.Data(GRAPHICS_VerticalRetraceTimer.ElapsedSeconds());

    if( GRAPHICS_VerticalRetraceOffsetSyncFlag )
    {
        GRAPHICS_VerticalRetraceOffsetErrorData.Data(t0);
    }

    GRAPHICS_VerticalRetraceOffsetSyncFlag = TRUE;
}

/******************************************************************************/

void GRAPHICS_VerticalRetraceCatch( double wait_msec )
{
BOOL done=FALSE;

    GRAPHICS_VerticalRetraceCatchTimer.Reset();

    while( !done )
    {
        if( VGA_VerticalRetraceOnset() )
        {
            GRAPHICS_VerticalRetraceCatchOnset();
        }

        if( VGA_VerticalRetraceOffset() )
        {
            GRAPHICS_VerticalRetraceCatchOffset();
        }

        if( wait_msec == 0.0 )
        {
            done = GRAPHICS_VerticalRetraceOnsetSyncFlag && GRAPHICS_VerticalRetraceOffsetSyncFlag;
        }
        else
        {
            done = GRAPHICS_VerticalRetraceCatchTimer.Expired(wait_msec);
        }
    }
}

/******************************************************************************/

void GRAPHICS_VerticalRetraceCatch( void )
{
    GRAPHICS_VerticalRetraceCatch(GRAPHICS_VerticalRetraceCatchTime);
}

/******************************************************************************/

void GRAPHICS_VerticalRetraceResults( void )
{
int n,e,i,c;
double t,p,jm,js;
BOOL ok;

    printf("GRAPHICS_VerticalRetrace...\n");

    n = GRAPHICS_VerticalRetraceOnsetTimeData.Count();
    t = GRAPHICS_VerticalRetraceOnsetTimeData.Range();
    e = (int)(t / GRAPHICS_VerticalRetracePeriod);
    p = 100.0 * (double)n / (double)e;
    jm = GRAPHICS_VerticalRetraceOnsetErrorData.Mean();
    js = GRAPHICS_VerticalRetraceOnsetErrorData.Mean();
    printf("Onset  %d (%.0lf%%) caught in %.1lf sec. Jitter=%.3lf (SD=%.3lf) msec.\n",n,p,t,jm,js);

    n = GRAPHICS_VerticalRetraceOffsetTimeData.Count();
    t = GRAPHICS_VerticalRetraceOffsetTimeData.Range();
    e = (int)(t / GRAPHICS_VerticalRetracePeriod);
    p = 100.0 * (double)n / (double)e;
    jm = GRAPHICS_VerticalRetraceOffsetErrorData.Mean();
    js = GRAPHICS_VerticalRetraceOffsetErrorData.SD();
    printf("Offset %d (%.0lf%%) caught in %.1lf sec. Jitter=%.3lf (SD=%.3lf) msec.\n",n,p,t,jm,js);

    n = GRAPHICS_SwapBuffersToVerticalRetraceTimeData.Count();
    if( n > 0 )
    {
        printf("GRAPHICS_SwapBuffersToVerticalRetraceTime...\n");
        t = seconds2milliseconds(GRAPHICS_VerticalRetracePeriod/2);
        for( i=0,c=0; (i < n); i++ )
        {
            if( GRAPHICS_SwapBuffersToVerticalRetraceTimeData.ValueD(i) > t )
            {
                c++;
            }
        }
        p = 100.0 * (double)c / (double)n;

        printf("Mean=%.3lf msec SD=%.3lf Min=%.3lf Max=%.3lf n=%d (%.1lf%% > %.0lf msec)\n",GRAPHICS_SwapBuffersToVerticalRetraceTimeData.Mean(),GRAPHICS_SwapBuffersToVerticalRetraceTimeData.SD(),GRAPHICS_SwapBuffersToVerticalRetraceTimeData.Min(),GRAPHICS_SwapBuffersToVerticalRetraceTimeData.Max(),n,p,t);

        //ok = GRAPHICS_SwapBuffersToVerticalRetraceTimeData.Save();
        //printf("SAVE: %s %s\n",GRAPHICS_SwapBuffersToVerticalRetraceTimeData.File(),STR_OkFailed(ok));
    }
}

/******************************************************************************/

double GRAPHICS_DisplayDelayTarget( matrix &posn )
{
static matrix R(3,3),P(3,1),D(3,1);
double xlen,ylen,x,y,t,d;
double ScreenTotalDistance,SecondsPerUnitDistance;

    if( !GRAPHICS_Screen(xlen,ylen) )
    {
        return(0.0);
    }

    D.zeros();
    D(1,1) = xlen;
    D(2,1) = ylen;

    // Take rotation of screen into account.
    R = SPMX_romxX(D2R(GRAPHICS_DisplayRotation[GRAPHICS_X])) * SPMX_romxY(D2R(GRAPHICS_DisplayRotation[GRAPHICS_Y])) * SPMX_romxZ(D2R(GRAPHICS_DisplayRotation[GRAPHICS_Z]));
    D = R * D;
    P = R * posn;

    xlen = fabs(D(1,1));
    ylen = fabs(D(2,1));

    ScreenTotalDistance = xlen * (1+ylen);
    SecondsPerUnitDistance = GRAPHICS_VerticalRetracePeriod / ScreenTotalDistance;

    x = (xlen/2.0)+P(1,1);
    y = (ylen/2.0)-P(2,1);

    d = (y*xlen)+x;
    t = GRAPHICS_DisplayDelayMinimum + (d * SecondsPerUnitDistance);

    return(t);
}

/******************************************************************************/

void GRAPHICS_FrameData( void )
{
    GRAPHICS_SwapBuffersLastOnsetTime = GRAPHICS_SwapBuffersOnsetTimer.ElapsedSeconds();
    GRAPHICS_SwapBuffersLastOffsetTime = GRAPHICS_SwapBuffersOffsetTimer.ElapsedSeconds();
    GRAPHICS_VerticalRetraceNextOnsetTime = milliseconds2seconds(GRAPHICS_VerticalRetraceOnsetTimeUntilNext());
    GRAPHICS_VerticalRetraceNextOffsetTime = milliseconds2seconds(GRAPHICS_VerticalRetraceOffsetTimeUntilNext());
}

/******************************************************************************/

void GRAPHICS_FrameData( MATDAT *data )
{
    data->AddVariable("GraphicsSwapBuffersCount",GRAPHICS_SwapBuffersCount);
    data->AddVariable("GraphicsSwapBuffersToVerticalRetraceTime",GRAPHICS_SwapBuffersToVerticalRetraceTime);
    data->AddVariable("GraphicsVerticalRetraceNextOnsetTime",GRAPHICS_VerticalRetraceNextOnsetTime);
    data->AddVariable("GraphicsVerticalRetraceNextOffsetTime",GRAPHICS_VerticalRetraceNextOffsetTime);
}

/******************************************************************************/

int   GRAPHICS_DisplayState;
int   GRAPHICS_DisplayStateLast;
TIMER GRAPHICS_DisplayStateTimer("GRAPHICS_DisplayStateTimer");

/******************************************************************************/

void GRAPHICS_DisplayStateNext( int state )
{
    if( GRAPHICS_DisplayState == state )
    {
        return;
    }

    GRAPHICS_DisplayStateTimer.Reset();
    GRAPHICS_DisplayStateLast = GRAPHICS_DisplayState;
    GRAPHICS_DisplayState = state;
}

/******************************************************************************/

void GRAPHICS_DisplayStateProcess( double SecondsPreVR, double SecondsCatchVR, void (*DisplayPreVR)( void ), void (*DisplayPostVR)( void ) )
{
    if( SecondsPreVR == 0.0 )
    {
        if( DisplayPostVR != NULL ) (*DisplayPostVR)();
        if( DisplayPreVR != NULL )  (*DisplayPreVR)();
        return;
    }

    switch( GRAPHICS_DisplayState )
    {
        case GRAPHICS_DISPLAY_INITIALIZE :
            GRAPHICS_VerticalRetraceCatch(0.0);
            GRAPHICS_DisplayStateNext(GRAPHICS_DISPLAY_PRE_VR);
            break;

        case GRAPHICS_DISPLAY_PRE_VR :
            if( GRAPHICS_VerticalRetraceOnsetTimeUntilNext() > (SecondsPreVR*1000.0) )
            {
                // Wait until time to next VR is within pre-display window.
                break;
            }

            (*DisplayPreVR)(); // Pre-VR Display function
            GRAPHICS_DisplayStateNext(GRAPHICS_DISPLAY_WAIT_VR);
            break;

        case GRAPHICS_DISPLAY_WAIT_VR :
            if( GRAPHICS_VerticalRetraceOnsetTimeUntilNext() < (SecondsPreVR*1000.0) )
            {
                // Wait until time to next VR has reset to large value (VR has occurred).
                break;
            }

            GRAPHICS_DisplayStateNext(GRAPHICS_DISPLAY_POST_VR);
            break;

        case GRAPHICS_DISPLAY_POST_VR :
            if( DisplayPostVR != NULL )
            {
                (*DisplayPostVR)(); // Post-VR Display function
            }

            GRAPHICS_DisplayStateNext(GRAPHICS_DISPLAY_PRE_VR);
            break;
    }

    // This function catches the vertical retrace, resetting a timer.
    GRAPHICS_VerticalRetraceCatch(SecondsCatchVR);

    // Give up remainder of this time-slice.
    Sleep(0);
}

/******************************************************************************/

void GRAPHICS_DisplayStateProcess( double SecondsPreVR, double SecondsCatchVR, void (*DisplayPreVR)( void ) )
{
    GRAPHICS_DisplayStateProcess(SecondsPreVR,SecondsCatchVR,DisplayPreVR,NULL);
}

/******************************************************************************/

BOOL GRAPHICS_GraphicsStart( int mode, BYTE flag, int clearcolor )
{
BOOL ok=TRUE;

    if( (ok=GRAPHICS_Start(mode)) )
    {
        GRAPHICS_OpenGL(flag,clearcolor);
    }

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_GraphicsStart( int mode, int clearcolor )
{
BOOL ok=TRUE;
BYTE flag=GRAPHICS_FLAG_LIGHTING;

    ok = GRAPHICS_GraphicsStart(mode,flag,clearcolor);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_GraphicsStart( int mode )
{
BOOL ok=TRUE;
int clearcolor=NOCOLOR;

    ok = GRAPHICS_GraphicsStart(mode,clearcolor);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_GraphicsStart( void  )
{
BOOL ok=TRUE;
int mode=GRAPHICS_DISPLAY_DEFAULT;

    ok = GRAPHICS_GraphicsStart(mode);

    return(ok);
}

/******************************************************************************/

void GRAPHICS_OculusOpenGL( int clearcolor )
{
int i;

float LightPosition[][4] = 
{
    { -8.0,2.0,10.0,1.0 },
    { 0.0,15.0,0.0,1.0 }
};

float LightColor[][4] = 
{
    { 0.8,0.8,0.8,1.0 },
    { 0.4,0.3,0.3,1.0 }
};

    for( i=0; (i < 2); i++ )
    {
        glLightfv(GL_LIGHT0+i,GL_POSITION,LightPosition[i]);
        glLightfv(GL_LIGHT0+i,GL_DIFFUSE,LightColor[i]);
    }

    GRAPHICS_FlagOpenGL = GRAPHICS_FLAG_LIGHTING;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
//  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
//  glShadeModel(GL_FLAT);

    // Default clear color.
    if( clearcolor != NOCOLOR )
    {
        GRAPHICS_ClearColorDefault = clearcolor;
    }

    GRAPHICS_ClearColor();
}

/******************************************************************************/

BOOL GRAPHICS_OculusStart( char *name )
{
int glut=GLUT_RGB | GLUT_DEPTH | GLUT_SINGLE;

    if( !OCULUS_Open() )
    {
        return(FALSE);
    }

    GRAPHICS_GlutInit(glut,0,0);
    GRAPHICS_WindowID = glutCreateWindow(name);
    glutFullScreen();

    if( !OCULUS_Start() )
    {
        return(FALSE);
    }

    return(TRUE);
}

/******************************************************************************/

void GRAPHICS_OculusStop( void )
{
    OCULUS_Close();
}

/******************************************************************************/

void GRAPHICS_GraphicsDisplay( void (*draw)( void ) )
{
    // Mark time before we start drawing the graphics scene.
    GRAPHICS_DisplayLatencyTimer.Before();

    // Clear "stereo" graphics buffers.
    GRAPHICS_ClearStereoLatencyTimer.Before();
    GRAPHICS_ClearStereo();
    GRAPHICS_ClearStereoLatency = GRAPHICS_ClearStereoLatencyTimer.After();

    // Loop for each eye (stereo 3D).
    GRAPHICS_EyeLoop(eye)
    {
        // Set view for each eye (stereo 3D).
        GRAPHICS_ViewCalib(eye);

        // Clear "mono" graphics buffers.
        GRAPHICS_ClearMonoLatencyTimer.Before();
        GRAPHICS_ClearMono();
        GRAPHICS_ClearMonoLatency = GRAPHICS_ClearMonoLatencyTimer.After();

        // Display any graphics text.
        GRAPHICS_TextDraw(&GRAPHICS_TextLatencyTimer);

        GRAPHICS_DrawLatencyTimer.Before();

        if( draw != NULL )
        {
            (*draw)();
        }

        GRAPHICS_DrawLatency = GRAPHICS_DrawLatencyTimer.After();
    }

    // Mark time now that scene has been drawn.
    GRAPHICS_DisplayLatency = GRAPHICS_DisplayLatencyTimer.After();

    // Display the graphics buffer we've just drawn.
    GRAPHICS_SwapBufferLatencyTimer.Before();
    GRAPHICS_SwapBuffers();
    GRAPHICS_SwapBufferLatency = GRAPHICS_SwapBufferLatencyTimer.After();
}

/******************************************************************************/

void GRAPHICS_SetWindow( void )
{
    glutSetWindow(GRAPHICS_WindowID);
}

/******************************************************************************/

void GRAPHICS_GraphicsDisplay( void )
{
    // Mark time before we start drawing the graphics scene.
    GRAPHICS_DisplayFrequencyPeriod = GRAPHICS_DisplayFrequencyTimer.Loop();

    GRAPHICS_SetWindow();

    if( GRAPHICS_StereoMode == GRAPHICS_STEREO_OCULUS )
    {
        OCULUS_GraphicsDisplay(GRAPHICS_DrawFunction);
    }
    else
    {
        GRAPHICS_GraphicsDisplay(GRAPHICS_DrawFunction);
    }
}

/******************************************************************************/

void GRAPHICS_GlutKeyboard( BYTE key, int x, int y )
{
    GRAPHICS_GlutKeyboardFrequencyTimer.Loop();

    // Pass event onto OCULUS keyboard function if required...
    if( GRAPHICS_StereoMode == GRAPHICS_STEREO_OCULUS )
    {
        if( OCULUS_GlutKeyboard(key,x,y) )
        {
            return;
        }
    }

    // Pass keyboard event on to application-define function...
    if( GRAPHICS_KeyboardFunction != NULL )
    {
      (*GRAPHICS_KeyboardFunction)(key,x,y);
    }
}

/******************************************************************************/

void GRAPHICS_GlutDisplay( void )
{
    GRAPHICS_GlutDisplayFrequencyTimer.Loop();
    GRAPHICS_GraphicsDisplay();
}

/******************************************************************************/

void GRAPHICS_GlutIdle( void )
{
    GRAPHICS_GlutIdleFrequencyTimer.Loop();

    // Is it time to call the graphics display function?
    if( GRAPHICS_AdaptiveDisplayReady() )
    {
        GRAPHICS_GraphicsDisplay();
    }

    // Check command-window keyboard events.
    KB_GLUT_Events();

    // Call application-defined idle function.
    if( GRAPHICS_IdleFunction != NULL )
    {
      (*GRAPHICS_IdleFunction)();
    }

    GRAPHICS_MonitorIdle();
}

/******************************************************************************/

void GRAPHICS_MainLoop( void (*KeyboardFunction)( BYTE key, int x, int y ), void (*DrawFunction)( void ), void (*IdleFunction)( void ) )
{
    GRAPHICS_KeyboardFunction = KeyboardFunction;
    GRAPHICS_DrawFunction = DrawFunction;
    GRAPHICS_IdleFunction = IdleFunction;

    glutKeyboardFunc(KB_GLUT_KeyboardFuncInstall(GRAPHICS_GlutKeyboard));
    glutDisplayFunc(GRAPHICS_GlutDisplay);
    glutIdleFunc(GRAPHICS_GlutIdle);

    glutMainLoop();
}

/******************************************************************************/

void GRAPHICS_glGetError( char *text, char *file, int line )
{
int rc;

    if( (rc=glGetError()) != GL_NO_ERROR )
    {
        GRAPHICS_errorf("%s glError=%d(0x%04X) file=%s, line=%d.\n",text,rc,rc,file,line);
        exit(0);
    }
}

/******************************************************************************/

void GRAPHICS_TimingResults( void )
{
    GRAPHICS_ClearStereoLatencyTimer.Results();
    GRAPHICS_ClearMonoLatencyTimer.Results();
    GRAPHICS_DisplayFrequencyTimer.Results();
    GRAPHICS_DisplayLatencyTimer.Results();
    GRAPHICS_TextLatencyTimer.Results();
    GRAPHICS_DrawLatencyTimer.Results();
    GRAPHICS_SwapBufferLatencyTimer.Results();
    GRAPHICS_GlutKeyboardFrequencyTimer.Results();
    GRAPHICS_GlutDisplayFrequencyTimer.Results();
    GRAPHICS_GlutIdleFrequencyTimer.Results();

    if( GRAPHICS_StereoMode == GRAPHICS_STEREO_OCULUS )
    {
        OCULUS_TimingResults();
    }

    GRAPHICS_MonitorTimingResults();
}

/******************************************************************************/

double GRAPHICS_AdaptiveDisplayWaitError;
double GRAPHICS_AdaptiveDisplayRetentionFactor = 0.99;
double GRAPHICS_AdaptiveDisplayLearningRate = 0.04;
double GRAPHICS_AdaptiveDisplayWaitTime;
TIMER  GRAPHICS_AdaptiveDisplayWaitTimer("GRAPHICS_AdaptiveDisplayWaitTimer");
double GRAPHICS_AdaptiveDisplayActualWaitTime;

DATAPROC GRAPHICS_SwapBufferLatencyData("GRAPHICS_SwapBufferLatency");
DATAPROC GRAPHICS_SwapBufferAbsoluteTimeData("GRAPHICS_SwapBufferAbsoluteTime");
DATAPROC GRAPHICS_AdaptiveDisplayWaitTimeData("GRAPHICS_AdaptiveDisplayWaitTime");
DATAPROC GRAPHICS_AdaptiveDisplayActualWaitTimeData("GRAPHICS_AdaptiveDisplayActualWaitTime");
DATAPROC GRAPHICS_AdaptiveDisplayWaitErrorData("GRAPHICS_AdaptiveDisplayWaitError");

/******************************************************************************/

void GRAPHICS_SwapBufferBefore( void )
{
    GRAPHICS_SwapBufferLatencyTimer.Before();
}

/******************************************************************************/

void GRAPHICS_SwapBufferAfter( void )
{
    GRAPHICS_SwapBufferLatency = milliseconds2seconds(GRAPHICS_SwapBufferLatencyTimer.After());
    GRAPHICS_SwapBufferAbsoluteTime = GRAPHICS_SwapBufferAbsoluteTimer.ElapsedSeconds();

    GRAPHICS_SwapBufferLatencyData.Data(GRAPHICS_SwapBufferLatency);
    GRAPHICS_SwapBufferAbsoluteTimeData.Data(GRAPHICS_SwapBufferAbsoluteTime);

    // The adaptive display algorithm tries to minimise the swap buffer latency.
    GRAPHICS_AdaptiveDisplayUpdate(GRAPHICS_SwapBufferLatency);
}

/******************************************************************************/

void GRAPHICS_AdaptiveDisplayUpdate( double SwapBufferLatency )
{
static BOOL first=TRUE;

    if( first )
    {
        // Initialize adpative display wait time to refresh period.
        GRAPHICS_AdaptiveDisplayWaitTime = GRAPHICS_VerticalRetracePeriod;
        first = FALSE;
    }

    /*GRAPHICS_AdaptiveDisplayWaitError = fmod(SwapBufferLatency,GRAPHICS_VerticalRetracePeriod);
    if( GRAPHICS_AdaptiveDisplayWaitError > (0.5*GRAPHICS_VerticalRetracePeriod) )
    {
        GRAPHICS_AdaptiveDisplayWaitError = GRAPHICS_AdaptiveDisplayWaitError - GRAPHICS_VerticalRetracePeriod;
    }*/

    GRAPHICS_AdaptiveDisplayWaitError = SwapBufferLatency;
    if( GRAPHICS_AdaptiveDisplayWaitError > GRAPHICS_VerticalRetracePeriod )
    {
        GRAPHICS_AdaptiveDisplayWaitError = GRAPHICS_VerticalRetracePeriod - GRAPHICS_AdaptiveDisplayWaitError;
    }

    GRAPHICS_AdaptiveDisplayWaitTime = (GRAPHICS_AdaptiveDisplayRetentionFactor * GRAPHICS_AdaptiveDisplayWaitTime) + (GRAPHICS_AdaptiveDisplayLearningRate * GRAPHICS_AdaptiveDisplayWaitError);

    if( GRAPHICS_AdaptiveDisplayWaitTime > GRAPHICS_VerticalRetracePeriod )
    {
        GRAPHICS_AdaptiveDisplayWaitTime -= GRAPHICS_VerticalRetracePeriod;
    }

    if( GRAPHICS_AdaptiveDisplayWaitTime < 0.0 )
    {
        GRAPHICS_AdaptiveDisplayWaitTime += GRAPHICS_VerticalRetracePeriod;
    }

    GRAPHICS_AdaptiveDisplayWaitTimeData.Data(GRAPHICS_AdaptiveDisplayWaitTime);
    GRAPHICS_AdaptiveDisplayWaitErrorData.Data(GRAPHICS_AdaptiveDisplayWaitError);
    GRAPHICS_AdaptiveDisplayWaitTimer.Reset();
}

/******************************************************************************/

BOOL GRAPHICS_AdaptiveDisplayReady( void )
{
BOOL flag=FALSE;

    if( GRAPHICS_AdaptiveDisplayWaitTimer.ExpiredSeconds(GRAPHICS_AdaptiveDisplayWaitTime) )
    {
        GRAPHICS_AdaptiveDisplayActualWaitTime = GRAPHICS_AdaptiveDisplayWaitTimer.ElapsedSeconds();
        GRAPHICS_AdaptiveDisplayActualWaitTimeData.Data(GRAPHICS_AdaptiveDisplayActualWaitTime);

        flag = TRUE;
    }

    return(flag);
}

/******************************************************************************/

void GRAPHICS_AdaptiveDisplayResults( BOOL saveflag )
{
BOOL ok;

    GRAPHICS_SwapBufferLatencyData.Results();
    GRAPHICS_AdaptiveDisplayWaitTimeData.Results();

    if( saveflag )
    {
        ok = TRUE;

        if( !GRAPHICS_SwapBufferLatencyData.Save() )
        {
            ok = FALSE;
        }

        if( !GRAPHICS_SwapBufferAbsoluteTimeData.Save() )
        {
            ok = FALSE;
        }

        if( !GRAPHICS_AdaptiveDisplayWaitTimeData.Save() )
        {
            ok = FALSE;
        }

        if( !GRAPHICS_AdaptiveDisplayActualWaitTimeData.Save() )
        {
            ok = FALSE;
        }

        if( !GRAPHICS_AdaptiveDisplayWaitErrorData.Save() )
        {
            ok = FALSE;
        }
    }
}

/******************************************************************************/

void GRAPHICS_AdaptiveDisplayResults( void )
{
BOOL saveflag=FALSE;

    GRAPHICS_AdaptiveDisplayResults(saveflag);
}

/******************************************************************************/

STRING GRAPHICS_MonitorConfigFile="MONITOR.CFG";
STRING GRAPHICS_MonitorName="Graphics Monitor Window";
int    GRAPHICS_MonitorWindowID=GRAPHICS_WINDOW_INVALID;
int    GRAPHICS_MonitorMainID=GRAPHICS_WINDOW_INVALID;
int    GRAPHICS_MonitorXY[2];
int    GRAPHICS_MonitorViewXY[2];
int    GRAPHICS_MonitorWindowW=0,GRAPHICS_MonitorWindowH=0;
int    GRAPHICS_MonitorW=0,GRAPHICS_MonitorH=0;
int    GRAPHICS_MonitorViews=4;
double GRAPHICS_MonitorFrequency=60.0;
double GRAPHICS_MonitorPeriod=0.0;
TIMER  GRAPHICS_MonitorTimer("GRAPHICS_MonitorTimer",TIMER_MODE_RESET);
double GRAPHICS_MonitorAspectRatio=0.0;

TIMER_Frequency GRAPHICS_MonitorDisplayFrequencyTimer("MonitorDisplayFrequency");
TIMER_Frequency GRAPHICS_MonitorIdleFrequencyTimer("MonitorIdleFrequency");
TIMER_Interval  GRAPHICS_MonitorDisplayLatencyTimer("MonitorDisplayLatency");
TIMER_Interval  GRAPHICS_MonitorTextLatencyTimer("MonitorTextLatency");
TIMER_Interval  GRAPHICS_MonitorDrawLatencyTimer("MonitorDrawLatency");
TIMER_Interval  GRAPHICS_MonitorSwapBufferLatencyTimer("MonitorSwapBufferLatency");
TIMER_Interval  GRAPHICS_MonitorClearStereoLatencyTimer("MonitorClearStereoLatency");
TIMER_Interval  GRAPHICS_MonitorClearMonoLatencyTimer("MonitorClearMonoLatency");

/******************************************************************************/

BOOL GRAPHICS_MonitorConfig( char *config )
{
char *file;

    CONFIG_reset();

    // Set up variable table for configuration...
    CONFIG_set("Description",GRAPHICS_MonitorName);
    CONFIG_set("Position",GRAPHICS_MonitorXY,2);
    CONFIG_set("Width",GRAPHICS_MonitorW);
    CONFIG_set("Frequency",GRAPHICS_MonitorFrequency);
    CONFIG_set("Views",GRAPHICS_MonitorViews);

    if( (file=FILE_Calibration(config)) == NULL )
    {
        GRAPHICS_errorf("GRAPHICS_MonitorConfig(%s) Cannot find file.\n",config);
        return(FALSE);
    }

    // Load configuration file...
    if( !CONFIG_read(file) )
    {
        GRAPHICS_errorf("GRAPHICS_MonitorConfig(%s) Cannot read file.\n",file);
        return(FALSE);
    }

    if( (GRAPHICS_MonitorViews != 1) && (GRAPHICS_MonitorViews != 4) )
    {
        GRAPHICS_errorf("GRAPHICS_MonitorConfig(%s) Invalid value (Views=%d).\n",file,GRAPHICS_MonitorViews);
        return(FALSE);
    }
  
    return(TRUE);
}

/******************************************************************************/

BOOL GRAPHICS_MonitorStarted( void )
{
BOOL flag;

    flag = GRAPHICS_Started() && (GRAPHICS_MonitorWindowID != GRAPHICS_WINDOW_INVALID);

    return(flag);
}

/******************************************************************************/

void GRAPHICS_MonitorTimingResults( void )
{
    if( !GRAPHICS_MonitorStarted() )
    {
        return;
    }

    GRAPHICS_MonitorDisplayFrequencyTimer.Results();
    GRAPHICS_MonitorDisplayLatencyTimer.Results();
    GRAPHICS_MonitorTextLatencyTimer.Results();
    GRAPHICS_MonitorDrawLatencyTimer.Results();
    GRAPHICS_MonitorSwapBufferLatencyTimer.Results();
    GRAPHICS_MonitorClearStereoLatencyTimer.Results();
    GRAPHICS_MonitorClearMonoLatencyTimer.Results();
    GRAPHICS_MonitorIdleFrequencyTimer.Results();
}

/******************************************************************************/

void GRAPHICS_MonitorSwapBuffers( void )
{
    if( GRAPHICS_DoubleBufferedFlag )
    {
        glutSwapBuffers();
    }
    else
    {
        glFlush();
    }
}

/******************************************************************************/

void GRAPHICS_MonitorDraw( void )
{
    if( GRAPHICS_DrawFunction != NULL )
    {
      (*GRAPHICS_DrawFunction)();
    }
}

/******************************************************************************/

void GRAPHICS_MonitorDisplay( void )
{
int view,i;
int x,y,w,h;
double sw=0.999,sh=0.999;
static matrix P1(3,1),P2(3,1);

    if( !GRAPHICS_MonitorStarted() )
    {
        return;
    }

    // Mark time for display frequency.
    GRAPHICS_MonitorDisplayFrequencyTimer.Loop();

    // Mark time before we start drawing the graphics scene.
    GRAPHICS_MonitorDisplayLatencyTimer.Before();

    // Make the monitor window active.
    GRAPHICS_MonitorSetWindow();

    // Clear "stereo" graphics buffers.
    GRAPHICS_MonitorClearStereoLatencyTimer.Before();
    GRAPHICS_ClearStereo();
    GRAPHICS_MonitorClearStereoLatencyTimer.After();

    // Clear "mono" graphics buffers.
    GRAPHICS_MonitorClearMonoLatencyTimer.Before();
    GRAPHICS_ClearMono();
    GRAPHICS_MonitorClearMonoLatencyTimer.After();

    GRAPHICS_LightingDisable();

    GRAPHICS_MonitorView();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);	
    glLoadIdentity();

    for( i=0; (i < 4); i++ )
    {
        P1.zeros();
        P2.zeros();

        switch( i )
        {
            case 0 :
                P1(1,1) = -sh;
                P1(2,1) = -sw;
                P2(1,1) = sh;
                P2(2,1) = -sw;
                break;

            case 1 :
                P1(1,1) = -sh;
                P1(2,1) = sw;
                P2(1,1) = sh;
                P2(2,1) = sw;
                break;

            case 2 :
                P1(1,1) = -sh;
                P1(2,1) = -sw;
                P2(1,1) = -sh;
                P2(2,1) = sw;
                break;

            case 3 :
                P1(1,1) = sh;
                P1(2,1) = -sw;
                P2(1,1) = sh;
                P2(2,1) = sw;
                break;
        }

        GRAPHICS_Line(P1,P2,1.0,GREY);
    }

    if( GRAPHICS_MonitorViews == 4 )
    {
        P1.zeros();
        P1(2,1) = -sw;

        P2.zeros();
        P2(2,1) = sw;

        GRAPHICS_Line(P1,P2,1.0,GREY);

        P1.zeros();
        P1(1,1) = -sh;

        P2.zeros();
        P2(1,1) = sh;

        GRAPHICS_Line(P1,P2,1.0,GREY);

    }

    GRAPHICS_LightingEnable();

    w = GRAPHICS_MonitorW/2;
    h = GRAPHICS_MonitorH/2;

    for( view=0; (view < GRAPHICS_MonitorViews); view++ )
    {
        if( view == 0 )
        {
            GRAPHICS_ViewCalib(EYE_MONO);
        }
        else
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0,GRAPHICS_MonitorAspectRatio,1,1000);
  
            glMatrixMode(GL_MODELVIEW);	
            glLoadIdentity();
        }

        switch( view )
        {
           case 0 :
              x = 0;
              y = h;
              break;

           case 1 :
              x = w;
              y = h;
              gluLookAt(0,-50,0,0,0,0,0,0,1);
              break;

           case 2 :
              x = 0;
              y = 0;
              gluLookAt(0,0,50,0,0,0,0,1,0);
              break;

           case 3 :
              x = w;
              y = 0;
              gluLookAt(50,0,0,0,0,0,0,0,1);
              break;
      
        }

        glViewport(GRAPHICS_MonitorViewXY[0]+x,GRAPHICS_MonitorViewXY[1]+y,w,h);

        if( view == 0 )
        {
            // Display any graphics text.
            GRAPHICS_TextDraw(&GRAPHICS_MonitorTextLatencyTimer);
        }

        // Application-defined scene draw function.
        GRAPHICS_MonitorDrawLatencyTimer.Before();
        GRAPHICS_MonitorDraw();
        GRAPHICS_MonitorDrawLatencyTimer.After();
    }

    // Display the graphics buffer we've just drawn.
    GRAPHICS_MonitorSwapBufferLatencyTimer.Before();
    GRAPHICS_MonitorSwapBuffers();
    GRAPHICS_MonitorSwapBufferLatencyTimer.After();

    // Make the main window active.
    GRAPHICS_SetWindow();

    // Mark time now that scene has been drawn.
    GRAPHICS_MonitorDisplayLatencyTimer.After();
}

/******************************************************************************/

void GRAPHICS_MonitorSize( int wid, int hgt )
{
int w,h;

    w = (int)(GRAPHICS_MonitorAspectRatio * (double)hgt);
    h = (int)((1.0/GRAPHICS_MonitorAspectRatio) * (double)wid);

    if( (w < wid) || (wid == 0) )
    {
        GRAPHICS_MonitorW = w;
        GRAPHICS_MonitorH = hgt;
    }

    if( (h < hgt) || (hgt == 0) )
    {
        GRAPHICS_MonitorW = wid;
        GRAPHICS_MonitorH = h;
    }

    w = (GRAPHICS_MonitorWindowW - GRAPHICS_MonitorW);
    GRAPHICS_MonitorViewXY[0] = w/2;

    h = (GRAPHICS_MonitorWindowH - GRAPHICS_MonitorH);
    GRAPHICS_MonitorViewXY[1] = h/2;
}

/******************************************************************************/

void GRAPHICS_MonitorView( void )
{
    glViewport(GRAPHICS_MonitorViewXY[0],GRAPHICS_MonitorViewXY[1],GRAPHICS_MonitorW,GRAPHICS_MonitorH);
}

/******************************************************************************/

void GRAPHICS_MonitorReshape( int wid, int hgt )
{
    GRAPHICS_MonitorWindowW = wid;
    GRAPHICS_MonitorWindowH = hgt;

    GRAPHICS_MonitorSize(wid,hgt);
    GRAPHICS_MonitorView();
}

/******************************************************************************/

BOOL GRAPHICS_MonitorStart( int xpos, int ypos, int wid, int views, double freq )
{
BOOL ok;

    GRAPHICS_MonitorXY[0] = xpos;
    GRAPHICS_MonitorXY[1] = ypos;
    GRAPHICS_MonitorW = wid;

    if( views != 0 )
    {
        GRAPHICS_MonitorViews = views;
    }

    if( freq != 0.0 )
    {
        GRAPHICS_MonitorFrequency = freq;
    }

    ok = GRAPHICS_MonitorStart(NULL);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_MonitorStart( int xpos, int ypos, int wid, double freq )
{
int views=0;
BOOL ok;

    ok = GRAPHICS_MonitorStart(xpos,ypos,wid,views,freq);

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_MonitorStart( char *config )
{
BOOL ok=TRUE;
double W,H;

    if( !GRAPHICS_Started() )
    {
        GRAPHICS_errorf("GRAPHICS_MonitorStart(...) Graphics not started.\n");
        return(FALSE);
    }

    if( GRAPHICS_MonitorStarted() )
    {
        return(TRUE);
    }

    if( config != NULL )
    {
        if( !GRAPHICS_MonitorConfig(config) )
        {
            return(FALSE);
        }
    }

    if( !GRAPHICS_Screen(W,H) )
    {
        GRAPHICS_errorf("GRAPHICS_MonitorStart(...) Cannot get main screen dimensions.\n");
        return(FALSE);
    }

    if( (W*H) == 0.0 )
    {
        GRAPHICS_errorf("GRAPHICS_MonitorStart(...) Invalid main screen dimensions (W=%.1lf,H=%.1lf).\n",W,H);
        return(FALSE);
    }

    if( GRAPHICS_MonitorFrequency == 0.0 )
    {
        GRAPHICS_MonitorFrequency = 60.0;
    }

    GRAPHICS_MonitorPeriod = 1.0/GRAPHICS_MonitorFrequency;

    GRAPHICS_MonitorAspectRatio = W/H;
    GRAPHICS_MonitorSize(GRAPHICS_MonitorW,0);

    GRAPHICS_MonitorViewXY[0] = 0;
    GRAPHICS_MonitorViewXY[1] = 0;

    // Save the window ID for the main window.
    GRAPHICS_MonitorMainID = glutGetWindow();
    GRAPHICS_messgf("GRAPHICS_MonitorMainID=%d\n",GRAPHICS_MonitorMainID);

    // Create the monitor window.
    GRAPHICS_MonitorWindowID = glutCreateWindow(GRAPHICS_MonitorName);
    GRAPHICS_messgf("GRAPHICS_MonitorWindowID=%d\n",GRAPHICS_MonitorWindowID);
    GRAPHICS_messgf("GRAPHICS_DoubleBufferedFlag=%s\n",STR_YesNo(GRAPHICS_DoubleBufferedFlag));
    GRAPHICS_messgf("GRAPHICS_MonitorAspectRatio=%.2lf\n",GRAPHICS_MonitorAspectRatio);

    // Set position and size of the monitor window.
    glutPositionWindow(GRAPHICS_MonitorXY[0],GRAPHICS_MonitorXY[1]);
    glutReshapeWindow(GRAPHICS_MonitorW,GRAPHICS_MonitorH);

    GRAPHICS_StandardOpenGL(GRAPHICS_FlagOpenGL,NOCOLOR);

    // If main window is on the Oculus HMD, set up frustum.
    if( GRAPHICS_StereoMode == GRAPHICS_STEREO_OCULUS )
    {
        GRAPHICS_FrustumInit();
        //ok = GRAPHICS_CalibUse();
    }

    // Set window-specific call-back functions.
    glutDisplayFunc(GRAPHICS_MonitorDisplay);
    glutReshapeFunc(GRAPHICS_MonitorReshape);
    glutKeyboardFunc(GRAPHICS_MonitorKeyboard);

    GRAPHICS_ClearColor();

    // Make the main window active.
    GRAPHICS_SetWindow();

    GRAPHICS_MonitorTimer.Reset();

    return(ok);
}

/******************************************************************************/

BOOL GRAPHICS_MonitorStart( void )
{
BOOL ok;

    ok = GRAPHICS_MonitorStart(GRAPHICS_MonitorConfigFile);

    return(ok);
}

/******************************************************************************/

void GRAPHICS_MonitorIdle( void )
{
    if( GRAPHICS_MonitorStarted() )
    {
        if( GRAPHICS_MonitorTimer.ExpiredSeconds(GRAPHICS_MonitorPeriod) )
        {
            GRAPHICS_MonitorDisplay();
        }
    }
}

/******************************************************************************/

void GRAPHICS_MonitorKeyboard( BYTE key, int x, int y )
{
    GRAPHICS_GlutKeyboard(key,x,y);
}

/******************************************************************************/

void GRAPHICS_MonitorWindowDestroy( void )
{
    if( GRAPHICS_MonitorWindowID != GRAPHICS_WINDOW_INVALID )
    {
        glutDestroyWindow(GRAPHICS_MonitorWindowID);
        GRAPHICS_MonitorWindowID = GRAPHICS_WINDOW_INVALID;
    }
}

/******************************************************************************/

void GRAPHICS_MonitorSetWindow( void )
{
    glutSetWindow(GRAPHICS_MonitorWindowID);
}

/******************************************************************************/

struct GRAPHICS_TextItem GRAPHICS_TextList[GRAPHICS_TEXT_LIST];

matrix GRAPHICS_TextDefaultPosition(3,1);
matrix GRAPHICS_TextDefaultRotation(3,1);
float  GRAPHICS_TextDefaultSize=1.0;
float  GRAPHICS_TextDefaultWidth=2.0;
float  GRAPHICS_TextDefaultColor=WHITE;
int    GRAPHICS_TextID=GRAPHICS_TEXT_INVALID;

/******************************************************************************/

void GRAPHICS_TextInit( void )
{
int item;

    for( item=0; (item < GRAPHICS_TEXT_LIST); item++ )
    {
        GRAPHICS_TextClear(item);
    }
}

/******************************************************************************/

int GRAPHICS_TextFind( char *text )
{
int item,i;

    for( item=GRAPHICS_TEXT_INVALID,i=0; (i < GRAPHICS_TEXT_LIST); i++ )
    {
        if( !GRAPHICS_TextList[i].inuse )
        {
            continue;
        }

        if( _stricmp(GRAPHICS_TextList[i].text,text) == 0 )
        {
            item = i;
            break;
        }
    }

    return(item);
}

/******************************************************************************/

BOOL GRAPHICS_TextValid( int item )
{
BOOL flag;

    flag = ((item >= 0) && (item < GRAPHICS_TEXT_LIST));

    return(flag);
}

/******************************************************************************/

BOOL GRAPHICS_TextInUse( void )
{
int item;
BOOL flag;

    for( flag=FALSE,item=0; (item < GRAPHICS_TEXT_LIST); item++ )
    {
        if( GRAPHICS_TextList[item].inuse )
        {
            flag = TRUE;
            break;
        }
    }

    return(flag);
}

/******************************************************************************/

void GRAPHICS_TextClear( int item )
{
    if( !GRAPHICS_TextValid(item) )
    {
        return;
    }

    GRAPHICS_TextList[item].inuse = FALSE;
    GRAPHICS_TextList[item].position.empty();
    GRAPHICS_TextList[item].rotation.empty();
    memset(GRAPHICS_TextList[item].text,0x00,STRLEN);
}

/******************************************************************************/

void GRAPHICS_TextClear( char *text )
{
int item;

    if( (item=GRAPHICS_TextFind(text)) != GRAPHICS_TEXT_INVALID )
    {
        GRAPHICS_TextClear(item);
    }
}

/******************************************************************************/

void GRAPHICS_TextClear( void )
{
int item;

    item = GRAPHICS_TextID;

    GRAPHICS_TextClear(item);
}

/******************************************************************************/

void GRAPHICS_TextPosition( matrix *position )
{
    if( position != NULL )
    {
        GRAPHICS_TextDefaultPosition = *position;
    }
}

/******************************************************************************/

void GRAPHICS_TextRotation( matrix *rotation )
{
    if( rotation != NULL )
    {
        GRAPHICS_TextDefaultRotation = *rotation;
    }
}

/******************************************************************************/

void GRAPHICS_TextColor( int color )
{
    if( color != NOCOLOR )
    {
        GRAPHICS_TextDefaultColor = color;
    }
}

/******************************************************************************/

void GRAPHICS_TextSize( float size )
{
    if( size > 0.0 )
    {
        GRAPHICS_TextDefaultSize = size;
    }
}

/******************************************************************************/

void GRAPHICS_TextWidth( float width )
{
    if( width > 0.0 )
    {
        GRAPHICS_TextDefaultWidth = width;
    }
}

/******************************************************************************/

void GRAPHICS_TextDefault( matrix *position, matrix *rotation, int color, float size, float width )
{
    GRAPHICS_TextPosition(position);
    GRAPHICS_TextRotation(rotation);
    GRAPHICS_TextColor(color);
    GRAPHICS_TextSize(size);
    GRAPHICS_TextWidth(width);
}

/******************************************************************************/

void GRAPHICS_TextDefault( matrix *position, int color, float size, float width )
{
    GRAPHICS_TextPosition(position);
    GRAPHICS_TextColor(color);
    GRAPHICS_TextSize(size);
    GRAPHICS_TextWidth(width);
}

/******************************************************************************/

void GRAPHICS_TextDefault( matrix *position, matrix *rotation, int color, float size )
{
    GRAPHICS_TextPosition(position);
    GRAPHICS_TextRotation(rotation);
    GRAPHICS_TextColor(color);
    GRAPHICS_TextSize(size);
}

/******************************************************************************/

void GRAPHICS_TextDefault( matrix *position, int color, float size )
{
    GRAPHICS_TextPosition(position);
    GRAPHICS_TextColor(color);
    GRAPHICS_TextSize(size);
}

/******************************************************************************/

void GRAPHICS_TextDefault( matrix *position, matrix *rotation, float size )
{
    GRAPHICS_TextPosition(position);
    GRAPHICS_TextRotation(rotation);
    GRAPHICS_TextSize(size);
}

/******************************************************************************/

void GRAPHICS_TextDefault( matrix *position, float size )
{
    GRAPHICS_TextPosition(position);
    GRAPHICS_TextSize(size);
}

/******************************************************************************/

void GRAPHICS_TextDefault( matrix *position, matrix *rotation )
{
    GRAPHICS_TextPosition(position);
    GRAPHICS_TextRotation(rotation);
}

/******************************************************************************/

void GRAPHICS_TextDefault( matrix *position )
{
    GRAPHICS_TextPosition(position);
}

/******************************************************************************/

void GRAPHICS_TextSet( int item, matrix *position, matrix *rotation, int color, float size, float width, char *text )
{
    GRAPHICS_TextClear(item);

    GRAPHICS_TextList[item].font = GLUT_STROKE_MONO_ROMAN;

    if( position != NULL )
    {
        GRAPHICS_TextList[item].position = *position;
    }

    if( rotation != NULL )
    {
        GRAPHICS_TextList[item].rotation = *rotation;
    }

    GRAPHICS_TextList[item].color = (color != NOCOLOR) ? color : GRAPHICS_TextDefaultColor;
    GRAPHICS_TextList[item].size = (size > 0.0) ? size : GRAPHICS_TextDefaultSize;
    GRAPHICS_TextList[item].width = (width > 0.0) ? width : GRAPHICS_TextDefaultWidth;
    strncpy(GRAPHICS_TextList[item].text,text,STRLEN);
    GRAPHICS_TextList[item].inuse = TRUE;

    GRAPHICS_TextID = item;
}

/******************************************************************************/

void GRAPHICS_TextSet( int item, matrix *position, int color, float size, float width, char *text )
{
matrix *rotation=NULL;

    GRAPHICS_TextSet(item,position,rotation,color,size,width,text);
}

/******************************************************************************/

void GRAPHICS_TextSet( int item, matrix *position, int color, float size, char *text )
{
float width=0.0;

    GRAPHICS_TextSet(item,position,color,size,width,text);
}

/******************************************************************************/

void GRAPHICS_TextSet( int item, matrix *position, int color, char *text )
{
float size=0.0;

    GRAPHICS_TextSet(item,position,color,size,text);
}

/******************************************************************************/

void GRAPHICS_TextSet( int item, int color, char *text )
{
matrix *position=NULL;

    GRAPHICS_TextSet(item,position,color,text);
}

/******************************************************************************/

void GRAPHICS_TextSet( int item, char *text )
{
int color=NOCOLOR;

    GRAPHICS_TextSet(item,color,text);
}

/******************************************************************************/

void GRAPHICS_TextSet( char *text )
{
int item=0;

    GRAPHICS_TextSet(item,text);
}

/******************************************************************************/

void GRAPHICS_TextDraw( int item )
{
static matrix position(3,1),rotation(3,1);
int i,w;
float size;

    if( !GRAPHICS_TextList[item].inuse )
    {
        return;
    }

    position = GRAPHICS_TextList[item].position.isempty() ? GRAPHICS_TextDefaultPosition : GRAPHICS_TextList[item].position;
    rotation = GRAPHICS_TextList[item].rotation.isempty() ? GRAPHICS_TextDefaultRotation : GRAPHICS_TextList[item].rotation;

    // Approximately centre the text.
    w = STR_length(GRAPHICS_TextList[item].text);
    size = 0.015 * GRAPHICS_TextList[item].size;
    position(1,1) -= (0.5 * (double)w) * (size * 100.0);

    glPushMatrix();
    GRAPHICS_LightingDisable();
    GRAPHICS_ColorSet(GRAPHICS_TextList[item].color);

    GRAPHICS_Translate(position);
    GRAPHICS_Rotate(rotation);
        
    //glLineWidth(GRAPHICS_TextList[item].width);
    glScalef(size,size,1.0);

    for( i=0; (i < w); i++ )
    {
        glutStrokeCharacter(GRAPHICS_TextList[item].font,GRAPHICS_TextList[item].text[i]);
    }

    GRAPHICS_LightingEnable();
    glPopMatrix();
}

/******************************************************************************/

void GRAPHICS_TextDraw( TIMER_Interval *latency )
{
int item;

    if( GRAPHICS_TextInUse() && (latency != NULL) )
    {
        latency->Before();
    }

    for( item=0; (item < GRAPHICS_TEXT_LIST); item++ )
    {
        GRAPHICS_TextDraw(item);
    }

    if( GRAPHICS_TextInUse() && (latency != NULL) )
    {
        latency->After();
    }
}

/******************************************************************************/

void GRAPHICS_TextDraw( void )
{
TIMER_Interval *latency=NULL;

    GRAPHICS_TextDraw(latency);
}

/******************************************************************************/

//this means the method in graphics-old.cpp will rewrite the function above.
#include "graphics-old.cpp"

/******************************************************************************/


