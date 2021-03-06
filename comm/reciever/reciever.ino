#include <esp_now.h>
#include <WiFi.h>

//pin definitions
#define PWMA 19
#define PWMB 27
#define A1  5
#define A2  32
#define B1  25
#define B2  26
#define stby 33

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
  Serial.println();
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

  Serial.print("Id: ");
  Serial.println(robot.id);
  Serial.print("V_L: ");
  Serial.println(linear);
  Serial.print("V_A: ");
  Serial.println(angular);
  Serial.println();

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

  // configura????o de pinos

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

  // configura????es comunica????o

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  
  // configura????o mpu

  mpu_init();
}

void loop() {
  second_mark = millis();

  v_l = robot.v_l;
  v_a = robot.v_a;

  if (second_mark - first_mark > 500) {
    v_l = 0.00;
    v_a = 0.00;
  }

  motors_control(v_l, v_a); //aplica os valores para os motores
}
