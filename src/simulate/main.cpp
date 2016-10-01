#include <vector>

#include "mbed.h"
#include "TMP006.h"
#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "ble/services/HealthThermometerService.h"
#include "ble/services/BatteryService.h"
#include "ble/services/DeviceInformationService.h"

#define I2C_SDA     p4
#define I2C_SCL     p5

#define tempSensorAddress         0x80     // I2C address of TMP006, can be 0x40-0x47
//int max30100_addr = 0xAE;


const static char     DEVICE_NAME[]         = "EpilepsyMonitor";

static const uint16_t uuid16_list[]         = { GattService::UUID_HEART_RATE_SERVICE,
                                                GattService::UUID_HEALTH_THERMOMETER_SERVICE,
                                                GattService::UUID_BATTERY_SERVICE,
                                                GattService::UUID_DEVICE_INFORMATION_SERVICE};


static volatile bool  triggerSensorPolling = false;


/* Initialize Simulation Variables */
float tempCounter = 100;     
uint8_t hrmCounter = 100;     
uint8_t batteryLevel = 50;
//uint16_t samples = TMP006_CFG_2SAMPLE;


DigitalOut led1(LED1);

/* Instantiate Sensor Objects */
//TMP006 healthThermometer(I2C_SDA, I2C_SCL, tempSensorAddress); 
//MAX30100 sensor(I2C_SDA, I2C_SCL, max30100_addr);

/* Instantiate Services */
HeartRateService         *hrService;
HealthThermometerService *thermService;
BatteryService           *batteryService;
DeviceInformationService *deviceInfo;

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params){
    BLE::Instance(BLE::DEFAULT_INSTANCE).gap().startAdvertising(); // restart advertising
}

void periodicCallback(void){   
    /* This function executes in interrupt context, so we trigger
     * "heavy-weight" sensor polling to run in the main thread. 
     */
    led1 = !led1;                                   /*  blink LED1 while waiting for BLE events */
    triggerSensorPolling = true;
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params){
    BLE &ble          = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);

    /* Initialize Services */
    hrService = new HeartRateService(ble, hrmCounter, HeartRateService::LOCATION_FINGER);
    thermService = new HealthThermometerService(ble, tempCounter, HealthThermometerService::LOCATION_BODY);
  
    batteryService = new BatteryService(ble, batteryLevel);
    deviceInfo = new DeviceInformationService(ble, "ARM", "Model1", "SN1", "hw-rev1", "fw-rev1", "soft-rev1");

    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_HEART_RATE_SENSOR);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_THERMOMETER);
//    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::PULSE_OXIMETER_GENERIC);

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();
}


int main(void){

    led1 = 1;
    Ticker ticker;
    ticker.attach(periodicCallback, 1); // blink LED every second

    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    ble.init(bleInitComplete);

    while (ble.hasInitialized()  == false){
        /* SpinWait for initialization to complete, since the
         * BLE object is used in the main loop below. 
         */
    }

    while (true) {
        // check for trigger from periodicCallback()
        if (triggerSensorPolling && ble.getGapState().connected) {
            triggerSensorPolling = false;

            /* Do blocking calls as necessary for sensor polling. */
            hrmCounter++;
            if (hrmCounter == 180) { //  100 <= HRM bps <=175
                hrmCounter = 100;
            }

            tempCounter++;
            if (tempCounter == 212) {
                tempCounter = 5;
            }

            batteryLevel++;
            if (batteryLevel > 100) {
                batteryLevel = 20;
            }

            hrService->updateHeartRate(hrmCounter);
            thermService->updateTemperature(tempCounter);
            batteryService->updateBatteryLevel(batteryLevel);
        } 
        else {
            ble.waitForEvent(); // low power wait for event
        }
    }
}
