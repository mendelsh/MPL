#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../arena/arena_chain.h"

#define TEST_PASS printf("✅ PASS: %s\n", __func__)
#define TEST_FAIL printf("❌ FAIL: %s - line %d\n", __func__, __LINE__)

/* Test 1: Basic allocation and integrity */
int test_basic_allocation() {
    arena_chain_t *arena = ac_init(128);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    int *nums = ac_get_memory(arena, sizeof(int) * 10);
    if (!nums) {
        ac_destroy(arena);
        TEST_FAIL;
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        nums[i] = i * 100;
    }

    for (int i = 0; i < 10; i++) {
        if (nums[i] != i * 100) {
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

/* Test 2: Multiple allocations persistence */
int test_multiple_allocations() {
    arena_chain_t *arena = ac_init(256);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    char *str1 = ac_get_memory(arena, 50);
    int *nums = ac_get_memory(arena, sizeof(int) * 20);
    char *str2 = ac_get_memory(arena, 100);
    double *doubles = ac_get_memory(arena, sizeof(double) * 10);

    if (!str1 || !nums || !str2 || !doubles) {
        ac_destroy(arena);
        TEST_FAIL;
        return 0;
    }

    strcpy(str1, "First string allocation");
    for (int i = 0; i < 20; i++) nums[i] = i;
    strcpy(str2, "Second string allocation with more text");
    for (int i = 0; i < 10; i++) doubles[i] = i * 3.14159;

    if (strcmp(str1, "First string allocation") != 0) {
        ac_destroy(arena);
        TEST_FAIL;
        return 0;
    }

    for (int i = 0; i < 20; i++) {
        if (nums[i] != i) {
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }
    }

    if (strcmp(str2, "Second string allocation with more text") != 0) {
        ac_destroy(arena);
        TEST_FAIL;
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        if (doubles[i] < (i * 3.14159 - 0.0001) || doubles[i] > (i * 3.14159 + 0.0001)) {
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

/* Test 3: Bucket chain growth */
int test_bucket_growth() {
    arena_chain_t *arena = ac_init(64);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    #define NUM_ALLOCS 100
    char *ptrs[NUM_ALLOCS];

    for (int i = 0; i < NUM_ALLOCS; i++) {
        ptrs[i] = ac_get_memory(arena, 128);
        if (!ptrs[i]) {
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }
        memset(ptrs[i], 'A' + (i % 26), 127);
        ptrs[i][127] = '\0';
    }

    for (int i = 0; i < NUM_ALLOCS; i++) {
        char expected = 'A' + (i % 26);
        for (int j = 0; j < 127; j++) {
            if (ptrs[i][j] != expected) {
                ac_destroy(arena);
                TEST_FAIL;
                return 0;
            }
        }
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

/* Test 4: Clear and reuse */
int test_clear_reuse() {
    arena_chain_t *arena = ac_init(256);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    for (int round = 0; round < 5; round++) {
        int *data = ac_get_memory(arena, sizeof(int) * 50);
        if (!data) {
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }

        for (int i = 0; i < 50; i++) {
            data[i] = round * 1000 + i;
        }

        for (int i = 0; i < 50; i++) {
            if (data[i] != round * 1000 + i) {
                ac_destroy(arena);
                TEST_FAIL;
                return 0;
            }
        }

        ac_clear(arena);
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

/* Test 5: Alignment check */
int test_alignment() {
    arena_chain_t *arena = ac_init(1024);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    for (int i = 0; i < 100; i++) {
        void *ptr = ac_get_memory(arena, 1 + (i % 17));
        if (!ptr) {
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }

        if (((uintptr_t)ptr) % alignof(max_align_t) != 0) {
            printf("Misaligned pointer at iteration %d: %p\n", i, ptr);
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

/* Test 6: Large allocation */
int test_large_allocation() {
    arena_chain_t *arena = ac_init(128);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    size_t large_size = 1024 * 1024;
    char *large = ac_get_memory(arena, large_size);
    if (!large) {
        ac_destroy(arena);
        TEST_FAIL;
        return 0;
    }

    for (size_t i = 0; i < large_size; i += 4096) {
        large[i] = (char)(i % 256);
    }

    for (size_t i = 0; i < large_size; i += 4096) {
        if (large[i] != (char)(i % 256)) {
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

/* Test 7: Interleaved allocations stress test */
int test_interleaved_stress() {
    arena_chain_t *arena = ac_init(512);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    #define STRESS_ALLOCS 500
    void *ptrs[STRESS_ALLOCS];
    size_t sizes[STRESS_ALLOCS];

    for (int i = 0; i < STRESS_ALLOCS; i++) {
        sizes[i] = (i % 200) + 1;
        ptrs[i] = ac_get_memory(arena, sizes[i]);
        if (!ptrs[i]) {
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }
        memset(ptrs[i], (i % 256), sizes[i]);
    }

    for (int i = 0; i < STRESS_ALLOCS; i++) {
        unsigned char *ptr = (unsigned char *)ptrs[i];
        for (size_t j = 0; j < sizes[i]; j++) {
            if (ptr[j] != (unsigned char)(i % 256)) {
                printf("Corruption at allocation %d, byte %zu\n", i, j);
                ac_destroy(arena);
                TEST_FAIL;
                return 0;
            }
        }
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

/* Test 8: Zero-size edge case */
int test_zero_size() {
    arena_chain_t *arena = ac_init(256);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    void *ptr = ac_get_memory(arena, 0);
    
    char *valid = ac_get_memory(arena, 10);
    if (!valid) {
        ac_destroy(arena);
        TEST_FAIL;
        return 0;
    }
    strcpy(valid, "test");

    if (strcmp(valid, "test") != 0) {
        ac_destroy(arena);
        TEST_FAIL;
        return 0;
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

/* Test 9: Sequential clear operations */
int test_sequential_clears() {
    arena_chain_t *arena = ac_init(1024);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    for (int cycle = 0; cycle < 10; cycle++) {
        for (int i = 0; i < 20; i++) {
            int *nums = ac_get_memory(arena, sizeof(int) * 100);
            if (!nums) {
                ac_destroy(arena);
                TEST_FAIL;
                return 0;
            }
            for (int j = 0; j < 100; j++) {
                nums[j] = cycle * 10000 + i * 100 + j;
            }
        }
        ac_clear(arena);
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

/* Test 10: Mixed size stress */
int test_mixed_sizes() {
    arena_chain_t *arena = ac_init(256);
    if (!arena) {
        TEST_FAIL;
        return 0;
    }

    size_t test_sizes[] = {1, 7, 8, 15, 16, 31, 32, 63, 64, 127, 128, 255, 256, 
                           511, 512, 1023, 1024, 2047, 2048, 4095, 4096};
    int num_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        char *ptr = ac_get_memory(arena, test_sizes[i]);
        if (!ptr) {
            ac_destroy(arena);
            TEST_FAIL;
            return 0;
        }
        
        memset(ptr, 'X', test_sizes[i]);
        
        for (size_t j = 0; j < test_sizes[i]; j++) {
            if (ptr[j] != 'X') {
                printf("Corruption at size %zu, byte %zu\n", test_sizes[i], j);
                ac_destroy(arena);
                TEST_FAIL;
                return 0;
            }
        }
    }

    ac_destroy(arena);
    TEST_PASS;
    return 1;
}

int main(void) {
    printf("=== Arena Chain Comprehensive Test Suite ===\n\n");

    int passed = 0;
    int total = 10;

    passed += test_basic_allocation();
    passed += test_multiple_allocations();
    passed += test_bucket_growth();
    passed += test_clear_reuse();
    passed += test_alignment();
    passed += test_large_allocation();
    passed += test_interleaved_stress();
    passed += test_zero_size();
    passed += test_sequential_clears();
    passed += test_mixed_sizes();

    printf("\n=== Results: %d/%d tests passed ===\n", passed, total);

    if (passed == total) {
        printf("🎉 All tests passed!\n");
        return 0;
    } else {
        printf("⚠️  Some tests failed\n");
        return 1;
    }
}