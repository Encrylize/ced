#include <stdlib.h>
#include <string.h>

#include "editor.h"


Line *line_new(void) {
    Line *new_line = malloc(sizeof(Line));

    if (new_line != NULL) {
        new_line->content = malloc(1);
        new_line->content[0] = '\0';
        new_line->next = NULL;
        new_line->prev = NULL;
    }

    return new_line;
}

void line_destroy(Line *line) {
    free(line->content);
    free(line);
}

Buffer *buffer_new(void) {
    Buffer *new_buffer = malloc(sizeof(Buffer));

    if (new_buffer != NULL) {
        new_buffer->cur_x = 0;
        new_buffer->cur_y = 0;
        new_buffer->top_line = line_new();
        new_buffer->cur_line = new_buffer->top_line;
    }

    return new_buffer;
}

void buffer_destroy(Buffer *buffer) {
    Line *prev;

    while (buffer->top_line != NULL) {
        prev = buffer->top_line;
        buffer->top_line = buffer->top_line->next;
        line_destroy(prev);
    }

    free(buffer);
}
