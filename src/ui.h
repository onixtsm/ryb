#ifndef UI_H
#define UI_H
#include <stdint.h>

/* init_font accepts string font path(like ./fonts/something)
 * initialises display as well if it was not initialised
 * returns 0 if all good
 */
int init_font(char *path);

/* Prints string on screen
 * Accepts char *string, start x and y position.
 * if string overflows screen width, then it is continued
 * on next row if it is possible. If not then returns not 0 code.
 * To color string add to string # and 4 hex digits [0..f] (ie #e43d, find colors here: https://rgbcolorpicker.com/565).
 * to print litteral # use ##.
 * Also understands \n and \t.
 */
int draw_string(char *string, uint8_t x, uint8_t y);

/* fills given amount of line from given x and y in color.
 * returns non zero if x or y out of bound
 */
int clear_lines(uint8_t line_count, uint8_t line_start_x, uint8_t line_start_y, uint16_t color);

/* fills all display with given color.
 * if dispplay is not initialised does nothing.
 */
void fill_display(uint16_t color);

#endif  // UI_H
