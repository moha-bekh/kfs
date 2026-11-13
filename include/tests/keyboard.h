#ifndef TESTS_KEYBOARD_H
#define TESTS_KEYBOARD_H
#include "types.h"
typedef struct s_keyboard_test_results {
  bool_t press_release;
  bool_t shift;
  bool_t caps_shift;
  bool_t caps_repeat;
  bool_t num_repeat;
  bool_t keypad_digits;
  bool_t keypad_shift;
  bool_t keypad_operators;
  bool_t super_screens;
  bool_t extended_modifiers;
  bool_t prefix_consumption;
  bool_t pause_printscreen;
  bool_t scroll_lock;
  bool_t backspace;
  bool_t backspace_boundary;
  bool_t backspace_color;
  bool_t backspace_wrap;
  bool_t tabs;
  bool_t tab_boundary;
  bool_t tab_scroll;
  bool_t navigation;
  bool_t keypad_navigation;
  bool_t ignored_navigation;
  bool_t decoder_reset;
  bool_t all_passed;
} t_keyboard_test_results;
/* Break here, inspect name, then finish to see the result. */
bool_t keyboard_test_case(bool_t (*test)(void), const char *name);
t_keyboard_test_results test_keyboard_all(void);
#endif
