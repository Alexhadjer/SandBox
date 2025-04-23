//
// Created by alex on 16.04.25.
//

#ifndef CONFIG_H
#define CONFIG_H
typedef struct sandbox_config {
    const char *rootdir;     // e.g., "/sandbox-root"
    uid_t uid;               // user ID to drop to, e.g., 65534
    gid_t gid;               // group ID to drop to
    int use_namespaces;      // 1 = enable unshare(CLONE_NEWNS), etc.
    int enable_seccomp;      // 1 = restrict syscalls
    int enable_rlimits;      // 1 = apply resource limits
    // ... add more flags later as needed
}cfg;
#endif //CONFIG_H
