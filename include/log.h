//
// Created by alex on 09.06.25.
//

#ifndef LOG_H
#define LOG_H
int  log_init(const char *path, int to_syslog,int type);   /* 0 = ok, -1 = error */

/* simple printf-style logging                               */
void log_msg(const char *fmt, ...);

/* use from a signal handler ( async-signal-safe )           */
void log_msg_safe(const char *msg, size_t len);

/* close file / flush syslog – optional                       */
void log_close(void);
#endif //LOG_H
