#ifndef IO_H
#define IO_H

/*
 * Port I/O helpers (inb, outb, ...) used by keyboard, pic, pit, VGA cursor.
 * Depends on: nothing. Typically small static inline functions using inline
 * asm.
 */

#include "types.h"

static inline void outb(uint16_t port, uint8_t value) {
  __asm__ volatile("out dx, al" : : "d"(port), "a"(value));
}

static inline uint8_t inb(uint16_t port) {
  uint8_t res;
  __asm__ volatile("in al, dx" : "=a"(res) : "d"(port));
  return res;
}

#endif /* IO_H */
