//
// Created by alex on 16.04.25.
//
void security_apply_seccomp(const struct sandbox_config *cfg) {
    //Build libseccomp filter, seccomp_load().
}

void security_apply_rlimits(const struct sandbox_config *cfg) {
    //setrlimit(RLIMIT_CPU / AS / NOFILE …)
}


void security_drop_caps(const struct sandbox_config *cfg) {
    //Use <sys/capability.h> or prctl() to clear unwanted capabilities
}