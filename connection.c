/**
 * Piotr Dobiech 316625
 */

#include "connection.h"

#include "io.h"

void initialize_connection(const int socket_fd, Connection* connection) {
    initialize_http_request_parser_state(connection->receive_buffer, &connection->parser_state);
    initialize_http_request(&connection->http_request);

    connection->connection_fd = accept_connection_on_socket(socket_fd);
}

void uninitialize_connection(Connection* connection) {
    close_fd(connection->connection_fd);
    connection->connection_fd = -1;

    uninitialize_http_request_parser_state(&connection->parser_state);
    uninitialize_http_request(&connection->http_request);
}
