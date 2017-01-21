#include <vector>
#include "mbed.h"
#include "TMP006.h"
#include "MAX30100.h"
#include "HR_functions.h"

#define I2C_SDA     p4
#define I2C_SCL     p5


Serial pc(USBTX, USBRX);    // for debug


float T = 0.05;             // Sampling period

uint16_t tempSamples = TMP006_CFG_2SAMPLE;
float MAX_temp, TMP_temp;



//DigitalOut led1(LED1);

/* Instantiate Sensor Objects */
MAX30100 hrSensor(I2C_SDA, I2C_SCL);
TMP006 tmpSensor(I2C_SDA, I2C_SCL); 

int main(void){

    pc.baud(115200);
    pc.printf("starting");

    hrSensor.begin(HR_mode, pw1600, i44, sr100);               // pw1600 allows for 16-bit resolution
    tmpSensor.config(TMP006_CFG_2SAMPLE);

    while (1) {
        //led1 = !led1; 
        hrSensor.readFIFO();
        MAX_temp = hrSensor.readTemperature();
        TMP_temp  = tmpSensor.readObjTempC();
        pc.printf("IR:\t%u\t| MAX_temp:\t%f\t| TMP_temp:\t%f\t", hrSensor.rawIR, MAX_temp,TMP_temp);
        pc.printf("hooray");
        wait(T);
    } 
}
