#ifndef TESTS_CONSOLE_H
#define TESTS_CONSOLE_H

#include "console/console.h"

typedef struct s_console_test_results {
  bool_t text_42;
  bool_t wrap;
  bool_t newlines_24;
  bool_t newlines_25;
  bool_t newlines_250;
  bool_t newlines_500;
  bool_t history_overwrite;
  bool_t history_wrap;
  bool_t scroll_up;
  bool_t scroll_down;
  bool_t history_output;
  bool_t history_output_wrap;
  bool_t history_recycling;
  bool_t resume_follow;
  bool_t init_blank;
  bool_t screen_independence;
  bool_t screen_invalid;
  bool_t all_passed;
} t_console_test_results;

/* Run every scenario, even if an earlier one fails. */
t_console_test_results test_console_all(t_console *console);

/* Observe each rendered stage with a breakpoint on console_visual_checkpoint. */
void test_console_visual(t_console *console);
void test_console_screens_visual(t_console *console);

/* Each scenario resets the console and returns TRUE when its checks pass.
 * Call console_render_screen(console) afterwards to inspect the result.
 * Use a static console: its five buffers are too large for the boot stack.
 */
bool_t test_console_42(t_console *console);
bool_t test_console_wrap(t_console *console);
bool_t test_console_24_newlines(t_console *console);
bool_t test_console_25_newlines(t_console *console);
bool_t test_console_250_newlines(t_console *console);
bool_t test_console_500_newlines(t_console *console);
bool_t test_console_history_overwrite(t_console *console);
bool_t test_console_history_wrap(t_console *console);
bool_t test_console_scroll_up(t_console *console);
bool_t test_console_scroll_down(t_console *console);
bool_t test_console_history_output(t_console *console);
bool_t test_console_history_output_wrap(t_console *console);
bool_t test_console_history_recycling(t_console *console);
bool_t test_console_resume_follow(t_console *console);
bool_t test_console_init_blank(t_console *console);
bool_t test_console_screen_independence(t_console *console);
bool_t test_console_screen_invalid(t_console *console);

#endif /* TESTS_CONSOLE_H */
