#ifndef _INTERFACE_H

#include <stdbool.h>
#include <stdio.h>

#include "view.h"

#define KEY_ESCAPE 27
#define KEY_ENTER_2 10

#define CTRL(ch) ((ch) & 037)


void interface_handle_key(View *view, int key);


#endif
