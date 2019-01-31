#include "main.h"

void vblank() {
    Gui::CURRENT_FRAME++;

    scanKeys();
    Gui::KEYS_DOWN = keysDown();
    touchRead(&Gui::TOUCH_POS);

    for (std::vector<InputHandler>::iterator it = Gui::HANDLERS.begin(); it != Gui::HANDLERS.end(); it++) {
        (*it)(Gui::KEYS_DOWN, &Gui::TOUCH_POS);
    }

    if (Gui::QUEUED_REDRAW || !(Gui::CURRENT_FRAME & 0xF)) {
        memcpy(Gui::DISPLAY_TOP, Gui::TEMP_DISPLAY_TOP, 256 * 256 * sizeof(u16));
        memcpy(Gui::DISPLAY_BOTTOM, Gui::TEMP_DISPLAY_BOTTOM, 256 * 256 * sizeof(u16));

        //GuiExitCode exitCode = Gui::MENU_STACK.back()(keys);

        Gui::QUEUED_REDRAW = false;
    }
}

int main() {
    Font::init();

    Gui::HANDLERS.push_back([](int keys, touchPosition *touch) -> void {
        if (keys & KEY_START) {
            Console::TOP->print("Closing socket...");
            Socket::INSTANCE.close();
            //exit(0);
        }
    });

    try {
        Console::TOP->print("Connecting to server...");
        Socket::INSTANCE.open("192.168.1.108", 8234);
        Console::TOP->print("Connected!");

        Gui::MENU_STACK.push_back(gui_loading);

        irqSet(IRQ_VBLANK, vblank);

        while (true) {
            swiWaitForVBlank();
            if (Gui::KEYS_DOWN & KEY_A) {
                Message *response = Socket::INSTANCE.sendAndWaitForResponse(new Message(2, "Hello server!", 13));
                Console::BOTTOM->printf("Server response: %s", response->data);
                delete response;
            }
            if (min(1, 2) == 3) {
                //this is just to trick my IDE into thinking that this loop isn't infinite
                break;
            }
        }
    } catch (const char *e) {
        Gui::drawText(5, 5, ARGB16(1, 31, 0, 0), BOTTOM, e);
    } catch (void *e) {
        Gui::drawText(5, 5, ARGB16(1, 31, 0, 0), BOTTOM, "Exception");
    }
    Socket::INSTANCE.close();
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
