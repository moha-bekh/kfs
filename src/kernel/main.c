#include "console/console.h"
#include "drivers/vga.h"
#include "kernel/kernel.h"
#include "tests/console.h"
#include "tests/output.h"
#include "tests/keyboard.h"

#include "drivers/keyboard.h"
#include "io.h"
#include "lib/printk.h"

int kmain(void) {

  t_console_test_results console_results = test_console_all();
  t_output_test_results output_results = test_output_all();
  t_keyboard_test_results keyboard_results = test_keyboard_all();
  /* Break on the console_init line below and inspect the three result structures. */
  (void)console_results;
  (void)output_results;
  (void)keyboard_results;
  console_init();
  kb_reset();

  uint8_t status = 0;
  while (1) {
    status = inb(PS2_STATUS);
    if (status & 0x01)
      kb_ps2_decode(inb(PS2_DATA));
  }

  return 0;
}
