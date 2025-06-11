//
// Created by alex on 16.04.25.
//
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/config.h"
#include <sys/prctl.h>
#include <linux/capability.h>
#include <seccomp.h>
#include <signal.h>
#include <ucontext.h>
#include <sys/prctl.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sched.h>
#include "../include/log.h"
#include "../include/sigsys.h"
#include "../include/log.h"


void security_apply_seccomp(const struct sandbox_config *cfg) {
    scmp_filter_ctx ctx;
    install_sigsys_handler();
    ctx = seccomp_init(SCMP_ACT_ERRNO(SCMP_ACT_TRAP));  // Blockiert alle calls ausser whitelisted

    if (!ctx) {
        log_msg("ERROR: seccomp_init failed");
        exit(EXIT_FAILURE);
    }
    //Prevent trights
    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);

    // Allow basic Syscalls
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(arch_prctl), 0);  // auf x86_64 wichtig
    //for bash shell check
    //newfstat
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(newfstatat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(access),     0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(statx),      0);


    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat),       0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close),        0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(lseek),        0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(readv),        0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(writev),       0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(futex),        0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(set_tid_address), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigaction),  0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigprocmask),0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(sigaltstack),   0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(prlimit64),     0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getrandom),     0);
    //seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve),     0); //big no
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(set_robust_list),0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(lseek),          0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(prctl),         0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rseq), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(readlinkat),0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect),0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat2), 0);   /* glibc ≥ 2.36 */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(faccessat2), 0);
    /* --- ld.so / glibc ------ */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(pread64),    0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(preadv),     0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(preadv2),    0);

    /* --- FD control & directory listing ----- */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fcntl),      0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getdents64), 0);

    /* --- event loop  -----*/
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(poll),           0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(ppoll),          0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clock_gettime),  0);
    /* -- process----- */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getcwd),   0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getpgid),  0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(setpgid),  0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(setsid),   0);

    /* ioctl was already allowed for terminal control, but make sure: */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(ioctl),    0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 0);
     seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execveat), 0);
    // Filters activate
    if (seccomp_load(ctx) != 0) {
        log_msg("ERROR: seccomp_load failed");
        perror("seccomp_load failed");
        seccomp_release(ctx);
        exit(EXIT_FAILURE);
    }

    seccomp_release(ctx);
    log_msg("INFO: seccomp filter +");
}

void security_apply_rlimits(const struct sandbox_config *cfg) {
    struct rlimit cpu_limit = { .rlim_cur = 2, .rlim_max = 2 };  // max. 2 Sekunden CPU-Zeit
    struct rlimit mem_limit = { .rlim_cur = 20 * 1024 * 1024, .rlim_max = 20 * 1024 * 1024 };  // 20 MB RAM

    if (setrlimit(RLIMIT_CPU, &cpu_limit) != 0) {
        log_msg("ERROR: setrlimit CPU failed");
        exit(EXIT_FAILURE);
    }

    if (setrlimit(RLIMIT_AS, &mem_limit) != 0) {
        log_msg("ERROR: setrlimit RAM failed");
        exit(EXIT_FAILURE);
    }
    log_msg("[+] Resource limits applied (CPU + RAM)");
}


void security_drop_caps(const struct sandbox_config *cfg) {
    if (prctl(PR_CAPBSET_DROP, CAP_NET_RAW, 0, 0, 0) == -1) {
        log_msg("ERROR: Dropping CAP_NET_RAW failed");
        exit(EXIT_FAILURE);
    }
    if (prctl(PR_CAPBSET_DROP, CAP_SYS_ADMIN, 0, 0, 0) == -1) {
        log_msg("ERROR: Dropping CAP_SYS_ADMIN failed");
        exit(EXIT_FAILURE);
    }
    if (prctl(PR_CAPBSET_DROP, CAP_SYS_MODULE, 0, 0, 0) == -1) {
        log_msg("ERROR: Dropping CAP_SYS_MODULE failed");
        exit(EXIT_FAILURE);
    }
    log_msg("[+] Dropped dangerous capabilities");
}
