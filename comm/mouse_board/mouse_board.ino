#include "MouseSensorADNS9500.hpp"

#define MSK_START (0b10101010)

#define RXD2 16
#define TXD2 17

#define CPI 1800

unsigned long lastNow;
unsigned long now;
int16_t dt = 0;

unsigned long lastSend=0;

HardwareSerial SerialPort(2);

uint8_t motion[6];

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  mouseInit(CPI);
}

void loop() {
  int16_t dx = 0;
  int16_t dy = 0;

  mouseReadXY(&dx, &dy);

  now = micros();

  if ((int16_t)(now - lastNow) < 800) {
    dt = (int16_t) (now - lastNow);
  }

  lastNow = now;

  motion[0] = MSK_START;
  motion[1] = (dy >> 0);
  motion[2] = (dy >> 8);
  motion[3] = (dt >> 0);
  motion[4] = (dt >> 8);
  motion[5] = (motion[0] ^ motion[1] ^ motion[2] ^ motion[3] ^ motion[4]);

  sendData();
}

void sendData(){
  if(millis() - lastSend >= 5){
    lastSend = millis();
    Serial2.write(motion, 6);
  }
}
