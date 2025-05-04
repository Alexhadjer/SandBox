#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/prctl.h>  /* Definition of PR_* constants */
#include <sys/prctl.h>
#define _GNU_SOURCE
#include  <sys/stat.h>
#include <sys/mount.h>

#include "../include/config.h"

pid_t sandbox_create( struct sandbox_config *cfg) {
    /*fork, unshare, chroot, drop privs, exe
     */
}
 void child_setup( struct sandbox_config *cfg) {
    //Runs inside the child right after fork(). Calls the helpers below in order.
}

 void setup_namespaces( struct sandbox_config *cfg) {
    //unshare() PID, mount, user, net namespaces as requested.y
}



 void enter_chroot(struct sandbox_config *config) {
    //chroot

    if (chdir(config->rootdir) == -1) {
        perror("failed entering chroot");
    }
    else {
        printf("chroot() succeeded\n");
    }
    if (chroot(".") == -1){
    perror("chroot() failed");
    exit(EXIT_FAILURE);

}
if (chdir("/") == -1) {
        perror("chdir(/) after chroot failed");
        exit(EXIT_FAILURE);
    }
    mkdir("/proc", 0555);
    if (mount("proc", "/proc", "proc", 0, NULL) == -1) {
        perror("mount(/proc) failed");
        exit(EXIT_FAILURE);
    }
    }



static void drop_privileges(const struct sandbox_config *cfg){
    //setgid(), setuid(), maybe prctl(PR_SET_NO_NEW_PRIVS, 1)
    const uid_t uid = cfg->uid;
    const gid_t gid = cfg->uid;
    setuid(uid);
    setgid(gid);
    prctl(PR_SET_NO_NEW_PRIVS, 1); // Set the calling thread's no_new_privs attribute.  With
    //no_new_privs set to 1, execve(2) promises not to grant privileges
    //to do anything that could not have been done without the execve(2) call //copied from api
    }
static void exec_target(char *const argv[]) {
    //Thin wrapper around execve()
}
