/******************************************************************************/
/*                                                                            */
/* MODULE  : KalmanFilter.h                                                   */
/*                                                                            */
/* PURPOSE : Kalman Filter class.                                             */
/*                                                                            */
/* DATE    : 24/Apr/2007                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 24/Apr/2007 - Initial development.                               */
/*                                                                            */
/* V2.0  JNI 24/Mar/2015 - Re-visit for 3BOT.                                 */
/*                                                                            */
/******************************************************************************/

#ifndef KALMANFILTER_H
#define KALMANFILTER_H

/******************************************************************************/

class KALMANFILTER
{
public:

    STRING ObjectName;

    BOOL OpenFlag;
    BOOL FirstFlag;

    TIMER *timer;
    double tlast;

#define KALMANFILTER_DATA 4
    DATAPROC *data[KALMANFILTER_DATA];

    TIMER_Frequency *freq;
    TIMER_Interval *latency;

    double w;
    double v;
    double dt;

    matrix H;
    matrix Q;
    matrix R;
    matrix x;
    matrix P;
    matrix A;
    matrix xm;
    matrix Pm;
    matrix K;

    double pos;
    double vel;
    double acc;

    double P11;
    double P12;
    double P13;
    double P21;
    double P22;
    double P23;
    double P31;
    double P32;
    double P33;

    KALMANFILTER( char *name );
    KALMANFILTER( );
   ~KALMANFILTER( );

    void Init( void );
    void Init( char *name );

    BOOL Opened( void );

    void Open( double kf_w, double kf_v, double kf_dt, int ditems );
    void Open( double kf_w, double kf_v, double kf_dt );
    void Close( void );

    void OldCalculate( double xraw, double dtime, double &xfilt, double &dxfilt );
    void Calculate( double xraw, double dtime, double &xfilt, double &dxfilt );
    void Calculate( double xraw, double &xfilt, double &dxfilt );

    BOOL DataSave( void );
    void DataFree( void );
};

/******************************************************************************/

#endif

