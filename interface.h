#ifndef _INTERFACE_H

#include <stdbool.h>

#include "editor.h"

#define KEY_ESCAPE 27
#define KEY_ENTER_2 10

#define interface_redraw_line(y, content) \
    do {                                  \
        mvprintw(y, 0, content);          \
        clrtoeol();                       \
    } while (0)


void interface_handle_key(Buffer *buf, int key);
void interface_redraw_lines(Buffer *buf);
bool interface_escape_is_pressed(int key);


#endif
