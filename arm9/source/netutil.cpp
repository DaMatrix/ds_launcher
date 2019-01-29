#include "netutil.h"

Socket Socket::INSTANCE;

Message::~Message() {
    if (this->data != nullptr) {
        delete this->data;
    }
}

bool Message::hasContent() {
    return this->data != nullptr && this->len > 0;
}

void Socket::open(const char *url, unsigned short port) {
    ensureWifiStarted();
    hostent *host = gethostbyname(url);
    this->open(host, port);
}

size_t Socket::receive(char *buffer, size_t size) {
    size_t total = 0, n = 0;
    while ((n = recv(this->socketId, buffer + total, size - total - 1, 0)) > 0) {
        total += n;
    }
    //buffer[total] = 0;
    return total;
}

void Socket::open(hostent *host, unsigned short port) {
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

void Socket::close() {
    if (this->socketId != -1) {
        closesocket(this->socketId);
        this->socketId = -1;
    }
}

bool Socket::isConnected() {
    return this->socketId != -1 && isWifiConnected();
}

Message Socket::sendAndWaitForResponse(Message message) {
    /*char* text = format("A number: %d", 12345);
    Gui::drawText(5, 5, ARGB16(1, 31, 0, 0), BOTTOM, text);
    delete text;
    text = format("%d jef %.2f %s", 12345, 3.146, "\"Hello World!\"");
    Gui::drawText(5, 15, ARGB16(1, 31, 0, 0), BOTTOM, text);
    delete text;*/

    this->sendWithoutWaiting(message);

    unsigned char id;
    this->receive((char*) &id, 1);

    int length;
    this->receive((char*) &length, 4);

    if (true) {
        char *text = format("Response ID: %d\nResponse length: %d", id, length);
        Gui::drawText(5, 5, ARGB16(1, 0, 31, 0), TOP, text);
        delete text;
    }

    char *data = nullptr;
    if (false && length > 0) {
        data = new char[length];
        this->receive(data, length);
    }
    Message response = {id, data, length};
    /*int length = 0;
    this->receive((char *) &length, 4);

    char *text = format("Content length: %d", length);
    Gui::drawText(5, 5, ARGB16(1, 31, 0, 0), BOTTOM, text);
    delete text;*/

    if (true) {
        char *text = format("Response ID: %d\nResponse length: %d\nResponse data: %s", response.id, response.len, "null");//response.data == nullptr ? "null" : response.data);
        Gui::drawText(5, 5, ARGB16(1, 31, 0, 0), BOTTOM, text);
        delete text;
    }

    return response;
}

void Socket::sendWithoutWaiting(Message message) {
    send(this->socketId, &message.id, 1, 0);
    if (message.data != nullptr && message.len > 0) {
        send(this->socketId, message.data, message.len, 0);
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
