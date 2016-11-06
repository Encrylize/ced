#include <curses.h>
#include <stdbool.h>
#include <ctype.h>

#include "editor.h"
#include "interface.h"


void interface_handle_key(Buffer *buf, int key) {
    switch (key) {
        case KEY_LEFT:
            buffer_move_rel(buf, -1, 0);
            break;
        case KEY_RIGHT:
            buffer_move_rel(buf, 1, 0);
            break;
        case KEY_UP:
            buffer_move_rel(buf, 0, -1);
            break;
        case KEY_DOWN:
            buffer_move_rel(buf, 0, 1);
            break;
        case KEY_BACKSPACE:
            if (buf->cur_x == 0) {
                /* Other lines will be affected, because
                 * a newline is being delected, so we'll
                 * redraw them.
                 */
                buffer_delete_char(buf);
                interface_redraw_lines(buf, buf->cur_line,
                                       buf->cur_y - buf->top_y);
            } else {
                buffer_delete_char(buf);
                interface_redraw_line(buf->cur_y - buf->top_y,
                                      buf->cur_line->content);
            }
            break;
        case KEY_ENTER:
        case KEY_ENTER_2:
            buffer_insert_char(buf, '\n');
            interface_redraw_lines(buf, buf->cur_line,
                                   buf->cur_y - buf->top_y);
            break;
        default:
            if (isprint(key)) {
                buffer_insert_char(buf, (char) key);
                interface_redraw_line(buf->cur_y - buf->top_y,
                                      buf->cur_line->content);
            }
    }

    if (buf->redraw) {
        interface_redraw_lines(buf, buf->top_line, 0);
        buf->redraw = false;
    }

    move(buf->cur_y - buf->top_y, buf->cur_x);
    refresh();
}

void interface_redraw_lines(Buffer *buf, Line *line, size_t y) {
    for (; line != NULL && y <= buf->max_y; y++) {
        clrtoeol();
        mvprintw(y, 0, line->content);
        line = line->next;
    }

    clrtobot();
}

int main(void) {
    int key;

    initscr();
    raw();
    noecho();
    keypad(stdscr, true);

    Buffer *buf = buffer_new(getmaxy(stdscr) - 1);

    while ((key = getch()) != KEY_ESCAPE)
        interface_handle_key(buf, key);

    endwin();
    buffer_print(buf);
    buffer_destroy(buf);

    return 0;
}
