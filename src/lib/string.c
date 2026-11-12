/*
 * KFS-1
 *
 * Role: Basic helpers: strlen, strcmp, memset, memcpy, ...
 * Depends on: nothing
 * Unlocks: printk, shell, everything else
 */

#include "lib/string.h"

size_t strlen(const char *s) {

  size_t i;

  i = 0;
  while (s[i])
    i++;

  return (i);
}

int strcmp(const char *s1, const char *s2) {

  size_t i;

  if (!s1 || !s2)
    return (-1);

  i = 0;
  while (s1[i] || s2[i]) {
    if (s1[i] != s2[i])
      return ((unsigned char)s1[i] - (unsigned char)s2[i]);
    i++;
  }

  return (0);
}

void *memset(void *s, int c, size_t n) {

  size_t i;

  i = 0;
  while (i < n) {
    ((unsigned char *)s)[i] = c;
    i++;
  }
  return (s);
}

int strncmp(const char *s1, const char *s2, size_t n) {

  if (!n)
    return (0);
  if (!s1 || !s2)
    return (-1);
  for (size_t i = 0; i < n; i++) {
    if (s1[i] != s2[i])
      return ((unsigned char)s1[i] - (unsigned char)s2[i]);
    if (!s1[i])
      return (0);
  }
  return (0);
}

void *memcpy(void *restrict dst, const void *restrict src, size_t n) {

  unsigned char *dest = dst;
  const unsigned char *source = src;

  for (size_t i = 0; i < n; i++)
    dest[i] = source[i];

  return (dst);
}

void *memmove(void *dst, const void *src, size_t n) {

  unsigned char *dest = dst;
  const unsigned char *source = src;

  /* Address ordering assumes this kernel's flat 32-bit address space. */
  if ((uintptr_t)dst > (uintptr_t)src) {
    while (n > 0) {
      n--;
      dest[n] = source[n];
    }
  } else {
    for (size_t i = 0; i < n; i++)
      dest[i] = source[i];
  }

  return (dst);
}

int memcmp(const void *s1, const void *s2, size_t n) {

  const unsigned char *first = s1;
  const unsigned char *second = s2;

  for (size_t i = 0; i < n; i++) {
    if (first[i] != second[i])
      return (first[i] - second[i]);
  }

  return (0);
}

/* Adapted from the project's libft string and memory helpers. */
char *strchr(const char *s, int c) {

  for (;;) {
    if (*s == (char)c)
      return (char *)s;
    if (!*s)
      return (void *)0;
    s++;
  }
}

char *strrchr(const char *s, int c) {

  const char *last = (void *)0;

  do {
    if (*s == (char)c)
      last = s;
  } while (*s++);

  return (char *)last;
}

size_t strlcpy(char *restrict dst, const char *restrict src, size_t size) {

  size_t i = 0;

  if (size > 0) {
    while (i < size - 1 && src[i]) {
      dst[i] = src[i];
      i++;
    }
    dst[i] = '\0';
  }

  return strlen(src);
}

size_t strlcat(char *restrict dst, const char *restrict src, size_t size) {

  size_t used = 0;
  /* Do not read beyond size when the destination has no terminator. */
  while (used < size && dst[used])
    used++;

  if (used == size)
    return size + strlen(src);

  return used + strlcpy(dst + used, src, size - used);
}

void *memchr(const void *s, int c, size_t n) {

  const unsigned char *bytes = s;

  for (size_t i = 0; i < n; i++)
    if (bytes[i] == (unsigned char)c)
      return (void *)(bytes + i);

  return (void *)0;
}
