/******************************************************************************/
/*                                                                            */
/* MODULE  : EKF.h                                                            */
/*                                                                            */
/* PURPOSE : Extended Kalman Filter (3rd order) class.                        */
/*                                                                            */
/* DATE    : 30/Jan/2009                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 30/Jan/2009 - Initial development.                               */
/*                                                                            */
/******************************************************************************/

#ifndef EKF_H
#define EKF_H

/******************************************************************************/

class EKF
{
public:

    STRING ObjectName;

    BOOL OpenFlag;
    BOOL FirstFlag;

    // Arm type (right or right configured)
    int ArmType;
#define EKF_ARM_UNKNOWN 0
#define EKF_ARM_RIGHT   1
#define EKF_ARM_LEFT    2

    TIMER *timer;
    double tlast;

#define EKF_DOF  3
#define EKF_DATA 5
    DATAPROC *data[EKF_DATA][EKF_DOF];

    TIMER_Frequency *freq;

    int dof;

    double w;
    double v;
    double dt;

#define EKF_LINK 2
    double link[EKF_LINK];

    matrix H;
    matrix Q;
    matrix R;
    matrix x;
    matrix P;
    matrix A;
    matrix xm;
    matrix Pm;
    matrix K;
    matrix J;
    matrix invJ;
    matrix V;
    matrix S;
    matrix invS;
    matrix z;
    matrix a;
    matrix px;

    EKF( char *name );
    EKF( );
   ~EKF( );

    void Init( void );
    void Init( char *name );
    void Init2D( void );
    void Init3D( void );

    BOOL Opened( void );

    void Open( int kf_dof, double kf_w, double kf_v, double kf_dt, double kf_link[], int ditems );
    void Open( int kf_dof, double kf_w, double kf_v, double kf_dt, double kf_link[] );
    void Close( void );

    void Calculate( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[], int &line );
    void Calculate( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[] );

    void Calculate2D( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[] );
    void Calculate2D( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[], int &line );

    void Calculate3D( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[], int &line );
    void Calculate3D( double xraw[], double dtime, double xfilt[], double dxfilt[], double ddxfilt[] );

    void Calculate( double xraw[], double xfilt[], double dxfilt[], double ddxfilt[], int &line );
    void Calculate( double xraw[], double xfilt[], double dxfilt[], double ddxfilt[] );

    BOOL DataSave( void );
    void DataFree( void );

    double MatrixDeterminant2x2( matrix &A );
    void MatrixInverse2x2( matrix &A, matrix &invA );
};

/******************************************************************************/

#endif

