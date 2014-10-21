#include <utils.h>

void usage(char *progname, FILE *stream)
{
	fprintf(stream, "Usage: %s [OPTION]... [HOSTNAME]\n"
			"Options:"	"\t-h"	"\tdisplay this help message\n"
					"\t\t\t-pARG"	"\tset port number to ARG\n",
					progname);
}

pthread_t launch_thread(void* (*start_routine)(void*), void *arg, int detach)
{
	pthread_t th;
	if(pthread_create(&th, NULL, start_routine, arg) != 0)
		log_message(LOG_ERROR, "Error creating thread!\n");
	else if(detach && pthread_detach(th) != 0)
		log_message(LOG_ERROR, "Error detaching thread\n");
	return th;
}

/* From 
http://www.emoticode.net/c/an-example-log-function-using-different-log-levels-and-variadic-macros.html
 */
static log_level_t __g_loglevel = LOG_DEBUG;
static FILE *__g_loghandle = NULL;

void init_log(FILE *stream)
{
	__g_loghandle = stream;
}

void log_message(log_level_t level, const char *format, ...)
{
	char buffer[255] = { 0 }, timestamp[255] = { 0 }, *slevel;
	va_list ap;
	time_t rawtime;
	struct tm *timeinfo;
	Color color = WHITE, bgcolor = BLACK;

	if(level >= __g_loglevel) {
		va_start(ap, format);
		vsnprintf(buffer, 255, format, ap);
		va_end(ap);
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(timestamp, 255, "%d/%m/%Y %X", timeinfo);

		switch(level) {
			case LOG_DEBUG:
				slevel = "DEBUG";
				break;
			case LOG_INFO:
				slevel = "INFO";
				color = GREEN;
				break;
			case LOG_WARNING:
				slevel = "WARNING";
				color = YELLOW;
				break;
			case LOG_ERROR:
				slevel = "ERROR";
				color = RED;
				break;
			case LOG_FATAL:
			default:
				slevel = "FATAL";
				bgcolor = RED;
				break;
		}
		if(__g_loghandle == stdout || __g_loghandle == stderr) {
			fprintf(__g_loghandle, "\x1B[%d;0m", color + 30);
			fprintf(__g_loghandle, "\x1B[%dm[%s] [%s] %s\n", bgcolor + 40, timestamp, slevel, buffer);
			reset_style(__g_loghandle);
			fflush(__g_loghandle);
		} else
			fprintf(__g_loghandle, "[%s] [%s] %s\n", timestamp, slevel, buffer);
	}
}

