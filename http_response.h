/**
 * Piotr Dobiech 316625
 */

#pragma once

#include <inttypes.h>
#include <stdlib.h>

typedef enum
{
    OK_RESPONSE_CODE, // 200
    MOVED_PERMANENTLY_RESPONSE_CODE, // 301
    FORBIDDEN_RESPONSE_CODE, // 403
    NOT_FOUND_RESPONSE_CODE, // 404
    NOT_IMPLEMENTED_RESPONSE_CODE, // 501
} ResponseCode;

typedef enum
{
    PLAIN_TEXT, // .txt text/plain
    HTML_TEXT, // .html text/html
    CSS_TEXT, // .css text/css
    JPEG_IMAGE, // .jpg .jpeg image/jpeg
    PNG_IMAGE, // .png image/png
    PDF_APPLICATION // .pdf application/pdf
} ContentType;

typedef struct {
    uint8_t* buffer;
    size_t buffer_content_length;
    size_t buffer_allocated_size;
} HttpResponse;

void initialize_response(HttpResponse* response);

void add_status_line_to_response(const ResponseCode response_code, HttpResponse* response);

void add_content_type_to_response(const ContentType content_type, HttpResponse* response);

void add_content_length_to_response(const size_t content_length, HttpResponse* response);

void add_location_to_response(const char* location, HttpResponse* response);

void add_header_terminator_to_response(HttpResponse* response);

void add_content_to_response(const void* buffer, const size_t buffer_size, HttpResponse* response);

void print_response(HttpResponse* response);

void uninitialize_response(HttpResponse* response);
