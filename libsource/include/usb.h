/******************************************************************************/
/*                                                                            */ 
/* MODULE  : ISA.h                                                            */ 
/*                                                                            */ 
/* PURPOSE : PC ISA Bus Interface functions.                                  */ 
/*                                                                            */ 
/* DATE    : 11/May/2000                                                      */ 
/*                                                                            */ 
/* CHANGES                                                                    */ 
/*                                                                            */ 
/* V1.0  JNI 11/May/2000 - Initial development.                               */ 
/*                                                                            */ 
/******************************************************************************/

BOOL    USB_API_start(  PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    USB_API_stop( void );
BOOL    USB_API_check( void );

/******************************************************************************/

BOOL    USB_patch( void );                            // Install WindowsNT USB patch.

void    USB_word( USHORT addr, USHORT word );         // Write word to USB port.
USHORT  USB_word( USHORT addr );                      // Read word from USB port.

void    USB_byte( USHORT addr, UCHAR byte );          // Write byte to USB port.
UCHAR   USB_byte( USHORT addr );                      // Read byte from USB port.

/******************************************************************************/

int     USB_LPT( int LPTn );                          // USB LPT (pallarel) port.
#define USB_LPT_INVALID      0x0000

/******************************************************************************/
int USB_initialize();
