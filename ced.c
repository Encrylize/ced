#include <curses.h>
#include <stdbool.h>
#include <ctype.h>

#include "editor.h"

#define KEY_ESCAPE 27


void init_screen() {
    initscr();
    raw();
    noecho();
}

void init_colors() {
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./interface <filename>\n");
        exit(1);
    }

    int key;

    init_screen();
    init_colors();
    refresh();

    Editor *editor = editor_new(argv[1]);
    editor_redraw_view(editor);

    while ((key = wgetch(editor->view->win)) != KEY_ESCAPE) {
        editor_handle_key(editor, key);
        editor_redraw_view(editor);
    }

    endwin();
    editor_destroy(editor);

    return 0;
}
