#include <netutil.h>

static struct hostent *host;

void initNetworking() {
    iprintf("Initializing Internet connection...\n");

    if (!Wifi_InitDefault(WFC_CONNECT)) {
        iprintf("Failed to connect!");
        return;
    }

    iprintf("Fetching server IP...\n");
    host = gethostbyname("ds-store.daporkchop.net");
    iprintf("Ready!\n\n");
}

char* requestData(char* url) {
    char *request_text[50 + strlen(url) + 23];
    sprintf(request_text, "GET %s HTTP/1.1\r\nHost: ds-store.daporkchop.net\r\nUser-Agent: Nintendo DS\r\n\r\n", url);

    iprintf(request_text);

    // Find the IP address of the server, with gethostbyname
    //struct hostent * myhost = gethostbyname(url);
    struct hostent * myhost = gethostbyname("ds-store.daporkchop.net");
    iprintf("Found IP Address!\n");

    // Create a TCP socket
    int my_socket;
    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    iprintf("Created Socket!\n");

    // Tell the socket to connect to the IP address we found, on port 80 (HTTP)
    struct sockaddr_in sain;
    sain.sin_family = AF_INET;
    sain.sin_port = htons(PORT_NUMBER);
    sain.sin_addr.s_addr = *((unsigned long *) (myhost->h_addr_list[0]));
    connect(my_socket, (struct sockaddr *) &sain, sizeof (sain));
    iprintf("Connected to server!\n");

    // send our request
    send(my_socket, request_text, strlen(request_text), 0);
    iprintf("Sent our request!\n");

    // Print incoming data
    iprintf("Printing incoming data:\n");

    int received = 0;
    char incoming_buffer[2];
    char* lengthBuffer = (char*) malloc(8 * sizeof (char));
    int stage = 0;
    Buffer buf;

    while (recv(my_socket, incoming_buffer, 1, 0) > 0) { // if recv returns 0, the socket has been closed.
        switch (stage) {
            case 0:
                if (incoming_buffer[0] == 127) {
                    stage++;
                }
                break;
            case 1:
                lengthBuffer[received++] = incoming_buffer[0];
                if (received == 8) {
                    initArray(&buf, decodeHex(lengthBuffer));
                    stage++;
                    //data = (char*) malloc((length = decodeHex(lengthBuffer)) * sizeof(char));
                    iprintf("Reading %u bytes (hex: %s)...\n", buf.size, lengthBuffer);
                    free(lengthBuffer);
                    received = 1;
                }
                break;
            case 2:
                //data[received++] = incoming_buffer[0];
                insertArray(&buf, incoming_buffer[0]);
                if (received++ == buf.size) {
                    goto LOOP;
                }
        }
    }

LOOP:
    insertArray(&buf, 0);
    //iprintf("%s\n", buf.array);

    iprintf("Other side closed connection!\n");

    shutdown(my_socket, 0); // good practice to shutdown the socket.
    closesocket(my_socket); // remove the socket.

    iprintf("Done!\n");
    return buf.array;
}
