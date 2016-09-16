/******************************************************************************/

BOOL    FOB_OpenParameters( int birdsinflock, int birdsonport, BYTE &datamode, BYTE &groupmode, BOOL &multiport, BOOL &multibird )
{
BOOL    ok=TRUE;

    multiport = FALSE;
    multibird = FALSE;

    if( ((birdsinflock == 0) || (birdsinflock > 1)) && (birdsonport == 1) )
    {                                                 // Multiple birds on multiple ports...
        multiport = TRUE;
        multibird = TRUE;
        datamode = FOB_DATAMODE_STREAM;               // Only STREAM mode supported...
        groupmode = FOB_GROUPMODE_OFF;                // Individual ports so not GROUP mode...
    }
    else
    if( birdsinflock == birdsonport )                 // Single port, one or more birds...
    {
        if( birdsonport > 1 )                         // Multi-bird setup...
        {
             multibird = TRUE;

             if( datamode == FOB_DATAMODE_STREAM )    // Must use GROUP mode for STREAMing...
             {
                 groupmode = FOB_GROUPMODE_ON;
             }
        }

        if( datamode == FOB_DATAMODE_DRC )            // GROUP mode doesn't work with DRC...
        {
            groupmode = FOB_GROUPMODE_OFF;
        }
    }
    else
    {
        ok = FALSE;
    }

    return(ok);
}

/******************************************************************************/
/* Base Open function. All other Open functions eventually call this one...   */
/******************************************************************************/

#define FOB_OPEN_PARAMETERS  0         // FOB Open steps...
#define FOB_OPEN_HANDLEGET   1
#define FOB_OPEN_COMOPEN     2
#define FOB_OPEN_COMSETUP    3
#define FOB_OPEN_HANDLESET   4
#define FOB_OPEN_COMRESET    5
#define FOB_OPEN_FOBRESET    6
#define FOB_OPEN_FOBCONFIG   7
#define FOB_OPEN_FOBERROR    8
#define FOB_OPEN_FOBSTATUS   9
#define FOB_OPEN_OBJECTS    10
#define FOB_OPEN_DONE       11

char   *FOB_OpenStep[] = { "Parameters","HandleGet","ComOpen","ComSetup","HandleSet","ComReset","FOBReset","FOBError","FOBConfig","FOBStatus","Objects","Done",NULL };
BOOL    FOB_Hardware[] = { FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,FALSE,TRUE,TRUE,TRUE,FALSE,FALSE };

