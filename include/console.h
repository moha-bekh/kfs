#ifndef CONSOLE_H
#define CONSOLE_H

#include "types.h"

typedef struct s_cursor {
  uint8_t row;
  uint8_t col;
} t_cursor;

typedef struct s_cell {
  uint8_t _char;
  uint8_t attr;
} t_cell;

#define CELL_ROW (25 * 10)
#define CELL_COL 80
#define VIEW_CAPACITY 25
typedef struct s_screen {
  t_cell cells[CELL_ROW][CELL_COL];
  uint8_t view_start;
  uint8_t line_old;
  uint8_t line_count;
  t_cursor cursor;
} t_screen;

void screen_init(t_screen *);
void screen_new_line(t_screen *);

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25
#define SCREEN_NUMBER 5
typedef struct s_console {
  t_screen screens[SCREEN_NUMBER];
  uint8_t screen_select;
} t_console;

void console_init(t_console *);
void console_putc(t_console *, t_cell);
void console_render_screen(t_console *);

void console_select_screen(t_console *, t_screen *);
void console_fill_screen(t_console *, uint8_t _char);

// end of column
// buffer full
// historique view
// user input

#endif /* CONSOLE_H */
