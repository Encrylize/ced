#include <curses.h>
#include <stdbool.h>

#include "alloc.h"
#include "buffer.h"
#include "view.h"


View *view_new(int rows, int cols, int begin_row, int begin_col, Line *top) {
    View *view = malloc_or_exit(sizeof(View));

    if (view != NULL) {
        view->win = newwin(cols, rows, begin_col, begin_row);
        view->top_line = top;
        view->top_row = 0;
        view->top_col = 0;
        keypad(view->win, true);
    }

    return view;
}

void view_destroy(View *view) {
    delwin(view->win);
    free(view);
}

void view_scroll(View *view, Line *cur_line, int row, int col) {
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
    else if ((size_t) new_row > cur_line->len)
        view->top_row = cur_line->len - 1;
    else
        view->top_row = new_row;
}

void view_redraw_line(View *view, Line *line, int col) {
    if (line->len > view->top_row)
        mvwprintw(view->win, col, 0, "%.*s",
                  getmaxx(view->win), &line->content[view->top_row]);
    else
        wmove(view->win, col, 0);

    /* If the current line is too long to fit on one
     * line, the cursor will move down to the next line.
     * To avoid clearing the next line, we check if it's
     * still on the right line first.
     */
    int x, y;
    getyx(view->win, y, x);
    if (y == col)
        wclrtoeol(view->win);
}

void view_redraw_lines(View *view, Line *line, int col) {
    for (; line != NULL && col <= getmaxy(view->win); col++) {
        view_redraw_line(view, line, col);
        line = line->next;
    }

    wclrtobot(view->win);
}

void view_redraw_all_lines(View *view) {
    view_redraw_lines(view, view->top_line, 0);
}

void view_update_cursor(View *view, int row, int col) {
    wmove(view->win, view_get_col(view, col), view_get_row(view, row));
    wrefresh(view->win);
}
