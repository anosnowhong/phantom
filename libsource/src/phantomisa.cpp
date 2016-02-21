/******************************************************************************/

#include <motor.h>

/******************************************************************************/

PHANTOMISA::PHANTOMISA( int address, char *name )
{
    Init(address,name);
}

/******************************************************************************/

PHANTOMISA::PHANTOMISA( int address )
{
    Init(address,NULL);
}

/******************************************************************************/

PHANTOMISA::~PHANTOMISA( void  )
{
}

/******************************************************************************/

void    PHANTOMISA::Init( int address, char *name )
{
int     channel;

    // Set general object variables...
    Address = address;
    OpenFlag = FALSE;
    memset(ObjectName,0,STRLEN);

    if( name != NULL )
    {
        strncpy(ObjectName,name,STRLEN);
    }

    for( channel=0; (channel < PHANTOMISA_CHANNEL); channel++ )
    {
        EncLast[channel] = 0;
        EncTurn[channel] = 0;
    }
}

/******************************************************************************/

BOOL    PHANTOMISA::Open( void )
{
BOOL    ok;

    if( OpenFlag )
    {
        return(TRUE);
    }

    ok = TRUE;

    OpenFlag = ok;

    return(ok);
}

/******************************************************************************/

void    PHANTOMISA::Close( void )
{
    if( !OpenFlag )
    {
        return;
    }

    OpenFlag = FALSE;
}

/******************************************************************************/

void    PHANTOMISA::Port( int port, BYTE data )
{
    if( OpenFlag )
    {
        ISA_byte(Address + port,data);
    }
}

/******************************************************************************/

BYTE    PHANTOMISA::Port( int port )
{
BYTE    data=0;

    if( OpenFlag )
    {
        data = ISA_byte(Address + port);
    }

    return(data);
}

/******************************************************************************/

BOOL    PHANTOMISA::Controller( BOOL enable )
{
BOOL    ok=TRUE;
BYTE    mask;

    mask = PHANTOM_CONTROLLER_MASK | (enable ? PHANTOM_CONTROLLER_ENABLE : PHANTOM_CONTROLLER_DISABLE);

    Port(PHANTOM_PORT_CONTROLLER,mask);

    return(ok);
}

/******************************************************************************/

BOOL    PHANTOMISA::Safe( void )
{
BOOL    safe=TRUE;

    return(safe);
}

/******************************************************************************/

BOOL    PHANTOMISA::Activated( void )
{
BOOL    activated=TRUE;
BYTE    flag;

    flag = Port(PHANTOM_PORT_CONTROLLER);

    activated = ((flag & PHANTOM_STATUS_ACTIVATED) == 0);

    return(activated);
}

/******************************************************************************/

void    PHANTOMISA::EncoderReset( int channel )
{
    // Toggle control bit to reset specific encoder...
    Port(PHANTOM_PORT_CONTROLLER,0xFF & ~(1 << channel));
    Port(PHANTOM_PORT_CONTROLLER,0xFF);

    EncLast[channel] = 0;
    EncTurn[channel] = 0;
}

/******************************************************************************/

long    PHANTOMISA::EncoderGet( int channel )
{
int     MSB,LSB;
long    EU,dEU;

    // Get 2 bytes (MSB & LSB) for 16 bit encoder reading...
    MSB = (int)Port((2*channel)+0);
    LSB = (int)Port((2*channel)+1);

    // Combine MSB & LSB to get 16 bit value...
    EU = (long)((MSB << 8) ^ LSB);

    // Change since last reading...
    dEU = EU - EncLast[channel];

    // Check if encoder has looped (gone around past it's last value)...
    if( dEU > +INT15_MAX )
    {
        EncTurn[channel]--;
    }
    else
    if( dEU < -INT15_MAX )
    {
        EncTurn[channel]++;
    }

    // Save encoder reading for next comparison...
    EncLast[channel] = EU;

    // Return corrected encoder value (adjusted for number of complete turns)...
    return(EU + (EncTurn[channel] * INT16_MAX));
}

/******************************************************************************/

void    PHANTOMISA::MotorReset( int channel )
{
    MotorSet(channel,0);
}

/******************************************************************************/

void    PHANTOMISA::MotorSet( int channel, long units )
{
int     MSB,LSB;
long    MUout;

    // Adjust for non-zero zero...
    MUout = INT15_MAX + units;

    // Split 16 bit value into 2 x 8 bit values....
    MSB = (int)((MUout >> 8) & 0xFF);
    LSB = (int)(MUout & 0xFF);

    // Write 2 bytes (MSB & LSB) for 16 bit torque value...
    Port((channel*2)+1,MSB);
    Port((channel*2)+0,LSB);
}

/******************************************************************************/

