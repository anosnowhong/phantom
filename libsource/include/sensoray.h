/******************************************************************************/
/*                                                                            */
/* MODULE  : SENSORAY.cpp                                                     */
/*                                                                            */
/* PURPOSE : Sensoray 626 MultiFunction I/O card - API                        */
/*                                                                            */
/* DATE    : 01/Aug/2002                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 01/Aug/2002 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 11/Mar/2004 - Changes to allow multiple cards to be addressed.   */
/*                                                                            */
/* V2.0  JNI 26/Apr/2007 - Encoder support (moved from CONTROLLER module).    */
/*                                                                            */
/*       JNI 26/Apr/2007 - Map two virtual cards onto a single card.          */
/*                                                                            */
/* V3.0  JNI 17/Jul/2013 - Added support for Sensoray 826 (S826) card.        */
/*                                                                            */
/* V3.1  JNI 15/Apr/2015 - Ongoing development of S826 interface for 3BOT.    */
/*                                                                            */
/******************************************************************************/

#ifndef SENSORAY_H
#define SENSORAY_H

/******************************************************************************/

#include "WIN626.H"          // S626 header file (library: win626.obj)
#include "826api.h"       // S826 header file (library: S826.lib)

/******************************************************************************/

int     SENSORAY_messgf( const char *mask, ... );
int     SENSORAY_errorf( const char *mask, ... );
int     SENSORAY_debugf( const char *mask, ... );

/******************************************************************************/

BOOL    SENSORAY_API_start( PRINTF messgf, PRINTF errorf, PRINTF debugf );
void    SENSORAY_API_stop( void );
BOOL    SENSORAY_API_check( void );

/******************************************************************************/

#define SENSORAY_CHANNEL_INVALID -1

/******************************************************************************/
/* S626-specific definitions.                                                 */
/******************************************************************************/

#define S626_DAC_CHANNELS     4
#define S626_ADC_CHANNELS    16
#define S626_DIO_CHANNELS    48
#define S626_ENC_CHANNELS     6

#define S626_DAC_BITS        14
#define S626_DAC_UMIN     -8191
#define S626_DAC_UMAX      8191
#define S626_DAC_UZERO        0
#define S626_DAC_URANGE   16382
#define S626_DAC_VMIN     -10.0
#define S626_DAC_VMAX      10.0
#define S626_DAC_VRANGE    20.0

#define S626_ENC_BITS        24

#define S626_ADC_BITS        16

/******************************************************************************/
/* S826-specific definitions.                                                 */
/******************************************************************************/



/******************************************************************************/

extern  struct  STR_TextItem  S626_ErrorText[];


/******************************************************************************/

#define SENSORAY_DAC_CHANNELS     8
#define SENSORAY_ADC_CHANNELS    16
#define SENSORAY_ENC_CHANNELS     6

#define SENSORAY_ADC_EOL         -1

#define SENSORAY_ADC_GAIN_5VOLT   0
#define SENSORAY_ADC_GAIN_10VOLT  1

/******************************************************************************/

struct SENSORAY_ADC_Session
{
    BOOL   Started;
    int    Count;
    double Frequency;
    int    Channels[SENSORAY_ADC_CHANNELS];
    BYTE   Gains[SENSORAY_ADC_CHANNELS];
    double VoltsPerUnit[SENSORAY_ADC_CHANNELS];
};

#define SENSORAY_SUBADDRESS_MAX 3

struct SENSORAY_SubAddress
{
    BOOL InUse;
    struct SENSORAY_ADC_Session ADC;
};

struct SENSORAY_Handle
{
    BOOL InUse;

    BYTE CardType;
#define SENSORAY_CARDTYPE_NONE  0
#define SENSORAY_CARDTYPE_S626  1



    DWORD CardAddress;

    int CardHandle;

    struct SENSORAY_SubAddress SubAddress[SENSORAY_SUBADDRESS_MAX];
};

/******************************************************************************/

#define SENSORAY_HANDLE_MAX       8
#define SENSORAY_HANDLE_INVALID  -1

