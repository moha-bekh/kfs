#ifndef VGA_H
#define VGA_H

#include "console.h"

#define VGA_ATTR_GREY_BLACK 0x07

void vga_putc(t_cell, uint8_t x, uint8_t y);

#endif /* VGA_H */
