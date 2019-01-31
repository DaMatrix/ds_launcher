#include "gui.h"

u16* getStuff(bool top, bool temp) {
    if (top) {
        if (temp)   {
            int id = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
            bgHide(id);
            return bgGetGfxPtr(id);
        }else {
            powerOn(POWER_ALL_2D);

            videoSetMode(MODE_3_2D);
            videoSetModeSub(MODE_3_2D);

            return bgGetGfxPtr(bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0));
        }
    } else {
        if (temp)   {
            int id = bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
            bgHide(id);
            return bgGetGfxPtr(id);
        }else {
            return bgGetGfxPtr(bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0));
        }
    }
}

u16* Gui::DISPLAY_TOP = getStuff(true, false);
u16* Gui::DISPLAY_BOTTOM = getStuff(false, false);
u16* Gui::TEMP_DISPLAY_TOP = getStuff(true, true);
u16* Gui::TEMP_DISPLAY_BOTTOM = getStuff(false, true);
int Gui::CURRENT_FRAME = 0;
touchPosition Gui::TOUCH_POS;
std::vector<MenuRenderer> Gui::MENU_STACK;
std::vector<InputHandler> Gui::HANDLERS;
int Gui::KEYS_DOWN;
bool Gui::QUEUED_REDRAW = true;

Console* Console::TOP = new Console(Screen::TOP);
Console* Console::BOTTOM = new Console(Screen::BOTTOM);

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
    register u16* p = screen == TOP ? TEMP_DISPLAY_TOP : TEMP_DISPLAY_BOTTOM;
    for (register int xx = w - 1; xx >= 0; xx--) {
        for (register int yy = h - 1; yy >= 0; yy--) {
            p[x + xx + ((y + yy) << 8)] = argb;
        }
    }
}

int Gui::drawText(int x, int y, u16 argb, Screen screen, const char* text)   {
    //i don't even know if register does anything on the nds or if i'm overusing it but hey why not
    register int i = 0;
    register char c;
    register u16* p = screen == TOP ? TEMP_DISPLAY_TOP : TEMP_DISPLAY_BOTTOM;
    int origX = x;
    while ((c = text[i++]))    {
        register unsigned int size = Font::SIZES[c];
        if (size == 0)    {
            if (c == '\n') {
                y += 10;
                x = origX;
            }
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

void Gui::clear() {
    for (int x = SCREEN_WIDTH - 1; x >= 0; x--) {
        for (int y = SCREEN_HEIGHT - 1; y >= 0; y--)    {
            int i = x | (y << 8);
            Gui::TEMP_DISPLAY_TOP[i] = 0;
            Gui::TEMP_DISPLAY_BOTTOM[i] = 0;
        }
    }
    Console::TOP->row = Console::BOTTOM->row = 0;
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

void Console::print(int x, int y, char *text) {
    Gui::drawText(x, y, ARGB16(1, 31, 31, 31), this->screen, text);
}

void Console::dprint(int x, int y, char *text) {
    this->print(x, y, text);
    delete text;
}

void Console::printf(int x, int y, char *text, ...) {
    va_list v1;
    va_start(v1, text);
    size_t size = vsnprintf(nullptr, 0, text, v1) + 1;
    char *result = new char[size];
    vsnprintf(result, size, text, v1);
    this->dprint(x, y, result);
}

void Console::dprintf(int x, int y, char *text, ...) {
    va_list v1;
    va_start(v1, text);
    size_t size = vsnprintf(nullptr, 0, text, v1) + 1;
    char *result = new char[size];
    vsnprintf(result, size, text, v1);
    this->dprint(x, y, result);
    delete text;
}

void Console::print(char *text) {
    this->print(5, (this->row++) * 10 + 5, text);
}

void Console::printf(char *text, ...) {
    va_list v1;
    va_start(v1, text);
    size_t size = vsnprintf(nullptr, 0, text, v1) + 1;
    char *result = new char[size];
    vsnprintf(result, size, text, v1);
    this->print(5, (this->row++) * 10 + 5, result);
    delete result;
}
