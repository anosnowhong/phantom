#ifndef GRAPHICS_OLD_H
#define GRAPHICS_OLD_H
#define frust_mono      GRAPHICS_Frustum[EYE_MONO]
#define frust_left      GRAPHICS_Frustum[EYE_LEFT]
#define frust_right     GRAPHICS_Frustum[EYE_RIGHT]

#define p2s             GRAPHICS_CalibMatrix

#define myReshape       GRAPHICS_Reshape

#define frustum_init    GRAPHICS_FrustumInit


#define LEFT 0   // for use here
#define RIGHT 1  // for use here
#define MONO 2   // for use here

#define FINGER 4
#define TFINGER 6
#define CIRCLE 2
#define SQUARE 3
#define PLAINFINGER 5

#define LINEA 123

#define D2 2
#define D3 3

#define LINE  1
#define POINT 2
#define FILL  3

#define STEREO 1

#define SOLID 0
#define WIRE 1

#define LON 0
#define LOFF 1

#define XOR 0
#define COPY 1

#define PERP 0
#define ORTHO 1

#define FINGER_SHAPE 100

#define SPHERE 101
#define DISC 102
#define BOX_SHAPE 103
#define RECTANGLE 104
#define TEAPOT 105
#define ICOS 106
#define OCT 108
#define CIRCLE_SHAPE 109
#define CYLINDER 110
#define INFUNC 107

//Spaceball buttons
#define REST    0
#define CLICK   1
#define RELEASE 2

// #define HPIX 1280
// #define VPIX 1024

// mid eye position in frame calibrates (rot.mat) coordinates (changed 23-11-01)...
// #define EX 0.0
// #define EY 0.0
// #define EZ 0.0
// #define ESEP 3.0   //half eye separation

// screen in frame calibrates coordinates (minor modifs by DW 23-11-01)...
// #define SZ -42.0   // z    
// #define SL -50.0   // xmin
// #define SR 50.0    // xmax
// #define ST 70.0    // ymax
// #define SB 0.0     // ymin

// #define SZ -25.0   // z    
// #define SL -20.0   // xmin
// #define SR 20.0    // xmax
// #define ST 40.0    // ymax
// #define SB 10.0     // ymin

//#ifdef DOS
//void CALLBACK reshape(GLsizei w, GLsizei h);
//#else
void myReshape(int w, int h);
//#endif

void old_check_event(void);
void check_event( BOOL SpaceWare );
void check_event(void);
void clearRGB(void);
void clearRGB1(void);
void clearDEPTH(void);
void clearSCREEN(void);
void clearSCREEN1(void);
void mySolidSphere(float radius, int depth);
void myWireSphere(float radius, int depth);
void print_set_up(void);
void printString(char *s, float x, float y, float z);
void set_view_cal(float x, float y, float z, int eye);
void set_view(float x, float y, float z, int eye);
void draw_finger_mono(matrix &p,float radius);
void frustum_init();
void  build_finger();
void  build_tfinger();
void xor_mode(int on);
void draw_finger_2d(matrix &p,float radius);
void draw_finger_3d(matrix &p,float radius);
void my_draw_finger(matrix &p,float radius);
void my_draw_finger_3d(matrix &p,float radius);
void my_draw_finger_2d(matrix &p,float radius);
void swapbuffers(void);
void translate(matrix pos);
void draw_finger_mono(matrix &p,float radius);
void mypoint(matrix &p,float radius);
void put_string(char *string,float size,float line);
void put_string(char *string);
void put_string_table(char *string,float size,float line);
void put_string_table(char *string);
void set_view_cal(matrix &p,int eye);
void set_view(matrix &p,int eye);
void polygon(int mode);
void my_circle(matrix &p,float radius);
void my_square(matrix &p,float radius);
void my_rectangle(matrix &p,float w,float h);
void my_point(matrix &p,float radius);
void build_circle(int sm);
void build_square();
void material_map(int colour);
void draw_arm(matrix &p,float radius);
void translate(float x,float y, float z);
void rest();
void rest_time(float s);
void rest_time2d(float s);
void ortho_init();
void draw_markers(matrix &seen);
void my_clear_finger();
void my_clear_finger_3d();
void my_clear_finger_2d();
void ep_time(float s);
void put_string_stereo(float x, float y,float sz,char *string);
void draw_markers(matrix &seen, int panic);
void my_draw_rgb_finger(matrix &p,float radius, matrix &col);
void my_clear_rgb_finger(float radius);
void draw_rgb_finger(matrix &p,float radius, matrix &col);
void build_plain_finger();
void start_index_2d();
void start_graphics_table();
void start_graphics_3d();
void start_index(int);
void new_color_init();
void stereo(int);
void my_sphere(matrix &pos, float radius, int colour, int depth);
void my_sphere_2d(matrix &pos, float radius, int colour, int depth);
void my_sphere_3d(matrix &pos, float radius, int colour, int depth);
int gmode();
void color_2d(int color);



