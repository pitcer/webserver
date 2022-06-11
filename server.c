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
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static inline bool allocate_real_path(const char* path, String* real_path) {
    real_path->characters = realpath(path, NULL);
    if (real_path->characters == NULL) {
        return false;
    }
    real_path->length = strlen(real_path->characters);
    return true;
}

void initialize_server(const InputArguments* arguments, Server* server) {
    server->socket_fd = create_socket();
#ifdef REUSE_ADDRESS_AND_PORT
    enable_reuse_address(server->socket_fd);
    enable_reuse_port(server->socket_fd);
#endif

    struct sockaddr_in server_address;
    create_socket_address_from_binary(INADDR_ANY, arguments->server_port, &server_address);
    bind_socket(server->socket_fd, &server_address);

    allocate_real_path(arguments->pages_directory_path, &server->pages_directory_path);
}

static inline void handle_request(Connection* connection, Server* server) {
    if (connection->http_request.host.characters == NULL) {
        connection_send_not_implemented_response(connection);
        return;
    }

    String host_name;
    string_split_backward(&connection->http_request.host, ':', &host_name, NULL);

    String host_path;
    const String* host_path_sources[]
        = { &server->pages_directory_path, &SLASH_STRING, &host_name };
    string_allocate_concat(3, host_path_sources, &host_path);

    String real_host_path;
    const bool host_path_found = allocate_real_path(host_path.characters, &real_host_path);
    if (!host_path_found) {
        connection_send_not_found_response(connection);
        return;
    }

    String full_path;
    const String* full_path_sources[]
        = { &host_path, &SLASH_STRING, &connection->http_request.path };
    string_allocate_concat(3, full_path_sources, &full_path);

    String real_full_path;
    const bool full_path_found = allocate_real_path(full_path.characters, &real_full_path);
    if (!full_path_found) {
        connection_send_not_found_response(connection);
        return;
    }

    if (!string_starts_with(&real_full_path, &host_path)) {
        connection_send_forbidden_response(connection);
        return;
    }

    struct stat status;
    get_status(real_full_path.characters, &status);

    if (S_ISDIR(status.st_mode)) {
        connection_send_moved_permanently_to_index_response(connection);
    } else {
        const ContentType content_type = get_content_type_from_path(&real_full_path);

        HttpResponse response;
        initialize_response(&response);

        generate_ok_response(status.st_size, content_type, &response);

        int file_fd = open_file(real_full_path.characters, O_RDONLY, 0);
        void* buffer[FILE_READ_BUFFER_SIZE];
        size_t bytes_read = 0;
        while ((bytes_read = read_fd(file_fd, buffer, FILE_READ_BUFFER_SIZE)) != 0) {
            add_content_to_response(buffer, bytes_read, &response);
        }

        connection_send_response(&response, connection);
        uninitialize_response(&response);

        close_fd(file_fd);
    }

    uninitialize_string(&host_path);
    uninitialize_string(&real_host_path);
    uninitialize_string(&full_path);
    uninitialize_string(&real_full_path);
}

static inline bool receive_request(Connection* connection, Server* server) {
    const size_t received = receive_packet(
        connection->connection_fd, connection->receive_buffer, sizeof(connection->receive_buffer));
    if (received == 0) {
        return false;
    }

    update_http_request_parser_state(received, &connection->parser_state);
    const ParseResult result
        = parse_http_request(&connection->parser_state, &connection->http_request);

    switch (result) {
    case TERMINATOR_REACHED_PARSE_RESULT: {
        reset_http_request_parser_state(&connection->parser_state);

        debug_print_http_request(&connection->http_request);
        handle_request(connection, server);

        const ConnectionType connection_type = connection->http_request.connection_type;
        reset_http_request(&connection->http_request);
        if (connection_type == CLOSE_CONNECTION_TYPE) {
            return false;
        }

        break;
    }
    case INVALID_REQUEST_PARSE_RESULT: {
        reset_http_request_parser_state(&connection->parser_state);
        reset_http_request(&connection->http_request);

        connection_send_not_implemented_response(connection);
        break;
    }
    default:
        break;
    }

    return true;
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
            const bool received = receive_request(connection, server);
            if (!received) {
                return;
            }
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
    uninitialize_string(&server->pages_directory_path);
    close_fd(server->socket_fd);
    server->socket_fd = -1;
}