int     FOB_Open( int comX, int birdsinflock, int birdsonport, BYTE addr, long baud, double freq, WORD scale, BYTE &datamode, BYTE &groupmode, BYTE dataformat[], BYTE hemisphere[] )
{
int     ID=FOB_PORT_INVALID;
int     step,retry,bird;
BOOL    ok,fatal,multiport,multibird;
int     comH=COM_INVALID;
BYTE    code;

    // Check that FOB API is running...
    if( !FOB_API_check() )
    {
        return(FOB_PORT_INVALID);
    }

    FOB_debugf("FOB_Open(COM%d,birds=%d/%d,base=0x%02X,%ld baud,...)\n",comX,birdsonport,birdsinflock,addr,baud);

    // Go through each step of FOB_Open() and continue only if successful...
    for( ok=TRUE,retry=0,step=FOB_OPEN_PARAMETERS; ((step <= FOB_OPEN_DONE) && ok); )
    {
        switch( step )
        {
            case FOB_OPEN_PARAMETERS :           // Set defaults and then check parameters...
               ok = FOB_OpenParameters(birdsinflock,birdsonport,datamode,groupmode,multiport,multibird);
               break;

            case FOB_OPEN_HANDLEGET :            // Find a free handle ID...
               if( (ID=FOB_Free()) == FOB_PORT_INVALID )
               {
                   FOB_errorf("FOB_Open(...) No free handles.\n");
                   ok = FALSE;
                   break;
               }

               FOB_debugf("ID=%d\n",ID);
               FOB_Init(ID);                     // Clear the handle item...
               break;

            case FOB_OPEN_COMOPEN :              // Open RS232 communications port... 
               ok = ((comH=COM_Open(comX)) != COM_INVALID);
               STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_Open(...) Open COM%d %s.\n",comX,STR_OkFailed(ok));
               break;

            case FOB_OPEN_COMSETUP :             // Set FOB RS232 characteristics...
               ok = COM_Setup(comH,baud,FOB_DCB);
               STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_Open(...) Setup COM%d %s.\n",comX,STR_OkFailed(ok));
               break;

            case FOB_OPEN_HANDLESET :            // Setup FOB handle structure with information...
               FOB_Port[ID].used = TRUE;
               FOB_Port[ID].started = FALSE;
               FOB_Port[ID].comH = comH;
               FOB_Port[ID].comX = comX;
               FOB_Port[ID].baudrate = baud;
               FOB_Port[ID].birdsinflock = birdsinflock;
               FOB_Port[ID].birdsonport = birdsonport;
               FOB_Port[ID].scale = scale;

               if( addr == FOB_ADDR_NULL )
               {
                   addr = 0x01;
               }

               for( bird=0; (bird < birdsonport); bird++ )
               {
                   FOB_Port[ID].dataformat[bird] = dataformat[bird];
                   FOB_Port[ID].hemisphere[bird] = hemisphere[bird];
                   FOB_Port[ID].address[bird] = addr++;
               }
               break;

            case FOB_OPEN_COMRESET :             // Reset COM buffer...
                ok = FOB_ResetCOM(ID);
                break;

            case FOB_OPEN_FOBRESET :             // Reset FOB hardware...
               if( retry > 0 )
               {
                   ok = FOB_ResetFOB(ID); 
               }
               break;

            case FOB_OPEN_FOBCONFIG :            // Set FOB configuration...
               ok = FOB_CmdConfig(ID);
               break;

            case FOB_OPEN_FOBERROR :             // Check FOB error status...
break;
               if( !(ok=FOB_GetErrorCode(ID,FOB_BIRD_NULL,code,fatal)) )
               {
                   break;
               }

               STR_printf(fatal,FOB_errorf,FOB_debugf,"FOB_Open(...) %s.\n",FOB_Error(code));

               if( fatal )
               {
                   ok = FALSE;
               }
               break;

            case FOB_OPEN_FOBSTATUS :            // Check bird status...
               if( !(ok=FOB_BirdStatusOK(ID)) )
               {
                   FOB_errorf("FOB_Open(...) Status Error.\n");
               }
ok = TRUE;
               break;

            case FOB_OPEN_OBJECTS :
               FOB_Port[ID].session = new TIMER(STR_stringf("FOB[%d] Session",ID));
               FOB_Port[ID].requestrate = new TIMER_Frequency(STR_stringf("FOB[%d] RequestRate",ID));
               FOB_Port[ID].looprate = new TIMER_Frequency(STR_stringf("FOB[%d] LoopRate",ID));
               FOB_Port[ID].framerate = new TIMER_Frequency(STR_stringf("FOB[%d] FrameRate",ID));
               FOB_Port[ID].frametime = new TIMER_Interval(STR_stringf("FOB[%d] FrameTime",ID));
               FOB_Port[ID].framelast = new TIMER(STR_stringf("FOB[%d] FrameLast",ID));
               FOB_Port[ID].framewait = new DATAPROC(STR_stringf("FOB[%d] FrameWait",ID));
               break;

            case FOB_OPEN_DONE :
               FOB_Use(ID);                      // Set global variable for default ID...
               break;
        }

        STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_Open(...) %s %s.\n",FOB_OpenStep[step],STR_OkFailed(ok));

        if( !ok && FOB_Hardware[step] )          // FOB hardware failed, try FOB reset...
        {
            if( ++retry <= 1 )
            {
                step = FOB_OPEN_FOBRESET;
                ok = TRUE;
            }
        }
        else
        {
            step++;
        }
    }
    
    if( !ok )                          // FOB open failed, so clean up...
    {
        COM_Close(comH);               // Close COM handle.
        FOB_Init(ID);                  // Clear FOB handle.
        ID = FOB_PORT_INVALID;
    }

    return(ID);
}


/******************************************************************************/

