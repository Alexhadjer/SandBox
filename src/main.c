//
// Created by alex on 16.04.25.
//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "../include/sandbox.h"
#include "../include/util.h"
#include "../include/config.h"

int main(int argc, char *argv[]) {
    // check if executed with argument (file-path)

    if(argc<2){
        printf("Usage: %s <exec-program>\n",argv[0]);
        return 0;
    }

    const char *target_program = argv[1];

    // Optional: Build an argument list for execve().
    // Right now, we simply pass the remainder of argv to the target.
    // (i.e., if user typed: ./sandbox /bin/ls -l, we forward "/bin/ls" and "-l")
    char **target_args = &argv[1];
    struct sandbox_config config= setup_config();
    // Step 2: Fork a Child Process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork() failed");
        return EXIT_FAILURE;
    }
    else if (pid == 0) {
        // Child Process

        // For now: Just exec the requested program (no chroot yet).
        child_setup(&config);
        if (execve(target_program, target_args, NULL) == -1) {
            perror("execve() failed");
            exit(EXIT_FAILURE);
        }
    }
    else {
        // Parent Process: Wait for the child to finish
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid() failed");
            return EXIT_FAILURE;
        }

        if (WIFEXITED(status)) {
            printf("Child exited with code %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child killed by signal %d\n", WTERMSIG(status));
        }
    }

    return 0;
}