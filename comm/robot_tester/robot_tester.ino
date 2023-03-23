#include <esp_now.h>
#include <WiFi.h>

//pin definitions
#define PWMA 32
#define PWMB 13
#define A1  25
#define A2  33
#define B1  26
#define B2  27

#define d1  35
#define d2  34
#define d3  39
#define d4  36

const int dip[4] = {35,34,39,36};

#define LOG Serial.print
#define ENDL Serial.println()

// This is de code for the board that is in robots
float v_l, v_a;
int first_mark, second_mark;


typedef struct recieved_message {
  int id;
  float v_l;
  float v_a;
} recieved_message;

recieved_message commands;

recieved_message robot;

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&commands, incomingData, sizeof(commands));
  // Update the structures with the new incoming data
  first_mark = millis();

  robot.id = commands.id;
  robot.v_l = commands.v_l;
  robot.v_a = commands.v_a;
  ENDL;
}

void motor_R(int speedR) { // se o valor for positivo gira para um lado e se for negativo troca o sentido
  if (speedR > 0) {
    digitalWrite(A1, 1);
    digitalWrite(A2, 0);
  } else {
    digitalWrite(A1, 0);
    digitalWrite(A2, 1);
  }
  ledcWrite(1, abs( speedR));
}
void motor_L(int speedL) {
  if (speedL > 0) {
    digitalWrite(B1, 1);
    digitalWrite(B2, 0);
  } else {
    digitalWrite(B1, 0);
    digitalWrite(B2, 1);
  }
  ledcWrite(2, abs( speedL));
}

void motors_control(float linear, float angular) {
  angular = pid(angular, - get_theta_speed());

  if (linear > 0 ) linear = map(linear, 0, 255, 60, 255);
  if (linear < 0 ) linear = map(linear, 0, -255, -60, -255);

  LOG("Id: ");
  LOG(robot.id); ENDL;
  LOG("V_L: ");
  LOG(linear); ENDL;
  LOG("V_A: ");
  LOG(angular); ENDL;
  

  float Vel_R = linear - angular; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
  float Vel_L = linear + angular;

  if (Vel_R < 15 && Vel_R > -15) Vel_R = 0;
  if (Vel_R > 255 ) Vel_R = 255;
  if (Vel_R < -255) Vel_R = -255;

  if (Vel_L < 15 && Vel_L > -15) Vel_L = 0;
  if (Vel_L > 255 ) Vel_L = 255;
  if (Vel_L < -255) Vel_L = -255;

  motor_R(Vel_R); //manda para a funcao motor um valor de -255 a 255, o sinal signifca a direcao
  motor_L(Vel_L);

}

void setup() {
  Serial.begin(115200);

  // configuração de pinos

  ledcAttachPin(PWMA, 1);
  ledcAttachPin(PWMB, 2);

  ledcSetup(1, 80000, 8);
  ledcSetup(2, 80000, 8);

  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  
  pinMode(d1, INPUT);
  pinMode(d2, INPUT);
  pinMode(d3, INPUT);
  pinMode(d4, INPUT);

  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

/*
  // configurações comunicação

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    LOG("Error initializing ESP-NOW"); ENDL
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  */

  // configuração mpu
  
  mpu_init();
  ina219_init();
  ws2812_init();
  test_current();
  dip_state();
}

void loop() {

  LOG("Test Start:"); ENDL;

  LOG("motors test:"); ENDL;
  for (int i = 0; i < 1; ++i)
  {
    
    LOG("motor A foward"); ENDL;
    motor_R(150);
    delay(1000);
    LOG("current A -----"); LOG(get_current()); ENDL;
    LOG("motor A backward"); ENDL;
    motor_R(-150);
    delay(1000);
    motor_R(0);
    LOG("motor B foward"); ENDL;
    motor_L(150);
    delay(1000);
    LOG("current B -----"); LOG(get_current()); ENDL;
    LOG("motor B backward"); ENDL;
    motor_L(-150);
    delay(1000);
    motor_L(0);
  }

  
   dip_state();

  ws2812_test();

}

void dip_state(){
  int state;

  LOG("DIP switch state:"); ENDL;

  for (int i = 0; i < 4; ++i)
  {
    state = digitalRead(dip[i]);
    LOG(state); LOG(",");
  }
 
}
