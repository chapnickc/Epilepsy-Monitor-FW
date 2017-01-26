
#include "MAX30100.h"

MAX30100::MAX30100(PinName sda, PinName scl): 
   i2c(sda, scl){}

   MAX30100::~MAX30100() {}

float MAX30100::getTemperatureC() const {
   return temperature;
}

float MAX30100::getTemperatureF() const {
   return temperature*(9/5) + 32;
}

uint16_t MAX30100::getIR() const {
   return IR;
}

uint16_t MAX30100::getRED() const {
   return RED;
}


void MAX30100::begin(modeControl_t mc, pulseWitdth_t pw, ledCurrent_t red, ledCurrent_t ir, sampleRate_t sr){
   /*  Write to the configuration registers of the device to
    *  set the mode sample rate and pulse width, 
    *  and enable high resolution 
    */

   uint8_t data;

   // 0x02 = HR only, 0x03 = SPO2 Enabled
   data =  mc;
   write_byte(MAX30100_MODE_CONFIG, data);

   data = (red << 4) | ir;       // set RED and IR led's
   write_byte(MAX30100_LED_CONFIG, data);
  
   data = (0x01 << 6) | (sr << 2) | pw; 
   write_byte(MAX30100_SPO2_CONFIG, data);
}


void MAX30100::readFIFO(void){
   /* Burst read four bytes from the FIFO 
    * to obtain 16-bit ADC values for IR 
    * and RED reflectance.
    */
   char data[4];
   data[0] = MAX30100_FIFO_DATA;
   i2c.write(MAX30100_ADDR, data, 1);
   i2c.read(MAX30100_ADDR, data, 4);  // Read four times from the FIFO

   // Combine bytes to get the actual number
   IR = (data[0]<<8) | data[1];    
   RED = (data[2]<<8) | data[3];
}

void MAX30100::startTemperatureSampling(){
   /* Mask the TEMP-EN bit in the mode 
    * configuration register to 1
    */
   char mode_config;
   char data[2];
   data[0] = MAX30100_MODE_CONFIG;
   i2c.write(MAX30100_ADDR, data, 1);
   i2c.read(MAX30100_ADDR, &mode_config, 1);

   data[1] = mode_config | (0x01 << 3);
   i2c.write(MAX30100_ADDR, data, 2);
}

void MAX30100::readTemperature(){

   char tempInt, tempFrac, reg; 

   // read temperature integer
   reg = MAX30100_TEMP_INTG;
   i2c.write(MAX30100_ADDR, &reg, 1);
   i2c.read(MAX30100_ADDR, &tempInt, 1);


   // read temperature fraction
   reg = MAX30100_TEMP_FRAC;
   i2c.write(MAX30100_ADDR, &reg, 1);
   i2c.read(MAX30100_ADDR, &tempFrac, 1);

   temperature  = (float) tempFrac * 0.0625 + (int8_t) tempInt;
}

void MAX30100::write_byte(char reg, char data){
   /*  Write to the register of a device. Note the char 
    *  type must be used due to the MBED i2c functions 
    */
   char transfer[2] = {reg, data};
   i2c.write(MAX30100_ADDR, transfer, 2);
}
