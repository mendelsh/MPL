
#include "../map.h"
#include <stdio.h>


int main(int argc, char **argv) {
    map_t *m = map_init();

    if (argc < 2) {
        printf("Usage: %s <number_of_entries>\n", argv[0]);
        return 1;
    }
    int max = atoi(argv[1]);

    for (int i = 0; i < max; i++) {
        char key[16];
        snprintf(key, sizeof(key), "key%d", i);
        map_add(m, key, i * 10);
    }

    MAP_ITERATE(m, idx, key, index, {
        if (idx % (max / 10) == 0) {
            printf("%s -> %d\n", key, index);
        }
    })
}





