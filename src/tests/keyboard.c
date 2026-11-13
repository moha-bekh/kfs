#include "tests/keyboard.h"
#include "console/console.h"
#include "drivers/keyboard.h"
#include "lib/string.h"

static volatile const t_vga *const video = (volatile const t_vga *)0xB8000;

static void key(uint8_t code) {
  kb_ps2_decode(code);
  kb_ps2_decode(code | PS2_SC_RELEASE_MASK);
}

static void extended_key(uint8_t code) {
  kb_ps2_decode(PS2_SC_EXTENDED);
  kb_ps2_decode(code);
  kb_ps2_decode(PS2_SC_EXTENDED);
  kb_ps2_decode(code | PS2_SC_RELEASE_MASK);
}

static bool_t row_is(uint8_t row, const char *text) {
  size_t n = strlen(text);
  if (n > CELL_COL)
    return FALSE;
  for (size_t i = 0; i < CELL_COL; i++)
    if (video[row * CELL_COL + i]._char != (i < n ? (uint8_t)text[i] : ' '))
      return FALSE;
  return TRUE;
}

static bool_t text_is(const char *text) {
  console_render_screen();
  if (!row_is(0, text))
    return FALSE;
  for (uint8_t row = 1; row < VIEW_CAPACITY; row++)
    if (!row_is(row, ""))
      return FALSE;
  return TRUE;
}

/* Distinct numbered rows expose history wrap, direction and boundary errors. */
static void fill_history(void) {
  for (unsigned i = 0; i < 300; i++) {
    console_putc('0' + i / 100);
    console_putc('0' + (i / 10) % 10);
    console_putc('0' + i % 10);
    console_putc('\n');
  }
  console_render_screen();
}

static bool_t top_is(unsigned n) {
  char expected[] = {'0' + n / 100, '0' + (n / 10) % 10, '0' + n % 10, 0};
  console_render_screen();
  return row_is(0, expected);
}

static bool_t check_press_release(void) {
  key(0x1E);
  return text_is("a");
}

static bool_t check_shift(void) {
  kb_ps2_decode(0x2A);
  key(0x1E);
  kb_ps2_decode(0x36);
  kb_ps2_decode(0xAA);
  key(0x1E);
  kb_ps2_decode(0xB6);
  key(0x1E);
  return text_is("AAa");
}

static bool_t check_caps_shift(void) {
  key(0x3A);
  key(0x1E);
  kb_ps2_decode(0x2A);
  key(0x1E);
  key(0x02);
  kb_ps2_decode(0xAA);
  key(0x02);
  key(0x3A);
  key(0x1E);
  return text_is("Aa!1a");
}

static bool_t check_caps_repeat(void) {
  kb_ps2_decode(0x3A);
  kb_ps2_decode(0x3A);
  kb_ps2_decode(0x3A);
  key(0x1E);
  kb_ps2_decode(0xBA);
  key(0x1E);
  key(0x3A);
  key(0x1E);
  return text_is("AAa");
}

static bool_t check_num_repeat(void) {
  kb_ps2_decode(0x45);
  kb_ps2_decode(0x45);
  key(0x4C);
  kb_ps2_decode(0xC5);
  key(0x4C);
  key(0x45);
  key(0x4C);
  return text_is("55");
}

static bool_t check_keypad_digits(void) {
  static const uint8_t codes[] = {0x52, 0x4F, 0x50, 0x51, 0x4B, 0x4C,
                                  0x4D, 0x47, 0x48, 0x49, 0x53};
  key(0x45);
  for (size_t i = 0; i < sizeof(codes); i++)
    key(codes[i]);
  return text_is("0123456789.");
}

static bool_t check_keypad_shift(void) {
  key(0x4C);
  kb_ps2_decode(0x2A);
  key(0x4C);
  kb_ps2_decode(0xAA);
  key(0x45);
  key(0x4C);
  kb_ps2_decode(0x36);
  key(0x4C);
  kb_ps2_decode(0xB6);
  key(0x4C);
  return text_is("555");
}

