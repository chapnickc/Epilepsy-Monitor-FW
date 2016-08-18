#ifndef HR_FUNCTIONS_H
#define HR_FUNCTIONS_H

#include <vector>
#include <stdint.h>

std::vector<uint8_t> find_peaks(std::vector<float> sample, float m = 2, uint8_t dt = 1);

float avg_peak_interval(std::vector<uint8_t> peak_ixs, float T);

uint8_t calc_bpm(float avg_d);
#endif
