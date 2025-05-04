//
// Created by alex on 16.04.25.
//
#include <stdio.h>

#include  "../include/config.h"
struct sandbox_config setup_config(void) {
    struct sandbox_config cfg = {
        .rootdir = "/home/alex/CLionProjects/SandBox/sandbox-root",
        .uid = 65534,
        .gid = 65534,
        .use_namespaces = 1,
        .enable_seccomp = 1,
        .enable_rlimits = 1
    };
    return cfg;
}