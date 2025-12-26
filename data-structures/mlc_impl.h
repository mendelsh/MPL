
/**
 *     MLC - MPL List Chain
 *     ====================
 *     
 *     A dynamic array implementation with O(1) amortized append and O(1) random access.
 *     
 *     OVERVIEW
 *     --------
 *     MLC is a list chain data structure that stores elements in exponentially growing blocks.
 *     Block sizes follow the pattern: a, 2a, 4a, 8a, ... where 'a' is the initial capacity.
 *     This design provides:
 *         - O(1) amortized append operations
 *         - O(1) random access to any element
 *         - Efficient memory usage with minimal reallocations
 *     
 *     CONFIGURATION
 *     -------------
 *     Before including this header, define MCL_TYPE to specify the element type:
 *     
 *         #define MCL_TYPE int
 *         #include "mlc.h"
 *     
 *     If MCL_TYPE is not defined, it defaults to uint8_t.
 *     
 *     Alternatively, you can include mlc_generic.h and define MLC_GEN_TYPE and MLC_GEN_NAME (see mlc_generic.h for details).
 *     
 *     BASIC USAGE
 *     -----------
 *     
 *     1. Initialize:
 *         mpl_list_chain_t *mlc = mlc_init(initial_capacity);
 *         
 *         Creates a new list chain with the specified initial block capacity.
 *         Example: mlc_init(4) creates first block with capacity 4.
 *     
 *     2. Append elements:
 *         mlc_append(mlc, value);
 *         
 *         Adds an element to the end of the list.
 *         Returns 0 on success, -1 on allocation failure.
 *     
 *     3. Access elements (get):
 *         MCL_TYPE value = *mlc_index(mlc, index);
 *         
 *         Returns pointer to element at given index.
 *         Returns NULL if index is out of bounds.
 *     
 *     4. Modify elements (set):
 *         *mlc_index(mlc, index) = new_value;
 *         
 *         Modifies element at given index.
 *     
 *     5. Free memory:
 *         mlc_free(mlc);
 *         
 *         Deallocates all blocks and the list chain structure.
 *     
 *     EXAMPLE
 *     -------
 *         #define MCL_TYPE int
 *         #include "mlc.h"
 *         
 *         mpl_list_chain_t *mlc = mlc_init(4);
 *         
 *         // Append elements
 *         for (int i = 0; i < 10; i++) {
 *             mlc_append(mlc, i * 10);
 *         }
 *         
 *         // Access elements
 *         printf("Element at index 5: %d\n", *mlc_index(mlc, 5));  // Output: 50
 *         
 *         // Modify element
 *         *mlc_index(mlc, 5) = 999;
 *         
 *         // Get list length
 *         printf("Length: %zu\n", mlc->length);
 *         
 *         // Clean up
 *         mlc_free(mlc);
 *     
 *     TIME COMPLEXITY
 *     ---------------
 *     
 *     mlc_init(capacity)          O(1)
 *         Allocates initial block with given capacity.
 *     
 *     mlc_append(mlc, value)      O(1) amortized
 *         - Best case: O(1) - append to current block
 *         - Worst case: O(log n) - create new block and resize pointer array
 *         - Amortized: O(1) - expensive operations happen rarely
 *         
 *         For n consecutive appends: Total time is O(n), not O(n log n).
 *         This is because only O(log n) blocks are created, and each block
 *         creation costs at most O(log n). Therefore:
 *             Total = O(n) + O(log² n) = O(n)
 *             Amortized per operation = O(n)/n = O(1)
 *     
 *     mlc_index(mlc, index)       O(1)
 *         Uses bit manipulation (CLZ instruction) to compute block index in
 *         constant time, then performs direct array access.
 *     
 *     mlc_free(mlc)               O(number of blocks) = O(log n)
 *         Must free each block individually. Since there are O(log n) blocks
 *         for n elements, this is O(log n).
 *     
 *     MATHEMATICAL FOUNDATION
 *     -----------------------
 *     Block i has capacity: a * 2^i
 *     Block i starts at position: a * (2^i - 1)
 *     
 *     To find block index for element k:
 *         block_index = floor(log₂(k/a + 1))
 *         local_index = k - a * (2^block_index - 1)
 *     
 *     This formula is computed in O(1) using the __builtin_clz intrinsic,
 *     which compiles to a single CPU instruction (CLZ/BSR).
 *     
 *     MEMORY LAYOUT
 *     -------------
 *     For initial_capacity = 4, after 18 appends:
 *     
 *         Block 0: [0, 1, 2, 3]                    (capacity 4)
 *         Block 1: [4, 5, 6, 7, 8, 9, 10, 11]      (capacity 8)
 *         Block 2: [12, 13, 14, 15, 16, 17, ...]   (capacity 16)
 *     
 *     Total capacity grows exponentially, minimizing reallocations.
 *     
 *     NOTES
 *     -----
 *     - Thread safety: Not thread-safe. Use external synchronization if needed.
 *     - Index bounds: mlc_index returns NULL for out-of-bounds access.
 *       Always check return value or ensure index < mlc->length.
 *     - Memory overhead: Stores O(log n) block pointers for n elements.
 *     - The pointer return from mlc_index allows both reading and writing,
 *       similar to standard array indexing.
 **/


