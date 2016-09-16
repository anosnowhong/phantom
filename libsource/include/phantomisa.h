/******************************************************************************/
/* Class implementation of PHANTOM ISA interface.                             */
/******************************************************************************/

class   PHANTOMISA
{
private:
    int Address;

    BOOL OpenFlag;

    STRING ObjectName;

#define PHANTOMISA_CHANNEL   6
    long EncLast[PHANTOMISA_CHANNEL];
    long EncTurn[PHANTOMISA_CHANNEL];

public:

    PHANTOMISA( int address, char *name );
    PHANTOMISA( int address );
   ~PHANTOMISA( void );

    void Init( int address, char *name );

    BOOL Open( void );
    void Close( void );

    void Port( int port, BYTE data );
    BYTE Port( int port );

    BOOL Controller( BOOL enable );

    BOOL Safe( void );
    BOOL Activated( void );

    void EncoderReset( int channel );
    long EncoderGet( int channel );

    void MotorReset( int channel );
    void MotorSet( int channel, long units );
};

/******************************************************************************/

#define PHANTOM_PORT_CONTROLLER        12

#define PHANTOM_STATUS_ACTIVATED     0x01
#define PHANTOM_STATUS_STYLUS        0x02

#define PHANTOM_CONTROLLER_MASK      0x3F
#define PHANTOM_CONTROLLER_DISABLE   0x80
#define PHANTOM_CONTROLLER_ENABLE    0x00

/******************************************************************************/

