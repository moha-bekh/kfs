#ifndef CONSOLE_CONSOLE_H
#define CONSOLE_CONSOLE_H

#include "drivers/vga.h"
#include "types.h"

typedef struct s_cursor {

  uint8_t row;
  uint8_t col;

} t_cursor;

#define CELL_ROW (25 * 10)
#define CELL_COL 80
#define VIEW_CAPACITY 25

typedef struct s_screen {

  t_vga cells[CELL_ROW][CELL_COL];

  uint8_t view_start;
  uint8_t line_old;
  uint8_t line_count;

  t_cursor cursor;
  uint8_t color;

} t_screen;

void screen_init(t_screen *);
void screen_new_line(t_screen *);
void screen_scroll_up(t_screen *);
void screen_scroll_down(t_screen *);

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25
#define SCREEN_NUMBER 5

typedef struct s_console {

  t_screen screens[SCREEN_NUMBER];
  uint8_t screen_select;

} t_console;

void console_init(void);
void console_select_screen(uint8_t index);
void console_render_screen(void);

void console_set_color(uint8_t fg, uint8_t bg);
void console_reset_color(void);

void console_putc(char c);
void console_putstr(const char *str);

#endif /* CONSOLE_CONSOLE_H */
