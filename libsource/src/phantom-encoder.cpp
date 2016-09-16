/******************************************************************************/
/* PHANTOM: Encoder functions...                                              */
/******************************************************************************/

//FIXME: Phantom is not used in the demo, here just comment the attribute that not exist, to pass the error
void    PHANTOM_EncoderGet( int ID, int axis )
{
    // If handle is valid, read encoder via ROBOT object...
    if( PHANTOM_Check(ID) )
    {
       //PHANTOM_Item[ID].Robot->EncoderUnits(axis,PHANTOM_EncoderEU[ID][axis]);
    }
}

/******************************************************************************/

void    PHANTOM_EncoderLoop( int ID )
{
int     axis;

    // Read encoder values for all axes...
    for( axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        PHANTOM_EncoderGet(ID,axis);
    }
}

/******************************************************************************/

//FIXME: comment the overload function
/*
void    PHANTOM_EncoderReset( int ID, int axis )
{
    // If handle is valid, reset encoder via ROBOT object...
    if( PHANTOM_Check(ID) )
    {
        PHANTOM_Item[ID].Robot->EncoderReset(axis);
    }
}
*/

/******************************************************************************/

//FIXME: comment the overload function
/*
void    PHANTOM_EncoderReset( int ID )
{
int     encoder;

    // Reset encoders for all axes...
    for( encoder=0; (encoder < PHANTOM_AXIS); encoder++ )
    {
        PHANTOM_EncoderReset(ID,encoder);
    }
}
*/

/******************************************************************************/

//FIXME: well I'll delete this useless file form cmakelist
long    PHANTOM_EncoderRead( int ID, int axis )
{
    if( PHANTOM_EncoderEU[ID][axis] == PHANTOM_EU_INIT )
    {
        PHANTOM_EncoderGet(ID,axis);
    }

    return(PHANTOM_EncoderEU[ID][axis]);
}

/******************************************************************************/

long    PHANTOM_Encoder( int ID, int axis )
{
long    EU;

    // Get the raw encoder value...
    EU = PHANTOM_EncoderRead(ID,axis);

    // Adjust sign of encoder...
    EU = PHANTOM_signE[ID][axis] * EU;

    return(EU);
}

/******************************************************************************/

void    PHANTOM_Encoder( int ID, long EU[] )
{
int     axis;

    for( axis=0; (axis < PHANTOM_AXIS); axis++ )
    {
        EU[axis] = PHANTOM_Encoder(ID,axis);
    }
}

/******************************************************************************/

