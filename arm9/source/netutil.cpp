#include "netutil.h"

void TCPSocket::open(const char *url, unsigned short port) {
    ensureWifiStarted();
    hostent *host = gethostbyname(url);
    this->open(host, port);
}

size_t TCPSocket::receive(char *buffer, size_t size) {
    size_t total = 0, n = 0;
    while((n = recv(this->socketId, buffer + total, size - total - 1, 0)) > 0) {
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
        if (connect(this->socketId, (sockaddr *) &sain, sizeof sain) < 0)   {
            throw "Unable to connect to server!";
        }
    }
}

void TCPSocket::close() {
    if (this->socketId != -1){
        closesocket(this->socketId);
        this->socketId = -1;
    }
}

bool TCPSocket::isConnected() {
    return this->socketId != -1 && isWifiConnected();
}

void TCPSocket::loadSimpleIconTest() {
    if (false) {
        char *pixels = new char[512];
        char *paletteBuf = new char[16 * 3];

        this->receive(paletteBuf, 16 * 3);
        //this->receive(paletteA, 32);
        this->receive(pixels, 512);

        u16 *palette = new u16[16];
        for (int i = 15; i >= 0; i--) {
            palette[i] = (u16) ((paletteBuf[i * 3] << 10) | (paletteBuf[i * 3 + 1] << 5) | paletteBuf[i * 3 + 2] |
                                ((paletteBuf[i * 3] & 0x80) ? 0x8000 : 0));
        }

        for (int x = 31; x >= 0; x--) {
            for (int y = 31; y >= 0; y--) {
                Gui::DISPLAY_TOP[x | (y << 8)] = palette[x & 0xF];

                int tileX = x >> 3;
                int tileY = y >> 3;
                int b = pixels[(((tileY << 2) | tileX) << 5) | ((y & 0x7) << 2) | ((x >> 1) & 0x3)] & 0xFF;
                Gui::DISPLAY_TOP[x | (y << 8)] = palette[(b >> ((x & 1) == 0 ? 0 : 4)) & 0xF];
            }
        }

        delete pixels;
        delete paletteBuf;
        delete palette;
    } else if (true)    {
        char* pixels = new char[32 * 32 * 3];
        this->receive(pixels, 32 * 32 * 3);

        int i = 0;
        for (int x = 31; x >= 0; x--) {
            for (int y = 31; y >= 0; y--) {
                Gui::DISPLAY_TOP[x | (y << 8)] = (u16) (((pixels[i] & 0x80) ? 0x8000 : 0) | (pixels[i++] << 10) | (pixels[i++] << 5) | pixels[i++]);
            }
        }

        delete pixels;
    }
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
