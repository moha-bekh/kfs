#include "tests/console.h"

/* History tests exercise screen_* directly; the live console stays private. */
static t_screen history;
static volatile const t_vga *const video = (volatile const t_vga *)0xB8000;

static bool_t state_is(uint8_t row, uint8_t col, uint8_t count,
                       uint8_t oldest, uint8_t view) {
  return history.cursor.row == row && history.cursor.col == col &&
         history.line_count == count && history.line_old == oldest &&
         history.view_start == view;
}

/* Prepare numbered rows; newline/scroll behavior uses the production screen API.
 * console_putc integration is checked separately by the public output tests.
 */
static void write_numbered_line(uint16_t number) {
  t_vga *line = history.cells[history.cursor.row];
  line[0] = (t_vga){'0' + number / 100, VGA_ATTR_GREY_BLACK};
  line[1] = (t_vga){'0' + number / 10 % 10, VGA_ATTR_GREY_BLACK};
  line[2] = (t_vga){'0' + number % 10, VGA_ATTR_GREY_BLACK};
  history.cursor.col = 3;
  screen_new_line(&history);
}

static void write_lines(uint16_t count) {
  screen_init(&history);
  for (uint16_t i = 0; i < count; i++)
    write_numbered_line(i);
}

bool_t console_test_case(bool_t (*test)(void), const char *name) {
  (void)name;
  return test();
}

t_console_test_results test_console_all(void) {
  t_console_test_results results;
  results.text_42 = console_test_case(test_console_42, "text_42");
  results.wrap = console_test_case(test_console_wrap, "wrap");
  results.newlines_24 = console_test_case(test_console_24_newlines, "newlines_24");
  results.newlines_25 = console_test_case(test_console_25_newlines, "newlines_25");
  results.newlines_250 = console_test_case(test_console_250_newlines, "newlines_250");
  results.newlines_500 = console_test_case(test_console_500_newlines, "newlines_500");
  results.history_overwrite = console_test_case(test_console_history_overwrite, "history_overwrite");
  results.history_wrap = console_test_case(test_console_history_wrap, "history_wrap");
  results.scroll_up = console_test_case(test_console_scroll_up, "scroll_up");
  results.scroll_down = console_test_case(test_console_scroll_down, "scroll_down");
  results.history_output = console_test_case(test_console_history_output, "history_output");
  results.history_output_wrap = console_test_case(test_console_history_output_wrap, "history_output_wrap");
  results.history_recycling = console_test_case(test_console_history_recycling, "history_recycling");
  results.resume_follow = console_test_case(test_console_resume_follow, "resume_follow");
  results.init_blank = console_test_case(test_console_init_blank, "init_blank");
  results.screen_independence = console_test_case(test_console_screen_independence, "screen_independence");
  results.screen_invalid = console_test_case(test_console_screen_invalid, "screen_invalid");
  results.all_passed =
      results.text_42 &&
      results.wrap &&
      results.newlines_24 &&
      results.newlines_25 &&
      results.newlines_250 &&
      results.newlines_500 &&
      results.history_overwrite &&
      results.history_wrap &&
      results.scroll_up &&
      results.scroll_down &&
      results.history_output &&
      results.history_output_wrap &&
      results.history_recycling &&
      results.resume_follow &&
      results.init_blank &&
      results.screen_independence &&
      results.screen_invalid;
  return results;
}

bool_t test_console_24_newlines(void) {
  write_lines(24);
  return state_is(24, 0, 25, 0, 0);
}

bool_t test_console_25_newlines(void) {
  write_lines(25);
  return state_is(25, 0, 26, 0, 1);
}

bool_t test_console_250_newlines(void) {
  write_lines(250);
  if (!state_is(0, 0, 250, 1, 226))
    return FALSE;
  /* Row 0 contained 000: all 80 cells must have been cleared. */
  for (uint8_t i = 0; i < 80; i++) {
    if (history.cells[0][i]._char != ' ' ||
        history.cells[0][i].attr != VGA_ATTR_GREY_BLACK)
      return FALSE;
  }
  return TRUE;
}

