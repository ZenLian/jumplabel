#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void show_opcode(const void *addr, const void *opcode, size_t len)
{
    size_t i;
    printf("len: %lu\n", len);
    printf("origin: ");
    for (i = 0; i < len; i++) {
        printf("%02x", ((uint8_t*)addr)[i]);
    }
    printf("\n");
    printf("opcode: ");
    for (i = 0; i < len; i++) {
        printf("%02x", ((uint8_t*)opcode)[i]);
    }
    printf("\n");
}

void *text_poke(void *addr, const void *opcode, size_t len)
{
    int pagesize;
    void *base;

    pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1)
        handle_error("sysconf");

    base = (void *)(((uintptr_t)addr) & ~(pagesize - 1));
    //printf("page start at: 0x%lx\n", (uintptr_t)base);

    if (mprotect(base, pagesize,
                PROT_READ | PROT_WRITE | PROT_EXEC) == -1)
        handle_error("mprotect");

    //show_opcode(addr, opcode, len);
    memcpy(addr, opcode, len);

    return addr;
}

