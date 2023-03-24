//Biblioteca do controle de PS4
#include <PS4Controller.h>

//Setup pinos Locomocão Ponte H (TB6612fng)
#define BOARD_V1 1
#define BOARD_V2 2
#define BOARD BOARD_V1
//pin definitions for board V1
#if BOARD == BOARD_V1
  #define PWMA 19
  #define PWMB 27
  #define A1    5
  #define A2   32
  #define B1   25
  #define B2   26
  #define stby 33
//pin definitions for board V2
#elif BOARD == BOARD_V2
  #define PWMA 32
  #define PWMB 13
  #define A1   25
  #define A2   33
  #define B1   26
  #define B2   27
#else
  #error Board not defined!
#endif

void motor_R(int speedR) { // se o valor for positivo gira para um lado e se for negativo troca o sentido
  if (speedR > 0) {
    digitalWrite(A1, 1);
    digitalWrite(A2, 0);
  } else {
    digitalWrite(A1, 0);
    digitalWrite(A2, 1);
  }
  ledcWrite(1, abs(speedR) );
}

void motor_L(int speedL) {
  if (speedL > 0) {
    digitalWrite(B1, 1);
    digitalWrite(B2, 0);
  } else {
    digitalWrite(B1, 0);
    digitalWrite(B2, 1);
  }
  ledcWrite(2, abs(speedL) );
}

void motors_control(int linear, int angular) {
  angular = pid(angular, - get_theta_speed() );

  if (linear > 0 ) linear = map(linear, 0,  255,  60,  255);
  if (linear < 0 ) linear = map(linear, 0, -255, -60, -255);
  
  float result_R = linear - angular; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
  float result_L = linear + angular;  
  
  //não envia valores de potencia abaixo de 15, que não são fortes o suficiente para mover o robo
  if(result_R < 15 && result_R > -15) result_R = 0; 
  if(result_L < 15 && result_L > -15 ) result_L = 0;
  //Não permite valores superiores a 255 ou inferiores a -255
  if(result_R >  255 ) result_R =  255; 
  if(result_R < -255 ) result_R = -255;
  if(result_L >  255 ) result_L =  255;
  if(result_L < -255 ) result_L = -255;  
  
  //manda para a funcao motor um valor de -255 a 255, o sinal signifca a direcao  
  motor_R(result_R); 
  motor_L(result_L);
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10); // will pause Zero, Leonardo, etc until serial console opens

  PS4.begin("b0:05:94:46:2d:7c");
  Serial.println("Ready.");
  
  ledcAttachPin(PWMA, 1);
  ledcAttachPin(PWMB, 2);

  ledcSetup(1, 80000, 8);
  ledcSetup(2, 80000, 8);

  pinMode(stby, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  digitalWrite(stby, 1);
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

  mpu_init();

  while(PS4.isConnected()!= true) delay(20);
}

void loop() {
  while(PS4.isConnected()) {
  
    if(PS4.LStickY() < -25 || PS4.LStickY() > 25){
      motors_control((1.8)*PS4.LStickY(),(1.3)*PS4.RStickX());

    } else { // Controle sobre valores pequenos devido a problemas na funcao map
      motors_control(0, 0);

    }
  }
  
  //Failsafe
  if(PS4.isConnected()!= true) {
  motors_control(0,0);
  Serial.println("Restart");
  PS4.end();
  ESP.restart();
  delay(20);
  } 
}
