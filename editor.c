#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "editor.h"


Line *line_new(void) {
    Line *new_line = malloc(sizeof(Line));

    if (new_line != NULL) {
        new_line->content = calloc(1, 1);
        new_line->next = NULL;
        new_line->prev = NULL;
        new_line->len = 0;
    }

    return new_line;
}

void line_destroy(Line *line) {
    free(line->content);
    free(line);
}

void line_append(Line *line1, Line *line2) {
    line2->prev = line1;
    line1->next = line2;
}

void line_prepend(Line *line1, Line *line2) {
    line2->next = line1;
    line1->prev = line2;
}

void line_insert(Line *line, size_t pos, const char *content, size_t len) {
    size_t new_len = line->len + len;
    line->content = realloc(line->content, new_len + 1);

    if (line->len > pos)
        /* If characters are being inserted before
         * the end of the string, we need to make
         * room first.
         */
        memmove(&line->content[pos + len], &line->content[pos],
                line->len - pos + 1);

    memcpy(&line->content[pos], content, len);
    line->content[new_len] = '\0';
    line->len = new_len;
}

size_t line_delete(Line *line, size_t pos, size_t len) {
    /* Get the amount of characters we can actually delete. */
    len -= (int) (len - pos) > 0 ? len - pos : 0;

    memmove(&line->content[pos - len], &line->content[pos],
            line->len - pos + 1);
    line->len -= len;
    line->content = realloc(line->content, line->len + 1);

    return len;
}

Buffer *buffer_new(size_t max_y) {
    Buffer *new_buffer = malloc(sizeof(Buffer));

    if (new_buffer != NULL) {
        new_buffer->cur_x = 0;
        new_buffer->cur_y = 0;
        new_buffer->top_y = 0;
        new_buffer->max_y = max_y;
        new_buffer->root_line = line_new();
        new_buffer->top_line = new_buffer->root_line;
        new_buffer->cur_line = new_buffer->root_line;
        new_buffer->redraw = false;
    }

    return new_buffer;
}

void buffer_destroy(Buffer *buf) {
    Line *prev;

    while (buf->root_line != NULL) {
        prev = buf->root_line;
        buf->root_line = buf->root_line->next;
        line_destroy(prev);
    }

    free(buf);
}

void buffer_insert(Buffer *buf, const char *content, size_t len) {
    char ch = content[0];
    size_t line_start = 0;
    size_t idx = 0;

    while (idx <= len) {
        if (ch == '\n' || ch == '\0') {
            size_t new_line_len;
            size_t insertion_len = idx - line_start;
            char *new_line = NULL;

            if (buf->cur_line->len > buf->cur_x && ch == '\n') {
                /* Copy everything behind the cursor to new_line. */
                new_line_len = buf->cur_line->len - buf->cur_x;
                new_line = malloc(new_line_len + 1);
                strcpy(new_line, &buf->cur_line->content[buf->cur_x]);
                buf->cur_line->len = buf->cur_x;
            }

            line_insert(buf->cur_line, buf->cur_x,
                        &content[line_start], insertion_len);
            buf->cur_x += insertion_len;

            if (ch == '\n') {
                buffer_insert_line(buf);
                if (new_line != NULL) {
                    /* Re-insert everything that we popped from
                     * the previous line onto the new line, then
                     * reset the cursor.
                     */
                    line_insert(buf->cur_line, buf->cur_x,
                                new_line, new_line_len);
                    buf->cur_x = 0;
                    free(new_line);
                }

                line_start = idx + 1;
            }
        }

        idx++;
        ch = idx != len ? content[idx] : '\0';
    }
}

void buffer_insert_line(Buffer *buf) {
    Line *new_line = line_new();
    Line *prev = buf->cur_line;
    Line *next = prev->next;

    line_append(prev, new_line);

    buffer_move_rel(buf, 0, 1);
    buf->cur_x = 0;

    if (next != NULL)
        line_prepend(next, new_line);
}

