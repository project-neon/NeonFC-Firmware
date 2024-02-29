#include "MouseSensorADNS9500.hpp"

#define MSK_START (0b10101010)

#define RXD2 16
#define TXD2 17

#define CPI 1800

unsigned long lastSend = 0;
unsigned long now;
int16_t dt = 0;
float dist = 0;


uint8_t motion[4];

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  mouseInit(CPI);
}

void loop() {
  now = millis();
  if(now - lastSend >= 5){
    int16_t dx = 0;
    int16_t dy = 0;

    
    mouseReadXY(&dx, &dy);
    dist += CountsToM(dy);

    unsigned long dt = now - lastSend;

    lastSend = now;

    Serial.print("d: ");Serial.println(dist);
    
    float v_f = (float)(1000000*CountsToM(dy)/dt);
    Serial.print("v: ");Serial.println(v_f);
    int16_t v = (int16_t) v_f;
    
    motion[0] = MSK_START;
    motion[1] = (v >> 0);
    motion[2] = (v >> 8);
    motion[3] = (motion[0] ^ motion[1] ^ motion[2]);

    
    Serial2.write(motion, 4);
  }
}

float CountsToM(int16_t counts) {
  return counts/(39.37*CPI);
}
