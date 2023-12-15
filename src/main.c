#include <libpynq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buttons.h"
#include "communication.h"
#include "display.h"
#include "helpers.h"
#include "hid.h"
#include "stress_calculator.h"
#include "uart.h"
#include "ui.h"
#include "util.h"

#define VL_SWITCH 0
#define HB_SWITCH 1

#define VL_PIN 1
#define HB_PIN 2
#define MT_PIN 3

#define EPS 0
#define TIMEOUT 20

int baby_calming(FA fa, int stress) {
  uint8_t amplitude = fa2amp(fa);
  uint8_t freq = fa2freq(fa);
  if (amplitude > SIZE - 1 || freq > SIZE - 1) {
    return 0;
  }

  uint8_t err, new_stress;
  size_t i = 0;
  while (i < 2) {
    err = transmit_data(MT_PIN, fa);  // Sending data twice, because UART 1 time may send a gbberish
    ++i;
    if (err != 0) {
      i--;
    }
  }
  char output[10] = {0};
  clear_lines(2, 170, 24 * 7, 0);
  sprintf(output, "%d", freq);
  draw_string(output, 170, 24 * 7);
  sprintf(output, "%d", amplitude);
  draw_string(output, 170, 24 * 8 - 1);

  uint8_t volume, hb;
  size_t tries;
  size_t global_tries = 0;
  int timeout = 0;
  do {
    if (global_tries > 0) {
      sleep_msec(1000);
    }
    global_tries++;
    tries = 0;
    do {
      run_buttons();
      if (get_switch_state(VL_SWITCH)) {
        sleep_msec(tries / 2 * 1000);
        volume = recieve_data_timer(VL_PIN, 100);
      } else {
        PRINT(stdout, "Enter volume: ");
        scanf(" %3hhd", &volume);
        while (getchar() != '\n')
          ;
      }
      char output[10];
      char color[6] = "#0f00";
      if (volume == 255) {
        strcpy(color, "#f000");
      } else if (volume > 100) {
        strcpy(color, "#ff00");
      }
      sprintf(output, "%s %d", color, volume);
      clear_lines(1, 150, 0, 0);
      draw_string(output, 150, 0);
      sprintf(output, "%zu", tries + 1);
      draw_string(output, 220, 0);
      tries++;
    } while (volume == 255);

    tries = 0;

    do {
      run_buttons();
      if (get_switch_state(HB_SWITCH)) {
        sleep_msec(tries / 2 * 1000);
        hb = recieve_data_timer(HB_PIN, 100);
      } else {
        PRINT(stdout, "Enter hearthbeat: ");
        scanf(" %3hhd", &hb);
        while (getchar() != '\n')
          ;
      }
      char output[10];
      char color[6] = "#0f00";
      if (hb == 255) {
        strcpy(color, "#f000");
      } else if (hb > 240 || hb < 60) {
        strcpy(color, "#ff00");
      }
      sprintf(output, "%s %d", color, hb);
      clear_lines(1, 150, 1 * 24, 0);
      draw_string(output, 150, 1 * 24);
      sprintf(output, "%zu", tries + 1);
      draw_string(output, 220, 1 * 24);
      tries++;
    } while (hb == 255);

    uint8_t hb_stress = get_stress_from_heartbeat(hb);
    uint8_t vl_stress = get_stress_from_crying_volume(volume);
    char output[10];
    clear_lines(3, 170, 3 * 24, 0);
    sprintf(output, "#%x %d", rgb_conv(255, 255 - vl_stress * 2.55, 255 - vl_stress * 2.55), vl_stress);
    draw_string(output, 170, 3 * 24);
    sprintf(output, "#%x %d", rgb_conv(255, 255 - hb_stress * 2.55, 255 - hb_stress * 2.55), hb_stress);
    draw_string(output, 170, 4 * 24);

    if (hb_stress > 50) {
      new_stress = hb_stress;
    } else {
      new_stress = vl_stress;
    }
    sprintf(output, "#%x %d", rgb_conv(255, 255 - new_stress * 2.55, 255 - new_stress * 2.55), new_stress);
    draw_string(output, 170, 5 * 24);
  } while (new_stress + EPS - stress >= EPS && timeout < TIMEOUT * 1000);

  if (new_stress + EPS <= 10) {
    return 1;
  }

  int r;
  r = baby_calming(add_freq_and_amplitude(freq, amplitude - 1), new_stress);
  if (r > 0) return r;
  r = baby_calming(add_freq_and_amplitude(freq - 1, amplitude), new_stress);
  if (r > 0) return r;

  return 0;
}

void open_stats(void) {
  draw_string("#0234Volume:", 0, 0 * 24);
  draw_string("#6969Hearthbeat:", 0, 1 * 24);
  draw_string("======STRESS======", 0, 2 * 24);
  draw_string("Volume stress:", 0, 3 * 24);
  draw_string("Hb stress:", 0, 4 * 24);
  draw_string("Stress:", 0, 5 * 24);
  draw_string("======MOTORS======", 0, 6 * 24);
  draw_string("Freq:", 0, 7 * 24);
  draw_string("Ampl:", 0, 8 * 24 - 1);
}

int main(void) {
  pynq_init();
  switches_init();
  reset_pins();
  set_pin(MT_PIN, 0);
  set_pin(VL_PIN, 1);
  set_pin(HB_PIN, 1);


  if (init_font("fonts/ILGH24XB.FNT")) {
    ERROR("No display found");
    ERROR("Aborting everything");
    return 1;
  }

  open_stats();

  FA fa = add_freq_and_amplitude(4, 4);
  baby_calming(fa, 100);

  reset_pins();
  switches_destroy();
  pynq_destroy();
  return 0;
}
