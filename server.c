/**
 * Piotr Dobiech 316625
 */

#include "server.h"

#include "connection.h"
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

static inline void send_response(Connection* connection, Server* server) { }

static inline void receive_request(Connection* connection, Server* server) {
    uint8_t* buffer = &connection->parser_state.input_buffer;
    const size_t received
        = receive(connection->connection_fd, buffer, sizeof(CONNECTION_RECEIVE_BUFFER_SIZE));
    update_http_request_parser_state(received, &connection->parser_state);
    bool received_terminator
        = parse_http_request(&connection->parser_state, &connection->http_request);

    if (received_terminator) {
        reset_http_request_parser_state(&connection->parser_state);

        send_response(connection, server);
        print_http_request(&connection->http_request); // TODO: remove

        reset_http_request(&connection->http_request);
    }
}

static inline void handle_connection(Connection* connection, Server* server) {
    struct timeval select_time
        = { .tv_sec = CONNECTION_TIMEOUT_SECONDS, .tv_usec = CONNECTION_TIMEOUT_MICROSECONDS };

    while (1) {
        fd_set select_descriptors;
        FD_ZERO(&select_descriptors);
        FD_SET(connection->connection_fd, &select_descriptors);

        const int ready
            = select(connection->connection_fd + 1, &select_descriptors, NULL, NULL, &select_time);

        if (ready < 0) {
            eprintln("select error: %s", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (ready == 0) {
            uninitialize_http_request_parser_state(&connection->parser_state);
            uninitialize_http_request(&connection->http_request);
            return;
        } else { // ready > 0
            receive_request(connection, server);
        }
    }
}

void start_server(Server* server) {
    listen_on_socket(server->socket_fd, CONNECTIONS_QUEUE_SIZE);

    while (1) {
        Connection connection;
        uint8_t buffer[CONNECTION_RECEIVE_BUFFER_SIZE];
        initialize_connection(buffer, server, &connection);

        handle_connection(&connection, server);

        uninitialize_connection(&connection);
    }
}

void uninitialize_server(Server* server) {
    free(server->pages_directory_path);
    server->pages_directory_path = NULL;
    close_fd(server->socket_fd);
    server->socket_fd = -1;
}
