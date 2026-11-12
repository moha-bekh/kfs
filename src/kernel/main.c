#include "console/console.h"
#include "drivers/vga.h"
#include "kernel/kernel.h"
#include "tests/console.h"
#include "tests/output.h"

#include "drivers/keyboard.h"
#include "io.h"
#include "lib/printk.h"

int kmain(void) {

  console_init();

  uint8_t status = 0;
  while (1) {
    status = inb(PS2_STATUS);
    if (status & 0x01)
      kb_ps2_decode(inb(PS2_DATA));
  }

  // KFS-1 Evaluation

  // t_output_test_results res = test_output_all();
  // (void)res; /* Inspect res in GDB after the suite returns. */
  // console_render_screen();

  // DEV TEST

  // t_console_test_results res = test_console_all();
  // (void)res;
  // test_console_screens_visual();

  return 0;
}
