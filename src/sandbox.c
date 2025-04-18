#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

pid_t sandbox_create(const struct sandbox_cfg *cfg) {
    /*fork, unshare, chroot, drop privs, exe
     */
}
static void child_setup(const struct sandbox_cfg *cfg) {
    //Runs inside the child right after fork(). Calls the helpers below in order.
}

static void setup_namespaces(const struct sandbox_cfg *cfg) {
    //unshare() PID, mount, user, net namespaces as requested.
}


static void enter_chroot(const char *new_root) {
    //chroot
}


static void drop_privileges(uid_t uid, gid_t gid){
    //setgid(), setuid(), maybe prctl(PR_SET_NO_NEW_PRIVS, 1)
    }
static void exec_target(char *const argv[]) {
    //Thin wrapper around execve()
}
