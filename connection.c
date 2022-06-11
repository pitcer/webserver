/**
 * Piotr Dobiech 316625
 */

#include "connection.h"

#include "http_response_generator.h"
#include "io.h"

void initialize_connection(const int socket_fd, Connection* connection) {
    initialize_http_request_parser_state(connection->receive_buffer, &connection->parser_state);
    initialize_http_request(&connection->http_request);

    connection->connection_fd = accept_connection_on_socket(socket_fd);
}

void connection_send_response(HttpResponse* response, Connection* connection) {
    size_t bytes_sent
        = send_packet(connection->connection_fd, response->buffer, response->buffer_content_length);

    while (bytes_sent < response->buffer_content_length) {
        uint8_t* buffer = response->buffer + bytes_sent;
        size_t buffer_length = response->buffer_content_length - bytes_sent;

        bytes_sent = send_packet(connection->connection_fd, buffer, buffer_length);
    }
}

void connection_send_moved_permanently_to_index_response(Connection* connection) {
    HttpResponse response;
    initialize_response(&response);

    String path;
    const String* path_sources[] = { &connection->http_request.path, &INDEX_HTML_FILE_PATH_STRING };
    string_allocate_concat(2, path_sources, &path);
    remove_consecutive_duplicates(&path, '/', &path);

    String location;
    const String* location_sources[]
        = { &HTTP_PROTOCOL_URL_PREFIX_STRING, &connection->http_request.host, &path };
    string_allocate_concat(3, location_sources, &location);

    generate_moved_permanently_response(location.characters, &response);
    connection_send_response(&response, connection);

    uninitialize_string(&path);
    uninitialize_string(&location);
    uninitialize_response(&response);
}

void connection_send_forbidden_response(Connection* connection) {
    HttpResponse response;
    initialize_response(&response);
    generate_forbidden_response(&response);
    connection_send_response(&response, connection);
    uninitialize_response(&response);
}

void connection_send_not_found_response(Connection* connection) {
    HttpResponse response;
    initialize_response(&response);
    generate_not_found_response(&response);
    connection_send_response(&response, connection);
    uninitialize_response(&response);
}

void connection_send_not_implemented_response(Connection* connection) {
    HttpResponse response;
    initialize_response(&response);
    generate_not_implemented_response(&response);
    connection_send_response(&response, connection);
    uninitialize_response(&response);
}

void uninitialize_connection(Connection* connection) {
    close_fd(connection->connection_fd);
    connection->connection_fd = -1;

    uninitialize_http_request_parser_state(&connection->parser_state);
    uninitialize_http_request(&connection->http_request);
}
