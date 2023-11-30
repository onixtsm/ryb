#include "test_matrix_generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UPPER 8
#define LOWER 2

void dump_matrix(int matrix[SIZE][SIZE]) {
  for (size_t i = 0; i < SIZE; ++i) {
    for (size_t j = 0; j < SIZE; ++j) {
      printf("%d ", matrix[i][j]);
    }
    printf("\n");
  }
}

void zero_init_matrix(int matrix[SIZE][SIZE]) {
  for (size_t i = 0; i < SIZE; ++i) {
    for (size_t j = 0; j < SIZE; ++j) {
      matrix[i][j] = 0;
    }
  }
}

void fill_matrix(int matrix[SIZE][SIZE]) {
  for (size_t i = 0; i < SIZE; ++i) {
    for (size_t j = 0; j < SIZE; ++j) {
      if (matrix[j][i] == 0) {
        matrix[j][i] = (rand() % (UPPER - LOWER + 1) + LOWER);
      }
    }
  }
}

void generate_matrix_path(int matrix[SIZE][SIZE], size_t max_number) {
  srand(time(0));
  zero_init_matrix(matrix);
  int coin;
  size_t x;
  size_t y;
  x = 0;
  y = 0;
  for (size_t i = 2; i < max_number; ++i) {
    coin = rand() % 2;
    if (coin) {
      if (x >= 4) {
        ++y;
      } else {
        ++x;
      }
    } else {
      if (y >= 4) {
        ++x;
      } else {
        ++y;
      }
    }
    matrix[y][x] = i;
  }
  matrix[0][0] = 1;
  matrix[SIZE - 1][SIZE - 1] = max_number;
  fill_matrix(matrix);
}
