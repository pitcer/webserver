/**
 * Piotr Dobiech 316625
 */

#include "io.h"
#include "server.h"
#include "utils.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static inline void validate_pages_directory(const char* pages_directory_path) {
    struct stat pages_directory_status;
    const bool director_exists = get_status(pages_directory_path, &pages_directory_status);

    if (!director_exists) {
        eprintln("Pages directory '%s' does not exist", pages_directory_path);
        exit(EXIT_FAILURE);
    }

    if (!S_ISDIR(pages_directory_status.st_mode)) {
        eprintln("Pages directory '%s' is not a directory", pages_directory_path);
        exit(EXIT_FAILURE);
    }
}

static inline void parse_input(int argc, char const* argv[], InputArguments* arguments) {
    if (argc != 3) {
        eprintln("Usage: webserver <server port> <directory>");
        exit(EXIT_FAILURE);
    }

    arguments->server_port = strtoumax(argv[1], NULL, 10);
    arguments->pages_directory_path = argv[2];

    validate_pages_directory(arguments->pages_directory_path);
}

int main(int argc, char const* argv[]) {
    InputArguments arguments;
    parse_input(argc, argv, &arguments);

    Server server;
    initialize_server(&arguments, &server);
    start_server(&server);
    uninitialize_server(&server);

    return EXIT_SUCCESS;
}