bool_t test_console_500_newlines(void) {
  write_lines(500);
  if (!state_is(0, 0, 250, 1, 226))
    return FALSE;
  /* Check the entire retained history: lines 251 through 499. */
  for (uint16_t number = 251; number < 500; number++) {
    t_vga *line = history.cells[number % 250];
    if (line[0]._char != '0' + number / 100 ||
        line[1]._char != '0' + number / 10 % 10 ||
        line[2]._char != '0' + number % 10)
      return FALSE;
    for (uint8_t col = 3; col < 80; col++) {
      if (line[col]._char != ' ')
        return FALSE;
    }
  }
  return TRUE;
}

bool_t test_console_history_overwrite(void) {
  write_lines(CELL_ROW);
  if (!state_is(0, 0, CELL_ROW, 1, 226))
    return FALSE;

  t_screen *screen = &history;
  /* Simulate scrolling back to the oldest retained line. */
  screen->view_start = screen->line_old;

  /* Recycling row 1 must move the view to the new oldest row, 2. */
  screen_new_line(&history);
  return state_is(1, 0, CELL_ROW, 2, 2);
}

bool_t test_console_history_wrap(void) {
  /* After 498 newlines, row 249 is the oldest retained row. */
  write_lines(2 * CELL_ROW - 2);
  if (!state_is(248, 0, CELL_ROW, 249, 224))
    return FALSE;

  t_screen *screen = &history;
  screen->view_start = screen->line_old;

  /* Recycling row 249 must wrap both the oldest row and the view to 0. */
  screen_new_line(&history);
  return state_is(249, 0, CELL_ROW, 0, 0);
}

bool_t test_console_scroll_up(void) {
  t_screen *screen = &history;
  screen_init(&history);
  screen_scroll_up(screen);
  if (!state_is(0, 0, 1, 0, 0))
    return FALSE;

  write_lines(30);
  /* Walk back to the oldest row, checking each step and the upper limit. */
  for (uint8_t view = 6; view > 0; view--) {
    screen_scroll_up(screen);
    if (!state_is(30, 0, 31, 0, view - 1))
      return FALSE;
  }
  screen_scroll_up(screen);
  if (!state_is(30, 0, 31, 0, 0))
    return FALSE;

  write_lines(498);
  /* Row 0 can scroll back to 249, which is the oldest retained row. */
  screen->view_start = 0;
  screen_scroll_up(screen);
  if (!state_is(248, 0, CELL_ROW, 249, 249))
    return FALSE;
  screen_scroll_up(screen);
  return state_is(248, 0, CELL_ROW, 249, 249);
}

bool_t test_console_scroll_down(void) {
  t_screen *screen = &history;
  /* Empty, partially filled and exactly full views cannot scroll down. */
  const uint8_t counts[] = {0, 10, 24};
  for (uint8_t i = 0; i < 3; i++) {
    write_lines(counts[i]);
    screen_scroll_down(screen);
    if (!state_is(counts[i], 0, counts[i] + 1, 0, 0))
      return FALSE;
  }

  write_lines(30);
  screen->view_start = 0;
  for (uint8_t view = 1; view <= 6; view++) {
    screen_scroll_down(screen);
    if (!state_is(30, 0, 31, 0, view))
      return FALSE;
  }
  screen_scroll_down(screen);
  if (!state_is(30, 0, 31, 0, 6))
    return FALSE;

  write_lines(498);
  screen->view_start = 249;
  /* Cross 249 -> 0, then continue until the cursor is at the bottom. */
  for (uint16_t view = 0; view <= 224; view++) {
    screen_scroll_down(screen);
    if (!state_is(248, 0, CELL_ROW, 249, view))
      return FALSE;
  }
  screen_scroll_down(screen);
  return state_is(248, 0, CELL_ROW, 249, 224);
}