void    SENSORAY_HandleClear( int handle );
void    SENSORAY_HandleInit( void );
int     SENSORAY_HandleFind( DWORD address );
int     SENSORAY_HandleAvailable( void );
void    SENSORAY_HandleClose( void );
void    SENSORAY_HandleClose( int handle );
BOOL    SENSORAY_HandleInUse( int handle );
void    SENSORAY_HandleClose( DWORD CardAddress, BYTE SubAddress );
int     SENSORAY_HandleOpen( DWORD &CardAddress, BYTE SubAddress, void (*ISR)( void ) );
BOOL    SENSORAY_HandleValid( int handle );
void    SENSORAY_HandlePrint( int handle );
void    SENSORAY_HandlePrint( void );

BOOL    S626_HandleOpen( int handle, DWORD &CardAddress, BYTE SubAddress, void (*ISR)( void ) );



/******************************************************************************/

BOOL    SENSORAY_Error( int handle, char *text, int &rc );
BOOL    SENSORAY_Error( int handle, char *text );

/******************************************************************************/

class SENSORAY
{
public:
    int    Handle;
    int    CardHandle;
    int    CardType;
    DWORD  CardAddress;
    BYTE   SubAddress;
    int    ObjectHandle;

    STRING ObjectName;

    STRING NameString;

    int    DAC_ChannelMax;
    int    ADC_ChannelMax;
    int    DIO_ChannelMax;
    int    ENC_ChannelMax;

    int    DAC_ChannelOffset;
    int    ADC_ChannelOffset;
    int    DIO_ChannelOffset;
    int    ENC_ChannelOffset;

    int    DAC_ResolutionBits;
    int    ADC_ResolutionBits;
    int    ENC_ResolutionBits;

    BOOL   ADC_Started;
    int    ADC_Count;
    int    ADC_Channels[SENSORAY_ADC_CHANNELS];
    BYTE   ADC_Gains[SENSORAY_ADC_CHANNELS];
    BYTE   ADC_CardGains[SENSORAY_ADC_CHANNELS];
    double ADC_VoltsPerUnit[SENSORAY_ADC_CHANNELS];

    double DAC_UnitsPerVolt[SENSORAY_DAC_CHANNELS];
    int    DAC_UnitsZeroOffset[SENSORAY_DAC_CHANNELS];

    DWORD  ENC_PreLoadValue;
    BOOL   ENC_OpenFlag[SENSORAY_ENC_CHANNELS];
    void (*ENC_IndexCallback)( int channel, long counts );

    BOOL   OpenFlag;

    TIMER CriticalSectionWaitTimer;

    SENSORAY( DWORD address, BYTE subaddress, char *name );
    SENSORAY( DWORD address, BYTE subaddress );
    SENSORAY( DWORD address, char *name );
    SENSORAY( DWORD address );
    SENSORAY( void );
   ~SENSORAY( void );

    void Init( DWORD address, BYTE subaddress, char *name );

    BOOL Error( char *text, int &rc );
    BOOL Error( char *text );

    char *Name( void );

    BOOL CheckOpen( char *text );

    BOOL Open( void );
    void Close( void );

    BOOL Opened( void );

    void DefaultOutput( void );

    int GetCardType( void );
    int GetCardHandle( void );

    int ChannelIndex( int type, int channel );
#define SENSORAY_DAC 0
#define SENSORAY_ADC 1
#define SENSORAY_DIO 2
#define SENSORAY_ENC 3

    BOOL DAC_ChannelValid( int channel );
    BOOL ADC_ChannelValid( int channel );
    BOOL DIO_ChannelValid( int channel );
    BOOL ENC_ChannelValid( int channel );

    BOOL DAC_ChannelIndex( int &channel );
    BOOL ADC_ChannelIndex( int &channel );
    BOOL DIO_ChannelIndex( int &channel );
    BOOL ENC_ChannelIndex( int &channel, int &counter );

    WORD RegRead( WORD addr );
    void RegWrite( WORD addr, WORD value );

