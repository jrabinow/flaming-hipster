#ifndef NETWORKING_H
#define NETWORKING_H

/* standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

extern int errno;

/* local header files */
#include <utils.h>

/* Networking */
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#define CONNECT_ERROR		-1

/***************** server *****************/
#ifdef __SERVER__

#define SERVER_TERMINATED	-1
/* Before being accept()ed, clients are queued. This is the default length
 * of the queue before new connections are dropped instead of being queued */
#define DEFAULT_MAX_CLIENTS	SOMAXCONN

/* Initializes a new server on port port. Returns a socket which you can
 * use to accept() new clients */
int create_server(unsigned port);
/* accept()s exactly one client. server_socket is a socket created with
 * create_server.
 * Returns socket you can use with send() or recv() */
int get_single_client(int server_socket);

#endif /* #ifdef __SERVER__ */

/***************** client *****************/
#ifdef __CLIENT__
int connect_to(char *server_name, unsigned port); 
#endif /* #ifdef __CLIENT__ */

#endif /* #ifndef NETWORKING_H */
