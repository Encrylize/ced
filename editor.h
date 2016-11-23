#ifndef _EDITOR_H
#define _EDITOR_H

#include <curses.h>

#include "buffer.h"
#include "view.h"


typedef struct Editor {
    View *view;
    Buffer *buf;
    WINDOW *status;
} Editor;

Editor *editor_new(char *filename);
void editor_destroy(Editor *e);
void editor_handle_key(Editor *e, int key);
void editor_handle_backspace(Editor *e);
void editor_handle_enter(Editor *e);
void editor_handle_tab(Editor *e);
void editor_handle_printable(Editor *e, char ch);
bool editor_adjust_view(Editor *e);
void editor_redraw_view(Editor *e);
void editor_set_status(Editor *e, char *message, ...);


#endif
