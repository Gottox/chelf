#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <elf.h>
#include <unistd.h>

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

static int process_file(const char *path)
{
	int fd, rv = -1;
	struct stat st;
	void *elf = NULL;
	const int open_flags = (mode == MODE_WRITE) ? O_RDWR : O_RDONLY;
	const int mmap_flags = (mode == MODE_WRITE) ?
				(PROT_WRITE|PROT_READ) : PROT_READ;

	/* open and map the elf file */

	if ((fd = open(path, open_flags, 0)) < 0 || fstat(fd, &st) < 0) {
		rv = err(path, NULL);
		goto cleanup;
	}

	if (st.st_size < sizeof(Elf32_Ehdr)) {
		rv = err(path, "too small to be an ELF file");
		goto cleanup;
	}

	elf = mmap(0, st.st_size, mmap_flags, MAP_SHARED, fd, 0);
	if (elf == MAP_FAILED) {
		rv = err(path, NULL);
		goto cleanup;
	}

	/* validation and parsing */

	if (memcmp(elf, ELFMAG, 4) != 0) {
		rv = err(path, "wrong ELF magic");
		goto cleanup;
	}
	switch (((char *)elf)[4]) {
		case ELFCLASS32:
			rv = process_elf32(path, elf, st.st_size);
			break;
		case ELFCLASS64:
			rv = process_elf64(path, elf, st.st_size);
			break;
		default:
			rv = err(path, "Unknown ELF class");
	}

cleanup:

	if (elf != NULL) {
		rv |= munmap(elf, st.st_size);
	}
	if (fd >= 0) {
		rv |= close(fd);
	}
	return rv;
}

static int usage(const char *arg0)
{
	fputs(arg0, stderr);
	fputs(" [-s STACKSIZE] FILE...", stderr);
	return -1;
}

int main(int argc, char *argv[])
{
	int opt, rv = 0;
	char *endptr;

	/* command line parsing */

	while ((opt = getopt(argc, argv, "vs:")) != -1) {
		switch(opt) {
		case 's':
			mode = MODE_WRITE;
			new_stack_size = strtol(optarg, &endptr, 10);
			if (endptr == optarg || new_stack_size < 0) {
				return -err(argv[0], "Invalid stacksize");
			}
			break;
		case 'v':
			fputs(argv[0], stderr);
			fputs("-" VERSION "\n", stderr);
			return EXIT_SUCCESS;
		default:
			return usage(argv[0]);
		}
	}

	if(optind == argc) {
		return usage(argv[0]);
	}

	for(; optind < argc; optind++) {
		rv |= process_file(argv[optind]);
	}

	return rv ? EXIT_FAILURE : EXIT_SUCCESS;
}
