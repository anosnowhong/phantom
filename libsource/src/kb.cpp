/******************************************************************************/
/*                                                                            */
/* MODULE  : kb.cpp                                                           */
/*                                                                            */
/* PURPOSE : Keyboard API.                                                    */
/*                                                                            */
/* DATE    : 29/Dec/2000                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 29/Dec/2000 - Initial development of module.                     */
/*                                                                            */
/* V1.1  JNI 27/Jan/2002 - Added function to deliver keystrokes in command    */
/*                         shell window to the GLUT keyboard handler.         */
/*                                                                            */
/* V1.2  JNI 18/Jul/2002 - Processing for special (cursor & function) keys.   */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "KB"
#define MODULE_TEXT     "Keyboard API"
#define MODULE_DATE     "19/07/2002"
#define MODULE_VERSION  "1.2"
#define MODULE_LEVEL    1

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

BOOL    KB_API_started=FALSE;

/******************************************************************************/

void  (*KB_IdleFunc[KB_IDLE_FUNC])( void );

/******************************************************************************/

void    KB_check( void )
{
    if( !KB_API_started )
    {
        KB_init();
        KB_API_started = TRUE;
    }
}

/******************************************************************************/

void    KB_init( void )
{
int     item;

    for( item=0; (item < KB_IDLE_FUNC); item++ )
    {
        KB_IdleFunc[item] = NULL;
    }
}

/******************************************************************************/

int     KB_find( void (*func)( void ) )
{
int     item,find=KB_IDLE_NULL;

    KB_check();

    for( item=0; (item < KB_IDLE_FUNC); item++ )
    {
        if( KB_IdleFunc[item] == func )
        {
            find = item;
            break;
        }
    }

    return(find);
}

/******************************************************************************/

void    KB_idle(  int func, void (*fptr )( void ) )
{
int     item;
void  (*fget)( void )=NULL;
void  (*fset)( void )=NULL;

    switch( func )
    {
        case KB_IDLE_START :
           fget = NULL;
           fset = fptr;
           break;

        case KB_IDLE_STOP :
           fget = fptr;
           fset = NULL;
           break;
    }

    if( (item=KB_find(fget)) != KB_IDLE_NULL )
    {
        KB_IdleFunc[item] = fset;
    }
}

/******************************************************************************/

void    KB_idle( void )
{
int     item;

    KB_check();

    for( item=0; (item < KB_IDLE_FUNC); item++ )
    {
        if( KB_IdleFunc[item] != NULL )
        {
          (*KB_IdleFunc[item])();
        }
    }
}

/******************************************************************************/

BYTE    KB_EXT_xlat[][2] =
{
    { KB_EXT_UP    ,KB_UP     },
    { KB_EXT_DOWN  ,KB_DOWN   },
    { KB_EXT_LEFT  ,KB_LEFT   },
    { KB_EXT_RIGHT ,KB_RIGHT  },
    { KB_EXT_HOME  ,KB_HOME   },
    { KB_EXT_END   ,KB_END    },
    { KB_EXT_INSERT,KB_INSERT },
    { KB_EXT_DELETE,KB_DELETE },
    { KB_EXT_PGUP  ,KB_PGUP   },
    { KB_EXT_PGDN  ,KB_PGDN   },
    { KB_EXT_F11   ,KB_F11    },
    { KB_EXT_F12   ,KB_F12    },
    { KB_NUL       ,KB_NUL    }
};

BYTE    KB_NUL_xlat[][2] =
{
    { KB_NUL_F1    ,KB_F1     },
    { KB_NUL_F2    ,KB_F2     },
    { KB_NUL_F3    ,KB_F3     },
    { KB_NUL_F4    ,KB_F4     },
    { KB_NUL_F5    ,KB_F5     },
    { KB_NUL_F6    ,KB_F6     },
    { KB_NUL_F7    ,KB_F7     },
    { KB_NUL_F8    ,KB_F8     },
    { KB_NUL_F9    ,KB_F9     },
    { KB_NUL_F10   ,KB_F10    },
    { KB_NUL       ,KB_NUL    }
};

/******************************************************************************/

BYTE    KB_xlat( BYTE xlat[][2], BYTE key )
{
int     i;
BYTE    ext=KB_NUL;

    for( i=0; (xlat[i][0] != KB_NUL); i++ )
    {
        if( xlat[i][0] == key )
        {
            ext = xlat[i][1];
            break;
        }
    }

    return(ext);
}

/******************************************************************************/

BYTE    KB_special( BYTE code )
{
BYTE    special,xlat;

    // The translation key should be waiting for us...
    if( !KB_key(xlat,KB_NOWAIT) )
    {
        return(KB_NUL);
    }

    switch( code )
    {
        case KB_EXT :
           special = KB_xlat(KB_EXT_xlat,xlat);
           break;

        case KB_NUL :
           special = KB_xlat(KB_NUL_xlat,xlat);
           break;
    }

    return(special);
}

