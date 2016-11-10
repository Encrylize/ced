#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdbool.h>
#include <stdio.h>

#include "line.h"

#define buffer_delete_char(buf) buffer_delete(buf, 1)


typedef struct Buffer {
    FILE *file;
    size_t cur_x;
    size_t cur_y;
    size_t max_y;
    size_t top_y;
    Line *root_line;
    Line *top_line;
    Line *cur_line;
    bool redraw;
} Buffer;

Buffer *buffer_new(size_t max_y);
void buffer_destroy(Buffer *buf);
void buffer_insert(Buffer *buf, const char *content, size_t len);
void buffer_insert_char(Buffer *buf, const char ch);
void buffer_insert_line(Buffer *buf);
void buffer_move_rel(Buffer *buf, int x, int y);
void buffer_delete(Buffer *buf, size_t len);
void buffer_delete_line(Buffer *buf);
void buffer_print(Buffer *buf);
void buffer_scroll(Buffer *buf, int y);
void buffer_read_file(Buffer *buf, FILE *file);
void buffer_write_file(Buffer *buf, FILE *file);


#endif
