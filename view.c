#include <curses.h>
#include <stdbool.h>

#include "buffer.h"
#include "view.h"
#include "alloc.h"


View *view_new(int rows, int cols, int begin_col, int begin_row, char *filename) {
    View *view = malloc_or_exit(sizeof(View));

    if (view != NULL) {
        view->buf = buffer_new(filename);
        view->win = newwin(cols, rows, begin_col, begin_row);
        view->top_line = view->buf->root_line;
        view->top_row = 0;
        view->top_col = 0;
        keypad(view->win, true);
    }

    return view;
}

void view_destroy(View *view) {
    delwin(view->win);
    buffer_destroy(view->buf);
    free(view);
}

bool view_adjust(View *view) {
    size_t cols = getmaxy(view->win) - 1;
    size_t rows = getmaxx(view->win) - 1;
    size_t scroll_cols = 0;
    size_t scroll_rows = 0;

    if (view->buf->col > (view->top_col + cols))
        scroll_cols = view_get_cursor_col(view) - cols;
    else if (view->buf->col < view->top_col)
        scroll_cols = view_get_cursor_col(view);

    if (view->buf->row > (view->top_row + rows))
        scroll_rows = view_get_cursor_row(view) - rows;
    else if (view->buf->row < view->top_row)
        scroll_rows = view_get_cursor_row(view);

    view_scroll(view, scroll_rows, scroll_cols);

    return scroll_cols != 0 || scroll_rows != 0;
}

void view_scroll(View *view, int row, int col) {
    if (col < 0) {
        /* Move up */
        while (view->top_line->prev != NULL && col < 0) {
            view->top_line = view->top_line->prev;
            view->top_col--;
            col++;
        }
    } else {
        /* Move down */
        while (view->top_line->next != NULL && col > 0) {
            view->top_line = view->top_line->next;
            view->top_col++;
            col--;
        }
    }

    int new_row = (int) view->top_row + row;
    if (new_row < 0)
        view->top_row = 0;
    else if ((size_t) new_row > view->buf->cur_line->len)
        view->top_row = view->buf->cur_line->len - 1;
    else
        view->top_row = new_row;
}

void view_redraw_line(View *view, Line *line, int col) {
    if (line->len > view->top_row)
        mvwprintw(view->win, col, 0, "%.*s",
                  getmaxx(view->win), &line->content[view->top_row]);
    else
        wmove(view->win, col, 0);
    wclrtoeol(view->win);
}

void view_redraw(View *view, Line *line, int col) {
    for (; line != NULL && col <= getmaxy(view->win); col++) {
        view_redraw_line(view, line, col);
        line = line->next;
    }

    wclrtobot(view->win);
}
