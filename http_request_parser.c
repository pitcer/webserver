/**
 * Piotr Dobiech 316625
 */

#include "http_request_parser.h"

#include "utils.h"
#include "webserver.h"
#include <malloc.h>

void initialize_http_request_parser_state(uint8_t* buffer, ParserState* state) {
    state->input_buffer = buffer;
    state->input_buffer_length = 0;
    state->current_line = malloc(PARSER_LINE_BUFFER_SIZE);
    state->line_buffer_size = PARSER_LINE_BUFFER_SIZE;
    state->line_length = 0;
    state->cr_received = false;
    state->first_line = true;
}

void update_http_request_parser_state(const size_t buffer_received_length, ParserState* state) {
    state->input_buffer_length = buffer_received_length;
}

void reset_http_request_parser_state(ParserState* state) {
    state->line_length = 0;
    state->cr_received = false;
    state->first_line = true;
}

static inline void parse_http_request_line(ParserState* state, HttpRequest* request) {
    print_chars(state->current_line, state->line_length);
}

/**
 * Returns true if HTTP header terminator (empty line) was reached, otherwise false.
 */
bool parse_http_request(ParserState* state, HttpRequest* request) {
    for (size_t index = 0; index < state->input_buffer_length; index++) {
        uint8_t current_byte = state->input_buffer[index];

        // If in last iteration we get '\r' character, then it can be the end of line.
        if (state->cr_received) {
            state->cr_received = false;

            // If this is in fact new line, then handle and clear it, otherwise parse as always.
            if (current_byte == '\n') {
                // As this is the end of line, we are not interested in the last '\r' character.
                state->line_length--;

                // If in this line we received only '\r\n', then its a header terminator.
                if (state->line_length == 0) {
                    println("--- End of HTTP request header ---");
                    return true;
                }

                parse_http_request_line(state, request);
                state->line_length = 0;
                continue;
            }
        }

        // Remember until the next iteration that we get '\r' character.
        if (current_byte == '\r') {
            state->cr_received = true;
        }

        // Increase line buffer by a factor of two if it is too small.
        // This change will persist until header terminator is reached.
        assert(state->line_length <= state->input_buffer_length);
        if (state->line_length == state->input_buffer_length) {
            state->current_line = realloc(state->current_line, state->input_buffer_length * 2);
            state->input_buffer_length *= 2;
        }

        // Save current byte to line buffer.
        state->current_line[state->line_length] = current_byte;
        state->line_length++;
    }

    return false;
}

void uninitialize_http_request_parser_state(ParserState* state) {
    free(state->current_line);
    state->current_line = NULL;
}

// Example request:

// GET / HTTP/1.1
// Host: localhost:12345
// User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:101.0) Gecko/20100101 Firefox/101.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate, br
// DNT: 1
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1
// Sec-Fetch-Dest: document
// Sec-Fetch-Mode: navigate
// Sec-Fetch-Site: cross-site
// Sec-GPC: 1
//
