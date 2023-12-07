#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdbool.h>
#include <stdint.h>

#define PIN_COUNT 14
#define CONTROLLER_COUNT 2

typedef struct {
  int8_t controller;  // Empty pin has -1 as controller
  bool type;
} pin_t;

#define TRANSMITTING 0b01
#define RECEIVING 0b10

typedef uint8_t _controller_t;

/* accepts pin_t array of pins size of PIN_COUNT
 * and copys pinmap to that array
 */
void get_pinmap(pin_t pins[PIN_COUNT]);

/* Recieves pin number [0..13] and data (uint8_t).
 * returns error on wrong pin or unset controller
 */
int transmit_data(const uint8_t pin, const uint8_t data);

/* Recieves pin [0..13], runs loop while the data has not been captured.
 * TODO: make errors understandable
 */
uint8_t recieve_data(const uint8_t pin);

/* Makes tries to get data form UART 5 times in given time interval a
 * returns 255 if did not get anything
 */
uint8_t recieve_data_timer(const uint8_t pin, const unsigned int a);

/* Recieves pin number [0..13] and type [0..1]
 * 0 - transmitting
 * 1 - receiving
 * returns errors on wrong pin or if all UART controllers are used
 */
int set_pin(const uint8_t pin, const bool type);

/* Recieves pin number [0..13].
 * sets pin as unused.
 * returns error if pin is wrong
 */
int free_pin(const uint8_t pin);

/* Recieves nothing.
 *(re)sets all pins as unused.
 *must be called before using any pin related function
 */
int reset_pins(void);

#endif  // COMMUNICATION_H
