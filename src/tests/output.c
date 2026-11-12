#include "tests/output.h"
#include "console/console.h"
#include "lib/printk.h"
#include "lib/string.h"

/* printk currently declares signed char rather than char for its format. */
#define FORMAT(s) ((const int8_t *)(s))

static volatile const t_vga *const video = (volatile const t_vga *)0xB8000;

static bool_t text_is(const char *expected) {
  console_render_screen();
  size_t len = strlen(expected);
  if (len > CELL_COL)
    return FALSE;
  for (size_t i = 0; i < len; i++)
    if (video[i]._char != (uint8_t)expected[i])
      return FALSE;
  /* Check for extra output as well as the expected prefix. */
  for (size_t i = len; i < CELL_COL; i++)
    if (video[i]._char != ' ')
      return FALSE;
  return TRUE;
}

static bool_t check_init_blank(void) {
  console_init();
  console_putc('X');
  console_set_color(4, 2);
  console_init();
  for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
    console_select_screen(n);
    console_render_screen();
    for (size_t i = 0; i < VIEW_CAPACITY * CELL_COL; i++)
      if (video[i]._char != ' ' || video[i].attr != VGA_ATTR_GREY_BLACK)
        return FALSE;
  }
  return TRUE;
}

static bool_t check_putc(void) {
  console_putc('4');
  console_putc('2');
  return text_is("42");
}

static bool_t check_putstr(void) {
  console_putstr("hello");
  return text_is("hello");
}

static bool_t check_colors(void) {
  console_set_color(0xA, 1);
  console_putc('A');
  console_reset_color();
  console_putc('B');
  return text_is("AB") && video[0].attr == 0x1A &&
         video[1].attr == VGA_ATTR_GREY_BLACK;
}

static bool_t check_screens(void) {
  for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
    console_select_screen(n);
    console_set_color(n + 1, 0);
    console_putc('A' + n);
  }
  for (uint8_t n = 0; n < SCREEN_NUMBER; n++) {
    console_select_screen(n);
    console_putc('0' + n);
    char expected[] = {'A' + n, '0' + n, 0};
    if (!text_is(expected) || video[0].attr != n + 1 || video[1].attr != n + 1)
      return FALSE;
  }
  return TRUE;
}

static bool_t check_invalid_screen(void) {
  console_select_screen(2);
  console_putc('A');
  console_select_screen(SCREEN_NUMBER);
  console_putc('B');
  console_select_screen(255);
  console_putc('C');
  return text_is("ABC");
}

static bool_t check_newline_wrap(void) {
  for (uint8_t i = 0; i < CELL_COL; i++)
    console_putc('X');
  console_putc('Y');
  console_putc('\n');
  console_putc('Z');
  console_render_screen();
  for (uint8_t i = 0; i < CELL_COL; i++)
    if (video[i]._char != 'X')
      return FALSE;
  return video[CELL_COL]._char == 'Y' &&
         video[2 * CELL_COL]._char == 'Z';
}

static bool_t check_history_navigation(void) {
  /* Large history buffers must not live on the boot stack. */
  static t_screen screen;
  screen_init(&screen);
  for (uint16_t line = 1; line <= 498; line++) {
    screen_new_line(&screen);
    uint8_t count = line < CELL_ROW ? line + 1 : CELL_ROW;
    uint8_t oldest = line < CELL_ROW ? 0 : (line + 1) % CELL_ROW;
    uint8_t view = line < VIEW_CAPACITY ? 0 : (line - 24) % CELL_ROW;
    if (screen.cursor.row != line % CELL_ROW || screen.line_count != count ||
        screen.line_old != oldest || screen.view_start != view)
      return FALSE;
  }
  /* Walk all the way back, including 0 -> 249, then verify the upper limit. */
  for (uint16_t step = 1; step <= 225; step++) {
    screen_scroll_up(&screen);
    if (screen.view_start != (224 + CELL_ROW - step) % CELL_ROW)
      return FALSE;
  }
  screen_scroll_up(&screen);
  if (screen.view_start != 249)
    return FALSE;
  screen_new_line(&screen);
  if (screen.view_start != 0 || screen.line_old != 0 || screen.cursor.row != 249)
    return FALSE;
  for (uint16_t step = 1; step <= 225; step++) {
    screen_scroll_down(&screen);
    if (screen.view_start != step)
      return FALSE;
  }
  screen_scroll_down(&screen);
  if (screen.view_start != 225)
    return FALSE;
  screen_new_line(&screen);
  return screen.view_start == 226 && screen.cursor.row == 0 &&
         screen.line_old == 1 && screen.line_count == CELL_ROW;
}

