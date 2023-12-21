#include <libpynq.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "communication.h"
#include "stress_calculator.h"
#include "test_matrix_generator.h"
#include "ui.h"

#define CRYING_PIN 1
#define HEARTBEAT_PIN 2
#define MOTORS_PIN 3

#define HB_TOLERANCE 5
#define CRY_TOLERANCE 10
#define STRESS_TOLERANCE 5

#define DISABLE_CRY 0

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef uint8_t FA;

FA add_freq_and_amplitude(uint8_t freq, uint8_t amplitude) {
  if (freq > 4 || amplitude > 4) {
    return 0b01000100;
  }
  if (freq < 0) freq = 0;
  if (amplitude < 0) amplitude = 0;
  return (FA)(freq << 4) | amplitude;
}

/**
 * tries to decrease amplitude, then frequency
 * waits for both separately to cause a change
 * if one of those works, recursively calls itself with current status
 * if none of those work, returns with 0, failed somewhere
 * if stress is 10(lowest value) returns with 1: found path
 * if stress increased, returns with 2: start over
 */
int find_path(FA fa, int stress) {
  uint8_t amplitude = fa & 0b00001111;
  uint8_t freq = (fa & 0b11110000) >> 4;

  FA faa = add_freq_and_amplitude(freq, amplitude - 1);
  FA faf = add_freq_and_amplitude(freq - 1, amplitude);

  int r;
  r = find_path(faa, new_stress);
  if (r > 0) return r;
  r = find_path(faf, new_stress);
  if (r > 0) return r;
  return 0;

  uint8_t err;
  do {
    err = transmit_data(MOTORS_PIN, fa);
  } while (err != 0);

  uint8_t bad = 0;
  uint8_t new_stress = stress;  // if bad, will be same as
  uint8_t volume, hb;
  time_t start = time(NULL);
  do {
#if DISABLE_CRY
    volume = 100;
#else
    volume = recieve_data(CRYING_PIN);
    if (volume > (100 + CRY_TOLERANCE)) {  // max volume value is 100% (proceeding with this might give weird stress values)
      fprintf(stderr, "Bad volume (%d)", volume);
      bad = 1;
    }
#endif
    hb = recieve_data(HEARTBEAT_PIN);
    if (hb > (240 + HB_TOLERANCE) || hb < (60 - HB_TOLERANCE)) {
      fprintf(stderr, "Bad heartbeat (%d)", hb);
      bad = 1;
    }
    if (!bad) {
      if (hb > 240)
        hb = 240;
      else if (hb < 60)
        hb = 60;
      if (volume > 100) volume = 100;
      new_stress = MIN(get_stress_from_heartbeat(hb), get_stress_from_crying_volume(volume));
      bad = 0;
    }
    usleep(1000 * 1000 / 64);
  } while (abs(new_stress - stress) < STRESS_TOLERANCE &&
           time(NULL) - start < 11);  // if diff is less than tolerance, no new step, just reading error

  if (abs(new_stress - stress) < STRESS_TOLERANCE) {  // stress did not change
    return 0;
  } else if (new_stress > stress) {  // if stress went up, did sth wrong (will start over)
    return 2;
  } else if (new_stress == 10) {  // 10 is lowest stress value possible in the mappings used -> VICTORY!
    return 1;
  }
}

int main(void) {
  pynq_init();
  reset_pins();

  int err;
  err = set_pin(CRYING_PIN, 1);
  if (err) {
    fprintf(stderr, "Cannot set CRYING_PIN\n");
    return 1;
  }

  err = set_pin(HEARTBEAT_PIN, 1);
  if (err) {
    fprintf(stderr, "Cannot set HEARTBEAT_PIN\n");
    return 1;
  }

  err = set_pin(MOTORS_PIN, 0);
  if (err) {
    fprintf(stderr, "Cannot set MOTORS_PIN\n");
    return 1;
  }

  // DEFAULT VALUES FOR FA 4 4
  FA fa = add_freq_and_amplitude(4, 4);
  int r = 0;
  while (r != 1) {  // if it fails, restart from beginning
    r = find_path(fa, 100);
    if (r == 2) printf("stress increased, trying again\n");
    if (r == 0) printf("impossible path, trying again\n");
  }

  printf("Found way\n");

  free_pin(CRYING_PIN);
  free_pin(HEARTBEAT_PIN);
  free_pin(MOTORS_PIN);
  pynq_destroy();

  return 0;
}
