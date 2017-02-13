#include "TMP006.h"

TMP006::TMP006(PinName sda, PinName scl): i2c(sda, scl){
   Tobj = 0;
}

TMP006::~TMP006() {}

void TMP006::config(uint16_t samples)
{ /* Configure the sensor with desired samples. */
   char data[2];
   data[0]   = 0x02;
   data[1]   = samples | TMP006_CFG_MODEON | TMP006_CFG_DRDYEN;
   i2c.write(TMP006_ADDR, data, 1); 
}

int16_t TMP006::readRawDieTemperature()
{ /* Get the raw data off the chip. */
   char reg[] = {TMP006_TAMB};
   char data[2];

   i2c.write(TMP006_ADDR, reg, 1);
   i2c.read(TMP006_ADDR, data, 2);

   int16_t raw = (data[0] << 8) | data[1];
   raw >>= 2;

   return raw;
}

int16_t TMP006::readRawVoltage(){ 
   /* Read the raw voltage from the thermopile */
   char reg[] = {TMP006_VOBJ};
   char data[2];
   i2c.write(TMP006_ADDR, reg, 1);
   i2c.read(TMP006_ADDR, data, 2);

   int16_t Vobj = (data[0] << 8) | data[1];
   return Vobj;
}

double TMP006::readObjTempC() { 
   /* Calculate the object temperature in celcius.*/

   double Tdie = readRawDieTemperature();
   double Vobj = readRawVoltage();

   Vobj *= 156.25;  // 156.25 nV per LSB
   Vobj /= 1000000000; // nV -> V
   Tdie *= 0.03125; // convert to celsius
   Tdie += 273.15; // convert to kelvin

   // Equations for calculating temperature found in section 5.1 in the user guide
   double tdie_tref = Tdie - TMP006_TREF;
   double S = (1 + TMP006_A1*tdie_tref + TMP006_A2*tdie_tref*tdie_tref);
   S *= TMP006_S0;
   S /= 10000000;
   S /= 10000000;

   double Vos = TMP006_B0 + TMP006_B1*tdie_tref + TMP006_B2*tdie_tref*tdie_tref;
   double fVobj = (Vobj - Vos) + TMP006_C2*(Vobj-Vos)*(Vobj-Vos);

   Tobj = sqrt(sqrt(Tdie * Tdie * Tdie * Tdie + fVobj/S));
   Tobj -= 273.15; // Kelvin -> *C

   return Tobj;
}


double TMP006::readDieTempC() { 
   /* Calculate the die temperature in celcius.*/
   double Tdie = readRawDieTemperature();
   Tdie *= 0.03125; // convert to celsius
   return Tdie;
}


double TMP006::readDieTempF() { 
   /* Calculate the die temperature in fahrenheit.*/
   double Tdie = readDieTempC();
   
   Tdie = Tdie * 9.0/5.0 + 32.0; // convert to fahrenheit
   return Tdie;
}

double TMP006::getObjTempF() { 
   /* return the obj temperature in fahrenheit.*/

   return Tobj * 9.0/5.0 + 32.0; // convert to fahrenheit
}

double TMP006::getObjTempC() { 
   /* return the obj temperature in fahrenheit.*/

   return Tobj; // convert to fahrenheit
}


