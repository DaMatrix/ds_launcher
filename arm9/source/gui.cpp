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

GuiExitCode gui_loading(int keys)  {
    //printf("\x1b[19;0HLoading %d      \n", keysHeld());
    return NOTHING;
}

//void error(char* msg)   {
    /*MenuRenderer gui = [](int keys) -> GuiExitCode {
        drawRect(8, 16, 8, 64, ARGB16(1, 0, 0, 31), TOP);
        return NOTHING;
    };
    guiStack.push_back(gui);*/
//}

void Gui::drawRect(int x, int y, int w, int h, u16 argb, Screen screen)  {
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
            Gui::DISPLAY_TOP[xx + 16 + ((yy + 16) << 8)] = ARGB16(1, 0, 0, 31);
        }
    }
}
