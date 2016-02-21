/*********************************************************************
** Include file to define LOAD interface to CED 1401
**
** Copyright Cambridge Electronic Design 1987
**
** Revision History
** 13-Feb-87 GPS  This is the first version
** 08-Apr-87 GPS  Lattice C version added
** 15-Nov-90 JCN  1)get1401info added
**                2)open1401 returns error codes (true and false reversed)
**                3)ld       ditto
**                4)function declarations brought up to modern standards.
**                  They are now the same in load.h, load.c and the doc.
** 02/Dec/92 GPS  Lattice C special code all removed.
**********************************************************************
*/
#include <stdio.h>

extern FILE *labo;
extern FILE *labi;

int open1401(void);
void reset1401(void);
void close1401(void);
int stat1401(void);
void get1401info(int *rev, int *bus, int *type1401, int *state);
int ldcmd(char *command);
int ld(char *vl,char *str);
void setseg(char far *);
int tohost(char far *object,unsigned long size,unsigned long addr1401);
int to1401(char far *object,unsigned long size,unsigned long addr1401);

