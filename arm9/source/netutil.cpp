#include "netutil.h"

void TCPSocket::connect(const char *url, uint16 port) {
    ensureWifiStarted();
    hostent *host = gethostbyname(url);
    this->connect(host, port);
}

void TCPSocket::connect(hostent *host, uint16 port) {
    ensureWifiStarted();
    if (this->isConnected()) {
        throw "Already connected!";
    }
    this->socketId = socket(AF_INET, SOCK_STREAM, 0);
}

bool TCPSocket::isConnected() {
    return this->socketId != -1 && isWifiConnected();
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
