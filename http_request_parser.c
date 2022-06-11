/**
 * Piotr Dobiech 316625
 */

#include "http_request_parser.h"

#include "utils.h"
#include "webserver.h"
#include <malloc.h>
#include <string.h>

void initialize_http_request_parser_state(uint8_t* buffer, ParserState* state) {
    state->input_buffer = buffer;
    state->input_buffer_length = 0;
    initialize_string(&state->current_line);
    state->current_line.characters = malloc(PARSER_LINE_BUFFER_SIZE);
    state->line_buffer_size = PARSER_LINE_BUFFER_SIZE;
    state->cr_received = false;
    state->first_line = true;
}

void update_http_request_parser_state(const size_t buffer_received_length, ParserState* state) {
    state->input_buffer_length = buffer_received_length;
}

void reset_http_request_parser_state(ParserState* state) {
    state->current_line.length = 0;
    state->cr_received = false;
    state->first_line = true;
}

static inline bool parse_get_method(String* line, HttpRequest* request) {
    String path;

    if (!string_split(line, ' ', &path, NULL)) {
        return false;
    }

    string_allocate_copy(&path, &request->path);
    return true;
}

static inline bool parse_start_line(ParserState* state, HttpRequest* request) {
    const String* line = &state->current_line;
    String remaining;

    if (string_strip_prefix(line, &HTTP_METHOD_NAME_GET_STRING, &remaining)) {
        request->method = GET_METHOD;
        return parse_get_method(&remaining, request);
    }

    // Unknown HTTP method reached.
    return false;
}

static inline bool parse_connection_type(const String* value, HttpRequest* request) {
    if (string_equals(value, &CONNECTION_TYPE_CLOSE_STRING)) {
        request->connection_type = CLOSE_CONNECTION_TYPE;
    } else if (string_equals(value, &CONNECTION_TYPE_KEEP_ALIVE_STRING)) {
        request->connection_type = KEEP_ALIVE_CONNECTION_TYPE;
    }

    // Simply leave connection type unknown.
    return true;
}

static inline bool parse_header_field(ParserState* state, HttpRequest* request) {
    const String* line = &state->current_line;
    String value;

    if (string_strip_prefix(line, &HOST_FIELD_PREFIX_STRING, &value)) {
        string_trim_whitespaces(&value, &value);
        string_allocate_copy(&value, &request->host);
    } else if (string_strip_prefix(line, &CONNECTION_FIELD_PREFIX_STRING, &value)) {
        string_trim_whitespaces(&value, &value);
        parse_connection_type(&value, request);
    }

    // Simply skip unknown fields.
    return true;
}

static inline bool parse_line(ParserState* state, HttpRequest* request) {
    if (state->first_line) {
        state->first_line = false;
        return parse_start_line(state, request);
    } else {
        return parse_header_field(state, request);
    }
}

/**
 * Returns true if HTTP header terminator (empty line) was reached, otherwise false.
 */
ParseResult parse_http_request(ParserState* state, HttpRequest* request) {
    for (size_t index = 0; index < state->input_buffer_length; index++) {
        uint8_t current_byte = state->input_buffer[index];

        // If in last iteration we get '\r' character, then it can be the end of line.
        if (state->cr_received) {
            state->cr_received = false;

            // If this is in fact new line, then handle and clear it, otherwise parse as always.
            if (current_byte == '\n') {
                // As this is the end of line, we are not interested in the last '\r' character.
                // We can replace it with null terminator.
                state->current_line.length--;
                state->current_line.characters[state->current_line.length] = '\0';

                // If in this line we received only '\r\n', then its a header terminator.
                if (state->current_line.length == 0) {
                    return TERMINATOR_REACHED_PARSE_RESULT;
                }

                const bool parsed_correctly = parse_line(state, request);
                if (!parsed_correctly) {
                    return INVALID_REQUEST_PARSE_RESULT;
                }
                state->current_line.length = 0;
                continue;
            }
        }

        // Remember until the next iteration that we get '\r' character.
        if (current_byte == '\r') {
            state->cr_received = true;
        }

        // Increase line buffer by a factor of two if it is too small.
        // This change will persist until header terminator is reached.
        assert(state->current_line.length <= state->input_buffer_length);
        if (state->current_line.length == state->input_buffer_length) {
            state->current_line.characters
                = realloc(state->current_line.characters, state->input_buffer_length * 2);
            state->input_buffer_length *= 2;
        }

        // Save current byte to line buffer.
        state->current_line.characters[state->current_line.length] = current_byte;
        state->current_line.length++;
    }

    return MORE_BYTES_PARSE_RESULT;
}

void uninitialize_http_request_parser_state(ParserState* state) {
    uninitialize_string(&state->current_line);
}
