/**
 * Piotr Dobiech 316625
 */

#include "http_request.h"

#include "utils.h"
#include "webserver.h"
#include <malloc.h>

void initialize_http_request(HttpRequest* request) {
    request->method = UNKNOWN_METHOD;
    initialize_string(&request->path);
    initialize_string(&request->host);
    request->connection_type = UNKNOWN_CONNECTION_TYPE;
}

void reset_http_request(HttpRequest* request) {
    uninitialize_http_request(request);
    initialize_http_request(request);
}

void print_http_request(const HttpRequest* request) {
    char* method = request->method == GET_METHOD ? HTTP_METHOD_NAME_GET : "UNKNOWN";
    char* connection_type = request->connection_type == CLOSE_CONNECTION_TYPE
        ? CONNECTION_TYPE_CLOSE
        : request->connection_type == KEEP_ALIVE_CONNECTION_TYPE ? CONNECTION_TYPE_KEEP_ALIVE
                                                                 : "unknown";
    println("%s '%s' HTTP/1.1", method, request->path.characters);
    println("Host: '%s'", request->host.characters);
    println("Connection: '%s'", connection_type);
}

void uninitialize_http_request(HttpRequest* request) {
    uninitialize_string(&request->path);
    uninitialize_string(&request->host);
}
