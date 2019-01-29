#ifndef TEMPLATE_NETUTIL_H
#define TEMPLATE_NETUTIL_H

#include "constants.h"
#include "game.h"
#include "gui.h"

#include <dswifi9.h>
#include <fat.h>

#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>


static bool wifiStarted = false;

class TCPSocket {
private:
    int socketId = -1;
public:
    void open(const char *url, uint16 port);
    void open(hostent *host, uint16 port);
    size_t receive(char *buffer, size_t size);
    void startRequestIndex();
    void download(Game* entry);
    void loadSimpleIconTest(int num);
    void close();
    bool isConnected();
};

void ensureWifiStarted();

bool isWifiConnected();

#endif //TEMPLATE_NETUTIL_H
