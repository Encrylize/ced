#ifndef _INTERFACE_H

#include <stdbool.h>
#include <stdio.h>

#include "buffer.h"

#define KEY_ESCAPE 27
#define KEY_ENTER_2 10

#define CTRL(ch) ((ch) & 037)
#define interface_redraw_line(col, content) \
    do {                                    \
        mvprintw(col, 0, content);          \
        clrtoeol();                         \
    } while (0)


void interface_handle_key(Buffer *buf, int key);
void interface_redraw_lines(Buffer *buf, Line *line, size_t col);


#endif
