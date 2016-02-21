#define REST    0
#define CLICK   1
#define RELEASE 2

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                  INITIALIZATION                            //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////




/**************************************************/
void swapbuffers(void)
{
  glutSwapBuffers();
}

/**************************************************/

void   clearSCREEN()
{
  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void clf()
{
  clearSCREEN();
  glutSwapBuffers();
  clearSCREEN();
}

/**************************************************/
void    start_graphics_3d( void )
{
BOOL    ok;

    ok = GRAPHICS_Start(GRAPHICS_DISPLAY_MONO);
}

/******************************************************************************/

void    start_graphics_2d( void )
{
BOOL    ok;

    ok = GRAPHICS_Start(GRAPHICS_DISPLAY_2D);
}

/******************************************************************************/

void start_graphics_table()
{
  int hpix=1024;
  int vpix=768;

  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(hpix,vpix);
  glutCreateWindow(""); 
  glutReshapeFunc(myReshape);
  glutFullScreen();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();	        // FRUSTUM
  glOrtho(1,hpix,1,vpix,-100,100);
  glViewport(0,0,hpix,vpix);
  cal2d_table();

  clearSCREEN();
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                    TEXT                                    //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


void put_info(char *string,float size,matrix &pos)
{
  void *font = GLUT_STROKE_ROMAN;
  float s=size*0.07*0.5*(1024/(119.05+33.33))/27.0;

  set_view_cal(pos,MONO);
  glColor3f(1.0,0.0,0.0);  
  set_color(RED);
  glScalef(s,s,1);
  for (int i=0;i<strlen(string);i++)  
    glutStrokeCharacter(font, string[i]);
  
}


void put_string(char *string,float size,float line)
{
  void *font = GLUT_STROKE_ROMAN;
  float s=size*0.07*0.5*(1024/(119.05+33.33))/27.0;

  set_view(scalar(-20) % (float)(2+(size*line*2)) % (float)-34,MONO);

  glColor3f(1.0,1.0,1.0);
  set_color(WHITE);
  glScalef(s,s,1);
  for (int i=0;i<strlen(string);i++)  
    glutStrokeCharacter(font, string[i]);
  
}

void put_string(char *string) { put_string(string,1,0); }

void put_string_table(char *string,float size,float line)
{
  void *font = GLUT_STROKE_ROMAN;
  float s=size*0.1;

  glPushMatrix();
  glLoadIdentity();  
  glTranslatef(20,20,0);

  glColor3f(1.0,1.0,1.0);
  glScalef(s,s,1);
  for (int i=0;i<strlen(string);i++)  
    glutStrokeCharacter(font, string[i]);
  glPopMatrix();
}

void put_string_table(char *string) { put_string_table(string,1,0); }

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                    FRUSTUM ROUTINES                        //
//                                                            //
////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

void    set_view_cal( float x, float y, float z, int eye )
{
    GRAPHICS_View(TRUE,x,y,z,eye);
}

/******************************************************************************/

void    set_view( float x, float y, float z, int eye )
{
    GRAPHICS_View(FALSE,x,y,z,eye);
}

/******************************************************************************/

void set_view_table(matrix &xyz)
{
  set_view_table(xyz(1,1),xyz(2,1),xyz(3,1));
}

void set_view_table(float x, float y, float z)
{
  set_view_table(x,y);
}

void set_view_table(float x, float y)
{
  glMatrixMode(GL_MODELVIEW);	
  glLoadIdentity();    
  glMultMatrixf(p2s);  // Calibration
  glTranslatef(x,y,0);
}
/**************************************************/
void set_view_cal( matrix &xyz, int eye )
{
  set_view_cal(xyz(1,1),xyz(2,1),xyz(3,1),eye);
}
/**************************************************/
void set_view(matrix &xyz,int eye)
{
  set_view(xyz(1,1),xyz(2,1),xyz(3,1),eye);
}
/**************************************************/

void ortho_init( void )
{
    GRAPHICS_FrustumInit(TRUE); // Ortho...
/*
  float scale=1.0;//fabs(EZ-SZ);
  float eyex;
  
  //LEFT EYE
  eyex=EX+ESEP;
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho(scale*(SL-eyex),scale*(SR-eyex),
	  scale*(SB-EY),scale*(ST-EY),1,1000);
  glGetDoublev(GL_PROJECTION_MATRIX,frust_left);

  //RIGHT EYE
  eyex=EX-ESEP;
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();	        // FRUSTUM
  glOrtho (scale*(SL-eyex),scale*(SR-eyex),
	     scale*(SB-EY),scale*(ST-EY),1,1000);
  glGetDoublev(GL_PROJECTION_MATRIX,frust_right);


  //MONOCULAR
  eyex=EX;
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();	        // FRUSTUM
  glOrtho (scale*(SL-eyex),scale*(SR-eyex),
	     scale*(SB-EY),scale*(ST-EY),1,1000);
  glGetDoublev(GL_PROJECTION_MATRIX,frust_mono); */

}
/**************************************************/
matrix cal_init(char *s)
{
  matrix calmat;
  char *root="S:/calib/people/";
  char str[150];
  
  sprintf(str,"%s%s",root,s);
   matrix_read(str,calmat);
  
//  ESEP=calmat(5,1);
  calmat=calmat(1,1,4,4);


for(int i=1; i<=4; i++)
    for(int j=1; j<=4; j++)
      p2s[(i-1)+4*(j-1)]=calmat(i,j);
  return calmat;
}

/**************************************************/
matrix cal_init()
{
  return   cal_init("current3d.f2s");
}

/**************************************************/
matrix cal2d_init()
{
return  cal_init("current2d.f2s");
}

/************************************************************/
matrix cal2d_table()
{
return  cal_init("air_table.f2s");
}

/************************************************************/

void translate(matrix pos)
{
  glTranslatef (pos(1,1),pos(2,1),pos(3,1));
}
/************************************************************/
void translate(float x,float y, float z)
{
  glTranslatef (x,y,z);
}
/************************************************************/

void my_draw_finger(matrix &pos, float radius)
{
  for(int eye=LEFT;eye<=RIGHT;eye++)
    {
      set_view_cal(pos,eye);
      glutSolidSphere(radius,8,8);
    }  
}



void polygon(int mode)
{
  if(mode==LINE) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  else if(mode==FILL) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  else if(mode==POINT) glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
}

/************************************************************/
void my_sphere_2d(matrix &pos, float radius, int colour, int depth)
{
  my_circle(pos,radius);
}

/************************************************************/

void my_circle(matrix &p,float radius)
{
  static int first=1;
  
  if(first)
    {
      build_circle(20);
      first=0;
    }
  
  set_view_cal(p(1,1),p(2,1),p(3,1),MONO);
  glScalef (radius,radius,radius);
  glCallList(CIRCLE);
}
/************************************************************/
void my_square(matrix &p,float radius)
{
  static int first=1;

  if(first)
    {
      build_square();
      first=0;
    }
  
  set_view_cal(p(1,1),p(2,1),p(3,1),MONO);
  glScalef (radius,radius,1.0);
  glCallList(SQUARE);
}
/************************************************************/
void my_rectangle(matrix &p,float w,float h)
{
  my_rectangle( p(1,1), p(2,1),p(3,1),w,h);
}
/************************************************************/
void my_rectangle(float p1, float p2, float p3,float w,float h)
{
  static int first=1;

  if(first)
    {
      build_square();
      first=0;
    }
  
  set_view_cal(p1,p2,p3,MONO);
  glScalef (w,h,1.0);
  glCallList(SQUARE);
    
}
/************************************************************/
void my_point(matrix &p,float radius)
{
  glPointSize(radius);  
  set_view_cal(p(1,1),p(2,1),p(3,1),MONO);
  glBegin(GL_POINTS);
  glVertex3f(0,0,0);
  glEnd();
}
/************************************************************/
void build_circle(int sm)
{
  int i;
  float s,c;
  
  glNewList(CIRCLE,GL_COMPILE);
  glBegin(GL_POLYGON);
  for (i=0;i<sm;i++)
    {
      c=cos(i*2*M_PI/(float)sm);
      s=sin(i*2*M_PI/(float)sm);
      glVertex2f(c,s);
    }
  glEnd();
  glEndList();
}
/************************************************************/
void build_square()
{
  float rad=1.0;
  
  glNewList(SQUARE,GL_COMPILE);
  glBegin(GL_POLYGON);
  glVertex2f(-rad/2.0,-rad/2.0);
  glVertex2f( rad/2.0,-rad/2.0);
  glVertex2f( rad/2.0, rad/2.0);
  glVertex2f(-rad/2.0, rad/2.0);
  glVertex2f(-rad/2.0,-rad/2.0);
  glEnd();
  glEndList();
}
/************************************************************/
void my_arrow(matrix &p, float length)
{
  my_arrow(p(1,1),p(2,1),p(3,1), length);
}
/************************************************************/
void my_arrow(float p1, float p2, float p3, float length)
{
  set_view_cal(p1,p2,p3,MONO);
  glScalef (length,length,1.0);

  glBegin(GL_LINES);
  glVertex2f(0,0);
  glVertex2f(1,0);

  glVertex2f(1,0);
  glVertex2f(0.8,0.2);

  glVertex2f(1,0);
  glVertex2f(0.8,-0.2);
  glEnd();

}
/************************************************************/

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                  2D  hand feedback                         //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
/************************************************************/
/******************************* 12/1/99 ****/

#define GRID 11

void draw_gridI()
{
  int k;
  int colour = 3;
  float shininess=10.0;
  
  glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
  glMaterialfv(GL_BACK, GL_SHININESS, &shininess);
  
  for(k=1;k<=2;k++)
    {
      for(int eye=LEFT;eye<=RIGHT;eye++)
	{
	  set_view_cal(0,0,0,eye);
	  //	  glMaterialiv(GL_FRONT, GL_COLOR_INDEXES, colmap[colour]);
	  glCallList(GRID);
	}
      swapbuffers();
    }
  

}

void draw_gridIII(int colour)
{
  int k;
  float shininess=10.0;
  
  glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
  glMaterialfv(GL_BACK, GL_SHININESS, &shininess);
  
  for(k=1;k<=2;k++)
    {
      for(int eye=LEFT;eye<=RIGHT;eye++)
	{
	  set_view_cal(0,0,0,eye);
	  //	  glMaterialiv(GL_FRONT, GL_COLOR_INDEXES, colmap[colour]);
	  glCallList(GRID);
	}
      swapbuffers();
    }
  

}



void make_gridI(float xmin,float xmax,float ymin,float ymax,float zpos)
{
  
  GLint s = 10;
  GLint i,g[12];
  GLfloat ypos[12],xpos[12];
  
  for (i=0;i<=s;i++)
    {
      if ((int)(i/2)==(float)0.5*(float)i) { g[i]=0; } else { g[i]=s-1; }
      ypos[i] = ymin + (ymax-ymin)*i/(s-1);
      xpos[i] = xmin + (xmax-xmin)*i/(s-1);
    }

  glNewList(GRID, GL_COMPILE);
  glBegin(GL_LINE_STRIP);
  for (i=0;i<s;i++)
    {
      glVertex3f(xpos[g[i]],ypos[i],zpos);
      glVertex3f(xpos[g[i+1]],ypos[i],zpos);
    }
  for (i=0;i<s;i++)
    {
      glVertex3f(xpos[i],ypos[g[i]],zpos);
      glVertex3f(xpos[i],ypos[g[i+1]],zpos);
    }
  glEnd();
  glEndList();
}





void make_gridII(float xmin,float xmax,\
		 float ymin,float ymax,\
		 float zpos,int which,int number)
{
  
  GLint s = 10;
  GLint i,g[11];
  GLfloat ypos[11],xpos[11];
  
  for (i=0;i<=s;i++)
    {
      if ((int)(i/2)==(float)0.5*(float)i) { g[i]=0; } else { g[i]=s-1; }
      xpos[i] = xmin + (xmax-xmin)*i/(s-1);
      ypos[i] = ymin + (ymax-ymin)*i/(s-1);
    }

  // this is where the coordinates come in

  switch(which)
    {

      
    case 1:  // xy
      glNewList(number, GL_COMPILE);
      glColor3f(1.0,1.0,1.0);
      glBegin(GL_LINE_STRIP);      
      for (i=0;i<s;i++)
	{
	  glVertex3f(xpos[g[i]],ypos[i],zpos);
	  glVertex3f(xpos[g[i+1]],ypos[i],zpos);
	}
      for (i=0;i<s;i++)
	{
	  glVertex3f(xpos[i],ypos[g[i]],zpos);
	  glVertex3f(xpos[i],ypos[g[i+1]],zpos);
	}
      glEnd();
      glEndList();
      break;
      
  case 2:      // xz
      glNewList(number, GL_COMPILE);
      glBegin(GL_LINE_STRIP);
      for (i=0;i<s;i++)
	{
	  glVertex3f(xpos[g[i]],zpos,ypos[i]);
	  glVertex3f(xpos[g[i+1]],zpos,ypos[i]);
	}
      for (i=0;i<s;i++)
	{
	  glVertex3f(xpos[i],zpos,ypos[g[i]]);
	  glVertex3f(xpos[i],zpos,ypos[g[i+1]]);
	}
      glEnd();
      glEndList();
      break;
      
      
  case 3:       // yz
      glNewList(number, GL_COMPILE);
      glBegin(GL_LINE_STRIP);
      for (i=0;i<s;i++)
	{
	  glVertex3f(zpos,xpos[g[i]],ypos[i]);
	  glVertex3f(zpos,xpos[g[i+1]],ypos[i]);
	}
      for (i=0;i<s;i++)
	{
	  glVertex3f(zpos,xpos[i],ypos[g[i]]);
	  glVertex3f(zpos,xpos[i],ypos[g[i+1]]);
	}
      glEnd();
      glEndList();
      break;
    }
}


void mySolidSphere(float r, int c)
{
  glutSolidSphere(r,c,c);
}
  
void draw_gridII()
{
  int k;
  float shininess=10.0;
  int colour = 3;
  
  glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
  glMaterialfv(GL_BACK, GL_SHININESS, &shininess);
  
  for(k=1;k<=2;k++)
    {
      for(int eye=LEFT;eye<=RIGHT;eye++)
	{
	  set_view_cal(0,0,0,eye);
	  //	  glMaterialiv(GL_FRONT, GL_COLOR_INDEXES, colmap[colour]);
	  glCallList(11);
	  glCallList(12);
	  glCallList(13);
	  glCallList(14);
	}
      swapbuffers();
    }
  

}



void my_explode(matrix pos,int colour, float radius)
{
  int eye;
  

  glDepthMask(GL_TRUE);        
  glDisable(GL_LOGIC_OP);
  glLogicOp(GL_COPY);
  clearSCREEN();

  glEnable(GL_LIGHTING);  // lighting disabled 
  glEnable(GL_LIGHT0);    // for now

  glPolygonMode(GL_FRONT, GL_FILL);   // else auxSolidCube isn't
  glPolygonMode(GL_BACK, GL_FILL);    // as xor_mode(0) does line
  float shininess=10.0;
  glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
  glMaterialfv(GL_BACK, GL_SHININESS, &shininess);

  for(int j=1;j<=4;j++)
    {
      clearSCREEN();
      for(float p=1;p<=6;p++)
	for(eye=LEFT;eye<=RIGHT;eye++)
	  {
	    set_view_cal(0,0,0,eye);
	    translate(pos);
	    glRotatef(60*p,0,0,1);
	    translate(j,0,0);
	    mySolidSphere(radius,2);
	  }
      for(  eye=LEFT;eye<=RIGHT;eye++)
	{
	  set_view_cal(0,0,0,eye);
	  glCallList(GRID);
	  
	  set_view_cal(0,0,0,eye);
      	  translate(pos);
	  mySolidSphere(radius,2);
	}
      swapbuffers();
    }
  
  glDisable(GL_LIGHTING);  // lighting disabled 
  glDisable(GL_LIGHT0);    // for now
  
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                  REST TIME ROUTINES                        //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void set_color( int color )
{
    GRAPHICS_ColorSet(color);
}

void rest()
{
  static int first=1;
  float t=0;
  
  GLfloat mat_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
  GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_position[] = { 10.0, 40.0, 0.0, 1.0 };
  GLfloat spot_direction[] = {-1.0, -1.0 ,-1.0};
  GLfloat lm_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
  glLightfv(GL_LIGHT0,GL_POSITION, light_position);
  glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,spot_direction);
  
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  glShadeModel (GL_SMOOTH);
  
  
  do
    {

      clearSCREEN();
      t+=5;
      
      for(int eye=LEFT;eye<=RIGHT;eye++)
	{
	  set_view_cal(-22,31,-27,eye);
	  glRotatef(t,1,-1,1);
	  set_color(BLUE);
	  glutSolidTeapot(11.0);
	}
      glutSwapBuffers();


    }
  while(read_keys()==0);
  clf();
  printf("Rest over\n");
}



void draw_markers(matrix &seen)
{
  for(int k=1;k<=cols(seen);k++)
    {
      if(seen(1,k)) 
	  cube(scalar(10) % (float)10 % (float)-34,1,1,0.2,WHITE);
      else
	  cube(scalar(10) % (float)10 % (float)-34,1,1,0.2,RED);
    }
}
/**********************************************************************/
void draw_markers(matrix &seen, int panic)
{
  int w=HPIX;
  int h=VPIX;

  set_view(0,0,0,MONO);
  glTranslatef(1150, 1010, 0);
  glPointSize(4);
  
  glBegin(GL_POINTS);
  for(int k=1;k<=cols(seen);k++)
    {
      if(seen(1,k)) glIndexi(703); else glIndexi(103);
      glVertex3f(k*10.0,0,0);
    }
  glEnd();
  
  glPointSize(6);
  glBegin(GL_POINTS);
  if(!panic) glIndexi(703); else glIndexi(103);
  glVertex3f((cols(seen)+2)*10.0,0,0);
  glEnd();
}


void cube(matrix &pos,float sx,float sy,float sz,int c,float ori)
{
  for(int eye=LEFT;eye<=RIGHT;eye++)
    {
      set_view_cal(pos,eye);
      set_color(c);
      glScalef(sx,sy,sz);
      glRotatef(ori,0.0,1.0,0.0);
      glutSolidCube(1.0);
    }
}

void cube(matrix &pos,float sx,float sy,float sz,int c) 
{ cube(pos,sx,sy,sz,c,0);}


void mySolidCylinder (int rd)
{
  float theta;
  glBegin(GL_QUAD_STRIP);
    for (int k=0;k<=rd;k++)
      {
	theta=2*M_PI*k/rd;
	glVertex3f(0.5*sin(theta),0.5*cos(theta),0);
	glVertex3f(0.5*sin(theta),0.5*cos(theta),1);
      }
  glEnd();
}






// load a width x height RGB .RAW file as a texture

GLuint LoadTextureRaw( const char * filename, int wrap, int width, int height)
{
  GLuint texture;
  void * data;
  FILE * file;

  // open texture data
  file = fopen( filename, "rb" );
  if ( file == NULL ) return 0;

  // allocate buffer

  data = malloc( width * height * 3 );

  // read texture data
  fread( data, width * height * 3, 1, file );
  fclose( file );

  // allocate a texture name
  glGenTextures( 1, &texture );

  // select our current texture
  glBindTexture( GL_TEXTURE_2D, texture );

  // select modulate to mix texture with color for shading
  //  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

  // when texture area is small, bilinear filter the closest MIP map
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                   GL_LINEAR_MIPMAP_NEAREST );
  // when texture area is large, bilinear filter the first MIP map
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  // if wrap is true, the texture wraps over at the edges (repeat)
  //       ... false, the texture ends at the edges (clamp)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                   wrap ? GL_REPEAT : GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                   wrap ? GL_REPEAT : GL_CLAMP );

  // build our texture MIP maps
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );

  // free buffer
  free( data );

  return texture;

}


