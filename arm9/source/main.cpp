#include "main.h"

void vblank() {
    Gui::CURRENT_FRAME++;

    scanKeys();
    int keys = keysDown();

    if (Gui::QUEUED_REDRAW || keys || !(Gui::CURRENT_FRAME & 0xF)) {
        /*for (int x = SCREEN_WIDTH - 1; x >= 0; x--) {
            for (int y = SCREEN_HEIGHT - 1; y >= 0; y--) {
                Gui::DISPLAY_TOP[x + (y << 8)] = ARGB16(1, 31, 0, 0);
                Gui::DISPLAY_BOTTOM[x + (y << 8)] = ARGB16(1, 0, 31, 0);
            }
        }*/

        if (keys & KEY_START) {
            exit(0);
        }

        touchRead(&Gui::TOUCH_POS);

        if (false) {
            // print at using ansi escape sequence \x1b[line;columnH
            printf("\x1b[10;0HFrame = %d", Gui::CURRENT_FRAME);
            printf("\x1b[16;0HTouch x = %04X, %04X\n", Gui::TOUCH_POS.rawx, Gui::TOUCH_POS.px);
            printf("Touch y = %04X, %04X\n", Gui::TOUCH_POS.rawy, Gui::TOUCH_POS.py);

            GuiExitCode exitCode = Gui::MENU_STACK.back()(keys);
        }

        Gui::QUEUED_REDRAW = false;
    }
}

int main() {
    Font::init();

    try {
        printf("Testing errors\n");

        TCPSocket socket;
        socket.open("192.168.1.108", 8234);
        socket.loadSimpleIconTest();
        printf("Error not thrown!\n");
    } catch (const char *e) {
        printf("Caught exception!\n");
        printf("Message: %s\n", e);
    }

    Gui::MENU_STACK.push_back(gui_loading);

    irqSet(IRQ_VBLANK, vblank);

    while (true) {
        swiWaitForVBlank();
        if (Gui::CURRENT_FRAME == 10)  {
            Gui::error("Hello World! 0123");
        }
    }
}

int min(int a, int b)   {
    return a < b ? a : b;
}

int max(int a, int b)   {
    return a > b ? a : b;
}

int clamp(int a, int min, int max)  {
    return a < min ? min : a > max ? max : a;
}
