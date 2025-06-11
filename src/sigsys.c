//
// Created by alex on 09.06.25.
//

#define _GNU_SOURCE
#include <seccomp.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/prctl.h>
#include "../include/log.h"

 void sigsys_handler(int signo, siginfo_t *info, void *ucontext)
{
    (void)signo;              /* unused */
    //find out syscall when using wrong one
    long syscall_nr = info->si_syscall;       /* available since Linux 3.17 */

    //logging in buffer
    char buf[256];
    snprintf(buf, sizeof(buf),
            "[SIGSYS] blocked by syscall=%ld errno=%d",
            syscall_nr,  info->si_errno);
    log_msg(buf);

    _exit(1);
}

 void install_sigsys_handler(void)
{   //signal action structure
    struct sigaction sa = {0};
    sa.sa_sigaction = sigsys_handler; //adding a handler
    sa.sa_flags     = SA_SIGINFO;  //special flag
    if (sigaction(SIGSYS, &sa, NULL) != 0) { //set action for SYG
        log_msg(" ERROR: sigaction failed");
        exit(EXIT_FAILURE);
    }
}

