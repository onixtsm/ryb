#include <libpynq.h>
#include <stdio.h>

int main(void) {
  pynq_init();
  printf("Hello, World\n");pynq_destroy();
  return 0;
}
