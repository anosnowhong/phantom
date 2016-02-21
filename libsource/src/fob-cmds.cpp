/******************************************************************************/

BYTE    FOB_FBB( BYTE addr )
{
BYTE    FBB;

    FBB = FOB_FBB_ADDRESS + addr;

    return(FBB);
}

/******************************************************************************/
/* FOB RS232 command buffer functions...                                      */
/******************************************************************************/

BYTE    FOB_CmdBuff[FOB_CMD_BUFF];     // Command buffer.
int     FOB_CmdBuffIndx=0;             // Current index into command buffer.

/******************************************************************************/

void    FOB_CmdBuffInit( void )
{
    memset(FOB_CmdBuff,NUL,FOB_CMD_BUFF);   // Clear command buffer.
    FOB_CmdBuffIndx = 0;                    // And reset command buffer index.
}

/******************************************************************************/

void    FOB_CmdBuffByte( BYTE data )
{
    if( FOB_CmdBuffIndx >= FOB_CMD_BUFF )
    {
        FOB_errorf("FOB_CmdBuffByte(...) Buffer exceeded.\n");
        return;
    }

    FOB_CmdBuff[FOB_CmdBuffIndx++] = data;
}

/******************************************************************************/

void    FOB_CmdBuffWord( WORD data )
{
    FOB_CmdBuffData(&data,sizeof(WORD));
}

/******************************************************************************/

void    FOB_CmdBuffCmd( UCHAR cmd )
{
    FOB_CmdBuffByte(cmd);
}

/******************************************************************************/

void    FOB_CmdBuffData( void *data, int size )
{
BYTE   *bptr=(BYTE *)data;
int     i;

    for( i=0; (i < size); i++ )
    {
        FOB_CmdBuffByte(bptr[i]);
    }
}

/******************************************************************************/

void    FOB_CmdBuffFBB( int ID, int bird )
{
BYTE    addr=FOB_ADDR_NULL;

    if( FOB_MultiBirdPort(ID) && (bird != FOB_BIRD_NULL) )
    {
        if( (addr=FOB_Port[ID].address[bird]) == FOB_ADDR_NULL )
        {
            FOB_errorf("FOB_CmdBuffFBB(ID=%d,bird=%d) NULL address.\n");
            addr = (BYTE)(bird+1);
        }
    }

    if( addr != FOB_ADDR_NULL )
    {
        FOB_CmdBuffByte(FOB_FBB(addr));
    }
}

/******************************************************************************/

int     FOB_CmdBuffSize( void )
{
    return(FOB_CmdBuffIndx);
}

/******************************************************************************/

