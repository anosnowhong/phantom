/******************************************************************************/

class   ENCODER
{
private:
    int Type;
#define ENCODER_TYPE_DRC_T23BA         0
#define ENCODER_TYPE_MAXON_HEDS5500    1

    double UnitsPerRevolution;

    CONTROLLER *Controller;
    int Channel;

    STRING ObjectName;

    long Last;
    long dEU;

public:

    ENCODER( int type, CONTROLLER *controller, int channel, char *name );
    ENCODER( int type, CONTROLLER *controller, int channel );
   ~ENCODER( void ); 

    void Init( int type, CONTROLLER *controller, int channel, char *name );

    BOOL Open( void );
    void Close( void );

    void Reset( void );

    long Units( void );

    double Revolutions( void );
    double Degrees( void );
    double Radians( void );
};

/******************************************************************************/

extern  struct  STR_TextItem  ENCODER_TypeText[];

/******************************************************************************/

