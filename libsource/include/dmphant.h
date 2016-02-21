#define BASE_ADDR 0x0280

#define ON 1
#define OFF 0

#define reset_encoder(axis) {\
	outportb(12+BASE_ADDR, 255-(1<<(axis)));\
	outportb(12+BASE_ADDR,255);}

#define torque(axis,current) {  \
	outportb(axis*2+1+BASE_ADDR,(char) (current >> 8)); \
	outportb(axis*2+BASE_ADDR,(char) (current & 0xff));}

#define digital_in(n) ((inportb(12+BASE_ADDR)>>n) & 0x0001)
#define PANIC (digital_in(0))
#define STYLUS_SW (digital_in(1))        /* active low */
long encoder(int axis);
void put_torque(int t1,int t2,int t3);
void overload();
long int *put_force(float forcex,float forcey,float forcez,float ramp);
void amp(int status);
void overheat();
matrix  force_sphere(matrix& pos, matrix& orig,float k);
matrix  sphere(matrix & orig,float radius);
matrix spring(matrix& orig,float k);
matrix spring(matrix& orig,matrix k);
void read_robot(matrix &pos);
matrix  force_sphere(matrix &pos,matrix & c,float k);
matrix spring(matrix &pos,matrix& orig,matrix k);
long int *put_force(matrix &force,float ramp);
double get_time();

// Dematrified:
float *temp_check(float servo_rate,  long Tm[], int axis);
void read_robot(float *x,float *y,float *z);
void force_box(float force[], float pos[], float orig[], float box[], float k);
void vel_simp(float vel[],float pos[],float twindow,float dt,int reset);
void vel_est(float vel[],float pos[],float twindow,float dt,int reset);
void acc_est(float acc[],float pos[],float twindow,float dt,int reset);
void signal_init();

#define hfz (1.323)  // (135.0*9.8/1000)
