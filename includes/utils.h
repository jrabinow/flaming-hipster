#ifndef UTILS_H
#define UTILS_H

/* standard header files */
#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>

/* define C89, C90 or C99 to know further on what version of C
 * we are using */
#if defined(__STDC__)
# define C89
# if defined(__STDC_VERSION__)
#  define C90
#  if (__STDC_VERSION__ >= 199901L) && ! defined C99
#   define C99
#  endif
# endif
#endif

/* bool is bad. Disabling until there's an actual reason to use them
 * https://lkml.org/lkml/2013/8/31/138 */ /*
#ifdef C99
# include <stdbool.h>
#else
 typedef enum { false = 0, true = 1 } bool;
#endif
*/

#define DETACH_THREAD		1
#define NO_DETACH_THREAD	0

/* launches start_routine in a new thread with arg as argument. detach is a flag
 * to tell if we should call pthread_detach on new thread or not.
 * Returns thread id variable */
pthread_t launch_thread(void *(start_routine)(void*), void *arg, int detach);

typedef enum {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FATAL
} log_level_t;

/* Initialize log system on file stream. You MUST call this before calling
 * the log_message function */
void init_log(FILE *stream);
/* logs a message following printf conventions */
void log_message(log_level_t level, const char *format, ... );

typedef enum { BLACK, RED, GREEN, YELLOW, BLUE, PINK, CYAN, WHITE } Color;
/* cancel all color codes sent to terminal and reset to default */
#define reset_style(stream)	fprintf(stream, "\x1B[0m")

/* writes a help message about how to invoke the program on file stream */
void usage(char *progname, FILE *stream);

#endif /* #ifndef UTILS_H */
