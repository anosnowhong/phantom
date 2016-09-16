/******************************************************************************/

class   MOTORDC
{
private:
    int Type;
#define MOTORDC_TYPE_MAXON_RE35   0
#define MOTORDC_TYPE_MAXON_RE25   1

    CONTROLLER *Controller;
    int Channel;

    double UnitsPerNm;

    STRING ObjectName;

public:

    MOTORDC( int type, CONTROLLER *controller, int channel, char *name );
    MOTORDC( int type, CONTROLLER *controller, int channel );
   ~MOTORDC( void ); 

    void Init( int type, CONTROLLER *controller, int channel, char *name );

    BOOL Open( void );
    void Close( void );

    void Reset( void );
    void Torque( double Nm );
    void Units( long units );
};

/******************************************************************************/

