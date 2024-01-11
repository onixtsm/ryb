#ifndef HID_H
#define HID_H

#include <stdint.h>

typedef struct button_t_ {
  void *(*callback)(struct button_t_ *self);
  void *params;
  uint8_t prev_state;
} button_t;

void setup_button(uint8_t button, void *(*func_p)(button_t *), void *params);

void run_buttons(void);

void *get_object_params(uint8_t button);

#endif