/******************************************************************************/

BOOL    KB_hit( void )
{
BOOL    hit;

    // Check if a key has been pressed (don't wait)...
    if( !(hit=_kbhit()) )
    {
        KB_idle();
    }

    return(hit);
}

/******************************************************************************/

BYTE    KB_get( int flag )
{
BYTE    code;

    // Wait for a key to be pressed...
    code = _getch();

    if( flag & KB_UPPER )
    {
        code = toupper(code);
    }

    switch( code )
    {
        case KB_EXT :
        case KB_NUL :
           code = KB_special(code);
           break;
    }

    return(code);
}

/******************************************************************************/

BOOL    KB_key( BYTE &code, int flag )
{
BOOL    done;

    if( (done=KB_hit()) )
    {
        code = KB_get(flag);
    }

    return(done);
}

/******************************************************************************/

BOOL    KB_key( BYTE &code )
{
BOOL    done;

    done = KB_key(code,KB_NOFLAGS);

    return(done);
}

/******************************************************************************/

BOOL    KB_anykey( void )
{
BOOL    hit;
BYTE    code;

     hit = KB_key(code);

     return(hit);
}

/******************************************************************************/

BYTE    KB_wait( int flag )
{
BYTE    code;

    while( !KB_key(code,flag) );

    return(code);
}

/******************************************************************************/

BYTE    KB_wait( void )
{
    return(KB_wait(KB_NOFLAGS));
}

/******************************************************************************/

BOOL    KB_ESC( int flag )
{
BOOL    hit,ok=FALSE;
BYTE    code;

     do
     {
         if( (hit=KB_key(code,flag)) )
         {
             ok = (code == ESC);
         }

     }
     while( (flag & KB_WAIT) && !hit );

     return(ok);
}

/******************************************************************************/

BOOL    KB_ESC( void )
{
    return(KB_ESC(KB_NOWAIT));
}

/******************************************************************************/

int     KB_GLUT_Translate[][2] = 
{
    { KB_F1 ,GLUT_KEY_F1  },
    { KB_F1 ,GLUT_KEY_F2  },
    { KB_F3 ,GLUT_KEY_F3  },
    { KB_F4 ,GLUT_KEY_F4  },
    { KB_F5 ,GLUT_KEY_F5  },
    { KB_F6 ,GLUT_KEY_F6  },
    { KB_F7 ,GLUT_KEY_F7  },
    { KB_F8 ,GLUT_KEY_F8  },
    { KB_F9 ,GLUT_KEY_F9  },
    { KB_F10,GLUT_KEY_F10 },
    { 0,0}
};

/******************************************************************************/

GLUTKEYBOARDFUNC KB_GLUT_KeyboardFunc=NULL;

GLUTKEYBOARDFUNC KB_GLUT_KeyboardFuncInstall( GLUTKEYBOARDFUNC func )
{
    KB_GLUT_KeyboardFunc = func;
    return(KB_GLUT_KeyboardFunc);
}

/******************************************************************************/

GLUTSPECIALFUNC  KB_GLUT_SpecialFunc=NULL;

GLUTSPECIALFUNC  KB_GLUT_SpecialFuncInstall( GLUTSPECIALFUNC func )
{
    KB_GLUT_SpecialFunc = func;
    return(KB_GLUT_SpecialFunc);
}

/******************************************************************************/

void    KB_GLUT_Events( void (*events)( void ) )
{
BOOL    done=FALSE;
BYTE    code;
int     i;

//  Call conventional GLUT event handler...
    if( events != NULL )
    {
        (*events)();
    }

//  Check for keys in command-shell...
    if( !KB_key(code,KB_NOWAIT) )
    {
        return;
    }

//  Process special function keys if required...
    if( KB_GLUT_SpecialFunc != NULL )
    {
//      Translate command-shell keys to GLUT keys...
        for( i=0; ((KB_GLUT_Translate[i][0] != 0) && !done); i++ )
        {
            if( KB_GLUT_Translate[i][0] == (int)code )
            {
                (*KB_GLUT_SpecialFunc)(KB_GLUT_Translate[i][1],0,0);
                done = TRUE;
            }
        }
    }

//  Key has been processed, to return...
    if( done )
    {
        return;
    }

//  Process regaular keys...
    if( KB_GLUT_KeyboardFunc == NULL )
    {
        return;
    }

    (*KB_GLUT_KeyboardFunc)(code,0,0);
}

/******************************************************************************/

void    KB_GLUT_Events( void )
{
    KB_GLUT_Events(NULL);
}

/******************************************************************************/

