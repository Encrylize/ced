#include <curses.h>
#include <stdbool.h>
#include <ctype.h>

#include "buffer.h"
#include "view.h"
#include "interface.h"


void interface_handle_key(View *view, int key) {
    switch (key) {
        case KEY_LEFT:
            buffer_move_rel(view->buf, -1, 0);
            break;
        case KEY_RIGHT:
            buffer_move_rel(view->buf, 1, 0);
            break;
        case KEY_UP:
            buffer_move_rel(view->buf, 0, -1);
            break;
        case KEY_DOWN:
            buffer_move_rel(view->buf, 0, 1);
            break;
        case KEY_BACKSPACE:
            if (view->buf->row == 0) {
                /* Other lines will be affected, because
                 * a newline is being deleted, so we'll
                 * redraw them.
                 */
                buffer_delete_char(view->buf);
                view_redraw_to_bottom(view);
            } else {
                buffer_delete_char(view->buf);
                view_redraw_current_line(view);
            }
            break;
        case KEY_ENTER:
        case KEY_ENTER_2:
            buffer_insert_char(view->buf, '\n');
            /* Redraw the previous line and everything below. */
            view_redraw(view, view->buf->cur_line->prev,
                        view_get_cursor_col(view) - 1);
            break;
        case CTRL('s'):
            buffer_write_file(view->buf);
            break;
        default:
            if (isprint(key)) {
                buffer_insert_char(view->buf, (char) key);
                view_redraw_current_line(view);
            }
    }

    if (view_adjust(view))
        view_redraw_full(view);
    else
        view_redraw_cursor(view);
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

    View *view = view_new(0, 0, 0, 0, argv[1]);
    keypad(view->win, true);
    refresh();

    buffer_read_file(view->buf);
    view_redraw_full(view);

    while ((key = wgetch(view->win)) != KEY_ESCAPE)
        interface_handle_key(view, key);

    endwin();
    buffer_print(view->buf);
    view_destroy(view);

    return 0;
}
