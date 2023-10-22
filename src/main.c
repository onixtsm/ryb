#include <libpynq.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "communication.h"
#include "helpers.h"

int main(int argc, char **argv) {
  pynq_init();
  switchbox_init();
  reset_pins();

  if (argc < 3) {
    ERROR("Usage <receiver pin> <transmitter pin>");
    return 1;
  }

  uint8_t receiver = *argv[1] - '0';
  uint8_t transmitter = *argv[2] - '0';
  set_pin(receiver, 0);
  set_pin(transmitter, 1);

  char c;
  char m[40];

  pid_t pid = fork();
  if (pid < 0) {
    return 1;
  } else if (pid == 0) {
    printf("Message :");
    while ((c = recieve_data(receiver)) != '\n') {
      printf("%c", c);
      fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
  } else {
    printf("Message? ");
    scanf("%s", m);
    for (size_t i = 0; i < strlen(m); ++i) {
      transmit_data(transmitter, m[i]);
    }
    transmit_data(transmitter, '\n');
  }
  wait(NULL);
  free_pin(receiver);
  free_pin(transmitter);

  switchbox_reset();
  pynq_destroy();
  return 0;
}
