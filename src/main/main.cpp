
#include "mbed.h"
#include "MAX30100.h"
#include "HR_functions.h"

#include <vector>

#define I2C_SDA     p4
#define I2C_SCL     p5

int max30100_addr = 0xAE;

Serial pc(USBTX, USBRX);

MAX30100 sensor(I2C_SDA, I2C_SCL, max30100_addr);
DigitalOut led1(LED1);


// HR Calculation variables
float T = 0.050;
float m = 2.00;
uint8_t dt = 1;

uint8_t samples = 100;

std::vector<uint8_t> ixs;
std::vector<float> IR_data(samples);

float avg_d;
uint8_t bpm;

/*          main            */
int main(void)
{
    pc.baud(115200);
    //sensor.init(pw1600, sr50, high, i44, i44);
    sensor.begin(pw1600, i44, sr100);               // pw1600 allows for 16-bit resolution
    while (true) {
        /*
        sensor.readSensor();
        // unsigned int is guaranteed to be at least 16 bits
        pc.printf("%u\r\n", (unsigned int) sensor.IR);  
        wait(T);
        */

        led1 =1;
        
        uint8_t i;
        for(i=0; i < samples; i++){
            sensor.readSensor();
            IR_data[i] = (float) sensor.IR;
            wait(T);                 // will take 5.00 seconds
        }
        
        ixs = find_peaks(IR_data, m, dt);   // get the peak indicies
        avg_d = avg_peak_interval(ixs, T);  // calculate avergae peak to peak distance
        bpm = calc_bpm(avg_d);              // estimate heart rate

        pc.printf("BPM: %d\r\n", (unsigned int) bpm);

        led1= !led1;
    }
}

