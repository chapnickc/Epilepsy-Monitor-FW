#include <vector>
#include "mbed.h"
#include "TMP006.h"
#include "MAX30100.h"
#include "MMA8452.h"
#include "HR_functions.h"

#define I2C_SDA     p4
#define I2C_SCL     p5

float T = 0.05;             // Sampling period
const int16_t* accel;     // array for acceleration data
float MAX_temp, TMP_temp;  


Serial pc(USBTX, USBRX);    // for debug
DigitalOut led1(LED1);



//DigitalOut led1(LED1);

/* Instantiate Sensor Objects */
MAX30100 MAX30100(I2C_SDA, I2C_SCL);
TMP006 TMP006(I2C_SDA, I2C_SCL); 
MMA8452 MMA8452(I2C_SDA, I2C_SCL); 

int main(void){
   pc.baud(115200);
   pc.printf("starting");

   MAX30100.begin(HR_mode, pw1600, i44, sr100);               // pw1600 allows for 16-bit resolution
   MAX30100.startTemperatureSampling();
   MMA8452.begin();
   TMP006.config(TMP006_CFG_2SAMPLE);

   while (1) {
      led1 = !led1; 
      MAX30100.readFIFO();
      MMA8452.readSensor();
      accel = MMA8452.getAcceleration(); 
      MAX_temp = MAX30100.readTemperature();
      TMP_temp  = TMP006.readObjTempC();
      pc.printf("IR: %u | MAX_temp: %f | TMP_temp: %f | XYZ: %d,%d,%d\n", MAX30100.rawIR, MAX_temp,TMP_temp,accel[0],accel[1],accel[2]);

      wait(T);
   } 
}
