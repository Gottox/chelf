#ifndef CHELF_H
#define CHELF_H

#define VERSION "0.2.2"

enum Mode {
	MODE_READ,
	MODE_WRITE,
};
extern enum Mode mode;
extern int new_stack_size;

int err(const char *what, const char *why);
int process_elf64(const char *, void *, size_t);
int process_elf32(const char *, void *, size_t);

#endif /* !CHELF_H */
