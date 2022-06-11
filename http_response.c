/**
 * Piotr Dobiech 316625
 */

#include "http_response.h"

#include "utils.h"
#include "webserver.h"
#include <stdarg.h>
#include <string.h>

void initialize_response(HttpResponse* response) {
    response->buffer = malloc(RESPONSE_START_BUFFER_SIZE);
    response->buffer_allocated_size = RESPONSE_START_BUFFER_SIZE;
    response->buffer_content_length = 0;
}

static inline void allocate_memory_for_new_content(
    const size_t content_length, HttpResponse* response) {

    const size_t available_space
        = response->buffer_allocated_size - response->buffer_content_length;
    if (content_length > available_space) {
        const size_t new_content_length = response->buffer_content_length + content_length;

        size_t new_allocated_size = response->buffer_allocated_size;
        while (new_content_length > new_allocated_size) {
            new_allocated_size *= 2;
        }

        response->buffer = realloc(response->buffer, new_allocated_size);
        response->buffer_allocated_size = new_allocated_size;
    }
}

static inline void add_to_response(HttpResponse* response, char* format, ...) {
    va_list arguments;
    va_list arguments_copy;
    va_start(arguments, format);
    va_copy(arguments_copy, arguments);

    int needed_space = vsnprintf(NULL, 0, format, arguments);

    if (needed_space < 0) {
        eprintln("snprintf error");
        exit(EXIT_FAILURE);
    }

    // We need two bytes for '\r' and '\n'.
    needed_space += 2;

    allocate_memory_for_new_content(needed_space, response);

    uint8_t* buffer = response->buffer + response->buffer_content_length;
    vsnprintf((char*)buffer, needed_space, format, arguments_copy);
    // Since vsnprintf force-writes null-terminator, we overwrite it with '\r'.
    buffer[needed_space - 2] = '\r';
    buffer[needed_space - 1] = '\n';

    response->buffer_content_length += needed_space;

    va_end(arguments);
    va_end(arguments_copy);
}

static inline uint32_t get_status_code_from_response_code(const ResponseCode response_code) {
    switch (response_code) {
    case OK_RESPONSE_CODE:
        return 200;
    case MOVED_PERMANENTLY_RESPONSE_CODE:
        return 301;
    case FORBIDDEN_RESPONSE_CODE:
        return 403;
    case NOT_FOUND_RESPONSE_CODE:
        return 404;
    case NOT_IMPLEMENTED_RESPONSE_CODE:
        return 501;
    }
    assert(false);
}

static inline char* get_reason_phrase_from_response_code(const ResponseCode response_code) {
    switch (response_code) {
    case OK_RESPONSE_CODE:
        return "OK";
    case MOVED_PERMANENTLY_RESPONSE_CODE:
        return "Moved Permanently";
    case FORBIDDEN_RESPONSE_CODE:
        return "Forbidden";
    case NOT_FOUND_RESPONSE_CODE:
        return "Not Found";
    case NOT_IMPLEMENTED_RESPONSE_CODE:
        return "Not Implemented";
    }
    assert(false);
}

void add_status_line_to_response(const ResponseCode response_code, HttpResponse* response) {
    const uint32_t status_code = get_status_code_from_response_code(response_code);
    const char* reason_phrase = get_reason_phrase_from_response_code(response_code);
    add_to_response(response, SERVER_HTTP_VERSION " %u %s", status_code, reason_phrase);
    debug(SERVER_HTTP_VERSION " %u %s", status_code, reason_phrase);
}

static inline char* get_mime_type_from_content_type(const ContentType content_type) {
    switch (content_type) {
    case PLAIN_TEXT:
        return "text/plain";
    case HTML_TEXT:
        return "text/html";
    case CSS_TEXT:
        return "text/css";
    case JPEG_IMAGE:
        return "image/jpeg";
    case PNG_IMAGE:
        return "image/png";
    case PDF_APPLICATION:
        return "application/pdf";
    case OCTET_STREAM_APPLICATION:
        return "application/octet-stream";
    }
    assert(false);
}

void add_content_type_to_response(const ContentType content_type, HttpResponse* response) {
    const char* mime_type = get_mime_type_from_content_type(content_type);
    add_to_response(response, HTTP_HEADER_FIELD_NAME_CONTENT_TYPE ": %s; charset=UTF-8", mime_type);
    debug(HTTP_HEADER_FIELD_NAME_CONTENT_TYPE ": %s; charset=UTF-8", mime_type);
}

void add_content_length_to_response(const size_t content_length, HttpResponse* response) {
    add_to_response(response, HTTP_HEADER_FIELD_NAME_CONTENT_LENGTH ": %lu", content_length);
    debug(HTTP_HEADER_FIELD_NAME_CONTENT_LENGTH ": %lu", content_length);
}

void add_location_to_response(const char* location, HttpResponse* response) {
    add_to_response(response, HTTP_HEADER_FIELD_NAME_LOCATION ": %s", location);
    debug(HTTP_HEADER_FIELD_NAME_LOCATION ": %s", location);
}

void add_header_terminator_to_response(HttpResponse* response) {
    add_to_response(response, "");
    debug();
}

void add_content_to_response(
    const void* content, const size_t content_size, HttpResponse* response) {

    allocate_memory_for_new_content(content_size, response);

    uint8_t* buffer = response->buffer + response->buffer_content_length;
    memcpy(buffer, content, content_size);

    response->buffer_content_length += content_size;
}

void debug_print_http_response(HttpResponse* response) {
    for (size_t index = 0; index < response->buffer_content_length; index++) {
        char character = response->buffer[index];
        putchar(character);
    }
    putchar('\n');
}

static const String TXT_EXTENSION_STRING = string_initializer_from_literal("txt");
static const String HTML_EXTENSION_STRING = string_initializer_from_literal("html");
static const String CSS_EXTENSION_STRING = string_initializer_from_literal("css");
static const String JPG_EXTENSION_STRING = string_initializer_from_literal("jpg");
static const String JPEG_EXTENSION_STRING = string_initializer_from_literal("jpeg");
static const String PNG_EXTENSION_STRING = string_initializer_from_literal("png");
static const String PDF_EXTENSION_STRING = string_initializer_from_literal("pdf");

ContentType get_content_type_from_path(const String* path) {
    String file_name;
    string_split_backward(path, '/', NULL, &file_name);
    String file_extension;
    bool extension_found = string_split_backward(&file_name, '.', NULL, &file_extension);

    if (!extension_found) {
        return OCTET_STREAM_APPLICATION;
    }

    if (string_equals(&file_extension, &TXT_EXTENSION_STRING)) {
        return PLAIN_TEXT;
    } else if (string_equals(&file_extension, &HTML_EXTENSION_STRING)) {
        return HTML_TEXT;
    } else if (string_equals(&file_extension, &CSS_EXTENSION_STRING)) {
        return CSS_TEXT;
    } else if (string_equals(&file_extension, &JPG_EXTENSION_STRING)
        || string_equals(&file_extension, &JPEG_EXTENSION_STRING)) {
        return JPEG_IMAGE;
    } else if (string_equals(&file_extension, &PNG_EXTENSION_STRING)) {
        return PNG_IMAGE;
    } else if (string_equals(&file_extension, &PDF_EXTENSION_STRING)) {
        return PDF_APPLICATION;
    } else {
        return OCTET_STREAM_APPLICATION;
    }
}

void uninitialize_response(HttpResponse* response) {
    free(response->buffer);
    response->buffer = NULL;
    response->buffer_allocated_size = 0;
}
