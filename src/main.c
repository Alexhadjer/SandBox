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
#define GNU_SOURCE
#include  <sys/stat.h>
#include <sys/mount.h>
extern char **environ;

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
    sandbox_config config= setup_config();
    // Step 2: Fork a Child Process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork() failed");
        return EXIT_FAILURE;
    }
    else if (pid == 0) {
        // Child Process

        // For now: Just exec the requested program (no chroot yet).
        //child_setup(&config);
        char *target_argv[] = { "/bin/bash", NULL };
        printf("hi");
        printf("hello world\n");
        printf("%s\n", config.rootdir);
        fflush(stdout);       printf("lol");

        enter_chroot(&config);
        if (config.enable_rlimits) {
            security_apply_rlimits(&config);
        }
        security_drop_caps(&config); 
        security_apply_seccomp(&config);
        printf("After chroot, checking if shell exists...\n");
        struct stat shell_stat;
        if (stat("/bin/bash", &shell_stat) == -1) {
            perror("bash not found in chroot");
        } else {
            printf("/bin/bash exists in chroot\n");
        }

        if (stat("/bin/sh", &shell_stat) == -1) {
            perror("sh not found in chroot");
        } else {
            printf("/bin/sh exists in chroot\n");
        }
        struct stat exec_stat;
        if (stat(target_program, &exec_stat) == -1) {
            perror("stat() on target program failed");
            printf("Target program %s does not exist or is not accessible\n", target_program);
            exit(EXIT_FAILURE);
        }

        printf("File exists. Checking if executable...\n");
        if (!(exec_stat.st_mode & S_IXUSR)) {
            printf("Target program %s is not executable\n", target_program);
            exit(EXIT_FAILURE);
        }
        printf("%s\n", target_program);

        char *minimal_env[] = { "PATH=/bin:/usr/bin", NULL };
        if (execve(target_program, target_args, minimal_env) == -1) {
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
