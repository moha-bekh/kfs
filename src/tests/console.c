#include "tests/console.h"
//
// t_console_test_results test_console_all(t_console *console) {
//   t_console_test_results results;
//
//   results.text_42 = test_console_42(console);
//   results.wrap = test_console_wrap(console);
//   results.newlines_24 = test_console_24_newlines(console);
//   results.newlines_25 = test_console_25_newlines(console);
//   results.newlines_250 = test_console_250_newlines(console);
//   results.newlines_500 = test_console_500_newlines(console);
//   results.history_overwrite = test_console_history_overwrite(console);
//   results.history_wrap = test_console_history_wrap(console);
//   results.scroll_up = test_console_scroll_up(console);
//   results.scroll_down = test_console_scroll_down(console);
//   results.history_output = test_console_history_output(console);
//   results.history_output_wrap = test_console_history_output_wrap(console);
//   results.history_recycling = test_console_history_recycling(console);
//   results.resume_follow = test_console_resume_follow(console);
//   results.init_blank = test_console_init_blank(console);
//   results.screen_independence = test_console_screen_independence(console);
//   results.screen_invalid = test_console_screen_invalid(console);
//   results.all_passed =
//       results.text_42 && results.wrap && results.newlines_24 &&
//       results.newlines_25 && results.newlines_250 && results.newlines_500 &&
//       results.history_overwrite && results.history_wrap && results.scroll_up
//       && results.scroll_down && results.history_output &&
//       results.history_output_wrap && results.history_recycling &&
//       results.resume_follow && results.init_blank &&
//       results.screen_independence && results.screen_invalid;
//   return results;
// }
//
// /* Numbered lines without depending on printk: 000, 001, ... */
// static void write_numbered_line(t_console *console, uint16_t number) {
//   console_putc((t_vga){'0' + number / 100, VGA_ATTR_GREY_BLACK});
//   console_putc((t_vga){'0' + number / 10 % 10, VGA_ATTR_GREY_BLACK});
//   console_putc((t_vga){'0' + number % 10, VGA_ATTR_GREY_BLACK});
//   console_putc((t_vga){'\n', VGA_ATTR_GREY_BLACK});
// }
//
// static void write_lines(uint16_t count) {
//   console_init();
//   for (uint16_t i = 0; i < count; i++)
//     write_numbered_line(i);
// }
//
// /* This function is a GDB checkpoint, not an automatic pause. */
// static void console_visual_checkpoint(const char *stage, t_console *console)
// {
//   (void)stage;
//   (void)console;
// }
//
// void test_console_visual(t_console *console) {
//   write_lines(console, 30);
//   t_screen *screen = &console->screens[0];
//   console_putc(console, (t_vga){'>', VGA_ATTR_GREY_BLACK});
//
//   /* View 6: cursor visible at column 1, row 24 after the marker. */
//   console_render_screen(console);
//   console_visual_checkpoint("bottom: visible cursor", console);
//
//   for (uint8_t i = 0; i < 5; i++)
//     screen_scroll_up(screen);
//   /* View 1: lines 001..025, with the write cursor off screen. */
//   console_render_screen(console);
//   console_visual_checkpoint("history: hidden cursor", console);
//
//   console_putc(console, (t_vga){'\n', VGA_ATTR_GREY_BLACK});
//   console_putc(console, (t_vga){'>', VGA_ATTR_GREY_BLACK});
//   /* New output must leave the displayed history unchanged. */
//   console_render_screen(console);
//   console_visual_checkpoint("history after output: unchanged view", console);
//
//   for (uint8_t i = 0; i < 6; i++)
//     screen_scroll_down(screen);
//   /* View 7: cursor visible again at column 1, row 24. */
//   console_render_screen(console);
//   console_visual_checkpoint("bottom again: visible cursor", console);
// }
//
// static bool_t state_is(t_console *console, uint8_t row, uint8_t col,
//                        uint8_t count, uint8_t oldest, uint8_t view) {
//   t_screen *screen = &console->screens[0];
//   return console->screen_select == 0 && screen->cursor.row == row &&
//          screen->cursor.col == col && screen->line_count == count &&
//          screen->line_old == oldest && screen->view_start == view;
// }
//
// bool_t test_console_42(t_console *console) {
//   console_init(console);
//   const char *text = "42\n42";
//   for (uint8_t i = 0; text[i]; i++)
//     console_putc(console, (t_vga){text[i], VGA_ATTR_GREY_BLACK});
//
//   t_screen *screen = &console->screens[0];
//   return state_is(console, 1, 2, 2, 0, 0) && screen->cells[0][0]._char == '4'
//   &&
//          screen->cells[0][1]._char == '2' && screen->cells[1][0]._char == '4'
//          && screen->cells[1][1]._char == '2';
// }
//
// bool_t test_console_wrap(t_console *console) {
//   console_init(console);
//   for (uint8_t i = 0; i < 80; i++)
//     console_putc(console, (t_vga){'X', VGA_ATTR_GREY_BLACK});
//
//   for (uint8_t i = 0; i < 80; i++) {
//     if (console->screens[0].cells[0][i]._char != 'X' ||
//         console->screens[0].cells[1][i]._char != ' ' ||
//         console->screens[0].cells[1][i].attr != VGA_ATTR_GREY_BLACK)
//       return FALSE;
//   }
//   return state_is(console, 1, 0, 2, 0, 0);
// }
//
// bool_t test_console_24_newlines(t_console *console) {
//   write_lines(console, 24);
//   return state_is(console, 24, 0, 25, 0, 0);
// }
//
// bool_t test_console_25_newlines(t_console *console) {
//   write_lines(console, 25);
//   return state_is(console, 25, 0, 26, 0, 1);
// }
//
// bool_t test_console_250_newlines(t_console *console) {
//   write_lines(console, 250);
//   if (!state_is(console, 0, 0, 250, 1, 226))
//     return FALSE;
//   /* Row 0 contained 000: all 80 cells must have been cleared. */
//   for (uint8_t i = 0; i < 80; i++) {
//     if (console->screens[0].cells[0][i]._char != ' ' ||
//         console->screens[0].cells[0][i].attr != VGA_ATTR_GREY_BLACK)
//       return FALSE;
//   }
//   return TRUE;
// }
//
// bool_t test_console_500_newlines(t_console *console) {
//   write_lines(console, 500);
//   if (!state_is(console, 0, 0, 250, 1, 226))
//     return FALSE;
//   /* Check the entire retained history: lines 251 through 499. */
//   for (uint16_t number = 251; number < 500; number++) {
//     t_vga *line = console->screens[0].cells[number % 250];
//     if (line[0]._char != '0' + number / 100 ||
//         line[1]._char != '0' + number / 10 % 10 ||
//         line[2]._char != '0' + number % 10)
//       return FALSE;
//     for (uint8_t col = 3; col < 80; col++) {
//       if (line[col]._char != ' ')
//         return FALSE;
//     }
//   }
//   return TRUE;
// }
//
// bool_t test_console_history_overwrite(t_console *console) {
//   write_lines(console, CELL_ROW);
//   if (!state_is(console, 0, 0, CELL_ROW, 1, 226))
//     return FALSE;
//
//   t_screen *screen = &console->screens[0];
//   /* Simulate scrolling back to the oldest retained line. */
//   screen->view_start = screen->line_old;
//
//   /* Recycling row 1 must move the view to the new oldest row, 2. */
//   console_putc(console, (t_vga){'\n', VGA_ATTR_GREY_BLACK});
//   return state_is(console, 1, 0, CELL_ROW, 2, 2);
// }
//
// bool_t test_console_history_wrap(t_console *console) {
//   /* After 498 newlines, row 249 is the oldest retained row. */
//   write_lines(console, 2 * CELL_ROW - 2);
//   if (!state_is(console, 248, 0, CELL_ROW, 249, 224))
//     return FALSE;
//
//   t_screen *screen = &console->screens[0];
//   screen->view_start = screen->line_old;
//
//   /* Recycling row 249 must wrap both the oldest row and the view to 0. */
//   console_putc(console, (t_vga){'\n', VGA_ATTR_GREY_BLACK});
//   return state_is(console, 249, 0, CELL_ROW, 0, 0);
// }
//
// bool_t test_console_scroll_up(t_console *console) {
//   t_screen *screen = &console->screens[0];
//   console_init(console);
//   screen_scroll_up(screen);
//   if (!state_is(console, 0, 0, 1, 0, 0))
//     return FALSE;
//
//   write_lines(console, 30);
//   /* Walk back to the oldest row, checking each step and the upper limit. */
//   for (uint8_t view = 6; view > 0; view--) {
//     screen_scroll_up(screen);
//     if (!state_is(console, 30, 0, 31, 0, view - 1))
//       return FALSE;
//   }
//   screen_scroll_up(screen);
//   if (!state_is(console, 30, 0, 31, 0, 0))
//     return FALSE;
//
//   write_lines(console, 498);
//   /* Row 0 can scroll back to 249, which is the oldest retained row. */
//   screen->view_start = 0;
//   screen_scroll_up(screen);
//   if (!state_is(console, 248, 0, CELL_ROW, 249, 249))
//     return FALSE;
//   screen_scroll_up(screen);
//   return state_is(console, 248, 0, CELL_ROW, 249, 249);
// }
//
// bool_t test_console_scroll_down(t_console *console) {
//   t_screen *screen = &console->screens[0];
//   /* Empty, partially filled and exactly full views cannot scroll down. */
//   const uint8_t counts[] = {0, 10, 24};
//   for (uint8_t i = 0; i < 3; i++) {
//     write_lines(console, counts[i]);
//     screen_scroll_down(screen);
//     if (!state_is(console, counts[i], 0, counts[i] + 1, 0, 0))
//       return FALSE;
//   }
//
//   write_lines(console, 30);
//   screen->view_start = 0;
//   for (uint8_t view = 1; view <= 6; view++) {
//     screen_scroll_down(screen);
//     if (!state_is(console, 30, 0, 31, 0, view))
//       return FALSE;
//   }
//   screen_scroll_down(screen);
//   if (!state_is(console, 30, 0, 31, 0, 6))
//     return FALSE;
//
//   write_lines(console, 498);
//   screen->view_start = 249;
//   /* Cross 249 -> 0, then continue until the cursor is at the bottom. */
//   for (uint16_t view = 0; view <= 224; view++) {
//     screen_scroll_down(screen);
//     if (!state_is(console, 248, 0, CELL_ROW, 249, view))
//       return FALSE;
//   }
//   screen_scroll_down(screen);
//   return state_is(console, 248, 0, CELL_ROW, 249, 224);
// }
//
// bool_t test_console_history_output(t_console *console) {
//   write_lines(console, 100);
//   t_screen *screen = &console->screens[0];
//   for (uint8_t i = 0; i < 10; i++)
//     screen_scroll_up(screen);
//   if (!state_is(console, 100, 0, 101, 0, 66))
//     return FALSE;
//
//   /* New output must preserve both the view and its numbered lines. */
//   for (uint16_t number = 100; number < 110; number++)
//     write_numbered_line(console, number);
//   if (!state_is(console, 110, 0, 111, 0, 66))
//     return FALSE;
//   for (uint16_t number = 66; number <= 90; number++) {
//     t_vga *line = screen->cells[number];
//     if (line[0]._char != '0' || line[1]._char != '0' + number / 10 ||
//         line[2]._char != '0' + number % 10)
//       return FALSE;
//   }
//   return TRUE;
// }
//
// bool_t test_console_history_output_wrap(t_console *console) {
//   write_lines(console, 499);
//   t_screen *screen = &console->screens[0];
//   screen->view_start = 100;
//
//   /* Character wrapping must recycle rows without moving a preserved view. */
//   for (uint8_t col = 0; col < CELL_COL; col++)
//     console_putc(console, (t_vga){'X', VGA_ATTR_GREY_BLACK});
//   if (!state_is(console, 0, 0, CELL_ROW, 1, 100))
//     return FALSE;
//   for (uint8_t col = 0; col < CELL_COL; col++) {
//     if (screen->cells[249][col]._char != 'X' ||
//         screen->cells[0][col]._char != ' ' ||
//         screen->cells[0][col].attr != VGA_ATTR_GREY_BLACK)
//       return FALSE;
//   }
//   /* Explicit newlines must preserve the same view as well. */
//   console_putc(console, (t_vga){'\n', VGA_ATTR_GREY_BLACK});
//   return state_is(console, 1, 0, CELL_ROW, 2, 100);
// }
//
// bool_t test_console_history_recycling(t_console *console) {
//   write_lines(console, 498);
//   t_screen *screen = &console->screens[0];
//   screen->view_start = screen->line_old;
//
//   /* Keep reading the oldest history through more than one complete turn. */
//   for (uint16_t step = 1; step <= 260; step++) {
//     console_putc(console, (t_vga){'\n', VGA_ATTR_GREY_BLACK});
//     uint8_t oldest = (249 + step) % CELL_ROW;
//     if (!state_is(console, (248 + step) % CELL_ROW, 0, CELL_ROW, oldest,
//                   oldest))
//       return FALSE;
//   }
//   return TRUE;
// }
//
// bool_t test_console_resume_follow(t_console *console) {
//   write_lines(console, 30);
//   t_screen *screen = &console->screens[0];
//   screen_scroll_up(screen);
//   screen_scroll_up(screen);
//   console_putc(console, (t_vga){'\n', VGA_ATTR_GREY_BLACK});
//   if (!state_is(console, 31, 0, 32, 0, 4))
//     return FALSE;
//   for (uint8_t i = 0; i < 3; i++)
//     screen_scroll_down(screen);
//   if (!state_is(console, 31, 0, 32, 0, 7))
//     return FALSE;
//   console_putc(console, (t_vga){'\n', VGA_ATTR_GREY_BLACK});
//   if (!state_is(console, 32, 0, 33, 0, 8))
//     return FALSE;
//
//   /* Repeat with a full buffer and an output cursor crossing 249 -> 0. */
//   write_lines(console, 498);
//   screen_scroll_up(screen);
//   console_putc(console, (t_vga){'\n', VGA_ATTR_GREY_BLACK});
//   if (!state_is(console, 249, 0, CELL_ROW, 0, 223))
//     return FALSE;
//   screen_scroll_down(screen);
//   screen_scroll_down(screen);
//   if (!state_is(console, 249, 0, CELL_ROW, 0, 225))
//     return FALSE;
//   console_putc(console, (t_vga){'\n', VGA_ATTR_GREY_BLACK});
//   return state_is(console, 0, 0, CELL_ROW, 1, 226);
// }
//
// bool_t test_console_init_blank(t_console *console) {
//   /* Dirty every cell first: static zero initialization must not hide
//   defects.
//    */
//   for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
//     t_screen *screen = &console->screens[n];
//     screen->view_start = 123;
//     screen->line_old = 42;
//     screen->line_count = CELL_ROW;
//     screen->cursor = (t_cursor){12, 34};
//     for (uint16_t row = 0; row < CELL_ROW; row++)
//       for (uint8_t col = 0; col < CELL_COL; col++)
//         screen->cells[row][col] = (t_vga){'X', 0x4F};
//   }
//   console->screen_select = SCREEN_NUMBER - 1;
//   console_init(console);
//   if (console->screen_select != 0)
//     return FALSE;
//   for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
//     t_screen *screen = &console->screens[n];
//     if (screen->view_start != 0 || screen->line_old != 0 ||
//         screen->line_count != 1 || screen->cursor.row != 0 ||
//         screen->cursor.col != 0)
//       return FALSE;
//     for (uint16_t row = 0; row < CELL_ROW; row++)
//       for (uint8_t col = 0; col < CELL_COL; col++)
//         if (screen->cells[row][col]._char != ' ' ||
//             screen->cells[row][col].attr != VGA_ATTR_GREY_BLACK)
//           return FALSE;
//   }
//   return TRUE;
// }
//
// bool_t test_console_screen_independence(t_console *console) {
//   console_init(console);
//   for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
//     console_select_screen(console, n);
//     if (console->screen_select != n)
//       return FALSE;
//     for (uint16_t line = 0; line < 30 + n; line++)
//       write_numbered_line(console, line);
//     console_putc(console, (t_vga){'A' + n, 0x10 + n});
//     for (uint8_t step = 0; step <= n; step++)
//       screen_scroll_up(&console->screens[n]);
//   }
//   /* Revisit every screen and verify all cells and its independent state. */
//   for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
//     console_select_screen(console, n);
//     if (console->screen_select != n)
//       return FALSE;
//     t_screen *screen = &console->screens[n];
//     if (screen->cursor.row != 30 + n || screen->cursor.col != 1 ||
//         screen->line_count != 31 + n || screen->line_old != 0 ||
//         screen->view_start != 5)
//       return FALSE;
//     for (uint16_t row = 0; row < CELL_ROW; row++) {
//       for (uint8_t col = 0; col < CELL_COL; col++) {
//         uint8_t expected = ' ';
//         uint8_t attr = VGA_ATTR_GREY_BLACK;
//         if (row < 30 + n && col < 3) {
//           if (col == 0)
//             expected = '0' + row / 100;
//           else if (col == 1)
//             expected = '0' + row / 10 % 10;
//           else
//             expected = '0' + row % 10;
//         } else if (row == 30 + n && col == 0) {
//           expected = 'A' + n;
//           attr = 0x10 + n;
//         }
//         if (screen->cells[row][col]._char != expected ||
//             screen->cells[row][col].attr != attr)
//           return FALSE;
//       }
//     }
//   }
//   return TRUE;
// }
//
// bool_t test_console_screen_invalid(t_console *console) {
//   console_init(console);
//   /* Invalid indices must preserve every possible current selection. */
//   for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
//     console_select_screen(console, n);
//     if (console->screen_select != n)
//       return FALSE;
//     for (uint16_t invalid = SCREEN_NUMBER; invalid <= 255; invalid++) {
//       console_select_screen(console, invalid);
//       if (console->screen_select != n)
//         return FALSE;
//     }
//   }
//   return TRUE;
// }
//
// void test_console_screens_visual(t_console *console) {
//   write_lines(console, 30);
//   console_putc(console, (t_vga){'A', VGA_ATTR_GREY_BLACK});
//   for (uint8_t i = 0; i < 5; i++)
//     screen_scroll_up(&console->screens[0]);
//   console_render_screen(console);
//   console_visual_checkpoint("screen 0: history 001..025, hidden cursor",
//                             console);
//
//   console_select_screen(console, 1);
//   console_render_screen(console);
//   console_visual_checkpoint("screen 1: blank, cursor at top left", console);
//
//   console_putc(console, (t_vga){'B', 0x0A});
//   console_render_screen(console);
//   console_visual_checkpoint("screen 1: green B, visible cursor after B",
//                             console);
//
//   console_select_screen(console, 0);
//   console_render_screen(console);
//   console_visual_checkpoint("screen 0: preserved history, hidden cursor",
//                             console);
//
//   console_select_screen(console, SCREEN_NUMBER - 1);
//   console_putc(console, (t_vga){'E', 0x0C});
//   console_render_screen(console);
//   console_visual_checkpoint("screen 4: red E, visible cursor after E",
//   console);
//
//   console_select_screen(console, 1);
//   console_render_screen(console);
//   console_visual_checkpoint("screen 1 again: preserved green B and cursor",
//                             console);
// }
