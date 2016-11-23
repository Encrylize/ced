#ifndef _VIEW_H
#define _VIEW_H

#include <curses.h>
#include <stdbool.h>

#include "buffer.h"

#define view_get_col(view, col) (col - view->top_col)
#define view_get_row(view, row) (row - view->top_row)


typedef struct View {
    WINDOW *win;
    Line *top_line;
    size_t top_row;
    size_t top_col;
} View;

View *view_new(int rows, int cols, int begin_row, int begin_col, Line *top);
void view_destroy(View *view);
void view_scroll(View *view, Line *cur_line, int row, int col);
void view_redraw_line(View *view, Line *line, int col);
void view_redraw_lines(View *view, Line *line, int col);
void view_redraw_all_lines(View *view);
void view_update_cursor(View *view, int row, int col);


#endif
