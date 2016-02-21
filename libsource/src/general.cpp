/******************************************************************************/

#define MODULE_NAME     "GENERAL"
#define MODULE_TEXT     "General Purpose API"
#define MODULE_DATE     "18/03/2002"
#define MODULE_VERSION  "2.0"
#define MODULE_LEVEL    1

/*****************************************************************************/

#include <motor.h>

/*****************************************************************************/

BOOL     FILE__open( char *file, char *mode );
void     FILE__close( char *file );

BOOL     FILE_printf( char *file, char *mask, ... );
BOOL     FILE_puts( char *file, char *buff, int size );
int      FILE_scanf( char *file, char *buff, ... );
int      FILE_gets( char *file, char *buff, int size );

/*****************************************************************************/

void quit() 
{ 
    printf("Quitting...\n"); 
    //    PHANTOM_Stop();
    //    PHANTOM_Close();
    //    FOB_Stop();
    //    FOB_Close();
    SiTerminate(); 
    //    printf("Bye.\n");
    exit(0); 
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                   32bit NT portio                          //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

/**********************************************************************/

BYTE    inportb( int port )
{  
    return(ISA_byte(port));
}

/**********************************************************************/

void    outportb( int port, BYTE val )
{
    ISA_byte(port,val);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                        TIMING                              //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
/**********************************************************************/

void delay(float ms)
{
  double t1;
  t1=get_mtime();
  while(get_mtime()-t1<ms); 
}
/**********************************************************************/
double get_time()
{
  timeb t;
  ftime(&t);
  return (double)t.time+(double)t.millitm/1000.0;
}

/************************************************************/
double get_mtime()
{
  timeb t;
  ftime(&t);
  return (double)t.time*1000.0+(double)t.millitm;
}

/************************************************************/

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                         AUDIO                              //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

/************************************************************/
void beep(int frequency,float dur,float amp)
{
  Beep(frequency,dur);
}

/************************************************************/
void beep()
{
  Beep(700,500);
}

/************************************************************/
void tone_exit( void )
{
    tone(0,0);
}

void tone(unsigned frequency, int on )
{
  static BOOL init=TRUE;
  static int play=0;
  static int control;
  
  if( init )
  {
      init = FALSE;
      atexit(tone_exit);
  }

  if(!on && play)
    {
      Beep(0,0);
      //outportb( 0x61, control );
      play=0;
    }       
  
  if(on && !play)
    {
      // Prepare timer by sending 10111100 to port 43. 
      outportb( 0x43, 0xb6 );
      
      // Divide input frequency by timer ticks per second and
      // _write (byte by byte) to timer.
      //
    
      // Save speaker control byte. 
      control = inportb( 0x61 );
      
      // Turn on the speaker (with bits 0 and 1). 
      outportb( 0x61, control | 0x3 );
      play=1;
    }

  if(on)
    {
      frequency = (int)(1193180L / frequency);
      outportb( 0x42, (char)frequency );
      outportb( 0x42, (char)(frequency >> 8) );
    }
}



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                         MENUS                              //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
char xxx[100];

int getanswer(char *xy,int def)
/* def=any non-zero value, default=YES; def=0, default=NO */
{
  int yesplease;
  char response;

  printf("%s [",xy);
  if (def) printf("Yes]");
  else  printf("No ]");

  response=_getch();
    
  switch (toupper(response))
    {
      
  case 13:   if (!def) goto def_to_NO;
  case 'Y':
      yesplease=1;
      printf("\b\b\b\b");
    printf("Yes]\n");
      break;
      
  def_to_NO:
  case 'N':
      yesplease=0;
      printf("\b\b\b\b");
      printf("No ]\n");
      break;
      
  default:
      printf("\r");
      response=NULL;
      yesplease=getanswer(xy,def);
      break;
    }
  
  return yesplease;
}

/****************************************************************/
void getval(char *xy,int &ixy)
{
  printf("%s - [%i] ",xy,ixy);
  gets(xxx);
  if (strlen(xxx)!=0)   ixy=atoi(xxx);
}

/****************************************************************/
void getval(char *xy,float& fxy)
{
  printf("%s - [%5.5f] ",xy,fxy);
  gets(xxx);
  if (strlen(xxx)!=0)
    fxy=atof(xxx);
}
/****************************************************************/
void getval(char *xy,double& fxy)
{
  printf("%s - [%5.5f] ",xy,fxy);
  gets(xxx);
  if (strlen(xxx)!=0)
    fxy=atof(xxx);
}
/****************************************************************/
void getval(char *xy,char *s)
{
  printf("%s - [%s] ",xy,s);
  gets(xxx);
  if (strlen(xxx)!=0)   strcpy(s,xxx);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                 FILE MANIPULATION                          //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void clear_file(char *s)
{
  FILE *strm;
  if( (  strm=fopen(s,"w"))==NULL) 
    {
      printf("%s cannot be _opened.\n",s);
      exit(0);
    }
  fclose(strm);
}
/**********************************************************************/
void to_file(char *name,char *s)
{
  FILE *strm;
  if( (  strm=fopen(name,"a"))==NULL) 
    {
      printf("%s cannot be _opened .\n",s);
      exit(0);
    }
  fprintf(strm,"%s",s);
  fclose(strm);
}
/**********************************************************************/
int exist(char *s)
{
  FILE *strm;
  char c;
  char r[10];
  if((strm=fopen(s,"r"))!=NULL) 
    {
      fclose(strm);
      printf("%s already exists. Continue? [y/n] ",s);
      while(!_kbhit());
      c=_getch();
      printf("%c\n",c);
      if(c!='\n' && c!='y'  && c!='Y')	  return 1;
    }
  return 0;
}
/**********************************************************************/
int keyhit(char *s)
{
  if(_kbhit())
    {
      s[0]=_getch();
      return 1;
    }
  else return 0;
}
/**********************************************************************/
int keyhit()
{
  if(_kbhit())
    {
      _getch();
      return 1;
    }
  else return 0;
}

/**********************************************************************/
int read_keys()
{
  int reg,ctrl,out;

  reg=inportb(0x379);
  ctrl=inportb(0x37a);

  out=(!((reg&64)/64))  *32;
  out+=((reg&128)/128)  *16;
  out+=(!((reg&32)/32)) * 8;
  out+=(!((reg&16)/16)) * 4;
  out+=(ctrl & 2)    ;
  out+=!(reg&8);

  return out;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********   ZEST BASED ROUTINES   ***********************************/
/**********************************************************************/
/**********************************************************************/
float prob(float x)
{
  return  1.0/(1.0+exp(-limit(x)));
}
/************************************************************/
float invprob(float x)
{
  return  log(x/(1.0-x));
}
/************************************************************/

matrix fieller_cl(struct logit &dir)
{ 
  matrix L(3);
  
  double k=1.96*1.96;
  double a=sqr(dir.B(2,1))-dir.C(2,2)*k;
  double b=2.0*dir.B(1,1)*dir.B(2,1)-2.0*k*dir.C(1,2);
  double c=sqr(dir.B(1,1))-dir.C(1,1)*k;

  double f=b*b-4.0*a*c;
  
  if(f>=0)
    {
      if(a>0)
	{
	  L(2,1)=(-b+sqrt(f))/(2.0*a);
	  L(1,1)=(-b-sqrt(f))/(2.0*a);
	}
      else
	{
	  L(1,1)=(-b+sqrt(f))/(2.0*a);
	  L(2,1)=(-b-sqrt(f))/(2.0*a);
	}
      L(3,1)=L(2,1)-L(1,1);
      if(L(1,1)>dir.theta || L(2,1)<dir.theta) L(3,1)=1000.0;
    }
  else L(3,1)=1000.0;
  
  return L;
}
/*************************************************************************/
matrix glm(struct logit &dir)
{
  float convacc=0.000001;
  matrix oldB(2,1);
  matrix U(2,1);
  matrix P,Q;
  matrix Y,X,M,X1;
  matrix K;
  matrix J(2,2);
  
  float s1=10.0;
  float s2=10.0;
  float m1=0.0;
  float m2=0.0;


  
  //column of 1s
  M=ones(rows(dir.D),1);
 
  //stimulus values
  X1=dir.D[1];
  
  // stimulus values + offset
  X=M|X1;

  //responses
  Y=dir.D[2];
  
  dir.B.zeros();         
  int j=0;
  
  do
    {
      j++;
      //probability of response 1 given current paramters
      P=apply(X*dir.B,prob);

      // calculate p*(1-p)
      Q= P^(M-P);
      
      //Hessian matrix - 2nd derivative of likelihood
      J(1,1)=sum(Q);
      J(2,1)=J(1,2)=sum(X1^Q);
      J(2,2)=sum(X1^X1^Q);
      
      //Score matrix - 1st derivative of likelihood
      U(1,1)=sum(Y-P);
      U(2,1)=sum(X1^(Y-P));
      
      if(!dir.unique) //add in priors
	{
	  U(1,1)+=(m1-dir.B(1,1))/s1;
	  U(2,1)+=(m2-dir.B(2,1))/s2;
	  
	  J(1,1)+=1.0/s1;
	  J(2,2)+=1.0/s2;
	}      
      oldB=dir.B;

      //Newtons 2nd order iterative method
      if(det(J)!=0)
        {
          dir.C=inv(J);
	  dir.B=dir.B+dir.C*U;
        }
      
      // change in parameter estimates
      K=dir.B-oldB;
      K=T(K)*K;
    }          
  while(K(1,1)>convacc && j<50);

  
  dir.beta=dir.B(2,1);
  dir.theta=-dir.B(1,1)/dir.B(2,1);
  
  return fieller_cl(dir); 
}

/************************************************************/      
void reset(struct logit &dir)
{
  dir.B=matrix_dim(2);
  dir.CL=matrix_dim(3);
  dir.C=matrix_dim(2,2);

  randomize();
  
  dir.unique=dir.theta=dir.beta=0;
  dir.CL.zeros();
  dir.CL(3,1)=1000;
  dir.B.zeros();
  dir.count=0;
}

/************************************************************/
void logit_output(struct logit &dir,char *s)
{
  FILE *fc;
  int p=rows(dir.D);
  fc=fopen(s,"a");
  fprintf(fc,"%2i %i %8.4f %i  %8.4f %8.4f %8.4f %8.4f %8.4f\n",
	  p,1-dir.unique,dir.D(p,1),
	  (int)dir.D(p,2),
	  dir.theta,dir.beta,
	  dir.CL(1,1),dir.CL(2,1),dir.CL(3,1));
  fclose(fc);  
}

/************************************************************/

float zest(struct logit &dir,float xmin,float xmax,matrix (*stimulus)(float),char *s)
{
  FILE *fc;
  float xhat;
  dir.count++;
  
    /*Make sure that you get different respones at xmax and xmin*/
  /*i.e. you contain the threshold in the interval*/

  if(dir.count==1)
    {
      fc=fopen(s,"w");
      fprintf(fc,"Trial prior x r theta beta low high range\n");
      fclose(fc);
      
      reset(dir);
      dir.count++;
      dir.D=stimulus(xmin);
      
    }
  else if(dir.count==2)
    {
      dir.D=dir.D%stimulus(xmax);
      if((dir.D(1,2)==dir.D(2,2)))
	{
	  printf("Completely hopeless\n");
	  exit(0);
	}
      dir.CL=glm(dir);
    }
  else
    {
      xhat=(invprob(drand(0.01,0.99))-dir.B(1,1))/dir.B(2,1);
      xhat=maxa(xhat,xmin);
      xhat=mina(xhat,xmax);
      
      dir.D=dir.D%stimulus(xhat);
      
      if(!dir.unique)
	{
	  matrix H=(dir.D(dir.D[2]==1.0))[1];
	  matrix L=(dir.D(dir.D[2]==0.0))[1];
	  if(mmin(L)<mmin(H) && mmin(H)<mmax(L)) dir.unique=1;
	  if(mmin(H)<mmin(L) && mmin(L)<mmax(H)) dir.unique=1;
	}
      
      dir.CL=glm(dir);
    }
  
  
  logit_output(dir,s);
  return  dir.theta;
}



/**************************************************************************/


/*
void move(struct movement &mv)
{
  static int panic;
  int c,done=0,frame=0;
  double tm=0,oldtm=0,t1,t2=0;
  float freq = 250;      // opto_freq
  char line[80];         // strings for socket sends
  matrix opos;

  mv.count=0;
  mv.done=0;
  mv.dur=0;
  mv.move_dur=0;
  t1=get_time();
  
  do
    {
      mv.count++;
      if(mv.use_robot)
	{
	  sscanf(line,"%lf %lf %lf %i\n",
		 &mv.hpos(1,1),
		 &mv.hpos(2,1),
		 &mv.hpos(3,1),
		 &panic);
	  mv.hpos(4,1)=1;
	  mv.pos=mv.hpos;
	  tm=get_time();
	}
      else
	{
	  do
	    {
	      do    mv.frame=get_pos(mv.pos,mv.seen,mv.nm);
	      while(mv.frame==mv.oframe);
	    }
	  while(mv.mustsee && !mv.seen(1,mv.hmarker));
	  if(mv.use_2m)
	    mv.hpos=mv.pos[1]+16.8*(mv.pos[2]-mv.pos[1])/norm(mv.pos[2]-mv.pos[1]);
          else
	    mv.hpos=mv.pos[mv.hmarker];
	}
      
      mv.dpos=mv.hpos; // set display position to hand position      
      if (mv.count>1)  // velocity calculation use one marker instead of hand
	{
	  if(mv.use_robot)
	    {mv.vel=(mv.pos[1]-opos)/(tm-oldtm); oldtm=tm;}
	  else
	    mv.vel=(mv.pos[mv.hmarker]-opos)*freq/(mv.frame-mv.oframe);
	  if(t2==0 && norm(mv.vel)>6.0) t2=get_time();
	}
      else
	mv.vel=mat(100)%100%100%0;
      
      if(mv.use_robot)	opos=mv.pos[1];  else opos=mv.pos[mv.hmarker];
      mv.dur=get_time()-t1;              //movement duration  calculation
      mv.move_dur=mv.dur-(t2-t1);
      if(mv.count==1) mv.initpos=mv.hpos;   //save initial position
      mv.dist=norm(mv.hpos-mv.initpos[1]);  //distance moved
   
      //   mv.err=norm(mv.stop-mv.hpos);             //ch
   mv.func(mv);                              //call a user defined function

   //Test conditions to end the movement
   if((mv.end & MV_POS)|      
      (mv.end & MV_VEL)|
      (mv.end & MV_DIS))
     {
       done=1;
       if(mv.end & MV_POS)   done=done & (mv.err<mv.posth);
       if(mv.end & MV_VEL)   done=done & (norm(mv.vel)<mv.velth);
       if(mv.end & MV_DIS)   done=done & (mv.dist>mv.disth);
	}
   else done=0;

   if(mv.end & MV_TIM)  if (mv.dur>mv.time) done=1;
   if(mv.end & MV_KEY)  { c=read_keys();if(c==4 || c==8) done=1;}      
   if(mv.done)          done=1;


   if(mv.draw_markers) 
     {if(mv.use_robot) draw_markers(mv.seen%panic);else draw_markers(mv.seen);}
   
   static oldhand=mv.hand;      
   
   mv.oframe=mv.frame;

   //   spool_update();

   check_event();
   
    }
  
  while(!done);
  
  mv.move_dur=mv.dur-(t2-t1);  
}
*/

/**************************************************************************/
void move_init(struct movement &mv,int nm)
{
  mv.seen=matrix_dim(1,nm);
  mv.pos=matrix_dim(4,nm);
  mv.stop=matrix_dim(4,1);
  mv.dpos=matrix_dim(4,1);
  mv.ipos=matrix_dim(4,1);
  mv.opos=matrix_dim(4,1);
  mv.hpos=matrix_dim(4,1);
  mv.start=matrix_dim(4,1);
  mv.count=0;
  mv.hand=1;
  mv.hrad=0.5;
  mv.end=0;
  mv.posth=1.0;
  mv.velth=3.0;
  mv.disth=5.0;
  mv.time=10.0;
  mv.done=0;
  mv.nm=nm;
  mv.mustsee=1;
  mv.draw_markers=0;
  mv.use_robot=0;
  mv.use_2m=0;
  mv.hmarker=1;
  mv.p1=mv.p2=mv.p3=mv.p4=mv.p5=mv.p6=mv.p7=mv.p8=mv.p9=mv.p10=0;
}
/**************************************************************************/
void move_init(struct movement &mv) { move_init(mv,1);}


//-----------------------------------------------------
matrix index_batch(int nmove,int npar, int mix, int return_move)
//-----------------------------------------------------
{
  matrix Tind;
  matrix Moveind;
  matrix Parind;
  matrix batch;
  matrix Temp;
  int i;
  
  // 1. Make up matrix of move labels 
  
  Tind=matrix_dim(nmove,1);
  for(i=1;i<=nmove; i++)
    Tind(i,1)=i;           // dimension [nmove,1]

  
  // 2. Make up matrix of paradigm labels
  
  Parind=matrix_dim(npar,1);
  for(i=1;i<=npar; i++)
    Parind(i,1)=i;           // dimension [npar,1]


  // 3. Make up a batch of randomised target and paradigms
  batch=matrix_dim(1,1);
  
  if(mix==MPAR)    // mix up the paradigms but not the moves
    {
      for(i=1;i<=nmove; i++)
	{
	  Temp=matrix_dim(npar,1);          // [npar,1]
	  Temp.fill((int)Tind(i,1));    // all elements set to Tind(i)
	  if(return_move)
	    batch=batch%(permute(Parind)|permute(Parind)|Temp);
	  else
	    batch=batch%(permute(Parind)|Temp);
	  
	}
    }
  
  else if (mix==MMOVE)
    {
      for(i=1;i<=npar; i++)
	{
	  Temp=matrix_dim(nmove,1);          // [nmove,1]
	  Temp.fill((int)Parind(i,1));
	  if(return_move)
	    batch=batch%(Temp|Temp|permute(Tind));
	  else
	    batch=batch%(Temp|permute(Tind));
	}
      
    }

  else if (mix==MMOVE_PAR)
    {

      for(i=1;i<=nmove; i++)
	{
	  Temp=matrix_dim(npar,1);          // [npar,1]
	  Temp.fill((int)Tind(i,1));    // all elements set to Tind(i)
	  if(return_move)
	    batch=batch%((Parind)|permute(Parind)|Temp);
	  else
	    batch=batch%(permute(Parind)|Temp);
	  
	  batch=permute(batch);
	}
    }

    else if (mix==MNONE)
    {
      for(i=1;i<=nmove; i++)
	{
	  Temp=matrix_dim(npar,1);          // [npar,1]
	  Temp.fill((int)Tind(i,1));    // all elements set to Tind(i)
	  if(return_move)
	    batch=batch%((Parind)|(Parind)|Temp);
	  else
	    batch=batch%((Parind)|Temp);
	}
    }

  return(batch);
}


//---------------------------------------------------------------
matrix index_to_val(matrix& batch,matrix& tardir1,matrix& tardir2, matrix& par)
//---------------------------------------------------------------
{

  // Use after batch=index_batch
  
  matrix parad;
  matrix pardir1,pardir2;
  int i,pi,ti;
  
  pardir1=matrix_dim(1,1);
  pardir2=matrix_dim(1,1);
  for(i=1; i<=rows(batch); i++)
    {
      pi=batch(i,1);
      ti=batch(i,3);
      pardir1=pardir1%(par(pi)|tardir1(ti));
      pi=batch(i,2);
      pardir2=pardir2%(par(pi)|tardir2(ti));
    }      
  parad=parad%interleave(pardir1,pardir2);
  return(parad);

}

//---------------------------------------------------------------
matrix index_to_val(matrix& batch,matrix& tardir, matrix& par)
//---------------------------------------------------------------
{

  // Use after batch=index_batch

  matrix parad;
  matrix pardir;
  int i,pi,ti;
  
  pardir=matrix_dim(1,1);
  for(i=1; i<=rows(batch); i++)
    {
      pi=batch(i,1);
      ti=batch(i,2);
      pardir=pardir%(par(pi)|tardir(ti));
    }      
  parad=parad%pardir;
  return(parad);
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//                                                            //
//                  ARM + RIGID BODY ROUTINES                 //
//                                                            //
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

float vecang(matrix p,matrix q)
{
  p=p(1,1,3,1);
  q=q(1,1,3,1);
  return acos((T(p)*q)(1,1)/norm(p)/norm(q));
}

/***********************************************************************/
/* get global finger coordinates, wrt origin at shoulder              */
/***********************************************************************/  
matrix angtofi(float lu,float ll, float theta,
	       float eta, float chi, float phi)
{
  matrix finger(4,1);
  float xw,yw,zw;
  float xe,ye,ze;


  xe = - lu*sin(eta)*sin(theta);  // elbow coords
  ye =   lu*cos(eta)*sin(theta);
  ze = - lu*cos(theta);
  
  xw =xe -ll*(sin(phi)*(cos(chi)*sin(eta)*cos(theta)
	       + sin(chi)*cos(eta)) + cos(phi)*sin(eta)*sin(theta));
  yw =ye +ll*(sin(phi)*(cos(chi)*cos(eta)*cos(theta)
               - sin(chi)*sin(eta)) + cos(phi)*cos(eta)*sin(theta));
  zw =ze +ll*(sin(phi)*cos(chi)*sin(theta)- cos(phi)*cos(theta));

  finger(1,1) = xw;
  finger(2,1) = yw;
  finger(3,1) = zw;
  return finger;
}

/*****************************************************************/
matrix angtofi(matrix arm)
{
  return  angtofi(arm(1,1),arm(2,1),arm(3,1),arm(4,1),arm(5,1),arm(6,1));
}

/***********************************************************************/
/* get global elbow coordinates, wrt origin at shoulder              */
/***********************************************************************/  
matrix angtoel(float lu,float ll, float theta,
	       float eta, float chi, float phi)
{
  matrix elbow(4,1);
  float xe,ye,ze;


  xe = - lu*sin(eta)*sin(theta);  // elbow coords
  ye =   lu*cos(eta)*sin(theta);
  ze = - lu*cos(theta);
  
  elbow(1,1) = xe;
  elbow(2,1) = ye;
  elbow(3,1) = ze;
  return elbow;
}


/*****************************************************************/
matrix angtoel(matrix arm)
{
  return  angtoel(arm(1,1),arm(2,1),arm(3,1),arm(4,1),arm(5,1),arm(6,1));
}

/*****************************************************************/
void arm_init(struct armset &arm)
{
  arm.na=18; // number of markers on port 1 for arm (10 l/urig + 8 finger)
  arm.nu=4;
  arm.nl=6;
  arm.nf=8;
  
  matrix um(4,arm.nu);
  matrix lm(4,arm.nl);
  matrix fm(4,arm.nf);
  
  matrix_read("upper",um);
  matrix_read("lower",lm);
  matrix_read("finger",fm);
  
  arm.urig =  um;
  arm.lrig =  lm;
  arm.frig =  fm;
  
}


/*****************************************************************/
matrix mark2arm(matrix mframes, matrix mmark,matrix mseen,struct armset aset)    
{
  static int first =1;
  static matrix shoulder,elbow,elu,ell;
  static matrix finger;
  matrix fi(4,1);
  matrix sh(4,1);
  
  
  matrix el1,el2,el;
  matrix umark,lmark,fmark;
  matrix useen,lseen,fseen;  
  matrix Ru,Rl,Rf;
  matrix uvec,lvec,p;
  float lu,ll;
  float theta,phi,chi,eta,hrot;
  matrix fingercentroid(4,1);
  fingercentroid(4,1) = 1.0;
  
  float err;
  int it;
  int is;
  matrix arm1(1,11);
  matrix armall;
  
  long frames;
  int markers;
  
  frames=rows(mframes);
  markers=cols(mmark);
  armall=matrix_dim(1,1);
  
  
  if(first)       // read shoulder and elbow definitions in l/urig only once
    {
      first=0;
      matrix_read("shoulder.mat",shoulder);
      matrix_read("elbow.mat",elbow);
      elu=elbow(1,1,4,1);
      ell=elbow(5,1,8,1);
    }

  
  // run throuh the frames NB arm=(framesx4,nmarkers)
  // -------------------------------------------------
  is=0;
  for(it=0; it<frames; it++)
    {
      
      useen=mseen(it+1,1,it+1,aset.nu);        // are enough markers seen?
      lseen=mseen(it+1,aset.nu+1,it+1,aset.nu+aset.nl);
      fseen=mseen(it+1,aset.na-aset.nf+1,it+1,aset.na);

      /*    disp(useen);
      disp(lseen);
      disp(fseen);
      printf("aset.nu,nl,nf,na=%i,%i,%i,%i\n",aset.nu,aset.nf,aset.nl,aset.na);*/
      
      if (!(sum(useen) <3 || sum(lseen) <3 || sum(fseen) <3))
	{
	  is++;
	  umark=mmark(it*4+1,1,it*4+4,aset.nu);        // markers on urig
	  lmark=mmark(it*4+1,aset.nu+1,it*4+4,aset.nu+aset.nl); // markers on lrig
	  fmark=mmark(it*4+1,aset.na-aset.nf+1,it*4+4,aset.na);    // markers on frig
	  
	  Ru=findR(aset.urig(useen),umark(useen));        // only use seen markers
	  Rl=findR(aset.lrig(lseen),lmark(lseen));
	  Rf=findR(aset.frig(fseen),fmark(fseen));
	  
	  // Ru [Rl] is the transformation matrix which takes the reference frame
	  // urig [lrig] into the current positions; so mark=Ru*urig 


	  sh=Ru*shoulder;        // absolute coordinates shoulder
	  el2=Rl*ell;            // absolute coordinates elbow (calc from lrig)
	  el1=Ru*elu;            // absolute coordinates elbow (calc from urig)
	  el=(el1+el2)/2.0;      // absolute coordinates elbow (mean lrig + urig)

	  fi=Rf*fingercentroid;  // absolute coordinates finger
	  lu=norm(sh-el);       // length upper arm
	  ll=norm(fi-el);       // length lower arm
	  uvec=el-sh;           // vector upper arm (shoulder to elbow)
	  lvec=el-fi;           // vector lower arm (elbow to finger)
	  
	  p=crossprod(uvec,lvec); //normal to the plane of the arm
	  
/***********************************************************************/
/* get 3 shoulder angles as rotations around Z X Z axes (dynamic axes) */
/***********************************************************************/
	  
	  phi=M_PI-vecang(uvec,lvec);            // phi (from Soechting)
	  theta=acos(-uvec(3,1)/lu);             // elevation angle      
	  hrot= -atan2(p(3,1),norm(p(1,1,2,1))); // angle p makes with horiz plane
	  chi=asin(sin(hrot)/sin(theta));        // humeral rotation
	  eta=atan2(-uvec(1,1),uvec(2,1));       // yaw 
	  
	  arm1(1,1)=mframes(it+1,1);
	  arm1(1,2)=lu;
	  arm1(1,3)=ll;
	  arm1(1,4)=theta;
	  arm1(1,5)=eta;
	  arm1(1,6)=chi;
	  arm1(1,7)=phi;
	  arm1(1,8)=sh(1,1);
	  arm1(1,9)=sh(2,1);
	  arm1(1,10)=sh(3,1);
	  arm1(1,11)=sh(4,1);

	  finger    = angtofi(T(arm1(1,2,1,7))) +sh;                     // real finger
	  err=norm(fi-finger);
	  
	  if(err>0.1)
	    {
	      arm1(1,6)=M_PI-arm1(1,6);
	      finger    = angtofi(T(arm1(1,2,1,7))) +sh;                     // real finger
	    }
	  
	  armall=armall%arm1;
	  
	}
      
      
    }

  return armall;
  
}


#ifndef DOS

/*****************************************************************/
matrix mark2finger(matrix fmark,matrix fseen,struct armset aset)    
{
  static int first =1;
  matrix finger;
  static matrix Rf;
  static matrix fingercentroid(4,1);
  
  if(first)       
    {
      first=0;
      fingercentroid(4,1) = 1.0;
    }
  
  finger=matrix_dim(1,1);  
  if (!(sum(fseen) <3))
    {
      Rf=findR(aset.frig(fseen),fmark(fseen));
      finger=Rf*fingercentroid;  // absolute coordinates finger
    }

  return finger;
}


#endif


void _write_fileb(char *s,float **d,int rows,int cols,float samp)
{
  int str;
  if((str=_open(s,O_RDWR|O_BINARY|O_APPEND))==-1)
    {
      printf("%s cannot be _opened .\n",s);
      exit(0);
    }
  short int stmp;
  float ftmp;
  
  stmp= rows;
  if(_write(str,&stmp,sizeof(stmp))==-1) exit(0);   //rows
  
  stmp=cols;
  if(_write(str,&stmp,sizeof(stmp))==-1) exit(0);     //channels
  
  ftmp=samp;
  if(_write(str,&ftmp,sizeof(ftmp))==-1) exit(0);     //sampling interval
  
  if(_write(str,d,4*rows*cols)==-1) exit(0);
  
  _close(str);
}

/******************************************************************************/

matrix  findR( matrix &p, matrix &q )
{
static  matrix  rtmx;

    SPMX_rtmx(rtmx,p,q);
    return(rtmx);
}

/******************************************************************************/

void draw_stick_3d(matrix &pos)
{
  for(int eye=LEFT;eye<=RIGHT;eye++)
    {
      set_view_cal(pos,eye);
      glScalef(0.5,0.5,40);
      glTranslatef(0,0,0.5);
      set_color(GREEN);
      glutSolidSphere(1.0,10,10);
    }
}

/******************************************************************************/





