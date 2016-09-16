/******************************************************************************/
/*                                                                            */ 
/* MODULE  : GOGGLES.h                                                        */ 
/*                                                                            */ 
/* PURPOSE : Plato Goggles Interface API.                                     */ 
/*                                                                            */ 
/* DATE    : 18/Nov/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 18/Nov/2000 - Initial development.                               */ 
/*                                                                            */ 
/******************************************************************************/

void    GOGGLES_port( int LPTn, UCHAR mask );
UCHAR   GOGGLES_port( int LPTn );

/******************************************************************************/

#define GOGGLES_LPT_INVALID      -1
#define GOGGLES_LPT_DEFAULT       1 

#define GOGGLES_EYE_LEFT          0
#define GOGGLES_EYE_RIGHT         1
#define GOGGLES_EYE_BOTH          2
#define GOGGLES_EYE_MASK       0x03

#define GOGGLES_OPAQUE            1
#define GOGGLES_TRANSPARENT       0

/******************************************************************************/

int     GOGGLES_errorf( const char *mask, ... );
int     GOGGLES_debugf( const char *mask, ... );
int     GOGGLES_messgf( const char *mask, ... );

/******************************************************************************/

extern  struct  STR_TextItem  GOGGLES_EyeText[];
extern  struct  STR_TextItem  GOGGLES_StateText[];

/******************************************************************************/

BOOL    GOGGLES_open( int LPT );
BOOL    GOGGLES_open( void );
void    GOGGLES_close( void );

void    GOGGLES_set( int eye, UCHAR state );

UCHAR   GOGGLES_state( int eye );
void    GOGGLES_state( UCHAR state[] );
void    GOGGLES_state( UCHAR &left, UCHAR &right );
void    GOGGLES_state( UCHAR state[] );
void    GOGGLES_toggle( int eye );
void    GOGGLES_swap( void );

void    GOGGLES_left( UCHAR state );
void    GOGGLES_right( UCHAR state );
void    GOGGLES_both( UCHAR state );
void    GOGGLES_opaque( int eye );
void    GOGGLES_transparent( int eye );

void    GOGGLES_left_opaque( void );
void    GOGGLES_left_transparent( void );
void    GOGGLES_right_opaque( void );
void    GOGGLES_right_transparent( void );
void    GOGGLES_both_oqaque( void );
void    GOGGLES_both_transparent( void );

/******************************************************************************/

