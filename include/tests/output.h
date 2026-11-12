#ifndef TESTS_OUTPUT_H
#define TESTS_OUTPUT_H

#include "types.h"

typedef struct s_output_test_results {
  bool_t init_blank;
  bool_t putc;
  bool_t putstr;
  bool_t colors;
  bool_t screens;
  bool_t invalid_screen;
  bool_t newline_wrap;
  bool_t history_navigation;
  bool_t literal;
  bool_t percent;
  bool_t character;
  bool_t string;
  bool_t signed_decimal;
  bool_t signed_min;
  bool_t unsigned_decimal;
  bool_t unsigned_zero;
  bool_t hexadecimal;
  bool_t hexadecimal_zero;
  bool_t pointers;
  bool_t mixed_arguments;
  bool_t return_count;
  bool_t all_passed;
} t_output_test_results;

/* Break here before each case; finish returns its pass/fail result. */
bool_t output_test_case(bool_t (*test)(void), const char *name);
t_output_test_results test_output_all(void);

#endif /* TESTS_OUTPUT_H */
