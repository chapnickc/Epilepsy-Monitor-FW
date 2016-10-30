#include "MAX30100.h"

MAX30100::MAX30100(PinName sda, PinName scl, int addr) : 
    i2c(sda, scl), addr(addr) {
}

MAX30100::~MAX30100() {}

void MAX30100::begin(pulseWidth pw, ledCurrent ir, sampleRate sr){
    char high_res = 0x01;
    char data[2];

    data[0] = MAX30100_MODE_CONFIG;
    data[1] = 0x02;                 // 0x02 = HR only, 0x03 = SPO2 Enabled
    i2c.write(addr, data, 2);
    
    data[0] = MAX30100_LED_CONFIG;
    data[1] = (ir << 4) | ir;       // set RED and IR led's
    i2c.write(addr, data, 2);

    data[0] = MAX30100_SPO2_CONFIG;
    data[1] = (high_res << 6) | (sr << 2) | pw ;
    i2c.write(addr, data, 2);
};


void  MAX30100::readSensor(void){
    char reg[1];
    char data[4] = {0};
    
    reg[0] = MAX30100_FIFO_DATA;
    i2c.write(addr, reg, 1);
    i2c.read(addr, data, 4);  // Read four times from the FIFO

    IR = (data[0]<<8) | data[1];    // Combine values to get the actual number
    RED = (data[2]<<8) | data[3];
};  
