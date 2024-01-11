#include "src/hid.h"

#include <stdlib.h>

#include "buttons.h"

button_t buttons[NUM_BUTTONS];
bool buttons_initialized = false;

void run_buttons(void) {
  if (!buttons_initialized) {
    return;
  }
  for (size_t i = 0; i < NUM_BUTTONS; ++i) {
    if (get_button_state(i)) {
      if (buttons[i].callback != NULL) {
        buttons[i].callback(&buttons[i]);
        buttons[i].prev_state = 1;
      }
    } else {
      buttons[i].prev_state = 0;
    }
  }
}

void setup_button(uint8_t button, void *(*func_p)(button_t *), void *params) {
  if (!buttons_initialized) {
    buttons_init();
    buttons_initialized = true;
  }
  if (button >= NUM_BUTTONS) {
    return;
  }
  buttons[button].params = params;
  buttons[button].callback = func_p;
  buttons[button].prev_state = 0;
}


void *get_object_params(uint8_t button) {
  if (button >= NUM_BUTTONS) {
    return NULL;
  }
  return buttons[button].params;
}
