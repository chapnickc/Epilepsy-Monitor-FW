#ifndef TMP006_H
#define TMP006_H
 
#include "mbed.h"
#include <stdint.h>

// 7-bit address of TMP006 can be 0x40-0x47
// converted to 8-bit address here for mbed API
#define TMP006_ADDR 0x80

// Registers to read thermopile voltage and sensor temperature
#define TMP006_VOBJ   0x00
#define TMP006_TAMB   0x01
#define TMP006_CONFIG 0x02



// Constants for calculating object temperature
#define TMP006_B0   -0.0000294
#define TMP006_B1   -0.00000057
#define TMP006_B2   0.00000000463
#define TMP006_C2   13.4
#define TMP006_TREF 298.15
#define TMP006_A2   -0.00001678
#define TMP006_A1   0.00175
#define TMP006_S0   6.4  // * 10^-14

// Configuration Settings
#define TMP006_CFG_RESET    0x8000
#define TMP006_CFG_MODEON   0x7000
#define TMP006_CFG_1SAMPLE  0x0000
#define TMP006_CFG_2SAMPLE  0x0200
#define TMP006_CFG_4SAMPLE  0x0400
#define TMP006_CFG_8SAMPLE  0x0600
#define TMP006_CFG_16SAMPLE 0x0800
#define TMP006_CFG_DRDYEN   0x0100
#define TMP006_CFG_DRDY     0x0080

class TMP006{
    public:
        TMP006(PinName sda, PinName scl);

        // Configures sensor, use before reading from it
        void config(uint16_t samples);
        int16_t readRawDieTemperature();
        int16_t readRawVoltage();
        double readObjTempC();
        double readObjTempF();
        double readDieTempC();  
        double readDieTempF();  
        ~TMP006();

    private:
        I2C i2c;
};

#endif
