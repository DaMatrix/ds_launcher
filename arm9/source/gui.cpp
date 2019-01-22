#include "gui.h"

GuiExitCode gui_loading(int keys)  {
    //printf("\x1b[19;0HLoading %d      \n", keysHeld());
    return NOTHING;
}

void error(char* msg)   {
    /*Gui gui = [](int keys) -> GuiExitCode {
        drawRect(8, 16, 8, 64, ARGB16(1, 0, 0, 31), TOP);
        return NOTHING;
    };
    guiStack.push_back(gui);*/
}

void drawRect(int x, int y, int w, int h, u16 argb, Screen screen)  {
    /*if (x < 0)  {
        w += x;
        x = 0;
    }
    if (y < 0)  {
        h += y;
        y = 0;
    }
    w = min(w, SCREEN_WIDTH - x);
    h = min(h, SCREEN_HEIGHT - y);
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT || w <= 0 || h <= 0)    {
        return;
    }*/
    //u16* p = screen == TOP ? DISPLAY_TOP : DISPLAY_BOTTOM;
    /*do {
        do {
            p[x + w + ((y + h) << 8)] = argb;
        } while (--h >= 0);
    } while (--w >= 0);*/
    for (int xx = 32; xx >= 0; xx--)    {
        for (int yy = 32; yy >= 0; yy--)    {
            DISPLAY_TOP[xx + 16 + ((yy + 16) << 8)] = ARGB16(1, 0, 0, 31);
        }
    }
}
