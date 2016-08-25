
#include <vector>

#include "mbed.h"

#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "ble/services/HealthThermometerService.h"
#include "ble/services/BatteryService.h"
#include "ble/services/DeviceInformationService.h"


#include "TMP006.h"
#include "MAX30100.h"
#include "HR_functions.h"

#define I2C_SDA     p4
#define I2C_SCL     p5


float T = 0.050;                            // Period

uint8_t batteryLevel = 0;

/* TMP006 Variables */
uint8_t tmp006_addr = 0x80;                 // 7-bit address of TMP006 can be 0x40-0x47,  converted to 8-bit address here
uint16_t tempSamples = TMP006_CFG_2SAMPLE;
float lastTemp = 0;


/* MAX30100 Variables */
uint8_t max30100_addr = 0xAE;
uint8_t lastBPM = 0;


const static char     DEVICE_NAME[]         = "EpilepsyMonitor";

static const uint16_t uuid16_list[]         = { GattService::UUID_HEART_RATE_SERVICE,
                                                GattService::UUID_HEALTH_THERMOMETER_SERVICE,
                                                GattService::UUID_BATTERY_SERVICE,
                                                GattService::UUID_DEVICE_INFORMATION_SERVICE};

static volatile bool  triggerSensorPolling = false;


DigitalOut led1(LED1);

/* Instantiate Sensor Objects */
MAX30100 hrSensor(I2C_SDA, I2C_SCL, max30100_addr);
TMP006 tempSensor(I2C_SDA, I2C_SCL, tmp006_addr); 

/* Instantiate Service Pointers */
HeartRateService            *hrService;
HealthThermometerService    *thermService;
BatteryService              *batteryService;
DeviceInformationService    *deviceInfo;

uint8_t readHrSensor(void){
	/* Collects T * samples number of data infrared data points
		from the MAX30100 to calculate a value for the heart rate
	*/
	uint8_t samples = 100;
	/* HR Calculation Variables */
	float m = 2.00;
	uint8_t dt = 1;

	std::vector<uint8_t> ixs;
	std::vector<float> IR_data(samples);
	

	uint8_t i;
	for(i=0; i < samples; i++){
		hrSensor.readSensor();
		IR_data[i] = (float) hrSensor.IR;
		wait(T);                         
	}


	ixs = find_peaks(IR_data, m, dt);   // get the peak indicies
	float avg_d = avg_peak_interval(ixs, T);  // calculate avergae peak to peak distance
	uint8_t bpm = calc_bpm(avg_d);              // estimate heart rate

	return bpm;
}   

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

    /* Initialize Primary Services */
    hrService = new HeartRateService(ble, lastBPM, HeartRateService::LOCATION_FINGER);
    thermService = new HealthThermometerService(ble, lastTemp, HealthThermometerService::LOCATION_BODY);
    
    /* Auxiliary Services */
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

    hrSensor.begin(pw1600, i44, sr100);               // pw1600 allows for 16-bit resolution
    tempSensor.config(tmp006_addr, tempSamples);

    while (true) {
        // check for trigger from periodicCallback()
        if (triggerSensorPolling && ble.getGapState().connected) {
            triggerSensorPolling = false;

            /* Do blocking calls as necessary for sensor polling. */
			lastBPM = readHrSensor();
            lastTemp = tempSensor.readObjTempC(tmp006_addr);

            batteryLevel++;
            if (batteryLevel == 100){
                batteryLevel = 0;
            }
 
            hrService->updateHeartRate(lastBPM);
            thermService->updateTemperature(lastTemp);
            batteryService->updateBatteryLevel(batteryLevel);
        } 
        else {
            ble.waitForEvent(); // low power wait for event
        }
    }
}