void set_view_table(matrix &p);
void set_view_table(float x, float y, float z);
void set_view_table(float x, float y);
matrix cal2d_table();

void my_rectangle(float p1, float p2, float p3,float w,float h);
void my_arrow(matrix &p, float length);
void my_arrow(float p1, float p2, float p3, float length);
void color_3d(int color);
void my_wire_sphere_3d(matrix &pos, float radius, int colour, int depth);
void my_wire_sphere(matrix &pos, float radius, int colour, int depth);
void cube(matrix &pos,float sx,float sy,float sz,int c) ;
void cube(matrix &pos,float sx,float sy,float sz,int c,float ori);


float **color_init();

matrix cal_init(char *s);
matrix cal_init();
matrix cal2d_init();


#define GRAPHICS_EXIT  printf("Got to  line %i in file %s ",__LINE__,__FILE__),  printf("\n"),exit_graphics();

struct shape
{
      int d2_3;          //  D2/D3
      int mono_stereo;  //  MONO/STEREO
      int xor_copy;      // XOR/COPY
      int solid_wire;     //  SOLID/WIRE  
      int lon_loff;      //  LON/LOFF lights on or off
      int color;
      int comp;
      int nbuff;
      int obtype;
      int use_cal;           // 0/1  calibration off or on
      int perp_ortho;    // PERP/ORTHO perspective/othographic projection
      float shiny;
      void (*func)(struct shape &ob);
      char *str;    // string for text;
      matrix rot;   // 4xn =s n four element rotation vectors NB last in first to apply
      matrix rad;
      matrix pos;
    };


void show(struct shape &ob);
void shape_init(struct shape &ob);
void shape_default(struct shape &ob);
void shape_disc(struct shape &ob);
void shape_sphere(struct shape &ob);
void shape_icos(struct shape &ob);
void shape_oct(struct shape &ob);
void shape_finger(struct shape &ob);
void shape_rect(struct shape &ob);
void shape_box(struct shape &ob);
void shape_point(struct shape &ob);
void shape_teapot(struct shape &ob);
void shape_draw_markers(matrix &seen);
void finger_shape_init(struct shape &ob);
void draw_shape_finger(matrix &p,float radius);
void clear_shape_finger();
void exit_graphics();
void set_view_ortho2d(float x, float y);
void set_view_ortho2d(matrix &p);
void show_shape(struct shape &ob);
void shape_octtriangle(struct shape &ob);
void shape_string(struct shape &ob);
void shape_put_string(float p1, float p2, float p3,float s, char *string, int col);
void shape_put_string(matrix &p,float sz,char *string, int col);
void shape_put_string(float fx, float fy, float sz, char *string, int col);
void shape_draw_markers(matrix &seen, int col, int nbuff);
void shape_torus(struct shape &ob);
void set_color(int c);
void shape_put_string1(float fx, float fy, float sz, char *string, int col);
void build_dos_finger();
void make_gridI(float xmin,float xmax,float ymin,float ymax,float zpos);
void draw_gridI();
void draw_gridIII(int colour);
void my_explode(matrix pos,int colour, float radius);
void make_gridII(float xmin,float xmax,float ymin,float ymax,float zpos,int which,int number);
void draw_gridII();
void draw_line(int colour, float xmin, float xmax, float y, float z);
void stick_3d(matrix &Rf);
void draw_stick_3d(matrix &Rf);
void clear_stick_3d();
void cube_3d(matrix &pos, float x, float y, float, int colour, int depth,float rotz);
void cube_3dw(matrix &pos, float x, float y, float z, int colour);
void cube_3dy(matrix &pos, float x, float y, float z, int colour, float roty);
void mySolidCylinder (int rd);

void FreeTexture( GLuint texture );
GLuint LoadTextureRaw( const char * filename, int wrap, int width, int height);
void restb();
void put_info(char *string,float size,matrix &pos);
void start_graphics_2d();
void SpaceballButtonFunc(void (*funk)(int,int));
void SpaceballMotionFunc(void (*funk)(int,int,int));
void SpaceballRotateFunc(void (*funk)(int,int,int));
int sbinit();
#endif


