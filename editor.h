#ifndef _EDITOR_H
#define _EDITOR_H

#include <stdlib.h>


typedef struct Line {
    char *content;
    size_t len;
    struct Line *prev;
    struct Line *next;
} Line;

typedef struct Buffer {
    int cur_x;
    int cur_y;
    Line *top_line;
    Line *cur_line;
} Buffer;

Line *line_new(void);
void line_destroy(Line *line);
void line_append(Line *line1, Line *line2);
void line_prepend(Line *line1, Line *line2);

Buffer *buffer_new(void);
void buffer_destroy(Buffer *buffer);
void buffer_insert(Buffer *buffer, const char *content, size_t len);
void buffer_insert_line(Buffer *buffer);
void buffer_move_rel(Buffer *buf, int x, int y);


#endif
