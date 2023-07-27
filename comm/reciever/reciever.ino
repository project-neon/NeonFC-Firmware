#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

//pin definitions
#define PWMA 32
#define PWMB 13
#define   A1 25
#define   A2 33
#define   B1 26
#define   B2 27

// This is de code for the board that is in robots
int robot_id = 1;
int id;
int first_mark = 0, second_mark;

float v_l, v_a;
float last_error = 0;
float error_sum = 0;

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
  Serial.println(commands);
}

void motor_R(int speedR) { // se o valor for positivo gira para um lado e se for negativo troca o sentido
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
  if (speedL > 0) {
    digitalWrite(B1, 1);
    digitalWrite(B2, 0);
  } else {
    digitalWrite(B1, 0);
    digitalWrite(B2, 1);
  }
  ledcWrite(2, abs(speedL));
}

void motors_control(float linear, float angular) {
  angular += pid(angular, - get_theta_speed());

  if (linear > 0 ) linear = map(linear, 0, 255, 60, 255);
  if (linear < 0 ) linear = map(linear, 0, -255, -60, -255);

  //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
  float Vel_R = linear - angular;
  float Vel_L = linear + angular;

  if (Vel_R < 15 && Vel_R > -15) Vel_R = 0;
  if (Vel_R > 255 ) Vel_R = 255;
  if (Vel_R < -255) Vel_R = -255;

  if (Vel_L < 15 && Vel_L > -15) Vel_L = 0;
  if (Vel_L > 255 ) Vel_L = 255;
  if (Vel_L < -255) Vel_L = -255;

  // manda para a funcao motor um valor de -255 a 255, o sinal significa a direcao
  motor_R(Vel_R);
  motor_L(Vel_L);
}

void setup() {
  Serial.begin(115200);

  // configuraao de pinos
  ledcAttachPin(PWMA, 1);
  ledcAttachPin(PWMB, 2);

  ledcSetup(1, 80000, 8);
  ledcSetup(2, 80000, 8);

  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

  // configuracoes comunicacao
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

  // configuracao mpu
  mpu_init();
}


void loop() {
  second_mark = millis();

  strcpy(tempChars, commands); // necessario para proteger a informacao original
  parseData();

  if (second_mark - first_mark > 500) {
    v_l = 0.00;
    v_a = 0.00;
    last_error = 0;
    error_sum = 0;
  }

  motors_control(v_l, v_a); // aplica os valores para os motores
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
