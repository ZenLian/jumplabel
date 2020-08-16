#ifndef ARCH_JUMP_LABEL_H
#define ARCH_JUMP_LABEL_H

#include "linux/inc.h"

#include "linux/jump_label.h"

#define JUMP_LABEL_NOP_SIZE 5
#define STATIC_KEY_INIT_NOP 0x0f,0x1f,0x44,0x00,0

struct static_key;
static __always_inline bool arch_static_branch(struct static_key *key)
{
    asm_volatile_goto("1:"
        ".byte " __stringify(STATIC_KEY_INIT_NOP) "\n\t"
        ".pushsection .jump_table,  \"aw\" \n\t"
        _ASM_ALIGN "\n\t"
        _ASM_PTR "1b, %l[l_yes], %c0 \n\t"
        ".popsection \n\t"
        : :  "i" (key) : : l_yes);
    return false;
l_yes:
    return true;
}

typedef uintptr_t jump_label_t;
struct jump_entry {
    jump_label_t code;
    jump_label_t target;
    jump_label_t key;
};

#endif /* ARCH_JUMP_LABEL_H */
