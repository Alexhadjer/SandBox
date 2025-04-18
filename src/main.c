//
// Created by alex on 16.04.25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "sandbox.h"
#include "util.h"

int main(int argc, char *argv[]) {
    // check if executed with argument (file-path)

    if(argc<2){
        printf("Usage: ");
        return 0;
    }

    const char *target_program = argv[1];

    // Optional: Build an argument list for execve().
    // Right now, we simply pass the remainder of argv to the target.
    // (i.e., if user typed: ./sandbox /bin/ls -l, we forward "/bin/ls" and "-l")
    char **target_args = &argv[1];

    // Step 2: Fork a Child Process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork() failed");
        return EXIT_FAILURE;
    }


    return 0;
}