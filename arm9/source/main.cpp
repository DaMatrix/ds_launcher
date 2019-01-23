#include "main.h"

void vblank() {
    Gui::CURRENT_FRAME++;

    scanKeys();
    int keys = keysDown();
    if (keys & KEY_START) {
        exit(0);
    }

    touchRead(&Gui::TOUCH_POS);

    // print at using ansi escape sequence \x1b[line;columnH
    printf("\x1b[10;0HFrame = %d", Gui::CURRENT_FRAME);
    printf("\x1b[16;0HTouch x = %04X, %04X\n", Gui::TOUCH_POS.rawx, Gui::TOUCH_POS.px);
    printf("Touch y = %04X, %04X\n", Gui::TOUCH_POS.rawy, Gui::TOUCH_POS.py);

    GuiExitCode exitCode = Gui::MENU_STACK.back()(keys);
}

int main() {

    try {
        printf("Testing errors\n");

        TCPSocket socket;
        //socket.open("ds-store.daporkchop.net", 12345);
        printf("Error not thrown!\n");
    } catch (const char *e) {
        printf("Caught exception!\n");
        printf("Message: %s\n", e);
    }

    for (int x = SCREEN_WIDTH - 1; x >= 0; x--) {
        for (int y = SCREEN_HEIGHT - 1; y >= 0; y--)    {
            Gui::DISPLAY_TOP[x + (y << 8)] = ARGB16(1, 31, 0, 0);
            Gui::DISPLAY_BOTTOM[x + (y << 8)] = ARGB16(1, 0, 31, 0);
        }
    }

    Gui::MENU_STACK.push_back(gui_loading);

    irqSet(IRQ_VBLANK, vblank);

    while (true) {
        swiWaitForVBlank();
        if (Gui::CURRENT_FRAME == 100)  {
            error("test!");
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
