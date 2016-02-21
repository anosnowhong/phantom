/*****************************************************************************
**
** machine.h
**
** Copyright (c) Cambridge Electronic Design Limited 1991,1992
**
** This file is included at the start of 'C' or 'C++' source file to define
** things needed to make Macintosh DOS and Windows sources more compatible
**
** Revision History
**
** 10/Jun/91  PNC   First Version
**  3/Mar/92  TDB   Added support for non-Windows DOS. Now expects to be 
**                  included after windows.h.
** 23/Jun/92  GPS   Tidied up. SONAPI definitions moved to son.h
** 27/Jul/92  GPS   Made routines that need to be far in MSDOS as F_xxxxxx
**                  and mapped this to actual name. Also added LPSTR and
**                  DWORD definitions.
** 24/Feb/93  PNC   Added new defines _IS_MSDOS_ for actual msdos (not for
**                  windows) and _IS_WINDOWS_ for windows (16 or 32 bit)
**
** 14/Jun/93  KJ    Made few changes, enabling it to be used by CFS for DOS,
**                  Windows and Macintosh.
**
** 01/Oct/93  PNC   Defined _near for NT 32 bit compile as this is an invalid
**                  keyword under this compiler.
**
*****************************************************************************/

/*****************************************************************************
012345678901234567890123456789012345678901234567890123456789012345678901234567
*****************************************************************************/

#ifndef __MACHINE__
    #define __MACHINE__

    #ifdef macintosh
        #include <types.h>        /* Needed for various types               */
        #include <memory.h>       /* for NewHandle etc                      */
        #include <string.h>       /* for string manipulations               */
    #else
        #include <sys\types.h>    /* Needed for various types               */
        #include <sys\stat.h>                            /*    ditto        */
    #endif
    
    #ifdef MSDOS              /* first see if we are aiming at msdos result */
       #define _IS_MSDOS_     /* if so define our ms dos symbol             */
    #endif

    #ifdef _WINNT_            /* if its windows define our windows symbol   */
       #define _IS_WINDOWS_   /* _WINNT_ is defined for 32-bit at moment    */
       #undef _IS_MSDOS_      /* and we arent doing msdos after all         */
    #endif

    #ifdef _INC_WINDOWS       /* the alternative windows symbolic defn      */
       #ifndef _IS_WINDOWS_   /* as above but _INC_WINDOWS is for 16 bit    */
          #define _IS_WINDOWS_
       #endif
       #undef _IS_MSDOS_      /* and we arent doing msdos after all         */
    #endif

    #ifndef _WINNT_
    typedef short BOOLEAN;
    #endif

    #ifndef TRUE
       #define TRUE 1
       #define FALSE 0
    #endif


    #ifdef _IS_WINDOWS_           /* Now set up for windows use             */
       #ifdef _WINNT_             /* if we are in NT all is SMALL           */
       #define F_memcpy memcpy    /* Define model-independent routines      */
       #define F_strlen strlen
       #define F_strcat strcat
       #define F_strcpy strcpy
       #define F_strcmp strcmp
       #define F_strncat strncat
       #define F_strncpy strncpy
       #define F_strncmp strncmp
       typedef long Coord;        /* this is LONG in the MacApp definitions */
       #define _near              /* stop compiler errors for 32 bit compile*/
       #else
       #define F_memcpy _fmemcpy  /* Define model-independent routines      */
       #define F_strlen lstrlen
       #define F_strcat lstrcat
       #define F_strcpy lstrcpy
       #define F_strcmp lstrcmp
       #define F_strncat _fstrncat
       #define F_strncpy _fstrncpy
       #define F_strncmp _fstrncmp
       typedef short  Coord;      /* this is LONG in the MacApp definitions */
       #endif

       typedef double fdouble;
       #define FDBL_DIG DBL_DIG
       typedef HGLOBAL THandle;

       #define M_AllocMem(x)     GlobalAlloc(GMEM_MOVEABLE,x)
       #define M_AllocClear(x)   GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,x)
       #define M_FreeMem(x)      GlobalFree(x)
       #define M_LockMem(x)      GlobalLock(x)
       #define M_MoveLockMem(x)  GlobalLock(x)
       #define M_UnlockMem(x)    (GlobalUnlock(x)==0)
       #define M_NewMemSize(x,y) (x = GlobalReAlloc(x,y,GMEM_MOVEABLE))
       #define M_GetMemSize(x)   GlobalSize(x)   
   #endif /* _IS_WINDOWS_ */

   #ifdef _IS_MSDOS_              /* and this is the stuff for MS-DOS only  */
       #define F_memcpy _fmemcpy  /* Define model-independent routines */
       #define F_strlen _fstrlen
       #define F_strcat _fstrcat
       #define F_strcpy _fstrcpy
       #define F_strcmp _fstrcmp
       #define F_strncat _fstrncat
       #define F_strncpy _fstrncpy
       #define F_strncmp _fstrncmp
       #define F_malloc _fmalloc
       #define F_free   _ffree
       #define F_calloc  _fcalloc
       #define F_realloc _frealloc
       #define F_msize   _fmsize
       #define FAR _far
       #define PASCAL pascal
       #define BOOL short

       typedef char _far * LPSTR;
       typedef unsigned short WORD;
       typedef unsigned long DWORD;
       typedef unsigned char BYTE;
       typedef void _far * THandle; /* dummy to allow dos compiles          */
       typedef WORD _far * HWND;    /* dummy to allow dos compiles          */
       typedef WORD _far * LPWORD;  /* dummy to allow dos compiles          */

       #define M_AllocMem(x)     F_malloc(x)
       #define M_AllocClear(x)   F_calloc(x)
       #define M_FreeMem(x)      F_free(x)
       #define M_LockMem(x)      (x)
       #define M_MoveLockMem(x)  (x)
       #define M_UnlockMem(x)    (x != NULL)
       #define M_NewMemSize(x,y) F_realloc(x,y)
       #define M_GetMemSize(x)   F_msize(x)
    #endif  /* _IS_MSDOS_ */

    #ifdef macintosh
        #define FAR
        #define PASCAL
        #define _far
        #define _near
        #define F_memcpy memcpy
        #define F_strcat strcat
        #define F_strcpy strcpy
        #define F_strcmp strcmp
        #define F_strcmpi strcmpi
        #define F_strncat strncat
        #define F_strncpy strncpy
        #define F_strncmp strncmp
        #define F_strlen strlen
        #define FDBL_DIG LDBL_DIG

        typedef char * LPSTR;
        typedef unsigned short WORD;
        typedef unsigned long  DWORD;
        typedef unsigned char  BYTE;
        typedef long double fdouble;
        typedef long Coord;     /*  Borrowed from MacApp */
        typedef Handle THandle;

        #define M_AllocMem(x)     NewHandle(x)
        #define M_AllocClear(x)   NewHandleClear(x)
        #define M_FreeMem(x)      DisposHandle(x)
        #define M_LockMem(x)      (HLock(x),*x)
        #define M_MoveLockMem(x)  (HLockHi(x),*x)
        #define M_UnlockMem(x)    (HUnlock(x),TRUE)
        #define M_NewMemSize(x,y) (SetHandleSize(x,y),MemError() == 0)
        #define M_GetMemSize(x)   GetHandleSize(x)
    #endif  /* macintosh */


#endif /* not defined __MACHINE__ */

 
