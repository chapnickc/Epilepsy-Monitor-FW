
#include "mbed.h"
#include "MAX30100.h"
#include "TMP006.h"
#include "HR_functions.h"

#include <vector>
//#include <string>                           // to convert temperature to float for printf

#define I2C_SDA     p4
#define I2C_SCL     p5

uint8_t max30100_addr = 0xAE;

uint8_t tmp006_addr = 0x80;              // 7-bit address of TMP006 can be 0x40-0x47,  converted to 8-bit address here
uint16_t tempSamples = TMP006_CFG_2SAMPLE;





Serial pc(USBTX, USBRX);
DigitalOut led1(LED1);


MAX30100 hrSensor(I2C_SDA, I2C_SCL, max30100_addr);
TMP006 tempSensor(I2C_SDA, I2C_SCL, tmp006_addr); 


float temperature;
float T = 0.050;

// HR Calculation variables
//float T = 0.050;
/*
float m = 2.00;
float avg_d;

uint8_t bpm;
uint8_t dt = 1;
uint8_t samples = 100;

std::vector<uint8_t> ixs;
std::vector<float> IR_data(samples);
*/


/*          main            */
int main(void) {

    pc.baud(115200);
    //hrSensor.init(pw1600, sr50, high, i44, i44);
    hrSensor.begin(pw1600, i44, sr100);               // pw1600 allows for 16-bit resolution
    tempSensor.config(tmp006_addr, tempSamples);

    while (true) {
        led1 =1;

        hrSensor.readSensor();
        temperature = tempSensor.readObjTempC(tmp006_addr);

        // unsigned int is guaranteed to be at least 16 bits
        pc.printf("IR: %u | Temp (deg C): %f\r\n", (unsigned int) hrSensor.IR, temperature);  
        
        //pc.printf("Temp: %f\r\n",  temperature);

        wait(T);

        /*
        uint8_t i;
        for(i=0; i < samples; i++){
            hrSensor.readSensor();
            IR_data[i] = (float) hrSensor.IR;
            wait(T);                            // will take 5.00 seconds
        }
        
        ixs = find_peaks(IR_data, m, dt);   // get the peak indicies
        avg_d = avg_peak_interval(ixs, T);  // calculate avergae peak to peak distance
        bpm = calc_bpm(avg_d);              // estimate heart rate

        pc.printf("BPM: %d\r\n", (unsigned int) bpm);
        */

        led1= !led1;
    }
}

