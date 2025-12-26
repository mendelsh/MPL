/**
 *     MLC_GENERIC.H - Generic Type Wrapper for MLC
 *     ============================================
 *     
 *     This file generates type-specific versions of the MLC (MPL List Chain) data structure.
 *     It allows you to create multiple MLC instances for different types in the same program.
 *     
 *     USAGE
 *     -----
 *     To generate a type-specific version, define MLC_GEN_TYPE and MLC_GEN_NAME before including:
 *     
 *         #define MLC_GEN_TYPE int           // The element type
 *         #define MLC_GEN_NAME int           // A name suffix for functions/types
 *         #include "mlc_generic.h"
 *     
 *     This generates:
 *         - Type: mpl_list_chain_t_int
 *         - Functions: mlc_init_int(), mlc_append_int(), mlc_index_int(), etc.
 *     
 *     MULTIPLE TYPES EXAMPLE
 *     ----------------------
 *         // Generate int version
 *         #define MLC_GEN_TYPE int
 *         #define MLC_GEN_NAME int
 *         #include "mlc_generic.h"
 *     
 *         // Generate float version
 *         #define MLC_GEN_TYPE float
 *         #define MLC_GEN_NAME float
 *         #include "mlc_generic.h"
 *     
 *         // Generate custom type version
 *         typedef struct { int x, y; } point_t;
 *         #define MLC_GEN_TYPE point_t
 *         #define MLC_GEN_NAME point
 *         #include "mlc_generic.h"
 *     
 *         int main() {
 *             // Use int version
 *             mpl_list_chain_t_int *ints = mlc_init_int(4);
 *             mlc_append_int(ints, 42);
 *             printf("%d\n", *mlc_index_int(ints, 0));
 *             mlc_free_int(ints);
 *     
 *             // Use float version
 *             mpl_list_chain_t_float *floats = mlc_init_float(4);
 *             mlc_append_float(floats, 3.14f);
 *             printf("%f\n", *mlc_index_float(floats, 0));
 *             mlc_free_float(floats);
 *     
 *             // Use point version
 *             mpl_list_chain_t_point *points = mlc_init_point(4);
 *             point_t p = {10, 20};
 *             mlc_append_point(points, p);
 *             printf("(%d, %d)\n", mlc_index_point(points, 0)->x,
 *                                  mlc_index_point(points, 0)->y);
 *             mlc_free_point(points);
 *         }
 *     
 *     NAMING CONVENTION
 *     -----------------
 *     All generated names follow the pattern: <base_name>_<MLC_GEN_NAME>
 *     
 *     For MLC_GEN_NAME = "int":
 *         - Type:      mpl_list_chain_t_int
 *         - Init:      mlc_init_int(capacity)
 *         - Append:    mlc_append_int(list, value)
 *         - Index:     mlc_index_int(list, index)
 *         - Free:      mlc_free_int(list)
 *         - Join:      mlc_join_int(list1, list2)
 *         - Sort:      mlc_mergesort_int(list, compare_fn)
 *     
 *     IMPORTANT NOTES
 *     ---------------
 *     - This file has NO include guards - it's designed to be included multiple times
 *     - Each inclusion generates a complete, independent implementation for that type
 *     - MLC_GEN_TYPE and MLC_GEN_NAME are automatically undefined after each inclusion
 *     - You can use any valid C type for MLC_GEN_TYPE (int, float, pointers, structs, etc.)
 *     - MLC_GEN_NAME should be a valid C identifier (no spaces, start with letter/underscore)
 *     
 *     COMMON PATTERNS
 *     ---------------
 *     String lists:
 *         #define MLC_GEN_TYPE char*
 *         #define MLC_GEN_NAME string
 *         #include "mlc_generic.h"
 *         // Use: mpl_list_chain_t_string, mlc_init_string(), etc.
 *     
 *     Pointer lists:
 *         #define MLC_GEN_TYPE void*
 *         #define MLC_GEN_NAME ptr
 *         #include "mlc_generic.h"
 *         // Use: mpl_list_chain_t_ptr, mlc_init_ptr(), etc.
 **/


#ifndef MLC_GEN_TYPE
#error "Must define MLC_GEN_TYPE before including mlc_generic.h"
#endif

#ifndef MLC_GEN_NAME
#error "Must define MLC_GEN_NAME before including mlc_generic.h"
#endif

#define MLC_CONCAT_(a, b) a##b
#define MLC_CONCAT(a, b) MLC_CONCAT_(a, b)

// Redefine all names with suffix
#define MCL_TYPE MLC_GEN_TYPE
#define mcl_block_s MLC_CONCAT(mcl_block_s_, MLC_GEN_NAME)
#define mcl_block_t MLC_CONCAT(mcl_block_t_, MLC_GEN_NAME)
#define mcl_block_init MLC_CONCAT(mcl_block_init_, MLC_GEN_NAME)
#define mcl_block_free MLC_CONCAT(mcl_block_free_, MLC_GEN_NAME)
#define mcl_blocks_ptr_t MLC_CONCAT(mcl_blocks_ptr_t_, MLC_GEN_NAME)
#define mcl_blocks_ptr_init MLC_CONCAT(mcl_blocks_ptr_init_, MLC_GEN_NAME)
#define mcl_blocks_ptr_free MLC_CONCAT(mcl_blocks_ptr_free_, MLC_GEN_NAME)
#define mcl_blocks_ptr_push MLC_CONCAT(mcl_blocks_ptr_push_, MLC_GEN_NAME)
#define mpl_list_chain_t MLC_CONCAT(mpl_list_chain_t_, MLC_GEN_NAME)
#define mlc_init MLC_CONCAT(mlc_init_, MLC_GEN_NAME)
#define mlc_free MLC_CONCAT(mlc_free_, MLC_GEN_NAME)
#define mlc_append MLC_CONCAT(mlc_append_, MLC_GEN_NAME)
#define mlc_index MLC_CONCAT(mlc_index_, MLC_GEN_NAME)
#define mlc_join MLC_CONCAT(mlc_join_, MLC_GEN_NAME)
#define mlc_mergesort MLC_CONCAT(mlc_mergesort_, MLC_GEN_NAME)
#define mlc_merge MLC_CONCAT(mlc_merge_, MLC_GEN_NAME)
#define mlc_mergesort_recursive MLC_CONCAT(mlc_mergesort_recursive_, MLC_GEN_NAME)
#define mlc_swap MLC_CONCAT(mlc_swap_, MLC_GEN_NAME)
#define ilog2 MLC_CONCAT(ilog2_, MLC_GEN_NAME)

#include "mlc_impl.h"

// Undefine everything to allow re-inclusion
#undef MCL_TYPE
#undef mcl_block_s
#undef mcl_block_t
#undef mcl_block_init
#undef mcl_block_free
#undef mcl_blocks_ptr_t
#undef mcl_blocks_ptr_init
#undef mcl_blocks_ptr_free
#undef mcl_blocks_ptr_push
#undef mpl_list_chain_t
#undef mlc_init
#undef mlc_free
#undef mlc_append
#undef mlc_index
#undef mlc_join
#undef mlc_mergesort
#undef mlc_merge
#undef mlc_mergesort_recursive
#undef ilog2
#undef MLC_CONCAT
#undef MLC_CONCAT_
#undef MLC_GEN_TYPE
#undef MLC_GEN_NAME
