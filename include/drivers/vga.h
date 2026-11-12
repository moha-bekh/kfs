#ifndef DRIVERS_VGA_H
#define DRIVERS_VGA_H

#include "types.h"

// Cell Memory Layout:
// Special    bg          fg        char
// [15]       [14 - 12]   [11 - 8]  [7 - 0]
//
// Members Bit Operations:
//
// Special
//  mask:   0x80 - 1000 0000
//  shift:  0x07 - 0111
//
// BG-color:
//  mask:   0x70 - 0111 0000
//  shift:  0x04 - 0100
//
// FG-color:
//  mask: 0x0F - 0000 1111
//  shift: 0
//
// attr = (special << 7) | (bg << 4) | fg;

#define VGA_ATTR_GREY_BLACK 0x07

typedef struct s_vga {

  uint8_t _char;
  uint8_t attr;

} t_vga;

void vga_putc(t_vga, uint8_t x, uint8_t y);
void vga_move_cursor(uint8_t x, uint8_t y);
void vga_show_cursor(void);
void vga_hide_cursor(void);

#endif /* DRIVERS_VGA_H */