static bool_t check_keypad_operators(void) {
  key(0x37);
  key(0x4A);
  key(0x4E);
  extended_key(0x35);
  key(0x45);
  key(0x37);
  key(0x4A);
  key(0x4E);
  kb_ps2_decode(0x2A);
  extended_key(0x35);
  kb_ps2_decode(0xAA);
  extended_key(0x1C);
  key(0x1E);
  console_render_screen();
  return row_is(0, "*-+/*-+/") && row_is(1, "a");
}

static bool_t check_super_screens(void) {
  for (uint8_t i = 0; i < 5; i++) {
    kb_ps2_decode(0xE0);
    kb_ps2_decode(0x5B);
    key(0x02 + i);
    kb_ps2_decode(0xE0);
    kb_ps2_decode(0xDB);
    key(0x1E);
  }
  for (uint8_t i = 0; i < 5; i++) {
    console_select_screen(i);
    if (!text_is("a"))
      return FALSE;
  }
  kb_ps2_decode(0xE0);
  kb_ps2_decode(0x5C);
  key(0x02);
  kb_ps2_decode(0xE0);
  kb_ps2_decode(0xDC);
  key(0x30);
  return text_is("ab");
}

static bool_t check_extended_modifiers(void) {
  static const uint8_t codes[] = {0x1D, 0x38, 0x5B, 0x5C};
  for (size_t i = 0; i < sizeof(codes); i++)
    extended_key(codes[i]);
  key(0x02);
  key(0x1E);
  return text_is("1a");
}

static bool_t check_prefix_consumption(void) {
  extended_key(0x7F);
  extended_key(0x2A);
  key(0x1E);
  kb_ps2_decode(0xE0);
  kb_ps2_decode(0x9D);
  key(0x1E);
  return text_is("aa");
}

static bool_t check_pause_printscreen(void) {
  static const uint8_t stream[] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5, 0xE0,
                                   0x2A, 0xE0, 0x37, 0xE0, 0xB7, 0xE0, 0xAA};
  for (size_t i = 0; i < sizeof(stream); i++)
    kb_ps2_decode(stream[i]);
  key(0x1E);
  key(0x4C);
  return text_is("a");
}

static bool_t check_scroll_lock(void) {
  kb_ps2_decode(0x46);
  kb_ps2_decode(0x46);
  kb_ps2_decode(0xC6);
  key(0x1E);
  key(0x46);
  key(0x30);
  return text_is("ab");
}

static bool_t check_backspace(void) {
  console_putstr("abc\bd");
  return text_is("abd");
}

static bool_t check_backspace_boundary(void) {
  console_putc('\b');
  console_putstr("a\n\bX");
  console_render_screen();
  return row_is(0, "a") && row_is(1, "X");
}

static bool_t check_backspace_color(void) {
  console_set_color(4, 1);
  console_putstr("ab\b");
  console_render_screen();
  return row_is(0, "a") && video[1].attr == 0x14;
}

static bool_t check_backspace_wrap(void) {
  for (uint8_t i = 0; i < 80; i++)
    console_putc('a');
  console_putstr("\bX");
  console_render_screen();
  return video[79]._char == 'a' && row_is(1, "X");
}

static bool_t check_tabs(void) {
  console_putstr("\tX");
  if (!text_is("    X"))
    return FALSE;
  console_init();
  console_putstr("a\tX\tY");
  return text_is("a   X   Y");
}

static bool_t check_tab_boundary(void) {
  for (uint8_t col = 76; col < 80; col++) {
    console_init();
    for (uint8_t i = 0; i < col; i++)
      console_putc('a');
    console_putstr("\tX");
    console_render_screen();
    if (!row_is(1, "X"))
      return FALSE;
    for (uint8_t i = col; i < 80; i++)
      if (video[i]._char != ' ')
        return FALSE;
  }
  return TRUE;
}

static bool_t check_tab_scroll(void) {
  for (uint8_t i = 0; i < 24; i++)
    console_putc('\n');
  for (uint8_t i = 0; i < 79; i++)
    console_putc('a');
  console_putstr("\tX");
  console_render_screen();
  return row_is(24, "X") && video[23 * 80 + 78]._char == 'a' &&
         video[23 * 80 + 79]._char == ' ';
}

