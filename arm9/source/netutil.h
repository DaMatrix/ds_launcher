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

class Message {
public:
    unsigned char id;
    char* data;
    int len;
    bool isConst;

    Message(unsigned char id, char* data, int len)  {
        this->id = id;
        this->data = data;
        this->len = len;
        this->isConst = false;
    }

    Message(unsigned char id, char* data, int len, bool isConst)  {
        this->id = id;
        this->data = data;
        this->len = len;
        this->isConst = isConst;
    }

    Message(unsigned char id, const char* data)  {
        this->id = id;
        this->data = (char*) data;
        this->len = lengthOf(data);
        this->isConst = true;
    }

    Message(unsigned char id, char* data)  {
        this->id = id;
        this->data = data;
        this->len = lengthOf(data);
        this->isConst = false;
    }

    virtual ~Message();
    bool hasContent();
};

class Socket {
private:
    int socketId = -1;
public:
    //static instance of socket that's used for communicating with the ds-store server
    static Socket INSTANCE;

    //functions
    /**
     * Connects to a remote host
     * @param url  the address of the host to connect to
     * @param port the port to connect on
     */
    void open(const char *url, uint16 port);

    /**
     * Connects to a remote host
     * @param host the host to connect to
     * @param port the port to connect on
     */
    void open(hostent *host, uint16 port);

    /**
     * Reads a given number of bytes into a buffer
     * @param buffer the buffer to read into
     * @param size   the number of bytes to read
     * @return the number of bytes actually read. This will only be different from the size parameter if the socket was closed while trying to read from it
     */
    size_t receive(char *buffer, size_t size);

    /**
     * Sends a request and waits for a response from the server
     * @param message the request to send
     * @return the response from the server
     */
    Message* sendAndWaitForResponse(Message* message);

    /**
     * Sends a request and waits for a response from the server
     * @param message the request to send
     * @param delet   whether or not to delete the message after sending it
     * @return the response from the server
     */
    Message* sendAndWaitForResponse(Message* message, bool delet);

    /**
     * Sends a request without waiting for a response
     * @param message the request to send
     * @return the message that was sent
     */
    Message* sendWithoutWaiting(Message* message);

    /**
     * Closes this socket
     */
    void close();

    /**
     * Checks if this socket is connected
     * @return whether or not this socket is connected
     */
    bool isConnected();
};

void ensureWifiStarted();

bool isWifiConnected();

#endif //TEMPLATE_NETUTIL_H
