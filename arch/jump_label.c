#include "linux/jump_label.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

union jump_code_union {
    char code[JUMP_LABEL_NOP_SIZE];
    struct {
        char jump;
        int offset;
    } __attribute__((packed));
};

static void bug_at(unsigned char *ip, int line)
{
    printf("Unexpected op at %pS %s:%d\n", ip, __FILE__, line);
    exit(-1);
}

void arch_jump_label_transform(struct jump_entry *entry,
        enum jump_label_type type)
{
    union jump_code_union code;
    const unsigned char default_nop[] = { STATIC_KEY_INIT_NOP };

    if (type == JUMP_LABEL_ENABLE) {
        if (unlikely(memcmp((void *)entry->code, default_nop, 5)
                    != 0))
            bug_at((void *)entry->code, __LINE__);

        code.jump = 0xe9;
        code.offset = entry->target -
            (entry->code + JUMP_LABEL_NOP_SIZE);
    } else {
        /*
         * We are disabling this jump label. If it is not what
         * we think it is, then something must have gone wrong.
         * If this is the first initialization call, then we
         * are converting the default nop to the ideal nop.
         */
        code.jump = 0xe9;
        code.offset = entry->target -
            (entry->code + JUMP_LABEL_NOP_SIZE);
        if (unlikely(memcmp((void *)entry->code, &code, 5) != 0))
            bug_at((void *)entry->code, __LINE__);
        memcpy(&code, default_nop, JUMP_LABEL_NOP_SIZE);
    }

extern void *text_poke(void *addr, const void *opcode, size_t len);
    text_poke((void *)entry->code, &code, JUMP_LABEL_NOP_SIZE);
}
