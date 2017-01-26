#ifndef MMA8452_H
#define MMA8452_H

#include "mbed.h"
#include <stdint.h>
#include <stdbool.h>
#include <vector>


// 8-bit I2C address with R/W bit set
#define MMA8452_ADDR             0x3B 

#define MMA8452_STATUS           0x00 
#define MMA8452_OUT_X_MSB        0x01
#define MMA8452_OUT_X_LSB        0x02
#define MMA8452_OUT_Y_MSB        0x03
#define MMA8452_OUT_Y_LSB        0x04
#define MMA8452_OUT_Z_MSB        0x05
#define MMA8452_OUT_Z_LSB        0x06
#define MMA8452_SYSMOD           0x0B
#define MMA8452_WHO_AM_I         0x0D
#define MMA8452_XYZ_DATA_CFG     0x0E
#define MMA8452_PL_STATUS        0x10
#define MMA8452_PL_CFG           0x11
#define MMA8452_PL_COUNT         0x12
#define MMA8452_PL_BF_ZCOMP      0x13
#define MMA8452_PL_THS_REG       0x14
#define MMA8452_FF_MT_CFG        0X15
#define MMA8452_FF_MT_SRC        0X16
#define MMA8452_FF_MT_THS        0X17
#define MMA8452_FF_COUNT         0X18
#define MMA8452_ASLP_COUNT       0x29
#define MMA8452_CTRL_REG_1       0x2A
#define MMA8452_CTRL_REG_2       0x2B
#define MMA8452_CTRL_REG_3       0x2C
#define MMA8452_CTRL_REG_4       0x2D
#define MMA8452_CTRL_REG_5       0x2E

// Defined in table 13 of the Freescale PDF
// (make these names beter)
#define STANDBY                 0x00    // State value returned after a SYSMOD request, it can be in state STANDBY, WAKE or SLEEP
#define WAKE                    0x01    // State value returned after a SYSMOD request, it can be in state STANDBY, WAKE or SLEEP
#define SLEEP                   0x02    // State value returned after a SYSMOD request, it can be in state STANDBY, WAKE or SLEEP
#define ACTIVE                  0x01    // Stage value returned and set in Control Register 1, it can be STANDBY=00, or ACTIVE=01

#define TILT_STATUS             0x03    // Tilt Status (Read only)
#define SRST_STATUS             0x04    // Sample Rate Status Register (Read only)
#define SPCNT_STATUS            0x05    // Sleep Count Register (Read/Write)
#define INTSU_STATUS            0x06    // Interrupt Setup Register
#define MODE_STATUS             0x07    // Mode Register (Read/Write)
#define SR_STATUS               0x08    // Auto-Wake and Active Mode Portrait/Landscape Samples per Seconds Register (Read/Write)
#define PDET_STATUS             0x09    // Tap/Pulse Detection Register (Read/Write)
#define PD_STATUS               0xA     // Tap/Pulse Debounce Count Register (Read/Write)

/* Mask Definitions */
#define MMA8452_ACTIVE_MASK             0x01    // for enabling/disabling standby
#define MMA8452_STANDBY_MASK            0xFE
#define MMA8452_DYNAMIC_RANGE_MASK      0xFC    // for dynamic range R/W
#define MMA8452_DATA_RATE_MASK          0xC7    //  for shift for data rate R/w
#define MMA8452_DATA_RATE_MASK_SHIFT    0x03
#define MMA8452_WRITE_MASK              0xFE    // for general R/W
#define MMA8452_READ_MASK               0x01
#define MMA8452_BIT_DEPTH_MASK          0xFD    // for bit depth R/W
#define MMA8452_BIT_DEPTH_MASK_SHIFT    0x01
#define MMA8452_STATUS_ZYXDR_MASK       0x08    // status masks and shifts
#define MMA8452_STATUS_ZDR_MASK         0x04
#define MMA8452_STATUS_YDR_MASK         0x02
#define MMA8452_STATUS_XDR_MASK         0x01




// 8bit I2C address of the MMA8452 
// with R/W bit set
//#define MMA8452_ADDR           0xAE  

// Registers
#define MMA8452_INT_STATUS     0x00  // Which interrupts are tripped
#define MMA8452_INT_ENABLE     0x01  // Which interrupts are active
#define MMA8452_FIFO_WR_PTR    0x02  // Where data is being written
#define MMA8452_OVRFLOW_CTR    0x03  // Number of lost samples
#define MMA8452_FIFO_RD_PTR    0x04  // Where to read from
#define MMA8452_FIFO_DATA      0x05  // Ouput data buffer
#define MMA8452_MODE_CONFIG    0x06  // Control register
#define MMA8452_SPO2_CONFIG    0x07  // Oximetry settings
#define MMA8452_LED_CONFIG     0x09  // Pulse width and power of LEDs
#define MMA8452_TEMP_INTG      0x16  // Temperature value, whole number
#define MMA8452_TEMP_FRAC      0x17  // Temperature value, fraction
#define MMA8452_REV_ID         0xFE  // Part revision
#define MMA8452_PART_ID        0xFF  // Part ID, normally 0x11



class MMA8452 {
   public:
      MMA8452(PinName sda, PinName scl);
      ~MMA8452();
      std::vector<int16_t> getAcceleration() const;
      void begin();
      void process_data(char* buffer);
      void readAcceleration();
   private:
      I2C i2c;
      std::vector<int16_t> accel = std::vector<int16_t>(3);       // x, y, z acceleration
      void write_byte(char reg, char data);
      //int16_t accel[3];       // x, y, z acceleration
};

#endif
