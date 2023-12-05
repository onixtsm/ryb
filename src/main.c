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

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef uint8_t FA;

FA add_freq_and_amplitude(uint8_t freq, uint8_t amplitude) {
  if (freq > SIZE - 1 || amplitude > SIZE - 1) {
    return 0b11111111;
  }
  return (FA)(freq << 4) | amplitude;
}

int matrix[5][5];
int find_path(FA fa, int stress) {
  uint8_t amplitude = fa & 0b00001111;
  uint8_t freq = (fa & 0b11110000) >> 4;

  if (amplitude > 4 || freq > 4) {
    return 0;
  }

  int new_stress = matrix[freq][amplitude];

  if (new_stress >= stress) {
    return 0;
  }
  if (new_stress == 1) {
    return 1;
  }
  FA faa = add_freq_and_amplitude(freq, amplitude - 1);
  FA faf = add_freq_and_amplitude(freq - 1, amplitude);

  int r;
  r = find_path(faa, new_stress);
  if (r > 0) return r;
  r = find_path(faf, new_stress);
  if (r > 0) return r;
  return 0;
}

int main(void) {
  generate_matrix_path(matrix, 9);
  FA fa = add_freq_and_amplitude(4, 4);
  dump_matrix(matrix);
  int r = find_path(fa, 10);
  if (r) {
    printf("Found way\n");
  } else {
    printf("did not found a way\n");
  }
  return 0;
}
