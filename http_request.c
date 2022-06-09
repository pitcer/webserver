/**
 * Piotr Dobiech 316625
 */

#include "http_request.h"

#include "utils.h"
#include <malloc.h>

void initialize_http_request(HttpRequest* request) {
    request->method = UNKNOWN_METHOD;
    request->path = NULL;
    request->host = NULL;
    request->connection_type = UNKNOWN_CONNECTION_TYPE;
}

void reset_http_request(HttpRequest* request) {
    uninitialize_http_request(request);
    initialize_http_request(request);
}

void print_http_request(const HttpRequest* request) {
    char* method = request->method == GET_METHOD ? "GET" : "UNKNOWN";
    char* connection_type = request->connection_type == CLOSE_CONNECTION_TYPE ? "close" : "unknown";
    println("%s '%s' HTTP/1.1", method, request->path);
    println("Host: '%s'", request->host);
    println("Connection: '%s'", connection_type);
}

void uninitialize_http_request(HttpRequest* request) {
    free(request->path);
    request->path = NULL;
    free(request->host);
    request->host = NULL;
}
