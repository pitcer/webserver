/**
 * Piotr Dobiech 316625
 */

#include "http_response_generator.h"

#include "webserver.h"

void generate_ok_response(
    const size_t content_length, const ContentType content_type, HttpResponse* response) {

    add_status_line_to_response(OK_RESPONSE_CODE, response);
    add_content_type_to_response(content_type, response);
    add_content_length_to_response(content_length, response);
    add_header_terminator_to_response(response);
}

void generate_moved_permanently_response(const char* location, HttpResponse* response) {
    const size_t content_length = sizeof(HTTP_RESPONSE_CONTENT_MOVED_PERMANENTLY) - 1;

    add_status_line_to_response(MOVED_PERMANENTLY_RESPONSE_CODE, response);
    add_content_type_to_response(HTML_TEXT, response);
    add_content_length_to_response(content_length, response);
    add_location_to_response(location, response);
    add_header_terminator_to_response(response);
    add_content_to_response(HTTP_RESPONSE_CONTENT_MOVED_PERMANENTLY, content_length, response);
}

void generate_forbidden_response(HttpResponse* response) {
    const size_t content_length = sizeof(HTTP_RESPONSE_CONTENT_FORBIDDEN) - 1;

    add_status_line_to_response(FORBIDDEN_RESPONSE_CODE, response);
    add_content_type_to_response(HTML_TEXT, response);
    add_content_length_to_response(content_length, response);
    add_header_terminator_to_response(response);
    add_content_to_response(HTTP_RESPONSE_CONTENT_FORBIDDEN, content_length, response);
}

void generate_not_found_response(HttpResponse* response) {
    const size_t content_length = sizeof(HTTP_RESPONSE_CONTENT_NOT_FOUND) - 1;

    add_status_line_to_response(NOT_FOUND_RESPONSE_CODE, response);
    add_content_type_to_response(HTML_TEXT, response);
    add_content_length_to_response(content_length, response);
    add_header_terminator_to_response(response);
    add_content_to_response(HTTP_RESPONSE_CONTENT_NOT_FOUND, content_length, response);
}

void generate_not_implemented_response(HttpResponse* response) {
    const size_t content_length = sizeof(HTTP_RESPONSE_CONTENT_NOT_IMPLEMENTED) - 1;

    add_status_line_to_response(NOT_IMPLEMENTED_RESPONSE_CODE, response);
    add_content_type_to_response(HTML_TEXT, response);
    add_content_length_to_response(content_length, response);
    add_header_terminator_to_response(response);
    add_content_to_response(HTTP_RESPONSE_CONTENT_NOT_IMPLEMENTED, content_length, response);
}
