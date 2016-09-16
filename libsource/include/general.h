// #define Set(name) assign(#name,name,var,val,fp);
#define MSEC(tod) (1000*(tod).tv_sec+(tod).tv_usec/1000.0)
// #define BATCH void assign_list(char *var, char *val, FILE *fp)
#define ARG int argc,char **argv
#define DB  fprintf(stderr,"Got to line %i in file %s\n",__LINE__,__FILE__);
#define DBK fprintf(stderr,"Got to line %i in file %s (ESCape to stop).\n",__LINE__,__FILE__); if( getch() == ESC ) exit(0);

#define ON 1
#define OFF 0

#define MV_TIM 1
#define MV_KEY 2
#define MV_POS 4
#define MV_VEL 8
#define MV_DIS 16

#define conf_limit(name) (name.CL(3,1))
#define limit(x)  (fabs(x) >15  ?  sgn(x)*15.0 : x)

#define MNONE 0
#define MMOVE 1
#define MPAR 2
#define MMOVE_PAR 3
#define RETURNMV 1
#define SINGLEMV 0


struct movement
{
  long int count;
  matrix initpos;
  matrix start; /* start position */
  matrix stop;  /*target position*/
  matrix Rf;    /* homogenous transformation -> stick orientation */
  matrix frig;
  matrix ipos;
  matrix opos;
  float dur;
  int hand;  /*hand marker on*/
  int end;   /*condition to end routine*/
  float hrad; /*hand marker radius*/
  float posth; /*positional accuracy threshold*/
  float velth; /*velocity threshold*/
  float disth; /*distance moved threshold*/
  float time; /*movement duration*/
  int done;   /* movement over*/
  void (*func)(struct movement &mv);
  matrix pos; /*marker positions */
  matrix dpos; /*displayed hand position */
  matrix hpos; /*hand position */
  matrix seen; /*hand position */
  matrix vel; /* hand velocity */
  float dist; /* hand velocity */
  float err; /* hand velocity */
  int oframe; /* frame number */
  int frame; /* frame number */
  int nm; /* numberof markers */
  int mustsee;
  int draw_markers;
  int use_robot;  //use sockets to get hand position
  int use_2m;   //use two markers on robot to get hand position
  double move_dur; //movement duration form first tim evel > 6.0
  int hmarker;  // marker used for hand
  float p1,p2,p3,p4,p5,p6,p7,p8,p9,p10 ; //paramters
	    
};


struct logit
{
  int unique;
  int count;
  float theta, beta; //current estimates of theshold and slope
  matrix D;          //data
  matrix B;          //b1+b2 x
  matrix C;          //covariance matrix
  matrix CL;         //confidence limits

};


struct armset
{
  int na;    // totol markers on arm
  int nu;   // markers on upper arm rigid body
  int nl;   // markers on lower arm rigid body
  int nf;   // markers on finger rigid body
  matrix urig;
  matrix lrig;
  matrix frig;
};



void quit();
void beep(int frequency,float dur,float amp);
void beep(int frequency,float dur);
void beep();
void sigint_int(int sig);
void sigint_fpe(int sig);
void sigint_segv(int sig);
void sigint_ill(int sig);
void signal_init();
void delay(float ms);
void clear_file(char *s);
void reset(struct logit &dir);
void logit_output(struct logit &dir,char *s);
void getval(char *,double&);
void getval(char *,int&);
void getval(char *,float&);
void getval(char *,char *);
void to_file(char *name, char *s);
void move_init(struct movement &mv);
void move_init(struct movement &mv,int N);
void move(struct movement &mv);
void moveit(struct movement &mv);
void move_stick(struct movement &mv);
void tone(float frequency, float amp);
void ComputeSinusoid(short *out, int length, double normalFrequency, double phase,double amplitude);
void sound(int on);
void playsound(char *name,float amp);

BYTE inportb( int port );
void outportb( int port, BYTE val );

void arm_init(struct armset &arm);
void draw_stick_3d(matrix &pos);
void write_fileb(char *s,float **d,int rows,int cols,float samp);

int exist(char *s);
int keyhit(char *s);
int keyhit();
int read_keys();
int getanswer(char *,int);

float prob(float x);
float invprob(float x);
float vecang(matrix p,matrix q);
float zest(struct logit &dir,float xmin,float xmax,matrix (*stimulus)(float),char *s);

double get_time();
double get_mtime();


unsigned char inportb(int port);

matrix mark2arm(matrix mframes, matrix mmark,matrix mseen, struct armset aset);
matrix mark2finger(matrix fmark,matrix fseen,struct armset aset);

void    matrix_rtmx( matrix &rtmx, matrix &xyz1, matrix &xyz2 );
matrix  findR( matrix &p, matrix &q );

matrix glm(struct logit &dir);
matrix index_batch(int nmove,int npar, int mix, int return_move);
matrix index_to_val(matrix& batch,matrix& tardir, matrix& par);
matrix angtofi(matrix arm);
matrix angtofi(float lu,float ll, float theta,float eta, float chi, float phi);
matrix angtoel(matrix arm);
matrix angtoel(float lu,float ll, float theta,float eta, float chi, float phi);
matrix index_to_val(matrix& batch,matrix& tardir1,matrix& tardir2, matrix& par);

void tone(unsigned frequency, int on );
