#include <stdio.h>
#include <time.h>
#include <math.h>

#define MLC_GEN_TYPE float
#define MLC_GEN_NAME float
#include "../mlc_generic.h"

#define MLC_GEN_TYPE int
#define MLC_GEN_NAME int
#include "../mlc_generic.h"

int compare(const float *a, const float *b) {
    return (*a > *b) - (*a < *b);
}

int compare_int(const int *a, const int *b) {
    return (*a > *b) - (*a < *b);
}


int main() {
    srand(time(NULL));

    mpl_list_chain_t_float *mlc = mlc_init_float(4);
    mpl_list_chain_t_int *mlc2 = mlc_init_int(4);
    
    for (size_t i = 0; i < 18; i++) {
        mlc_append_float(mlc, (float)rand() / RAND_MAX * 10000);
        mlc_append_int(mlc2, (int)rand() % 100);
    }

    for (size_t i = 0; i < mlc->length; i++) {
        printf("%f ", *mlc_index_float(mlc, i));
    }
    printf("\n\n");

    for (size_t i = 0; i < mlc2->length; i++) {
        printf("%d ", *mlc_index_int(mlc2, i));
    }
    printf("\n\n");

    *mlc_index_float(mlc, mlc->length - 1) = 0.0f;
    *mlc_index_int(mlc2, mlc2->length - 1) = 0;

    mlc_mergesort_float(mlc, compare);
    mlc_mergesort_int(mlc2, compare_int);

    for (size_t i = 0; i < mlc->length; i++) {
        printf("%f ", *mlc_index_float(mlc, i));
    }
    printf("\n\n");

    for (size_t i = 0; i < mlc2->length; i++) {
        printf("%d ", *mlc_index_int(mlc2, i));
    }
    printf("\n\n");

    mlc_free_float(mlc);
    mlc_free_int(mlc2);

    return 0;
}
