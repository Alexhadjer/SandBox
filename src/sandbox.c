#define _GNU_SOURCE
#include <limits.h>     /* for PATH_MAX */
#include  <sys/stat.h>
#include <sys/mount.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>
#include "security.c"
#include "../include/banner.h"
#include "../include/log.h"
#include <sys/sysmacros.h>
#include <pty.h>
#include <sched.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/prctl.h>  /* Definition of PR_* constants */
#include <sys/prctl.h>
#include <sys/sysmacros.h>
#include <pty.h>
#include "../include/log.h"
#include "../include/util.h"
#include <stdio.h>         // for perror(), fprintf()
#include <stdlib.h>
#include "../include/config.h"
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include "../include/sigsys.h"

pid_t sandbox_create( struct sandbox_config *cfg) {
    /*fork, unshare, chroot, drop privs, exe
     */
}
 void child_setup( struct sandbox_config *cfg) {
    //Runs inside the child right after fork(). Calls the helpers below in order.
}

 void setup_namespaces( struct sandbox_config *cfg) {
    //unshare() PID, mount, user, net namespaces as requested.y
}static void safe_mount_devtmpfs(const char *tmpmnt)
{
    /* 1) Make sure our temporary mount point exists */
    if (mkdir(tmpmnt, 0755) == -1 && errno != EEXIST)
        die("mkdir devtmpfs dir");

    /* 2) Unmount any stale mounts */
    umount2(tmpmnt, MNT_DETACH);       // ignore errors
    umount2("/dev", MNT_DETACH);

    /* 3) Mount the new devtmpfs on tmpmnt */
    if (mount("devtmpfs", "/dev-new", "devtmpfs", 0, NULL) < 0)
        die("mount devtmpfs");
    system("grep /dev-new /proc/self/mountinfo");

    /* 4) Move it into place: tmpmnt → /dev */
    if (mount(tmpmnt, "/dev", NULL, MS_MOVE, NULL) < 0)
        die("move devtmpfs to /dev");
}

