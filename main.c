#include <stdio.h>
#include <stdlib.h>
#include "linux/jump_label.h"

#define ASSERT_EQAULT(a, b) do {\
    if (a != b) {\
        printf("%s[%d]: expect %d, got %d\n", __func__, __LINE__, b, a);\
        exit(-1);\
    }\
} while (0)

struct static_key fkey = STATIC_KEY_INIT_FALSE;
struct static_key tkey = STATIC_KEY_INIT_TRUE;

const int kLikely = 42;
const int kUnlikely = 24;

int check_fkey1(void)
{
    int val;
    if (static_key_unlikely(&fkey)) {
        val = kUnlikely;
    } else {
        val = kLikely;
    }
    return val;
}

int check_tkey1(void)
{
    int val;
    if (static_key_likely(&tkey)) {
        val = kLikely;
    } else {
        val = kUnlikely;
    }
    return val;
}


int check_fkey2(void)
{
    int val;
    if (static_key_unlikely(&fkey)) {
        val = kUnlikely;
    } else {
        val = kLikely;
    }
    return val;
}

int check_fkey3(void)
{
    int val;
    if (static_key_unlikely(&fkey)) {
        val = kUnlikely;
    } else {
        val = kLikely;
    }
    return val;
}
int check_tkey2(void)
{
    int val;
    if (static_key_likely(&tkey)) {
        val = kLikely;
    } else {
        val = kUnlikely;
    }
    return val;
}

int check_tkey3(void)
{
    int val;
    if (static_key_likely(&tkey)) {
        val = kLikely;
    } else {
        val = kUnlikely;
    }
    return val;
}

int main(int argc, char *argv[])
{
    jump_label_init();

    printf("\n=== init status...\n");
    ASSERT_EQAULT(check_fkey1(), kLikely);
    ASSERT_EQAULT(check_fkey2(), kLikely);
    ASSERT_EQAULT(check_fkey3(), kLikely);
    ASSERT_EQAULT(check_tkey1(), kLikely);
    ASSERT_EQAULT(check_tkey2(), kLikely);
    ASSERT_EQAULT(check_tkey3(), kLikely);

    printf("\n=== enable fkey...\n");
    static_key_enable(&fkey);
    ASSERT_EQAULT(check_fkey1(), kUnlikely);
    ASSERT_EQAULT(check_fkey2(), kUnlikely);
    ASSERT_EQAULT(check_fkey3(), kUnlikely);
    ASSERT_EQAULT(check_tkey1(), kLikely);
    ASSERT_EQAULT(check_tkey2(), kLikely);
    ASSERT_EQAULT(check_tkey3(), kLikely);

    printf("\n=== disable fkey...\n");
    static_key_disable(&fkey);
    ASSERT_EQAULT(check_fkey1(), kLikely);
    ASSERT_EQAULT(check_fkey2(), kLikely);
    ASSERT_EQAULT(check_fkey3(), kLikely);
    ASSERT_EQAULT(check_tkey1(), kLikely);
    ASSERT_EQAULT(check_tkey2(), kLikely);
    ASSERT_EQAULT(check_tkey3(), kLikely);

    printf("\n=== disable tkey...\n");
    static_key_disable(&tkey);
    ASSERT_EQAULT(check_fkey1(), kLikely);
    ASSERT_EQAULT(check_fkey2(), kLikely);
    ASSERT_EQAULT(check_fkey3(), kLikely);
    ASSERT_EQAULT(check_tkey1(), kUnlikely);
    ASSERT_EQAULT(check_tkey2(), kUnlikely);
    ASSERT_EQAULT(check_tkey3(), kUnlikely);

    printf("\n=== enable tkey...\n");
    static_key_enable(&tkey);
    ASSERT_EQAULT(check_fkey1(), kLikely);
    ASSERT_EQAULT(check_fkey2(), kLikely);
    ASSERT_EQAULT(check_fkey3(), kLikely);
    ASSERT_EQAULT(check_tkey1(), kLikely);
    ASSERT_EQAULT(check_tkey2(), kLikely);
    ASSERT_EQAULT(check_tkey3(), kLikely);

    printf("\n=== test pass ===\n");

    return 0;
}