int    _FOB_Open( int comX, int birdsinflock, int birdsonport, BYTE addr, long baud, double freq, WORD scale, BYTE &datamode, BYTE &groupmode, BYTE dataformat[], BYTE hemisphere[] )
{
int     ID=FOB_PORT_INVALID;
int     step,retry,bird;
BOOL    ok,fatal,multiport,multibird;
int     comH=COM_INVALID;
BYTE    code;

//  Check that FOB API is running...
    if( !FOB_API_check() )
    {
        return(FOB_PORT_INVALID);
    }

    FOB_debugf("FOB_Open(COM%d,birds=%d/%d,base=0x%02X,%ld baud,...)\n",comX,birdsonport,birdsinflock,addr,baud);

//  Go through each step of FOB_Open() and continue only if successful...
    for( ok=TRUE,retry=0,step=FOB_OPEN_PARAMETERS; ((step <= FOB_OPEN_DONE) && ok); )
    {
        switch( step )
        {
            case FOB_OPEN_PARAMETERS :           // Set defaults and then check parameters...
               ok = FOB_OpenParameters(birdsinflock,birdsonport,datamode,groupmode,multiport,multibird);
               break;

            case FOB_OPEN_HANDLEGET :            // Find a free handle ID...
               if( (ID=FOB_Free()) == FOB_PORT_INVALID )
               {
                   FOB_errorf("FOB_Open(...) No free handles.\n");
                   ok = FALSE;
                   break;
               }

               FOB_debugf("ID=%d\n",ID);
               FOB_Init(ID);                     // Clear the handle item...
               break;

            case FOB_OPEN_COMOPEN :              // Open RS232 communications port... 
               ok = ((comH=COM_Open(comX)) != COM_INVALID);
               STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_Open(...) Open COM%d %s.\n",comX,STR_OkFailed(ok));
               break;

            case FOB_OPEN_COMSETUP :             // Set FOB RS232 characteristics...
               ok = COM_Setup(comH,baud,FOB_DCB);
               STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_Open(...) Setup COM%d %s.\n",comX,STR_OkFailed(ok));
               break;

            case FOB_OPEN_HANDLESET :            // Setup FOB handle structure with information...
               FOB_Port[ID].used = TRUE;
               FOB_Port[ID].started = FALSE;
               FOB_Port[ID].comH = comH;
               FOB_Port[ID].comX = comX;
               FOB_Port[ID].baudrate = baud;
               FOB_Port[ID].birdsinflock = birdsinflock;
               FOB_Port[ID].birdsonport = birdsonport;
               FOB_Port[ID].scale = scale;

               if( addr == FOB_ADDR_NULL )
               {
                   addr = 0x01;
               }

               for( bird=0; (bird < birdsonport); bird++ )
               {
                   FOB_Port[ID].dataformat[bird] = dataformat[bird];
                   FOB_Port[ID].hemisphere[bird] = hemisphere[bird];
                   FOB_Port[ID].address[bird] = addr++;
               }
               break;

            case FOB_OPEN_COMRESET :             // Reset COM buffer...
                ok = FOB_ResetCOM(ID);
                break;

            case FOB_OPEN_FOBRESET :             // Reset FOB hardware...
               if( retry > 0 )
               {
                   ok = FOB_ResetFOB(ID); 
               }
               break;

            case FOB_OPEN_FOBERROR :             // Check FOB error status...
               if( !(ok=FOB_GetErrorCode(ID,FOB_BIRD_NULL,code,fatal)) )
               {
                   break;
               }

               STR_printf(fatal,FOB_errorf,FOB_debugf,"FOB_Open(...) %s.\n",FOB_Error(code));

               if( fatal )
               {
                   ok = FALSE;
               }
               break;

            case FOB_OPEN_FOBCONFIG :            // Set FOB configuration...
               ok = FOB_CmdConfig(ID);
               break;

            case FOB_OPEN_FOBSTATUS :            // Check status of flock...
               if( !(ok=FOB_BirdStatusOK(ID)) )
               {
                   FOB_errorf("FOB_Open(...) Status Error.\n");
               }
               break;

            case FOB_OPEN_DONE :
               FOB_Use(ID);                      // Set global variable for default ID...
               break;
        }

        STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_Open(...) %s %s.\n",FOB_OpenStep[step],STR_OkFailed(ok));

        if( !ok && FOB_Hardware[step] )          // FOB hardware failed, try FOB reset...
        {
            if( ++retry <= 1 )
            {
                step = FOB_OPEN_FOBRESET;
                ok = TRUE;
            }
        }
        else
        {
            step++;
        }
    }
    
    if( !ok )                          // FOB open failed, so clean up...
    {
        COM_Close(comH);               // Close COM handle.
        FOB_Init(ID);                  // Clear FOB handle.
        ID = FOB_PORT_INVALID;
    }

    return(ID);
}


/******************************************************************************/

