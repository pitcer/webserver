/**
 * Piotr Dobiech 316625
 */

#pragma once

#include "http_request.h"
#include "http_request_parser.h"
#include "http_response.h"
#include "webserver.h"

typedef struct {
    int connection_fd;
    uint8_t receive_buffer[CONNECTION_RECEIVE_BUFFER_SIZE];
    ParserState parser_state;
    HttpRequest http_request;
} Connection;

void initialize_connection(const int socket_fd, Connection* connection);

void connection_send_response(HttpResponse* response, Connection* connection);

void connection_send_moved_permanently_to_index_response(Connection* connection);

void connection_send_forbidden_response(Connection* connection);

void connection_send_not_found_response(Connection* connection);

void connection_send_not_implemented_response(Connection* connection);

void uninitialize_connection(Connection* connection);
