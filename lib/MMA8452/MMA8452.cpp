
#include "MMA8452.h"

MMA8452::MMA8452(PinName sda, PinName scl): i2c(sda, scl){
   for (int i=0; i < accel.size(); i++){
      accel[i] = 0;
   }
}

MMA8452::~MMA8452() {}

std::vector<int16_t> MMA8452::getAcceleration() const{
   return accel;
}

void MMA8452::begin(){
   /*  Write to configuration registers of the device */

   // set to standby mode to change registers 
   write_byte(MMA8452_CTRL_REG_1, 0x00);

   // set range to +/- 2g
   write_byte(MMA8452_XYZ_DATA_CFG, 0x00);

   // set to active mode to change receive data
   write_byte(MMA8452_CTRL_REG_1, 0x01);
}

void MMA8452::readAcceleration(void){
   /* Burst read the MSB and LSB registers 
    * containing acceleration data
    */
   char buffer[7];
   buffer[0] = MMA8452_STATUS;
   i2c.write(MMA8452_ADDR, buffer, 1);
   i2c.read(MMA8452_ADDR, buffer, 7);
   process_data(buffer);
}

void MMA8452::process_data(char* buffer){ 
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

void MMA8452::write_byte(char reg, char data){
/*  Write to the register of a device. Note the char 
 *  type must be used due to the MBED i2c functions 
 */
   char transfer[2] = {reg, data};
   i2c.write(MMA8452_ADDR, transfer, 2);
}
