/**
 * Piotr Dobiech 316625
 */

#include "server.h"

#include "io.h"
#include "utils.h"
#include "webserver.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void initialize_server(const InputArguments* arguments, Server* server) {
    server->socket_fd = create_socket();
    enable_reuse_address(server->socket_fd); // TODO: remove
    enable_reuse_port(server->socket_fd); // TODO: remove

    struct sockaddr_in server_address;
    create_socket_address_from_binary(INADDR_ANY, arguments->server_port, &server_address);
    bind_socket(server->socket_fd, &server_address);

    server->pages_directory_path = to_real_path(arguments->pages_directory_path);
}

static inline void handle_connection(const int connection_fd, Server* server) {
    (void)server;

    struct timeval select_time
        = { .tv_sec = CONNECTION_TIMEOUT_SECONDS, .tv_usec = CONNECTION_TIMEOUT_MICROSECONDS };

    while (1) {
        fd_set select_descriptors;
        FD_ZERO(&select_descriptors);
        FD_SET(connection_fd, &select_descriptors);

        const int ready = select(connection_fd + 1, &select_descriptors, NULL, NULL, &select_time);

        if (ready < 0) {
            eprintln("select error: %s", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (ready == 0) {
            return;
        } else { // ready > 0
            uint8_t buffer[CONNECTION_RECEIVE_BUFFER_SIZE];
            const size_t received = receive(connection_fd, buffer, sizeof(buffer));
            print_chars(buffer, received);
        }
    }
}

void start_server(Server* server) {
    listen_on_socket(server->socket_fd, CONNECTIONS_QUEUE_SIZE);

    while (1) {
        const int connection_fd = accept_connection_on_socket(server->socket_fd);
        handle_connection(connection_fd, server);
        close_fd(connection_fd);
    }
}

void uninitialize_server(Server* server) {
    free(server->pages_directory_path);
    close_fd(server->socket_fd);
}

// Example request:

// GET / HTTP/1.1
// Host: localhost:12345
// User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:101.0) Gecko/20100101 Firefox/101.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate, br
// DNT: 1
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1
// Sec-Fetch-Dest: document
// Sec-Fetch-Mode: navigate
// Sec-Fetch-Site: cross-site
// Sec-GPC: 1
//
