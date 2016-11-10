#include <curses.h>
#include <stdbool.h>
#include <ctype.h>

#include "buffer.h"
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
            if (buf->row == 0) {
                /* Other lines will be affected, because
                 * a newline is being delected, so we'll
                 * redraw them.
                 */
                buffer_delete_char(buf);
                interface_redraw_lines(buf, buf->cur_line,
                                       buf->col - buf->top_col);
            } else {
                buffer_delete_char(buf);
                interface_redraw_line(buf->col - buf->top_col,
                                      buf->cur_line->content);
            }
            break;
        case KEY_ENTER:
        case KEY_ENTER_2:
            buffer_insert_char(buf, '\n');
            interface_redraw_lines(buf, buf->cur_line,
                                   buf->col - buf->top_col);
            break;
        case CTRL('s'):
            buf->file = freopen(NULL, "wb", buf->file);
            buffer_write_file(buf, buf->file);
            break;
        default:
            if (isprint(key)) {
                buffer_insert_char(buf, (char) key);
                interface_redraw_line(buf->col - buf->top_col,
                                      buf->cur_line->content);
            }
    }

    if (buf->redraw) {
        interface_redraw_lines(buf, buf->top_line, 0);
        buf->redraw = false;
    }

    move(buf->col - buf->top_col, buf->row);
    refresh();
}

void interface_redraw_lines(Buffer *buf, Line *line, size_t col) {
    for (; line != NULL && col <= buf->max_cols; col++) {
        clrtoeol();
        /* The string is passed in as an argument to the
         * format string to escape potential format specifiers
         * inside the line. I figured this out the hard way.
         */
        mvprintw(col, 0, "%s", line->content);
        line = line->next;
    }

    clrtobot();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./interface <filename>\n");
        exit(1);
    }

    int key;

    initscr();
    raw();
    noecho();
    keypad(stdscr, true);

    Buffer *buf = buffer_new(getmaxy(stdscr) - 1);

    FILE *file = fopen(argv[1], "rb");
    if (file != NULL) {
        buffer_read_file(buf, file);
        interface_redraw_lines(buf, buf->root_line, 0);
        move(0, 0);
    }

    while ((key = getch()) != KEY_ESCAPE)
        interface_handle_key(buf, key);

    endwin();
    buffer_print(buf);
    buffer_destroy(buf);
    fclose(file);

    return 0;
}
