
#include "MMA8452.h"

MMA8452::MMA8452(PinName sda, PinName scl): i2c(sda, scl){
      for (int i=0; i < 3; i++){
         accel[i] = 0;
      }
}
   MMA8452::~MMA8452() {}


const int16_t* MMA8452::getAcceleration() const{
   return accel;
}

void MMA8452::begin(){
   /*  Write to configuration registers of the device */
   char data[2];

   // set to standby mode to change registers 
   data[0] = MMA8452_CTRL_REG_1;
   data[1] = 0x00;                 
   i2c.write(MMA8452_ADDR, data, 2);

   // set range to +/- 2g
   data[0] = MMA8452_XYZ_DATA_CFG;
   data[1] = 0x00;
   i2c.write(MMA8452_ADDR, data, 2);

   // set to active mode to change receive data
   data[0] = MMA8452_CTRL_REG_1;
   data[1] = 0x01;                 
   i2c.write(MMA8452_ADDR, data, 2);
}

void MMA8452::readSensor(void){
   /* Burst read the MSB and LSB registers 
    * containing acceleration data
    */
   char buffer[7];
   buffer[0] = MMA8452_STATUS;
   i2c.write(MMA8452_ADDR, buffer, 1);
   i2c.read(MMA8452_ADDR, buffer, 7);
   processData(buffer);
}

void MMA8452::processData(char* buffer){ 
   /* Convert the signed acceleration values to base 10*/
   accel[0] = ((buffer[1] * 256) + buffer[2]) / 16;
   if(accel[0] > 2047){
      accel[0] -= 4096;
   }

   accel[1] = ((buffer[3] * 256) + buffer[4]) / 16;
   if(accel[1] > 2047){
      accel[1] -= 4096;
   }

   accel[2] = ((buffer[5] * 256) + buffer[6]) / 16;
   if(accel[2] > 2047){
      accel[2] -= 4096;
   }
}
