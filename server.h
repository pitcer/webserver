/**
 * Piotr Dobiech 316625
 */

#pragma once

#include <inttypes.h>

typedef struct {
    uint16_t server_port;
    const char* pages_directory_path;
} InputArguments;

typedef struct {
    int socket_fd;
    char* pages_directory_path;
} Server;

void initialize_server(const InputArguments* arguments, Server* server);

void start_server(Server* server);

void uninitialize_server(Server* server);
