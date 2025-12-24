#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../arena/arena_chain.h"

/* Test 1: Basic lifecycle - init, use, destroy */
void test_basic_lifecycle() {
    printf("Test 1: Basic lifecycle...\n");
    arena_chain_t *arena = ac_init(256);
    
    char *str = ac_get_memory(arena, 50);
    strcpy(str, "Hello, World!");
    
    int *nums = ac_get_memory(arena, sizeof(int) * 100);
    for (int i = 0; i < 100; i++) {
        nums[i] = i;
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 2: Multiple bucket creation and destruction */
void test_multiple_buckets() {
    printf("Test 2: Multiple buckets...\n");
    arena_chain_t *arena = ac_init(64);
    
    // Force creation of multiple buckets
    for (int i = 0; i < 50; i++) {
        void *ptr = ac_get_memory(arena, 256);
        memset(ptr, 0xAB, 256);
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 3: Clear and reuse multiple times */
void test_clear_reuse() {
    printf("Test 3: Clear and reuse...\n");
    arena_chain_t *arena = ac_init(512);
    
    for (int round = 0; round < 20; round++) {
        for (int i = 0; i < 30; i++) {
            void *ptr = ac_get_memory(arena, 128);
            memset(ptr, round % 256, 128);
        }
        ac_clear(arena);
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 4: Stress test with varying sizes */
void test_stress_varying_sizes() {
    printf("Test 4: Stress test with varying sizes...\n");
    arena_chain_t *arena = ac_init(128);
    
    for (int i = 0; i < 1000; i++) {
        size_t size = 1 + (i % 500);
        void *ptr = ac_get_memory(arena, size);
        if (ptr) {
            memset(ptr, i % 256, size);
        }
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 5: Large allocations */
void test_large_allocations() {
    printf("Test 5: Large allocations...\n");
    arena_chain_t *arena = ac_init(256);
    
    // Allocate several large chunks
    for (int i = 0; i < 5; i++) {
        void *ptr = ac_get_memory(arena, 1024 * 1024);
        if (ptr) {
            // Touch first and last page to ensure allocation
            ((char*)ptr)[0] = 'A';
            ((char*)ptr)[1024 * 1024 - 1] = 'Z';
        }
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 6: Clear after bucket growth */
void test_clear_after_growth() {
    printf("Test 6: Clear after bucket growth...\n");
    arena_chain_t *arena = ac_init(64);
    
    // Grow to multiple buckets
    for (int i = 0; i < 100; i++) {
        void *ptr = ac_get_memory(arena, 200);
        memset(ptr, 0xFF, 200);
    }
    
    // Clear and allocate again
    ac_clear(arena);
    
    for (int i = 0; i < 50; i++) {
        void *ptr = ac_get_memory(arena, 100);
        memset(ptr, 0xEE, 100);
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 7: Interleaved small and large allocations */
void test_interleaved_sizes() {
    printf("Test 7: Interleaved sizes...\n");
    arena_chain_t *arena = ac_init(256);
    
    for (int i = 0; i < 50; i++) {
        // Small allocation
        void *small = ac_get_memory(arena, 16);
        memset(small, 0x11, 16);
        
        // Large allocation
        void *large = ac_get_memory(arena, 4096);
        memset(large, 0x22, 4096);
        
        // Medium allocation
        void *medium = ac_get_memory(arena, 512);
        memset(medium, 0x33, 512);
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 8: Boundary conditions */
void test_boundary_conditions() {
    printf("Test 8: Boundary conditions...\n");
    arena_chain_t *arena = ac_init(128);
    
    // Zero size
    void *zero = ac_get_memory(arena, 0);
    
    // Very small
    void *tiny = ac_get_memory(arena, 1);
    if (tiny) ((char*)tiny)[0] = 'x';
    
    // Power of 2 sizes
    for (int i = 0; i < 16; i++) {
        size_t size = 1 << i; // 1, 2, 4, 8, ... 32768
        void *ptr = ac_get_memory(arena, size);
        if (ptr && size > 0) {
            memset(ptr, 0xDD, size);
        }
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 9: Rapid allocation/clear cycles */
void test_rapid_cycles() {
    printf("Test 9: Rapid allocation/clear cycles...\n");
    arena_chain_t *arena = ac_init(512);
    
    for (int cycle = 0; cycle < 100; cycle++) {
        for (int i = 0; i < 10; i++) {
            void *ptr = ac_get_memory(arena, 256);
            memset(ptr, cycle % 256, 256);
        }
        ac_clear(arena);
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 10: NULL arena handling */
void test_null_handling() {
    printf("Test 10: NULL handling...\n");
    
    // These should not crash
    ac_clear(NULL);
    ac_destroy(NULL);
    
    printf("  ✓ Completed\n");
}

/* Test 11: Memory pattern verification after many operations */
void test_memory_patterns() {
    printf("Test 11: Memory pattern verification...\n");
    arena_chain_t *arena = ac_init(256);
    
    #define PATTERN_ALLOCS 200
    struct {
        void *ptr;
        size_t size;
        unsigned char pattern;
    } allocs[PATTERN_ALLOCS];
    
    // Allocate with unique patterns
    for (int i = 0; i < PATTERN_ALLOCS; i++) {
        allocs[i].size = 10 + (i % 100);
        allocs[i].pattern = (unsigned char)i;
        allocs[i].ptr = ac_get_memory(arena, allocs[i].size);
        
        if (allocs[i].ptr) {
            memset(allocs[i].ptr, allocs[i].pattern, allocs[i].size);
        }
    }
    
    // Verify all patterns are intact
    for (int i = 0; i < PATTERN_ALLOCS; i++) {
        if (allocs[i].ptr) {
            unsigned char *bytes = (unsigned char *)allocs[i].ptr;
            for (size_t j = 0; j < allocs[i].size; j++) {
                if (bytes[j] != allocs[i].pattern) {
                    printf("  ⚠ Pattern mismatch at alloc %d, byte %zu\n", i, j);
                }
            }
        }
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

/* Test 12: Extreme bucket growth */
void test_extreme_growth() {
    printf("Test 12: Extreme bucket growth...\n");
    arena_chain_t *arena = ac_init(32);
    
    // Start tiny and force aggressive growth
    for (int i = 0; i < 30; i++) {
        size_t size = 1024 * (i + 1);
        void *ptr = ac_get_memory(arena, size);
        if (ptr) {
            ((char*)ptr)[0] = 'S';
            ((char*)ptr)[size - 1] = 'E';
        }
    }
    
    ac_destroy(arena);
    printf("  ✓ Completed\n");
}

int main(void) {
    printf("=== Memory Safety Test Suite ===\n");
    printf("Run with: valgrind --leak-check=full --show-leak-kinds=all ./test\n");
    printf("Or compile with: gcc -fsanitize=address -g\n\n");
    
    test_basic_lifecycle();
    test_multiple_buckets();
    test_clear_reuse();
    test_stress_varying_sizes();
    test_large_allocations();
    test_clear_after_growth();
    test_interleaved_sizes();
    test_boundary_conditions();
    test_rapid_cycles();
    test_null_handling();
    test_memory_patterns();
    test_extreme_growth();
    
    printf("\n=== All tests completed ===\n");
    printf("Check Valgrind/ASan output above for any memory issues.\n");
    
    return 0;
}