bool_t test_console_history_output(void) {
  write_lines(100);
  t_screen *screen = &history;
  for (uint8_t i = 0; i < 10; i++)
    screen_scroll_up(screen);
  if (!state_is(100, 0, 101, 0, 66))
    return FALSE;

  /* New output must preserve both the view and its numbered lines. */
  for (uint16_t number = 100; number < 110; number++)
    write_numbered_line(number);
  if (!state_is(110, 0, 111, 0, 66))
    return FALSE;
  for (uint16_t number = 66; number <= 90; number++) {
    t_vga *line = screen->cells[number];
    if (line[0]._char != '0' || line[1]._char != '0' + number / 10 ||
        line[2]._char != '0' + number % 10)
      return FALSE;
  }
  return TRUE;
}

bool_t test_console_history_output_wrap(void) {
  write_lines(499);
  t_screen *screen = &history;
  screen->view_start = 100;

  /* Character wrapping must recycle rows without moving a preserved view. */
  for (uint8_t col = 0; col < CELL_COL; col++) {
    history.cells[history.cursor.row][history.cursor.col++] = (t_vga){'X', VGA_ATTR_GREY_BLACK};
    if (history.cursor.col == CELL_COL)
      screen_new_line(&history);
  }
  if (!state_is(0, 0, CELL_ROW, 1, 100))
    return FALSE;
  for (uint8_t col = 0; col < CELL_COL; col++) {
    if (screen->cells[249][col]._char != 'X' ||
        screen->cells[0][col]._char != ' ' ||
        screen->cells[0][col].attr != VGA_ATTR_GREY_BLACK)
      return FALSE;
  }
  /* Explicit newlines must preserve the same view as well. */
  screen_new_line(&history);
  return state_is(1, 0, CELL_ROW, 2, 100);
}

bool_t test_console_history_recycling(void) {
  write_lines(498);
  t_screen *screen = &history;
  screen->view_start = screen->line_old;

  /* Keep reading the oldest history through more than one complete turn. */
  for (uint16_t step = 1; step <= 260; step++) {
    screen_new_line(&history);
    uint8_t oldest = (249 + step) % CELL_ROW;
    if (!state_is((248 + step) % CELL_ROW, 0, CELL_ROW, oldest,
                  oldest))
      return FALSE;
  }
  return TRUE;
}

bool_t test_console_resume_follow(void) {
  write_lines(30);
  t_screen *screen = &history;
  screen_scroll_up(screen);
  screen_scroll_up(screen);
  screen_new_line(&history);
  if (!state_is(31, 0, 32, 0, 4))
    return FALSE;
  for (uint8_t i = 0; i < 3; i++)
    screen_scroll_down(screen);
  if (!state_is(31, 0, 32, 0, 7))
    return FALSE;
  screen_new_line(&history);
  if (!state_is(32, 0, 33, 0, 8))
    return FALSE;

  /* Repeat with a full buffer and an output cursor crossing 249 -> 0. */
  write_lines(498);
  screen_scroll_up(screen);
  screen_new_line(&history);
  if (!state_is(249, 0, CELL_ROW, 0, 223))
    return FALSE;
  screen_scroll_down(screen);
  screen_scroll_down(screen);
  if (!state_is(249, 0, CELL_ROW, 0, 225))
    return FALSE;
  screen_new_line(&history);
  return state_is(0, 0, CELL_ROW, 1, 226);
}

bool_t test_console_42(void) {
  console_init();
  console_putstr("42\n42");
  console_render_screen();
  return video[0]._char == '4' && video[1]._char == '2' &&
         video[CELL_COL]._char == '4' && video[CELL_COL + 1]._char == '2' &&
         video[CELL_COL + 2]._char == ' ';
}

bool_t test_console_wrap(void) {
  console_init();
  for (uint8_t i = 0; i < CELL_COL; i++)
    console_putc('X');
  console_putc('Y');
  console_render_screen();
  for (uint8_t i = 0; i < CELL_COL; i++)
    if (video[i]._char != 'X')
      return FALSE;
  return video[CELL_COL]._char == 'Y' && video[CELL_COL + 1]._char == ' ';
}

