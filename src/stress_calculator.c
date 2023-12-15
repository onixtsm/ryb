#include "stress_calculator.h"

#include <stdint.h>

float get_stress_from_crying_volume(uint8_t crying_volume) {
  if (crying_volume < 10) {
    return 10;
  }
  if (crying_volume > 50) {
    return 100;
  }
  return (float)(crying_volume - 10) * 9 / 4 + 10;
}
float get_stress_from_heartbeat(uint8_t heartbeat) {
  if (heartbeat < 60) {
    return 10;
  }
  return (float)(heartbeat - 60) / 2 + 10;
}

FA add_freq_and_amplitude(uint8_t freq, uint8_t amplitude) {
  if (freq > SIZE - 1 || amplitude > SIZE - 1) {
    return 0b11111111;
  }
  return (FA)(freq << 4) | amplitude;
}

uint8_t fa2amp(FA fa) {
  return fa & 0b00001111;
}

uint8_t fa2freq(FA fa) {
  return (fa & 0b11110000) >> 4;
}

