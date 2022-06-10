/**
 * Piotr Dobiech 316625
 */

#include "str.h"

#include <malloc.h>
#include <string.h>

void initialize_string(String* string) {
    string->characters = NULL;
    string->length = 0;
}

bool string_equals(const String* first, const String* second) {
    if (first->length != second->length) {
        return false;
    }
    return strncmp(first->characters, second->characters, first->length) == 0;
}

bool string_strip_prefix(const String* string, const String* prefix, String* result) {
    if (strncmp(string->characters, prefix->characters, prefix->length) != 0) {
        return false;
    }

    result->characters = string->characters + prefix->length;
    result->length = string->length - prefix->length;
    return true;
}

bool string_split(const String* string, const char split, String* left, String* right) {
    char* string_characters = string->characters;
    const size_t string_length = string->length;

    for (size_t index = 0; index < string_length; index++) {
        const char character = string_characters[index];
        if (character == split) {
            if (left != NULL) {
                left->characters = string_characters;
                left->length = index;
            }
            if (right != NULL) {
                right->characters = string_characters + index;
                right->length = string_length - index - 1;
            }
            return true;
        }
    }

    return false;
}

static inline bool is_whitespace(const char character) {
    return character == ' ';
}

void string_trim_whitespaces(const String* string, String* result) {
    const char* string_characters = string->characters;
    const size_t string_length = string->length;

    result->characters = string->characters;
    result->length = string->length;

    for (size_t index = 0; index < string_length; index++) {
        const char character = string_characters[index];
        if (!is_whitespace(character)) {
            break;
        }
        result->characters++;
        result->length--;
    }

    const size_t result_length = result->length;
    for (size_t index = 0; index < result_length; index++) {
        const char character = string_characters[result_length - index - 1];
        if (!is_whitespace(character)) {
            break;
        }
        result->length--;
    }
}

void string_allocate_copy(const String* source, String* destination) {
    destination->characters = malloc(source->length + 1);
    memcpy(destination->characters, source->characters, source->length);
    destination->characters[source->length] = '\0';
    destination->length = source->length;
}

void string_print(const String* string) {
    for (size_t index = 0; index < string->length; index++) {
        const char character = string->characters[index];
        putchar(character);
    }
    putchar('\n');
}

void uninitialize_string(String* string) {
    free(string->characters);
    string->characters = NULL;
    string->length = 0;
}
