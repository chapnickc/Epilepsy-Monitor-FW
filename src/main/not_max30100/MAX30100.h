/*
 Library for the Maxim MAX30100 pulse oximetry system
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAX30100_H
#define MAX30100_H

#include "mbed.h"


/******************************************************************************/
/*********** PULSE OXIMETER AND HEART RATE REGISTER MAPPING  **************/
/******************************************************************************/

// status registers
#define MAX30100_INT_STATUS          0x00
#define MAX30100_INT_ENABLE          0x01

// FIFO registers
#define MAX30100_FIFO_W_POINTER      0x02
#define MAX30100_OVR_COUNTER         0x03
#define MAX30100_FIFO_R_POINTER      0x04
#define MAX30100_FIFO_DATA_REG       0x05

// configuration registers
#define MAX30100_CONFIG              0x06
#define MAX30100_SPO2_CONFIG         0x07
#define MAX30100_LED_CONFIG          0x09

// temperature registers
#define MAX30100_TEMP_INTEGER        0x16
#define MAX30100_TEMP_FRACTION       0x17

// PART ID registers
#define MAX30100_REVISION_ID         0xFE
#define MAX30100_PART_ID             0xFF

#define I_AM_MAX30100                0x11

/************************************** REGISTERS VALUE *******************************************/

// I2C address
#define MAX30100_ADDRESS             0xAE

//Enable interrupts
#define MAX30100_INT_ENB_A_FULL      ((uint8_t)0x80)
#define MAX30100_INT_ENB_TEMP_RDY    ((uint8_t)0x40)
#define MAX30100_INT_ENB_HR_RDY      ((uint8_t)0x20)
#define MAX30100_INT_ENB_SO2_RDY     ((uint8_t)0x10)

//Mode configuration
#define MAX30100_MODE_SHDN           ((uint8_t)0x80)
#define MAX30100_MODE_RESET          ((uint8_t)0x40)
#define MAX30100_MODE_TEMP_EN        ((uint8_t)0x08)
#define MAX30100_MODE_HR             ((uint8_t)0x02)
#define MAX30100_MODE_SPO2           ((uint8_t)0x03)

//SPO2 configuration
#define MAX30100_SPO2_HI_RES_EN           ((uint8_t)0x40)

typedef enum{ // This is the same for both LEDs
    pw200,    // 200us pulse
    pw400,    // 400us pulse
    pw800,    // 800us pulse
    pw1600    // 1600us pulse
}pulseWidth;

typedef enum{
    sr50,    // 50 samples per second
    sr100,   // 100 samples per second
    sr167,   // 167 samples per second
    sr200,   // 200 samples per second
    sr400,   // 400 samples per second
    sr600,   // 600 samples per second
    sr800,   // 800 samples per second
    sr1000   // 1000 samples per second
}sampleRate;

typedef enum{
    i0,    // No current
    i4,    // 4.4mA
    i8,    // 7.6mA
    i11,   // 11.0mA
    i14,   // 14.2mA
    i17,   // 17.4mA
    i21,   // 20.8mA
    i27,   // 27.1mA
    i31,   // 30.6mA
    i34,   // 33.8mA
    i37,   // 37.0mA
    i40,   // 40.2mA
    i44,   // 43.6mA
    i47,   // 46.8mA
    i50    // 50.0mA
}ledCurrent;

typedef enum{
    low,    // low resolution SPO2
    high    // high resolution SPO2 (16 bit with 1.6ms LED pulse width)
}high_resolution;

typedef enum
{
    OXIMETER_OK = 0,
    OXIMETER_ERROR = 1,
    OXIMETER_TIMEOUT = 2,
    OXIMETER_NOT_IMPLEMENTED = 3
} OXIMETER_StatusTypeDef;

/**
 * @brief  MAX30100 driver extended internal structure definition
 */
typedef struct
{
    OXIMETER_StatusTypeDef (*Enable_Free_Fall_Detection) (void);
    OXIMETER_StatusTypeDef (*Disable_Free_Fall_Detection) (void);
    OXIMETER_StatusTypeDef (*Get_Status_Free_Fall_Detection) (uint8_t *);
} MAX30100_DrvExtTypeDef;

class MAX30100 {
public:
        
    /* Public Methods */
    
    uint16_t IR;      // Last infrared datapoint
    uint16_t HR;      // Last heart rate datapoint
    uint16_t SPO2;    // Last oximetry datapoint
    
    void  setLEDs(pulseWidth pw, ledCurrent red, ledCurrent ir);  // Sets the LED state
    void  setSPO2(sampleRate sr, high_resolution hi_res); // Setup the SPO2 sensor, disabled by default
    int   getNumSamp(void);       // Get number of samples
    void  readSensor(void);       // Updates the values
    void  shutdown(void);   // Instructs device to power-save
    void  reset(void);      // Resets the device
    void  startup(void);    // Leaves power-save
    char  getRevID(void);   // Gets revision ID
    char  getPartID(void);  // Gets part ID
    void  begin(pulseWidth pw = pw1600, // Longest pulseWidth
                ledCurrent ir = i50,    // Highest current
                sampleRate sr = sr100); // 2nd lowest sampleRate
    void  init(pulseWidth pw, sampleRate sr, high_resolution hi_res, ledCurrent red, ledCurrent ir);
    void  setTemp(void);
    int   readTemp(void);
    void  setSPO2mode(void);
    void  setInterruptSPO2(void);
    void  printRegisters(void); // Dumps contents of registers for debug
};

#endif /* __MAX30100_H */