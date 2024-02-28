#include <SoftwareSerial.h>

#define MSK_START (0b10101010)

#define RX2 18
#define TX2 19
#define CPI 1800

TaskHandle_t SerialComm;
QueueHandle_t MotionQueue;

SoftwareSerial SerialPort(RX2, TX2);

void SerialCommCode(void* pvParameters) {
  uint8_t received[6];
  int16_t motion[2];

  while(1) {
    while(SerialPort.available()) {
      SerialPort.readBytes(received, sizeof(received));

      if(received[0] == MSK_START){

        uint8_t checksum = received[0] ^ received[1] ^ received[2] ^ received[3] ^ received[4];

        if(checksum == received[5]) {
          motion[0] = received[1] | (received[2] << 8); 
          motion[1] = received[3] | (received[4] << 8);

          xQueueSend(MotionQueue, &(motion), ( TickType_t ) 0); 
        }

      }

    }
  }
}

float CountsToMM(int16_t counts) {
  return counts/(25.4*CPI);
}

float CountsToCM(int16_t counts) {
  return counts/(2.54*CPI);
}

float CountsToM(int16_t counts) {
  return counts/(.254*CPI);
}

void mouse_init() {
  SerialPort.begin(115200);
  Serial.begin(115200);

  MotionQueue = xQueueCreate(1, 2*sizeof(int16_t));

  xTaskCreatePinnedToCore(
      SerialCommCode,
      "SerialComm",
      10000,
      NULL,
      0,
      &SerialComm,
      0
    );
}

float get_linear_speed() {
  int16_t motion[2];

  if(xQueueReceive(MotionQueue, &(motion), ( TickType_t ) 80) == pdTRUE) {
    float dt = (float)(motion[1]) / 1000000.0f;

    Serial.print("dy: "); Serial.println(motion[0]);
    Serial.print("dt: "); Serial.println(dt, 6);

    Serial.print("vl: "); 
    return (CountsToM(motion[0]) / dt);
  }
}