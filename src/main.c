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

#define FONT_SIZE 24

int main(void) {
  pynq_init();
  switchbox_init();
  reset_pins();
  init_font("./fonts/ILGH24XB.FNT");

  switchbox_set_pin(IO_AR0, SWB_PWM0);
  switchbox_set_pin(IO_AR1, SWB_PWM1);
  pwm_init(PWM0, 256);
  pwm_init(PWM1, 256);

  const int UARTpin = 0;

  uint8_t rec = 0;
  char formatted[100] = "";

  sprintf(formatted, "#fc60Waiting for data...");
  draw_string(formatted, 0, DISPLAY_HEIGHT / 2 - (FONT_SIZE / 2));

  bool received = false;

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
    char freq = rec >> 4;         // the first 4 received bits (in case of 0110 1101, >> 4 will result in 0000 0110)
    char amp = rec & 0b00001111;  // the last 4 received bits, binary AND to set first 4 to zero (0b00001111 == 15)
    // char freq = (uint8_t)(rec << 4) >> 4; // [ALTERNATIVE] the last 4 received bits, shift left and then right to set first 4
    // bits to zero

    printf("rec: %d, amp: %d, freq: %d\n", rec, amp, freq);

    pwm_set_duty_cycle(PWM0, amp * 16 + 8);  // add 8 to range PWM from 8-248 instead of 0-240
    pwm_set_duty_cycle(PWM1, freq * 16 + 8);

    // Display on screen; Rubric: Both received amplitude as well as frequency commands are displayed as well as their correct
    // corresponding duty cycles. font height is 16px
    clear_lines(2, 0, FONT_SIZE, 0);
    clear_lines(2, 0, (DISPLAY_HEIGHT / 2) + FONT_SIZE, 0);
    sprintf(formatted, "#07e0Amplitude: %d%%", (int)((float)amp / 16 * 100) + 3);  // green
    draw_string(formatted, 0, FONT_SIZE);
    sprintf(formatted, "#001fFrequency: %.2fHz", (float)freq / 30 + 0.2);  // blue
    draw_string(formatted, 0, FONT_SIZE * 2);
    sprintf(formatted, "#07e0Amplitude: %d%%", (int)((float)amp / 16 * 100) + 3);  // green
    draw_string(formatted, 0, (DISPLAY_HEIGHT / 2) + FONT_SIZE);
    sprintf(formatted, "#001fFrequency: %d%%", (int)((float)freq / 16 * 100));  // blue
    draw_string(formatted, 0, (DISPLAY_HEIGHT / 2) + FONT_SIZE * 2);
  }

  pwm_destroy(PWM0);
  pwm_destroy(PWM1);

  switchbox_reset();
  pynq_destroy();
  return 0;
}
