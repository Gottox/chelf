CFLAGS += -Wall -Werror

.PHONY: clean all
all: chstk

elf64.o: elf.c chstk.h
	$(CC) -c $(CFLAGS) $(LDFLAGS) $< -o $@

elf32.o: elf.c chstk.h
	$(CC) -c -DUSE_ELF_32 $(CFLAGS) $(LDFLAGS) $< -o $@

chstk: main.c elf64.o elf32.o chstk.h
	$(CC) $(CFLAGS) $(LDFLAGS) elf64.o elf32.o $< -o $@


clean:
	rm -f elf64.o elf32.o chstk
