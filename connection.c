/**
 * Piotr Dobiech 316625
 */

#include "connection.h"

static inline void initialize_connection(
    uint8_t* buffer, const int socket_fd, Connection* connection) {

    initialize_http_request_parser_state(buffer, &connection->parser_state);
    initialize_http_request(&connection->http_request);

    connection->connection_fd = accept_connection_on_socket(socket_fd);
}

static inline void uninitialize_connection(Connection* connection) {
    close_fd(connection->connection_fd);
}
