#include <HardwareSerial.h>
#include "MouseSensorADNS9500.hpp"

#define RXD2 16
#define TXD2 17

int CPI = 1800;

HardwareSerial SerialPort(2);

typedef struct mouse_packet {
  int16_t dx;
  int16_t dy;
} mouse_packet;

mouse_packet packet;

void setup() {
  Serial.begin(115200);
  SerialPort.begin(115200, SERIAL_8N1, RXD2, TXD2);

  mouseInit(CPI);
}

void loop() {
  int16_t dx = 0;
  int16_t dy = 0;

  mouseReadXY(&dx, &dy);

  packet.dx = dx;
  packet.dy = dy;

  SerialPort.write((uint8_t *) &packet, sizeof(packet));
}