bool_t test_console_init_blank(void) {
  /* Dirty the live screens through public calls, then reset all of them. */
  console_init();
  for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
    console_select_screen(n);
    console_set_color(4, 1);
    for (uint16_t i = 0; i < VIEW_CAPACITY * CELL_COL; i++)
      console_putc('X');
  }
  console_init();
  console_putc('0');
  console_render_screen();
  if (video[0]._char != '0' || video[0].attr != VGA_ATTR_GREY_BLACK)
    return FALSE;
  console_init();
  for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
    console_select_screen(n);
    console_render_screen();
    for (uint16_t i = 0; i < VIEW_CAPACITY * CELL_COL; i++)
      if (video[i]._char != ' ' || video[i].attr != VGA_ATTR_GREY_BLACK)
        return FALSE;
  }
  /* Check all history cells and metadata independently on a dirty fixture. */
  for (uint16_t row = 0; row < CELL_ROW; row++)
    for (uint8_t col = 0; col < CELL_COL; col++)
      history.cells[row][col] = (t_vga){'X', 0x4F};
  history.cursor = (t_cursor){12, 34};
  history.view_start = 123;
  history.line_old = 42;
  history.line_count = CELL_ROW;
  history.color = 0x4F;
  screen_init(&history);
  if (!state_is(0, 0, 1, 0, 0) || history.color != VGA_ATTR_GREY_BLACK)
    return FALSE;
  for (uint16_t row = 0; row < CELL_ROW; row++)
    for (uint8_t col = 0; col < CELL_COL; col++)
      if (history.cells[row][col]._char != ' ' ||
          history.cells[row][col].attr != VGA_ATTR_GREY_BLACK)
        return FALSE;
  return TRUE;
}

bool_t test_console_screen_independence(void) {
  console_init();
  for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
    console_select_screen(n);
    console_set_color(n + 1, 0);
    for (uint16_t row = 0; row < 30 + n; row++) {
      console_putc('A' + n);
      console_putc('\n');
    }
    console_putc('a' + n);
  }
  for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
    console_select_screen(n);
    console_putc('0' + n);
    console_render_screen();
    for (uint8_t row = 0; row < VIEW_CAPACITY - 1; row++)
      if (video[row * CELL_COL]._char != 'A' + n ||
          video[row * CELL_COL].attr != n + 1)
        return FALSE;
    uint16_t bottom = (VIEW_CAPACITY - 1) * CELL_COL;
    if (video[bottom]._char != 'a' + n ||
        video[bottom + 1]._char != '0' + n ||
        video[bottom + 1].attr != n + 1 || video[bottom + 2]._char != ' ')
      return FALSE;
  }
  return TRUE;
}

bool_t test_console_screen_invalid(void) {
  console_init();
  for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
    console_select_screen(n);
    console_putc('A' + n);
  }
  for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
    console_select_screen(n);
    for (uint16_t invalid = SCREEN_NUMBER; invalid <= 255; invalid++) {
      console_select_screen(invalid);
      console_render_screen();
      if (video[0]._char != 'A' + n)
        return FALSE;
    }
  }
  return TRUE;
}

/* Set a GDB breakpoint here to observe each completed render. */
static void console_visual_checkpoint(const char *stage) {
  (void)stage;
}

void test_console_screens_visual(void) {
  console_init();
  console_putstr("Screen 0: preserved text");
  console_render_screen();
  console_visual_checkpoint("screen 0: text and visible cursor");

  console_select_screen(1);
  console_render_screen();
  console_visual_checkpoint("screen 1: blank, cursor at top left");
  console_set_color(0xA, 0);
  console_putc('B');
  console_render_screen();
  console_visual_checkpoint("screen 1: green B, cursor after B");

  console_select_screen(0);
  console_render_screen();
  console_visual_checkpoint("screen 0: preserved text and cursor");

  console_select_screen(SCREEN_NUMBER - 1);
  console_set_color(0xC, 0);
  console_putc('E');
  console_render_screen();
  console_visual_checkpoint("screen 4: red E, cursor after E");

  console_select_screen(1);
  console_render_screen();
  console_visual_checkpoint("screen 1 again: preserved green B and cursor");
}
