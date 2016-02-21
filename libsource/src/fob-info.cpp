/******************************************************************************/

struct  STR_TextItem    FOB_ErrorText[] = 
{
    { FOB_ERROR_NONE           ,"NONE"            },
    { FOB_ERROR_RAM            ,"RAM"             },
    { FOB_ERROR_EEPROMWRITE    ,"EEPROMWRITE"     },
    { FOB_ERROR_EEPROMCORRUPT  ,"EEPROMCORRUPT"   },
    { FOB_ERROR_TRANSMITCORRUPT,"TRANSMITCORRUPT" },
    { FOB_ERROR_SENSORCORRUPT  ,"SENSORCORRUPT"   },
    { FOB_ERROR_RS232COMMAND   ,"RS232COMMAND"    },
    { FOB_ERROR_NOTFBBMASTER   ,"NOTFBBMASTER"    },
    { FOB_ERROR_NOBIRDS        ,"NOBIRDS"         },
    { FOB_ERROR_BIRDINIT       ,"BIRDINIT"        },
    { FOB_ERROR_FBBRECVBIRD    ,"FBBRECVBIRD"     },
    { FOB_ERROR_RS232RECV      ,"RS232RECV"       },
    { FOB_ERROR_FBBBRECVHOST   ,"FBBBRECVHOST"    },
    { FOB_ERROR_FBBRECVSLAVE   ,"FBBRECVSLAVE"    },
    { FOB_ERROR_FBBCOMMAND     ,"FBBCOMMAND"      },
    { FOB_ERROR_FBBRUNTIME     ,"FBBRUNTIME"      },
    { FOB_ERROR_CPUSPEED       ,"CPUSPEED"        },
    { FOB_ERROR_NODATA         ,"NODATA"          },
    { FOB_ERROR_BAUDRATE       ,"BAUDRATE"        },
    { FOB_ERROR_SLAVEACK       ,"SLAVEACK"        },
    { FOB_ERROR_CRTSYNC        ,"CRTSYNC"         },
    { FOB_ERROR_NOTRANSMIT     ,"NOTRANSMIT"      },
    { FOB_ERROR_NOEXTTRANSMIT  ,"NOEXTTRANSMIT"   },
    { FOB_ERROR_CPUTIME        ,"CPUTIME"         },
    { FOB_ERROR_SATURATED      ,"SATURATED"       },
    { FOB_ERROR_SLAVECONFIG    ,"SLAVECONFIG"     },
    { FOB_ERROR_WATCHDOG       ,"WATCHDOG"        },
    { FOB_ERROR_OVERTEMP       ,"OVERTEMP"        },
    { FOB_ERROR_GENERAL        ,"GENERAL"         },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

char   *FOB_StageText[] = { "INIT","SYNC","FRAMEBYTE","FRAMEBODY","DONE",NULL };
BOOL    FOB_StageComs[] = { FALSE,FALSE,TRUE,TRUE,FALSE };

/******************************************************************************/

struct  STR_TextItem    FOB_DataFormatText[] =
{   // Bird data formats...
    { FOB_DATAFORMAT_NOBIRDDATA    ,"NOBIRDDATA"     },         // no data (NOTE: RS232 and ISA modes have no way of specifying this format)
    { FOB_DATAFORMAT_POSITION      ,"POSITION"       },         // position only
    { FOB_DATAFORMAT_ANGLES        ,"ANGLES"         },         // angles only
    { FOB_DATAFORMAT_MATRIX        ,"MATRIX"         },         // matrix only
    { FOB_DATAFORMAT_POSNANGLES    ,"POSNANGLES"     },         // position and angles
    { FOB_DATAFORMAT_POSNMATRIX    ,"POSNMATRIX"     },         // position and matrix
    { FOB_DATAFORMAT_QUATERNION    ,"QUATERNION"     },         // quaternion only
    { FOB_DATAFORMAT_POSNQUATERNION,"POSNQUATERNION" },         // position and quaternion
    { FOB_DATAFORMAT_MATRIX        ,"ROTATIONS"      },         // matrix only
    { FOB_DATAFORMAT_MATRIX        ,"ROMX"           },         // matrix only
    { FOB_DATAFORMAT_POSNMATRIX    ,"RTMX"           },         // position and matrix
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

//      Data types..  .       NONE ,POS  ,ANG  ,MTX  ,POSANG,POSMTX,QTN  ,POSQTN
BOOL    FOB_DataTypePOS[] = { FALSE,TRUE ,FALSE,FALSE,TRUE  ,TRUE  ,FALSE,TRUE  };
BOOL    FOB_DataTypeANG[] = { FALSE,FALSE,TRUE ,FALSE,TRUE  ,FALSE ,FALSE,FALSE };
BOOL    FOB_DataTypeMTX[] = { FALSE,FALSE,FALSE,TRUE ,FALSE ,TRUE  ,FALSE,FALSE };
BOOL    FOB_DataTypeQTN[] = { FALSE,FALSE,FALSE,FALSE,FALSE ,FALSE ,TRUE ,TRUE  };

/******************************************************************************/

BOOL    FOB_DataType( int ID, int bird, BOOL list[] )
{
BOOL    flag=FALSE;

    if( ID == FOB_PORT_ALL )
    {
        if( bird == FOB_BIRD_ALL )
        {
            flag = FOB_DataType(list);
        }
        else
        {
            flag = list[FOB_dataformat[bird]];
        }
    }
    else
    if( bird == FOB_BIRD_ALL )
    {
        flag = FOB_DataType(ID,list);
    }
    else
    if( FOB_Check(ID) )
    {
        flag = list[FOB_Port[ID].dataformat[bird]];
    }

    return(flag);
}

/******************************************************************************/

BOOL    FOB_DataType( int ID, BOOL list[] )
{
BOOL    flag;
int     bird;

    for( flag=TRUE,bird=0; ((bird < FOB_BirdsOnPort(ID)) && flag); bird++ )
    {
        flag = FOB_DataType(ID,bird,list);
    }

    return(flag);
}

/******************************************************************************/

BOOL    FOB_DataType( BOOL list[] )
{
BOOL    flag;
int     ID;

    for( flag=TRUE,ID=0; ((ID < FOB_PortsInFlock()) && flag); ID++ )
    {
        flag = FOB_DataType(ID,list);
    }

    return(flag);
}

/******************************************************************************/

BOOL    FOB_DataTypePosition( int ID, int bird )
{
    return(FOB_DataType(ID,bird,FOB_DataTypePOS));
}

/******************************************************************************/

BOOL    FOB_DataTypePosition( int bird )
{
    return(FOB_DataType(FOB_PORT_ALL,bird,FOB_DataTypePOS));
}

/******************************************************************************/

BOOL    FOB_DataTypePosition( void )
{
    return(FOB_DataType(FOB_DataTypePOS));
}

/******************************************************************************/

BOOL    FOB_DataTypeAngles( int ID, int bird )
{
    return(FOB_DataType(ID,bird,FOB_DataTypeANG));
}

/******************************************************************************/

BOOL    FOB_DataTypeAngles( int bird )
{
    return(FOB_DataType(FOB_PORT_ALL,bird,FOB_DataTypeANG));
}

/******************************************************************************/

BOOL    FOB_DataTypeAngles( void )
{
    return(FOB_DataType(FOB_DataTypeANG));
}

/******************************************************************************/

BOOL    FOB_DataTypeMatrix( int ID, int bird )
{
    return(FOB_DataType(ID,bird,FOB_DataTypeMTX));
}

/******************************************************************************/

BOOL    FOB_DataTypeMatrix( int bird )
{
    return(FOB_DataType(FOB_PORT_ALL,bird,FOB_DataTypeMTX));
}

/******************************************************************************/

BOOL    FOB_DataTypeMatrix( void )
{
    return(FOB_DataType(FOB_DataTypeMTX));
}
/******************************************************************************/

BOOL    FOB_DataTypeQuaternion( int ID, int bird )
{
    return(FOB_DataType(ID,bird,FOB_DataTypeQTN));
}

/******************************************************************************/

BOOL    FOB_DataTypeQuaternion( int bird )
{
    return(FOB_DataType(FOB_PORT_ALL,bird,FOB_DataTypeQTN));
}

/******************************************************************************/

BOOL    FOB_DataTypeQuaternion( void )
{
    return(FOB_DataType(FOB_DataTypeQTN));
}

/******************************************************************************/

BOOL    FOB_DataTypePOMX( int bird )
{
    return(FOB_DataType(FOB_PORT_ALL,bird,FOB_DataTypePOS));
}

/******************************************************************************/

BOOL    FOB_DataTypeROMX( int bird )
{
    return(FOB_DataType(FOB_PORT_ALL,bird,FOB_DataTypeMTX));
}

/******************************************************************************/

BOOL    FOB_DataTypeRTMX( int bird )
{
    return(FOB_DataType(FOB_PORT_ALL,bird,FOB_DataTypePOS) && FOB_DataType(FOB_PORT_ALL,bird,FOB_DataTypeMTX));
}

/******************************************************************************/

struct  STR_TextItem    FOB_HemisphereText[] =
{   // Bird hemisphere codes...
    { FOB_HEMISPHERE_FRONT,"FRONT" },
    { FOB_HEMISPHERE_REAR ,"REAR"  },
    { FOB_HEMISPHERE_UPPER,"UPPER" },
    { FOB_HEMISPHERE_LOWER,"LOWER" },
    { FOB_HEMISPHERE_LEFT ,"LEFT"  },
    { FOB_HEMISPHERE_RIGHT,"RIGHT" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem    FOB_AngleUnitText[] =
{   // Bird hemisphere codes...
    { FOB_ANGLEUNIT_DEGREES,"DEGREES" },
    { FOB_ANGLEUNIT_RADIANS,"RADIANS" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

UCHAR   FOB_DataFormatCode[] =         // Data format command codes...
{
    0x00,
    FOB_CMD_POSN,
    FOB_CMD_ANGLES,
    FOB_CMD_MATRIX,
    FOB_CMD_POSNANGLES,
    FOB_CMD_POSNMATRIX,
    FOB_CMD_QUATERNION,
    FOB_CMD_POSNQUATERNION,
};

/******************************************************************************/

struct  STR_TextItem    FOB_BirdStatusFlag[] =
{   // Bird status flags...
    { FOB_BIRDSTATUS_MASTER   ,"MASTER"          },
    { FOB_BIRDSTATUS_CONFIG   ,"AUTO-CONFIGURED" },
    { FOB_BIRDSTATUS_ERROR    ,"ERROR"           },
    { FOB_BIRDSTATUS_RUNNING  ,"RUNNING"         },
    { FOB_BIRDSTATUS_HOSTSYNC ,"HOST-SYNC"       },
    { FOB_BIRDSTATUS_EXPANDED ,"EXANDED-ADDRESS" },
    { FOB_BIRDSTATUS_CRTSYNC  ,"CRT-SYNC"        },
    { FOB_BIRDSTATUS_NOSYNC   ,"NO-SYNC"         },
    { FOB_BIRDSTATUS_TESTMODE ,"FACTORY-TEST"    },
    { FOB_BIRDSTATUS_XOFF     ,"XOFF"            },
    { FOB_BIRDSTATUS_SLEEP    ,"SLEEPING"        },
    { FOB_BIRDSTATUS_STREAM   ,"STREAM"          },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem    FOB_BirdStatusFormatText[] =
{   // Bird status data formats...
    { FOB_BIRDSTATUS_POS   ,"POS"     },
    { FOB_BIRDSTATUS_ANG   ,"ANG"     },
    { FOB_BIRDSTATUS_MTX   ,"MTX"     },
    { FOB_BIRDSTATUS_POSANG,"POS-ANG" },
    { FOB_BIRDSTATUS_POSMTX,"POS-MTX" },
    { FOB_BIRDSTATUS_QTN   ,"QTN"     },
    { FOB_BIRDSTATUS_POSQTN,"POS-QTN" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem    FOB_FlockStatusFlag[] =
{   // Flock status flags...
    { FOB_FLOCKSTATUS_USED    ,"USED"        },
    { FOB_FLOCKSTATUS_RUNNING ,"RUNNING"     },
    { FOB_FLOCKSTATUS_SENSOR  ,"SENSOR"      },
    { FOB_FLOCKSTATUS_ERT     ,"ERT"         },
    { FOB_FLOCKSTATUS_TRANSMIT,"TRANSMITTER" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem    FOB_ValueText[] =
{   // FOB parameter values...
    { FOB_VAL_BIRDSTATUS    ,"BIRDSTATUS"    },
    { FOB_VAL_SOFTWARE      ,"SOFTWARE"      },
    { FOB_VAL_BIRDSPEED     ,"BIRDSPEED"     },
    { FOB_VAL_POSNSCALE     ,"POSNSCALE"     },
    { FOB_VAL_FILTERSTATUS  ,"FILTERSTATUS"  },
    { FOB_VAL_DCALPHAMIN    ,"DCALPHAMIN"    },
    { FOB_VAL_BIRDRATECOUNT ,"BIRDRATECOUNT" },
    { FOB_VAL_BIRDRATE      ,"BIRDRATE"      },
    { FOB_VAL_DATAREADY     ,"DATAREADY"     },
    { FOB_VAL_DATAREADYCHAR ,"DATAREADYCHAR" },
    { FOB_VAL_ERRORCODE     ,"ERRORCODE"     },
    { FOB_VAL_ONERROR       ,"ONERROR"       },
    { FOB_VAL_DCTABLEVM     ,"DCTABLEVM"     },
    { FOB_VAL_DCALPHAMAX    ,"DCALPHAMAX"    },
    { FOB_VAL_OUTPUTCHANGE  ,"OUTPUTCHANGE"  },
    { FOB_VAL_SYSTEMID      ,"SYSTEMID"      },
    { FOB_VAL_EXTERRORCODE  ,"EXTERRORCODE"  },
    { FOB_VAL_XYZFRAME      ,"XYZFRAME"      },
    { FOB_VAL_TRANSMITMODE, "TRANSMITMODE"   },
    { FOB_VAL_FBBMODE       ,"FBBMODE"       },
    { FOB_VAL_FILTERFREQ    ,"FILTERFREQ"    },
    { FOB_VAL_FBBADDRESS    ,"FBBADDRESS"    },
    { FOB_VAL_HEMISPHERE    ,"HEMISPHERE"    },
    { FOB_VAL_ANGLEALIGN2   ,"ANGLEALIGN2"   },
    { FOB_VAL_REFFRAME2     ,"REFFRAME2"     },
    { FOB_VAL_BIRDSERIAL    ,"BIRDSERIAL"    },
    { FOB_VAL_SENSORSERIAL  ,"SENSORSERIAL"  },
    { FOB_VAL_TRANSMITSERIAL,"TRANSMITSERIAL"},
    { FOB_VAL_FBBHOSTDELAY  ,"FBBHOSTDELAY"  },
    { FOB_VAL_GROUPMODE     ,"GROUPMODE"     },
    { FOB_VAL_FLOCKSTATUS   ,"FLOCKSTATUS"   },
    { FOB_VAL_AUTOCONFIG    ,"AUTOCONFIG"    },
};

/******************************************************************************/

struct  STR_TextItem    FOB_FilterStatusFlag[] =
{   // FOB filter flags...
    { FOB_FILTER_ACNARROWOFF ,"AC-NARROW-OFF"   },
    { FOB_FILTER_ACWIDEOFF   ,"AC-WIDE-OFF"     },
    { FOB_FILTER_DCOFF       ,"DC-OFF"          },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem    FOB_FilterStatusText[] = 
{   // FOB filter text for configuration file...
    { FOB_FILTER_NONE    ,"NONE"        },
    { FOB_FILTER_ACNARROW,"AC-NARROW"   },
    { FOB_FILTER_ACWIDE  ,"AC-WIDE"     },
    { FOB_FILTER_DC      ,"DC"          },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem    FOB_DataModeText[] =
{   // FOB filter flags...
    { FOB_DATAMODE_DRC   ,"DRC"    },
    { FOB_DATAMODE_POINT ,"POINT"  },
    { FOB_DATAMODE_STREAM,"STREAM" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem    FOB_TransmitModeText[] =
{   // FOB filter flags...
//  { FOB_TRANSMITMODE_NONPULSED,"NONPULSED" },
    { FOB_TRANSMITMODE_PULSED   ,"PULSED"    },
    { FOB_TRANSMITMODE_COOLDOWN ,"COOLDOWN"  },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

struct  STR_TextItem    FOB_SyncText[] =
{   // Sync modes...
    { FOB_SYNC_NONE,"NONE" },
    { FOB_SYNC_CRT1,"CRT1" },
    { FOB_SYNC_CRT2,"CRT2" },
    { FOB_SYNC_HOST,"HOST" },
    { FOB_SYNC_TEST,"TEST" },
    { STR_TEXT_ENDOFTABLE },
};

/******************************************************************************/

void    FOB_ScaleCode( WORD scale, WORD &code )
{
    switch( scale )
    {
        case FOB_SCALE_72INCH :
           code = FOB_SCALEFLAG_72INCH;
           break;

        case FOB_SCALE_36INCH :
        default :
           code = FOB_SCALEFLAG_36INCH;
           break;
    }
}
 
/******************************************************************************/

WORD    FOB_ScaleCode( WORD code )
{
WORD    scale;

    switch( code )
    {
        case FOB_SCALEFLAG_72INCH :
           scale = FOB_SCALE_72INCH;
           break;

        case FOB_SCALEFLAG_36INCH :
        default :
           scale = FOB_SCALE_36INCH;
           break;
    }

    return(scale);
}

/******************************************************************************/

WORD    FOB_BirdRate2Code( float rate )
{
WORD    code;

    code = (WORD)(rate * FOB_BIRDRATE_DIVISOR);

    return(code);
}

/******************************************************************************/

float   FOB_BirdCode2Rate( WORD code )
{
float   rate;

    rate = (float)code / FOB_BIRDRATE_DIVISOR;

    return(rate);
}

/******************************************************************************/

BYTE    FOB_HemisphereAxis[] = { FOB_HEMI_AXIS_X,FOB_HEMI_AXIS_X,FOB_HEMI_AXIS_Z,FOB_HEMI_AXIS_Z,FOB_HEMI_AXIS_Y,FOB_HEMI_AXIS_Y };
BYTE    FOB_HemisphereSign[] = { FOB_HEMI_SIGN_P,FOB_HEMI_SIGN_N,FOB_HEMI_SIGN_N,FOB_HEMI_SIGN_P,FOB_HEMI_SIGN_N,FOB_HEMI_SIGN_P };
BYTE    FOB_HemisphereFlip[] = { FOB_HEMISPHERE_REAR,FOB_HEMISPHERE_FRONT,FOB_HEMISPHERE_LOWER,FOB_HEMISPHERE_UPPER,FOB_HEMISPHERE_RIGHT,FOB_HEMISPHERE_LEFT };

/******************************************************************************/

int     FOB_HemisphereCode( WORD code )
{
int     item,hemisphere;
BYTE    axis,sign;

    axis = (code & 0x00FF);
    sign = (code & 0xFF00) >> 8;

    for( hemisphere=FOB_HEMISPHERE_FRONT,item=0; (item < FOB_HEMISPHERE_MAX); item++ )
    {
        if( (FOB_HemisphereAxis[item] == axis) && (FOB_HemisphereSign[item] == sign) )
        {
            hemisphere = item;
        }
    }

    return(hemisphere);
}
 
/******************************************************************************/

void    FOB_HemisphereCode( int hemisphere, WORD &code )
{
BYTE    axis,sign;

    axis = FOB_HemisphereAxis[hemisphere];
    sign = FOB_HemisphereSign[hemisphere];

    code = axis + (sign << 8);
}

/******************************************************************************/

struct  LOOKUP_Int2Bool
{
    long code;
    BOOL data;
};

#define LOOKUP_ENDOFTABLE  LONG_MAX
#define LOOKUP_NOTFOUND          -1

/******************************************************************************/

BOOL    LOOKUP_EoT( struct LOOKUP_Int2Bool list[], int item )
{
BOOL    EoT;

    EoT = (list[item].code == LOOKUP_ENDOFTABLE);

    return(EoT);
}

/******************************************************************************/

BOOL    LOOKUP_Item( struct LOOKUP_Int2Bool list[], long code, BOOL &data )
{
int     item,find;
BOOL    ok=FALSE;

    for( find=LOOKUP_NOTFOUND,item=0; !LOOKUP_EoT(list,item); item++ )
    {
        if( list[item].code == code )
        {
            find = item;
            break;
        }
    }

    if( find != LOOKUP_NOTFOUND )
    {
        data = list[find].data;
        ok = TRUE;
    }

    return(ok);
}

/******************************************************************************/

BOOL    LOOKUP_Item( struct LOOKUP_Int2Bool list[], int code, BOOL &data )
{
    return(LOOKUP_Item(list,(long)code,data));
}
/******************************************************************************/

BOOL    LOOKUP_Item( struct LOOKUP_Int2Bool list[], BYTE code, BOOL &data )
{
    return(LOOKUP_Item(list,(long)code,data));
}

/******************************************************************************/

struct  LOOKUP_Int2Bool      FOB_ErrorFatalList[] = 
{
    { FOB_ERROR_NONE           ,FALSE },
    { FOB_ERROR_RAM            ,TRUE  },
    { FOB_ERROR_EEPROMWRITE    ,TRUE  },
    { FOB_ERROR_EEPROMCORRUPT  ,FALSE },
    { FOB_ERROR_TRANSMITCORRUPT,FALSE },
    { FOB_ERROR_SENSORCORRUPT  ,FALSE },
    { FOB_ERROR_RS232COMMAND   ,FALSE },
    { FOB_ERROR_NOTFBBMASTER   ,FALSE },
    { FOB_ERROR_NOBIRDS        ,FALSE },
    { FOB_ERROR_BIRDINIT       ,FALSE },
    { FOB_ERROR_FBBRECVBIRD    ,FALSE },
    { FOB_ERROR_RS232RECV      ,FALSE },
    { FOB_ERROR_FBBBRECVHOST   ,FALSE },
    { FOB_ERROR_FBBRECVSLAVE   ,FALSE },
    { FOB_ERROR_FBBCOMMAND     ,FALSE },
    { FOB_ERROR_FBBRUNTIME     ,TRUE  },
    { FOB_ERROR_CPUSPEED       ,TRUE  },
    { FOB_ERROR_NODATA         ,FALSE },
    { FOB_ERROR_BAUDRATE       ,FALSE },
    { FOB_ERROR_SLAVEACK       ,FALSE },
    { FOB_ERROR_CRTSYNC        ,FALSE },
    { FOB_ERROR_NOTRANSMIT     ,FALSE },
    { FOB_ERROR_NOEXTTRANSMIT  ,FALSE },
    { FOB_ERROR_CPUTIME        ,FALSE },
    { FOB_ERROR_SATURATED      ,FALSE },
    { FOB_ERROR_SLAVECONFIG    ,FALSE },
    { FOB_ERROR_WATCHDOG       ,FALSE },
    { FOB_ERROR_OVERTEMP       ,FALSE },
    { FOB_ERROR_GENERAL        ,TRUE  },
    { LOOKUP_ENDOFTABLE },
};

/******************************************************************************/

BOOL    FOB_ErrorFatal( BYTE code )
{
BOOL    fatal=FALSE;

    if( !LOOKUP_Item(FOB_ErrorFatalList,code,fatal) )
    {
        fatal = FALSE;
    }

    return(fatal);
}

/******************************************************************************/

