/******************************************************************************/

#ifndef SNMS_H
#define SNMS_H

/******************************************************************************/

#define BITS_1     0x01
#define BITS_2     0x03
#define BITS_3     0x07
#define BITS_4     0x0F
#define BITS_5     0x1F
#define BITS_6     0x3F
#define BITS_7     0x7F
#define BITS_8     0xFF

/******************************************************************************/

#define SNMS_BAUD                      9600      // BAUD rate for RS232 port link.
#define SNMS_CHANNELS                  8         // Number of channels.

#define SNMS_PACKET_MAX                8         // Maximum size of SNMS packet.
#define SNMS_PACKET_PULSE              4         // Size of LabMode pulse packet.
#define SNMS_PACKET_LABMODE            3         // Size of LabMode start packet.
#define SNMS_PACKET_CONTROL            2         // Size of LabMode control packet.

#define SNMS_CONTROL_ESCAPE_1          0x35      // Control: Escape (first byte in sequence).
#define SNMS_CONTROL_ESCAPE_2          0xBA      // Control: Escape (second byte in sequence).

#define SNMS_LABMODE_START_FCB         0xAC      // First Compulsory Byte to start LabMode.
#define SNMS_LABMODE_START_SCB         0xBD      // Second Compulsory Byte to start LabMode.

#define SNMS_PACKET_TERMINATE          0x80      // Bit 7 set in last byte of LabMode packet.

#define SNMS_LABMODE_PULSE             0x40      // Bit 6 set for pulse data.
#define SNMS_LABMODE_CONTROL           0x00      // Bit 6 clear for control data.

#define SNMS_LABMODE_PADDING           0x00      // Padding byte.

#define SNMS_WIDTH_UNITS_HALFMICROSEC  0x00      // Pulse width 0.5E-6 seconds.
#define SNMS_WIDTH_UNITS_4MICROSEC     0x20      // Pulse width 4.0E-6 seconds.

#define SNMS_WIDTH_MAX_HALFMICROSEC    1.0       // Maximum width at in 0.5E-6 second units (msec).
#define SNMS_WIDTH_MAX_4MICROSEC       8.0       // Maximum width at in 4.0E-6 second units (msec).

#define SNMS_CURRENT_MAX_INT           10        // Maximum current (integer value).
#define SNMS_CURRENT_MAMP2INT          0.1       // Conversion current (real mA) to integer.

/******************************************************************************/

BOOL    SNMS_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    SNMS_API_stop( void );
BOOL    SNMS_API_check( void );

/******************************************************************************/

void    SNMS_DCB( DCB *dcb );
BOOL    SNMS_Check( void );
BOOL    SNMS_Send( BYTE *buff, int size );
BOOL    SNMS_Recv( void );
void    SNMS_Proc( void );
BYTE   *SNMS_Packet( int size );
BOOL    SNMS_Pulse( int channel, int current, int width, int units );
BOOL    SNMS_LabMode( BYTE channels[] );
BOOL    SNMS_Escape( void );

/******************************************************************************/

BOOL    SNMS_Open( int comX, BYTE channels[] );
BOOL    SNMS_Open( int comX );
void    SNMS_Close( BOOL Exit );
void    SNMS_Close( void );
BOOL    SNMS_Pulse( int channel, float current_mAmp, float width_msec );

/******************************************************************************/

#endif

