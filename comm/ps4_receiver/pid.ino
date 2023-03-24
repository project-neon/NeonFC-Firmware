float pid(float target, float atual){
	float kp = 1; // motor antigo, motor novo kp = .25
	float kd = 0;
	float ki = 0;

	float error = target - atual;

	float output = error * kp ;

	return output;

}
