float pid(float target, float atual, int *iterations, float *error_total){
	float error = target - atual;
  error_sum += error;

  (*iterations)++;
  *error_total = (((*error_total) * ((*iterations) - 1)) + error) / (*iterations);
 
  float P = error * kp;
  float I = error_sum * ki;
  float D = (error - last_error) * kd;
  
	float output = P+I+D;

	return output;
}