#include <stdlib.h>
#include <stdint.h>


/*
 *    user should define MCL_TYPE before including this header,
 *    to specify the data type of the mpl_list_chain_t structure.
 *    if not defined, it defaults to byte (uint8_t).
 */

#ifndef MCL_TYPE
#define MCL_TYPE uint8_t
#endif

typedef struct mcl_block_s {
    MCL_TYPE *data;
    size_t size;
    size_t capacity;
    struct mcl_block_s *next;
} mcl_block_t;

static inline mcl_block_t* mcl_block_init(size_t initial_capacity) {
    mcl_block_t *mcl = (mcl_block_t*)malloc(sizeof(mcl_block_t));
    if (!mcl) return NULL;

    mcl->data = (MCL_TYPE*)malloc(initial_capacity * sizeof(MCL_TYPE));
    if (!mcl->data) {
        free(mcl);
        return NULL;
    }

    mcl->size = 0;
    mcl->capacity = initial_capacity;
    mcl->next = NULL;

    return mcl;
}

static inline void mcl_block_free(mcl_block_t *mcl) {
    if (!mcl) return;
    free(mcl->data);
    free(mcl);
}

#ifndef MCL_BLOCK_CAPACITY
#define MCL_BLOCK_CAPACITY 8
#endif

typedef struct {
    mcl_block_t **blocks;
    size_t size;
    size_t capacity;
} mcl_blocks_ptr_t;

static inline mcl_blocks_ptr_t mcl_blocks_ptr_init(size_t initial_capacity) {
    mcl_blocks_ptr_t mbp;
    mbp.blocks = (mcl_block_t**)malloc(initial_capacity * sizeof(mcl_block_t*));
    mbp.size = 0;
    mbp.capacity = initial_capacity;
    return mbp;
}

static inline void mcl_blocks_ptr_free(mcl_blocks_ptr_t *mbp) {
    free(mbp->blocks);
    mbp->blocks = NULL;
    mbp->size = 0;
    mbp->capacity = 0;
}

static inline int mcl_blocks_ptr_push(mcl_blocks_ptr_t *mbp, mcl_block_t *value) {
    if (mbp->size >= mbp->capacity) {
        size_t new_capacity = mbp->capacity * 2;
        mcl_block_t **new_blocks = (mcl_block_t**)realloc(mbp->blocks, new_capacity * sizeof(mcl_block_t*));
        if (!new_blocks) return -1;
        mbp->blocks = new_blocks;
        mbp->capacity = new_capacity;
    }
    mbp->blocks[mbp->size++] = value;
    return 0;
}

typedef struct {
    mcl_block_t *head;
    size_t length;
    mcl_blocks_ptr_t blocks_ptr;
} mpl_list_chain_t;

static inline mpl_list_chain_t* mlc_init(size_t block_initial_capacity) {
    mpl_list_chain_t *mlc = (mpl_list_chain_t*)malloc(sizeof(mpl_list_chain_t));
    if (!mlc) return NULL;

    mlc->head = mcl_block_init(block_initial_capacity);
    if (!mlc->head) {
        free(mlc);
        return NULL;
    }

    mlc->length = 0;
    mlc->blocks_ptr = mcl_blocks_ptr_init(MCL_BLOCK_CAPACITY);
    if (!mlc->blocks_ptr.blocks) {
        mcl_block_free(mlc->head);
        free(mlc);
        return NULL;
    }

    if (mcl_blocks_ptr_push(&mlc->blocks_ptr, mlc->head) != 0) {
        mcl_blocks_ptr_free(&mlc->blocks_ptr);
        mcl_block_free(mlc->head);
        free(mlc);
        return NULL;
    }

    return mlc;
}

static inline void mlc_free(mpl_list_chain_t *mlc) {
    if (!mlc) return;

    mcl_block_t *current = mlc->head;
    while (current) {
        mcl_block_t *next = current->next;
        mcl_block_free(current);
        current = next;
    }

    mcl_blocks_ptr_free(&mlc->blocks_ptr);
    free(mlc);
}

