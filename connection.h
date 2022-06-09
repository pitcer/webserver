/**
 * Piotr Dobiech 316625
 */

#pragma once

#include "http_request.h"
#include "http_request_parser.h"

typedef struct {
    int connection_fd;
    ParserState parser_state;
    HttpRequest http_request;
} Connection;

static inline void initialize_connection(
    uint8_t* buffer, const int socket_fd, Connection* connection);

void uninitialize_connection(Connection* connection);
