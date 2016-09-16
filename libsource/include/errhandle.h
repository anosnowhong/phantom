/*****************************************************************/
/***********              errhandle.h                *************/
/***********    Handles Phantom I/O Library Errors   *************/
/*********** Author: Gary Planthaber       June 1996 *************/
/***********        SensAble Technologies, Inc.      *************/
/*****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* print an error to stdout */
void print_phantom_error(int err_num, int phantom_id, char *user_str);


/* Disable phantom, print error message and quit program */
void handle_phantom_error(int err_num, int phantom_id, char *user_str);

#ifdef __cplusplus
}
#endif

