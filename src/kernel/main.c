#include "console/console.h"
#include "drivers/vga.h"
#include "kernel/kernel.h"
#include "tests/console.h"
#include "tests/output.h"

int kmain(void) {

  // KFS-1 Evaluation

  t_output_test_results res = test_output_all();
  (void)res; /* Inspect res in GDB after the suite returns. */
  console_render_screen();

  // DEV TEST

  // t_console_test_results res = test_console_all(&console);
  // (void)res;
  // test_console_screens_visual(&console);

  return 0;
}
