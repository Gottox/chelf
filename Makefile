override CFLAGS += -Wall -Werror

.PHONY: clean all

all: chelf

elf64.o: elf.c chelf.h
	$(CC) -c $(CFLAGS) $(LDFLAGS) $< -o $@

elf32.o: elf.c chelf.h
	$(CC) -c -DUSE_ELF_32 $(CFLAGS) $(LDFLAGS) $< -o $@

chelf: main.c elf64.o elf32.o chelf.h
	$(CC) $(CFLAGS) $(LDFLAGS) elf64.o elf32.o $< -o $@


clean:
	rm -f elf64.o elf32.o chelf
