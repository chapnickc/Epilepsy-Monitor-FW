#include <Arduino.h>
#include <Wire.h>
#include <MAX30100.h>

#include <vector>

using namespace std;

float T = 0.050;
int T_ms = (int) (T * 1000); // for the delay() function
int samples = 100;

vector<float> IR_data; 

float m = 2;
int dt = 1;
vector<int> ixs;       // holds the indicies of peak values

float bpm;


MAX30100 sensor;


vector<float> read_sensor_IR(int samples){
    vector<float> IR_data; 

    for(int i=0; i < samples; i++){
        sensor.readSensor();
        IR_data.push_back(sensor.IR);
        delay(T_ms);
    }

    return IR_data;
}

vector<int> find_peak_ixs(vector<float> sample, float m = 2, int dt = 1) {
    vector<int> ixs;
    for(int i=1; i < sample.size() - 1; i++){
        bool peak_detected = (sample[i] - sample[i-dt] > m) & (sample[i] - sample[i+dt] > m);
        if (peak_detected){
            ixs.push_back(i);
        }
    }
    return ixs;
}

float calc_bpm(vector<int> peak_ixs, float T = 0.050){
    vector<float> tp;
    // calculate the time at even peaks
    for(int i=0; i < peak_ixs.size(); i++){
        if(i % 2 == 0) {
            float t = peak_ixs[i] * T;
            tp.push_back(t);
        }
    }
    
    vector<float> ds;
    for(int i=0; i < tp.size()-1; i++){
        float d = tp[i+1] - tp[i];
        ds.push_back(d);
    }

    float total_interval = 0; 
    for(int i=0; i < ds.size(); i++){
        total_interval += ds[i];
    }
    
    // (1 beat/avg_interval sec)*(60 sec/1 min) = bpm
    float avg_d = total_interval/ds.size();
    float bpm = (1/avg_d) * 60;

    return bpm;
}



/*      main     */
void setup() {
    Wire.begin();
    Serial.begin(9600);
    while(!Serial){};                   // wait for serial conn
    //    sensor.printRegisters();
    sensor.begin(pw1600, i44, sr50);
}

void loop() {
    IR_data = read_sensor_IR(samples);
    ixs = find_peak_ixs(IR_data, m, dt);
    bpm = calc_bpm(ixs, T);
    Serial.println(bpm);
}

/*int main(){*/
    //setup();

    //while (1)
        //loop();
    
    //return 0; // never reached
//}


