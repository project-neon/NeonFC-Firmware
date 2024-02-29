#define MSK_START (0b10101010)

#define RX2 18
#define TX2 19
#define CPI 1800


int16_t motion;

void SerialCommCode() {
  uint8_t received[4];
  while(Serial2.available()) {
    Serial2.readBytes(received, sizeof(received));

    if(received[0] == MSK_START){

      uint8_t checksum = received[0] ^ received[1] ^ received[2];

      if(checksum == received[3]) {
        motion = received[1] | (received[2] << 8); 
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
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
}

float get_linear_speed() {
  float v = (float) motion;
  v /= 1000 ;
  Serial.print("vl: "); Serial.println(v);
  return v;
}
