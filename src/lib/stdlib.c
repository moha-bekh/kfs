#include "lib/stdlib.h"
#include "lib/ctype.h"

/* Adapted from libft. Negative accumulation also handles LONG_MIN. */
long atol(const char *s) {
  long result = 0;
  int negative = 0;
  while (isspace((unsigned char)*s))
    s++;
  if (*s == '+' || *s == '-') {
    negative = *s == '-';
    s++;
  }
  while (isdigit((unsigned char)*s)) {
    result = result * 10 - (*s - '0');
    s++;
  }
  return negative ? result : -result;
}

int atoi(const char *s) {
  return (int)atol(s);
}

int atoi_base(const char *s, int base) {
  int result = 0;
  int negative = 0;
  if (base < 2 || base > 16)
    return 0;
  while (isspace((unsigned char)*s))
    s++;
  if (*s == '+' || *s == '-') {
    negative = *s == '-';
    s++;
  }
  while (*s) {
    int c = tolower((unsigned char)*s);
    int digit;
    if (isdigit(c))
      digit = c - '0';
    else if (c >= 'a' && c <= 'f')
      digit = c - 'a' + 10;
    else
      break;
    if (digit >= base)
      break;
    result = result * base - digit;
    s++;
  }
  return negative ? result : -result;
}
