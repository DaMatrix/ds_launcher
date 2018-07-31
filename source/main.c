/*---------------------------------------------------------------------------------

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <fat.h>
#include <sys/dir.h>

#include <stdio.h>
#include <string.h>

#define PORT 8236

char* hex = "0123456789abcdef";

char* getHttp(char* addr, char* url);

unsigned int decodeHex(char* in) {
    unsigned int val = 0;
    for (int i = 0; i < 8; i++) {
        char c = in[i];
        for (int j = 0; j < 16; j++) {
            if (hex[j] == c) {
                val |= j << (i << 2);
                break;
            }
        }
    }
    return val;
}

int main(void) {
    consoleDemoInit(); //setup the sub screen for printing

    iprintf("\n\n\tStarting the PorkStore...\n\n");
    iprintf("Initializing filesystem...\n");
    //fatInitDefault();

    iprintf("Initializing Internet connection...\n");

    if (!Wifi_InitDefault(WFC_CONNECT)) {
        iprintf("Failed to connect!");
    } else {
        iprintf("Ready!\n\n");

        getHttp("192.168.1.102", "/index.txt");
    }

    while (1) {
        swiWaitForVBlank();
        int keys = keysDown();
        if (keys) break;
    }

    return 0;
}

char* getHttp(char* addr, char* url) {
    // Let's send a simple HTTP request to a server and print the results!

    // store the HTTP request for later
    char *request_text[50 + strlen(addr) + strlen(url)];
    sprintf(request_text, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Nintendo DS\r\n\r\n", url, addr);

    iprintf(request_text);

    /*const char * request_text =
            "GET  HTTP/1.1\r\n"
            "Host: \r\n"
            "User-Agent: Nintendo DS\r\n\r\n";
    iprintf("%u (hex: %x)", strlen(request_text), strlen(request_text));*/

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
    sain.sin_port = htons(80);
    sain.sin_addr.s_addr = *((unsigned long *) (myhost->h_addr_list[0]));
    connect(my_socket, (struct sockaddr *) &sain, sizeof (sain));
    iprintf("Connected to server!\n");

    // send our request
    send(my_socket, request_text, strlen(request_text), 0);
    iprintf("Sent our request!\n");

    // Print incoming data
    iprintf("Printing incoming data:\n");

    /*int recvd_len;
    char *sizeBuf = malloc(sizeof (*sizeBuf) * 8);
    unsigned int total_len = -1;
    unsigned int read_len = 0;

    while ((recvd_len = recv(my_socket, sizeBuf, 8, 0)) != 0) { // if recv returns 0, the socket has been closed.
        if (recvd_len > 0) { // data was received!
            read_len += recvd_len;
            if (read_len == 8) {
                unsigned int fullLength = decodeHex(sizeBuf);
                iprintf("%u (hex: %x)", fullLength, fullLength);
                free(sizeBuf);
                break;
            }
        }
    }*/

    int recvd_len;
    char incoming_buffer[256];

    while ((recvd_len = recv(my_socket, incoming_buffer, 255, 0)) != 0) { // if recv returns 0, the socket has been closed.
        if (recvd_len > 0) { // data was received!
            incoming_buffer[recvd_len] = 0; // null-terminate
            iprintf(incoming_buffer);
        }
    }

    iprintf("Other side closed connection!");

    shutdown(my_socket, 0); // good practice to shutdown the socket.

    closesocket(my_socket); // remove the socket.
    char lol[3];
    return lol;
}
