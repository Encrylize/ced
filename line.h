#ifndef _LINE_H
#define _LINE_H

#include <stdlib.h>


typedef struct Line {
    char *content;
    size_t len;
    struct Line *prev;
    struct Line *next;
} Line;


Line *line_new(const char *content, size_t len);
void line_destroy(Line *line);
void line_append(Line *line1, Line *line2);
void line_prepend(Line *line1, Line *line2);
int line_insert(Line *line, size_t index, const char *content, size_t len);
int line_delete(Line *line, size_t index, size_t len, size_t *deletion_len);


#endif
