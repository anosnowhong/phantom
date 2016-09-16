/******************************************************************************/
/* PHANTOM: Functions to interconvert various units.                          */
/******************************************************************************/

float   PHANTOM_ConvEU2Rev( int ID, int encoder, long EU )
{
float   EU2Rev=10160.0; // Number of encoder counts per revolution.
float   Rev;

    Rev = (float)EU / EU2Rev;

    return(Rev);
}

/******************************************************************************/

float   PHANTOM_ConvEU2Rad( int ID, int encoder, float EU )
{
float   Rad;

    Rad = PHANTOM_EU2Rad[ID][encoder] * EU;

    return(Rad);
}

/******************************************************************************/

float   PHANTOM_ConvEU2Rad( int ID, int encoder, long EU )
{
    return(PHANTOM_ConvEU2Rad(ID,encoder,(float)EU));
}

/******************************************************************************/

long    PHANTOM_ConvNm2MU( int ID, int motor, float Nm )
{
long    MU;

//  Convert torques from Nm to PHANTOM Motor Units (MUs)...
//  Multiply by 256.0 in original Sensable demo code (V2.2)...

#ifdef  PHANTOM_OLD_FORCES        // V2.4
    MU = (long)(PHANTOM_Nm2MU[ID][motor] * Nm);
#else
    MU = (long)(PHANTOM_Nm2MU[ID][motor] * Nm * 256.0);
#endif

    return(MU);
}

/******************************************************************************/

float   PHANTOM_ConvMU2Nm( int ID, int motor, long MU )
{
float   Nm;

//  Convert torques back from PHANTOM Motor Units (MUs) to Nm...
//  Multiply by 256.0 in original Sensable demo code (V2.2)...

#ifdef  PHANTOM_OLD_FORCES        // V2.4
    Nm = (float)MU / PHANTOM_Nm2MU[ID][motor];
#else
    Nm = (float)MU / PHANTOM_Nm2MU[ID][motor] / 256.0;
#endif

    return(Nm);
}

/******************************************************************************/


