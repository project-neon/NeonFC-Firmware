#include <esp_now.h>
#include <WiFi.h>

// This is the code for the board that is connected to PC 

// STRUCTURE TO SEND DATA
//-----------------------------------------------------------------------------------------------------------
// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress1[] = {0xA4, 0xCF, 0x12, 0x72, 0xB7, 0x20};
//uint8_t broadcastAddress2[] = {0xFF, , , , , };
//uint8_t broadcastAddress3[] = {0xFF, , , , , };

// Structure to send data must match the receiver structure
typedef struct send_message {
    int id; // must be unique for each sender board
    int v_r;
    int v_l;
} send_message;

// Create a struct_message called myData
send_message myDataSend1;
send_message myDataSend2;
send_message myDataSend3;

// Create peer interface
esp_now_peer_info_t peerInfo;
//----------------------------------------------------------------------------------------------------

// STRUCTURE TO RECIEVE DATA
//-----------------------------------------------------------------------------------------------------------
// Must match the sender structure
typedef struct recieved_message {
  int id;
  int battery;
}recieved_message;

// Create a struct_message called myData
recieved_message myDataRecieved;

// Create a structure to hold the readings from each board
recieved_message board1;
recieved_message board2;
recieved_message board3;

// Create an array with all the structures
recieved_message boardsStruct[3] = {board1, board2, board3};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&myDataRecieved, incomingData, sizeof(myDataRecieved));
  // Update the structures with the new incoming data
  boardsStruct[myDataRecieved.id-1].battery = myDataRecieved.battery;
}
//-----------------------------------------------------------------------------------------------------------


void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

// SETUP TO RECIEVE
//--------------------------------------------------------------------------------
  // Once ESPNow is successfully Init, we will register for recv CB to get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
//--------------------------------------------------------------------------------

  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  /*
  // register second peer  
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  /// register third peer
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }*/
}

void loop() { 
// SEND LOOP
//---------------------------------------------------------------------------------
// Set values to send

  // Value coming form python script
  myDataSend1.id = 1;
  myDataSend1.v_r = Serial.parseInt();
  myDataSend1.v_l = Serial.parseInt();
  /*
  myDataSend2.id = 2;
  myDataSend2.v_r = Serial.read();
  myDataSend2.v_l = Serial.read();

  myDataSend3.id = 3;
  myDataSend3.v_r = Serial.read();
  myDataSend3.v_l = Serial.read();*/

  // Send message via ESP-NOW
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &myDataSend1, sizeof(myDataSend1));
  //esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &myDataSend2, sizeof(myDataSend2));
  //esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *) &myDataSend3, sizeof(myDataSend3));
  
  if (result1 != ESP_OK) {
    Serial.println("Error sending the data");
  }

  /*if (result2 != ESP_OK) {
    Serial.println("Error sending the data 2");
  }

  if (result3 != ESP_OK) {
    Serial.println("Error sending the data 3");
  }
  */
//------------------------------------------------------------------------------------
  // Acess the variables for each board
  //int board1X = boardsStruct[0].battery;
  int board2X = boardsStruct[1].battery;
  //int board3X = boardsStruct[2].battery;
  Serial.print("Bateria: ");
  Serial.print(board2X);
  
  //delay(500);  
}
