//
// Created by alex on 16.04.25.
//

#define GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "../include/sandbox.h"
#include "../include/security.h"
#include "../include/util.h"
#include "../include/config.h"
#include  <sys/stat.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "../include/banner.h"
#include "../include/log.h"
#include "../include/security.h"
#include "../include/sandbox.h"
#include <sys/sysmacros.h>
#include <pty.h>
#include <seccomp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/sigsys.h"

extern char **environ;
int soft_env = 0;

int main(int argc, char *argv[]) {
    int argi = 1;

    if (argc > 1 && strcmp(argv[argi], "--softenv") == 0) {
        soft_env = 1;
        argi++;
    }
    if (argi >= argc) {
        fprintf(stderr, "Usage: %s [--softenv] <exec> [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *target = argv[argi];
    char **targv = &argv[argi];

    // 1) Initialize logging (parent+child)
    if (log_init("./sandbox.log", 0,1) != 0) {
        perror("log_init");
        return EXIT_FAILURE;
    }
    log_msg("Starting sandbox [argc=%d]", argc);
    sandbox_banner();
    log_msg("INFO: Sandbox started");

    // 2) Load config
    sandbox_config cfg = setup_config();
    if (soft_env) {
        cfg.enable_seccomp = 0;
        cfg.enable_rlimits = 0;
        cfg.use_namespaces = 0;
    }
    log_msg("Config: rootdir=%s seccomp=%d rlimits=%d namespaces=%d",
            cfg.rootdir, cfg.enable_seccomp, cfg.enable_rlimits, cfg.use_namespaces);

    // 3) Fork
    pid_t pid = fork();
    if (pid < 0) {
        log_msg("ERROR: fork failed: %s", strerror(errno));
        log_close();
        return EXIT_FAILURE;
    }
    else if (pid == 0) {
        // Child
        log_msg("Child[%d]: start, target=%s", getpid(), target);

        // a) chroot/jail setup
        enter_chroot(&cfg) ;


        log_msg("Child[%d]: chroot OK", getpid());

        // b) drop privileges
        drop_privileges(&cfg);
        log_msg("Child[%d]: privileges dropped to uid=%d", getpid(), cfg.uid);

        // c) prepare environment
        new_environ(soft_env);
        log_msg("Child[%d]: env sanitized (soft_env=%d)", getpid(), soft_env);

        // d) install SIGSYS handler
        install_sigsys_handler();
        log_msg("Child[%d]: SIGSYS handler installed", getpid());

        // e) install seccomp filter
        if (cfg.enable_seccomp) {
            log_msg("Child[%d]: installing seccomp filter", getpid());
            security_apply_seccomp(&cfg);
            log_msg("Child[%d]: seccomp filter installed", getpid());
        }
        char *safe_env[] = { "PATH=/usr/bin:/bin", "TERM=xterm-256color", NULL };


        // your argv too:
        char* const argv[] = { "/bin/sh", "bin/test.sh", NULL };
        // f) execute target
        log_msg("Child[%d]: execve %s", getpid(), target);
        execve(argv[0], argv, (char* const*)safe_env);
        perror("execve");
        // if execve returns, it failed
        log_msg("ERROR: execve failed: %s", strerror(errno));
        _exit(EXIT_FAILURE);
    }
    else {
        // Parent
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            log_msg("ERROR: waitpid failed: %s", strerror(errno));
            log_close();
            return EXIT_FAILURE;
        }
        if (WIFEXITED(status)) {
            log_msg("Child[%d] exited with code %d", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            log_msg("Child[%d] killed by signal %d", pid, WTERMSIG(status));
        }
    }

    log_msg("Sandbox run complete");
    log_close();
    return EXIT_SUCCESS;
}