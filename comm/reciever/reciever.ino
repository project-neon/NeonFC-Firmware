#include <esp_now.h>
#include <WiFi.h>

// This is de code for the board that is in robots

// STRUCTURE TO SEND DATA
//-----------------------------------------------------------------------------------------------------------
// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x0C, 0xDC, 0x7E, 0x5E, 0x97, 0x0C};

// Structure to send data must match the receiver structure
typedef struct send_message {
    int id; // must be unique for each sender board
    int battery;
  }send_message;

// Create a struct_message called myData
send_message myDataSend;

// Create peer interface
esp_now_peer_info_t peerInfo;
//-----------------------------------------------------------------------------------------------------------


// STRUCTURE TO RECIEVE DATA
//-----------------------------------------------------------------------------------------------------------
// Must match the sender structure
typedef struct recieved_message {
  int id;
  int v_r;
  int v_l;
}recieved_message;

// Create a struct_message called myData
recieved_message myDataRecieved;

// Create a structure to hold the readings from each board
recieved_message board1;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&myDataRecieved, incomingData, sizeof(myDataRecieved));
  // Update the structures with the new incoming data
  board1.v_r = myDataRecieved.v_r;
  board1.v_l = myDataRecieved.v_l;
  Serial.println();
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
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
//--------------------------------------------------------------------------------

   // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  // Acess the variables for each board
  int board1v_r = board1.v_r;
  int board1v_l = board1.v_l;

// SEND LOOP
//---------------------------------------------------------------------------------
  // Set values to send
  myDataSend.id = 2;
  myDataSend.battery = 100;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myDataSend, sizeof(myDataSend));

   if (result != ESP_OK) {
    Serial.println("Error sending the data");
  }
//------------------------------------------------------------------------------------
  
  Serial.println(board1.v_r);
  Serial.println(board1.v_l);
  //delay(500);  
}
