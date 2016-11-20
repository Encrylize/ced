#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdbool.h>
#include <stdio.h>

#include "line.h"

#define TAB_SIZE 4
#define buffer_delete_char(buf) buffer_delete(buf, 1)


typedef struct Buffer {
    char *filename;
    size_t row;
    size_t col;
    Line *root_line;
    Line *cur_line;
} Buffer;

Buffer *buffer_new(char *filename);
void buffer_destroy(Buffer *buf);
void buffer_insert(Buffer *buf, const char *content, size_t len);
void buffer_insert_line(Buffer *buf, const char *content, size_t len);
void buffer_insert_char(Buffer *buf, const char ch);
void buffer_insert_tab(Buffer *buf);
void buffer_move_rel(Buffer *buf, int row, int col);
void buffer_delete(Buffer *buf, size_t len);
void buffer_delete_line(Buffer *buf);
void buffer_delete_spaces(Buffer *buf);
void buffer_print(Buffer *buf);
int buffer_read_file(Buffer *buf);
int buffer_write_file(Buffer *buf);


#endif
