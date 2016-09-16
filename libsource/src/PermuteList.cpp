/******************************************************************************/
/*                                                                            */
/* MODULE  : PermuteList.cpp                                                  */
/*                                                                            */
/* PURPOSE : Functions to permute a list of integers.                         */
/*                                                                            */
/* DATE    : 14/Jun/2076                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 14/Jun/2007 - Initial Development of module.                     */
/*                                                                            */
/******************************************************************************/

#include <motor.h>

/******************************************************************************/

PERMUTELIST::PERMUTELIST( int min, int max, BOOL permuted )
{
    Init(min,max,permuted);
}

/******************************************************************************/

PERMUTELIST::PERMUTELIST( int min, int max )
{
    Init(min,max);
}

/******************************************************************************/

PERMUTELIST::PERMUTELIST(  )
{
    Init();
}

/******************************************************************************/

PERMUTELIST::~PERMUTELIST(  )
{
}

/******************************************************************************/

void PERMUTELIST::Init( void )
{
    Min = 0;
    Max = 0;
    Count = 0;
    Index = 0;
    Loops = 0;
    PermuteFlag = FALSE;
}

/******************************************************************************/

void PERMUTELIST::Init( int min, int max, BOOL permuted )
{
int i;

    Init();
    if( (min < 0) || (max < 0) )
    {
        return;
    }

    Min = min;
    Max = max;
    Count = (max-min)+1;
    PermuteFlag = permuted;
    List.dim(Count,1);

    for( i=0; (i < Count); i++ )
    {
        List(i+1,1) = Min+i;
    }

    Reset();
}

/******************************************************************************/

void PERMUTELIST::Init( int min, int max )
{
    Init(min,max,TRUE);
}

/******************************************************************************/

void PERMUTELIST::Reset( void )
{
    Loops = 0;
    Index = 0;
}

/******************************************************************************/

BOOL PERMUTELIST::NextValue( int &i )
{
BOOL loop=FALSE;

    if( Count == 0 )
    {
        i = 0;
        return(FALSE);
    }

    if( (Index == 0) && PermuteFlag )
    {
        List = permute(List);
    }

    i = List.I(++Index,1);

    if( Index == Count )
    {
        Index = 0;
        Loops++;
        loop = TRUE;
    }

    return(loop);
}

/******************************************************************************/

int PERMUTELIST::GetNext( int &loop )
{
BOOL flag;
int i;

    flag = NextValue(i);
    loop = LoopCount();

    return(i);
}

/******************************************************************************/

int PERMUTELIST::GetNext( void )
{
int i,loop;

    i = GetNext(loop);

    return(i);
}

/******************************************************************************/

int PERMUTELIST::LoopCount( void )
{
    return(Loops);
}

/******************************************************************************/

