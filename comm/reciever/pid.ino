float ang_last, ang_total = 0, 0
float lin_last, lin_total = 0, 0

float pid_ang(float target){
    return pid(target, - get_theta_speed(), ang_ks, &ang_last, &ang_total);
}

float pid_lin(float target){
    return pid(target, - get_linear_speed(), lin_ks, &lin_last, &lin_total);
}

float pid(float target, float atual, float *ks, float *last, float *total){
    float error = target - atual;
    (*error_sum) += error;

    float P = error * ks[0];
    float I = error_sum * ks[1];
    float D = (error - last_error) * ks[3];

    float output = P+I+D;
    *last = error

    return output;
}
