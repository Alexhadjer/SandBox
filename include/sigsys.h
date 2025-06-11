//
// Created by alex on 09.06.25.
//

#ifndef SIGSYS_H
#define SIGSYS_H
 void sigsys_handler(int signo, siginfo_t *info, void *ucontext);
 void install_sigsys_handler(void);
 void install_seccomp_filter(void);
#endif //SIGSYS_H
