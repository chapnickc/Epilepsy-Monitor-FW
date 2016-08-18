/*
 Library for the Maxim MAX30100 pulse oximetry system
 */

#include "MAX30100.h"
#include "functions.h"


static Serial pc(USBTX, USBRX);

void MAX30100::setLEDs(pulseWidth pw, ledCurrent red, ledCurrent ir){
    char data_read[1];
    char data_write[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, data_read, 1);
    data_read[0] = data_read[0] & 0xFC; // Set LED_PW to 00
    data_write[0] = data_read[0] | pw;
    i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, data_write, 1);     // Mask LED_PW
    data_write[0] = ((red<<4) | ir);
    i2c_write(MAX30100_ADDRESS, MAX30100_LED_CONFIG, data_write, 1); // write LED configs
}

void MAX30100::setSPO2(sampleRate sr, high_resolution hi_res){
    char data_read[1];
    char data_write[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, data_read, 1);
    data_read[0] = data_read[0] & 0xA3; // Set SPO2_SR to 000 and SPO2_HI_RES_EN to 0
    data_write[0] = data_read[0] | (sr<<2) | (hi_res<<6);
    i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, data_write, 1); // Mask SPO2_SR
    i2c_read(MAX30100_ADDRESS, MAX30100_CONFIG, data_read, 1);
    data_write[0] = data_read[0] & 0xF8; // Set Mode to 000
    i2c_write(MAX30100_ADDRESS, MAX30100_CONFIG, data_write, 1); // Mask MODE
}

void MAX30100::setInterruptSPO2(void){
    char data_read[1];
    char data_write[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_INT_ENABLE, data_read, 1);
    data_write[0] = data_read[0] | 0x10; // Set Interrupt enable for SPO2
    i2c_write(MAX30100_ADDRESS, MAX30100_INT_ENABLE, data_write, 1); // Mask ENB_SPO2_RDY
}

int MAX30100::getNumSamp(void){
    char data_read[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_W_POINTER, data_read, 1);
    char wrPtr = data_read[0];
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_R_POINTER, data_read, 1);
    char rdPtr = data_read[0];
    return (abs( 16 + (int)wrPtr - (int)rdPtr ) % 16);
}

void MAX30100::setTemp(void){
    char data_read[1];
    char data_write[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_CONFIG, data_read, 1);
    data_write[0] = data_read[0] | 0x0B;    // Set SPO2 Mode and enable temperature
    i2c_write(MAX30100_ADDRESS, MAX30100_CONFIG, data_write, 1); // Mask MODE
    i2c_read(MAX30100_ADDRESS, MAX30100_CONFIG, data_read, 1);
}

void MAX30100::setSPO2mode(void){
    char data_read[1];
    char data_write[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_CONFIG, data_read, 1);
    data_write[0] = data_read[0] | 0x03;    // Set SPO2 Mode
    i2c_write(MAX30100_ADDRESS, MAX30100_CONFIG, data_write, 1);
}

int MAX30100::readTemp(void){
    char data_read[1];
    char temp_int, temp_fract;
    int temp_measured;
    i2c_read(MAX30100_ADDRESS, MAX30100_TEMP_INTEGER, data_read, 1);
    temp_int = data_read[0];
    i2c_read(MAX30100_ADDRESS, MAX30100_TEMP_FRACTION, data_read, 1);
    temp_fract = data_read[0] & 0x0F;
    temp_measured = ((int)temp_int)+(((int)temp_fract) >> 4);
    return temp_measured;
}

void MAX30100::readSensor(void){
    char data_read[4];
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_DATA_REG, data_read, 4);  // Read four times from the FIFO
    IR = (data_read[0]<<8) | data_read[1];    // Combine values to get the actual number
    SPO2 = (data_read[2]<<8) | data_read[3];  // Combine values to get the actual number
}

void MAX30100::shutdown(void){
    char data_read[1];
    char data_write[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_CONFIG, data_read, 1);  // Get the current register
    data_write[0] = data_read[0] | 0x80;
    i2c_write(MAX30100_ADDRESS, MAX30100_CONFIG, data_write, 1);   // mask the SHDN bit
}

void MAX30100::reset(void){
    char data_read[1];
    char data_write[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_CONFIG, data_read, 1);  // Get the current register
    data_write[0] = data_read[0] | 0x40;
    i2c_write(MAX30100_ADDRESS, MAX30100_CONFIG, data_write, 1);   // mask the RESET bit
}

void MAX30100::startup(void){
    char data_read[1];
    char data_write[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_CONFIG, data_read, 1);  // Get the current register
    data_write[0] = data_read[0] & 0x7F;
    i2c_write(MAX30100_ADDRESS, MAX30100_CONFIG, data_write, 1);   // mask the SHDN bit
}

char MAX30100::getRevID(void){
    char data_read[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_REVISION_ID, data_read, 1);
    return data_read[0];
}

char MAX30100::getPartID(void){
    char data_read[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_PART_ID, data_read, 1);
    return data_read[0];
}

void MAX30100::begin(pulseWidth pw, ledCurrent ir, sampleRate sr){
    char data_write[1];
    data_write[0] = 0x02;                       // Heart rate only
    i2c_write(MAX30100_ADDRESS, MAX30100_CONFIG, data_write, 1);
    data_write[0] = ir;
    i2c_write(MAX30100_ADDRESS, MAX30100_LED_CONFIG, data_write, 1);
    data_write[0] = ((sr<<2)|pw);
    i2c_write(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, data_write, 1);
}

void MAX30100::init(pulseWidth pw, sampleRate sr, high_resolution hi_res, ledCurrent red, ledCurrent ir){
    
    setLEDs(pw, red, ir);
    setSPO2(sr, hi_res);
    
}

void MAX30100::printRegisters(void){
    char data_read[1];
    i2c_read(MAX30100_ADDRESS, MAX30100_INT_STATUS, data_read, 1);
    pc.printf("INT_STATUS: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_INT_ENABLE, data_read, 1);
    pc.printf("INT_ENABLE: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_W_POINTER, data_read, 1);
    pc.printf("FIFO_W_POINTER: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_OVR_COUNTER, data_read, 1);
    pc.printf("OVR_COUNTER: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_R_POINTER, data_read, 1);
    pc.printf("FIFO_R_POINTER: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_FIFO_DATA_REG, data_read, 1);
    pc.printf("FIFO_DATA_REG: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_CONFIG, data_read, 1);
    pc.printf("CONFIG: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, data_read, 1);
    pc.printf("SPO2_CONFIG: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_LED_CONFIG, data_read, 1);
    pc.printf("LED_CONFIG: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_TEMP_INTEGER, data_read, 1);
    pc.printf("TEMP_INTEGER: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_TEMP_FRACTION, data_read, 1);
    pc.printf("TEMP_FRACTION: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_REVISION_ID, data_read, 1);
    pc.printf("REVISION_ID: %#4X\r\n", data_read[0]);
    i2c_read(MAX30100_ADDRESS, MAX30100_PART_ID, data_read, 1);
    pc.printf("PART_ID: %#4X\r\n", data_read[0]);
}




