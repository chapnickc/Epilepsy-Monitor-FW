
#include <vector>
#include "mbed.h"
#include "TMP006.h"
#include "MAX30100.h"
#include "HR_functions.h"
#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "ble/services/HealthThermometerService.h"
#include "ble/services/BatteryService.h"
#include "ble/services/DeviceInformationService.h"

#define I2C_SDA     p4
#define I2C_SCL     p5
#define DEBUG       1

Serial      pc(USBTX, USBRX);
DigitalOut  led1(LED1);

MAX30100 hrSensor(I2C_SDA, I2C_SCL);
TMP006   tmpSensor(I2C_SDA, I2C_SCL); 


float   T           = 0.05; // sampling period
float   lastTemp    = 0.0;
uint8_t lastIR      = 0;
uint8_t batteryLevel= 0;

static volatile bool    triggerSensorPolling = false;
static const char       DEVICE_NAME[]       = "EpilepsyMonitor";
static const uint16_t   uuid16_list[]       = { GattService::UUID_HEART_RATE_SERVICE,
                                                GattService::UUID_HEALTH_THERMOMETER_SERVICE,
                                                GattService::UUID_BATTERY_SERVICE,
                                                GattService::UUID_DEVICE_INFORMATION_SERVICE};

static HeartRateService            *hrService;
static HealthThermometerService    *thermService;
static BatteryService              *batteryService;
static DeviceInformationService    *deviceInfo;

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params){
    BLE::Instance(BLE::DEFAULT_INSTANCE).gap().startAdvertising(); // restart advertising
}

void periodicCallback(void){
    /* This function executes in interrupt context, so we trigger
        "heavy-weight" sensor polling to run in the main thread. */
    led1 = !led1;   //  blink LED1 while waiting for BLE events
    triggerSensorPolling = true;
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params){
    BLE &ble          = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);

    /* Initialize services */
    hrService       = new HeartRateService(ble, lastIR, HeartRateService::LOCATION_FINGER);
    thermService    = new HealthThermometerService(ble, lastTemp, HealthThermometerService::LOCATION_BODY);
    batteryService  = new BatteryService(ble, batteryLevel);
    deviceInfo      = new DeviceInformationService(ble, "ARM", "Model1", "SN1", "hw-rev1", "fw-rev1", "soft-rev1");

    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_HEART_RATE_SENSOR);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_THERMOMETER);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));

    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();
}


int main(void){
    pc.baud(115200);

    Ticker ticker;
    ticker.attach(periodicCallback, 1); // blink LED every second

    hrSensor.begin(HR_mode, pw1600, i44, sr100); // pw1600 allows for 16-bit resolution
    tmpSensor.config(TMP006_CFG_2SAMPLE);

    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    ble.init(bleInitComplete);

    /* wait the BLE object to initialize*/  
    while (ble.hasInitialized() == false){};

    while (true) {
            // check for trigger from periodicCallback()
        if (triggerSensorPolling && ble.getGapState().connected) {
            triggerSensorPolling = false;

            /* Do blocking calls as necessary for sensor polling. */
			lastIR = hrSensor.rawIR;
            lastTemp = tmpSensor.readObjTempC();

            batteryLevel++;
            if (batteryLevel == 100){ 
                batteryLevel = 0; 
            }
 
            hrService->updateHeartRate(lastIR);
            thermService->updateTemperature(lastTemp);
            batteryService->updateBatteryLevel(batteryLevel);

            if (DEBUG){
                pc.printf("IR: %u | Temp (deg C): %f\r\n", lastIR, lastTemp);  
            }
        } 
        else {
            ble.waitForEvent(); // low power wait for event
        }
    }
}
