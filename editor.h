#ifndef _EDITOR_H
#define _EDITOR_H

#include <stdbool.h>
#include <stdlib.h>

#define buffer_delete_char(buf) buffer_delete(buf, 1)


typedef struct Line {
    char *content;
    size_t len;
    struct Line *prev;
    struct Line *next;
} Line;

typedef struct Buffer {
    size_t cur_x;
    size_t cur_y;
    size_t max_y;
    size_t top_y;
    Line *root_line;
    Line *top_line;
    Line *cur_line;
    bool redraw;
} Buffer;

Line *line_new(void);
void line_destroy(Line *line);
void line_append(Line *line1, Line *line2);
void line_prepend(Line *line1, Line *line2);
void line_insert(Line *line, size_t pos, const char *content, size_t len);
size_t line_delete(Line *line, size_t pos, size_t len);

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


#endif
