#include "ui.h"

#include <libpynq.h>
#include <stdint.h>

#include "display.h"
#include "helpers.h"

display_t display;
bool display_initialised = false;
uint8_t fontHeight_fx16G, fontWidth_fx16G;

FontxFile fx16G[2];

void fill_display(uint16_t color) {
  if (!display_initialised) {
    return;
  }
  displayFillScreen(&display, color);
}

int clear_lines(uint8_t line_count, uint8_t line_start_x, uint8_t line_start_y, uint16_t color) {
  if (line_start_x >= DISPLAY_WIDTH || line_start_y + (line_count * fontHeight_fx16G) >= DISPLAY_HEIGHT) {
    return 14;
  }
  displayDrawFillRect(&display, line_start_x, line_start_y, DISPLAY_WIDTH - 1, line_start_y + (line_count * fontHeight_fx16G) , color);
  return 0;
}

int init_font(char *path) {
  if (!display_initialised) {
    display_init(&display);
    display_initialised = true;
  }
  displayFillScreen(&display, 0);
  uint8_t buffer_fx18G[FontxGlyphBufSize];
  InitFontx(fx16G, path, "");
  if (!GetFontx(fx16G, 0, buffer_fx18G, &fontWidth_fx16G, &fontHeight_fx16G)) {
    return 10;
  }
  return 0;
}

uint16_t get_color_hex(char *string) { return strtol(string, &string + sizeof(char) * 4, 16); }

int draw_string(char *string, uint8_t x, uint8_t y) {
  if (!fx16G[0].valid) {
    return 12;
  }
  if (x >= DISPLAY_WIDTH - fontWidth_fx16G || y >= DISPLAY_HEIGHT - fontHeight_fx16G) {
    return 13;
  }
  if (!display_initialised) {
    ERROR("Display not initialised");
    return 11;
  }

  uint8_t pos = x;
  uint16_t color = RGB_GREEN;

  while (*string != 0) {
    if (pos + fontWidth_fx16G >= DISPLAY_WIDTH) {
      if (y + fontHeight_fx16G >= DISPLAY_HEIGHT) {
        return 14;
      } else {
        y += fontHeight_fx16G;
        pos = x;
      }
    }
    switch (*string) {
      case '#':
        if (*++string == '#') {
          pos = displayDrawChar(&display, fx16G, pos, y + fontHeight_fx16G, '#', color);
        } else {
          color = get_color_hex(string);
          string += 3;
        }
        break;
      case '\t':
        pos += 4;
        break;
      case '\n':
        y += fontHeight_fx16G;
        pos = x;
        break;
      default:
        pos = displayDrawChar(&display, fx16G, pos, y + fontHeight_fx16G, *string, color);
    }
    string++;
  }
  return 0;
}
