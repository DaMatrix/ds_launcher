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
    while ((n = recv(this->socketId, buffer + total, size - total, 0)) > 0) {
        total += n;
    }
    //Console::BOTTOM->printf(5, 95, "n=%d, total=%d, val=%d", n, total, buffer[0] & 0xFF);
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
        setsockopt(this->socketId, SOL_SOCKET, SO_LINGER, &i, sizeof i);

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

Message *Socket::sendAndWaitForResponse(Message *message) {
    Console::TOP->printf("Sending request...");
    this->sendWithoutWaiting(message);

    Console::TOP->printf("Reading response...");

    char id = 0;
    Console::TOP->printf("Reading ID...");
    if (this->receive(&id, 1) != 1) {
        throw "Couldn't read ID!";
    } else if (id == 0 && this->receive(&id, 1) != 1) {
        throw "Couldn't read ID!";
    }
    Console::TOP->printf("ID: %d", id);

    int length = 0;
    Console::TOP->printf("Reading Length...");
    if (this->receive((char *) &length, 4) != 4) {
        throw "Couldn't read length!";
    }
    Console::TOP->printf("Length: %d", length);

    if (true) {
        char *text = format("Response length: %d\nResponse ID: %d", length, id & 0xFF);
        Gui::drawText(SCREEN_WIDTH >> 1, 35, ARGB16(1, 0, 31, 0), TOP, text);
        delete text;
    }

    Message *response = new Message(0, nullptr, 0);
    if (length > 0) {
        response->data = new char[length + 1];
        this->receive(response->data, length);
        response->data[length] = 0;
    }

    response->id = id;
    response->len = length;

    if (true) {
        char *text = format("Response ID: %d\nResponse length: %d\nResponse data: %s", response->id & 0xFF, response->len, response->hasContent() ? response->data : "null");
        Gui::drawText(5, 5, ARGB16(1, 0, 31, 0), BOTTOM, text);
        delete text;
    }

    return response;
}

void Socket::sendWithoutWaiting(Message *message) {
    if (message->hasContent() > 0) {
        int total = message->len + 4 + 1 + 1;
        char* temp = new char[total];
        temp[0] = message->id;
        ((int*) (temp + 1))[0] = message->len;
        memcpy(temp + 5, message->data, message->len);
        temp[total - 1] = 0;
        send(this->socketId, message->data, message->len, 0);
        delete temp;
    } else {
        char* temp = new char[2];
        temp[0] = message->id;
        temp[1] = 0;
        send(this->socketId, temp, 2, 0);
        delete temp;
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
