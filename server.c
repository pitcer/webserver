/**
 * Piotr Dobiech 316625
 */

#include "server.h"

#include "connection.h"
#include "http_response_generator.h"
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

static inline void send_response(Connection* connection, HttpResponse* response) {
    size_t bytes_sent
        = send_packet(connection->connection_fd, response->buffer, response->buffer_content_length);

    while (bytes_sent < response->buffer_content_length) {
        uint8_t* buffer = response->buffer + bytes_sent;
        size_t buffer_length = response->buffer_content_length - bytes_sent;

        bytes_sent = send_packet(connection->connection_fd, buffer, buffer_length);
    }
}

static inline void handle_request(Connection* connection, Server* server) {
    HttpResponse response;
    initialize_response(&response);
    generate_ok_response(&response);
    send_response(connection, &response);
    uninitialize_response(&response);
    (void)server;
}

static inline void receive_request(Connection* connection, Server* server) {
    const size_t received = receive_packet(
        connection->connection_fd, connection->receive_buffer, sizeof(connection->receive_buffer));
    update_http_request_parser_state(received, &connection->parser_state);
    const ParseResult result
        = parse_http_request(&connection->parser_state, &connection->http_request);

    switch (result) {
    case TERMINATOR_REACHED_PARSE_RESULT: {
        reset_http_request_parser_state(&connection->parser_state);

        handle_request(connection, server);

        reset_http_request(&connection->http_request);
        break;
    }
    case INVALID_REQUEST_PARSE_RESULT: {
        reset_http_request_parser_state(&connection->parser_state);
        reset_http_request(&connection->http_request);

        HttpResponse response;
        initialize_response(&response);
        generate_not_implemented_response(&response);
        send_response(connection, &response);
        uninitialize_response(&response);
        break;
    }
    default:
        break;
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
        initialize_connection(server->socket_fd, &connection);

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
