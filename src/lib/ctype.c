#include "lib/ctype.h"

/* Adapted from libft; preserve the standard int return types. */

int	isalpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (1);
	return (0);
}

int	isdigit(int c)
{
	if ((c >= '0' && c <= '9'))
		return (1);
	return (0);
}

int	isalnum(int c)
{
	if (isalpha(c) || isdigit(c))
		return (1);
	return (0);
}

int	isspace(int c)
{
	if (c == 32 || (c >= 9 && c <= 13))
		return (1);
	return (0);
}

int	isprint(int c)
{
	if (c >= 32 && c <= 126)
		return (1);
	return (0);
}

int	isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);
	return (0);
}

int	tolower(int c)
{
	if (c >= 65 && c <= 90)
		c += 32;
	return (c);
}

int	toupper(int c)
{
	if (c >= 97 && c <= 122)
		c -= 32;
	return (c);
}
