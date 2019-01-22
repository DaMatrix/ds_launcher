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

    GuiExitCode exitCode = guiStack.back()(keys);
}

int main() {
    irqSet(IRQ_VBLANK, vblank);

    //consoleDemoInit();
    //lcdSwap();

    try {
        printf("Testing errors\n");

        TCPSocket socket;
        //socket.open("ds-store.daporkchop.net", 12345);
        printf("Error not thrown!\n");
    } catch (const char *e) {
        printf("Caught exception!\n");
        printf("Message: %s\n", e);
    }

    powerOn(POWER_ALL);

    videoSetMode(MODE_3_2D);
    videoSetModeSub(MODE_3_2D);
    DISPLAY_TOP = bgGetGfxPtr(bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0));
    DISPLAY_BOTTOM = bgGetGfxPtr(bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0));
    for (int x = SCREEN_WIDTH - 1; x >= 0; x--) {
        for (int y = SCREEN_HEIGHT - 1; y >= 0; y--)    {
            DISPLAY_TOP[x + (y << 8)] = ARGB16(1, 31, 0, 0);
            DISPLAY_BOTTOM[x + (y << 8)] = ARGB16(1, 0, 31, 0);
        }
    }
    drawRect(0, 0, 0, 0, 0, TOP);

    guiStack.push_back(gui_loading);

    while (true) {
        swiWaitForVBlank();
    }
}
