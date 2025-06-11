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

## Authors
Alex Hadjer; Jennifer Jäger __ develpoted as part of the Operating Systems FS25 course.

---

## License

This project is intended for educational purposes and does not guarantee full process isolation or security.
