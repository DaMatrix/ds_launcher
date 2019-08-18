#include "main.h"
#include <porklib/crypto/digest/sha1.h>

void vblank() {
    Gui::CURRENT_FRAME++;

    scanKeys();
    Gui::KEYS_DOWN |= keysDown();
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

typedef struct {
    int w;
    int h;
    u16 pixels[32 * 32];
} Icon;

int main() {
    Font::init();
    irqSet(IRQ_VBLANK, vblank);

    Gui::HANDLERS.push_back([](int keys, touchPosition *touch) -> void {
        if (keys & KEY_START) {
            Console::TOP->print("Closing socket...");
            Socket::INSTANCE.close();
            //exit(0);
        }
    });

    Console::TOP->print("Waiting...");
    while (Gui::CURRENT_FRAME < 200) swiWaitForVBlank();

    try {
        Console::TOP->print("Connecting to server...");
        Socket::INSTANCE.open("192.168.1.108", 8234);
        Console::TOP->print("Connected!");

        Gui::MENU_STACK.push_back(gui_loading);

        int counter = 0;
        while (true) {
            swiWaitForVBlank();
            if (Gui::KEYS_DOWN & KEY_A) {
                Message *response = Socket::INSTANCE.sendAndWaitForResponse(new Message(2, "Hello server!"));
                Console::BOTTOM->printf("Server response: %s", response->data);
                delete response;
            } else if (Gui::KEYS_DOWN & KEY_B) {
                Message *response = Socket::INSTANCE.sendAndWaitForResponse(new Message(3, (char*) &counter, 4, true));
                counter++;
                Icon* icon = (Icon*) response->data;
                Console::TOP->printf("Loading %dx%d bitmap...", icon->w, icon->h);
                for (int x = icon->w - 1; x >= 0; x--)    {
                    for (int y = icon->h - 1; y >= 0; y--)    {
                        Gui::TEMP_DISPLAY_BOTTOM[x | (y << 8)] = icon->pixels[(icon->w - 1 - x) * icon->w + icon->h - 1 - y];
                    }
                }
                Console::TOP->print("Done!");
                delete response;
                Gui::QUEUED_REDRAW = true;
            }

            Gui::KEYS_DOWN = 0;
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
