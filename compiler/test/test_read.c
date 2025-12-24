#include "../read.h"
#include "../../data-structures/map.h"


int main() {
    line_t line;
    line_init(&line);

    // FILE *f = fopen("test_read.txt", "r");
    // if (!f) {
    //     perror("Failed to open file");
    //     return 1;
    // }

    // while (line_read(f, &line) == 0) {
    //     if (is_blank_line(&line)) continue;
    //     printf("%.*s\n", (int)line.len, line.str);
    // // }

    // line_free(&line);
    // fclose(f);

    block_t block;

    if (read_src_file("test_read.mpl", &block) != 0) {
        fprintf(stderr, "Failed to read source file\n");
        return 1;
    }

    printf("Read block with %zu instructions, %zu constants, %zu locals\n",
           block.instruction_size,
           block.constant_count,
           block.local_count);


    map_t *m = map_init();

    for (int i = 0; i < 1000000; i++) {
        char key[16];
        snprintf(key, sizeof(key), "key%d", i);
        map_add(m, key, i);
    }

    MAP_ITERATE(m, idx, key, index, {
        if (idx % 100) printf("%s -> %d\n", key, index);
    })

    return 0;
}

