#ifndef _EDITOR_H
#define _EDITOR_H


typedef struct Line {
    char *content;
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

Buffer *buffer_new(void);
void buffer_destroy(Buffer *buffer);


#endif
