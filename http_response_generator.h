/**
 * Piotr Dobiech 316625
 */

#pragma once

#include "http_response.h"

void generate_ok_response(
    const size_t content_length, const ContentType content_type, HttpResponse* response);

void generate_moved_permanently_response(const char* location, HttpResponse* response);

void generate_forbidden_response(HttpResponse* response);

void generate_not_found_response(HttpResponse* response);

void generate_not_implemented_response(HttpResponse* response);
