#include <stdlib.h>
#include <string.h>

#include "line.h"
#include "alloc.h"


Line *line_new(const char *content, size_t len) {
    Line *line = malloc_or_exit(sizeof(Line));

    line->next = NULL;
    line->prev = NULL;
    line->len = len;
    line->content = malloc_or_exit(len + 1);
    line->content[len] = '\0';
    memcpy(line->content, content, len);

    return line;
}

void line_destroy(Line *line) {
    free(line->content);
    free(line);
}

void line_insert(Line *line, size_t index, const char *content, size_t len) {
    size_t new_len = line->len + len;
    char *new_content = realloc_or_exit(line->content, new_len + 1);
    line->content = new_content;

    if (line->len > index)
        /* If characters are being inserted before
         * the end of the string, we need to make
         * room first.
         */
        memmove(&line->content[index + len], &line->content[index],
                line->len - index + 1);

    memcpy(&line->content[index], content, len);
    line->content[new_len] = '\0';
    line->len = new_len;
}

size_t line_delete(Line *line, size_t index, size_t len) {
    /* Get the amount of characters we can actually delete. */
    len -= (int) (len - index) > 0 ? len - index : 0;
    char *new_content = realloc_or_exit(line->content, line->len + 1);

    line->content = new_content;
    memmove(&line->content[index - len], &line->content[index],
            line->len - index + 1);
    line->len -= len;

    return len;
}
