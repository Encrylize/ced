#include <stdlib.h>
#include <string.h>

#include "line.h"


Line *line_new(void) {
    Line *line = malloc(sizeof(Line));

    if (line != NULL) {
        line->next = NULL;
        line->prev = NULL;
        line->len = 0;
        line->content = calloc(1, 1);

        if (line->content == NULL) {
            free(line);
            return NULL;
        }
    }

    return line;
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

int line_insert(Line *line, size_t pos, const char *content, size_t len) {
    size_t new_len = line->len + len;
    char *new_content = realloc(line->content, new_len + 1);

    if (new_content == NULL)
        return -1;

    line->content = new_content;

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

    return 0;
}

int line_delete(Line *line, size_t pos, size_t len, size_t *deletion_len) {
    /* Get the amount of characters we can actually delete. */
    *deletion_len = len - ((int) (len - pos) > 0 ? len - pos : 0);
    char *new_content = realloc(line->content, line->len + 1);

    if (new_content == NULL)
        return -1;

    line->content = new_content;
    memmove(&line->content[pos - *deletion_len], &line->content[pos],
            line->len - pos + 1);
    line->len -= *deletion_len;

    return 0;
}
