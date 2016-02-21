/******************************************************************************/

class   MOTOR
{
private:
    int Type;
#define MOTOR_TYPE_MAXON_RE35   0
#define MOTOR_TYPE_MAXON_RE25   1

    CONTROLLER *Controller;
    int Channel;

    double UnitsPerNm;

    STRING ObjectName;

public:

    MOTOR( int type, CONTROLLER *controller, int channel, char *name );
    MOTOR( int type, CONTROLLER *controller, int channel );
   ~MOTOR( void ); 

    void Init( int type, CONTROLLER *controller, int channel, char *name );

    char *TypeText( void );

    long CurrentUnits;

    BOOL Open( void );
    void Close( void );

    BOOL Start( void );
    void Stop( void );

    void Reset( void );

    void Torque( double Nm );
    void Units( long units );
};

/******************************************************************************/

extern  struct  STR_TextItem  MOTOR_TypeText[];

/******************************************************************************/

