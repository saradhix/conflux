#ifndef conflux_log_H
#define conflux_log_H

#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_NOTICE 2
#define LOG_WARNING 3

#define CONFLUX_MAX_LOGLEN 1024

void slog(int level, const char *fmt, ...);

#endif