BOOL    FOB_Open( int comX[], int birds, BYTE addr, long baud, double freq, WORD scale, BYTE &datamode, BYTE &groupmode, BYTE dataformat[], BYTE hemisphere[] )
{
BOOL    ok=TRUE;
int     fob,ID,ports,birdsinflock=birds,birdsperport,portbird,flockbird;
BYTE    df[FOB_BIRD_MAX],hs[FOB_BIRD_MAX];

//  Check that FOB API is running...
    if( !FOB_API_check() )
    {
        return(FALSE);
    }

//  Count total number of ports...
    for( ports=0,fob=0; (fob < FOB_PORT_MAX); fob++ )
    {
        if( comX[fob] == 0 ) // Any more COM ports?
        {
            break;
        }

        ports++;
    }

    if( ports == 0 )         // No COM ports is just plain wrong...
    {
        FOB_errorf("FOB_Open(...) No COM ports specified.\n");
        ok = FALSE;
    }
    else
    if( ports == 1 )         // All birds on a single COM port is fine...
    {
        birdsperport = birds;
    }
    else
    if( ports != birds )     // Otherwise, each bird needs its own COM port...
    {
        FOB_errorf("FOB_Open(...) Multiple COM ports must match birds.\n");
        ok = FALSE;
    }
    else                     // Multiple COM ports, 1 bird per port...
    {
        birdsperport = 1;
    }

    if( !ok )
    {
        return(FALSE);
    }

//  Save bird and port numbers in global variables...
    FOB_PortCount = ports;
    FOB_BirdCount = birds;

    FOB_debugf("FOB_Open(...) ports=%d birds=%d.\n",FOB_PortsInFlock(),FOB_BirdsInFlock());

    if( addr == FOB_ADDR_NULL )
    {
        addr = 0x01;
    }

//  Open each FOB on the appropriate COM port...
    for( ok=TRUE,flockbird=0,fob=0; ((fob < ports) && ok); fob++ )
    {
        for( portbird=0; (portbird < birdsperport); portbird++,flockbird++ )
        {
            df[portbird] = dataformat[flockbird];
            hs[portbird] = hemisphere[flockbird];
        }

        if( (ID=FOB_Open(comX[fob],birdsinflock,birdsperport,addr,baud,freq,scale,datamode,groupmode,df,hs)) == FOB_PORT_INVALID )
        {
            FOB_errorf("FOB_Open(...) FOB[%d] Failed.\n",fob);
            ok = FALSE;
            continue;
        }

        FOB_debugf("FOB_Open(...) FOB[%d].ID=%d COM%d %d birds.\n",fob,ID,comX[fob],birdsperport);

//      Set total number of birds in flock for first (MASTER) port only...
        birdsinflock = 0;

//      Increment bird address...
        addr += (BYTE)birdsperport;
    }

//  Special processing for multiple ports...
    if( ports > 1 )
    {
        FOB_Use(FOB_PORT_ALL);    // Set default handle ID to use all ports...

        if( !ok )                 // Failed to open all ports, so clean up...
        {
            FOB_Close();
        }
    }

    return(ok);
}

/******************************************************************************/

BOOL    FOB_Open( char *cnfg )
{
BOOL    ok;

    // Load FOB parameters from configuration file...
    if( !FOB_CnfgLoad(cnfg) )
    {
        return(FALSE);
    }

    FOB_debugf("FOB_Open(%s)\n",cnfg);
    CONFIG_list(CONFIG_FLAG_READ,TRUE,FOB_debugf);

    // Open FOB port(s) with loaded parameters...
    ok = FOB_Open(FOB_comX,            // Array of COM ports (zero terminated)...
                  FOB_birds,           // Number of birds in flock.
                  FOB_ADDR_NULL,       // Addres of first bird in flock (default 0x01).
                  FOB_baudrate,        // Baud rate for COM port.
                  FOB_framerate,       // Frame rate for measurement (Hz).
                  FOB_scale,           // Scale for positions (inches).
                  FOB_datamode,        // Data mode for collecting points.
                  FOB_groupmode,       // Group mode.
                  FOB_dataformat,      // Array for data formats (per bird).
                  FOB_hemisphere);     // Array for operating hemisphere (per bird).

    return(ok);
}

/******************************************************************************/

void    FOB_Close( int ID )
{
BOOL    ok;

    // Multiple ports, so re-enter function for each port...
    if( ID == FOB_PORT_ALL )
    {
        for( ID=FOB_BirdsInFlock()-1; (ID >= 0); ID-- )
        {
            FOB_Close(ID);
        }

        return;
    }

    // Check if API started and handle ID is okay...
    if( !FOB_Check(ID) )
    {
        return;
    }

    // First, make sure FOB is stopped (not flying)...
    FOB_Stop(ID);

    // Put flock to sleep...
    ok = FOB_CmdSleep(ID);
    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_Close(ID=%d) Sleep %s.\n",ID,STR_OkFailed(ok));

     // Reset FOB (TRUE = no wait)...
    ok = FOB_ResetFOB(ID,TRUE);
    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_Close(ID=%d) Reset %s.\n",ID,STR_OkFailed(ok));

    // Close FOB RS232 port...
    ok = COM_Close(FOB_Port[ID].comH);
    STR_printf(ok,FOB_debugf,FOB_errorf,"FOB_Close(ID=%d) Close COM%d %s.\n",ID,FOB_Port[ID].comX,STR_OkFailed(ok));

    // Delete objects...
    delete FOB_Port[ID].session;
    delete FOB_Port[ID].requestrate;
    delete FOB_Port[ID].looprate;
    delete FOB_Port[ID].framerate;
    delete FOB_Port[ID].frametime;
    delete FOB_Port[ID].framelast;
    delete FOB_Port[ID].framewait;

    // Clear the handle...
    FOB_Init(ID);
}

/******************************************************************************/

void    FOB_CloseAll( void )
{
    FOB_Close(FOB_PORT_ALL);
}

/******************************************************************************/

