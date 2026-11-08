#ifndef KERNEL_KERNEL_H
#define KERNEL_KERNEL_H

/*
 * Kernel entry point, called from src/boot/boot.s.
 * The prototype is only needed by -Wmissing-prototypes: boot.s (NASM) cannot
 * read C headers, so no C file calls kmain.
 */
int kmain(void);

#endif /* KERNEL_KERNEL_H */
