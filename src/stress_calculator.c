#include "stress_calculator.h"

#include <stdint.h>

float get_stress_from_crying_volume(uint8_t crying_volume) {
  if (crying_volume < 10) { // crying volume will not get lower than 10%
    return 10;
  }
  if (crying_volume > 98) { // crying volume is 100% for stress levels 50-100
    return 100;
  }
  return (float)(crying_volume - 10) * 9 / 4 + 10;
}
float get_stress_from_heartbeat(uint8_t heartbeat) {
  if (heartbeat <= 61) { // heartbeat does not get lower than 60 BPM (assuming it's alive)
    return 10;
  }
  return (float)(heartbeat - 60) / 2 + 10; // HB range from 60-240 corresponds to stress 10-100%
}
