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
int robot_id = 3;
int id;
int first_mark = 0, second_mark;

float v_l, v_a;

const byte numChars = 64;
char commands[numChars];
char tempChars[numChars];


typedef struct struct_message{
  char message[64];
  } struct_message;

struct_message rcv_commands;


void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&rcv_commands, incomingData, sizeof(rcv_commands));
  // Update the structures with the new incoming data
  first_mark = millis();
  strcpy(commands, rcv_commands.message);
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
  ESP_ERROR_CHECK( esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

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
}


void loop() {
  second_mark = millis();

  strcpy(tempChars, commands); // necessário para proteger a informação original
  parseData();

  if (second_mark - first_mark > 500) {
    v_l = 0.00;
    v_a = 0.00;
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
