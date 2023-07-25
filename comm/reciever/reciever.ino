#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

//pin definitions for board V1
/*
#define PWMA 19
#define PWMB 27
#define A1  5
#define A2  32
#define B1  25
#define B2  26
#define stby 33
*/

//pin definitions for Placa V2
#define PWMA 32
#define PWMB 13
#define A1  25
#define A2  33
#define B1  26
#define B2  27

// This is de code for the board that is in robots
int robot_id = 2;
int id;
int first_mark = 0, second_mark;

float v_l, v_a;
float last_error = 0;
float error_sum = 0;

float kps[3] = {2.0317307, 1.2771797, 1.702362};

const byte numChars = 64;
char commands[numChars];
char tempChars[numChars];
int header;


typedef struct struct_message{
  int header;
  char message[64];
  } struct_message;

struct_message rcv_commands;

float wheelRadius = 0.035/2;
float robotRadius = 0.075/2;
float ks = 0.99;
float kv = 0.1736;
float speedMin = 0.05;


void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&rcv_commands, incomingData, sizeof(rcv_commands));
  // Update the structures with the new incoming data
  first_mark = millis();
  strcpy(commands, rcv_commands.message);
  header = rcv_commands.header;
  // blink_led();
}


void motor_R(float speedR) { // se o valor for positivo gira para um lado e se for negativo troca o sentido
  if (speedR > 0) {
    digitalWrite(A1, 1);
    digitalWrite(A2, 0);
  } else {
    digitalWrite(A1, 0);
    digitalWrite(A2, 1);
  }

  speedR = abs(speedR);
  float angular = speedR/wheelRadius;
  float voltage = ks + kv*angular;
  float pwm = map(voltage, 0, get_voltage(), 0, 255);
  if(pwm > 255) pwm = 255;
  if(speedR < speedMin) pwm = 0;

  ledcWrite(1, pwm);
}

void motor_L(float speedL) {
  if (speedL > 0) {
    digitalWrite(B1, 1);
    digitalWrite(B2, 0);
  } else {
    digitalWrite(B1, 0);
    digitalWrite(B2, 1);
  }

  speedL = abs(speedL);
  float angular = speedL/wheelRadius;
  float voltage = ks + kv*angular;
  float pwm = map(voltage, 0, get_voltage(), 0, 255);
  if(pwm > 255) pwm = 255;
  if(speedL < speedMin) pwm = 0;

  ledcWrite(2, pwm);
}


void motors_control(float linear, float angular) {
  angular = angular + pid(angular, - get_theta_speed());

  float Vel_R = linear - robotRadius * angular; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
  float Vel_L = linear + robotRadius * angular;

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

  //pinMode(stby, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  //digitalWrite(stby, 1);
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

  // configurações comunicação

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK( esp_wifi_start());
  ESP_ERROR_CHECK( esp_wifi_set_channel(14, WIFI_SECOND_CHAN_NONE));

  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  
  // configuração mpu

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  mpu_init();

  ws2812_init();
}


void loop() {
  if(header == 1910){
    second_mark = millis();
    strcpy(tempChars, commands); // necessário para proteger a informação original
    parseData();
    header = 0;
  }

  if (second_mark - first_mark > 500) {
    v_l = 0.00;
    v_a = 0.00;
    last_error = 0;
    error_sum = 0;

  }

  motors_control(v_l, v_a); //aplica os valores para os motores
}


void parseData(){
    char * strtokIndx;
  
    strtokIndx = strtok(tempChars, ",");
    
    while (strtokIndx != NULL){
        id = atoi(strtokIndx);
        
        if(id == robot_id){         
          strtokIndx = strtok(NULL, ",");  
          v_l = atof(strtokIndx);       
          strtokIndx = strtok(NULL, ",");         
          v_a = atof(strtokIndx);
          strtokIndx = strtok(NULL, ","); 
       }

       else{
          strtokIndx = strtok(NULL, ",");     
          strtokIndx = strtok(NULL, ",");         
          strtokIndx = strtok(NULL, ","); 
       }
   } 
}
