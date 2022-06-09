/**
 * Piotr Dobiech 316625
 */

#pragma once

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

#define println(format, ...) printf(format "\n" __VA_OPT__(, ) __VA_ARGS__)

#define eprintln(format, ...) fprintf(stderr, format "\n" __VA_OPT__(, ) __VA_ARGS__)

#define DEBUG
#ifdef DEBUG
#define debug(...) println(__VA_ARGS__)
#else
#define debug(...)
#endif

#ifdef DEBUG
#define debug_assert(expression) assert(expression)
#else
#define debug_assert(expression)
#endif

void print_as_bytes(const unsigned char* buff, ssize_t length);

void print_chars(const uint8_t* buffer, size_t length);
