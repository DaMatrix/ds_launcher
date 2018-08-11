#include <netutil.h>

#include "registry.h"

static struct hostent* host;

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
    char* request_text = (char*) malloc((50 + strlen(url) + 23) * sizeof (char));
    sprintf(request_text, "GET %s HTTP/1.1\r\nHost: ds-store.daporkchop.net\r\nUser-Agent: Nintendo DS\r\n\r\n", url);

    iprintf(request_text);

    // Create a TCP socket
    int my_socket;
    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    iprintf("Created Socket!\n");

    // Tell the socket to connect to the IP address we found, on port 80 (HTTP)
    struct sockaddr_in sain;
    sain.sin_family = AF_INET;
    sain.sin_port = htons(PORT_NUMBER);
    sain.sin_addr.s_addr = *((unsigned long *) (host->h_addr_list[0]));
    connect(my_socket, (struct sockaddr *) &sain, sizeof (sain));
    iprintf("Connected to server!\n");

    // send our request
    send(my_socket, request_text, strlen(request_text), 0);
    free(request_text);
    iprintf("Sent our request!\n");

    // Print incoming data
    iprintf("Printing incoming data:\n");

    int received = 0;
    char* incoming_buffer = (char*) malloc(2 * sizeof (char));
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
    free(incoming_buffer);

    iprintf("Other side closed connection!\n");

    shutdown(my_socket, 0); // good practice to shutdown the socket.
    closesocket(my_socket); // remove the socket.

    iprintf("Done!\n");
    return buf.array;
}

void downloadFile(Entry* entry) {
    char* request_text = (char*) malloc((60 + strlen(entry->name) + 23) * sizeof (char));
    sprintf(request_text, "GET /download/%s HTTP/1.1\r\nHost: ds-store.daporkchop.net\r\nUser-Agent: Nintendo DS\r\n\r\n", entry->name);

    iprintf("%s\n", request_text);

    // Create a TCP socket
    int my_socket;
    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    iprintf("Created Socket!\n");

    // Tell the socket to connect to the IP address we found, on port 80 (HTTP)
    struct sockaddr_in sain;
    sain.sin_family = AF_INET;
    sain.sin_port = htons(PORT_NUMBER);
    sain.sin_addr.s_addr = *((unsigned long *) (host->h_addr_list[0]));
    connect(my_socket, (struct sockaddr *) &sain, sizeof (sain));
    iprintf("Connected to server!\n");

    // send our request
    send(my_socket, request_text, strlen(request_text), 0);
    free(request_text);
    iprintf("Sent our request!\n");

    // Print incoming data

    int received = 0;
    int total = 0;
    char* incoming_buffer = (char*) malloc(2 * sizeof (char));
    char* lengthBuffer = (char*) malloc(8 * sizeof (char));
    int stage = 0;
    Buffer buf;
    iprintf("Setting up file names\n");
    char* origFile = (char*) malloc((strlen(entry->name) + 1) * sizeof (char));
    sprintf(origFile, "/%s", entry->name);
    //char* tempFile = (char*) malloc((strlen(entry->name) + 6) * sizeof (char));
    //sprintf(tempFile, "/%s.temp", entry->name);
    iprintf("Opening actual file\n");
    remove(origFile);
    FILE* fp = fopen(origFile, "wb");

    iprintf("Downloading file...\n");
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
                    initArray(&buf, 4096);
                    total = decodeHex(lengthBuffer);
                    stage++;
                    //data = (char*) malloc((length = decodeHex(lengthBuffer)) * sizeof(char));
                    iprintf("Reading %u bytes...\n", total);
                    free(lengthBuffer);
                    received = 0;
                }
                break;
            case 2:
                insertArray(&buf, incoming_buffer[0]);
                if (((++received) & 0xFFF) == 0) {
                    //flush buffer
                    buf.used = 0;
                    fwrite(buf.array, 1, 4096, fp);
                    iprintf("%09d/%09d (%3d%%)\n", received, total, received / (total / 100));
                } else if (received == total) {
                    fwrite(buf.array, 1, buf.used, fp);
                    goto LOOP;
                }
        }
    }

LOOP:
    freeArray(&buf);
    free(incoming_buffer);

    free(origFile);

    iprintf("Other side closed connection!\n");

    shutdown(my_socket, 0); // good practice to shutdown the socket.
    closesocket(my_socket); // remove the socket.

    fclose(fp);

    iprintf("Done! Read %u bytes!\n", total);
}
