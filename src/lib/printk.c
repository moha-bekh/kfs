/*
 * KFS-1 (bonus)
 *
 * Role: printk / printf-like formatted output (%d %x %s %c %p ...).
 * <stdarg.h> is a freestanding header provided by the compiler.
 * Depends on: console, kstring
 * Unlocks: stack dump (KFS-2), panic (KFS-3), exception messages (KFS-4)
 */

#include "lib/printk.h"
#include "console/console.h"
#include "drivers/vga.h"

#include <stdarg.h>
#include <stdint.h>

static void putchar(char c, uint32_t *len) {

  console_putc(c);
  ++(*len);
}

static void putnbr_base(uint32_t nb, uint8_t base, bool_t upper,
                        uint32_t *len) {

  if (nb >= base)
    putnbr_base(nb / base, base, upper, len);

  uint8_t mod = nb % base;
  if (mod < 10)
    putchar(mod + '0', len);
  else {
    if (upper)
      putchar(mod - 10 + 'A', len);
    else
      putchar(mod - 10 + 'a', len);
  }
}

static void putptr(uintptr_t ptr, uint32_t *len) {

  if (ptr >= 16)
    putptr(ptr / 16, len);

  uint32_t mod = ptr % 16;
  if (mod < 10)
    putchar(mod + '0', len);
  else
    putchar(mod - 10 + 'a', len);
}

static void put_format(va_list *args, const char *format, uint32_t *index,
                       uint32_t *len) {

  switch (format[++(*index)]) {

  case '%': {

    putchar(format[(*index)++], len);
    break;
  }

  case 'c': {

    int8_t c = va_arg(*args, int32_t);
    putchar(c, len);
    (*index)++;
    break;
  }

  case 's': {

    const char *str = va_arg(*args, char *);

    uint32_t i = 0;
    while (str[i])
      putchar(str[i++], len);

    (*index)++;
    break;
  }

  case 'd':
  case 'i': {

    int32_t nb = va_arg(*args, int32_t);
    uint32_t magnitude = (uint32_t)nb;

    if (nb < 0) {
      putchar('-', len);
      magnitude = 0U - magnitude;
    }

    putnbr_base(magnitude, 10, FALSE, len);
    (*index)++;
    break;
  }

  case 'u': {

    uint32_t nb = va_arg(*args, uint32_t);

    putnbr_base(nb, 10, FALSE, len);
    (*index)++;
    break;
  }

  case 'x': {

    uint32_t nb = va_arg(*args, uint32_t);

    putnbr_base(nb, 16, FALSE, len);
    (*index)++;
    break;
  }

  case 'X': {

    uint32_t nb = va_arg(*args, uint32_t);

    putnbr_base(nb, 16, TRUE, len);
    (*index)++;
    break;
  }

  case 'p': {

    void *ptr = va_arg(*args, void *);
    uintptr_t addr = (uintptr_t)ptr;
    if (!addr) {
      console_putstr("(nil)");
      (*len) += 5;
    } else {
      console_putstr("0x");
      putptr(addr, len);
      (*len) += 2;
    }
    (*index)++;
    break;
  }

  default:
    break;
  }
}

int printk(const char *format, ...) {

  va_list args;
  va_start(args, format);

  uint32_t len = 0;
  uint32_t i = 0;

  while (format[i]) {
    switch (format[i]) {
    case '%':
      put_format(&args, format, &i, &len);
      break;
    default:
      putchar(format[i], &len);
      i++;
      break;
    }
  }

  console_render_screen();
  va_end(args);
  return len;
}
