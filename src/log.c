//
// Created by alex on 09.06.25.
//
#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/syscall.h>
#include <syslog.h>
#include <stdarg.h>
#include <errno.h>

static int  fd   = -1;
static int  use_syslog = 0;
static int detailed = 0;

static void timestamp(char *buf, size_t len)
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    struct tm tm;
    localtime_r(&t.tv_sec, &tm);
    strftime(buf, len, "%F %T", &tm);   /*In the format YYYY-MM-DD HH:MM:SS */
}

int log_init(const char *path, int to_syslog,int type)
{
    detailed = type; //setting it to using the date default.
    use_syslog = to_syslog;
    if (use_syslog) {
        openlog("sandbox", LOG_PID|LOG_NDELAY, LOG_USER); //makes with sandbox pre-fix no delay P_IP and the user level messages
        return 0;
    }

    fd = open(path, O_WRONLY|O_CREAT|O_APPEND, 0644); //write/apend only create if not existend
    if (fd < 0) {return -1;}
    else return 0;
}

void log_msg(const char *format_text, ...)
{
    char line[512], timestamp_buff[32];
    if (detailed)
    {
        timestamp(timestamp_buff, sizeof(timestamp_buff));
    }//always adding timestamp. There was an plan to add mode with simpl logs
    va_list argumet;
    va_start(argumet, format_text);
    int n = snprintf(line, sizeof(line), "[%s pid=%d] ",
                     timestamp_buff, (int)getpid());
    n += vsnprintf(line + n, sizeof(line) - n, format_text, argumet); //ad line
    va_end(argumet);
    //Nuline
    if (line[n-1] != '\n' && n < (int)sizeof(line)-1) {
        line[n++] = '\n';
        line[n]   = '\0';
    }

    if (use_syslog) {
        syslog(LOG_INFO, "%s", line);
    } else if (fd != -1) {
        write(fd, line, n);
    }
}

void log_msg_safe(const char *msg, size_t len)
{
    // write() only
    if (fd != -1) {
        write(fd, msg, len);
        write(fd, "\n", 1);
    }
}

void log_close(void)
{
    if (use_syslog) {
        closelog();
    } else if (fd != -1) {
        close(fd);
        fd = -1;
    }
}
