#define MSK_START (0b10101010)

#define RX2 18
#define TX2 19
#define CPI 1800


int16_t motion[2];

void SerialCommCode() {
  uint8_t received[6];
  while(Serial2.available()) {
    Serial2.readBytes(received, sizeof(received));

    if(received[0] == MSK_START){

      uint8_t checksum = received[0] ^ received[1] ^ received[2] ^ received[3] ^ received[4];

      if(checksum == received[5]) {
        motion[0] = received[1] | (received[2] << 8); 
        motion[1] = received[3] | (received[4] << 8);
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
  float dt = (float)(motion[1]) / 1000000.0f;

  Serial.print("dy: "); Serial.println(motion[0]);
  Serial.print("dt: "); Serial.println(dt, 8);

  float v = CountsToM(motion[0]) / dt;
  Serial.print("vl: "); Serial.println(v);
  return v;
}