static bool_t check_navigation(void) {
  fill_history();
  extended_key(0x49);
  if (!top_is(275))
    return FALSE;
  extended_key(0x51);
  if (!top_is(276))
    return FALSE;
  extended_key(0x47);
  if (!top_is(51))
    return FALSE;
  extended_key(0x49);
  if (!top_is(51))
    return FALSE;
  extended_key(0x4F);
  return top_is(276);
}

static bool_t check_keypad_navigation(void) {
  fill_history();
  key(0x49);
  if (!top_is(275))
    return FALSE;
  key(0x51);
  if (!top_is(276))
    return FALSE;
  key(0x47);
  if (!top_is(51))
    return FALSE;
  key(0x4F);
  return top_is(276);
}

static bool_t check_ignored_navigation(void) {
  fill_history();
  extended_key(0x4B);
  extended_key(0x4D);
  extended_key(0x52);
  extended_key(0x53);
  return top_is(276);
}

static bool_t check_decoder_reset(void) {
  key(0x3A);
  kb_ps2_decode(0x2A);
  kb_ps2_decode(0xE0);
  kb_reset();
  key(0x1E);
  kb_ps2_decode(0xE1);
  kb_reset();
  key(0x30);
  return text_is("ab");
}

bool_t keyboard_test_case(bool_t (*test)(void), const char *name) {
  (void)name;
  console_init();
  kb_reset();
  return test();
}

t_keyboard_test_results test_keyboard_all(void) {
  t_keyboard_test_results results;
  results.press_release =
      keyboard_test_case(check_press_release, "press_release");
  results.shift = keyboard_test_case(check_shift, "shift");
  results.caps_shift = keyboard_test_case(check_caps_shift, "caps_shift");
  results.caps_repeat = keyboard_test_case(check_caps_repeat, "caps_repeat");
  results.num_repeat = keyboard_test_case(check_num_repeat, "num_repeat");
  results.keypad_digits =
      keyboard_test_case(check_keypad_digits, "keypad_digits");
  results.keypad_shift = keyboard_test_case(check_keypad_shift, "keypad_shift");
  results.keypad_operators =
      keyboard_test_case(check_keypad_operators, "keypad_operators");
  results.super_screens =
      keyboard_test_case(check_super_screens, "super_screens");
  results.extended_modifiers =
      keyboard_test_case(check_extended_modifiers, "extended_modifiers");
  results.prefix_consumption =
      keyboard_test_case(check_prefix_consumption, "prefix_consumption");
  results.pause_printscreen =
      keyboard_test_case(check_pause_printscreen, "pause_printscreen");
  results.scroll_lock = keyboard_test_case(check_scroll_lock, "scroll_lock");
  results.backspace = keyboard_test_case(check_backspace, "backspace");
  results.backspace_boundary =
      keyboard_test_case(check_backspace_boundary, "backspace_boundary");
  results.backspace_color =
      keyboard_test_case(check_backspace_color, "backspace_color");
  results.backspace_wrap =
      keyboard_test_case(check_backspace_wrap, "backspace_wrap");
  results.tabs = keyboard_test_case(check_tabs, "tabs");
  results.tab_boundary = keyboard_test_case(check_tab_boundary, "tab_boundary");
  results.tab_scroll = keyboard_test_case(check_tab_scroll, "tab_scroll");
  results.navigation = keyboard_test_case(check_navigation, "navigation");
  results.keypad_navigation =
      keyboard_test_case(check_keypad_navigation, "keypad_navigation");
  results.ignored_navigation =
      keyboard_test_case(check_ignored_navigation, "ignored_navigation");
  results.decoder_reset =
      keyboard_test_case(check_decoder_reset, "decoder_reset");
  results.all_passed =
      results.press_release && results.shift && results.caps_shift &&
      results.caps_repeat && results.num_repeat && results.keypad_digits &&
      results.keypad_shift && results.keypad_operators &&
      results.super_screens && results.extended_modifiers &&
      results.prefix_consumption && results.pause_printscreen &&
      results.scroll_lock && results.backspace && results.backspace_boundary &&
      results.backspace_color && results.backspace_wrap && results.tabs &&
      results.tab_boundary && results.tab_scroll && results.navigation &&
      results.keypad_navigation && results.ignored_navigation &&
      results.decoder_reset;
  console_init();
  kb_reset();
  return results;
}
