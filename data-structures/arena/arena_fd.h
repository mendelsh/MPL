#ifndef ARENA_FD_H
#define ARENA_FD_H

#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stddef.h>

#ifdef WIN32
    // Windows specific includes
#else

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#endif

typedef enum { AFD_MALLOC, AFD_MMAP } afd_alloc_type_t;


typedef uint8_t byte_t;

typedef struct afd_bucket_s {
    byte_t *memory;
    size_t offset;
    size_t capacity;
    struct afd_bucket_s *next;
    struct afd_bucket_s *prev;
    afd_alloc_type_t alloc_type;
} afd_bucket_t;

typedef struct {
    afd_bucket_t *head;
    afd_bucket_t *curr;
    size_t size;
    size_t big_alloc_threshold;

#ifndef WIN32
    int backing_fd;
    size_t backing_size;
    size_t backing_offset;
#else
    // TODO: Windows specific mmap backing store members
#endif
} arena_fd_t;

static inline size_t afd_page_align(size_t size) {
    #ifndef WIN32
        long page_size = sysconf(_SC_PAGESIZE);
        if (page_size <= 0) page_size = 4096;
        return (size + page_size - 1) & ~(page_size - 1);
    #else
        return (size + 4095) & ~4095;
    #endif
}

static inline byte_t* afd_alloc_bucket_memory(arena_fd_t *arena, size_t size, afd_alloc_type_t *out_type) {
    if (size > arena->big_alloc_threshold) {
        #ifdef WIN32
            // TODO: Windows specific mmap alternative
            return NULL;
        #else
            size_t offset = afd_page_align(arena->backing_offset);
            size_t aligned_size = afd_page_align(size);
            size_t new_end = offset + aligned_size;

            if (new_end > arena->backing_size) {
                if (ftruncate(arena->backing_fd, new_end) != 0) return NULL;
                arena->backing_size = new_end;
            }

            byte_t *mem = (byte_t*)mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_SHARED, arena->backing_fd, offset);
            if (mem == MAP_FAILED) return NULL;
            arena->backing_offset = new_end;
            if (out_type) *out_type = AFD_MMAP;
            return mem;
        #endif
    } else {
        byte_t *mem = (byte_t*)malloc(size);
        if (out_type) *out_type = AFD_MALLOC;
        return mem;
    }
}

static inline afd_bucket_t* afd_create_bucket(arena_fd_t *arena, size_t size, afd_bucket_t *prev) {
    afd_bucket_t *bucket = (afd_bucket_t*)malloc(sizeof(afd_bucket_t));
    if (!bucket) return NULL;

    bucket->memory = afd_alloc_bucket_memory(arena, size, &bucket->alloc_type);
    if (!bucket->memory) {
        free(bucket);
        return NULL;
    }

    bucket->offset = 0;
    bucket->capacity = size;
    bucket->next = NULL;
    bucket->prev = prev;

    return bucket;
}

static inline void afd_free_bucket(afd_bucket_t *bucket) {
    if (!bucket) return;

    if (bucket->alloc_type == AFD_MALLOC && bucket->memory) free(bucket->memory);
    #ifdef WIN32
        // todo: Windows specific munmap alternative
    #else
        if (bucket->alloc_type == AFD_MMAP && bucket->memory) {
            size_t aligned_size = afd_page_align(bucket->capacity);
            munmap(bucket->memory, aligned_size);
        }
    #endif
    free(bucket);
}





  //////////////////////////////////////////////////
 ////////////////// Public API ////////////////////
//////////////////////////////////////////////////

static inline arena_fd_t* afd_init(size_t initial_capacity, size_t big_alloc_threshold) {
    arena_fd_t *arena = (arena_fd_t*)malloc(sizeof(arena_fd_t));
    if (!arena) return NULL;

    arena->big_alloc_threshold = big_alloc_threshold;
    arena->size = sizeof(arena_fd_t) + sizeof(afd_bucket_t) + initial_capacity;

    #ifndef WIN32
        arena->backing_fd = open("arena.tmp", O_RDWR | O_CREAT | O_TRUNC, 0600);
        if (arena->backing_fd < 0) {
            free(arena);
            return NULL;
        }

        unlink("arena.tmp");            // auto delete on exit

        arena->backing_size = 0;
        arena->backing_offset = 0;
    #else
        // TODO: Windows specific mmap alternative
    #endif

    arena->head = afd_create_bucket(arena, initial_capacity, NULL);
    if (!arena->head) {
        free(arena);
        return NULL;
    }
    arena->curr = arena->head;

    return arena;
}

static inline void afd_clear(arena_fd_t *arena) {
    if (!arena) return;
    arena->head->offset = 0;
    arena->curr = arena->head;
}

static inline void* afd_get_memory(arena_fd_t *arena, size_t size) {
    size_t alignment = alignof(max_align_t);
    size_t offset = (arena->curr->offset + alignment - 1) & ~(alignment - 1);

    if (offset + size > arena->curr->capacity) {
        goto new_bucket_label;

        loop_label:
            if (offset + size <= arena->curr->capacity) goto offset_label;
            else {
                afd_bucket_t *to_free = arena->curr;
                if (arena->curr->prev) {
                    if (arena->curr->next) {
                        arena->curr->prev->next = arena->curr->next;
                        arena->curr = arena->curr->prev;
                    } else {
                        arena->curr = arena->curr->prev;
                        arena->curr->next = NULL;
                    }
                }
                afd_free_bucket(to_free);

                goto new_bucket_label;
            }

        new_bucket_label:
            if (!arena->curr->next) {
                size_t new_capacity = (size + arena->curr->capacity) * 2;
                afd_bucket_t *new_bucket = afd_create_bucket(arena, new_capacity, arena->curr);
                if (!new_bucket) return NULL;

                arena->curr->next = new_bucket;
                arena->curr = new_bucket;
                arena->size += sizeof(afd_bucket_t) + new_capacity;
            } else {
                arena->curr = arena->curr->next;
                goto loop_label;
            }

        offset_label:
            offset = 0;
    }

    void *ptr = arena->curr->memory + offset;
    arena->curr->offset = offset + size;
    return ptr;
}

static inline void afd_destroy(arena_fd_t *arena) {
    if (!arena) return;

    #ifndef WIN32
        if (arena->backing_fd != -1) close(arena->backing_fd);
    #else
        // todo: Windows specific munmap alternative
    #endif

    afd_bucket_t *bucket = arena->head;
    while (bucket) {
        afd_bucket_t *next = bucket->next;
        afd_free_bucket(bucket);
        bucket = next;
    }

    free(arena);
}


#endif // ARENA_FD_H