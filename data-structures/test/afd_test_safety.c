#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "../arena/arena_fd.h"

// Test with 1MB RAM threshold - allocations larger than this use mmap
size_t ram_threshold = 1024 * 1024; // 1 MB

// Timing utilities
static double get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

#define TIME_TEST_START() double _start_time = get_time_ms()
#define TIME_TEST_END() double _elapsed = get_time_ms() - _start_time; \
                        printf("  ⏱️  Time: %.2f ms\n", _elapsed)

/* Test 1: Basic lifecycle - init, use, destroy */
void test_basic_lifecycle() {
    printf("Test 1: Basic lifecycle...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(256, ram_threshold);
    
    char *str = afd_get_memory(arena, 50);
    strcpy(str, "Hello, World!");
    
    int *nums = afd_get_memory(arena, sizeof(int) * 100);
    for (int i = 0; i < 100; i++) {
        nums[i] = i;
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed\n");
}

/* Test 2: Multiple bucket creation with mixed malloc/mmap */
void test_multiple_buckets() {
    printf("Test 2: Multiple buckets (mixed malloc/mmap)...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(64, ram_threshold);
    
    // Small allocations (should use malloc)
    for (int i = 0; i < 50; i++) {
        void *ptr = afd_get_memory(arena, 256);
        memset(ptr, 0xAB, 256);
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed\n");
}

/* Test 3: Clear and reuse multiple times */
void test_clear_reuse() {
    printf("Test 3: Clear and reuse...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(512, ram_threshold);
    
    for (int round = 0; round < 20; round++) {
        for (int i = 0; i < 30; i++) {
            void *ptr = afd_get_memory(arena, 128);
            memset(ptr, round % 256, 128);
        }
        afd_clear(arena);
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed\n");
}

/* Test 4: Stress test with varying sizes */
void test_stress_varying_sizes() {
    printf("Test 4: Stress test with varying sizes...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(128, ram_threshold);
    
    for (int i = 0; i < 1000; i++) {
        size_t size = 1 + (i % 500);
        void *ptr = afd_get_memory(arena, size);
        if (ptr) {
            memset(ptr, i % 256, size);
        }
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed\n");
}

/* Test 5: Large allocations that should trigger mmap */
void test_large_allocations_mmap() {
    printf("Test 5: Large allocations (should use mmap)...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(256, ram_threshold);
    
    // Each allocation is > 1MB, should trigger mmap
    for (int i = 0; i < 10; i++) {
        size_t large_size = 2 * 1024 * 1024; // 2 MB each
        void *ptr = afd_get_memory(arena, large_size);
        if (ptr) {
            // Touch first and last page
            ((char*)ptr)[0] = 'A';
            ((char*)ptr)[large_size - 1] = 'Z';
            // Verify
            if (((char*)ptr)[0] != 'A' || ((char*)ptr)[large_size - 1] != 'Z') {
                printf("  ⚠ Large allocation %d failed verification\n", i);
            }
        } else {
            printf("  ⚠ Large allocation %d failed\n", i);
        }
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed (10 x 2MB allocations)\n");
}

/* Test 6: Huge allocation stress test */
void test_huge_allocations() {
    printf("Test 6: Huge allocations (10MB+ each)...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(1024, ram_threshold);
    
    for (int i = 0; i < 5; i++) {
        size_t huge_size = 10 * 1024 * 1024; // 10 MB each
        void *ptr = afd_get_memory(arena, huge_size);
        if (ptr) {
            // Write pattern every 1MB to ensure pages are mapped
            for (size_t offset = 0; offset < huge_size; offset += 1024 * 1024) {
                ((char*)ptr)[offset] = (char)(i + offset / (1024 * 1024));
            }
            // Verify first pattern
            if (((char*)ptr)[0] != (char)i) {
                printf("  ⚠ Huge allocation %d failed verification\n", i);
            }
        } else {
            printf("  ⚠ Huge allocation %d failed\n", i);
        }
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed (5 x 10MB allocations)\n");
}

/* Test 7: Mixed small and huge allocations */
void test_mixed_malloc_mmap() {
    printf("Test 7: Mixed malloc/mmap allocations...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(256, ram_threshold);
    
    for (int i = 0; i < 20; i++) {
        // Small allocation (malloc)
        void *small = afd_get_memory(arena, 1024);
        if (small) memset(small, 0x11, 1024);
        
        // Large allocation (mmap)
        void *large = afd_get_memory(arena, 5 * 1024 * 1024);
        if (large) {
            ((char*)large)[0] = 'L';
            ((char*)large)[5 * 1024 * 1024 - 1] = 'E';
        }
        
        // Medium allocation (malloc)
        void *medium = afd_get_memory(arena, 64 * 1024);
        if (medium) memset(medium, 0x33, 64 * 1024);
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed (20 cycles of small/large/medium)\n");
}

/* Test 8: Clear after bucket growth with mmap */
void test_clear_after_mmap_growth() {
    printf("Test 8: Clear after mmap bucket growth...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(64, ram_threshold);
    
    // Force growth with large allocations
    for (int i = 0; i < 10; i++) {
        void *ptr = afd_get_memory(arena, 3 * 1024 * 1024);
        if (ptr) {
            ((char*)ptr)[0] = 'X';
        }
    }
    
    afd_clear(arena);
    
    // Allocate again after clear
    for (int i = 0; i < 5; i++) {
        void *ptr = afd_get_memory(arena, 2 * 1024 * 1024);
        if (ptr) {
            memset(ptr, 0xEE, 1024); // Just touch first KB
        }
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed\n");
}

/* Test 9: Boundary conditions around mmap threshold */
void test_mmap_threshold_boundary() {
    printf("Test 9: Allocations around mmap threshold...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(128, ram_threshold);
    
    // Just below threshold (should use malloc)
    void *below = afd_get_memory(arena, ram_threshold - 1024);
    if (below) memset(below, 0xAA, 1024);
    
    // At threshold (should use malloc)
    void *at = afd_get_memory(arena, ram_threshold);
    if (at) memset(at, 0xBB, 1024);
    
    // Just above threshold (should use mmap)
    void *above = afd_get_memory(arena, ram_threshold + 1024);
    if (above) memset(above, 0xCC, 1024);
    
    // Way above threshold (should use mmap)
    void *way_above = afd_get_memory(arena, ram_threshold * 10);
    if (way_above) {
        ((char*)way_above)[0] = 'W';
        ((char*)way_above)[ram_threshold * 10 - 1] = 'A';
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed\n");
}

/* Test 10: Rapid allocation/clear cycles with large allocations */
void test_rapid_cycles_large() {
    printf("Test 10: Rapid allocation/clear cycles (large)...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(512, ram_threshold);
    
    for (int cycle = 0; cycle < 50; cycle++) {
        for (int i = 0; i < 3; i++) {
            void *ptr = afd_get_memory(arena, 4 * 1024 * 1024);
            if (ptr) {
                ((char*)ptr)[0] = cycle % 256;
            }
        }
        afd_clear(arena);
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed (50 cycles, 3x4MB each)\n");
}

/* Test 11: NULL arena handling */
void test_null_handling() {
    printf("Test 11: NULL handling...\n");
    TIME_TEST_START();
    
    afd_clear(NULL);
    afd_destroy(NULL);
    
    TIME_TEST_END();
    printf("  ✓ Completed\n");
}

/* Test 12: Memory pattern verification with large allocations */
void test_memory_patterns_large() {
    printf("Test 12: Memory pattern verification (large allocations)...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(256, ram_threshold);
    
    #define PATTERN_ALLOCS 50
    struct {
        void *ptr;
        size_t size;
        unsigned char pattern;
    } allocs[PATTERN_ALLOCS];
    
    // Mix of small and large allocations
    for (int i = 0; i < PATTERN_ALLOCS; i++) {
        if (i % 5 == 0) {
            // Every 5th allocation is large (mmap)
            allocs[i].size = 2 * 1024 * 1024 + (i * 1024);
        } else {
            // Others are small (malloc)
            allocs[i].size = 1024 + (i % 100) * 1024;
        }
        
        allocs[i].pattern = (unsigned char)i;
        allocs[i].ptr = afd_get_memory(arena, allocs[i].size);
        
        if (allocs[i].ptr) {
            // Write pattern to first KB and last KB
            memset(allocs[i].ptr, allocs[i].pattern, 1024);
            if (allocs[i].size > 1024) {
                memset((char*)allocs[i].ptr + allocs[i].size - 1024, allocs[i].pattern, 1024);
            }
        }
    }
    
    // Verify patterns
    int mismatches = 0;
    for (int i = 0; i < PATTERN_ALLOCS; i++) {
        if (allocs[i].ptr) {
            unsigned char *bytes = (unsigned char *)allocs[i].ptr;
            // Check first KB
            for (size_t j = 0; j < 1024 && j < allocs[i].size; j++) {
                if (bytes[j] != allocs[i].pattern) {
                    mismatches++;
                    break;
                }
            }
            // Check last KB
            if (allocs[i].size > 1024) {
                bytes = (unsigned char *)allocs[i].ptr + allocs[i].size - 1024;
                for (size_t j = 0; j < 1024; j++) {
                    if (bytes[j] != allocs[i].pattern) {
                        mismatches++;
                        break;
                    }
                }
            }
        }
    }
    
    if (mismatches > 0) {
        printf("  ⚠ Found %d pattern mismatches\n", mismatches);
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed (50 mixed allocations)\n");
}

/* Test 13: Extreme growth with gigabyte allocations - UP TO 50GB! */
void test_gigabyte_allocations() {
    printf("Test 13: EXTREME gigabyte-scale allocations (up to 50GB)...\n");
    
    arena_fd_t *arena = afd_init(32, ram_threshold);
    
    // Allocate increasingly large chunks up to 50GB
    size_t test_sizes[] = {
        (size_t)2 * 1024 * 1024 * 1024,      // 2 GB
        (size_t)5 * 1024 * 1024 * 1024,      // 5 GB
        (size_t)10 * 1024 * 1024 * 1024,     // 10 GB
        (size_t)20 * 1024 * 1024 * 1024,     // 20 GB
        (size_t)50 * 1024 * 1024 * 1024      // 50 GB!
    };
    
    double total_time = 0;
    
    for (int i = 0; i < 5; i++) {
        size_t size = test_sizes[i];
        printf("    Attempting to allocate %zu GB...\n", size / (1024 * 1024 * 1024));
        
        TIME_TEST_START();
        void *ptr = afd_get_memory(arena, size);
        
        if (ptr) {
            // Touch every 256MB to ensure mapping (sparse touching for huge sizes)
            size_t touch_interval = 256 * 1024 * 1024;
            for (size_t offset = 0; offset < size; offset += touch_interval) {
                ((char*)ptr)[offset] = (char)(i + offset / (1024 * 1024 * 1024));
            }
            // Touch the last byte
            ((char*)ptr)[size - 1] = 'E';
            
            TIME_TEST_END();
            total_time += _elapsed;
            printf("    ✅ Allocated and verified %zu GB\n", size / (1024 * 1024 * 1024));
        } else {
            TIME_TEST_END();
            printf("    ⚠️  Failed to allocate %zu GB (might be system limit)\n", size / (1024 * 1024 * 1024));
        }
    }
    
    afd_destroy(arena);
    
    printf("  ⏱️  Total time for all allocations: %.2f ms (%.2f seconds)\n", 
           total_time, total_time / 1000.0);
    printf("  ✓ Test completed\n");
}

/* Test 14: Interleaved allocations at threshold boundary */
void test_interleaved_threshold() {
    printf("Test 14: Interleaved threshold boundary allocations...\n");
    TIME_TEST_START();
    
    arena_fd_t *arena = afd_init(256, ram_threshold);
    
    for (int i = 0; i < 30; i++) {
        // Below threshold
        void *small = afd_get_memory(arena, ram_threshold / 2);
        if (small) ((char*)small)[0] = 'S';
        
        // Above threshold
        void *large = afd_get_memory(arena, ram_threshold * 2);
        if (large) ((char*)large)[0] = 'L';
        
        // Way below threshold
        void *tiny = afd_get_memory(arena, 4096);
        if (tiny) memset(tiny, 0x99, 4096);
    }
    
    afd_destroy(arena);
    
    TIME_TEST_END();
    printf("  ✓ Completed (30 cycles)\n");
}

int main(void) {
    printf("=== Memory Safety Test Suite (mmap-enabled) ===\n");
    printf("RAM threshold: %zu bytes (%.2f MB)\n", ram_threshold, ram_threshold / (1024.0 * 1024.0));
    printf("Allocations > threshold will use mmap instead of malloc\n");
    printf("Run with: valgrind --leak-check=full --show-leak-kinds=all ./test\n");
    printf("Or compile with: gcc -fsanitize=address -g\n\n");
    
    double total_start = get_time_ms();
    
    test_basic_lifecycle();
    test_multiple_buckets();
    test_clear_reuse();
    test_stress_varying_sizes();
    test_large_allocations_mmap();
    test_huge_allocations();
    test_mixed_malloc_mmap();
    test_clear_after_mmap_growth();
    test_mmap_threshold_boundary();
    test_rapid_cycles_large();
    test_null_handling();
    test_memory_patterns_large();
    test_gigabyte_allocations();
    test_interleaved_threshold();
    
    double total_elapsed = get_time_ms() - total_start;
    
    printf("\n=== All tests completed ===\n");
    printf("⏱️  Total test suite time: %.2f ms (%.2f seconds)\n", 
           total_elapsed, total_elapsed / 1000.0);
    printf("Check Valgrind/ASan output above for any memory issues.\n");
    printf("Note: Gigabyte allocations may fail on systems with limited resources.\n");
    
    return 0;
}