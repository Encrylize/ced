#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "editor.h"


Line *line_new(void) {
    Line *new_line = malloc(sizeof(Line));

    if (new_line != NULL) {
        new_line->content = malloc(1);
        new_line->content[0] = '\0';
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
    if (len == 0)
        /* If len is 0, we'll assume content is null-terminated. */
        /* NOTE: This behaviour only exists to make testing easier
         *       and should NOT be relied on for other code, as
         *       it could possibly get removed in the future.
         */
        len = strlen(content);

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

void buffer_destroy(Buffer *buf) {
    Line *prev;

    while (buf->top_line != NULL) {
        prev = buf->top_line;
        buf->top_line = buf->top_line->next;
        line_destroy(prev);
    }

    free(buf);
}

/* TODO: This code is shoddy and repetitive, but it'll
 *       do as a placeholder for now. Replace it with
 *       some better code later.
 */
void buffer_insert(Buffer *buf, const char *content, size_t len) {
    char ch = content[0];
    size_t line_start = 0;
    size_t idx = 0;

    while (idx <= len) {
        size_t new_len;
        size_t insertion_len;

        if (ch == '\n') {
            insertion_len = idx - line_start;
            new_len = buf->cur_x + insertion_len;
            char *new_line = NULL;
            size_t new_line_len = 0;

            if (buf->cur_line->len > (size_t) buf->cur_x) {
                /* If characters are being inserted before the
                 * end of the line, we'll push all following
                 * characters onto a new line.
                 */

                new_line_len = buf->cur_line->len - buf->cur_x;
                new_line = malloc(new_line_len + 1);
                strcpy(new_line, &buf->cur_line->content[buf->cur_x]);
            }

            buf->cur_line->content = realloc(buf->cur_line->content,
                                             new_len + 1);
            memcpy(&buf->cur_line->content[buf->cur_x],
                   &content[line_start], insertion_len);
            buf->cur_line->content[new_len] = '\0';
            buf->cur_line->len = new_len;
            buffer_insert_line(buf);

            if (new_line_len > 0) {
                buffer_insert(buf, new_line, new_line_len);
                free(new_line);
                buf->cur_x = 0;
            }

            line_start = idx + 1;
        } else if (idx == len) {
            insertion_len = idx - line_start;
            new_len = buf->cur_line->len + insertion_len;
            buf->cur_line->content = realloc(buf->cur_line->content,
                                             new_len + 1);

            if (buf->cur_line->len > (size_t) buf->cur_x)
                /* Same thing as with a newline, but this time
                 * we just push the characters forward.
                 */
                memmove(&buf->cur_line->content[buf->cur_x + insertion_len],
                        &buf->cur_line->content[buf->cur_x],
                        buf->cur_line->len - buf->cur_x);

            memcpy(&buf->cur_line->content[buf->cur_x],
                   &content[line_start], insertion_len);
            buf->cur_line->content[new_len] = '\0';
            buf->cur_line->len = new_len;
            buf->cur_x += insertion_len;

            break;
        }

        idx++;
        ch = content[idx];
    }
}

void buffer_insert_line(Buffer *buf) {
    Line *new_line = line_new();
    Line *prev = buf->cur_line;
    Line *next = prev->next;

    line_append(prev, new_line);

    buf->cur_line = new_line;
    buf->cur_y++;
    buf->cur_x = 0;

    if (next != NULL)
        line_prepend(next, new_line);
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

    int new_pos = buf->cur_x + x;

    if (new_pos > 0)
        buf->cur_x = (size_t) new_pos <= buf->cur_line->len ?
                     new_pos : (int) buf->cur_line->len;
    else
        buf->cur_x = new_pos >= 0 ? new_pos : 0;
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
    Line *prev = buf->cur_line->prev;
    Line *next = buf->cur_line->next;

    line_destroy(buf->cur_line);
    buf->cur_line = prev;
    prev->next = next;

    if (next != NULL)
        next->prev = prev;

    buf->cur_y--;
    buf->cur_x = buf->cur_line->len;
}
