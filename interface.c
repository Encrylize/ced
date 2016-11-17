#include <curses.h>
#include <stdbool.h>
#include <ctype.h>

#include "buffer.h"
#include "view.h"

#define KEY_ESCAPE 27
#define CTRL(ch) ((ch) & 037)
#define set_status(content, ...)                                              \
    do {                                                                      \
        mvwprintw(status, 0, 0, content, ##__VA_ARGS__);                      \
        wclrtoeol(status);                                                    \
        wrefresh(status);                                                     \
        view_redraw_cursor(view);                                             \
    } while (0)


WINDOW *status;
View *view;

void handle_key(int key) {
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
                view_redraw(view, view->buf->cur_line,
                            view_get_cursor_col(view));
            } else {
                buffer_delete_char(view->buf);
                view_redraw_line(view, view->buf->cur_line,
                                 view_get_cursor_col(view));
            }
            break;
        case '\n':
            buffer_insert_char(view->buf, '\n');
            /* Redraw the previous line and everything below. */
            view_redraw(view, view->buf->cur_line->prev,
                        view_get_cursor_col(view) - 1);
            break;
        case CTRL('s'):
            if (buffer_write_file(view->buf) == 0)
                set_status("Saved to file");
            else
                set_status("Could not write to file");
            break;
        default:
            if (isprint(key)) {
                buffer_insert_char(view->buf, (char) key);
                view_redraw_line(view, view->buf->cur_line,
                                 view_get_cursor_col(view));
            }
    }
}

void init_screen() {
    initscr();
    raw();
    noecho();

    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
}

void init_status() {
    status = newwin(1, 0, getmaxy(stdscr) - 1, 0);
    wbkgd(status, COLOR_PAIR(1));
    wrefresh(status);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./interface <filename>\n");
        exit(1);
    }

    int key;

    init_screen();
    refresh();
    init_status();
    view = view_new(0, getmaxy(stdscr) - 1, 0, 0, argv[1]);

    if (buffer_read_file(view->buf) != 0)
        set_status("Could not read file");
    view_redraw_full(view);

    while ((key = wgetch(view->win)) != KEY_ESCAPE) {
        handle_key(key);

        if (view_adjust(view))
            view_redraw_full(view);
        else
            view_redraw_cursor(view);
    }

    endwin();
    view_destroy(view);

    return 0;
}
