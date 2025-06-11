//
// Created by alex on 16.04.25.
//
#include <stdio.h>
#include <stdlib.h>
#include  "../include/config.h"
struct sandbox_config setup_config(void) {
    struct sandbox_config cfg = {
        .rootdir = "sandbox-root", //might be absolute path needed. Worked now relativ. /home/alex/CLionProjects/SandBox/
        .uid = 65534,
        .gid = 65534,
        .use_namespaces = 1,
        .enable_seccomp = 1,
        .enable_rlimits = 1
    };
    return cfg;

}

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}