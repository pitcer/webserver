/**
 * Piotr Dobiech 316625
 */

#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* characters;
    size_t length;
} String;

#define string_initializer_from_literal(literal)                                                   \
    { .characters = literal, .length = sizeof(literal) - 1 }

void initialize_string(String* string);

bool string_equals(const String* first, const String* second);

/**
 * Returns true if string has the given prefix and places striped one in result.
 */
bool string_strip_prefix(const String* string, const String* prefix, String* result);

/**
 * Returns true if split character was found, otherwise false.
 * Left and right strings are slices of the given string.
 */
bool string_split(const String* string, const char split, String* left, String* right);

/**
 * Returns string with pointer to place in buffer of the given string.
 */
void string_trim_whitespaces(const String* string, String* result);

void string_allocate_copy(const String* source, String* destination);

void string_print(const String* string);

void uninitialize_string(String* string);
