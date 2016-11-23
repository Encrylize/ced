#include <curses.h>
#include <ctype.h>
#include <stdbool.h>

#include "alloc.h"
#include "buffer.h"
#include "editor.h"
#include "view.h"

#define CTRL(ch) ((ch) & 037)
#define redraw_current_line(e)                                                \
    do {                                                                      \
        view_redraw_line(e->view, e->buf->cur_line,                           \
                         view_get_col(e->view, e->buf->col));                 \
    } while (0)


Editor *editor_new(char *filename) {
    Editor *e = malloc_or_exit(sizeof(Editor));

    e->buf = buffer_new(filename);
    e->view = view_new(0, getmaxy(stdscr) - 1, 0, 0, e->buf->root_line);
    e->status = newwin(1, 0, getmaxy(stdscr) - 1, 0);

    /* TODO: This stuff needs a better home. */
    wbkgd(e->status, COLOR_PAIR(1));
    wrefresh(e->status);

    if (buffer_read_file(e->buf) != 0)
        editor_set_status(e, "Could not read file");
    view_redraw_all_lines(e->view);

    return e;
}

void editor_destroy(Editor *e) {
    buffer_destroy(e->buf);
    view_destroy(e->view);
    delwin(e->status);
    free(e);
}

void editor_handle_key(Editor *e, int key) {
    switch (key) {
        case KEY_LEFT:
            buffer_move_rel(e->buf, -1, 0);
            break;
        case KEY_RIGHT:
            buffer_move_rel(e->buf, 1, 0);
            break;
        case KEY_UP:
            buffer_move_rel(e->buf, 0, -1);
            break;
        case KEY_DOWN:
            buffer_move_rel(e->buf, 0, 1);
            break;
        case KEY_BACKSPACE:
            editor_handle_backspace(e);
            break;
        case '\n':
            editor_handle_enter(e);
            break;
        case '\t':
            editor_handle_tab(e);
            break;
        /* TODO: Create editor_write function. */
        case CTRL('s'):
            if (buffer_write_file(e->buf) == 0)
                editor_set_status(e, "Saved to file");
            else
                editor_set_status(e, "Could not write to file");
            break;
        default:
            if (isprint(key))
                editor_handle_printable(e, (char) key);
    }
}

void editor_handle_backspace(Editor *e) {
    if (e->buf->row == 0) {
        /* Other lines will be affected, because
         * a newline is being deleted, so we'll
         * redraw them.
         */
        buffer_delete_char(e->buf);
        view_redraw_lines(e->view, e->buf->cur_line,
                          view_get_col(e->view, e->buf->col));
    } else {
        if (e->buf->cur_line->content[e->buf->row - 1] == ' ')
            buffer_delete_spaces(e->buf);
        else
            buffer_delete_char(e->buf);

        redraw_current_line(e);
    }
}

void editor_handle_enter(Editor *e) {
    buffer_insert_char(e->buf, '\n');
    /* Redraw the previous line and everything below. */
    view_redraw_lines(e->view, e->buf->cur_line->prev,
                      view_get_col(e->view, e->buf->col - 1));
}

void editor_handle_tab(Editor *e) {
    buffer_insert_tab(e->buf);
    redraw_current_line(e);
}

void editor_handle_printable(Editor *e, char ch) {
    buffer_insert_char(e->buf, ch);
    redraw_current_line(e);
}

bool editor_adjust_view(Editor *e) {
    size_t cols = getmaxy(e->view->win) - 1;
    size_t rows = getmaxx(e->view->win) - 1;
    size_t scroll_cols = 0;
    size_t scroll_rows = 0;

    if (e->buf->col > (e->view->top_col + cols))
        scroll_cols = view_get_col(e->view, e->buf->col) - cols;
    else if (e->buf->col < e->view->top_col)
        scroll_cols = view_get_col(e->view, e->buf->col);

    if (e->buf->row > (e->view->top_row + rows))
        scroll_rows = view_get_row(e->view, e->buf->row) - rows;
    else if (e->buf->row < e->view->top_row)
        scroll_rows = view_get_row(e->view, e->buf->row);

    view_scroll(e->view, e->buf->cur_line, scroll_rows, scroll_cols);

    return scroll_cols != 0 || scroll_rows != 0;
}

void editor_redraw_view(Editor *e) {
    if (editor_adjust_view(e))
        view_redraw_all_lines(e->view);
    view_update_cursor(e->view, e->buf->row, e->buf->col);
}

void editor_set_status(Editor *e, char *format, ...) {
    va_list args;
    va_start(args, format);

    wmove(e->status, 0, 0);
    vwprintw(e->status, format, args);
    wclrtoeol(e->status);
    wrefresh(e->status);

    view_update_cursor(e->view, e->buf->row, e->buf->col);
}
