/////////////////////////////////////////////////////////////////////
// Polhemus Inc.,  www.polhemus.com
// © 2003 Alken, Inc. dba Polhemus, All Rights Reserved
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
//  Filename:           $Workfile: PDItracker.h $
//
//  Project Name:       Polhemus Developer Interface  
//
//  Description:        Tracker-Specific definitions
//
//  VSS $Header: /PIDevTools/Inc/PDItracker.h 11    4/27/04 12:00p Sgagnon $  
//
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
#ifndef _PDITRACKER_H_
#define _PDITRACKER_H_

/////////////////////////////////////////////////////////////////////
// Liberty Binary Mode Header Format
/////////////////////////////////////////////////////////////////////
typedef struct _BIN_HDR_STRUCT 
{
	short preamble;              
	BYTE station;
	BYTE cmd;
	BYTE err;
	BYTE reserved;
	short length;

}*LPBINHDR,BINHDR;

#define LIBERTY_PREAMBLE 0x594C
#define PATRIOT_PREAMBLE 0x4150

/////////////////////////////////////////////////////////////////////
// Liberty WhoAmI Binary Mode Header Format
/////////////////////////////////////////////////////////////////////
typedef struct _BINWHOAMI_STRUCT
{
	BYTE ucMaxSensors;
	BYTE ucTrackerType;
	BYTE ucReserved[6];

}*LPBINWHO, BINWHO;

/////////////////////////////////////////////////////////////////////
// Tracker Types
/////////////////////////////////////////////////////////////////////
typedef enum
{
	PI_TRK_LIBERTY
	, PI_TRK_PATRIOT

	, PI_TRK_UNKNOWN
	, PI_TRK_MAX = PI_TRK_UNKNOWN
} ePiTrackerType;

#define LIBERTY_MAX_SENSORS	16
#define PATRIOT_MAX_SENSORS 2

/////////////////////////////////////////////////////////////////////
// Liberty Error Codes
/////////////////////////////////////////////////////////////////////
typedef enum
{
	PI_DEVERR_NO_ERROR = 0				//	0   ' '

	// Command errors
	, PI_DEVERR_INVALID_COMMAND = 1		//	1
	, PI_DEVERR_STATION_OUT_OF_RANGE	//	2
	, PI_DEVERR_INVALID_PARAMETER		//	3
	, PI_DEVERR_TOO_FEW_PARAMETERS		//	4
	, PI_DEVERR_TOO_MANY_PARAMETERS		//	5
	, PI_DEVERR_PARAMETER_BELOW_LIMIT	//	6
	, PI_DEVERR_PARAMETER_ABOVE_LIMIT	//	7
	, PI_DEVERR_SP_COM_FAILURE			//	8
	, PI_DEVERR_SP1_INIT_FAILURE		//	9
	, PI_DEVERR_SP2_INIT_FAILURE		//	10
	, PI_DEVERR_SP3_INIT_FAILURE		//	11
	, PI_DEVERR_SP4_INIT_FAILURE		//	12
	, PI_DEVERR_SP_NONE_DETECTED		//	13
	, PI_DEVERR_SRC_INIT_FAILURE		//	14
	, PI_DEVERR_MEM_ALLOC_ERROR			//	15
	, PI_DEVERR_EXCESS_CMD_CHARS		//	16
	, PI_DEVERR_EXIT_UTH				//	17
	, PI_DEVERR_SOURCE_READ_ERROR		//	18
	, PI_DEVERR_READONLY_ERROR			//  19
	, PI_DEVERR_TEXT_MESSAGE			//  20
	, PI_DEVERR_MAP_LOAD				//  21
	, PI_DEVERR_SYNC_SENSORS			//  22
	, PI_DEVERR_FW_UPGRADE_REQ			//  23		
	, PI_DEVERR_FW_UPGRADE_REC			//  24

	// Realtime errors 
	, PI_DEVERR_RT_SRC_FAIL_a_X			//  25 'a'
	, PI_DEVERR_RT_SRC_FAIL_b_Y			//  26 'b'
	, PI_DEVERR_RT_SRC_FAIL_c_XY		//  27 'c'
	, PI_DEVERR_RT_SRC_FAIL_d_Z			//  28 'd'
	, PI_DEVERR_RT_SRC_FAIL_e_XZ		//  29 'e'
	, PI_DEVERR_RT_SRC_FAIL_f_YZ		//  30 'f'
	, PI_DEVERR_RT_SRC_FAIL_g_XYZ		//  31 'g'

	, PI_DEVERR_RT_OUT_OF_MAP_u=39		//  39  'u'

	, PI_DEVERR_RT_SRC_FAIL_A_X			//  40 'A'
	, PI_DEVERR_RT_SRC_FAIL_B_Y			//  41 'B'
	, PI_DEVERR_RT_SRC_FAIL_C_XY		//  42 'C'
	, PI_DEVERR_RT_SRC_FAIL_D_Z			//  43 'D'
	, PI_DEVERR_RT_SRC_FAIL_E_XZ		//  44 'E'
	, PI_DEVERR_RT_SRC_FAIL_F_YZ		//  45 'F'
	, PI_DEVERR_RT_SRC_FAIL_G_XYZ		//  46 'G'

	, PI_DEVERR_RT_BITERR				//  47 'I'

	, PI_DEVERR_MAX
}ePiDevError;

/////////////////////////////////////////////////////////////////////
// END $Workfile: PDItracker.h $
/////////////////////////////////////////////////////////////////////
#endif // _PDITRACKER_H_
