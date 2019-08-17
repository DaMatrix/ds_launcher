#ifndef TEMPLATE_REFS_H
#define TEMPLATE_REFS_H

#include "constants.h"
#include "font.h"

#include <vector>
#include <functional>

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

typedef std::function<GuiExitCode (int keys)> MenuRenderer;
typedef std::function<void (int keys, touchPosition* touchPos)> InputHandler;

struct Gui {
    static u16* DISPLAY_TOP;
    static u16* TEMP_DISPLAY_TOP;
    static u16* DISPLAY_BOTTOM;
    static u16* TEMP_DISPLAY_BOTTOM;
    static int CURRENT_FRAME;
    static int KEYS_DOWN;
    static touchPosition TOUCH_POS;
    static std::vector<MenuRenderer> MENU_STACK;
    static std::vector<InputHandler> HANDLERS;
    static bool QUEUED_REDRAW;
public:
    //draw methods
    static void drawRect(int x, int y, int w, int h, u16 argb, Screen screen);
    static int drawText(int x, int y, u16 argb, Screen screen, const char* text);

    static void clear();

    //helper methods
    static void error(char* msg);
};

GuiExitCode gui_loading(int keys);

class Console {
public:
    static Console* TOP;
    static Console* BOTTOM;

    //fields
    Screen screen;
    int row = 0;

    Console(Screen screen)  {
        this->screen = screen;
    }

    void printf(int x, int y, const char* text, ...);
    void print(int x, int y, const char* text);
    void printf(const char* text, ...);
    void print(const char* text);
    void dprintf(int x, int y, const char* text, ...);
    void dprint(int x, int y, const char* text);
};

#endif //TEMPLATE_REFS_H
