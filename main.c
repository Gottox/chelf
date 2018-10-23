#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <elf.h>

#include "chstk.h"

int new_stack_size = -1;

void err(const char *msg) {
	if (errno) {
		perror(msg);
	} else {
		fputs(msg, stderr);
		fputc('\n', stderr);
	}
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int fd;
	struct stat st;
	const char *path;
	char *endptr;
	int prot = PROT_READ;
	int open_flags = O_RDONLY;

	if (argc < 2 || argc > 3) {
		fputs(argv[0], stderr);
		err(" [STACKSIZE] FILE");
	} else if (argc == 3) {
		prot |= PROT_WRITE;
		open_flags = O_RDWR;
		new_stack_size = strtol(argv[1], &endptr, 10);
		if (endptr == argv[1] || new_stack_size < 0) {
			err("Invalid stacksize");
		}
	}
	path = argv[argc - 1];

	if ((fd = open(path, open_flags, 0)) < 0 || fstat(fd, &st) < 0) {
		err(path);
	}

	if (st.st_size < sizeof(Elf32_Ehdr)) {
		err("too small to be an ELF file");
	}

	void *elf = mmap(0, st.st_size, prot, MAP_SHARED, fd, 0);
	if(elf == MAP_FAILED) {
		err(path);
	}
	if (memcmp(elf, ELFMAG, 4) != 0)
		err("wrong magic");
	switch(((char *)elf)[4]) {
	case ELFCLASS32:
		process_elf32(elf, st.st_size);
		break;
	case ELFCLASS64:
		process_elf64(elf, st.st_size);
		break;
	default:
		err("Unknown ELF class");
	}

	munmap(elf, st.st_size);

	return EXIT_SUCCESS;
}
