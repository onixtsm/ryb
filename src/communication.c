#include "communication.h"

/* Transmission < recieving */

#include <assert.h>
#include <libpynq.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "helpers.h"
#include "pinmap.h"
#include "switchbox.h"
#include "uart.h"
#include "util.h"

#undef DEBUG

#define BIT_TYPE(a) ((a) ? TRANSMITTING : RECEIVING)

_controller_t _controllers[CONTROLLER_COUNT] = {0};

bool validate_pin(const uint8_t pin) { return pin < PIN_COUNT; }

pin_t _pins[PIN_COUNT] = {
    {-1, false},
};

void get_pinmap(pin_t pins[PIN_COUNT]) {
  memcpy(pins, _pins, sizeof(pin_t) * PIN_COUNT);
}

int8_t find_suitable_controller(const bool type) {
  for (int8_t i = 0; i < CONTROLLER_COUNT; ++i) {
#ifdef DEBUG
    LOG(d, _controllers[i]);
#endif
    if (type) {
      if ((_controllers[i] & RECEIVING) == 0) {
        return i;
      }
    } else {
      if ((_controllers[i] & TRANSMITTING) == 0) {
        return i;
      }
    }
  }
  return -1;
}

int transmit_data(const uint8_t pin, const uint8_t data) {
  if (!validate_pin(pin)) {
    return 1;
  }
  int8_t controller = _pins[pin].controller;
  if (controller == -1) {
    return 4;
  }
  if (!uart_has_space(controller)) {
    return 5;
  }
#ifdef DEBUG
  LOG(c, data);
  LOG(d, controller);
  LOG(d, pin);
#endif

  uart_send(controller, data);
  // sleep_msec(150);
  return 0;
}

uint8_t recieve_data_timer(const uint8_t pin, const unsigned int a) {
  if (!validate_pin(pin)) {
    return 255;
  }
  int8_t controller = _pins[pin].controller;
  if (controller == -1) {
    return 255;
  }
#ifdef DEBUG
  LOG(d, controller);
  LOG(d, pin);
#endif

  for(size_t i = 0; i < 5; i++) {
     if (uart_has_data(controller)) {
      return uart_recv(controller);
    }
    sleep_msec(a/5);
  }
  return 255;
}


uint8_t recieve_data(const uint8_t pin) {
  if (!validate_pin(pin)) {
    return 1;
  }
  int8_t controller = _pins[pin].controller;
  if (controller == -1) {
    return 4;
  }
#ifdef DEBUG
  LOG(d, controller);
  LOG(d, pin);
#endif
  for (;;) {
#ifdef DEBUG
    LOG(d, uart_has_data(controller));
#endif
    if (uart_has_data(controller)) {
      return uart_recv(controller);
    }
    sleep_msec(150);
  }
  return 255;
}

int reset_pins(void) {
  for (uint8_t i = 0; i < PIN_COUNT; ++i) {
    _pins[i].controller = -1;
    _pins[i].type = false;
  }
  for (uint8_t i = 0; i < CONTROLLER_COUNT; ++i) {
    _controllers[i] = 0;
  }
  return 0;
}

int set_pin(const uint8_t pin, const bool type) {
  if (!validate_pin(pin)) {
    return 1;
  }

  if (_pins[pin].controller != -1) {
    return 3;
  }

  int8_t controller = find_suitable_controller(type);
  if (controller < 0) {
    return 2;
  }

  if (type) {
    _controllers[controller] |= RECEIVING;
  } else {
    _controllers[controller] |= TRANSMITTING;
  }

  if (controller) {                                       // In case of UART 1 use correct offset
    switchbox_set_pin(pin + IO_AR0, type + SWB_UART1_TX);  // Transmiter is 0 and reciever ir 1;
  } else {
    switchbox_set_pin(pin + IO_AR0, type + SWB_UART0_TX);
  }
#ifdef DEBUG
  LOG(d, type + SWB_UART0_TX);
  LOG(d, pin + IO_A0);
#endif

  _pins[pin].type = type;
  _pins[pin].controller = controller;
#ifdef DEBUG
  LOG(d, type);
  LOG(d, controller);
#endif
  uart_init(controller);
  uart_reset_fifos(controller);
  return 0;
}

int free_pin(const uint8_t pin) {
  if (!validate_pin(pin)) {
    return 1;
  }
  _controllers[_pins[pin].controller] ^= BIT_TYPE(_pins[pin].type);
  _pins[pin].controller = -1;

  return 0;
}
