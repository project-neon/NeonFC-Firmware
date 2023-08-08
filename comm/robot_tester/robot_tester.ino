#include <ESPmDNS.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

uint8_t broadcast_adr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

const byte numChars = 64;
char receivedChars[numChars];
char tempChars[numChars];   
boolean newData = false;     
int id, count;

//==============

esp_now_peer_info_t peerInfo;

//pin definitions
#define PWMA 32
#define PWMB 13
#define A1  25
#define A2  33
#define B1  26
#define B2  27
//#define S1  16

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
float lastValue = 0;

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

//  pinMode(S1, OUTPUT);
 
  pinMode(d1, INPUT);
  pinMode(d2, INPUT);
  pinMode(d3, INPUT);
  pinMode(d4, INPUT);

  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));
  esp_wifi_set_max_tx_power(84);

  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcast_adr, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) 
  {
    Serial.println("Failed to add peer");
    return;
  }

  // configuração mpu
  
  mpu_init();
  ina219_init();
  ws2812_init();
  test_current();
}

void loop() {

  LOG("Test Start:"); ENDL;
  LOG(get_theta_speed()); ENDL;
  
  LOG("motors test:"); ENDL;

//  tone(S1, 1000);
//  delay(1000);
//  noTone(S1);
  
  for (int i = 0; i < 1; ++i)
  { 
    LOG("motor A forward"); ENDL;
    motor_R(150);
    delay(1000);
    // LOG("current A -----"); LOG(get_current()); ENDL;
    // LOG("voltage -------"); LOG(get_voltage()); ENDL;
    LOG("motor A backward"); ENDL;
    motor_R(-150);
    delay(1000);
    motor_R(0);
    LOG("motor B forward"); ENDL;
    motor_L(150);
    delay(1000);
    // LOG("current B -----"); LOG(get_current()); ENDL;
    // LOG("voltage -------"); LOG(get_voltage()); ENDL;
    LOG("motor B backward"); ENDL;
    motor_L(-150);
    delay(1000);
    motor_L(0);
  }

//  dip_state();

//  ws2812_test();

//  sendData();
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

void sendData() {   
    // esse delay é necessário para que os dados sejam enviados corretamente
    esp_err_t message = esp_now_send(broadcast_adr, (uint8_t *) &commands, sizeof(commands));
    delay(3);
}
