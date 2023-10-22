#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdbool.h>
#include <stdint.h>

#define PIN_COUNT 6
#define CONTROLLR_COUNT 2

typedef struct {
  int8_t controller;  // Empty pin has -1 as controller
  bool type;
} _pin_t;

#define TRANSMITTING 0b01
#define RECEIVING 0b10

typedef uint8_t _controller_t;

int transmit_data(const uint8_t pin, const uint8_t data);

uint8_t recieve_data(const uint8_t pin);

int set_pin(const uint8_t pin, const bool type);

int free_pin(const uint8_t pin);

int reset_pins(void);

#endif //COMMUNICATION_H
