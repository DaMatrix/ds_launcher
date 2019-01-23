#include "gui.h"

u16* getStuff(bool top) {
    if (top) {
        powerOn(POWER_ALL_2D);

        videoSetMode(MODE_3_2D);
        videoSetModeSub(MODE_3_2D);

        return bgGetGfxPtr(bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0));
    } else {
        return bgGetGfxPtr(bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0));
    }
}

u16* Gui::DISPLAY_TOP = getStuff(true);
u16* Gui::DISPLAY_BOTTOM = getStuff(false);
int Gui::CURRENT_FRAME = 0;
touchPosition Gui::TOUCH_POS;
std::vector<MenuRenderer> Gui::MENU_STACK;

GuiExitCode gui_loading(int keys)  {
    Gui::drawRect(16, 32, 16, 32, ARGB16(1, 0, 0, 31), TOP);
    //printf("\x1b[19;0HLoading %d      \n", keysHeld());
    return NOTHING;
}

void error(char* msg)   {
    MenuRenderer gui = [=] (int keys) -> GuiExitCode {
        Gui::drawRect(8, 16, 8, 64, ARGB16(1, 0, 31, 31), TOP);
        return NOTHING;
    };
    Gui::MENU_STACK.push_back(gui);
}

void Gui::drawRect(int x, int y, int w, int h, u16 argb, Screen screen)  {
    if (x < 0)  {
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
    }
    register u16* p = screen == TOP ? DISPLAY_TOP : DISPLAY_BOTTOM;
    for (register int xx = w - 1; xx >= 0; xx--) {
        for (register int yy = h - 1; yy >= 0; yy--) {
            p[x + xx + ((y + yy) << 8)] = argb;
        }
    }
}
