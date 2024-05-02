#include <global_variable.h>

#if !defined _kalman_filter_h_
#define _kalman_filter_h_
float x_kalman(float Xvalue)
{
    X_XT_update = X_XT_prev;
    X_PT_update = X_PT_prev + Q;

    X_KT = X_PT_update / (X_PT_update + R);
    X_XT = X_XT_update + (X_KT * (Xvalue - X_XT_update));

    X_PT = (1 - X_KT) * X_PT_update;
    X_XT_prev = X_XT;
    X_PT_prev = X_PT;
    return X_XT;
}

float y_kalman(float Yvalue)
{
    Y_XT_update = Y_XT_prev;
    Y_PT_update = Y_PT_prev + Q;

    Y_KT = Y_PT_update / (Y_PT_update + R);
    Y_XT = Y_XT_update + (Y_KT * (Yvalue - Y_XT_update));

    Y_PT = (1 - Y_KT) * Y_PT_update;
    Y_XT_prev = Y_XT;
    Y_PT_prev = Y_PT;
    return Y_XT;
}

float z_kalman(float Zvalue)
{
    Z_XT_update = Z_XT_prev;
    Z_PT_update = Z_PT_prev + Q;

    Z_KT = Z_PT_update / (Z_PT_update + R);
    Z_XT = Z_XT_update + (Z_KT * (Zvalue - Z_XT_update));

    Z_PT = (1 - Z_KT) * Z_PT_update;
    Z_XT_prev = Z_XT;
    Z_PT_prev = Z_PT;
    return Z_XT;
}

#endif