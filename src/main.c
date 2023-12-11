#include <libpynq.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "communication.h"
#include "helpers.h"
#include "ui.h"

#define UARTDEBUG 0

#define FONT_SIZE 24

pthread_mutex_t lock;

// these are used in both threads each cycle, so best to be global
const uint8_t dutyv[5] = {5, 20, 40, 60, 80};
const uint8_t freqv[5] = {20, 35, 50, 65, 70};
const uint8_t ampv[5] = {20, 40, 60, 80, 100};

typedef struct _vals_t {
  uint8_t freq;
  uint8_t amp;
  int index;
  uint8_t error;
} vals_t;

void *display_thread(void *arg) {
  vals_t *values = (vals_t *)arg;

  char formatted[100] = "";
  vals_t local = {255, 255, -1, 1};
  bool received = false;

  while (true) {
    if (values->index == local.index) continue;
    pthread_mutex_lock(&lock);
    local = *values;
    pthread_mutex_unlock(&lock);
    printf("[display] index: %d, amp: %d, freq: %d, err: %d\n", local.index, local.amp, local.freq, local.error);
    if (local.error != 0) {
      fill_display(0);
      received = false;  // set to false to reset screen on valid data
      switch (local.error) {
        case 1:
          draw_string("#fc60Waiting for data...", 0, (DISPLAY_HEIGHT / 2) - (FONT_SIZE / 2));
          break;
        case 2:
          draw_string("#fc60Error while receiving (255)", 0, (DISPLAY_HEIGHT / 2) - (FONT_SIZE / 2));
          break;
        default:
          draw_string("#fc60Unknown error", 0, (DISPLAY_HEIGHT / 2) - (FONT_SIZE / 2));
          break;
      }
      continue;
    }

    if (!received) {
      clear_lines(2, 0, DISPLAY_HEIGHT / 2 - (FONT_SIZE / 2), 0);
      draw_string("#ffffReceived:", 0, 0);
      draw_string("#ffffDuty cycles:", 0, (DISPLAY_HEIGHT / 2));
      received = true;
    }

    // Display on screen; Rubric: Both received amplitude as well as frequency commands are displayed as well as their correct
    // corresponding duty cycles. font height is 16px
    clear_lines(2, 11 * 12, FONT_SIZE, 0);
    sprintf(formatted, "#07e0Amplitude: %d%%", ampv[local.amp]);  // green
    draw_string(formatted, 0, FONT_SIZE);
    sprintf(formatted, "#001fFrequency: 0.%dHz", freqv[local.freq]);  // blue
    draw_string(formatted, 0, FONT_SIZE * 2);
    clear_lines(2, 11 * 12, (DISPLAY_HEIGHT / 2) + FONT_SIZE, 0);
    sprintf(formatted, "#07e0Amplitude: %d%%", dutyv[local.amp]);  // green
    draw_string(formatted, 0, (DISPLAY_HEIGHT / 2) + FONT_SIZE);
    sprintf(formatted, "#001fFrequency: %d%%", dutyv[local.freq]);  // blue
    draw_string(formatted, 0, (DISPLAY_HEIGHT / 2) + FONT_SIZE * 2);
  }
  return NULL;
}
void create_thread(pthread_t *tid, vals_t *values) {
  if (pthread_mutex_init(&lock, NULL) != 0) {
    printf("\n mutex init has failed\n");
    draw_string("#fc60Mutex init failed", 0, DISPLAY_HEIGHT / 2 - (FONT_SIZE / 2));
    exit(1);
  }

  int error = pthread_create(tid, NULL, &display_thread, (void *)values);
  if (error != 0) {
    printf("\nThread can't be created :[%s]", strerror(error));
    draw_string("#fc60Thread creation failed", 0, DISPLAY_HEIGHT / 2 - (FONT_SIZE / 2));
    exit(2);
  }
}

int main(void) {
  pynq_init();
  switchbox_init();
  reset_pins();
  init_font("./fonts/ILGH24XB.FNT");

  switchbox_set_pin(IO_AR0, SWB_PWM0);
  switchbox_set_pin(IO_AR1, SWB_PWM1);
  pwm_init(PWM0, 100000);  // 100000*10ns = 1ms, results in 1kHz
  pwm_init(PWM1, 100000);

  const int UARTpin = 2;
  printf("set pin result %d\n", set_pin(UARTpin, 1));

  uint8_t rec = 0;
  vals_t values = {255, 255, 0, 1};

  pthread_t tid;
  create_thread(&tid, &values);

#if UARTDEBUG
  // TO USE THIS: connect pin 5 to pin 2
  // forks a childprocess that sends values 0-255 from uart pin 5
  set_pin(5, 0);

  pid_t pid = fork();

  if (pid == -1)
    printf("Error forking\n");
  else if (pid == 0) {
    for (char temp = 0;; temp++) {
      transmit_data(5, temp);
      // printf("transmit %d result %d\n", temp, transmit_data(5, temp));
      sleep_msec(200);
    }
    exit(0);
  }
#endif

  while (true) {
    pthread_mutex_unlock(&lock);  // unlock to give display time to read
    rec = recieve_data(UARTpin);  // for now I assume this is long enough for the display to take it's moment
    pthread_mutex_lock(&lock);    // make sure the other thread isn't using the variable while modifying
    values.index++;
    values.error = 0;
    if (rec == 255) {
      printf("Error receiving data (%d)", rec);
      values.error = 2;
      continue;  // try again
    }

    // first 4 bits: freq, last 4 bits: amp
    values.freq = rec >> 4;         // the first 4 received bits (in case of 0110 1101, >> 4 will result in 0000 0110)
    values.amp = rec & 0b00001111;  // the last 4 received bits, binary AND to set first 4 to zero (0b00001111 == 15)
    // char freq = (uint8_t)(rec << 4) >> 4; // [ALTERNATIVE] the last 4 received bits, shift left and then right to set first 4
    // bits to zero

    printf("[main] rec: %d, amp: %d, freq: %d\n", rec, values.amp, values.freq);

    if (values.freq > 4) values.freq = 4;
    if (values.amp > 4) values.amp = 4;

    pwm_set_duty_cycle(PWM0, dutyv[values.amp] * 1000);  // add 8 to range PWM from 8-248 instead of 0-240
    pwm_set_duty_cycle(PWM1, dutyv[values.freq] * 1000);
  }

  // pthread_join(tid, NULL); shouldn't have to wait if both are in an infinite loop
  pthread_mutex_destroy(&lock);

  free_pin(UARTpin);

  pwm_destroy(PWM0);
  pwm_destroy(PWM1);

  switchbox_reset();
  pynq_destroy();
  return 0;
}
