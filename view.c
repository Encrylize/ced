#include <curses.h>
#include <stdbool.h>

#include "buffer.h"
#include "view.h"


View *view_new(int rows, int cols, int begin_col, int begin_row) {
    View *view = malloc(sizeof(View));

    if (view != NULL) {
        view->buf = buffer_new();

        if (view->buf == NULL) {
            free(view);
            return NULL;
        }

        view->win = newwin(cols, rows, begin_col, begin_row);
        view->top_line = view->buf->root_line;
        view->top_col = 0;
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

    if (view->buf->col > (view->top_col + cols))
        view_scroll(view, view_get_cursor_col(view) - cols);
    else if (view->buf->col < view->top_col)
        view_scroll(view, view_get_cursor_col(view));
    else
        return false;

    return true;
}

void view_scroll(View *view, int col) {
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
}

void view_redraw(View *view, Line *line, int col) {
    for (; line != NULL && col <= getmaxy(view->win); col++) {
        view_redraw_line(view, line, col);
        line = line->next;
    }

    wclrtobot(view->win);
}
