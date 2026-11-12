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

/* Break here and inspect name to identify a console scenario. */
bool_t console_test_case(bool_t (*test)(void), const char *name);

/* Run every scenario, even if an earlier one fails. */
t_console_test_results test_console_all(void);

/* Observe each rendered stage with a breakpoint on console_visual_checkpoint. */
void test_console_screens_visual(void);

/* History cases use a private static screen fixture to exercise screen_*.
 * Output and selection cases use the public console API and rendered VGA cells.
 * Every scenario resets the state it uses. No access to the private console.
 */
bool_t test_console_42(void);
bool_t test_console_wrap(void);
bool_t test_console_24_newlines(void);
bool_t test_console_25_newlines(void);
bool_t test_console_250_newlines(void);
bool_t test_console_500_newlines(void);
bool_t test_console_history_overwrite(void);
bool_t test_console_history_wrap(void);
bool_t test_console_scroll_up(void);
bool_t test_console_scroll_down(void);
bool_t test_console_history_output(void);
bool_t test_console_history_output_wrap(void);
bool_t test_console_history_recycling(void);
bool_t test_console_resume_follow(void);
bool_t test_console_init_blank(void);
bool_t test_console_screen_independence(void);
bool_t test_console_screen_invalid(void);

#endif /* TESTS_CONSOLE_H */
