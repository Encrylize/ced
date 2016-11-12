#ifndef _VIEW_H
#define _VIEW_H

#include <curses.h>
#include <stdbool.h>

#include "buffer.h"

#define view_get_cursor_col(view) (view->buf->col - view->top_col)
#define view_redraw_line(view, line, col)                                     \
    do {                                                                      \
        mvwprintw(view->win, col, 0, "%s", line->content);                    \
        wclrtoeol(view->win);                                                 \
    } while (0)

#define view_redraw_current_line(view)                                        \
    do {                                                                      \
        view_redraw_line(view, view->buf->cur_line,                           \
                         view_get_cursor_col(view));                          \
    } while (0)

#define view_redraw_to_bottom(view)                                           \
    do {                                                                      \
        view_redraw(view, view->buf->cur_line, view_get_cursor_col(view));    \
    } while (0)

#define view_redraw_full(view)                                                \
    do {                                                                      \
        view_redraw(view, view->top_line, 0);                                 \
        view_redraw_cursor(view);                                             \
    } while (0)

#define view_redraw_cursor(view)                                              \
    do {                                                                      \
        wmove(view->win, view_get_cursor_col(view), view->buf->row);          \
        wrefresh(view->win);                                                  \
    } while (0)


typedef struct View {
    WINDOW *win;
    Buffer *buf;
    Line *top_line;
    size_t top_col;
} View;

View *view_new(int rows, int cols, int begin_col, int begin_row);
void view_destroy(View *view);
bool view_adjust(View *view);
void view_scroll(View *view, int col);
void view_redraw(View *view, Line *line, int col);


#endif