void enter_chroot(struct sandbox_config *config)
{
    /* chroot into the new rootdir */
    char host_bin[PATH_MAX];

    //  New mount namespace + private mounts
    if (config->use_namespaces){
    if (unshare(CLONE_NEWNS) < 0) {
        perror("unshare(CLONE_NEWNS)"); return -1;
    }
    if (mount(NULL, "/", NULL, MS_REC|MS_PRIVATE, NULL) < 0) {
        perror("mount MS_PRIVATE"); return -1;
    }
    }
    else
    {
        log_msg("No namespaces because soft mode");
    }
    // Bind host /bin → sandbox-root/bin
    snprintf(host_bin, sizeof(host_bin), "%s/bin", config->rootdir);
    if (mkdir(host_bin, 0755) == -1 && errno != EEXIST) {
        perror("mkdir host_bin"); return -1;
    }
    if (mount("/bin", host_bin, NULL, MS_BIND|MS_REC, NULL) < 0) {
        perror("mount bind /bin"); return -1;
    }

    if (chdir(config->rootdir) < 0) {
        log_msg("ERROR: Chdir failed");
        exit(EXIT_FAILURE);
    }
    if (chroot(".") < 0) {
        log_msg("ERROR: chroot()");
        exit(EXIT_FAILURE);
    }
    if (chdir("/") < 0) {
        log_msg("ERROR: chdir() after chroot");
        exit(EXIT_FAILURE);
    }
    umount2("/dev", MNT_DETACH); //unmounting to not get dev device already in use
    if (mount("devtmpfs", "/dev", "devtmpfs", 0, NULL) < 0) {
        log_msg("ERROR: mount devtmpfs");return -1;

    }
    // devpts + ptmx + tty
    mkdir("/dev/pts", 0755);
    if (mount("devpts", "/dev/pts", "devpts", 0,
              "newinstance,ptmxmode=0666,mode=0620,gid=5") < 0) {
        log_msg("ERROR: mount /dev/pts");
        return -1;
              }
    // Create /dev/ptmx — the pseudoterminal multiplexer (used for spawning terminals)
    mknod("/dev/ptmx", 0666, makedev(5,2));
    symlink("pts/ptmx", "/dev/ptmx");
    // Create /dev/tty — represents the controlling terminal for the current process
    mknod("/dev/tty",  0666, makedev(5,0));
    log_msg("Mounted /dev/ptmx");
    
    // Setup of /proc + /sys
    mkdir("/proc", 0555);
    log_msg("mkdir /proc");
    umount2("/proc", MNT_DETACH); log_msg("unmount /proc");
    // Mount procfs — needed to access process/system info
    if (mount("proc", "/proc", "proc", 0, NULL) < 0)    { log_msg("ERROR: when mounting /proc"); return -1; }

    mkdir("/sys", 0555); log_msg("mkdir /sys");
    umount2("/sys", MNT_DETACH); log_msg("unmount /sys");
    // Mount sysfs — provides hardware and kernel information to userspace
    if (mount("sysfs", "/sys", "sysfs", 0, NULL) < 0)   { log_msg("ERROR: when mounting /sys"); return -1; }

    return 0;
    }
 int launch_shell(struct sandbox_config *config) {
    //no longer used, used for debugging.
    //Here you can see how it was before loggin everythin here
    printf("DEBUG: Attempting to launch shell\n");
    fflush(stdout);

    // Check if shell exists
    if (access("/bin/bash", X_OK) == 0) {
        printf("DEBUG: Found /bin/bash, executing...\n");
        fflush(stdout);
        execl("/bin/bash", "bash", NULL);
    } else if (access("/bin/sh", X_OK) == 0) {
        printf("DEBUG: Found /bin/sh, executing...\n");
        fflush(stdout);
        execl("/bin/sh", "sh", NULL);
    } else {
        printf("ERROR: No shell found in chroot!\n");
        printf("Contents of /bin:\n");
        system("ls -la /bin 2>/dev/null || echo 'Cannot list /bin'");
        return -1;
    }




}

 void drop_privileges(const struct sandbox_config *cfg){
    //setgid(), setuid(), maybe prctl(PR_SET_NO_NEW_PRIVS, 1)
    const uid_t uid = cfg->uid; //setting priv.
    const gid_t gid = cfg->uid; //setting group priv.
    setuid(uid);
    setgid(gid);
    prctl(PR_SET_NO_NEW_PRIVS, 1); // Set the calling thread's no_new_privs attribute.  With
    //no_new_privs set to 1, execve(2) promises not to grant privileges
    //to do anything that could not have been done without the execve(2) call //copied from api
    }
void new_environ(int soft_env){
    clearenv();
    setenv("PATH", "/usr/bin:/bin", 1);
    setenv("HOME", "/nonexistent", 1);
    setenv("USER", "sandbox", 1);
    setenv("PWD", "/", 1);

    if (soft_env) {
        // preserve LANG, TERM, SHELL from host
        const char *lang = getenv("LANG");
        const char *term = getenv("TERM");
        const char *shell = getenv("SHELL");

        setenv("LANG", lang ? lang : "C.UTF-8", 1);
        setenv("TERM", term ? term : "xterm-256color", 1);
        setenv("SHELL", shell ? shell : "/bin/sh", 1);

        log_msg("Softenv active: LANG=%s TERM=%s SHELL=%s",
                getenv("LANG"), getenv("TERM"), getenv("SHELL"));
    } else {
        // hard sanitize
        setenv("LANG", "C.UTF-8", 1);
        setenv("TERM", "xterm-256color", 1);
        setenv("SHELL", "/bin/sh", 1);
        log_msg("Hard sanitized env set");
    }

}
static void exec_target(char *const argv[]) {
    //Thin wrapper around execve()
}
