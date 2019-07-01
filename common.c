#include <stdio.h>

#include "chelf.h"

enum Mode mode = MODE_READ;
int new_stack_size = -1;

int err(const char *what, const char *why)
{
	if (why == NULL) {
		perror(what);
	} else {
		fputs(what, stderr);
		fputs(": ", stderr);
		fputs(why, stderr);
		fputc('\n', stderr);
	}
	return -1;
}

