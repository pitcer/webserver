/**
 * Piotr Dobiech 316625
 */

#pragma once

#include "str.h"

#define REUSE_ADDRESS_AND_PORT

#define CONNECTIONS_QUEUE_SIZE 16

#define CONNECTION_RECEIVE_BUFFER_SIZE 4096

#define CONNECTION_TIMEOUT_SECONDS 1
#define CONNECTION_TIMEOUT_MICROSECONDS 0

#define PARSER_LINE_BUFFER_SIZE 128

#define RESPONSE_START_BUFFER_SIZE 128

#define FILE_READ_BUFFER_SIZE 4096

#define HTTP_METHOD_NAME_GET "GET"
static const String HTTP_METHOD_NAME_GET_STRING
    = string_initializer_from_literal(HTTP_METHOD_NAME_GET " ");

#define HTTP_HEADER_FIELD_NAME_HOST "Host"
#define HTTP_HEADER_FIELD_NAME_CONNECTION "Connection"

static const String HOST_FIELD_PREFIX_STRING
    = string_initializer_from_literal(HTTP_HEADER_FIELD_NAME_HOST ":");

static const String CONNECTION_FIELD_PREFIX_STRING
    = string_initializer_from_literal(HTTP_HEADER_FIELD_NAME_CONNECTION ":");

#define HTTP_HEADER_FIELD_NAME_CONTENT_TYPE "Content-Type"
#define HTTP_HEADER_FIELD_NAME_CONTENT_LENGTH "Content-Length"
#define HTTP_HEADER_FIELD_NAME_LOCATION "Location"

#define CONNECTION_TYPE_CLOSE "close"
static const String CONNECTION_TYPE_CLOSE_STRING
    = string_initializer_from_literal(CONNECTION_TYPE_CLOSE);

#define CONNECTION_TYPE_KEEP_ALIVE "keep-alive"
static const String CONNECTION_TYPE_KEEP_ALIVE_STRING
    = string_initializer_from_literal(CONNECTION_TYPE_KEEP_ALIVE);

#define HTTP_PROTOCOL_URL_PREFIX "http://"
static const String HTTP_PROTOCOL_URL_PREFIX_STRING
    = string_initializer_from_literal(HTTP_PROTOCOL_URL_PREFIX);

static const String SLASH_STRING = string_initializer_from_literal("/");

#define INDEX_HTML_FILE_PATH "/index.html"
static const String INDEX_HTML_FILE_PATH_STRING
    = string_initializer_from_literal(INDEX_HTML_FILE_PATH);

#define SERVER_HTTP_VERSION "HTTP/1.1"

#define HTTP_RESPONSE_CONTENT_MOVED_PERMANENTLY                                                    \
    "<html>"                                                                                       \
    "   <head>"                                                                                    \
    "       <title>301 Moved Permanently</title>"                                                  \
    "   </head>"                                                                                   \
    "   <body>"                                                                                    \
    "       <h1>301 Moved Permanently</h1>"                                                        \
    "   </body>"                                                                                   \
    "</html>"

#define HTTP_RESPONSE_CONTENT_FORBIDDEN                                                            \
    "<html>"                                                                                       \
    "   <head>"                                                                                    \
    "       <title>403 Forbidden</title>"                                                          \
    "   </head>"                                                                                   \
    "   <body>"                                                                                    \
    "       <h1>403 Forbidden</h1>"                                                                \
    "   </body>"                                                                                   \
    "</html>"

#define HTTP_RESPONSE_CONTENT_NOT_FOUND                                                            \
    "<html>"                                                                                       \
    "   <head>"                                                                                    \
    "       <title>404 Not Found</title>"                                                          \
    "   </head>"                                                                                   \
    "   <body>"                                                                                    \
    "       <h1>404 Not Found</h1>"                                                                \
    "   </body>"                                                                                   \
    "</html>"

#define HTTP_RESPONSE_CONTENT_NOT_IMPLEMENTED                                                      \
    "<html>"                                                                                       \
    "   <head>"                                                                                    \
    "       <title>501 Not Implemented</title>"                                                    \
    "   </head>"                                                                                   \
    "   <body>"                                                                                    \
    "       <h1>501 Not Implemented</h1>"                                                          \
    "   </body>"                                                                                   \
    "</html>"
