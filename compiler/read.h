#ifndef READ_H
#define READ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct line_s {
    char *str;
    size_t len;
    size_t cap;
} line_t;

void line_init(line_t *line);
void line_free(line_t *line);
int line_read(FILE *file, line_t *line);
bool is_blank_line(const line_t *line);

 ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


#include "../vm/vm.h"

int read_src_file(const char *filename, block_t *out_block);




#endif // READ_H