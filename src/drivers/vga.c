#include "vga.h"
#include "console.h"

#define VGA_MEMORY_ADDR 0xB8000

void vga_putc(t_cell cell, uint8_t x, uint8_t y) {
  volatile uint16_t *video = (uint16_t *)VGA_MEMORY_ADDR;
  video[y * 80 + x] = cell._char | (cell.attr << 8);
}
