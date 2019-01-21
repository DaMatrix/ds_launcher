#ifndef TEMPLATE_REFS_H
#define TEMPLATE_REFS_H

#include <nds.h>

#include <vector>

static int frame = 0;
static touchPosition touchXY;

enum GuiState   {
    CONNECTING = 0,
    CONNECTED = 1
};

enum GuiExitCode    {
    NOTHING = 0,
    BACK = 1,
    LOADING = 2
};

typedef GuiExitCode (* Gui) (int keys);
static std::vector<Gui> stack;

GuiExitCode gui_loading(int keys)  {
    printf("\x1b[19;0HLoading %d      \n", keysHeld());
    //printf("\x1b[19;0HLoading %d      \n", frame);
    return NOTHING;
}

#endif //TEMPLATE_REFS_H
