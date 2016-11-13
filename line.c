#include <stdlib.h>
#include <string.h>

#include "line.h"


Line *line_new(const char *content, size_t len) {
    Line *line = malloc(sizeof(Line));

    if (line != NULL) {
        line->next = NULL;
        line->prev = NULL;
        line->len = len;
        line->content = malloc(len + 1);

        if (line->content == NULL) {
            free(line);
            return NULL;
        } else if (content != NULL) {
            memcpy(line->content, content, len);
        }

        line->content[len] = '\0';
    }

    return line;
}

void line_destroy(Line *line) {
    free(line->content);
    free(line);
}

int line_insert(Line *line, size_t index, const char *content, size_t len) {
    size_t new_len = line->len + len;
    char *new_content = realloc(line->content, new_len + 1);

    if (new_content == NULL)
        return -1;

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

    return 0;
}

int line_delete(Line *line, size_t index, size_t len, size_t *deletion_len) {
    /* Get the amount of characters we can actually delete. */
    *deletion_len = len - ((int) (len - index) > 0 ? len - index : 0);
    char *new_content = realloc(line->content, line->len + 1);

    if (new_content == NULL)
        return -1;

    line->content = new_content;
    memmove(&line->content[index - *deletion_len], &line->content[index],
            line->len - index + 1);
    line->len -= *deletion_len;

    return 0;
}
