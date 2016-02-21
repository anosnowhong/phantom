/******************************************************************************/
/*                                                                            */
/* MODULE  : MetaConfig.cpp                                                   */
/*                                                                            */
/* PURPOSE : Specified arbitrary sequences of configuration files.            */
/*                                                                            */
/* DATE    : 29/Sep/2007                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 29/Sep/2009 - Initial development of module.                     */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "METACONFIG"
#define MODULE_TEXT     "MetaConfiguration API"
#define MODULE_DATE     "29/09/2007"
#define MODULE_VERSION  "1.0"
#define MODULE_LEVEL    1

/******************************************************************************/

#include <motor.h>                               // Includes (almost) everything we need.

/******************************************************************************/

STRING METACONFIG_Units[METACONFIG_UNITS][METACONFIG_ITEMS];
int METACONFIG_UnitCount[METACONFIG_UNITS];
int METACONFIG_Type[METACONFIG_UNITS];
BOOL METACONFIG_Permute[METACONFIG_UNITS];
STRING METACONFIG_Sequence;
int METACONFIG_SequenceLength;
PERMUTELIST METACONFIG_PermuteList[METACONFIG_UNITS];
int METACONFIG_Repeat[STRLEN];

/******************************************************************************/

void METACONFIG_Setup( void )
{
int i;
char unit;

    // Reset configuration variable list...
    CONFIG_reset();

    for( i=0; (i < METACONFIG_UNITS); i++ )
    {
        unit = 'A'+i;
        CONFIG_set(STR_stringf("%c",unit),METACONFIG_Units[i],METACONFIG_ITEMS);
        CONFIG_setBOOL(STR_stringf("%c-Permute",unit),METACONFIG_Permute[i]);
    }

    CONFIG_set("Sequence",METACONFIG_Sequence);
}

/******************************************************************************/

void METACONFIG_Init( void )
{
int i,j;

    for( i=0; (i < METACONFIG_UNITS); i++ )
    {
        for( j=0; (j < METACONFIG_ITEMS); j++ )
        {
            memset(METACONFIG_Units[i][j],0,STRLEN);
        }

        METACONFIG_Type[i] = METACONFIG_TYPE_NOTUSED;
        METACONFIG_UnitCount[i] = 0;
        METACONFIG_Permute[i] = FALSE;
    }

    METACONFIG_SequenceLength = 0;
    memset(METACONFIG_Sequence,0,STRLEN);
}

/******************************************************************************/

BOOL METACONFIG_Load( char *file )
{
BOOL ok=TRUE;
int i,j,s,b;
STRING buff;

    // Initialization.
    METACONFIG_Init();
    METACONFIG_Setup();

    // Load configuration file...
    if( !CONFIG_read(file) )
    {
        printf("METACONFIG_Load(%s) Cannot read file.\n",file);
        ok = FALSE;
    }

    if( !ok )
    {
        return(FALSE);
    }

    for( i=0; (i < METACONFIG_UNITS); i++ )
    {
        METACONFIG_Type[i] = METACONFIG_TYPE_NOTUSED;
        METACONFIG_UnitCount[i] = 0;
        METACONFIG_PermuteList[i].Reset();

        for( j=0; (j < METACONFIG_ITEMS); j++ )
        {
            if( STR_null(METACONFIG_Units[i][j]) )
            {
                break;
            }

            METACONFIG_Type[i] = METACONFIG_TYPE_FILE;
            METACONFIG_UnitCount[i]++;
        }

        if( METACONFIG_UnitCount[i] == 0 )
        {
            continue;
        }

        METACONFIG_PermuteList[i].Init(0,METACONFIG_UnitCount[i]-1,METACONFIG_Permute[i]);

        for( j=0; (j < METACONFIG_UnitCount[i]); j++ )
        {
            if( METACONFIG_Units[i][j][0] == '(' )
            {
                METACONFIG_Type[i] = METACONFIG_TYPE_SUBSEQUENCE;

                strncpy(buff,METACONFIG_Units[i][j],STRLEN);
                memset(METACONFIG_Units[i][j],0,STRLEN);

                for( s=0,b=0; (b < strlen(buff)); b++ )
                {
                    if( (buff[b] != '(') && (buff[b] != ')') )
                    {
                        METACONFIG_Units[i][j][s++] = buff[b];
                    }
                }
            }
        }

        //printf("%c: type=%d count=%d permute=%s\n",'A'+i,METACONFIG_Type[i],METACONFIG_UnitCount[i],STR_YesNo(METACONFIG_Permute[i]));
        for( j=0; (j < METACONFIG_UnitCount[i]); j++ )
        {
            //printf("%d %s\n",j,METACONFIG_Units[i][j]);
        }
    }

    METACONFIG_SequenceLength = strlen(METACONFIG_Sequence);

    return(TRUE);
}

/******************************************************************************/

BOOL METACONFIG_Process( char *file, int &items, STRING list[] )
{
int i,j,k,n,s,count,unit,index;
STRING number="";
char c;
char sbuf[2][METACONFIG_SEQUENCE_LENGTH];
int slen[2];

    if( !METACONFIG_Load(file) )
    {
        return(FALSE);
    }

    strncpy(sbuf[0],METACONFIG_Sequence,METACONFIG_SEQUENCE_LENGTH);
    slen[0] = METACONFIG_SequenceLength;

    do
    {
        strncpy(sbuf[1],sbuf[0],METACONFIG_SEQUENCE_LENGTH);
        slen[1] = slen[0];

        memset(sbuf[0],0,METACONFIG_SEQUENCE_LENGTH);
        slen[0] = 0;

        for( s=0,i=0,n=0; (i < slen[1]); i++ )
        {
            c = toupper(sbuf[1][i]);

            if( isdigit(c) )
            {
                number[n++] = c;
            }
            else
            if( isalpha(c) )
            {
                if( n > 0 )
                {
                    number[n] = 0;
                    n = 0;
                    count = atoi(number);
                }
                else
                {
                    count = 1;
                }

                unit = c-'A';

                for( j=0; (j < count); j++ )
                {
                    switch( METACONFIG_Type[unit] )
                    {
                        case METACONFIG_TYPE_FILE :
                           sbuf[0][s++] = c;
                           break;

                        case METACONFIG_TYPE_SUBSEQUENCE :
                           index = METACONFIG_PermuteList[unit].GetNext();

                           for( k=0; (k < strlen(METACONFIG_Units[unit][index])); k++ )
                           {
                               sbuf[0][s++] = METACONFIG_Units[unit][index][k];
                           }
                           break;
                    }
                }
            }
        }

        slen[0] = s;
    }
    while( slen[0] != slen[1] );

    //printf("%s\n",sbuf[0]); while( !KB_anykey() );

    for( items=0,i=0,n=0; (i < slen[0]); i++ )
    {
        c = toupper(sbuf[0][i]);
        unit = c-'A';
        index = METACONFIG_PermuteList[unit].GetNext();

        strncpy(list[items],METACONFIG_Units[unit][index],STRLEN);
        items++;
        //printf("%02d: %s\n",items,list[items]);
    }

    return(TRUE);
}

/******************************************************************************/

