#include "console/console.h"
#include "drivers/vga.h"
#include "kernel/kernel.h"

int kmain(void) {

  static t_console console;
  console_init(&console);
  console_putc(&console, (t_cell){'4', VGA_ATTR_GREY_BLACK});
  console_putc(&console, (t_cell){'2', VGA_ATTR_GREY_BLACK});
  console_putc(&console, (t_cell){'\n', VGA_ATTR_GREY_BLACK});
  console_putc(&console, (t_cell){'4', VGA_ATTR_GREY_BLACK});
  console_putc(&console, (t_cell){'2', VGA_ATTR_GREY_BLACK});

  // console_fill_screen(&console, 'X');
  console_render_screen(&console);

  // vga_putc((t_cell){'4', 0x07}, 0, 0);
  // vga_putc((t_cell){'2', 0x07}, 1, 0);

  return 0;
}
