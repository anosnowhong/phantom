/************************************************/
/* 						*/
/*      SuperVGA 32768 BGI driver defines	*/
/*		Copyright (c) 1991		*/
/*	    Jordan Hargraphix Software		*/
/*						*/
/************************************************/

extern int far _Cdecl Svga32k_fdriver[];

/* These are the currently supported modes */
#ifndef SVGA320x200x32768
#define	SVGA320x200x32768	0	/* 320x200x32768 HiColor VGA */
#define	SVGA640x350x32768	1	/* 640x350x32768 HiColor VGA */
#define	SVGA640x400x32768	2	/* 640x400x32768 HiColor VGA */
#define	SVGA640x480x32768	3	/* 640x480x32768 HiColor VGA */
#define	SVGA800x600x32768	4	/* 800x600x32768 HiColor VGA */
#endif

#ifndef XNOR_PUT
#define	XNOR_PUT	5
#define NOR_PUT		6
#define NAND_PUT	7
#define TRANS_COPY_PUT	8	/* Doesn't work with 16-color drivers */
#endif

#define RGB(r,g,b) ((r & 31)<<10) | ((g & 31)<<5) | (b & 31)

int RealDrawColor(int color)
{
  if (getmaxcolor() > 256) 
    setrgbpalette(1024,(color>>10)&31,(color>>5)&31,color&31);
  return(color);
}

int RealFillColor(int color)
{
  if (getmaxcolor() > 256)
    setrgbpalette(1025,(color>>10)&31,(color>>5)&31,color&31);
  return(color);
}

int RealColor(int color)
{
  if (getmaxcolor() > 256)
    setrgbpalette(1026,(color>>10)&31,(color>>5)&31,color&31);
  return(color);
}