void FreeTexture( GLuint texture )
{
  glDeleteTextures( 1, &texture );
}



void restb()  // resting teapot
{
  float t=0;
  
  do
    {
      clearSCREEN();
      t+=5;
      
      for(int eye=LEFT;eye<=RIGHT;eye++)
	{
	  set_view_cal(-22,31,-27,eye);
	  glRotatef(t,1,-1,1);
	  set_color(GREEN);
	  glutSolidTeapot(11.0);
	}
      glutSwapBuffers();
    }
  while(read_keys()==0);
  
  clearSCREEN();
  glutSwapBuffers();
  clearSCREEN();
}

/***************************************************************************/
// Space ball bits

void (*sb_but_func)( int button, int event )=NULL;
void (*sb_mot_func)( int x, int y, int z )=NULL;
void (*sb_rot_func)( int x, int y, int z )=NULL;

/***************************************************************************/
SiHdl hdl;             /* Spaceball device handle */


/***************************************************************************/

int sbinit()
{
  int res;                             /* result of SiOpen, to be returned  */
  SiOpenData oData;                    /* OS Independent data to open ball  */ 
  
  /*init the SpaceWare input library */
  if (SiInitialize() == SPW_DLL_LOAD_ERROR)  
     {
	 fprintf(stderr,"Error: Could not load SiAppDll dll files");
	 }

  SiOpenWinInit (&oData,GetActiveWindow() );    /* init Win. platform specific data  */
  
  /* open data, which will check for device type and return the device handle
     to be used by this function */ 
  
  if ( (hdl = SiOpen ("sbtestpb", SI_ANY_DEVICE, SI_NO_MASK,  
		      SI_EVENT, &oData)) == NULL) 
    {
      SiTerminate();  /* called to shut down the SpaceWare input library */
      res = 0;        /* could not open device */
      return res; 
    }
  else
    {
      SiSetUiMode(hdl, SI_UI_NO_CONTROLS); /* No Config Display */
      res = 1;        /* opened device succesfully */ 
      return res;
    }  
}


