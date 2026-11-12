#ifndef LIB_CTYPE_H
#define LIB_CTYPE_H

/* ASCII classification and case conversion; no locale support. */
int isalpha(int c);
int isdigit(int c);
int isalnum(int c);
int isspace(int c);
int isprint(int c);
int isascii(int c);
int tolower(int c);
int toupper(int c);

#endif /* LIB_CTYPE_H */
