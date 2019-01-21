#include "main.h"


void vblank() {
    frame++;

    scanKeys();
    int keys = keysDown();
    if (keys & KEY_START) {
        exit(0);
    }

    touchRead(&touchXY);

    // print at using ansi escape sequence \x1b[line;columnH
    printf("\x1b[10;0HFrame = %d", frame);
    printf("\x1b[16;0HTouch x = %04X, %04X\n", touchXY.rawx, touchXY.px);
    printf("Touch y = %04X, %04X\n", touchXY.rawy, touchXY.py);

    GuiExitCode exitCode = stack.back()(keys);
}

int main() {
    irqSet(IRQ_VBLANK, vblank);

    consoleDemoInit();

    try {
        printf("Testing errors\n");

        TCPSocket socket;
        //socket.open("ds-store.daporkchop.net", 12345);
        printf("Error not thrown!\n");
    } catch (const char *e) {
        printf("Caught exception!\n");
        printf("Message: %s\n", e);
    }

    stack.push_back(gui_loading);
    stack.push_back(gui_loading);
    stack.push_back(gui_loading);

    while (true) {
        swiWaitForVBlank();
    }
}
