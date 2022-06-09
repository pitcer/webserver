/**
 * Piotr Dobiech 316625
 */

#pragma once

typedef enum {
    GET_METHOD,
    UNKNOWN_METHOD,
} HttpMethod;

typedef enum {
    CLOSE_CONNECTION_TYPE,
    UNKNOWN_CONNECTION_TYPE,
} ConnectionType;

typedef struct {
    HttpMethod method;
    char* path;
    char* host;
    ConnectionType connection_type;
} HttpRequest;

void initialize_http_request(HttpRequest* request);

void reset_http_request(HttpRequest* request);

void print_http_request(const HttpRequest* request);

void uninitialize_http_request(HttpRequest* request);
