#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../build/index_html.h"

#define PORT 8000

int main() {
    printf("Size of index_html: %d\n", sizeof(index_html));

    int server_socket, client_socket;

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on http://0.0.0.0:%d\n", PORT);

    while(1) {
        client_socket = accept(
            server_socket,
            (struct sockaddr *)&client_addr,
            &client_addr_len
        );

        if (client_socket < 0) {
            perror("Accept failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        printf("Connection from client\n");

        // Send HTTP headers
        /*
        char headers[90];
        sprintf(
            headers,
            "HTTP/1.1 200 OK\r\n"
            "Content-Encoding: gzip\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %d\r\n\r\n",
            sizeof(index_html)
        );
        */

        send(client_socket, index_html, sizeof(index_html), 0);

        close(client_socket);
        printf("all done\n");
    }

    return 0;
}

