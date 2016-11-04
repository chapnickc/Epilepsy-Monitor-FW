#include <vector>
#include "mbed.h"
#include "TMP006.h"
#include "MAX30100.h"
#include "HR_functions.h"

#define I2C_SDA     p4
#define I2C_SCL     p5


Serial pc(USBTX, USBRX);    // for debug

float Fs = 125.0;           // Hz 
float T = 1/Fs;             // Sampling period

/* TMP006 Variables */
uint8_t tmp006_addr = 0x80;                 // 7-bit address of TMP006 can be 0x40-0x47,  converted to 8-bit address here
uint16_t tempSamples = TMP006_CFG_2SAMPLE;

/* MAX30100 Variables */
uint8_t max30100_addr = 0xAE;

//DigitalOut led1(LED1);

/* Instantiate Sensor Objects */
MAX30100 hrSensor(I2C_SDA, I2C_SCL, max30100_addr);
TMP006 tempSensor(I2C_SDA, I2C_SCL, tmp006_addr); 

int main(void){
    pc.baud(115200);
    //led1 = 1;

    hrSensor.begin(pw1600, i44, sr100);               // pw1600 allows for 16-bit resolution
    tempSensor.config(tmp006_addr, tempSamples);

    while (true) {
        //led1 = !led1; 
        hrSensor.readSensor();
        pc.printf("%f\n", (float) hrSensor.IR);
        wait(T);
    } 
}
