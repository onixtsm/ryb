#ifndef STRESS_CALCULATOR_H
#define STRESS_CALCULATOR_H

#include <stdint.h>

#define SIZE 5
typedef uint8_t FA;

float get_stress_from_crying_volume(uint8_t crying_volume);

float get_stress_from_heartbeat(uint8_t heartbeat);

FA add_freq_and_amplitude(uint8_t freq, uint8_t amplitude);

uint8_t fa2amp(FA fa);

uint8_t fa2freq(FA fa);

#endif
