#ifndef LIB_STDLIB_H
#define LIB_STDLIB_H

/* Inputs must represent values within the return type's range.
 * These conversions do not report errors or provide overflow detection.
 */
int atoi(const char *s);
long atol(const char *s);
/* Project helper: bases 2..16, optional sign, no 0x prefix handling.
 * An invalid base or a missing initial digit returns 0.
 */
int atoi_base(const char *s, int base);

#endif /* LIB_STDLIB_H */