static bool_t check_literal(void) {
  printk(FORMAT("hello 42"));
  return text_is("hello 42");
}

static bool_t check_percent(void) {
  printk(FORMAT("a%%b"));
  return text_is("a%b");
}

static bool_t check_character(void) {
  printk(FORMAT("[%c]"), 'A');
  return text_is("[A]");
}

static bool_t check_string(void) {
  printk(FORMAT("[%s]"), "hello");
  return text_is("[hello]");
}

static bool_t check_signed_decimal(void) {
  printk(FORMAT("%d|%i|%d"), -42, 17, 0);
  return text_is("-42|17|0");
}

static bool_t check_signed_min(void) {
  printk(FORMAT("%d"), (-2147483647 - 1));
  return text_is("-2147483648");
}

static bool_t check_unsigned_decimal(void) {
  printk(FORMAT("%u"), 4294967295U);
  return text_is("4294967295");
}

static bool_t check_unsigned_zero(void) {
  printk(FORMAT("%u"), 0U);
  return text_is("0");
}

static bool_t check_hexadecimal(void) {
  printk(FORMAT("%x|%X"), 0xDEADBEEFU, 0xABCDU);
  return text_is("deadbeef|ABCD");
}

static bool_t check_hexadecimal_zero(void) {
  printk(FORMAT("%x|%X"), 0U, 0U);
  return text_is("0|0");
}

static bool_t check_pointers(void) {
  /* These addresses are formatted only, never dereferenced. */
  printk(FORMAT("%p|%p"), (void *)0, (void *)(uintptr_t)0x1234);
  return text_is("(nil)|0x1234");
}

static bool_t check_mixed_arguments(void) {
  printk(FORMAT("%c:%s:%d:%u:%x"), 'A', "BC", -12, 34U, 0x56U);
  return text_is("A:BC:-12:34:56");
}

static bool_t check_return_count(void) {
  /* Chosen contract: return the number of emitted characters. */
  int count = printk(FORMAT("a%%:%d"), 42);
  return text_is("a%:42") && count == 5;
}

bool_t output_test_case(bool_t (*test)(void), const char *name) {
  (void)name;
  console_init();
  return test();
}

t_output_test_results test_output_all(void) {
  t_output_test_results results;
#define RUN(field) results.field = output_test_case(check_##field, #field)
  RUN(init_blank);
  RUN(putc);
  RUN(putstr);
  RUN(colors);
  RUN(screens);
  RUN(invalid_screen);
  RUN(newline_wrap);
  RUN(history_navigation);
  RUN(literal);
  RUN(percent);
  RUN(character);
  RUN(string);
  RUN(signed_decimal);
  RUN(signed_min);
  RUN(unsigned_decimal);
  RUN(unsigned_zero);
  RUN(hexadecimal);
  RUN(hexadecimal_zero);
  RUN(pointers);
  RUN(mixed_arguments);
  RUN(return_count);
#undef RUN
  results.all_passed = results.init_blank && results.putc && results.putstr &&
      results.colors && results.screens && results.invalid_screen &&
      results.newline_wrap && results.history_navigation &&
      results.literal && results.percent &&
      results.character && results.string && results.signed_decimal &&
      results.signed_min && results.unsigned_decimal && results.unsigned_zero &&
      results.hexadecimal && results.hexadecimal_zero && results.pointers &&
      results.mixed_arguments && results.return_count;
  return results;
}
