#ifndef CHSTK_H
#define CHSTK_H

extern int new_stack_size;

void err(const char *msg);
int process_elf64(void *, size_t);
int process_elf32(void *, size_t);

#endif /* !CHSTK_H */
