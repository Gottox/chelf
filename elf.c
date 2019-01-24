#include <stdio.h>
#include <elf.h>

#include "chelf.h"

#ifdef USE_ELF_32
#	define PROCESS_FUNC process_elf32
#	define MEMSZ_FLAG "%u"

typedef Elf32_Ehdr ElfN_Ehdr;
typedef Elf32_Phdr ElfN_Phdr;
typedef Elf32_Shdr ElfN_Shdr;
#else
#	define PROCESS_FUNC process_elf64
#	if __WORDSIZE == 64
#		define MEMSZ_FLAG "%lu"
#	else
#		define MEMSZ_FLAG "%llu"
#	endif

typedef Elf64_Ehdr ElfN_Ehdr;
typedef Elf64_Phdr ElfN_Phdr;
typedef Elf64_Shdr ElfN_Shdr;
#endif

static int process_gnu_stack(void *elf, size_t size, ElfN_Phdr *header)
{
	if (mode == MODE_READ) {
		printf(" STACKSIZE=" MEMSZ_FLAG, header->p_memsz);
	} else if (new_stack_size >= 0) {
		header->p_memsz = new_stack_size;
	}
	return 0;
}

static int process_interp(void *elf, size_t size, ElfN_Phdr *header)
{
	if (mode == MODE_READ && header->p_memsz > 0) {
		fputs(" INTERP=", stdout);
		if(header->p_offset + header->p_memsz >= size) {
			fputs(" INVALID", stdout);
			return -1;
		}
		fwrite(elf + header->p_offset, header->p_memsz - 1,
				sizeof(char), stdout);
	}
	return 0;
}

static int process_phead(void *elf, size_t size, ElfN_Phdr *header)
{
	switch (header->p_type) {
	case PT_GNU_STACK:
		return process_gnu_stack(elf, size, header);
	case PT_INTERP:
		return process_interp(elf, size, header);
	default:
		return 0;
	}
}

static int get_phead_count(const char *path, void *elf, size_t size,
		ElfN_Ehdr *head)
{
	if (head->e_phnum != PN_XNUM) {
		return head->e_phnum;
	}

	if (head->e_shnum < 1) {
		return err(path, "Expected at least one Section Header");
	} else if (head->e_shoff + sizeof(ElfN_Shdr) > size) {
		return err(path, "EOF while accessing Section Headers");
	}

	ElfN_Shdr *section = elf + head->e_shoff;
	return section->sh_info;
}

int PROCESS_FUNC(const char *path, void *elf, size_t size)
{
	int i, off, phnum, rv = 0;
	ElfN_Ehdr *head = elf;

	phnum = get_phead_count(path, elf, size, head);
	if (phnum < 0)
		return -1;

	off = head->e_phoff;
	if (mode == MODE_READ) {
		fputs(path, stdout);
		fputc(':', stdout);
	}
	for (i = 0; i < phnum && rv == 0; i++, off += head->e_phentsize) {
		if (off + sizeof(ElfN_Phdr) > size) {
			rv = err(path, "EOF while accessing Program Headers");
			continue;
		}

		rv = process_phead(elf, size, elf + off);
	}

	if (mode == MODE_READ) {
		fputc('\n', stdout);
	}


	return rv;
}
