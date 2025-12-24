#include <stdio.h>
#include <string.h>
#include "../arena/arena_chain.h"

int main(void) {
    arena_chain_t *arena = ac_init(64);
    if (!arena) {
        printf("init failed\n");
        return 1;
    }

    /* Step 1: allocate persistent memory */
    char *p = ac_get_memory(arena, 32);
    if (!p) {
        printf("allocation failed\n");
        return 1;
    }

    /* Write known pattern */
    memset(p, 'A', 31);
    p[31] = '\0';

    printf("Initial p contents: %s\n", p);

    /* Step 2: force allocator stress */
    for (int i = 0; i < 1000; i++) {
        void *tmp = ac_get_memory(arena, 1024);
        if (!tmp) {
            printf("stress allocation failed at %d\n", i);
            break;
        }
    }

    /* Step 3: verify old pointer */
    printf("After stress p contents: %s\n", p);

    /* Check integrity */
    int ok = 1;
    for (int i = 0; i < 31; i++) {
        if (p[i] != 'A') {
            ok = 0;
            break;
        }
    }

    if (ok)
        printf("\n✅ PASS (memory still valid)\n");
    else
        printf("\n❌ FAIL (memory corrupted or freed)\n");

    ac_destroy(arena);
    return 0;
}
