#include <inttypes.h>
#include <stdio.h>

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

int main(void) {
  for (size_t i = 0; i <= 100; ++i) {
    printf("Cry: %zu -> %.2f\n", i, get_stress_from_crying_volume(i));
  }
  return 0;
}
