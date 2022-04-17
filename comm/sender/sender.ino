#include <esp_now.h>
#include <WiFi.h>

// This is the code for the board that is connected to PC

// MAC Adress de cada uma das placas que receberao comandos
//uint8_t broadcastAddress1[] = {0xA4, 0xCF, 0x12, 0x72, 0xB7, 0x20};
uint8_t broadcastAddress2[] = {0x0C,0xDC ,0x7E ,0x5E ,0xA3 ,0xE8 };
//uint8_t broadcastAddress3[] = {0x24,0x6F ,0x28 ,0xAD ,0xD4 ,0x80 };

//==============

const byte numChars = 64;
char receivedChars[numChars];
char tempChars[numChars];   
boolean newData = false;     

//==============

typedef struct{
    int id;
    float v_r;
    float v_l;
} commands;

commands robot_1;
commands robot_2;
commands robot_3;

//==============

esp_now_peer_info_t peerInfo;

//==============

void setup() 
{
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
/*
  //register first peer
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) 
  {
    Serial.println("Failed to add peer");
    return;
  }
  */
  // register second peer
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  /*
  // register third peer
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  */
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
        //  <[id1],[v_r],[v_l],[id2],[v_r],[v_l],[id3],[v_r],[v_l]>
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

    strtokIndx = strtok(tempChars,",");      // get the first part
    robot_1.id = atoi(strtokIndx);           // convert this part to an integer
    strtokIndx = strtok(NULL, ",");          // this continues where the previous call left off
    robot_1.v_r = atof(strtokIndx);          // convert this part to an float
    strtokIndx = strtok(NULL, ",");          // this continues where the previous call left off
    robot_1.v_l = atof(strtokIndx);          // convert this part to an float

    strtokIndx = strtok(NULL,",");      
    robot_2.id = atoi(strtokIndx); 
    strtokIndx = strtok(NULL, ","); 
    robot_2.v_r = atof(strtokIndx);     
    strtokIndx = strtok(NULL, ",");
    robot_2.v_l = atof(strtokIndx);     
    
    strtokIndx = strtok(NULL,",");     
    robot_3.id = atoi(strtokIndx); 
    strtokIndx = strtok(NULL, ","); 
    robot_3.v_r = atof(strtokIndx);   
    strtokIndx = strtok(NULL, ",");
    robot_3.v_l = atof(strtokIndx);     
}

//===============

void sendData()
{

  

    //esp_err_t result_1 = esp_now_send(broadcastAddress1, (uint8_t *) &robot_1, sizeof(robot_1));
    esp_err_t result_2 = esp_now_send(broadcastAddress2, (uint8_t *) &robot_2, sizeof(robot_2));
    //esp_err_t result_3 = esp_now_send(broadcastAddress3, (uint8_t *) &robot_3, sizeof(robot_3));
    /*
    if (result_1 != ESP_OK) 
    {
      Serial.println("Error sending the data for robot 1");
    }
    */
    if (result_2 != ESP_OK) 
    {
      Serial.println("Error sending the data for robot 2");
    }
    /*
    if (result_3 != ESP_OK) 
    {
      Serial.println("Error sending the data for robot 3");
    }
    */
}
