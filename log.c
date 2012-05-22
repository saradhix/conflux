#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "log.h"
#include "config.h"

extern struct config server;
void slog(int level, const char *fmt, ...)
{
	const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
	const char *c = ".-*#";
	time_t now = time(NULL);
	va_list ap;
	FILE *fp;
	char buf[64];
	char msg[CONFLUX_MAX_LOGLEN];

	if (level < server.loglevel) return;

	fp = (server.logfile == NULL) ? stdout : fopen(server.logfile,"a");
	if (!fp) return;

	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	strftime(buf,sizeof(buf),"%d %b %H:%M:%S",localtime(&now));
	fprintf(fp,"[%d] %s %c %s\n",(int)getpid(),buf,c[level],msg);
	fflush(fp);

	if (server.logfile) fclose(fp);

	if (server.syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}

