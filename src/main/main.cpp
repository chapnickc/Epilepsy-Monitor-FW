
#include <vector>
#include "mbed.h"
#include "TMP006.h"
#include "MAX30100.h"
#include "MMA8452.h"

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

MAX30100    MAX30100(I2C_SDA, I2C_SCL);
TMP006      TMP006(I2C_SDA, I2C_SCL); 
MMA8452     MMA8452(I2C_SDA, I2C_SCL); 

uint16_t BPM;
uint16_t RED, IR;
uint8_t batteryLevel= 0;
float T = 0.05; // sampling period
float MAX_temp = 0;
float TMP_temp = 0;  
std::vector<int16_t> accel(3);     // array for acceleration data


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
   pc.printf("Restarting advertising.");
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


   /*  Reallocate space in memory and instantiate services */
   hrService       = new HeartRateService(ble, BPM, HeartRateService::LOCATION_FINGER);
   thermService    = new HealthThermometerService(ble, TMP_temp, HealthThermometerService::LOCATION_BODY);
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
   pc.printf("\nConnected to nRF51...");

   Ticker ticker;
   ticker.attach(periodicCallback, 2); // blink LED every second

   pc.printf("Starting MAX30100");
   MAX30100.begin(SPO2_mode, pw1600, i11, i24, sr100);               // pw1600 allows for 16-bit resolution
   //MAX30100.startTemperatureSampling();
   pc.printf("Starting MMA8452");
   MMA8452.begin();

   pc.printf("Starting TMP006");
   TMP006.config(TMP006_CFG_2SAMPLE);



   BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
   ble.init(bleInitComplete);

   /* wait the BLE object to initialize*/  
   while (ble.hasInitialized() == false){};

   while (true) {
      // check for trigger from periodicCallback()
      if (triggerSensorPolling && ble.getGapState().connected) {
         triggerSensorPolling = false;

         /* Do blocking calls as necessary for sensor polling. */
         
         MMA8452.readAcceleration();
         TMP006.readObjTempF();     // need to add temperature attribute to TMP006

         /* Get the sensor values */
         size_t N = 10;
         float T = 0.008;
         std::vector<uint16_t> IR_buffer(N);
         std::vector<uint16_t> RED_buffer(N);
         std::vector<float> TMP_buffer(N);

         for (int i=0; i < N; i++){
            MAX30100.readFIFO();
            TMP006.readObjTempF();     // need to add temperature attribute to TMP006
            IR_buffer[i] = MAX30100.getIR();
            RED_buffer[i] = MAX30100.getRED();
            TMP_buffer[i] = TMP006.getObjTempF();
            wait_ms(10);
         }

         // just show the average IR and temperature values for now
         BPM = std::accumulate(IR_buffer.begin(), IR_buffer.end(), 0.0)/IR_buffer.size();
         RED = std::accumulate(RED_buffer.begin(), RED_buffer.end(), 0.0)/RED_buffer.size();
         TMP_temp = std::accumulate(TMP_buffer.begin(), TMP_buffer.end(), 0.0)/TMP_buffer.size();

         MMA8452.readAcceleration();        
         accel = MMA8452.getAcceleration(); 
         
            //pc.printf("IR: %u | RED: %u | TMP_temp: %f | XYZ: (%d, %d, %d)\n", IR,RED,TMP_temp,accel[0],accel[1],accel[2]);
         pc.printf("IR: %u | RED: %u | TMP_temp: %f | XYZ: (%d, %d, %d)\n", BPM,RED, TMP_temp,accel[0],accel[1],accel[2]);

         batteryLevel++;
         if (batteryLevel == 100){ 
            batteryLevel = 0; 
         }

         hrService->updateHeartRate(BPM);
         thermService->updateTemperature(TMP_temp);
         batteryService->updateBatteryLevel(batteryLevel);

      } 
      else {
         ble.waitForEvent(); // low power wait for event
      }
   }
}
