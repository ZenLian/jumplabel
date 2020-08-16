#ifndef LINUX_INC_H
#define LINUX_INC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#ifndef __always_inline
#define __always_inline __attribute__((always_inline))
#endif

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif /* likely */
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif /* unlikely */

#ifndef asm_volatile_goto
#define asm_volatile_goto(x) do { asm goto(x); asm (""); } while (0)
#endif /* ifndef asm_volatile_goto(x) */

#define __stringify_1(x...) #x
#define __stringify(x...) __stringify_1(x)

#define _ASM_PTR   " .quad "
#define _ASM_ALIGN " .balign 8 "

typedef int atomic_t;

#define ATOMIC_INIT(x) (x)

static inline int atomic_read(atomic_t *v)
{
    return *(volatile int *)v;
}

static inline void atomic_set(atomic_t *v, int i)
{
    *(volatile int *)v = i;
}

static inline void atomic_set_release(atomic_t *v, int i)
{
    atomic_set(v, i);
    __sync_synchronize();
}

#define atomic_inc(v) __sync_fetch_and_add(v, 1)
#define atomic_dec(v) __sync_fetch_and_sub(v, 1)
#define atomic_cmpxchg(v, old, new) __sync_val_compare_and_swap(v, old, new)



#endif
