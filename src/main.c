#include <libpynq.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "communication.h"
#include "helpers.h"
#include "ui.h"

/* Call with TX pin to use(should be set as TX)
will ask for string to send, and appends \n at the end*/
int sendChars(int pin) {
  char str[80];
  printf("String to send: ");
  scanf(" %s", str);
  int r = 0;
  for (size_t i = 0; i < strlen(str); i++) {
    r = transmit_data(pin, str[i]);
    if (r != 0) return r;
  }
  r = transmit_data(pin, '\n');
  if (r != 0) return r;
  return 0;
}
void printHelp() {
  // print usage information
  printf(
      "== UART TESTING GROUP 10 ==\nCommands:\nh - print this help section\np - print current assigned pins\nt[0|1] - send a "
      "series of characters from TX "
      "pin [0|1]\nr[0|1] - receive "
      "characters until \\n from RX pin [0|1]\ns[r|t] - set (t=TX or r=RX) pin\n");
}

int main() {
  pynq_init();
  switchbox_init();
  reset_pins();
  init_font("./fonts/ILGH16XB.FNT");
  char c[3];             // store current command
  int TX[2] = {10, 10};  // store assigned pins
  int RX[2] = {10, 10};
  int r = 0;  // result/error temporary store
  bool run = true;
  pid_t pid;

  printHelp();
  for (; run;) {
    printf("Choose a command: ");
    scanf(" %s", c);
    r = 0;
    switch (c[0]) {
      case 't':
        // send sth
        if (c[1] > '2') {
          printf("Invalid range\n");
          break;
        }
        if (TX[atoi(&c[1])] == 10) {
          printf("Pin not yet defined\n");
          break;
        }
        r = sendChars(TX[atoi(c)]);
        break;
      case 'r':
        if (c[1] > '2') {
          printf("Invalid range\n");
          break;
        }
        if (RX[atoi(&c[1])] == 10) {
          printf("Pin not yet defined\n");
          break;
        }
        pid = fork();  // fork a childprocess for receiving the data, the main process will enforce a timeout
        if (pid < 0) {
          printf("Error creating childprocess\n");
          break;
        }
        if (pid == 0) {  // childprocess
          uint8_t rec;
          while ((rec = recieve_data(RX[atoi(&c[1])])) != '\n') printf("%c", rec);
          printf("\n");
          return EXIT_SUCCESS;
        } else {  // this same process
          // time_t clock(void) isn't implemented on pynq so this is the way we're timing
          bool running = true;
          for (int i = 0; i < 8000; i++) {
            sleep_msec(1);
            if (waitpid(pid, NULL, WNOHANG) != 0) {  // child process stopped; received a newline
              running = false;
              break;
            }
          }
          if (running) {  // if the child process did not terminate following a newline, kill it here
            kill(pid, SIGKILL);
            printf("timed out waiting for data\n");
          }
          break;
        }
        break;
      case 's':
        // set pin for RX or TX
        if (strchr("rt", c[1]) == NULL) {
          printf("r or t not specified\n");
          break;
        }
        int* p = RX;  // pointer to relevant array (RX/TX)
        if (c[1] == 't') p = TX;
        if (p[0] != 10 && p[1] != 10) {
          printf("Pins already defined\n");
          break;
        }
        // ask for pin to set
        printf("Set which pin(0-5)? ");
        int pin;
        scanf(" %d", &pin);
        r = set_pin(pin, (c[1] == 't') ? 0 : 1);
        // if succesfully set, store in array
        if (r == 0) ((p[0] == 10) ? (p[0] = pin) : (p[1] = pin));
        // start listening for UART signals and display on screen
        pid = fork();  // fork a childprocess for receiving the data, the main process will enforce a timeout
        if (pid < 0) {
          printf("Error creating childprocess\n");
          break;
        }
        if (pid == 0) {  // childprocess
          int childpin = pin;
          uint8_t rec;
          // font height is 16px
          char formatted[100] = "";
          sprintf(formatted, "#f000pin %d:", childpin);
          int nr2 = (p[1] == childpin);
          draw_string(formatted, 0, (nr2 ? (DISPLAY_HEIGHT / 2) : 0));
          char str[80];
          while (1) {
            int i = 0;
            while ((rec = recieve_data(childpin)) != '\n') str[i++] = rec;
            str[i] = 0;
            // display on screen
            clear_lines(1, 0, (nr2 ? (DISPLAY_HEIGHT / 2) : 0) + 16, 0);
            sprintf(formatted, "#f00f->%s", str);
            printf("%d, %d\n", childpin, nr2);
            draw_string(formatted, 0, (nr2 ? (DISPLAY_HEIGHT / 2) : 0) + 16);
          }
          // if child process smh reaches this part, terminate
          return EXIT_SUCCESS;
        }  // else just continue
        break;
      case 'p':
        // print only if already set
        for (int i = 0; i < 2; i++)
          if (TX[i] != 10) printf("TX pin %d = %d\n", i, TX[i]);
        for (int i = 0; i < 2; i++)
          if (RX[i] != 10) printf("RX pin %d = %d\n", i, RX[i]);
        break;
      case 'q':
        printf("Bye!\n");
        run = false;
        break;
      case 'h':
        printHelp();
        break;
      default:
        printf("Invalid command '%s'\n", c);
        break;
    }
    if (r != 0) {
      printf("Error nr %d\n", r);
    }
  }
  for (int i = 0; i < 2; i++)
    if (TX[i] != 10) free_pin(TX[i]);
  for (int i = 0; i < 2; i++)
    if (RX[i] != 10) free_pin(RX[i]);

  switchbox_reset();
  pynq_destroy();
  return 0;
}
