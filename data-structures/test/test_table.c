#include <stdio.h>
#include "../table.h"

typedef struct {
    int x, y;
} point_t;

typedef point_t* point_ptr;


#define TABLE_EQ_point_ptr(a, b) ((a)->x == (b)->x && (a)->y == (b)->y)
#define TABLE_FREE_KEY_point_ptr(key)  free(key)
#define TABLE_FREE_VALUE_int(value) NO_FREE

size_t point_table_hash(point_t *p) {
    return (size_t)(p->x * 31 + p->y);
}

TABLE_T(point_table, point_ptr, int)


int main(void) {
    point_table_t *table = point_table_init();
    if (!table) {
        fprintf(stderr, "Failed to initialize point table\n");
        return 1;
    }

    point_t *p1 = malloc(sizeof(point_t));
    p1->x = 1; p1->y = 2;
    point_t *p2 = malloc(sizeof(point_t));
    p2->x = 3; p2->y = 4;
    point_t *p3 = malloc(sizeof(point_t));
    p3->x = 5; p3->y = 6;

    point_table_add(table, p1, 10);
    point_table_add(table, p2, 21);
    point_table_add(table, p3, 30);

    point_table_result_t res;

    res = point_table_get(table, p2);
    if (res.err == TABLE_SUCCESS) {
        printf("Point (3,4) has value: %d\n", res.value);
    } else {
        printf("Point (3,4) not found\n");
    }

    point_table_free(table);
    return 0;
}
