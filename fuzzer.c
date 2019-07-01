/*
 * fuzzer.c
 * Copyright (C) 2019 Enno Boland <g@s01.de>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdlib.h>
#include "chelf.h"

enum Mode mode = MODE_DRY;
int new_stack_size = -1;

int err(const char *what, const char *why)
{
	/* NOOP */
	return -1;
}

int LLVMFuzzerTestOneInput(char *data, size_t size) {
	process_elf64("/path", data, size);
  return 0;  // Non-zero return values are reserved for future use.
}
