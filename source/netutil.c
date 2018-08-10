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
    char* request_text = (char*) malloc((50 + strlen(url) + 23) * sizeof(char));
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

void downloadFile(Entry entry) {
    char* request_text = (char*) malloc((91) * sizeof(char));
    char* idHex = encodeHex(entry.id);
    sprintf(request_text, "GET /download/%s HTTP/1.1\r\nHost: ds-store.daporkchop.net\r\nUser-Agent: Nintendo DS\r\n\r\n", idHex);
    
    iprintf("%s\n", idHex);
    iprintf("%s", request_text);
    free(idHex);

    // Create a TCP socket
    int my_socket;
    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    iprintf("Created Socket!\n");

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
    iprintf("Saving inbound data...\n");

    int received = 0;
    char incoming_buffer[2];
    char* lengthBuffer = (char*) malloc(8 * sizeof (char));
    int stage = 0;
    Buffer buf;
    FILE* fp;
    char* filename;
    char* tempfilename;
    int totalLength;

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
                    iprintf("Reading %u bytes (hex: %s)...\n", buf.size, lengthBuffer);
                    received = 1;
                }
                break;
            case 2:
                insertArray(&buf, incoming_buffer[0]);
                if (received++ == buf.size) {
                    received = 0;
                    stage++;
                    filename = (char*) malloc(buf.size * sizeof (char));
                    sprintf(filename, "%s", buf.array);
                    tempfilename = (char*) malloc((buf.size + 5) * sizeof (char));
                    sprintf(tempfilename, "%s.temp", filename);
                    fp = fopen(tempfilename, "wb");
                    freeArray(&buf);
                    initArray(&buf, 256);

                    iprintf("Downloading %s\n", tempfilename);
                }
                break;
            case 3:
                lengthBuffer[received++] = incoming_buffer[0];
                if (received == 8) {
                    totalLength = decodeHex(lengthBuffer);
                    stage++;
                    iprintf("Reading %u bytes (hex: %s)...\n", buf.size, lengthBuffer);
                    free(lengthBuffer);
                    received = 0;
                }
                break;
            case 4:
                insertArray(&buf, incoming_buffer[0]);
                if (((++received) & 0xFF) == 0) {
                    //flush buffer
                    buf.used = 0;
                    fwrite(buf.array, 1, 256, fp);
                    iprintf("Downloaded %09d/%09d bytes", received, totalLength);
                } else if (received == totalLength) {
                    fwrite(buf.array, 1, buf.used, fp);
                    freeArray(&buf);
                    fclose(fp);
                    goto LOOP;
                }
        }
    }

LOOP:
    while (0) {
    }

    remove(filename);
    rename(tempfilename, filename);

    free(filename);
    free(tempfilename);

    iprintf("Other side closed connection!\n");

    shutdown(my_socket, 0); // good practice to shutdown the socket.
    closesocket(my_socket); // remove the socket.

    iprintf("Done!\n");
    return buf.array;
}
