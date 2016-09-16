/*************************************************************************
Name:            ANALYSIS.H

Description:
    This file defines the header format for floating point
data files.  It also defines some floating point data constants.

Modified:

**************************************************************************/

#ifndef INCLUDE_ANALYSIS
#define INCLUDE_ANALYSIS

/**************************************************************************
  Defines
**************************************************************************/

#define GENERAL 0x20        /* General format filetype */
#define MAX_ITEMS 64
#define MAX_SUBITEMS 18
#define EXTENDED_HEADER 12345       /* Indicates new extended header format */

/**************************************************************************
 Global Structures
**************************************************************************/

typedef struct HeadFloats
	{
	char        filetype;           /* Type of file being loaded in */
	int         items;              /* Number of main data types */
	int         subitems;           /* Number of subitems in items */
	long int    numframes;          /* Number of frames of data in file */
	float       freq;               /* Frequency of the frames. */
	char        UserComments[60];  /* User comments for the file */
	char        SysComments[60];   /* System comments for the file */
	char        DescripFile[30];   /* File containing descriptions */
	int         cutoff;             /* Filtering cutoff frequency */
	char        CollTime[10];      /* Time data was collected */
	char        CollDate[10];      /* Date data was collected */
	long int    FrameSstart;        /* frame start offset */
	int         ExtendedHeader;    /* If equals EXTENDED_HEADER then a */
									/* new extended header */
	int         CharSubitems;      /* Number of subitems of type char */
	int         IntSubitems;       /* Number of subitems of size int  */
	int         DoubleSubitems;    /* Number of subitems of size double */
	int         ItemSize;          /* Size of an item including all subitems */
	char        padding[57];        /* Used to pad to 256 bytes */
	} HeadFloat;

/**************************************************************************
 External Variables
**************************************************************************/

/**************************************************************************
 Routine Definitions
**************************************************************************/

#endif