void buffer_insert_char(Buffer *buf, char ch) {
    buffer_insert(buf, &ch, 1);
}

void buffer_move_rel(Buffer *buf, int x, int y) {
    if (y < 0) {
        /* Move up */
        while (buf->cur_line->prev != NULL && y < 0) {
            buf->cur_line = buf->cur_line->prev;
            buf->cur_y--;
            y++;
        }
    } else {
        /* Move down */
        while (buf->cur_line->next != NULL && y > 0) {
            buf->cur_line = buf->cur_line->next;
            buf->cur_y++;
            y--;
        }
    }

    if (buf->cur_y > (buf->top_y + buf->max_y))
        buffer_scroll(buf, buf->cur_y - buf->max_y - buf->top_y);
    else if (buf->cur_y < buf->top_y)
        buffer_scroll(buf, buf->cur_y - buf->top_y);

    int new_pos = buf->cur_x + x;

    if (new_pos >= 0)
        buf->cur_x = (size_t) new_pos <= buf->cur_line->len ?
                     (size_t) new_pos : buf->cur_line->len;
    else
        buf->cur_x = 0;
}

void buffer_delete(Buffer *buf, size_t len) {
    char *leftover = calloc(1, 1);
    size_t leftover_len = 0;

    while (len > 0 && !(buf->cur_x == 0 && buf->cur_y == 0)) {
        if (buf->cur_x == 0 && buf->cur_line->prev != NULL) {
            /* Append the string on the current line to leftover. */
            leftover_len += strlen(buf->cur_line->content);
            leftover = realloc(leftover, leftover_len + 1);
            strcat(leftover, buf->cur_line->content);

            buffer_delete_line(buf);
            len--;
        }

        size_t deletion_len = line_delete(buf->cur_line, buf->cur_x, len);
        len -= deletion_len;
        buf->cur_x -= deletion_len;
    }

    line_insert(buf->cur_line, buf->cur_x, leftover, leftover_len);
    free(leftover);
}

void buffer_delete_line(Buffer *buf) {
    Line *del = buf->cur_line;
    Line *next = buf->cur_line->next;

    buffer_move_rel(buf, 0, -1);
    buf->cur_x = buf->cur_line->len;
    line_destroy(del);
    buf->cur_line->next = next;

    if (next != NULL)
        next->prev = buf->cur_line;
}

void buffer_print(Buffer *buf) {
    printf("Cursor: (%zu, %zu)\n", buf->cur_x, buf->cur_y);
    printf("Current line: %s\n", buf->cur_line->content);
    printf("Top line: %s\n", buf->top_line->content);
    printf("Max Y: %zu\n", buf->max_y);
    printf("Needs redraw: %s\n", buf->redraw ? "true" : "false");
    printf("----- Lines -----\n");

    Line *head = buf->root_line;
    for (int i = 0; head != NULL; i++) {
        printf("%d. %s\n", i, head->content);
        head = head->next;
    }
}

void buffer_scroll(Buffer *buf, int y) {
    if (y < 0) {
        /* Move up */
        while (buf->top_line->prev != NULL && y < 0) {
            buf->top_line = buf->top_line->prev;
            buf->top_y--;
            y++;
        }
    } else {
        /* Move down */
        while (buf->top_line->next != NULL && y > 0) {
            buf->top_line = buf->top_line->next;
            buf->top_y++;
            y--;
        }
    }

    buf->redraw = true;
}

void buffer_read_file(Buffer *buf, FILE *file) {
    buf->file = file;

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *content = malloc(file_size);
    fread(content, 1, file_size, file);

    buffer_insert(buf, content, file_size);
    buffer_move_rel(buf, 0, -buf->cur_y);

    free(content);
}

void buffer_write_file(Buffer *buf, FILE *file) {
    for (Line *line = buf->root_line; line != NULL; line = line->next) {
        fputs(line->content, file);
        fputc('\n', file);
    }
}
