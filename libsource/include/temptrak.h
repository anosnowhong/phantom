/******************************************************************************/

#define TEMPTRAK_AXIS 6

/******************************************************************************/

class TEMPTRAK
{
private:

    // Object open flag.
    BOOL OpenFlag;

    STRING ObjectName;

    // Number of axes.
    int Axis;

    // Room temperature.
    double T_Air;

    // Configuration file.
    STRING ConfigFile;
    STRING ConfigPath;

    // Model running flag.
    BOOL StartedFlag;

    // Temperature to suspend (over-heated) motors .
    double T_OverHeat;

    // Temperature to resume (cooled) motors.
    double T_Cooled;

    // Electrical Resistance of copper core.
    double ER_Cu[TEMPTRAK_AXIS];

    // Conversion from DAC Units to current.
    double DAC2I[TEMPTRAK_AXIS];

    // Thermal Resistance of heat flow.
    double TR_Cu2Fe[TEMPTRAK_AXIS];
    double TR_Fe2Air[TEMPTRAK_AXIS];

    // Mass of copper core and iron casing.
    double MassCu[TEMPTRAK_AXIS];
    double MassFe[TEMPTRAK_AXIS];

    // Temperature of copper core and iron casing.
    double T_Cu[TEMPTRAK_AXIS];
    double T_Fe[TEMPTRAK_AXIS];

    // Change in temperature.
    double dT_Cu[TEMPTRAK_AXIS];
    double dT_Fe[TEMPTRAK_AXIS];

    // Time to resume (seconds).
    double t_Save;

    // Time step (seconds).
    double dt;

    // Frequency timer.
    TIMER_Frequency *FreqTimer;

public:
    TEMPTRAK( char *name, int axis, char *config );
    TEMPTRAK( char *name, int axis );
   ~TEMPTRAK( void );

    void ConfigSet( void );
    BOOL ConfigLoad( void );
    BOOL ConfigSave( void );

    BOOL Resume( int func );

    void Init( void );
    BOOL Init( char *name, int axis, char *config );
    BOOL Init( char *name, int axis );

    BOOL Opened( void );
    BOOL Started( void );

    BOOL Start( double frequency );
    BOOL Stop( void );

    void Update( int axis, long DAC );
    void Update( long DAC[] );

    void dTdt( int axis, long DAC, double step );
    void dTdt( int axis, long DAC );

    double T_Core( int axis );
    double T_Case( int axis );

    BOOL CoreExceed( double T );
    BOOL OverHeated( void );
    BOOL Cooled( void );

    double Current( int axis, long DAC );
    long DAC( int axis, double Current );
};

/******************************************************************************/

