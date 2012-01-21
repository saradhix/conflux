#include "log.h"
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>

FILE *dbg_file ;
char logDirStr[PATH_MAX];
char confluxlog[CONFLUX_MAX_LOGLEN];

void _conflux_log(char *log,const char *filename, int line)
{
	_conflux_file_log (AUDITFILE, log, filename, line);
}


void _conflux_file_log(char *fname, char *log,const char *filename, int line)
{
	time_t      now;
	struct tm   *utc;
	int         retStat;
	int         fd;
	char        fileStr[PATH_MAX + 1];
	char        tmpStr[BUFSIZ];


	if (log == NULL)
	{
		conflux_debug_log("conflux_log() - No text given\n");
		return;
	}
	else if (strlen(log) > CONFLUX_MAX_LOGLEN) 
	{
		log[CONFLUX_MAX_LOGLEN] = '\0';       
	}

	/* Filename of form auditfileyymmdd */
        now = time((time_t *)0);
	if (now != -1)
	  utc = localtime((time_t *)&now);

	if(utc != NULL)
	{
	(void)snprintf(tmpStr,sizeof(tmpStr),"%s%2.2d%2.2d%2.2d", fname,
		      utc->tm_year % 100, ++utc->tm_mon, utc->tm_mday);

	(void)strcpy(logDirStr, LOGDIR);
	(void)strcpy(fileStr, logDirStr);
	(void)strcat(fileStr, "/");
	(void)strcat(fileStr, tmpStr);
	}

	/* Open the file, creating it if necessary */

	do
	{
		fd = open(fileStr, O_WRONLY | O_CREAT | O_APPEND, URDWRMASK);
	}
	while ( fd == -1 && errno == EINTR );

	if (fd == -1)
	{
		conflux_debug_log("conflux_log() - Failed to open log file");
		return ;
	}

	/* Change protection to avoid umask problems */

	do
	{
		retStat = fchmod(fd, URDWRMASK);
	}
	while ( retStat == -1 && errno == EINTR );

	/* Now write the data */
	if(utc != NULL)
	{
	(void)snprintf(tmpStr,sizeof(tmpStr),
		      "%2.2d%2.2d%2.2d%2.2d%2.2d%2.2d   %s in %s, line %d\n",
		      utc->tm_year % 100, utc->tm_mon, utc->tm_mday, utc->tm_hour,
		      utc->tm_min, utc->tm_sec, log, filename, line);
	}
	while (1)
	{
		retStat = write(fd, tmpStr, (unsigned)strlen(tmpStr));
		if (retStat == -1)
		{
			if (errno != EINTR)
			{
				conflux_debug_log("conflux_log() - Log write failed\n");
				while (close(fd) == -1)
				{
					if (errno != EINTR)
					{
						conflux_debug_log("conflux_log() - Failed to close log file \n");
						return ;
					}
				}
				return ;
			}
		}
		else
		{
			break;
		}

	}

	while (close(fd) == -1)
	{
		if (errno != EINTR)
		{
			conflux_debug_log("conflux_log() - FAILED to close log file\n");
			return ;
		}
	}
	_conflux_debug_log(log, filename, line );
	return ;
}

void _conflux_debug_log( char *debug_msg,const char *filename, int line)
{
	time_t      now;
	struct tm   *utc;

	if (CONFLUX_DEBUG)
	{
		now = time((time_t *)0);
		if (now != -1)
			utc = localtime((time_t *)&now);
		if (utc != NULL)
		{
		fprintf(dbg_file, "%2.2d%2.2d%2.2d%2.2d%2.2d%2.2d   %s in %s, line %d\n",
				utc->tm_year % 100, utc->tm_mon+1, utc->tm_mday, utc->tm_hour,
				utc->tm_min, utc->tm_sec, debug_msg, filename, line);
		fflush(dbg_file);
		}
	}
}

void init_debug_file()
{
	char dbg_fl_name[PATH_MAX];
	char *str;
	char *debug_str;
	int debug;

	CONFLUX_DEBUG = FALSE;
	if ((debug_str = getenv("CONFLUX_DEBUG")) != NULL) 
	{
		debug = atoi(debug_str);
		if(debug == TRUE)
		{
			CONFLUX_DEBUG = TRUE;
			if (((str = getenv(CONFLUX_DEBUG_FILE)) != NULL ) && strlen(str) > 0) 
			{
				strncpy(dbg_fl_name,str,PATH_MAX);

			}
			else
			{
				strncpy(dbg_fl_name,DEF_CONFLUX_DBG_FILE,PATH_MAX);


			}

			if ((dbg_file = fopen(dbg_fl_name, "a+")) == NULL)

				dbg_file = stderr;
			snprintf(confluxlog,sizeof(confluxlog),"DEBUG File name is: %s",dbg_fl_name);
			conflux_log(confluxlog);

		}

	}
}


