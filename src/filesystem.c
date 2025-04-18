//
// Created by alex on 16.04.25.
//
int fs_prepare_root(const char *rootdir) {
    //Verify bin/, lib/, etc. exist; copy libs if missing
}

int fs_bind_mount(const char *src, const char *dst, int ro) {
    //Utility for optional bind‑mounts inside the chroot
}

int fs_make_private() {
    //	`mount(NULL, "/", NULL, MS_REC
}