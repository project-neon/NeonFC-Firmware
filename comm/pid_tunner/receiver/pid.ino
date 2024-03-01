float pid_ang(float target, float *err){
    return pid(target, - get_theta_speed(), ang_ks, &ang_last, &ang_total, err);
}

float pid_lin(float target, float *err){
    return pid(target, - get_linear_speed(), lin_ks, &lin_last, &lin_total, err);
}

float pid(float target, float atual, float *ks, float *last, float *total, float *err){
    float error = target - atual;
    (*err) += error;
    (*total) += error;

    float P = error * ks[0];
    float I = (*total) * ks[1];
    float D = (error - *last) * ks[2];

    float output = P+I+D;
    *last = error;

    return output;
}
