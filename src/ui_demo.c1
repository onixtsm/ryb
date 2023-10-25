#include <libpynq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"

int main(void) {
  pynq_init();
  init_font("./fonts/ILGH16XB.FNT");

  draw_string("#f00fHello, #ffffworld\naaaa", 210, 0);
  clear_lines(2, 210, 0, 0);

  pynq_destroy();
  return 0;
}
