#ifndef MAIN_H
#define MAIN_H

/* standard header files */
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

/* project header files */
#include <p2p.h>

/* Default port number to use when creating servers */
#define PORTNUM		9600

/* demo functions. main.c is for code that tests the rest of the project */
void do_work(void);
void *worker_func_write(void *arg);
void *worker_func_read(void *arg);

#endif /* #ifndef MAIN_H */
