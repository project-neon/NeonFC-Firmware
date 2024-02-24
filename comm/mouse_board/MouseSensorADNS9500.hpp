#include <SPI.h>
#include "constants.h"

SPIClass vspi = SPIClass(VSPI);

uint8_t mouseRead(char reg_addr);
void mouseWrite(char reg_addr, char data);
bool mouseUploadFirmware();

// Begin SPI Transaction
void beginSlaveCommunication(){
  digitalWrite(SS, LOW);
}

// End SPI Transaction
void endSlaveCommunication(){
  digitalWrite(SS, HIGH);
}

// Initialize Mouse
bool mouseInit(int cpi){

  // Initialize SPI
  vspi.begin(SCK, MISO, MOSI, SS);
  vspi.beginTransaction(SPISettings(2*MHz, MSBFIRST, SPI_MODE3));
  pinMode(MISO, INPUT_PULLDOWN); // Give MISO a pullDown Connection
  
  // Setup CSN pin as output and ensure that the serial port is reset
  pinMode(SS, OUTPUT);

  endSlaveCommunication();
  beginSlaveCommunication();
  endSlaveCommunication();

  // Force reset
  mouseWrite(REG_Power_Up_Reset, 0x5a);

  // Wait for it to reboot
  delay(50);

  // Read registers 0x02 to 0x06 (and discard the data)
  mouseRead(REG_Motion);
  mouseRead(REG_Delta_X_L);
  mouseRead(REG_Delta_X_H);
  mouseRead(REG_Delta_Y_L);
  mouseRead(REG_Delta_Y_H);

  // Upload the firmware
  if(!mouseUploadFirmware())
    return false;

  delay(10);

  // Enable laser(bit 0 = 0b), in normal mode (bits 3,2,1 = 000b)
  // reading the actual value of the register is important because the real
  // default value is different from what is said in the datasheet, and if you
  // change the reserved bytes (like by writing 0x00...) it would not work.
  byte laser_ctrl0 = mouseRead(REG_LASER_CTRL0);
  mouseWrite(REG_LASER_CTRL0, laser_ctrl0 & 0xf0 );

  // Set to 900DPI (=1800CPI)        //00010100
  mouseWrite(REG_Configuration_I, uint8_t(cpi/90));

  // Finish up setup
  delay(1);

  return true;
}

// Read register
uint8_t mouseRead(char reg_addr){
  beginSlaveCommunication();

  // Send adress of the register, with MSBit = 0 to indicate it's a read
  vspi.transfer(reg_addr & 0x7f );
  delayMicroseconds(100); // tSRAD
  // Read data
  uint8_t data = vspi.transfer(0);
  // tSCLK-NCS for read operation is 120ns
  delayMicroseconds(1);
  endSlaveCommunication();
  // tSRW/tSRR (=20us) minus tSCLK-NCS
  delayMicroseconds(19);

  return data;
}

// Write register
void mouseWrite(char reg_addr, char data){
  beginSlaveCommunication();

  // Send adress of the register, with MSBit = 1 to indicate it's a write
  vspi.transfer(reg_addr | 0x80 );
  vspi.transfer(data);
  // tSCLK-NCS for write operation
  delayMicroseconds(20);
  endSlaveCommunication();
  // tSWW/tSWR (=120us) minus tSCLK-NCS.
  // Could be shortened, but is looks like a safe lower bound
  delayMicroseconds(100);
}

// Upload firmware to Mouse
bool mouseUploadFirmware(){
  // set the configuration_IV register in 3k firmware mode
  // bit 1 = 1 for 3k mode, other bits are reserved
  mouseWrite(REG_Configuration_IV, 0x02);
  // write 0x1d in SROM_enable reg for initializing
  mouseWrite(REG_SROM_Enable, 0x1d);
  // wait for more than one frame period
  // assume that the frame rate is as low as 100fps...
  // even if it should never be that low
  delay(10);
  // write 0x18 to SROM_enable to start SROM download
  mouseWrite(REG_SROM_Enable, 0x18);
  // write the SROM file (=firmware data)
  beginSlaveCommunication();
  // write burst destination adress
  vspi.transfer(REG_SROM_Load_Burst | 0x80);
  delayMicroseconds(15);

  // send all bytes of the firmware
  unsigned char c;
  for(int i = 0; i < firmware_length; i++){
    c = (unsigned char)pgm_read_byte(firmware_data + i);
    vspi.transfer(c);
    delayMicroseconds(15);
  }
  endSlaveCommunication();

  return true;
}

// Reads Motion
void mouseReadXY(int16_t* x, int16_t* y){
  // Freeze motion
  uint8_t mot = mouseRead(REG_Motion);

  //if(!(mot & 0b10000000))
    //return;

  // Reads X
  *x = uint16_t(mouseRead(REG_Delta_X_L)) | uint16_t(mouseRead(REG_Delta_X_H) << 8);
  *y = uint16_t(mouseRead(REG_Delta_Y_L)) | uint16_t(mouseRead(REG_Delta_Y_H) << 8);

}

int mouseReadSqual() {
  return mouseRead(REG_SQUAL);
}

// Prints useful stuff
void mouseDebug(){
  static int oreg[7] = {
    REG_Product_ID,
    REG_Inverse_Product_ID,
    REG_SROM_ID,
    REG_Motion,
    REG_Configuration_I,
    REG_SQUAL
  };

  static char* oregname[] = {
    "Product_ID",
    "Inverse_Product_ID",
    "SROM_Version",
    "Motion",
    "DPI",
    "SQUAL",
  };

  byte regres;

  beginSlaveCommunication();

  int rctr=0;
  for(rctr=0; rctr < sizeof(oregname) / sizeof(oregname[0]); rctr++){
    vspi.transfer(oreg[rctr]);
    delay(1);
    Serial.println("---");
    Serial.println(oregname[rctr]);
    Serial.println(oreg[rctr],HEX);
    regres = vspi.transfer(0);
    Serial.print(regres,HEX);
    Serial.write('\t');
    Serial.print(regres,BIN);
    Serial.write('\t');
    Serial.println(regres);
    delay(1);
  }

  endSlaveCommunication();
}