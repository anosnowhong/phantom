#define BASE_ADDR 0x0280

#define ON 1
#define OFF 0

#define PANIC digital_in(0)


#ifdef BORCPP


#define reset_encoder(axis) {\
        outportb(12+BASE_ADDR, 255-(1<<(axis)));\
        outportb(12+BASE_ADDR,255);}

#define torque(axis,current) {  \
        outportb(axis*2+1+BASE_ADDR,(char) (current >> 8)); \
        outportb(axis*2+BASE_ADDR,(char) (current & 0xff));}

#define digital_in(n) ((inportb(12+BASE_ADDR)>>n) & 0x0001)

#else

#define reset_encoder(axis) {\
        _outp(12+BASE_ADDR, 255-(1<<(axis)));\
        _outp(12+BASE_ADDR,255);}

#define torque(axis,current) {  \
        _outp(axis*2+1+BASE_ADDR,(current >> 8)); \
        _outp(axis*2+BASE_ADDR,current & 255);}

#define digital_in(n) ((_inp(12+BASE_ADDR)>>n) & 0x0001)
#endif

long int *put_ced_force(float fx,float fy,float fz,float ramp);
long int *put_ced_force_dm(float fx,float fy,float fz,float ramp);
long int *put_ced_force(matrix&  force,float ramp);
void amp(int status);
long encoder(int axis);
long raw_encoder(int axis);
void read_robot(matrix &pos);
void read_raw_robot(matrix &pos);
void read_robot(float &x,float &y,float &z);
void read_robot_angles(float &t1,float &t23,float &t2);
void read_robot(double &x,double &y,double &z);
void read_raw_robot(double &x,double &y,double &z);
void put_torque(int t1,int t2,int t3);
void overheat();
matrix  force_box(matrix &pos,matrix& orig, matrix& box,float k);
matrix solid_box(matrix &pos,matrix& orig, matrix& box, matrix &zunit,float k);
void solid_box(float *force, float *pos,float* orig,float* box, float *zunit,float k);
matrix  force_sphere(matrix &pos,matrix & c,float k);
matrix spring(matrix &pos,matrix& orig,matrix k);
;
long int *put_force(float fx,float fy,float fz,float ramp);
void temp_check(float servo_rate,  long Tm[], int axis);
matrix  force_outbox(matrix &pos,matrix& orig, matrix& box,float k);
long int *put_force(matrix &force,float ramp);
matrix mass_spring(matrix &pos, matrix &M,float dt);
matrix vel_est(matrix &pos,int window,int reset);
matrix acc_est(matrix &vel,int window,float dt,int reset);
void read_phantomcal();
void read_marker(matrix &pos);
matrix wall(matrix pos, matrix c,float ks);
