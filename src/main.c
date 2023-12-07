#include <libpynq.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "communication.h"
#include "helpers.h"
#include "ui.h"

#define UARTDEBUG 0

#define FONT_SIZE 24

int main(void) {
  sleep_msec(12000);
  pynq_init();
  switchbox_init();
  reset_pins();
  init_font("./fonts/ILGH24XB.FNT");

  switchbox_set_pin(IO_AR0, SWB_PWM0);
  switchbox_set_pin(IO_AR1, SWB_PWM1);
  pwm_init(PWM0, 100000);
  pwm_init(PWM1, 100000);

  const int UARTpin = 2;
  printf("set pin result %d\n", set_pin(UARTpin, 1));

  uint8_t rec = 0;
  char formatted[100] = "";

  sprintf(formatted, "#fc60Waiting for data...");
  draw_string(formatted, 0, DISPLAY_HEIGHT / 2 - (FONT_SIZE / 2));

  bool received = false;

#if UARTDEBUG
  // forks a childprocess that sends values 0-255 from uart pin 5
  set_pin(5, 0);

  pid_t pid = fork();

  if (pid == -1)
    printf("Error forking\n");
  else if (pid == 0) {
    for (char temp = 0;; temp++) {
      // UARTDEBUG
      printf("transmit %d result %d\n", temp, transmit_data(5, temp));
      sleep_msec(3000);
    }
    exit(0);
  }
#endif

  while (true) {
    rec = recieve_data(UARTpin);
    if (rec == 255) {
      printf("Error receiving data (%d)", rec);
      fill_display(0);
      draw_string("#f800Error while receiving (255)", 0, (DISPLAY_HEIGHT / 2) - (FONT_SIZE / 2));
      received = false;  // set to false to reset screen on valid data
      continue;          // try again
    }
    if (!received) {  // first incoming data
      clear_lines(2, 0, DISPLAY_HEIGHT / 2 - (FONT_SIZE / 2), 0);
      draw_string("#ffffReceived:", 0, 0);
      draw_string("#ffffDuty cycles:", 0, (DISPLAY_HEIGHT / 2));
      received = true;
    }

    // first 4 bits: freq, last 4 bits: amp
    uint8_t freq = rec >> 4;         // the first 4 received bits (in case of 0110 1101, >> 4 will result in 0000 0110)
    uint8_t amp = rec & 0b00001111;  // the last 4 received bits, binary AND to set first 4 to zero (0b00001111 == 15)
    // char freq = (uint8_t)(rec << 4) >> 4; // [ALTERNATIVE] the last 4 received bits, shift left and then right to set first 4
    // bits to zero

    printf("rec: %d, amp: %d, freq: %d\n", rec, amp, freq);

    uint8_t dutyv[5] = {5, 20, 40, 60, 80};
    uint8_t freqv[5] = {20, 35, 50, 65, 70};
    uint8_t ampv[5] = {20, 40, 60, 80, 100};

    if (freq > 4) freq = 4;
    if (amp > 4) amp = 4;

    pwm_set_duty_cycle(PWM0, dutyv[amp] * 1000);  // add 8 to range PWM from 8-248 instead of 0-240
    pwm_set_duty_cycle(PWM1, dutyv[freq] * 1000);

    // Display on screen; Rubric: Both received amplitude as well as frequency commands are displayed as well as their correct
    // corresponding duty cycles. font height is 16px
    clear_lines(2, 0, FONT_SIZE, 0);
    clear_lines(2, 0, (DISPLAY_HEIGHT / 2) + FONT_SIZE, 0);
    sprintf(formatted, "#07e0Amplitude: %d%%", ampv[amp]);  // green
    draw_string(formatted, 0, FONT_SIZE);
    sprintf(formatted, "#001fFrequency: 0.%dHz", freqv[freq]);  // blue
    draw_string(formatted, 0, FONT_SIZE * 2);
    sprintf(formatted, "#07e0Amplitude: %d%%", dutyv[amp]);  // green
    draw_string(formatted, 0, (DISPLAY_HEIGHT / 2) + FONT_SIZE);
    sprintf(formatted, "#001fFrequency: %d%%", dutyv[freq]);  // blue
    draw_string(formatted, 0, (DISPLAY_HEIGHT / 2) + FONT_SIZE * 2);
  }

  free_pin(UARTpin);

  pwm_destroy(PWM0);
  pwm_destroy(PWM1);

  switchbox_reset();
  pynq_destroy();
  return 0;
}
