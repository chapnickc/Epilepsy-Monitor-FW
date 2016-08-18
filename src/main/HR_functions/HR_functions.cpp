#include "HR_functions.h"
#include <vector>

std::vector<uint8_t> find_peaks(std::vector<float> sample, float m,  uint8_t dt) {
    /*  Determines the indicies which 
     *  correspond to peak values from the 
     *  signal. 
     */
    std::vector<uint8_t> ixs;
    for(uint8_t i=dt; i < sample.size() - dt; i++){
        bool peak_detected = (sample[i] - sample[i-dt] > m) & (sample[i] - sample[i+dt] > m);
        if (peak_detected){
            ixs.push_back(i);
        }
    }
    return ixs;
}

float avg_peak_interval(std::vector<uint8_t> peak_ixs, float T){
   /*  The first argument, `peak_ixs` is a vector 
    *  containg the indicies that correspond to peak values 
    *  in the signal vector.
    */

    std::vector<float> tp;
    std::vector<float> ds;                       // contains distance between elements 
    uint8_t i;              // index

    // calculate the time at even peaks
    for(i=0; i < peak_ixs.size(); i++){
        if(i % 2 == 0) {
            float t = peak_ixs[i] * T;
            tp.push_back(t);
        }
    }

    // find distance between each element and subsequent
    for(i=0; i < tp.size()-1; i++){
        float d = tp[i+1] - tp[i];
        ds.push_back(d);
    }

    float total_interval = 0; 
    for(i=0; i < ds.size(); i++){
        total_interval += ds[i];
    }

    float avg_d = total_interval/ds.size();
    return avg_d;
}

uint8_t calc_bpm(float avg_d){
    /* Given the average interval (in seconds) for peak 
     * infrared absorbtion, this function can
     * estimate an individual's heart rate.
     * (1 beat/avg_interval sec)*(60 sec/1 min) = bpm
     */
    uint8_t bpm = 60/avg_d;
    return bpm;
}


