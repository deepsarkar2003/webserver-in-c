#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BACKLOG 3
#define BUFFER_SIZE 1024
#define PORT_START 8080

// Function to load a file into memory
char* load_file(const char *filename, long *length) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("File open error");
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = malloc(*length);
    if (buffer) {
        fread(buffer, 1, *length, f);
    }
    fclose(f);
    return buffer;
}

// Function to find an available port
int find_available_port(int *server_fd, struct sockaddr_in *address) {
    int port = PORT_START;
    while (port < 65535) {
        *server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (*server_fd == 0) {
            perror("Socket failed");
            return -1;
        }
        address->sin_port = htons(port);
        if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) == 0) {
            return port;
        }
        close(*server_fd);
        port++;
    }
    return -1;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char *response;
    long response_length;

    // Address setup
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    // Find an available port
    int port = find_available_port(&server_fd, &address);
    if (port == -1) {
        fprintf(stderr, "No available ports found\n");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d\n", port);

    // Listen for incoming connections
    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Main loop to handle incoming connections
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        // Read the request
        read(new_socket, buffer, BUFFER_SIZE);
        printf("Request received:\n%s\n", buffer);

        // Load the HTML file into memory
        response = load_file("frontend/index.html", &response_length);
        if (!response) {
            close(new_socket);
            continue;
        }

        // Create a dynamic response header
        char header[BUFFER_SIZE];
        snprintf(header, sizeof(header),
                 "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %ld\n\n", response_length);

        // Send the header
        write(new_socket, header, strlen(header));

        // Send the HTML content
        write(new_socket, response, response_length);

        // Clean up
        free(response);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}
