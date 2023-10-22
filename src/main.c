#include <libpynq.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "communication.h"
#include "helpers.h"
#include "switchbox.h"
#include "uart.h"
#include "util.h"

int main(int argc, char **argv) {
  pynq_init();
  switchbox_init();
  reset_pins();
  if (argc < 2) {
    ERROR("Not enough args\n");
    return 1;
  }

  const uint8_t pin = *argv[2] - '0';
  int err;

  switch (*argv[1]) {
    case 't':
      err = set_pin(pin, 0);
      if (err != 0) {
        return 1;
      }
      char *buffer = "Hello, seaman\n";
      for (size_t i = 0; buffer[i] != 0; i++) {
        printf("%c", buffer[i]);
        transmit_data(pin, buffer[i]);
      }
      break;
    case 'r':
      err = set_pin(pin, 1);
      if (err != 0) {
        ERROR("Could not set pin");
        printf("%d\n", err);
        return 1;
      }
      char info[16] = {0};
      size_t i = 0;
      do {
        info[i] = recieve_data(pin);
      }
      while (info[i++] != '\n');
      printf("done\n");
      printf("%s", info);
      fflush(stdout);
      break;
    default:
      printf("No such command\n");
  }

  pynq_destroy();
  return 0;
}
