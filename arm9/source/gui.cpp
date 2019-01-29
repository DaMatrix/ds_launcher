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
bool Gui::QUEUED_REDRAW = true;

GuiExitCode gui_loading(int keys)  {
    Gui::drawRect(16, 32, 16, 32, ARGB16(1, 0, 0, 31), TOP);
    //printf("\x1b[19;0HLoading %d      \n", keysHeld());
    return NOTHING;
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

int Gui::drawText(int x, int y, u16 argb, Screen screen, const char* text)   {
    //i don't even know if register does anything on the nds or if i'm overusing it but hey why not
    register int i = 0;
    register int c;
    register u16* p = screen == TOP ? DISPLAY_TOP : DISPLAY_BOTTOM;
    while ((c = (unsigned char) text[i++]))    {
        register unsigned int size = Font::SIZES[c];
        if (size == 0)    {
            continue;
        }
        register unsigned int letter = Font::LETTERS[c];
        register int w = size & 0xF;
        register int h = (size >> 4) & 0xF;
        register int off = (size >> 8) & 0xF;
        for (register int xx = w - 1; xx >= 0 ; xx--)    {
            for (register int yy = h - 1; yy >= 0; yy--) {
                if (letter & (1 << (yy * w + xx)))   {
                    p[x + xx + ((y + yy + off) << 8)] = argb;
                }
            }
        }
        x += w + 1;
    }
    return i;
}

void Gui::error(char* msg)   {
    MenuRenderer gui = [=] (int keys) -> GuiExitCode {
        Gui::drawText(64, 64, ARGB16(1, 31, 31, 31), TOP, msg);

        if (false)   {
            Gui::drawText(64, 64 + 10 * 1, ARGB16(1, 31, 31, 31), TOP, "0123456789ABCD");
            Gui::drawText(64, 64 + 10 * 2, ARGB16(1, 31, 31, 31), TOP, "EFGHIJKLMNOPQR");
            Gui::drawText(64, 64 + 10 * 3, ARGB16(1, 31, 31, 31), TOP, "STUVWXYZabcdef");
            Gui::drawText(64, 64 + 10 * 4, ARGB16(1, 31, 31, 31), TOP, "ghijklmnopqrst");
            Gui::drawText(64, 64 + 10 * 5, ARGB16(1, 31, 31, 31), TOP, "uvwxyz!?.,:;-_");
            Gui::drawText(64, 64 + 10 * 6, ARGB16(1, 31, 31, 31), TOP, "()[]+\"*#%&$='");
        }
        return NOTHING;
    };
    Gui::MENU_STACK.push_back(gui);
}
