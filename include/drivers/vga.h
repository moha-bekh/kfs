#ifndef DRIVERS_VGA_H
#define DRIVERS_VGA_H

#include "console/console.h"

#define VGA_ATTR_GREY_BLACK 0x07

void vga_putc(t_cell, uint8_t x, uint8_t y);

#endif /* DRIVERS_VGA_H */