/***************************************************************************/

void SpaceballButtonFunc(void (*func)( int button, int event ))
{
  sb_but_func=func;
}

void SpaceballMotionFunc(void (*func)( int x, int y, int z ))
{
  sb_mot_func=func;
}

void SpaceballRotateFunc(void (*func)( int x, int y, int z ))
{
  sb_rot_func=func;
}

/*************************************************************************/

BOOL    SpaceWareEvent( MSG *mesg )
{
SiSpwEvent         Event;    // SpareWare event strucutre.
SiGetEventData     eData;    // SpaceWare event data structure.
int     num;

//  Extract SpareWare event data from message (if appropriate)...
    SiGetEventWinInit(&eData,mesg->message,mesg->wParam,mesg->lParam);

//  Is this a SpareWare event (if not, return FALSE)...
    if( SiGetEvent(hdl,0,&eData,&Event) != SI_IS_EVENT )
    {
        return(FALSE);
    }

//  What kind of SpareWare event are we processing?
    switch( Event.type )
    {
        case SI_MOTION_EVENT :
           if( sb_mot_func != NULL ) 
           {
             (*sb_mot_func)(Event.u.spwData.mData[SI_TX],
                            Event.u.spwData.mData[SI_TY],
                            Event.u.spwData.mData[SI_TZ]);
           }

           if( sb_rot_func != NULL )
           {
             (*sb_rot_func)(Event.u.spwData.mData[SI_RX],
                            Event.u.spwData.mData[SI_RY],
                            Event.u.spwData.mData[SI_RZ]);
           }
           break;
		  
        case SI_ZERO_EVENT :
           if( sb_mot_func != NULL ) 
           {
             (*sb_mot_func)(0,0,0);
           }

           if( sb_rot_func != NULL ) 
           {
             (*sb_rot_func)(0,0,0);
           }
           break;
		  
        case SI_BUTTON_EVENT :
           if( sb_but_func != NULL )
           {
               if( (num=SiButtonPressed(&Event)) != SI_NO_BUTTON )	
               {
                  (*sb_but_func)(num,CLICK);
               }

               if( (num=SiButtonReleased(&Event)) != SI_NO_BUTTON )       
               {
                  (*sb_but_func)(num,RELEASE);		    
               }
           }
           break;

        default :
           break;
    }

    return(TRUE);
}

