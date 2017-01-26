#ifndef MAX30100_H
#define MAX30100_H

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"


// 8bit I2C address of the MAX30100 
// with R/W bit set
#define MAX30100_ADDR           0xAE  

// Registers
#define MAX30100_INT_STATUS     0x00  // Which interrupts are tripped
#define MAX30100_INT_ENABLE     0x01  // Which interrupts are active
#define MAX30100_FIFO_WR_PTR    0x02  // Where data is being written
#define MAX30100_OVRFLOW_CTR    0x03  // Number of lost samples
#define MAX30100_FIFO_RD_PTR    0x04  // Where to read from
#define MAX30100_FIFO_DATA      0x05  // Ouput data buffer
#define MAX30100_MODE_CONFIG    0x06  // Control register
#define MAX30100_SPO2_CONFIG    0x07  // Oximetry settings
#define MAX30100_LED_CONFIG     0x09  // Pulse width and power of LEDs
#define MAX30100_TEMP_INTG      0x16  // Temperature value, whole number
#define MAX30100_TEMP_FRAC      0x17  // Temperature value, fraction
#define MAX30100_REV_ID         0xFE  // Part revision
#define MAX30100_PART_ID        0xFF  // Part ID, normally 0x11


typedef enum {
   HR_mode = 0x02, // IR only
   SPO2_mode       // RED and IR
} modeControl_t;

typedef enum{ // This is the same for both LEDs
   pw200,    // 200us pulse
   pw400,    // 400us pulse
   pw800,    // 800us pulse
   pw1600    // 1600us pulse
} pulseWitdth_t;

typedef enum{
   sr50,    // 50 samples per second
   sr100,   // 100 samples per second
   sr167,   // 167 samples per second
   sr200,   // 200 samples per second
   sr400,   // 400 samples per second
   sr600,   // 600 samples per second
   sr800,   // 800 samples per second
   sr1000   // 1000 samples per second
} sampleRate_t;

typedef enum{
   i0,    // No current
   i4,    // 4.4mA
   i8,    // 7.6mA
   i11,   // 11.0mA
   i14,   // 14.2mA
   i17,   // 17.4mA
   i21,   // 20.8mA
   i24,   // 24 mA
   i27,   // 27.1mA
   i31,   // 30.6mA
   i34,   // 33.8mA
   i37,   // 37.0mA
   i40,   // 40.2mA
   i44,   // 43.6mA
   i47,   // 46.8mA
   i50    // 50.0mA
} ledCurrent_t;

class MAX30100 {
   private:
      I2C i2c;
      float temperature;
      uint16_t IR, RED; 

   public:
      MAX30100(PinName sda, PinName scl);
      ~MAX30100();
      float getTemperatureC() const;
      float getTemperatureF() const;
      uint16_t getIR() const;
      uint16_t getRED() const;
      void begin(modeControl_t mc = HR_mode,
            pulseWitdth_t pw = pw1600,   // Longest pulseWidth
            ledCurrent_t red = i24,       
            ledCurrent_t ir = i24,
            sampleRate_t sr = sr100);   // 2nd lowest sampleRate
      void startTemperatureSampling();
      void readFIFO(void);
      void readTemperature();
      void write_byte(char reg, char data);
};

#endif
