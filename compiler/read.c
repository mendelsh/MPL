#include "read.h"
#include <ctype.h>

void line_init(line_t *line) {
    line->str = NULL;
    line->len = 0;
    line->cap = 0;
}

void line_free(line_t *line) {
    if (line->str) free(line->str);
    line_init(line);
}

int line_read(FILE *file, line_t *line) {
    if (!file || !line) return -1;

    line->len = 0;
    int c;

    while ((c = fgetc(file)) != EOF) {
        if (line->len + 1 >= line->cap) {
            size_t new_cap = line->cap == 0 ? 1024 : line->cap * 2;
            char *new_str = realloc(line->str, new_cap);
            if (!new_str) return -1;

            line->str = new_str;
            line->cap = new_cap;
        }

        if (c == '\n') break;

        line->str[line->len++] = (char)c;
    }

    if (line->str) line->str[line->len] = '\0';

    return c == EOF && line->len == 0 ? -1 : 0;
}

bool is_blank_line(const line_t *line) {
    if (!line || line->len == 0) return true;
    for (size_t i = 0; i < line->len; i++) {
        if (!isspace((unsigned char)line->str[i])) return false;
    }
    return true;
}

 ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "../data-structures/map.h"

#define TOKEN_IN(str, token) (strstr(str, token) != NULL)

bool is_word_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

// Extract variable name after "let" keyword
// Returns pointer to start of variable name and sets *out_len to its length
// Returns NULL if no valid variable found
const char* find_let_variable(const char *str, size_t len, size_t *out_len) {
    bool in_string = false;
    
    for (size_t i = 0; i <= len - 3; i++) {
        char c = str[i];
        
        // Toggle in_string if " found
        if (c == '"') in_string = !in_string;
        if (in_string) continue;
        
        // Check for "let"
        if (strncmp(&str[i], "let", 3) == 0) {
            // Check word boundaries
            if ((i == 0 || !is_word_char(str[i - 1])) &&
                (i + 3 == len || !is_word_char(str[i + 3]))) {
                
                // Found "let", now find variable name
                size_t j = i + 3;
                
                // Skip whitespace after "let"
                while (j < len && isspace((unsigned char)str[j])) {
                    j++;
                }
                
                // Find variable name length
                size_t var_start = j;
                while (j < len && is_word_char(str[j])) {
                    j++;
                }
                
                size_t var_len = j - var_start;
                if (var_len > 0) {
                    *out_len = var_len;
                    return &str[var_start];
                }
            }
        }
    }
    return NULL;
}

bool is_let_declaration(const line_t *line) {
    if (!line || !line->str) return false;
    
    size_t var_len;
    return find_let_variable(line->str, line->len, &var_len) != NULL;
}

int read_src_file(const char *filename, block_t *out_block) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;
    
    line_t line;
    line_init(&line);
    
    map_t *var_map = map_init();
    if (!var_map) {
        fclose(f);
        line_free(&line);
        return -1;
    }
    
    size_t local_count = 0, constant_count = 0, instruction_size = 0;
    
    while (line_read(f, &line) == 0) {
        if (is_blank_line(&line)) continue;
        
        // Try to find variable from let declaration
        size_t var_len;
        const char *var_start = find_let_variable(line.str, line.len, &var_len);
        
        if (var_start) {
            // Allocate and copy variable name (null-terminated)
            char *var_name = malloc(var_len + 1);
            if (!var_name) {
                map_free(var_map);
                fclose(f);
                line_free(&line);
                return -1;
            }
            
            memcpy(var_name, var_start, var_len);
            var_name[var_len] = '\0';
            
            // Check if variable already exists
            int existing_index = map_get(var_map, var_name);
            if (existing_index < 0) {
                // New variable, add to map
                if (map_add(var_map, var_name, (int)local_count)) {
                    local_count++;
                } else {
                    // Map add failed
                    free(var_name);
                    map_free(var_map);
                    fclose(f);
                    line_free(&line);
                    return -1;
                }
            }
            // If variable exists, we could handle redeclaration here
            
            free(var_name);
        }
    }
    
    fclose(f);
    line_free(&line);
    
    out_block->local_count = local_count;
    out_block->constant_count = constant_count;
    out_block->instruction_size = instruction_size;
    out_block->instructions = NULL;
    out_block->constants = NULL;
    
    printf("variable count: %zu\n", local_count);
    printf("variable indices:\n");

    MAP_ITERATE(var_map, i, key, index, {
        printf("%s -> %d\n", key, index);
    })

    map_free(var_map);
    
    return 0;
}

