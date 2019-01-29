#include "netutil.h"

void TCPSocket::open(const char *url, unsigned short port) {
    ensureWifiStarted();
    hostent *host = gethostbyname(url);
    this->open(host, port);
}

size_t TCPSocket::receive(char *buffer, size_t size) {
    size_t total = 0, n = 0;
    while ((n = recv(this->socketId, buffer + total, size - total - 1, 0)) > 0) {
        total += n;
    }
    //buffer[total] = 0;
    return total;
}

void TCPSocket::open(hostent *host, unsigned short port) {
    ensureWifiStarted();
    if (this->isConnected()) {
        throw "Already connected!";
    }
    this->socketId = socket(AF_INET, SOCK_STREAM, 0);
    {
        //configure and start socket

        //make socket non-blocking
        //fcntl(this->socketId, F_SETFL, O_NONBLOCK);

        //enable so_keepalive
        int i = 1;
        setsockopt(this->socketId, SOL_SOCKET, SO_KEEPALIVE, &i, sizeof i);

        //actually connect
        sockaddr_in sain;
        sain.sin_family = AF_INET;
        sain.sin_port = htons(port);
        sain.sin_addr.s_addr = *((unsigned long *) (host->h_addr_list[0]));
        if (connect(this->socketId, (sockaddr *) &sain, sizeof sain) < 0) {
            throw "Unable to connect to server!";
        }
    }
}

void TCPSocket::close() {
    if (this->socketId != -1) {
        closesocket(this->socketId);
        this->socketId = -1;
    }
}

bool TCPSocket::isConnected() {
    return this->socketId != -1 && isWifiConnected();
}

void TCPSocket::loadSimpleIconTest(int num) {
    send(this->socketId, &num, 4, 0);

    for (int x = SCREEN_WIDTH - 1; x >= 0; x--) {
        for (int y = SCREEN_HEIGHT - 1; y >= 0; y--) {
            Gui::DISPLAY_TOP[x + (y << 8)] = ARGB16(1, 0, 0, 0);
            Gui::DISPLAY_BOTTOM[x + (y << 8)] = ARGB16(1, 0, 0, 0);
        }
    }

    char *pixels = new char[32 * 32 * 3];

    Gui::drawText(5, 5, ARGB16(1, 31, 0, 0), BOTTOM, "Receiving data...");
    this->receive(pixels, 32 * 32 * 3);

    char* text = new char[256];

    Gui::drawText(5, 15, ARGB16(1, 31, 0, 0), BOTTOM, "Receiving text...");
    this->receive(text, 256);

    Gui::drawText(5, 25, ARGB16(1, 31, 0, 0), BOTTOM, "Drawing text...");
    int i = 0;
    i += Gui::drawText(5, 5, ARGB16(1, 31, 31, 31), BOTTOM, text);
    i += Gui::drawText(5, 15, ARGB16(1, 31, 31, 31), BOTTOM, text + i);
    i += Gui::drawText(5, 25, ARGB16(1, 31, 31, 31), BOTTOM, text + i);

    Gui::drawText(5, 35, ARGB16(1, 31, 0, 0), BOTTOM, "Drawing icon...");
    i = 0;
    for (int x = 31; x >= 0; x--) {
        for (int y = 31; y >= 0; y--) {
            Gui::DISPLAY_TOP[x | (y << 8)] = (u16) (((pixels[i] & 0x80) ? 0x8000 : 0) | (pixels[i++] << 10) |
                                                    (pixels[i++] << 5) | pixels[i++]);
        }
    }

    Gui::drawText(5, 45, ARGB16(1, 31, 0, 0), BOTTOM, "Done!");
    delete text;
    delete pixels;
}

void ensureWifiStarted() {
    if (!wifiStarted || !isWifiConnected()) {
        if (Wifi_InitDefault(WFC_CONNECT)) {
            wifiStarted = true;
        } else {
            throw "Couldn't connect to WiFi!";
        }
    }
}

bool isWifiConnected() {
    return Wifi_AssocStatus() == ASSOCSTATUS_ASSOCIATED;
}
