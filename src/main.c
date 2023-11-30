#include <stdio.h>

#include "communication.h"
#include "stress_calculator.h"
#include "test_matrix_generator.h"
#include "ui.h"

int main(void) { 
  int matrix[SIZE][SIZE];

  generate_matrix_path(matrix, 9);
  dump_matrix(matrix);
  return 0;
}