/*************************************************************************/

void    check_event( BOOL SpaceWare )
{
static  BOOL init=FALSE;
static  MSG  mesg;
BOOL    rc;

    // Make sure GRAPHICS system is running...
    if( !GRAPHICS_Started() )
    {
        return;
    }

    // Do initialization stuff only once...
    if( !init )
    {
        if( SpaceWare )                // SpareWare space ball...
        {
            sbinit();
        }

        init = TRUE;
    }

//  Peek to see if there is a message waiting...
    rc = PeekMessage(&mesg,            // Pointer to message structure.
                      NULL,            // Handle to window (NULL?).
                      0,0,             // Message filter (min,max). Set for no filtering.
                      PM_NOREMOVE);    // Don't remove message from queue.

//  Was there a message waiting?
    if( !rc )
    {
        return;
    }

//  Get message from queue...
    rc = GetMessage(&mesg,             // Pointer to message structure.
                     NULL,             // Handle to window (NULL=Messages for window).
                     0,0);             // Message filter (min,max). As above.

//  Check return code...
    if( !rc )
    {
        printf("GetMessage(...) Failed.\n");
        return;
    }
  
//  Do SpareWare processing if required...
    if( SpaceWare )
    {
        if( SpaceWareEvent(&mesg) )    // Message processed by SpareWare?
        {
            return;
        }
    }

//  Translate virtual-key messages to character messages...
    rc = TranslateMessage(&mesg);		

//  Dispatch message to application call-back function...
    rc = DispatchMessage(&mesg);		
}

/*************************************************************************/

void    check_event( void )
{
    check_event(GRAPHICS_SpaceBall);   // Check events (optional SpaceWare processing)...
}

/*************************************************************************/

void    old_check_event( void )
{
    check_event(FALSE);                // Check events (without SpaceWare processing)...
}

/*************************************************************************/

