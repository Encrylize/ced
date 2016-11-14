#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "line.h"
#include "buffer.h"
#include "alloc.h"


Buffer *buffer_new(char *filename) {
    Buffer *buf = malloc_or_exit(sizeof(Buffer));

    if (buf != NULL) {
        buf->row = 0;
        buf->col = 0;
        buf->filename = filename;
        buf->root_line = line_new(NULL, 0);
        buf->cur_line = buf->root_line;
    }

    return buf;
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
            size_t new_line_len = 0;
            size_t insertion_len = idx - line_start;
            char *new_line = NULL;

            if (buf->cur_line->len > buf->row && ch == '\n') {
                /* Copy everything behind the cursor to new_line. */
                new_line_len = buf->cur_line->len - buf->row;
                new_line = malloc_or_exit(new_line_len + 1);
                strcpy(new_line, &buf->cur_line->content[buf->row]);
                buf->cur_line->len = buf->row;
            }

            line_insert(buf->cur_line, buf->row,
                        &content[line_start], insertion_len);
            buf->row += insertion_len;

            if (ch == '\n') {
                buffer_insert_line(buf, new_line, new_line_len);
                free(new_line);

                line_start = idx + 1;
            }
        }

        idx++;
        ch = idx != len ? content[idx] : '\0';
    }
}

void buffer_insert_line(Buffer *buf, const char *content, size_t len) {
    Line *new_line = line_new(content, len);
    Line *prev = buf->cur_line;
    Line *next = prev->next;

    prev->next = new_line;
    new_line->prev = prev;

    buffer_move_rel(buf, -buf->row, 1);

    if (next != NULL) {
        next->prev = new_line;
        new_line->next = next;
    }
}

void buffer_insert_char(Buffer *buf, char ch) {
    buffer_insert(buf, &ch, 1);
}

void buffer_move_rel(Buffer *buf, int row, int col) {
    if (col < 0) {
        /* Move up */
        while (buf->cur_line->prev != NULL && col < 0) {
            buf->cur_line = buf->cur_line->prev;
            buf->col--;
            col++;
        }
    } else {
        /* Move down */
        while (buf->cur_line->next != NULL && col > 0) {
            buf->cur_line = buf->cur_line->next;
            buf->col++;
            col--;
        }
    }

    int new_pos = buf->row + row;

    if (new_pos >= 0)
        buf->row = (size_t) new_pos <= buf->cur_line->len ?
                   (size_t) new_pos : buf->cur_line->len;
    else
        buf->row = 0;
}

void buffer_delete(Buffer *buf, size_t len) {
    char *leftover = calloc(1, 1);
    size_t leftover_len = 0;

    while (len > 0 && !(buf->row == 0 && buf->col == 0)) {
        if (buf->row == 0 && buf->cur_line->prev != NULL) {
            /* Append the string on the current line to leftover. */
            leftover_len += strlen(buf->cur_line->content);
            leftover = realloc_or_exit(leftover, leftover_len + 1);
            strcat(leftover, buf->cur_line->content);

            buffer_delete_line(buf);
            len--;
        }

        size_t deletion_len = line_delete(buf->cur_line, buf->row, len);
        len -= deletion_len;
        buf->row -= deletion_len;
    }

    line_insert(buf->cur_line, buf->row, leftover, leftover_len);
    free(leftover);
}

void buffer_delete_line(Buffer *buf) {
    Line *del = buf->cur_line;
    Line *next = buf->cur_line->next;

    buffer_move_rel(buf, 0, -1);
    buf->row = buf->cur_line->len;
    line_destroy(del);
    buf->cur_line->next = next;

    if (next != NULL)
        next->prev = buf->cur_line;
}

void buffer_print(Buffer *buf) {
    printf("Cursor: (%zu, %zu)\n", buf->row, buf->col);
    printf("Current line: %s\n", buf->cur_line->content);
    printf("----- Lines -----\n");

    Line *head = buf->root_line;
    for (int i = 0; head != NULL; i++) {
        printf("%d. %s\n", i, head->content);
        head = head->next;
    }
}

void buffer_read_file(Buffer *buf) {
    FILE *file = fopen(buf->filename, "r");

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *content = malloc_or_exit(file_size);
    fread(content, 1, file_size, file);

    buffer_insert(buf, content, file_size);
    buffer_move_rel(buf, 0, -buf->col);

    free(content);
    fclose(file);
}

void buffer_write_file(Buffer *buf) {
    FILE *file = fopen(buf->filename, "w");

    for (Line *line = buf->root_line; line != NULL; line = line->next) {
        fputs(line->content, file);

        if (line->next != NULL)
            fputc('\n', file);
    }

    fclose(file);
}
