#include "console/console.h"
#include "drivers/vga.h"

static t_console console;

void screen_init(t_screen *screen) {

  screen->view_start = 0;
  screen->line_old = 0;
  screen->line_count = 1;
  screen->cursor.row = 0;
  screen->cursor.col = 0;
  screen->color = VGA_ATTR_GREY_BLACK;

  for (uint16_t i = 0; i < CELL_ROW; i++) {
    for (uint16_t j = 0; j < CELL_COL; j++) {
      screen->cells[i][j] = (t_vga){' ', screen->color};
    }
  }
}

void screen_new_line(t_screen *screen) {

  screen->cursor.col = 0;

  uint16_t visible_lines =
      (screen->cursor.row + CELL_ROW - screen->view_start) % CELL_ROW;
  if (visible_lines == VIEW_CAPACITY - 1) {
    if (++screen->view_start == CELL_ROW)
      screen->view_start = 0;
  }

  if (++screen->cursor.row == CELL_ROW)
    screen->cursor.row = 0;
  for (uint16_t i = 0; i < CELL_COL; i++) {
    screen->cells[screen->cursor.row][i] = (t_vga){' ', VGA_ATTR_GREY_BLACK};
  }

  if (screen->line_count < CELL_ROW)
    screen->line_count++;
  else {
    if (screen->cursor.row == screen->line_old &&
        screen->cursor.row == screen->view_start) {
      if (++screen->view_start == CELL_ROW)
        screen->view_start = 0;
    }
    if (++screen->line_old == CELL_ROW)
      screen->line_old = 0;
  }
}

void screen_scroll_up(t_screen *screen) {

  if (screen->view_start == screen->line_old)
    return;

  if (screen->view_start == 0)
    screen->view_start = CELL_ROW - 1;
  else
    screen->view_start--;
}

void screen_scroll_down(t_screen *screen) {

  if ((screen->cursor.row + CELL_ROW - screen->view_start) % CELL_ROW <=
      VIEW_CAPACITY - 1)
    return;

  if (screen->view_start == CELL_ROW - 1)
    screen->view_start = 0;
  else
    screen->view_start++;
}

void console_init(void) {

  console.screen_select = 0;

  for (uint16_t i = 0; i < SCREEN_NUMBER; i++) {
    screen_init(&console.screens[i]);
  }
}

void console_select_screen(uint8_t n) {

  if (n > SCREEN_NUMBER - 1)
    return;

  console.screen_select = n;
}

void console_render_screen(void) {

  t_screen *screen = &console.screens[console.screen_select];

  for (uint16_t y = 0; y < VIEW_CAPACITY; y++) {
    uint8_t buff_row = (screen->view_start + y) % CELL_ROW;
    for (uint16_t x = 0; x < CELL_COL; x++) {
      vga_putc(screen->cells[buff_row][x], x, y);
    }
  }

  uint16_t visible_lines =
      (screen->cursor.row + CELL_ROW - screen->view_start) % CELL_ROW;
  if (visible_lines < VIEW_CAPACITY) {
    vga_move_cursor(screen->cursor.col, visible_lines);
    vga_show_cursor();
  } else
    vga_hide_cursor();
}

void console_set_color(uint8_t fg, uint8_t bg) {

  t_screen *screen = &console.screens[console.screen_select];
  screen->color = (bg << 4) | (fg & 0x0F);
}

void console_reset_color(void) {

  t_screen *screen = &console.screens[console.screen_select];

  screen->color = VGA_ATTR_GREY_BLACK;
}

void console_putc(char c) {

  t_screen *screen = &console.screens[console.screen_select];

  t_vga cell = {._char = c, .attr = screen->color};

  if (cell._char == '\n') {
    screen_new_line(screen);
    return;
  }

  screen->cells[screen->cursor.row][screen->cursor.col] = cell;

  if (++screen->cursor.col == CELL_COL)
    screen_new_line(screen);
}

void console_putstr(const char *str) {

  uint32_t i = 0;
  while (str[i])
    console_putc(str[i++]);
}
