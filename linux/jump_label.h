#ifndef JUMP_LABLE_H
#define JUMP_LABLE_H

#include "linux/inc.h"
#include "arch/jump_label.h"

struct static_key {
    atomic_t enabled;
    union {
        unsigned long type;
        struct jump_entry *entries;
    };
};

#define JUMP_LABEL_TYPE_FALSE_BRANCH  0UL
#define JUMP_LABEL_TYPE_TRUE_BRANCH   1UL
#define JUMP_LABEL_TYPE_MASK          1UL


enum jump_label_type {
    JUMP_LABEL_DISABLE = 0,
    JUMP_LABEL_ENABLE,
};

static __always_inline bool static_key_unlikely(struct static_key *key)
{
    return arch_static_branch(key);
}

static __always_inline bool static_key_likely(struct static_key *key)
{
    return !arch_static_branch(key);
}

extern void jump_label_init(void);
extern void static_key_enable(struct static_key *key);
extern void static_key_disable(struct static_key *key);
void arch_jump_label_transform(struct jump_entry *entry,
        enum jump_label_type type);

#define STATIC_KEY_INIT_TRUE \
    {.enabled = ATOMIC_INIT(1),\
     {.entries = (void*)JUMP_LABEL_TYPE_TRUE_BRANCH} }
#define STATIC_KEY_INIT_FALSE \
    {.enabled = ATOMIC_INIT(0),\
     {.entries = (void*)JUMP_LABEL_TYPE_FALSE_BRANCH} }

#define STATIC_KEY_INIT STATIC_KEY_INIT_FALSE

static inline bool static_key_enabled(struct static_key *key)
{
    return atomic_read(&key->enabled) > 0;
}

#endif /* JUMP_LABLE_H */
