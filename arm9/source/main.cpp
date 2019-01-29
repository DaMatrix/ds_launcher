#include "main.h"

void vblank() {
    Gui::CURRENT_FRAME++;

    scanKeys();
    int keys = keysDown();

    if (Gui::QUEUED_REDRAW || keys || !(Gui::CURRENT_FRAME & 0xF)) {
        if (keys & KEY_START) {
            Console::TOP->print("Closing socket...");
            Socket::INSTANCE.close();
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
        Console::TOP->print("Connecting to server...");
        Socket::INSTANCE.open("192.168.1.108", 8234);
        Console::TOP->print("Connected!");

        Gui::MENU_STACK.push_back(gui_loading);

        irqSet(IRQ_VBLANK, vblank);

        //char* value = Socket::INSTANCE.sendAndWaitForResponse({1, "hello world", 12});
        delete Socket::INSTANCE.sendAndWaitForResponse(new Message(2, nullptr, 0));

        Console::TOP->print("Waiting...");
        for (int i = 0; i < 120; i++)   {
            swiWaitForVBlank();
        }

        Gui::clear();
        delete Socket::INSTANCE.sendAndWaitForResponse(new Message(2, "hello world", 12));
        Console::TOP->print("Done!");
        Console::BOTTOM->print("Done!");
    } catch (const char *e) {
        Gui::drawText(5, 5, ARGB16(1, 31, 0, 0), BOTTOM, e);
    } catch (void* e) {
        Gui::drawText(5, 5, ARGB16(1, 31, 0, 0), BOTTOM, "Exception");
    }
    while (true) {
        swiWaitForVBlank();
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

char *format(char *format, ...) {
    va_list v1;
    va_start(v1, format);
    size_t size = vsnprintf(nullptr, 0, format, v1) + 1;
    char *result = new char[size];
    vsnprintf(result, size, format, v1);
    return result;
}
