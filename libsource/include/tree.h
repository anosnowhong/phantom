/******************************************************************************/
/*                                                                            */
/* MODULE  : TREE.h                                                           */
/*                                                                            */
/* PURPOSE : Functions for dealing with (segmented) trees.                    */
/*                                                                            */
/* DATE    : 30/Nov/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 30/Nov/2000 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define TREE_SEGMENT_MAX    90         // Maximum number of segments in tree.
#define TREE_SEGMENT_NULL   -1         // NULL segment value.
#define TREE_SEGMENT_ROOT   "ROOT"     // Segment is root (no parent).

/******************************************************************************/

struct  TREE_Stack                     // Stack operation.
{
    BOOL operate;
    int  segment;
};

/******************************************************************************/

struct  TREE_Segment                   // Segment details...
{
    STRING    segment_name;            // Segment name string.
    int       segment;                 // Segment index (linearized tree).

    STRING    parent_name;             // Segment parent node.
    int       parent;                  // Parent segment (root) index.

    STRING    file;                    // Configuration file.

    int       children;                // Number of child segments.
    int       child[TREE_SEGMENT_MAX]; // Array of child segment indexes.

    int       mirror;                  // Mirror segment for symmetrical trees.

    BOOL      done;                    // Flag for linearizing tree.

    struct TREE_Stack push;            // Do we need to push state?
    struct TREE_Stack pop;             // Do we need to pop state?

#define TREE_DATA    5120
    UCHAR     data[TREE_DATA];         // Application-specific data.
};

#define TREE_ENDOFLIST    { "",0,"",0,"" }
#define TREE_ROOT         0
#define TREE_ERROR       -1

/******************************************************************************/

int     TREE_messgf( const char *mask, ... );
int     TREE_errorf( const char *mask, ... );
int     TREE_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    TREE_EOL( struct TREE_Segment list[], int segment );
char   *TREE_Segment( struct TREE_Segment list[], int segment );
void    TREE_Copy( struct TREE_Segment dest[], struct TREE_Segment srce[], int indx[] );
void    TREE_Copy( struct TREE_Segment dest[], struct TREE_Segment srce[] );
BOOL    TREE_Push( struct TREE_Segment list[], int segment );
BOOL    TREE_Pop( struct TREE_Segment list[], int segment );
int     TREE_Count( struct TREE_Segment list[], char *segment_name );
BOOL    TREE_Init( struct TREE_Segment list[] );

int     TREE_Count( struct TREE_Segment list[], int mode );
#define TREE_COUNT        0
#define TREE_COUNT_PUSH   1
#define TREE_COUNT_POP    2
#define TREE_COUNT_DONE   3

int     TREE_Count( struct TREE_Segment list[] );
int     TREE_PushPop( struct TREE_Segment list[], int child, int parent );
int     TREE_NextChild( struct TREE_Segment list[], int parent );
int     TREE_PreviousParent( struct TREE_Segment list[], int child );
BOOL    TREE_Names( struct TREE_Segment list[], struct STR_TextItem name[] );
int     TREE_Find( struct STR_TextItem name[], char *segment_name );
BOOL    TREE_Parents( struct TREE_Segment list[], struct STR_TextItem name[] );
BOOL    TREE_Children( struct TREE_Segment list[] );
BOOL    TREE_Nodes( struct TREE_Segment list[], struct STR_TextItem name[] );
BOOL    TREE_Linearize( struct TREE_Segment list[], int indx[] );
void    TREE_List( struct TREE_Segment list[], struct STR_TextItem name[], PRINTF prnf );
int     TREE_Setup( struct TREE_Segment list[], struct STR_TextItem name[] );
int     TREE_Setup( struct TREE_Segment list[], struct STR_TextItem name[], BOOL reorganize );
int     TREE_Setup( struct TREE_Segment list[], struct STR_TextItem name[], int indx[] );
BOOL    TREE_Mirror( struct TREE_Segment list[], struct STR_TextItem name[] );

/******************************************************************************/

extern  struct  TREE_Segment  TREE_TestList[TREE_SEGMENT_MAX+1];
extern  struct  STR_TextItem  TREE_TestName[TREE_SEGMENT_MAX+1];

/******************************************************************************/

#define STACK_MAX     100         // Number of entries in stack.

BOOL    STACK_push( int value );
BOOL    STACK_pop( int &value );

/******************************************************************************/

