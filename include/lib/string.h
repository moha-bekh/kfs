#ifndef LIB_STRING_H
#define LIB_STRING_H

/* KFS-1 — see src/lib/string.c */

#include "types.h"

size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
void *memset(void *s, int c, size_t n);
/* memcpy requires non-overlapping ranges; memmove supports overlap. */
void *memcpy(void *restrict dst, const void *restrict src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
size_t strlcpy(char *restrict dst, const char *restrict src, size_t size);
size_t strlcat(char *restrict dst, const char *restrict src, size_t size);
void *memchr(const void *s, int c, size_t n);

#endif /* LIB_STRING_H */
