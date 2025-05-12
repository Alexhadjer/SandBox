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
#include <sys/prctl.h>

void security_apply_seccomp(const struct sandbox_config *cfg) {
    scmp_filter_ctx ctx;

    ctx = seccomp_init(SCMP_ACT_KILL);  // Alles blockieren außer explizit erlaubt

    if (!ctx) {
        perror("seccomp_init failed");
        exit(EXIT_FAILURE);
    }

    // Erlaube grundlegende Syscalls
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(arch_prctl), 0);  // auf x86_64 wichtig

    // Filter aktivieren
    if (seccomp_load(ctx) != 0) {
        perror("seccomp_load failed");
        seccomp_release(ctx);
        exit(EXIT_FAILURE);
    }

    seccomp_release(ctx);
    printf("[+] Seccomp filter applied\n");
}

void security_apply_rlimits(const struct sandbox_config *cfg) {
    struct rlimit cpu_limit = { .rlim_cur = 2, .rlim_max = 2 };  // max. 2 Sekunden CPU-Zeit
    struct rlimit mem_limit = { .rlim_cur = 20 * 1024 * 1024, .rlim_max = 20 * 1024 * 1024 };  // 20 MB RAM

    if (setrlimit(RLIMIT_CPU, &cpu_limit) != 0) {
        perror("setrlimit CPU failed");
        exit(EXIT_FAILURE);
    }

    if (setrlimit(RLIMIT_AS, &mem_limit) != 0) {
        perror("setrlimit RAM failed");
        exit(EXIT_FAILURE);
    }

    printf("[+] Resource limits applied (CPU + RAM)\n");
}


void security_drop_caps(const struct sandbox_config *cfg) {
    if (prctl(PR_CAPBSET_DROP, CAP_NET_RAW, 0, 0, 0) == -1) {
        perror("Dropping CAP_NET_RAW failed");
        exit(EXIT_FAILURE);
    }
    if (prctl(PR_CAPBSET_DROP, CAP_SYS_ADMIN, 0, 0, 0) == -1) {
        perror("Dropping CAP_SYS_ADMIN failed");
        exit(EXIT_FAILURE);
    }
    if (prctl(PR_CAPBSET_DROP, CAP_SYS_MODULE, 0, 0, 0) == -1) {
        perror("Dropping CAP_SYS_MODULE failed");
        exit(EXIT_FAILURE);
    }

    printf("[+] Dropped dangerous capabilities\n");
}
