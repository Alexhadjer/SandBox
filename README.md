# Mini Sandbox - Process Isolation & Security 

**SandBox** is a lightweight C-based sandboxing tool developed as part of the **Operating Systems FS25** course. The goal of this project is to understand and apply core OS concepts like process isolation, system call filtering, and resource restrictions—without relying on external container technologies like Docker or Firejail.

---

## Features

- **Filesystem Isolation** using `chroot()`
- **Privilege Dropping** via `setuid()` (run as "nobody")
- **Capability Reduction** to remove dangerous kernel permissions
- **Resource Limiting** using `setrlimit()` for CPU time and memory usage
- **System Call Filtering** with **seccomp** (only allow selected syscalls)
- **Minimal sandbox-root** setup for isolated file access

---

## Project Structure
SandBox/
├── bin/ # Compiled output (e.g., Firejail binary)
├── include/ # Header files (config, security, util, etc.)
├── sandbox-root/ # Filesystem root for chroot (must be created manually)
├── src/ # C source code
├── Makefile # Build instructions
└── README.md # This file
---

## Build Instructions

### 1. Install dependencies (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential libseccomp-dev
 
### 2. Clone and build
git clone https://github.com/Alexhadjer/SandBox.git
cd SandBox
make

### 3. (Optional) Set up sandbox-root
sudo mkdir -p sandbox-root/{bin,lib,lib64,dev}
sudo cp /bin/bash sandbox-root/bin/
ldd /bin/bash | awk '{print $3}' | grep '^/' | xargs -I{} sudo cp --parents {} sandbox-root/
sudo mknod -m 666 sandbox-root/dev/null c 1 3
sudo mknod -m 666 sandbox-root/dev/zero c 1 5

---

## Usage
sudo ./bin/Firejail /bin/bash
Expected ouput
[+] Resource limits applied (CPU + RAM)
[+] chroot() succeeded
[+] Dropped dangerous capabilities
[+] Seccomp filter applied
[+] Executing /bin/bash in sandbox...

---

## Testing Examples
Test CPU & memory limits:
sudo ./bin/Firejail /usr/bin/sha1sum /dev/zero
# Should terminate after ~2 seconds with: "Child killed by signal 24"
Trace system calls (requires strace)
sudo apt install strace
strace -f -o trace.log ./bin/Firejail /bin/bash
---
## Configuration

Edit include/config.h to adjust sandbox parameters:

struct sandbox_config {
    const char *rootdir;
    int enable_rlimits;
    int enable_seccomp;
    ...
};
---
## Authors

Alex Hadjer
Jennifer Jaeger
Developed as part of the Operating Systems FS25 course.
--
## License

This project is intended for educational purposes and does not guarantee full process isolation or security.
