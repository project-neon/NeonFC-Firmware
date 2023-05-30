float last_error = 0;
float error_sum = 0;

float pid(float target, float atual){
	float kp = -0.125;
	float kd = 0;
	float ki = 0;

	float error = target - atual;
  error_sum += error;
 
  float P = error * kp;
  float I = error_sum * ki;
  float D = (error - last_error) * kd;
  
	float output = P+I+D;

	return output;
}
