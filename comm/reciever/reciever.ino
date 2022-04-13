#include <esp_now.h>
#include <WiFi.h>

// This is de code for the board that is in robots

typedef struct recieved_message {
  int id;
  float v_r;
  float v_l;
}recieved_message;

recieved_message commands;

recieved_message robot;

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&commands, incomingData, sizeof(commands));
  // Update the structures with the new incoming data
  robot.id = commands.id;
  robot.v_r = commands.v_r;
  robot.v_l = commands.v_l;
  Serial.println();
}

 
void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);


}
 
void loop() {
  float v_r = robot.v_r;
  float v_l = robot.v_l;
  
  Serial.print("Id: ");
  Serial.println(robot.id);
  Serial.print("V_R: ");
  Serial.println(v_r);
  Serial.print("V_L: ");
  Serial.println(v_l);
  Serial.println();
}
