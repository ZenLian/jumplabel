#include "linux/jump_label.h"
#include <stdio.h>
#include <stdlib.h>

extern struct jump_entry __start___jump_table[];
extern struct jump_entry __stop___jump_table[];

static bool static_key_initialized = false;

#define STATIC_KEY_CHECK_USE() do {\
    if (!static_key_initialized) {\
        printf("%s used before call to jump_lable_init\n", __func__);\
        exit(-1);\
    }\
} while (0)

static void jump_label_lock(void)
{
    //TODO
}

static void jump_label_unlock(void)
{
    //TODO
}

static inline struct jump_entry *jump_label_get_entries(struct static_key *key)
{
    return (struct jump_entry *)((unsigned long)key->entries
            & ~JUMP_LABEL_TYPE_MASK);
}

static inline bool jump_label_get_branch_default(struct static_key *key)
{
    if (((unsigned long)key->entries & JUMP_LABEL_TYPE_MASK) ==
            JUMP_LABEL_TYPE_TRUE_BRANCH)
        return true;
    return false;
}

static void __jump_label_update(struct static_key *key,
        struct jump_entry *entry,
        struct jump_entry *stop, int enable)
{
}

static void jump_label_update(struct static_key *key, int enable)
{
    struct jump_entry *stop = __stop___jump_table;
    struct jump_entry *entry = jump_label_get_entries(key);
    /* if there are no users, entry can be NULL */
    if (entry) {
        for (; (entry < stop) &&
                (entry->key == (jump_label_t)(unsigned long)key);
                entry++) {
            arch_jump_label_transform(entry, enable);
        }
    }
}

// TODO: LOCKING conditions to be confirmed
void static_key_slow_inc(struct static_key *key)
{
    int v, v1;
    STATIC_KEY_CHECK_USE();

    for (v = atomic_read(&key->enabled); v > 0; v = v1) {
        v1 = atomic_cmpxchg(&key->enabled, v, v + 1);
        if (likely(v1 == v)) {
            return;
        }
    }

    jump_label_lock();
    if (atomic_read(&key->enabled) == 0) {
        atomic_set(&key->enabled, -1);
        if (!jump_label_get_branch_default(key)) {
            jump_label_update(key, JUMP_LABEL_ENABLE);
        } else {
            jump_label_update(key, JUMP_LABEL_DISABLE);
        }
        /*
         * Ensure that if the above cmpxchg loop observes our positive
         * value, it must also observe all the text changes.
         */
        atomic_set_release(&key->enabled, 1);
    } else {
        atomic_inc(&key->enabled);
    }
    jump_label_unlock();
}

// TODO: LOCKING conditions to be confirmed
void static_key_slow_dec(struct static_key *key)
{
    STATIC_KEY_CHECK_USE();
    jump_label_lock();
    if (!jump_label_get_branch_default(key))
        jump_label_update(key, JUMP_LABEL_DISABLE);
    else
        jump_label_update(key, JUMP_LABEL_ENABLE);
    atomic_dec(&key->enabled);
    jump_label_unlock();
}

static int jump_label_cmp(const void *a, const void *b)
{
    const struct jump_entry *jea = a;
    const struct jump_entry *jeb = b;

    if (jea->key < jeb->key)
        return -1;

    if (jea->key > jeb->key)
        return 1;

    return 0;
}

static void jump_label_sort_entries(struct jump_entry *start, struct jump_entry *stop)
{
    unsigned long size;

    size = (((unsigned long)stop - (unsigned long)start)
            / sizeof(struct jump_entry));
    printf("jump table size: %lu\n", size);
    qsort(start, size, sizeof(struct jump_entry), jump_label_cmp);
}

static void show_entry(struct jump_entry *iter)
{
    static int cnt = 0;
    cnt++;
    printf("entry %d at 0x%08lx, code=0x%08lx, target=0x%08lx, key=0x%08lx\n",
            cnt, (unsigned long)iter, iter->code, iter->target, iter->key);
}

void jump_label_init(void)
{
    struct jump_entry *iter_start = __start___jump_table;
    struct jump_entry *iter_stop = __stop___jump_table;
    struct static_key *key = NULL;
    struct jump_entry *iter;

    jump_label_lock();
    jump_label_sort_entries(iter_start, iter_stop);

    for (iter = iter_start; iter < iter_stop; iter++) {
        struct static_key *iterk;

        iterk = (struct static_key *)(unsigned long)iter->key;
        //show_entry(iter);
        //arch_jump_label_transform_static(iter, jump_label_type(iterk));
        if (iterk == key)
            continue;

        key = iterk;
        /*
         * Set key->entries to iter, but preserve JUMP_LABEL_TRUE_BRANCH.
         */
        *((unsigned long *)&key->entries) += (unsigned long)iter;
    }
    static_key_initialized = true;
    jump_label_unlock();
}


void static_key_enable(struct static_key *key)
{
    STATIC_KEY_CHECK_USE();
    if (atomic_read(&key->enabled) > 0) {
        //WARN_ON_ONCE(atomic_read(&key->enabled) != 1);
        return;
    }

    jump_label_lock();
    if (atomic_read(&key->enabled) == 0) {
        atomic_set(&key->enabled, -1);
        if (!jump_label_get_branch_default(key))
            jump_label_update(key, JUMP_LABEL_ENABLE);
        else
            jump_label_update(key, JUMP_LABEL_DISABLE);
        atomic_set_release(&key->enabled, 1);
    }
    jump_label_unlock();
}

void static_key_disable(struct static_key *key)
{
    STATIC_KEY_CHECK_USE();

    if (atomic_read(&key->enabled) != 1) {
        //WARN_ON_ONCE(atomic_read(&key->enabled) != 0);
        return;
    }

    jump_label_lock();
    if (atomic_cmpxchg(&key->enabled, 1, 0)) {
        if (!jump_label_get_branch_default(key))
            jump_label_update(key, JUMP_LABEL_DISABLE);
        else
            jump_label_update(key, JUMP_LABEL_ENABLE);
    }
    jump_label_unlock();
}

