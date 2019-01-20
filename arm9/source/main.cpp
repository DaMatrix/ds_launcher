#include "main.h"

volatile int frame = 0;

void vblank() {
    frame++;
}

int main() {
    touchPosition touchXY;
    std::string lastPressed = "None  ";

    irqSet(IRQ_VBLANK, vblank);

    consoleDemoInit();

    printf("      Hello World!\n");
    printf("     \x1b[32mwww.devkitpro.org\n");
    printf("   \x1b[32;1mwww.daporkchop.net\x1b[39m");

    while (true) {
        swiWaitForVBlank();
        scanKeys();
        int keys = keysDown();
        if (keys & KEY_START) {
            return 0;
        } else if (keys & KEY_A) {
            lastPressed = "A     ";
        } else if (keys & KEY_B) {
            lastPressed = "B     ";
        } else if (keys & KEY_X) {
            lastPressed = "X     ";
        } else if (keys & KEY_Y) {
            lastPressed = "Y     ";
        } else if (keys & KEY_L) {
            lastPressed = "L     ";
        } else if (keys & KEY_R) {
            lastPressed = "R     ";
        } else if (keys & KEY_UP) {
            lastPressed = "Up    ";
        } else if (keys & KEY_DOWN) {
            lastPressed = "Down  ";
        } else if (keys & KEY_LEFT) {
            lastPressed = "Left  ";
        } else if (keys & KEY_RIGHT) {
            lastPressed = "Right ";
        } else if (keys & KEY_SELECT) {
            lastPressed = "Select";
        }

        touchRead(&touchXY);

        // print at using ansi escape sequence \x1b[line;columnH
        printf("\x1b[10;0HFrame = %d", frame);
        printf("\x1b[16;0HTouch x = %04X, %04X\n", touchXY.rawx, touchXY.px);
        printf("Touch y = %04X, %04X\n", touchXY.rawy, touchXY.py);

        printf("\x1b[19;0HLast pressed: %s\n", lastPressed.c_str());
    }
}
