#include "console/console.h"
#include "drivers/vga.h"

void screen_init(t_screen *screen) {

  screen->view_start = 0;
  screen->line_old = 0;
  screen->line_count = 1;
  screen->cursor.row = 0;
  screen->cursor.col = 0;

  for (uint8_t i = 0; i < CELL_COL; i++) {
    screen->cells[0][i] = (t_cell){' ', VGA_ATTR_GREY_BLACK};
  }
}

void console_init(t_console *console) {

  console->screen_select = 0;

  for (uint8_t i = 0; i < SCREEN_NUMBER; i++) {
    screen_init(&console->screens[i]);
  }
}

void screen_new_line(t_screen *screen) {

  screen->cursor.col = 0;
  if (++screen->cursor.row == CELL_ROW) {
    screen->cursor.row = screen->line_old;
    screen->line_old++;
  }

  for (uint8_t i = 0; i < screen->cursor.col; i++) {
    screen->cells[screen->cursor.row][i] = (t_cell){' ', VGA_ATTR_GREY_BLACK};
  }

  screen->line_count++;

  // if (screen->cursor.row == (screen->view_start + VIEW_CAPACITY))
}

void console_putc(t_console *console, t_cell cell) {

  t_screen *screen = &console->screens[console->screen_select];

  if (cell._char == '\n') {
    screen_new_line(screen);
    return;
  }

  screen->cells[screen->cursor.row][screen->cursor.col] = cell;

  if (++screen->cursor.col == CELL_COL)
    screen_new_line(screen);
}

void console_render_screen(t_console *console) {

  t_screen *screen = &console->screens[console->screen_select];

  for (uint8_t y = 0; y < VIEW_CAPACITY; y++) {
    uint8_t buff_row = (screen->view_start + y) % CELL_ROW;
    for (uint8_t x = 0; x < CELL_COL; x++) {
      vga_putc(screen->cells[buff_row][x], x, y);
    }
  }
}

void console_fill_screen(t_console *console, uint8_t _char) {

  t_screen *screen = &console->screens[console->screen_select];

  for (uint8_t y = 0; y < VIEW_CAPACITY; y++) {
    uint8_t buff_row = (screen->view_start + y) % CELL_ROW;
    for (uint8_t x = 0; x < CELL_COL; x++) {
      screen->cells[buff_row][x] = (t_cell){_char, VGA_ATTR_GREY_BLACK};
    }
  }
}
