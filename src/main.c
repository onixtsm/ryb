#include <libpynq.h>
#include <stdio.h>

int main(void) {
  pynq_init();
  printf("Hello, Seaman\n");
  pynq_destroy();
  return 0;
}
