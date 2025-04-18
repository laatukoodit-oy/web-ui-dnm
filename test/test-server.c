#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "index_html.h"

int port = 8000;

int main() {
    printf("Size of index_html: %d\n", sizeof(index_html));

    int server_socket, client_socket;

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
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
    while (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        server_addr.sin_port = htons(++port);
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on http://0.0.0.0:%d\n", port);

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

        printf("Connection from client.\n");

        char buffer[1024] = {0};
        int bytes_received = recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytes_received < 0) {
            perror("Receive failed");
        } else {
            buffer[bytes_received] = '\0'; // Null-terminate the string
            char *first_line = strtok(buffer, "\r\n");
            printf("Client requested:\n\t%s\n", first_line);
        }

        char endpoint = buffer[5] & 7;

        // Endpoint was a space -> send index_html
        if (endpoint == 0) {
            send(client_socket, index_html, sizeof(index_html), 0);
            printf("Sent index page\n");
        }
        else if (endpoint < 6) {
            const char ok[] =
                "HTTP/1.1 200 OK\r\n"
                "Connection: close\r\n"
                "\r\n";

            printf("Button %d pressed\n", endpoint);
            send(client_socket, ok, sizeof(ok), 0);
            printf("Sent 200 status\n");
        }
        // Endpoint was not 'a'-'e'
        else {
            const char not_found[] =
                "HTTP/1.1 404 Not Found\r\n"
                "Connection: close\r\n"
                "\r\n";

            send(client_socket, not_found, sizeof(not_found), 0);
            printf("Sent 404 status.\n");
        }

        close(client_socket);
        printf("Connection closed.\n\n");
    }

    return 0;
}