BOOL    FOB_CmdBuffWrite( int comH )
{
BOOL    ok=FALSE;

    if( comH != COM_INVALID )
    {
        ok = COM_Write(comH,FOB_CmdBuff,FOB_CmdBuffSize());
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdBuffSend( int ID )
{
BOOL    ok=FALSE;

    if( FOB_Check(ID) )
    {
        ok = FOB_CmdBuffWrite(FOB_Port[ID].comH);
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdBuffSend( int ID, void *recv, int size )
{
BOOL    ok;

    if( (ok=FOB_CmdBuffSend(ID)) )     // Send FOB command...
    {                                  // and receive reply...
        ok = COM_Read(FOB_Port[ID].comH,recv,size);
    }

    return(ok);
}

/******************************************************************************/
/* Implementations of FOB commands...                                         */
/******************************************************************************/

BOOL    FOB_CmdDataFormat( int ID )
{
BOOL    ok;
BYTE    dataformat;
int     bird;

//  Start a new command buffer...
    FOB_CmdBuffInit();

//  Set data format for each bird on this port (so use FBB address)...
    for( bird=0; (bird < FOB_BirdsOnPort(ID)); bird++ )
    {
        dataformat = FOB_DataFormatCode[FOB_Port[ID].dataformat[bird]];
        FOB_CmdBuffFBB(ID,bird);                 // Which FBB bird address?
        FOB_CmdBuffCmd(dataformat);              // COMMAND: Data formant.
    }

//  Sent command buffer...
    ok = FOB_CmdBuffSend(ID);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_CmdDataFormat(ID=%d,birds=%d) %s.\n",ID,FOB_BirdsOnPort(ID),STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

#define FOB_CONFIG_DATAFORMAT     0    // FOB Configuration steps...
#define FOB_CONFIG_AUTOCONFIG     1
#define FOB_CONFIG_GROUPMODE      2
#define FOB_CONFIG_SETFRAMERATE   3
#define FOB_CONFIG_GETFRAMERATE   4
#define FOB_CONFIG_FILTERSTATUS   5
#define FOB_CONFIG_DCFILTER       6
#define FOB_CONFIG_OUTPUTCHANGE   7
#define FOB_CONFIG_TRANSMITMODE   8
#define FOB_CONFIG_BIRDSPEED      9
#define FOB_CONFIG_SYNC          10
#define FOB_CONFIG_HEMISPHERE    11

char   *FOB_ConfigStep[] = { "DataFormat","AutoConfig","GroupMode","SetFrameRate","GetFrameRate","FilterStatus","DCFilter","OutputChange","TransmitterMode","BirdSpeed","Sync","Hemisphere",NULL };

BOOL    FOB_CmdConfig( int ID )
{
BOOL    ok,done;
int     step,bird;

    for( ok=TRUE,step=FOB_CONFIG_DATAFORMAT; ((step <= FOB_CONFIG_HEMISPHERE) && ok); step++ )
    {
        done = FALSE;

        switch( step )
        {
            case FOB_CONFIG_DATAFORMAT :
               ok = FOB_CmdDataFormat(ID);
               done = TRUE;
               break;

            case FOB_CONFIG_AUTOCONFIG :
               if( !FOB_Master(ID) )
               {
                   continue;
               }

               if( (ok=FOB_CmdAutoConfig(ID)) )
               {
                   ok = FOB_FlockStatusOK(ID);
               }

               done = TRUE;
               break;

            case FOB_CONFIG_GROUPMODE :
               if( !FOB_MultiBirdPort(ID) )
               {
                   continue;
               }

               ok = FOB_ValGroupMode(ID,FOB_VAL_SET,FOB_groupmode);
               done = TRUE;
               break;

            case FOB_CONFIG_SETFRAMERATE :
               if( !FOB_cfg_read[FOB_cfg_framerate] )
               {
                   break;
               }

               for( bird=0; ((bird < FOB_BirdsOnPort(ID)) && ok); bird++ )
               {
                   ok = FOB_ValBirdRate(ID,bird,FOB_VAL_SET,FOB_framerate);
               }

               done = TRUE;
               break;

           case FOB_CONFIG_GETFRAMERATE :
               if( !FOB_Master(ID) )
               {
                   break;
               }
         
               if( (ok=FOB_ValBirdRate(ID,FOB_BIRD_NULL,FOB_VAL_GET,FOB_frameHz)) )
               {
                   FOB_framemsec = 1000.0 / FOB_frameHz;
               }

               done = TRUE;
               break;

            case FOB_CONFIG_FILTERSTATUS :
               if( !FOB_cfg_read[FOB_cfg_filters] )
               {
                   break;
               }

               for( bird=0; ((bird < FOB_BirdsOnPort(ID)) && ok); bird++ )
               {
                   if( (ok=FOB_ValFilterStatus(ID,bird,FOB_VAL_SET,FOB_filterstatus)) )
                   {
                       ok = FOB_ValFilterStatus(ID,bird,FOB_VAL_GET,FOB_filterstatus);
                   }
               }

               done = TRUE;
               break;

            case FOB_CONFIG_DCFILTER :
               for( bird=0; ((bird < FOB_BirdsOnPort(ID)) && ok); bird++ )
               {
                   if( FOB_cfg_read[FOB_cfg_DCalphamin] )
                   {
                       done = TRUE;

                       if( !FOB_ValDCAlphaMin(ID,bird,FOB_VAL_SET,FOB_DCalphamin) )
                       {
                           ok = FALSE;
                           continue;
                       }
                   }

                   if( FOB_cfg_read[FOB_cfg_DCalphamax] )
                   {
                       done = TRUE;

                       if( !FOB_ValDCAlphaMax(ID,bird,FOB_VAL_SET,FOB_DCalphamax) )
                       {
                           ok = FALSE;
                           continue;
                       }
                   }

                   if( FOB_cfg_read[FOB_cfg_DCtableVm] )
                   {
                       done = TRUE;

                       if( !FOB_ValDCTableVm(ID,bird,FOB_VAL_SET,FOB_DCtableVm) )
                       {
                           ok = FALSE;
                           continue;
                       }
                   }
               }
               break;

            case FOB_CONFIG_OUTPUTCHANGE :
               if( !FOB_cfg_read[FOB_cfg_outputchange] )
               {
                   break;
               }

               for( bird=0; ((bird < FOB_BirdsOnPort(ID)) && ok); bird++ )
               {
                   ok = FOB_ValOutputChange(ID,bird,FOB_VAL_SET,FOB_outputchange);
               }

               done = TRUE;
               break;

           case FOB_CONFIG_TRANSMITMODE :
               if( !FOB_Master(ID) )
               {
                   break;
               }
         
               if( !FOB_cfg_read[FOB_cfg_transmittermode] )
               {
                   break;
               }

               if( (ok=FOB_ValTransmitMode(ID,FOB_VAL_SET,FOB_transmittermode)) )
               {
                   ok = FOB_ValTransmitMode(ID,FOB_VAL_GET,FOB_transmittermode);
               }

               done = TRUE;
               break;

           case FOB_CONFIG_BIRDSPEED :
               if( !FOB_Master(ID) )
               {
                   break;
               }

               ok = FOB_GetBirdSpeed(ID,FOB_Port[ID].birdspeed);
               break;

           case FOB_CONFIG_SYNC :
               if( !FOB_Master(ID) )
               {
//                   break;
               }
         
               if( !FOB_cfg_read[FOB_cfg_sync] )
               {
                   break;
               }

               ok = FOB_CmdSync(ID,FOB_sync);
               done = TRUE;
               break;

           case FOB_CONFIG_HEMISPHERE :
               break;

               for( bird=0; ((bird < FOB_BirdsOnPort(ID)) && ok); bird++ )
               {
                   ok = FOB_CmdHemisphere(ID,bird,FOB_Port[ID].hemisphere[bird]);
                   FOB_debugf("FOB_CmdHemisphere(ID=%d,bird=%d,hemisphere=%s) %s.\n",ID,bird,STR_TextCode(FOB_HemisphereText,FOB_Port[ID].hemisphere[bird]),STR_OkFailed(ok));
               }

               done = TRUE;
               break;
        }

        STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_CmdConfig(ID=%d) %s %s.\n",ID,FOB_ConfigStep[step],done ? STR_OkFailed(ok) : "Skipped");
    }

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_CmdConfig(ID=%d) %s.\n",ID,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdAutoConfig( int ID )
{
BOOL    ok;

    ok = FOB_SetFBB(ID,FOB_BirdsInFlock());

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_CmdAutoConfig(ID=%d) %s.\n",ID,STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdRun( int ID )
{
BOOL    ok;

    FOB_CmdBuffInit();
    FOB_CmdBuffCmd(FOB_CMD_RUN);
    
    ok = FOB_CmdBuffSend(ID);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdResetViaRTS( int ID, BOOL nowait )
{
BOOL    ok;
BOOL    flag[] = { ON,OFF };
double  wait[] = { FOB_WAIT_RTS,FOB_WAIT_RESET };
int     step;

    if( nowait )
    {
        wait[1] = 0.0;
    }

    for( ok=TRUE,step=0; ((step < 2) && ok); step++ )
    {
        if( (ok=COM_RTS(FOB_Port[ID].comH,flag[step])) )
        {
            TIMER_delay(wait[step]);
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdResetViaFBB( int ID )
{
BOOL    ok;

    FOB_CmdBuffInit();
    FOB_CmdBuffCmd(FOB_CMD_FBBRESET);
    
    ok = FOB_CmdBuffSend(ID);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdSleep( int ID )
{
BOOL    ok;

    FOB_CmdBuffInit();
    FOB_CmdBuffCmd(FOB_CMD_SLEEP);
    
    ok = FOB_CmdBuffSend(ID);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdSync( int ID, BYTE mode, float &voltage, float &scanrate )
{
BOOL    ok;
WORD    buff[2];
int     value;

    FOB_CmdBuffInit();
    FOB_CmdBuffCmd(FOB_CMD_SYNC);
    FOB_CmdBuffByte(mode);
    
    switch( mode )
    {
        case FOB_SYNC_NONE :
        case FOB_SYNC_CRT1 :
        case FOB_SYNC_CRT2 :
        case FOB_SYNC_HOST :
           ok = FOB_CmdBuffSend(ID);
           break;

        case FOB_SYNC_TEST :
           if( (ok=FOB_CmdBuffSend(ID,buff,sizeof(buff))) )
           {
               // Calculate voltage...
               value = (unsigned int)buff[0];
               if( buff[0] & 0x8000 )
               {
                   value -= 0x10000;
               }

               voltage = (4.99 / (float)0x7FFF) * (float)value;

               // Calculate detected scan-rate...
               value = (unsigned int)buff[1];

               scanrate = 500000.0 / ((8.0 / (float)FOB_Port[ID].birdspeed) * (float)value);
           }
           break;

        default :
           break;
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdSync( int ID, BYTE mode )
{
float   voltage,scanrate;
BOOL    ok;

    ok = FOB_CmdSync(ID,mode,voltage,scanrate);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdPoint( int ID, int bird )
{
BOOL    ok;

    FOB_CmdBuffInit();
    FOB_CmdBuffFBB(ID,bird);
    FOB_CmdBuffCmd(FOB_CMD_POINT);

    ok = FOB_CmdBuffSend(ID);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdStream( int ID )
{
BOOL    ok;

    FOB_CmdBuffInit();
    FOB_CmdBuffCmd(FOB_CMD_STREAM);

    ok = FOB_CmdBuffSend(ID);

    return(ok);
}
/******************************************************************************/

BOOL    FOB_CmdHemisphere( int ID, int bird, BYTE axis, BYTE sign )
{
BOOL    ok;

    FOB_CmdBuffInit();
    FOB_CmdBuffFBB(ID,bird);
    FOB_CmdBuffCmd(FOB_CMD_HEMISPHERE);
    FOB_CmdBuffByte(axis);
    FOB_CmdBuffByte(sign);

    ok = FOB_CmdBuffSend(ID);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdHemisphere( int ID, int bird, int hemisphere )
{
BOOL    ok;
BYTE    axis;
BYTE    sign;

    axis = FOB_HemisphereAxis[hemisphere];
    sign = FOB_HemisphereSign[hemisphere];

    ok = FOB_CmdHemisphere(ID,bird,axis,sign);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdGetValue( int ID, int bird, BYTE value, void *buff, int size )
{
BOOL    ok;

//  Sequentially build our FOB command buffer...
    FOB_CmdBuffInit();                           // Reset command buffer.
    FOB_CmdBuffFBB(ID,bird);                     // Address specific bird on FBB?
    FOB_CmdBuffCmd(FOB_CMD_EXAMINEVALUE);        // Set command code (examine value).
    FOB_CmdBuffByte(value);                      // Set value to examine.

//  Send command buffer and get response...
    ok = FOB_CmdBuffSend(ID,buff,size);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_CmdGetValue(ID=%d,bird=%d,value=%s,...) %s.\n",
               ID,bird,STR_TextCode(FOB_ValueText,value),
               STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_CmdSetValue( int ID, int bird, BYTE value, void *buff, int size )
{
BOOL    ok;

//  Sequentially build our FOB command buffer...
    FOB_CmdBuffInit();                           // Reset command buffer.
    FOB_CmdBuffFBB(ID,bird);                     // Address specific bird on FBB?
    FOB_CmdBuffCmd(FOB_CMD_CHANGEVALUE);         // Set command code (examine value).
    FOB_CmdBuffByte(value);                      // Set value to examine.
    FOB_CmdBuffData(buff,size);                  // Set value.

//  Send command buffer, no response...
    ok = FOB_CmdBuffSend(ID);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_CmdSetValue(ID=%d,bird=%d,value=%s,...) %s.\n",
               ID,bird,STR_TextCode(FOB_ValueText,value),
               STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL  (*FOB_ValFunc[])( int ID, int bird, BYTE value, void *buff, int size ) =
{
    FOB_CmdGetValue,
    FOB_CmdSetValue,
};

char   *FOB_ValText[] = 
{
    "Get",
    "Set",
};

/******************************************************************************/

BOOL    FOB_ValByte( int ID, int bird, BYTE value, int func, BYTE *buff )
{
BOOL    ok;

    ok = (*FOB_ValFunc[func])(ID,bird,value,buff,sizeof(BYTE));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValWord( int ID, int bird, BYTE value, int func, WORD *buff )
{
BOOL    ok;

    ok = (*FOB_ValFunc[func])(ID,bird,value,buff,sizeof(WORD));

    return(ok);
}
/******************************************************************************/

BOOL    FOB_ValBuff( int ID, int bird, BYTE value, int func, void *buff, int size )
{
BOOL    ok;

    ok = (*FOB_ValFunc[func])(ID,bird,value,buff,size);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetBirdStatus( int ID, int bird, WORD &status )
{
BOOL    ok;

    ok = FOB_ValWord(ID,bird,FOB_VAL_BIRDSTATUS,FOB_VAL_GET,&status);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_GetBirdStatus(ID=%d,bird=%d) %s.\n[%s]\n",
               ID,bird,
               STR_OkFailed(ok),
               STR_TextFlag(FOB_BirdStatusFlag,status));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetBirdSpeed( int ID, WORD &birdspeed )
{
BOOL    ok;

    ok = FOB_ValWord(ID,FOB_BIRD_NULL,FOB_VAL_BIRDSPEED,FOB_VAL_GET,&birdspeed);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_GetBirdSpeed(ID=%d) %s.\n[%d]\n",
               ID,
               STR_OkFailed(ok),
               birdspeed);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetFlockStatus( int ID, BYTE status[] )
{
BOOL    ok;
int     addr,bird;

    ok = FOB_ValBuff(ID,FOB_BIRD_NULL,FOB_VAL_FLOCKSTATUS,FOB_VAL_GET,status,FOB_FBB_NORMAL);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_GetFlockStatus(ID=%d) %s\n",ID,STR_OkFailed(ok));

    if( ok )
    {
        for( bird=0,addr=1; (bird < FOB_FBB_NORMAL); bird++,addr++ )
        {
            FOB_debugf("STATUS[0x%02X]=0x%02X %s\n",addr,status[bird],STR_TextFlag(FOB_FlockStatusFlag,status[bird]));
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValFilterStatus( int ID, int bird, int func, WORD &filter )
{
BOOL    ok;

    ok = FOB_ValWord(ID,bird,FOB_VAL_FILTERSTATUS,func,&filter);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sFilterStatus(ID=%d,bird=%d,...) %s 0x%04X [%s].\n",
               FOB_ValText[func],
               ID,bird,
               STR_OkFailed(ok),
               filter,
               STR_TextFlag(FOB_FilterStatusFlag,filter));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValFilterFreq( int ID, int bird, int func, BYTE &freq )
{
BOOL    ok;

    ok = FOB_ValByte(ID,bird,FOB_VAL_FILTERFREQ,func,&freq);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sFilterFreq(ID=%d,bird=%d,...) %s [%d Hz].\n",
               FOB_ValText[func],
               ID,bird,
               STR_OkFailed(ok),
               freq);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValDCAlphaMin( int ID, int bird, int func, WORD buff[] )
{
BOOL    ok;

    ok = FOB_ValBuff(ID,bird,FOB_VAL_DCALPHAMIN,func,buff,FOB_DC_TABLE*sizeof(WORD));

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sDCAlphaMin(ID=%d,bird=%d,...) %s [0x%04X,...].\n",
               FOB_ValText[func],
               ID,bird,
               STR_OkFailed(ok),
               buff[0]);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValDCAlphaMax( int ID, int bird, int func, WORD buff[] )
{
BOOL    ok;

    ok = FOB_ValBuff(ID,bird,FOB_VAL_DCALPHAMAX,func,buff,FOB_DC_TABLE*sizeof(WORD));

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sDCAlphaMax(ID=%d,bird=%d,...) %s [0x%04X,...].\n",
               FOB_ValText[func],
               ID,bird,
               STR_OkFailed(ok),
               buff[0]);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValDCTableVm( int ID, int bird, int func, WORD buff[] )
{
BOOL    ok;

    ok = FOB_ValBuff(ID,bird,FOB_VAL_DCTABLEVM,func,buff,FOB_DC_TABLE*sizeof(WORD));

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sDCTableVm(ID=%d,bird=%d,...) %s [0x%04X,...].\n",
               FOB_ValText[func],
               ID,bird,
               STR_OkFailed(ok),
               buff[0]);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValDataReady( int ID, int bird, int func, BYTE &dr )
{
BOOL    ok;

    ok = FOB_ValByte(ID,bird,FOB_VAL_DATAREADY,func,&dr);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sDataReady(ID=%d,bird=0x%02X,...) %s [%s].\n",
               FOB_ValText[func],
               ID,FOB_Port[ID].address[bird],
               STR_OkFailed(ok),
               STR_YesNo(dr));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValDataReadyChar( int ID, int bird, int func, BYTE &drc )
{
BOOL    ok;

    ok = FOB_ValByte(ID,bird,FOB_VAL_DATAREADYCHAR,func,&drc);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sDataReadyChar(ID=%d,bird=0x%02X,...) %s [0x%02X].\n",
               FOB_ValText[func],
               ID,FOB_Port[ID].address[bird],
               STR_OkFailed(ok),
               drc);

    return(ok);
}
/******************************************************************************/

BOOL    FOB_ValOutputChange( int ID, int bird, int func, BYTE &flag )
{
BOOL    ok;

    ok = FOB_ValByte(ID,bird,FOB_VAL_OUTPUTCHANGE,func,&flag);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sOutputChange(ID=%d,bird=0x%02X,...) %s [%s].\n",
               FOB_ValText[func],
               ID,FOB_Port[ID].address[bird],
               STR_OkFailed(ok),
               STR_LockUnlock(flag));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValScale( int ID, int bird, int func, WORD &scale )
{
BOOL    ok=TRUE;
WORD    code;

    if( func == FOB_VAL_SET )
    {
        FOB_ScaleCode(scale,code);
    }

    ok = FOB_ValWord(ID,bird,FOB_VAL_POSNSCALE,func,&code);

    if( func == FOB_VAL_GET )
    {
        scale = FOB_ScaleCode(code);
    }

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sScale(ID=%d,...) %s [0x%04X] %d inch.\n",
               FOB_ValText[func],
               ID,
               STR_OkFailed(ok),
               code,scale);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValBirdRate( int ID, int bird, int func, float &rate )
{
BOOL    ok=TRUE;
WORD    code;

    if( func == FOB_VAL_SET )
    {
        code = FOB_BirdRate2Code(rate);
    }

    ok = FOB_ValWord(ID,bird,FOB_VAL_BIRDRATE,func,&code);

    if( func == FOB_VAL_GET )
    {
        rate = FOB_BirdCode2Rate(code);
    }

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sBirdRate(ID=%d) %s [%d] %.2f Hz.\n",
               FOB_ValText[func],
               ID,
               STR_OkFailed(ok),
               code,rate);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValHemisphere( int ID, int bird, int func, int &hemisphere )
{
BOOL    ok=TRUE;
WORD    code;

    if( func == FOB_VAL_SET )
    {
        FOB_HemisphereCode(hemisphere,code);
    }

    ok = FOB_ValWord(ID,bird,FOB_VAL_HEMISPHERE,func,&code);

    if( func == FOB_VAL_GET )
    {
        hemisphere = FOB_HemisphereCode(code);
    }

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sHemisphere(ID=%d,bird=%d) %s %s[0x%04X].\n",
               FOB_ValText[func],
               ID,bird,
               STR_OkFailed(ok),
               STR_TextCode(FOB_HemisphereText,hemisphere),code);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValGroupMode( int ID, int func, BYTE &flag )
{
BOOL    ok;

    ok = FOB_ValByte(ID,FOB_BIRD_NULL,FOB_VAL_GROUPMODE,func,&flag);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sGroupMode(ID=%d) %s [%s].\n",
               FOB_ValText[func],
               ID,
               STR_OkFailed(ok),
               STR_YesNo(flag));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetErrorCode( int ID, int bird, BYTE &code, BOOL &fatal )
{
BOOL    ok;

    fatal = FALSE;

    if( (ok=FOB_ValByte(ID,bird,FOB_VAL_ERRORCODE,FOB_VAL_GET,&code)) )
    {
        fatal = FOB_ErrorFatal(code);
    }
    else
    {
        code = FOB_ERROR_GENERAL;
    }

    STR_printf(ok && !fatal,FOB_debugf,FOB_errorf,"FOB_GetErrorCode(ID=%d,bird=0x%02X) %s [%s].\n",
               ID,FOB_Port[ID].address[bird],
               STR_OkFailed(ok),
               STR_TextCode(FOB_ErrorText,code));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_GetErrorCode( int ID, int bird, BYTE &code )
{
BOOL    ok,fatal;

    ok = FOB_GetErrorCode(ID,bird,code,fatal);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ValTransmitMode( int ID, int func, BYTE &mode )
{
BOOL    ok;

    ok = FOB_ValByte(ID,FOB_BIRD_NULL,FOB_VAL_TRANSMITMODE,func,&mode);

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_%sTransmitMode(ID=%d,...) %s %s[%d].\n",
               FOB_ValText[func],
               ID,
               STR_OkFailed(ok),
               STR_TextCode(FOB_TransmitModeText,mode),mode);

    return(ok);
}

/******************************************************************************/

void    FOB_PutFlockStatus( BYTE addr, BYTE status, struct FOB_FlockStatusBird *bird )
{
    memset(bird,NUL,sizeof(struct FOB_FlockStatusBird));
    bird->addr  = addr;
    bird->awake = (status & FOB_FLOCKSTATUS_RUNNING ) != 0;
    bird->bird  = (status & FOB_FLOCKSTATUS_SENSOR  ) != 0;
    bird->ERT   = (status & FOB_FLOCKSTATUS_ERT     ) != 0;
    bird->xmit  = (status & FOB_FLOCKSTATUS_TRANSMIT) != 0;
}

/******************************************************************************/

int     FOB_GetFlockStatus( int ID, struct FOB_FlockStatusBird status[] )
{
BOOL    ok=TRUE;
BYTE    buffer[FOB_FBB_NORMAL],addr;
int     bird;

    if( !FOB_GetFlockStatus(ID,buffer) )
    {
        return(FALSE);
    }

    for( addr=1,bird=0; (bird < FOB_FBB_NORMAL); bird++,addr++ )
    {
        if( (buffer[bird] & FOB_FLOCKSTATUS_USED) == 0 )
        {
            break;
        }

        FOB_PutFlockStatus(addr,buffer[bird],&status[bird]);
    }

    return(ok ? bird : FOB_BIRD_NULL);
}

/******************************************************************************/

int     FOB_PutFBB( int ID, struct FOB_StatusFBB *status, BOOL &MS, FOB_BirdFBB fbb[] )
{
int     bird,birds;
BYTE    addr;
WORD    flag;

    MS = (status->mode == FOB_FBB_MASTERSLAVE);

    for( flag=1,addr=1,birds=0,bird=0; (bird < FOB_FBB_NORMAL); bird++ )
    {
        flag = (flag << 1);

        if( (status->devices & flag) == 0 )
        {
            FOB_debugf("FOB_PutFBB(ID=%d) 0x%02X[%02d] Not Used.\n",ID,addr,bird);
            continue;
        }

        fbb[bird].addr = addr;
        fbb[bird].dependent = (status->dependent & flag) != 0;
        FOB_debugf("FOB_PutFBB(ID=%d) Addr=0x%02X[%02d] dependent=%1.1s\n",ID,addr,bird,STR_YesNo(fbb[bird].dependent));

        birds++;
    }

    return(birds);
}

/******************************************************************************/

int     FOB_GetFBB( int ID, BOOL &MS, struct FOB_BirdFBB fbb[] )
{
BOOL    ok;
int     birds=FOB_BIRD_NULL;
struct  FOB_StatusFBB  status;

    if( (ok=FOB_ValBuff(ID,FOB_BIRD_NULL,FOB_VAL_AUTOCONFIG,FOB_VAL_GET,&status,sizeof(struct FOB_StatusFBB))) )
    {
        birds = FOB_PutFBB(ID,&status,MS,fbb);
    }

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_GetFBB(ID=%d) %s.\n",ID,STR_OkFailed(ok));

    return(birds);
}

/******************************************************************************/

BOOL    FOB_SetFBB( int ID, BYTE birds )
{
BOOL    ok;

    if( (ok=FOB_ValByte(ID,FOB_BIRD_NULL,FOB_VAL_AUTOCONFIG,FOB_VAL_SET,&birds)) )
    {
        TIMER_delay(FOB_WAIT_AUTOCONFIG);        // Wait for FOB to auto-configure...
    }

    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_SetFBB(ID=%d,birds=%d) %s.\n",
               ID,birds,
               STR_OkFailed(ok));

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ResetFOB( int ID, BOOL nowait )
{
BOOL    ok;

    if( !FOB_Check(ID) )
    {
        return(FALSE);
    }

    if( (ok=FOB_CmdResetViaRTS(ID,nowait)) )     // Reset Primary bird via RS232 RTS line.
    {
        if( FOB_MultiBirdPort(ID) )              // Secondary birds present?
        {
            ok = FOB_CmdResetViaFBB(ID);         // Reset Secondary birds via FBB interface.
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ResetFOB( int ID )
{
BOOL    ok;

    ok = FOB_ResetFOB(ID,FALSE);

    return(ok);
}

/******************************************************************************/

BOOL    FOB_ResetCOM( int ID )
{
BOOL    ok,complete;
BYTE    buff[COM_BUFF];

    if( !FOB_Check(ID) )
    {
        return(FALSE);
    }

    ok = COM_Read(FOB_Port[ID].comH,buff,sizeof(buff),complete,0L);

    return(ok);
}

/******************************************************************************/

