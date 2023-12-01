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

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef uint8_t FA;

FA add_freq_and_amplitude(uint8_t freq, uint8_t amplitude) {
  if (freq > SIZE - 1 || amplitude > SIZE - 1) {
    return 0b11111111;
  }
  return (FA)(freq << 4) | amplitude;
}

int findPath(uint8_t f, uint8_t a, float stress) {
  if (f == 0 && a == 0) {
    return 1;
  }

  if (f > SIZE - 1 || a > SIZE - 1) {
    return 0;
  }
  FA freqamp = add_freq_and_amplitude(f, a);
  uint8_t err = transmit_data(MOTORS_PIN, freqamp);
  printf("%08b\n", add_freq_and_amplitude(f, a));
  if (err != 0) {
    fprintf(stderr, "Cannot send data %d, because of %d\n", freqamp, err);
  }

  float new_stress;
  do {
    uint8_t crying_volume = recieve_data(CRYING_PIN);
    if (crying_volume == 255) {
      fprintf(stderr, "Cannot receive crying volume, because of %d\n", err);
    }
    uint8_t heartbeat_freq = recieve_data(HEARTBEAT_PIN);
    if (heartbeat_freq) {
      fprintf(stderr, "Cannot receive heartbeat freq, because of %d\n", err);
    }
    new_stress = MIN(get_stress_from_crying_volume(crying_volume), get_stress_from_heartbeat(heartbeat_freq));

    sleep(1000 / 64);
  } while (stress == new_stress);

  if (new_stress > stress) {
    return 0;
  }
  int r = findPath(f - 1, a, new_stress);
  if (r) {
    return 1;
  }
  r = findPath(f, a - 1, new_stress);
  if (r) {
    return 1;
  }
  return 1;
}

int main(void) {
  int matrix[SIZE][SIZE];

  findPath(4, 4, 100);

  generate_matrix_path(matrix, 9);
  dump_matrix(matrix);
  return 0;
}