static inline int mlc_append(mpl_list_chain_t *mlc, MCL_TYPE value) {
    mcl_blocks_ptr_t *blocks_ptr = &mlc->blocks_ptr;
    mcl_block_t *tail = blocks_ptr->blocks[blocks_ptr->size - 1];
    if (tail->size >= tail->capacity) {
        mcl_block_t *new_block = mcl_block_init(tail->capacity * 2);
        if (!new_block) return -1;

        tail->next = new_block;
        tail = new_block;

        if (mcl_blocks_ptr_push(&mlc->blocks_ptr, new_block) != 0) {
            mcl_block_free(new_block);
            return -1;
        }
    }

    tail->data[tail->size++] = value;
    mlc->length++;
    return 0;
}


/*

        we want to get random access to the elements in the list chain (in O(1) time complexity).
        for that, we need to calculate which block contains the element at the given index,
        and the local index within that block. we will call these block_index and local_index respectively.
        for example, if we want to access to element k, we need to find block_index and local_index,
        so we can access the element as blocks[block_index]->data[local_index].

        here I will calculate it mathematically:

            Let A be the geometric series a, 2a, 2^2 * a, ...
            from the formula of the sum of a geometric series, we will get:

                S_i = a(q^i - 1) / (q - 1)

            where S_i is the sum of the first i terms, a is the first term, q is the common ratio, and i is the number of terms.

            ->  S_i = a(2^i - 1)

            which means, block i contains positions [a(2^i - 1), a(2^(i+1) - 1) - 1].
            so, for k (0 indexed) in block i, k in [a(2^i - 1), a(2^(i+1) - 1) - 1],

            ->  a(2^i - 1) <= k < a(2^(i+1) - 1)
            ->  2^i - 1 <= k/a < 2^(i+1) - 1
            ->  2^i <= k/a + 1 < 2^(i+1)
            ->  i <= log2(k/a + 1) < i + 1
            
            therefore, i = floor(log2(k/a + 1))
            in other words: 
            
                block_index = floor(log2(k/block_initial_capacity + 1)).
            ->  local_index = k - block_initial_capacity * (2^block_index - 1)
*/

static inline int ilog2(uint32_t n) {
    return 31 - __builtin_clz(n);
}

/*    you can use this function to get or set an element in the list chain.
      for example:
      MLC_TYPE value = *mlc_index(mlc, 10);
      *mlc_index(mlc, 10) = value;    */
static inline MCL_TYPE* mlc_index(mpl_list_chain_t *mlc, size_t index) {
    if (index >= mlc->length) return NULL;

    size_t a = mlc->head->capacity;

    size_t block_index = ilog2((uint32_t)(index/a + 1));
    size_t local_index = index - a*((1 << block_index) - 1);

    return &mlc->blocks_ptr.blocks[block_index]->data[local_index];
}

static inline int mlc_join(mpl_list_chain_t *mlc, mpl_list_chain_t *other) {
    for (size_t i = 0; i < other->length; i++) {
        if (mlc_append(mlc, *mlc_index(other, i)) != 0) return -1;
    }
    return 0;
}

// Merge two sorted ranges within the data structure
static inline void mlc_merge(mpl_list_chain_t *mlc, size_t left, size_t mid, size_t right, int (*cmp)(const MCL_TYPE*, const MCL_TYPE*)) {
    size_t n1 = mid - left;
    size_t n2 = right - mid;
    
    MCL_TYPE *L = (MCL_TYPE*)malloc(n1 * sizeof(MCL_TYPE));
    MCL_TYPE *R = (MCL_TYPE*)malloc(n2 * sizeof(MCL_TYPE));
    
    for (size_t i = 0; i < n1; i++) L[i] = *mlc_index(mlc, left + i);
    for (size_t i = 0; i < n2; i++) R[i] = *mlc_index(mlc, mid + i);
    
    size_t i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (cmp(&L[i], &R[j]) <= 0) *mlc_index(mlc, k++) = L[i++];
        else *mlc_index(mlc, k++) = R[j++];
    }

    while (i < n1) *mlc_index(mlc, k++) = L[i++];
    while (j < n2) *mlc_index(mlc, k++) = R[j++];
    
    free(L);
    free(R);
}

static void mlc_mergesort_recursive(mpl_list_chain_t *mlc, size_t left, size_t right, int (*cmp)(const MCL_TYPE*, const MCL_TYPE*)) {
    if (right - left <= 1) return;
    
    size_t mid = (left + right) / 2;
    mlc_mergesort_recursive(mlc, left, mid, cmp);
    mlc_mergesort_recursive(mlc, mid, right, cmp);
    mlc_merge(mlc, left, mid, right, cmp);
}

static inline void mlc_mergesort(mpl_list_chain_t *mlc, int (*cmp)(const MCL_TYPE*, const MCL_TYPE*)) {
    mlc_mergesort_recursive(mlc, 0, mlc->length, cmp);
}