    WORD DIOWriteBankGet( WORD group );
    void DIOWriteBankSet( WORD group, WORD states );

    WORD DIOReadBank( WORD group );

    void DIO_Init( void );

    BOOL S626_DIO_Channel( int &channel, WORD &group, WORD &mask );


    BOOL S626_DIO_Get( int channel, int dio );

    BOOL DIO_Get( int channel, int dio );
#define SENSORAY_DIO_OUTPUT  0
#define SENSORAY_DIO_INPUT   1

    void S626_DO_Set( int channel, BOOL state );


    void DO_Set( int channel, BOOL state );
    BOOL DO_Get( int channel );
    BOOL DI_Get( int channel );

    BOOL ResetADC( BYTE list[], BYTE gain[], int count );
    BOOL ReadADC( WORD data[] );

    void WriteDAC( int channel, int units );

    int DAC_Units( int channel, double volts );
    void DAC_Init( void );
    void DAC_Write( int channel, double volts );

    void ADC_Init( void );

    BOOL ADC_Setup( int channels[], BYTE gains[], int count );
    BOOL ADC_Setup( int channels[], BYTE gains[] );
    BOOL ADC_Setup( int channels[], BYTE gain, int count );
    BOOL ADC_Setup( int channels[], BYTE gain );
    BOOL ADC_Setup5V( int channels[], int count );
    BOOL ADC_Setup5V( int channels[] );
    BOOL ADC_Setup10V( int channels[], int count );
    BOOL ADC_Setup10V( int channels[] );

    void ADC_Stop( void );

    int ADC_SignedInteger( WORD raw );
    double ADC_SignedDouble( WORD raw );
    void ADC_SampleUnits( int units[] );
    void ADC_SampleVolts( double volts[] );

    BOOL CounterModeSet( WORD chan, WORD options );
    WORD CounterModeGet( WORD chan );
    BOOL CounterEnableSet( WORD chan, WORD cond );
    BOOL CounterPreload( WORD chan, DWORD value );
    BOOL CounterLoadTrigSet( WORD chan, WORD events );
    BOOL CounterLatchSourceSet( WORD chan, WORD events );
    DWORD CounterReadLatch( WORD counter );
    BOOL CounterSoftIndex( WORD chan );
    BOOL CounterIntSourceSet( WORD chan, WORD events );

    BOOL EncoderChannel( int channel );

    BOOL S626_EncoderOpen( int counter, int multiplier );

    BOOL EncoderOpen( int channel, int multiplier );

    void S626_EncoderClose( int counter );

    void EncoderClose( int channel );

    long S626_EncoderCount( int counter );


    long EncoderCount( int channel );

    void S626_EncoderReset( int counter, long value );

    void EncoderReset( int channel, long value );
    void EncoderReset( int channel );









    void EncoderIndexSetCallback( void (*func)( int channel, long counts ) );
    void EncoderIndexEvent( int channel, long counts );

    void CriticalSectionEnter( void );
    void CriticalSectionExit( void );


};

/******************************************************************************/

void SENSORAY_ISR_Object0( void );
void SENSORAY_ISR_Object1( void );
void SENSORAY_ISR_Object2( void );
void SENSORAY_ISR_Object3( void );
void SENSORAY_ISR_Object4( void );
void SENSORAY_ISR_Object5( void );
void SENSORAY_ISR_Object6( void );
void SENSORAY_ISR_Object7( void );
void SENSORAY_ISR_Object( int object );

void SENSORAY_ISR( SENSORAY *SX26 );

/******************************************************************************/

#define   SENSORAY_OBJECT_MAX       8
#define   SENSORAY_OBJECT_INVALID  -1

void      SENSORAY_ObjectInit( void );
int       SENSORAY_ObjectOpen( SENSORAY *SX26 );
void      SENSORAY_ObjectClose( SENSORAY *SX26 );
SENSORAY *SENSORAY_ObjectSX26( int object );

/******************************************************************************/

#endif
