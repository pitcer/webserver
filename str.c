/**
 * Piotr Dobiech 316625
 */

#include "str.h"

#include "utils.h"
#include <assert.h>
#include <malloc.h>
#include <stdarg.h>
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

bool string_starts_with(const String* string, const String* prefix) {
    return strncmp(string->characters, prefix->characters, prefix->length) == 0;
}

bool string_strip_prefix(const String* string, const String* prefix, String* result) {
    if (!string_starts_with(string, prefix)) {
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

bool string_split_backward(const String* string, const char split, String* left, String* right) {
    char* string_characters = string->characters;
    const size_t string_length = string->length;

    for (size_t index = 0; index < string_length; index++) {
        const char character = string_characters[string_length - index - 1];
        if (character == split) {
            if (left != NULL) {
                left->characters = string_characters;
                left->length = string_length - index - 1;
            }
            if (right != NULL) {
                right->characters = string_characters + string_length - index;
                right->length = index;
            }
            return true;
        }
    }

    return false;
}

void remove_consecutive_duplicates(const String* string, const char character, String* result) {
    bool character_found = false;
    size_t new_length = 0;
    const size_t string_length = string->length;

    for (size_t index = 0; index < string_length; index++) {
        const char current_character = string->characters[index];
        if (current_character == character) {
            if (character_found) {
                continue;
            }
            character_found = true;
        } else {
            character_found = false;
        }
        result->characters[new_length] = current_character;
        new_length++;
    }

    result->characters[new_length] = '\0';
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

void string_allocate_concat(
    const size_t sources_size, const String** sources, String* destination) {

    initialize_string(destination);

    for (size_t index = 0; index < sources_size; index++) {
        destination->length += sources[index]->length;
    }

    destination->characters = malloc(destination->length + 1);

    size_t length = 0;
    for (size_t index = 0; index < sources_size; index++) {
        const String* source = sources[index];
        const size_t source_length = source->length;
        memcpy(destination->characters + length, source->characters, source_length);
        length += source_length;
    }
    assert(length == destination->length);

    destination->characters[destination->length] = '\0';
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
