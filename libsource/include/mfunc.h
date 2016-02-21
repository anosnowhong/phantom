/*
 * MATLAB Compiler: 2.1
 * Date: Sat Apr 13 16:46:09 2002
 * Arguments: "-B" "macro_default" "-O" "all" "-O" "fold_scalar_mxarrays:on"
 * "-O" "fold_non_scalar_mxarrays:on" "-O" "optimize_integer_for_loops:on" "-O"
 * "array_indexing:on" "-O" "optimize_conditionals:on" "-t" "-L" "C" "-I"
 * "..\m" "pivot.m" "minq.m" "minqdef.m" "pr01.m" "getalp.m" "-W" "lib:mfunc" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __mfunc_h
#define __mfunc_h 1

#ifdef __cplusplus
extern "C" {
#endif

#include "libmatlb.h"

extern void InitializeModule_mfunc(void);
extern void TerminateModule_mfunc(void);
extern mxArray * mlfPivot(mxArray * * err, mxArray * p);
extern void mlxPivot(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);
extern mxArray * mlfMinq(mxArray * * fct,
                         mxArray * * ier,
                         mxArray * * nsub,
                         mxArray * gam,
                         mxArray * c,
                         mxArray * G,
                         mxArray * xu,
                         mxArray * xo,
                         mxArray * prt,
                         mxArray * xx);
extern void mlxMinq(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);
extern mxArray * mlfMinqdef(mxArray * * y,
                            mxArray * * ier,
                            mxArray * c,
                            mxArray * G,
                            mxArray * A,
                            mxArray * b,
                            mxArray * eq,
                            mxArray * prt,
                            mxArray * xx);
extern void mlxMinqdef(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);
extern mxArray * mlfPr01(mxArray * name, mxArray * x);
extern void mlxPr01(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);
extern mxArray * mlfGetalp(mxArray * * lba,
                           mxArray * * uba,
                           mxArray * * ier,
                           mxArray * alpu,
                           mxArray * alpo,
                           mxArray * gTp,
                           mxArray * pTGp);
extern void mlxGetalp(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);
extern void mfuncInitialize(void);
extern void mfuncTerminate(void);

#ifdef __cplusplus
}
#endif

#endif
