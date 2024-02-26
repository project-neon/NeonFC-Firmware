#include <HardwareSerial.h>
#include "MouseSensorADNS9500.hpp"

#define MSK_START (0b10101010)

#define RXD2 16
#define TXD2 17

int CPI = 1800;

HardwareSerial SerialPort(2);

uint8_t motion[6];

void setup() {
  Serial.begin(115200);
  SerialPort.begin(115200, SERIAL_8N1, RXD2, TXD2);

  mouseInit(CPI);
}

void loop() {
  int16_t dx = 0;
  int16_t dy = 0;

  mouseReadXY(&dx, &dy);

  motion[0] = MSK_START;
  motion[1] = (dx >> 0);
  motion[2] = (dx >> 8);
  motion[3] = (dy >> 0);
  motion[4] = (dy >> 8);
  motion[5] = motion[0] ^ motion[1] ^ motion[2] ^ motion[3] ^ motion[4] ;

  SerialPort.write(motion, 6*sizeof(uint8_t));
}
