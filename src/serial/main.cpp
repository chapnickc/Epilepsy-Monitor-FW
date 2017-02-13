#include <vector>
#include "mbed.h"
#include "TMP006.h"
#include "MAX30100.h"
#include "MMA8452.h"
#include "HR_functions.h"

#define I2C_SDA     p4
#define I2C_SCL     p5


float T = 0.05;             // Sampling period
float MAX_temp;
double TMP_temp;  
std::vector<int16_t> accel(3);     // array for acceleration data

Serial pc(USBTX, USBRX);    // for debug
DigitalOut led1(LED1);

/* Instantiate Sensor Objects */
//MAX30100 MAX30100(I2C_SDA, I2C_SCL);
TMP006 TMP006(I2C_SDA, I2C_SCL); 
MMA8452 MMA8452(I2C_SDA, I2C_SCL); 

int main(void){
   pc.baud(115200);
   pc.printf("starting");

   //MAX30100.begin(SPO2_mode, pw1600, i11, i24, sr100);               // pw1600 allows for 16-bit resolution
   //MAX30100.startTemperatureSampling();
   MMA8452.begin();
   TMP006.config(TMP006_CFG_2SAMPLE);

   uint16_t IR, RED;
   while (1) {
      led1 = !led1; 
      //MAX30100.readFIFO();
      MMA8452.readAcceleration();
      TMP006.readObjTempC();     // need to add temperature attribute to TMP006

      //IR = MAX30100.getIR();
      //RED = MAX30100.getRED();
      //MAX_temp = MAX30100.getTemperatureC();
      accel = MMA8452.getAcceleration(); 
      TMP_temp  = TMP006.getObjTempC();

      //pc.printf("IR: %u | RED: %u | MAX_temp: %f | TMP_temp: %f | XYZ: (%d, %d, %d)\n", IR,RED, MAX_temp,TMP_temp,accel[0],accel[1],accel[2]);
      //pc.printf("(%u,%u)\n",IR,RED);
      pc.printf("XYZ: (%d,%d,%d) | Temperature: %f\n", accel[0], accel[1], accel[2], TMP_temp);
      wait(T);
   } 
}
