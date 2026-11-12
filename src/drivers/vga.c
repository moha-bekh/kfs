#include "drivers/vga.h"
#include "console/console.h"
#include "io.h"
#include "types.h"

#define VGA_MEMORY_ADDR 0xB8000

volatile uint16_t *vga = (volatile uint16_t *)VGA_MEMORY_ADDR;

void vga_putc(t_vga cell, uint8_t x, uint8_t y) {
  vga[y * 80 + x] = (cell.attr << 8) | cell._char;
}

#define CRTC_REG_SELECT 0x3D4
#define CRTC_REG_RW 0x3D5

void vga_move_cursor(uint8_t x, uint8_t y) {

  uint32_t position = y * CELL_COL + x;

  outb(CRTC_REG_SELECT, 0x0F); // CURSOR LOW BYTE
  outb(CRTC_REG_RW, position);

  outb(CRTC_REG_SELECT, 0x0E); // CURSOR HIGH BYTE
  outb(CRTC_REG_RW, (position >> 8));
}

void vga_show_cursor(void) {

  uint8_t cursor_config;

  outb(CRTC_REG_SELECT, 0x0A); // CURSOR VISIBILITY
  cursor_config = inb(CRTC_REG_RW);

  cursor_config = cursor_config & ~0x20;
  outb(CRTC_REG_RW, cursor_config);
}

void vga_hide_cursor(void) {

  uint8_t cursor_config;

  outb(CRTC_REG_SELECT, 0x0A); // CURSOR VISIBILITY
  cursor_config = inb(CRTC_REG_RW);

  cursor_config = cursor_config | 0x20;
  outb(CRTC_REG_RW, cursor_config);
}
