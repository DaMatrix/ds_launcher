#include "main.h"

static int counter = 0;
static TCPSocket theSocket;

void vblank() {
    Gui::CURRENT_FRAME++;

    scanKeys();
    int keys = keysDown();

    if (Gui::QUEUED_REDRAW || keys || !(Gui::CURRENT_FRAME & 0xF)) {
        if (keys & KEY_START) {
            theSocket.close();
            //exit(0);
        }
        touchRead(&Gui::TOUCH_POS);

        //GuiExitCode exitCode = Gui::MENU_STACK.back()(keys);

        Gui::QUEUED_REDRAW = false;
    }
}

int main() {
    Font::init();

    try {
        printf("Testing errors\n");

        theSocket.open("192.168.1.108", 8234);
        printf("Error not thrown!\n");
    } catch (const char *e) {
        printf("Caught exception!\n");
        printf("Message: %s\n", e);
    }

    Gui::MENU_STACK.push_back(gui_loading);

    irqSet(IRQ_VBLANK, vblank);

    while (true) {
        swiWaitForVBlank();
        if (!(Gui::CURRENT_FRAME & 0x7))   {
            theSocket.loadSimpleIconTest();
        }
    }
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int clamp(int a, int min, int max) {
    return a < min ? min : a > max ? max : a;
}
