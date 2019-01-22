#ifndef TEMPLATE_REFS_H
#define TEMPLATE_REFS_H

#include "constants.h"

#include <nds.h>
#include <vector>
#include <stdio.h>

static int frame = 0;
static touchPosition touchXY;

static u16* DISPLAY_TOP;
static u16* DISPLAY_BOTTOM;

#define ICON_SIZE 32 //the side length of a game icon

enum GuiState   {
    CONNECTING = 0,
    CONNECTED = 1
};

enum GuiExitCode    {
    NOTHING = 0,
    BACK = 1,
    LOADING = 2
};

enum Screen {
    TOP = 0,
    BOTTOM = 1
};

typedef GuiExitCode (* Gui) (int keys);
static std::vector<Gui> guiStack;

GuiExitCode gui_loading(int keys);

void error(char* msg);

//draw methods
void drawRect(int x, int y, int w, int h, u16 argb, Screen screen);

#endif //TEMPLATE_REFS_H
