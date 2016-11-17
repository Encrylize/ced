#ifndef _VIEW_H
#define _VIEW_H

#include <curses.h>
#include <stdbool.h>

#include "buffer.h"

#define view_get_cursor_col(view) (view->buf->col - view->top_col)
#define view_get_cursor_row(view) (view->buf->row - view->top_row)
#define view_redraw_full(view)                                                \
    do {                                                                      \
        view_redraw(view, view->top_line, 0);                                 \
        view_redraw_cursor(view);                                             \
    } while (0)

#define view_redraw_cursor(view)                                              \
    do {                                                                      \
        wmove(view->win,                                                      \
              view_get_cursor_col(view), view_get_cursor_row(view));          \
        wrefresh(view->win);                                                  \
    } while (0)


typedef struct View {
    WINDOW *win;
    Buffer *buf;
    Line *top_line;
    size_t top_row;
    size_t top_col;
} View;

View *view_new(int rows, int cols, int begin_col, int begin_row, char *filename);
void view_destroy(View *view);
bool view_adjust(View *view);
void view_scroll(View *view, int row, int col);
void view_redraw_line(View *view, Line *line, int col);
void view_redraw(View *view, Line *line, int col);


#endif
