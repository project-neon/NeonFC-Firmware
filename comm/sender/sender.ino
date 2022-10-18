#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define CHANNEL 3

// This is the code for the board that is connected to PC

// MAC Adress de cada uma das placas que receberao comandos
// uint8_t broadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// uint8_t broadcastAddress0[] = {0xA4, 0xCF, 0x12, 0x72, 0xB7, 0x20};
uint8_t broadcastAddress0[] = {0x0C, 0xDC, 0x7E, 0x5E, 0x97, 0x0C};
uint8_t broadcastAddress3[] = {0x0C, 0xDC, 0x7E, 0x5E, 0xA3, 0xE8};
uint8_t broadcastAddress9[] = {0x24, 0x6F, 0x28, 0xAD, 0xD4, 0x80};

//==============

const byte numChars = 64;
char receivedChars[numChars];
char tempChars[numChars];   
boolean newData = false;     
int id, count;

//==============

typedef struct{
    int id;
    float v_l;
    float v_a;
} commands;

commands robot_0;
commands robot_3;
commands robot_9;

//==============

esp_now_peer_info_t peerInfo;

void setup() 
{
  Serial.begin(115200);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK( esp_wifi_start());
  ESP_ERROR_CHECK( esp_wifi_set_channel(14, WIFI_SECOND_CHAN_NONE));
  esp_wifi_set_max_tx_power(84);

// ESP_ERROR_CHECK
  // WiFi.mode(WIFI_STA);
  // esp_wifi_set_channel(14, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  peerInfo.channel = CHANNEL;


  //register first peer
  memcpy(peerInfo.peer_addr, broadcastAddress0, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) 
  {
    Serial.println("Failed to add peer");
    return;
  }
  
  // register second peer
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
 
  // register third peer
  memcpy(peerInfo.peer_addr, broadcastAddress9, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

//=============

void loop() {
  recvWithStartEndMarkers();
  if (newData == true){
      strcpy(tempChars, receivedChars);
          // this temporary copy is necessary to protect the original data
          //   because strtok() used in parseData() replaces the commas with \0
      parseData();
      sendData();
      newData = false;
  }
}

//==============

void recvWithStartEndMarkers(){
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char in;

    while (Serial.available()){
        //  Message format:
        //  <[id1],[v_l1],[v_a1],[id2],[v_l2],[v_a2],[id3],[v_l3],[v_a3]>
        in = Serial.read();

        if (recvInProgress == true){
            if (in != endMarker){
                receivedChars[ndx] = in;
                ndx++;
                if (ndx >= numChars){
                    ndx = numChars - 1;
                }
            }
            else{
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (in == startMarker){
            recvInProgress = true;
        }
    }
}

//===============

void parseData(){      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars, ",");
    
    while (strtokIndx != NULL){
        id = atoi(strtokIndx);
        
        if(id == 0){
          robot_0.id = 0;         
          strtokIndx = strtok(NULL, ",");     
          robot_0.v_l = atof(strtokIndx);       
          strtokIndx = strtok(NULL, ",");         
          robot_0.v_a = atof(strtokIndx);
          strtokIndx = strtok(NULL, ","); 
          }
  
        else if(id == 3){
          robot_3.id = 3;         
          strtokIndx = strtok(NULL, ",");     
          robot_3.v_l = atof(strtokIndx);       
          strtokIndx = strtok(NULL, ",");         
          robot_3.v_a = atof(strtokIndx);
          strtokIndx = strtok(NULL, ","); 
          }
  
        else{
          robot_9.id = 9;         
          strtokIndx = strtok(NULL, ",");     
          robot_9.v_l = atof(strtokIndx);       
          strtokIndx = strtok(NULL, ",");         
          robot_9.v_a = atof(strtokIndx);
          strtokIndx = strtok(NULL, ","); 
          }
    }
    
}

//===============

void sendData()
{   
    // esse delay é necessário para que os dados sejam enviados corretamente
    esp_err_t result_0 = esp_now_send(broadcastAddress0, (uint8_t *) &robot_0, sizeof(robot_0));
    delay(3);
    esp_err_t result_3 = esp_now_send(broadcastAddress3, (uint8_t *) &robot_3, sizeof(robot_3));
    delay(3);
    esp_err_t result_9 = esp_now_send(broadcastAddress9, (uint8_t *) &robot_9, sizeof(robot_9));
    delay(3);

}
