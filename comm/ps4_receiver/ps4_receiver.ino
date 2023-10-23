//Biblioteca do controle de PS4
#include <PS4Controller.h>
#define NOTE_B0  31

//Setup pinos Locomocão Ponte H (TB6612fng)
#define BOARD_V1 1
#define BOARD_V2 2
#define BOARD BOARD_V2
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

#define MAC_ADDRESS "56:A4:53:A2:01:8F"

int PS4_L =  0;
int PS4_R2 = 0;
int PS4_L2 = 0;
int linear = 0;
int angular = 0;
int speed_limit = 90;
int first_mark;
int second_mark;
int third_mark;
int spinR = 0;
int spinL = 0;
int spinSpeed = 150;

void motor_R(int speedR) { // se o valor for positivo gira para um lado e se for negativo troca o sentido
  if(spinR) {
    speedR =  spinSpeed;
  }

  if(spinL) {
    speedR = -spinSpeed;
  }
  
  if (speedR > 0) {
    digitalWrite(A1, 1);
    digitalWrite(A2, 0);
  } else {
    digitalWrite(A1, 0);
    digitalWrite(A2, 1);
  }

  ledcWrite(1, abs(speedR));
}

void motor_L(int speedL) {
  if(spinR) {
    speedL = -spinSpeed;
  }

  if(spinL) {
    speedL =  spinSpeed;
  }

  if (speedL > 0) {
    digitalWrite(B1, 1);
    digitalWrite(B2, 0);
  } else {
    digitalWrite(B1, 0);
    digitalWrite(B2, 1);
  }

  ledcWrite(2, abs(speedL));
}

void motors_control(int linear, int angular) {
  angular += pid(angular, - get_theta_speed());

  float Vel_R = 0; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
  float Vel_L = 0;

  if ((linear == 0) && (angular != 0)){
    Vel_L = angular*(0.6);
    Vel_R = angular*(-1)*0.6;
  }
  else if (angular == 0){
    Vel_L = linear;
    Vel_R = linear;
  }
  else if (angular > 0){
    Vel_L = linear; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
    Vel_R = linear - (angular*0.6);
  }
  else if (angular < 0){
    Vel_L = linear + (angular*0.6);
    Vel_R = linear;
  }

  motor_R(Vel_R); //manda para a funcao motor um valor de -255 a 255, o sinal signifca a direcao
  motor_L(Vel_L);
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10); // will pause Zero, Leonardo, etc until serial console opens

  PS4.begin(MAC_ADDRESS);
  Serial.println("Ready.");

  //configuração de pinos
  
  ledcAttachPin(PWMA, 1);
  ledcAttachPin(PWMB, 2);

  ledcSetup(1, 80000, 8);
  ledcSetup(2, 80000, 8);

  #if BOARD == BOARD_V1
    pinMode(stby, OUTPUT);
    digitalWrite(stby, 1);
  #endif
    
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

  //configuração mpu
  mpu_init();

  while(PS4.isConnected()!= true) delay(20);

  first_mark = millis();
  third_mark = millis();
}

void loop() {
  while(PS4.isConnected()) {
    PS4_L =  PS4.LStickX();
    PS4_R2 = PS4.R2Value();
    PS4_L2 = PS4.L2Value();

    second_mark = millis();

    if(second_mark - first_mark > 1000) {
      speed_limit = 90;
    }

    if(PS4.Square()) {
      first_mark = millis();
      speed_limit = 150;
    }

    if (PS4_L < 15 && PS4_L > -15) PS4_L = 0;
    if (PS4_R2 < 15) PS4_R2 = 0;
    if (PS4_L2 < 15) PS4_L2 = 0;

    linear  = map(PS4_R2, 0, 255, 0, speed_limit) - map(PS4_L2, 0, 255, 0, speed_limit);
    angular = PS4_L;

    if(second_mark - third_mark > 500) {
      spinR = 0;
      spinL = 0;
    }

    if(!spinR && !spinL) {
      if(PS4.R1()) {
        third_mark = millis();
        spinR = 1;
      }

      if(PS4.L1()) {
        third_mark = millis();
        spinL = 1;
      }
    }

    motors_control(linear, angular);

    delay(3);
  }
  
  //Failsafe
  if(PS4.isConnected()!= true) {
    linear = 0;
    angular = 0;
    motors_control(0,0);
    Serial.println("Restart");
    PS4.end();
    ESP.restart();
    delay(20);
  } 
}
