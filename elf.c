#include <stdio.h>
#include <elf.h>

#include "chstk.h"

#ifdef USE_ELF_CLASS_32
#	define PROCESS_FUNC process_elf32
#	define MEMSZ_FLAG "%u"
typedef Elf32_Ehdr ElfN_Ehdr;
typedef Elf32_Phdr ElfN_Phdr;
typedef Elf32_Shdr ElfN_Shdr;
#else
#	define PROCESS_FUNC process_elf64
#	define MEMSZ_FLAG "%lu"
typedef Elf64_Ehdr ElfN_Ehdr;
typedef Elf64_Phdr ElfN_Phdr;
typedef Elf64_Shdr ElfN_Shdr;
#endif

static int process_prog_header(ElfN_Phdr *header) {
	switch(header->p_type) {
	case PT_GNU_STACK:
		if(new_stack_size >= 0)
			header->p_memsz = new_stack_size;
		else
			printf(MEMSZ_FLAG "\n", header->p_memsz);
	}
	return 0;
}

int PROCESS_FUNC (void *elf, size_t size) {
	ElfN_Ehdr *head = elf;

	int phnum = head->e_phnum;
	if (phnum == PN_XNUM) {
		if (head->e_shnum < 1) {
			err("Expected at least one Section Header");
		} else if (head->e_shoff + sizeof(ElfN_Shdr) > size) {
			err("EOF while accessing Section Headers");
		}
		ElfN_Shdr *section = elf + head->e_shoff;
		phnum = section->sh_info;
	}

	int off = head->e_phoff;
	for (int i = 0; i < phnum; i++, off += head->e_phentsize) {
		if (off + sizeof(ElfN_Phdr) > size) {
			err("EOF while accessing Program Headers");
		}
		ElfN_Phdr *prog_head = elf + off;
		process_prog_header(prog_head);
	}

	return 0;
}
