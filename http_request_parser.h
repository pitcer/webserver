/**
 * Piotr Dobiech 316625
 */

#pragma once

#include "http_request.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    const uint8_t* input_buffer;
    size_t input_buffer_length;
    char* current_line;
    size_t line_buffer_size;
    size_t line_length;
    bool cr_received;
    bool first_line;
} ParserState;

void initialize_http_request_parser_state(uint8_t* buffer, ParserState* state);

void update_http_request_parser_state(const size_t buffer_received_length, ParserState* state);

void reset_http_request_parser_state(ParserState* state);

bool parse_http_request(ParserState* state, HttpRequest* request);

void uninitialize_http_request_parser_state(ParserState* state);
