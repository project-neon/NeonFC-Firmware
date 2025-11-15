#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define LED 2

// MAC Adress genérico para enviar os dados no canal selecionado
uint8_t broadcast_adr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

//==================================================================================
const byte numChars = 64;
char receivedChars[numChars];
char tempChars[numChars];
boolean newData = false;
int id, count;

typedef struct struct_message {
  char message[numChars];
} struct_message;

struct_message snd_commands;

// ==================================================================================
typedef struct rcv_struct_message {
  float value;
} rcv_struct_message;

rcv_struct_message rcv_commands;

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&rcv_commands, incomingData, sizeof(rcv_commands));
  // Serial.println(rcv_commands.value);
  delay(2);
  // grade = rcv_commands.value;
}
// ==================================================================================


esp_now_peer_info_t peerInfo;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));
  esp_wifi_set_max_tx_power(84);


  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  memcpy(peerInfo.peer_addr, broadcast_adr, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  //Serial.print("MAC Address: ");
  //Serial.println(WiFi.macAddress());
}

//=============

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    digitalWrite(LED, LOW);
    delay(750 / 2);
    digitalWrite(LED, HIGH);
    delay(750 / 2);
    //nao chega aqui
    strcpy(snd_commands.message, receivedChars);
    sendData();
    newData = false;
  } else {
    //digitalWrite(LED, LOW);
    //delay(750);
    //digitalWrite(LED, HIGH);
    //delay(750);
  }
}

//==============

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char in;

  while (Serial.available()) {
    //  Formato da mensagem::
    //  <[id1],[v_l1],[v_a1],[id2],[v_l2],[v_a2],[id3],[v_l3],[v_a3]>
    //Serial.print("oi");
    // digitalWrite(LED, HIGH);
    // delay(100);
    // digitalWrite(LED, LOW);
    // delay(100);
    // chega aqui

    //digitalWrite(LED, HIGH);
    //delay(10);
    //digitalWrite(LED, LOW);
    //delay(10);

    in = Serial.read();
    if (recvInProgress == true) {
      if (in != endMarker) {
        receivedChars[ndx] = in;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0';  // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (in == startMarker) {
      recvInProgress = true;
      //digitalWrite(LED, HIGH);
      //delay(10 * 1000);
      //digitalWrite(LED, LOW);
    }
  }
}


void sendData() {
  digitalWrite(LED, HIGH);
  // esse delay é necessário para que os dados sejam enviados corretamente
  esp_err_t message = esp_now_send(broadcast_adr, (uint8_t *)&snd_commands, sizeof(snd_commands));
  //delay(750000000000);  // agora ele chega???
  delay(250);
  digitalWrite(LED, LOW);

}
