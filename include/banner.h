/* banner.h ── include once anywhere you want the greeting */
#ifndef SANDBOX_BANNER_H
#define SANDBOX_BANNER_H

#include <stdio.h>

/* ANSI colour codes */
#define C_RESET   "\033[0m"
#define C_CYAN    "\033[96m"
#define C_GREEN   "\033[92m"
#define C_YELLOW  "\033[93m"
#define C_WHITE     "\033[37m"
#define C_MAGENTA   "\033[35m"
#define C_BLUE     "\033[34m"
#define C_BROWN    "\033[1m"
#define C_RED     "\033[31m"
#define C_GREY    "\033[37m"

static inline void sandbox_banner(void)
{
    puts(C_CYAN
"╔═════════════════════════════════════════════════════╗\n"
"║" C_GREEN "      Welcome to SandBox-Jail v1.0                   " C_CYAN "║\n"
"║" C_YELLOW "  Your program is now running safely!                " C_CYAN "║\n"
"║"C_CYAN"  Authors: Jennifer Jaeger and Alexander Hadjistamov " C_CYAN "║\n"
"╚═════════════════════════════════════════════════════╝" C_RESET);
}

#endif
