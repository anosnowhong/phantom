void opto_dld(char *s);
void opto_init(char *s);
void opto_init();
void start_opto(int freq,int s1, int s2, int s3, int s4);
void stop_opto();
void activate();
void deactivate();
void readframe();
void readframe(char *s);
int get_pos(matrix& pos, matrix& seen, int num);
int get_pos_nc(matrix& pos, matrix& seen, int num);
int get_rpos(matrix& pos, matrix& seen, int num);
void deinit();
void init_buffering(int s1,float hz);
void init_buffering(int s1,int s2,float hz);
void init_buffering(int s1,int s2,int s3,int s4, float hz);
void init_buffering_ext(int markers, float hz);
void start_buffering();
int spool_update();
void stop_buffering();
void opt2file(char *s);
matrix opt2mat();
void spool_unpack_fp(FILE *fp, int markers, float ctime, float hz);
void spool_unpack_file(char *s, int markers, float ctime, float hz);
void opto2_init();
void init_timed_buffering(int s1,float t, float hz);
matrix set_rigid(int m1, int m2);
int get_pos(matrix& pos, matrix& seen, int m1, int m2);
int get_rigid(matrix &R,matrix gotpos,int m1,int m2);
int get_seen_pos(matrix& pos, matrix& seen);
float opto_freq();
void opt2matseen(matrix& Aframes,matrix& A,matrix& Aseen);
void opt2file_mark(char *s,int No, int Nf);
void arm2file(char *s, matrix arm);
void opt2file_arm(char *s,struct armset aset);
int opt2data(int **data, matrix& frig, matrix &fcent);
int opt2dataII(int **data, matrix& frig,matrix &transform,matrix &orig);
int opt2dataIII(int **data, matrix& frig,matrix &transform,matrix &orig);
int opt2dataIIIa(float **data,matrix &frig);
int opt2data_alex(float **data);
int opt2dataIV(float **data, matrix &frig);
void InitBuffering(int s1,int s2,int s3,int s4, float freq,float **OptoBuffer);
void InitBufferingExt(int markers, float freq);















