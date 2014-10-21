#ifndef P2P_H
#define P2P_H

/* standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <uuid/uuid.h>

/* project header files */
#include <utils.h>
#define __SERVER__
#define __CLIENT__
#include <networkingV2.h>

/* Codes to send on network with send_code() function */
typedef enum { REQUEST_PEERS, REQUEST_RESOURCE, DETACHING } Control_Codes;
/* Thread status codes */
typedef enum { LIMBO, WORKING, IDLE, EXITED } Tstatus;

/* numnodes indicates the most up-to-date known number of peers this particular
 * node has. maxnodes indicates the maximum number of peers will connect to */
typedef struct {
	uuid_t node_id;
	int socket;
	unsigned numnodes, maxnodes;
} Node;

/* comm_code and data are for passing data between threads. Will probably have
 * to add a mutex :-S */
typedef struct {
	pthread_t thread_id;
	Tstatus comm_code;
	void *data;
} Thread;

typedef struct {
	__extension__
	union {
		Node *nd;
		Thread *th;
	};
	volatile size_t size;
	pthread_mutex_t lock;
} Array;

/* must be a power of 2 */
#define NODE_ARRAY_INIT_SIZE	32
Array node_array;
Array thread_array;

/* should be renamed to init_p2p_network or something. sockfd is a server
 * socket created with create_server() (networkingV2.c). array_init_size
 * is an estimate of the number of peers we will be dealing with */
void init_node_data(int sockfd, unsigned array_init_size);

/* send data on socket socket. Returns 0 on success, errno value otherwise */
int send_data(int socket, void *data, size_t len, int flags);
int send_str(int sockfd, char *str, int flags);
int send_code(int sockfd, int code, int flags);

/* receive data from socket socket. Returns 0 on success, errno value otherwise */
int receive_data(int socket, void **data, size_t *len, int flags);
int recv_code(int sockfd, int *code, int flags);

#define NO_LIMIT	0
/* accept() incoming connections. Pass in a positive integer value or NO_LIMIT*/
void accept_incoming_nodes(unsigned max_nodes);
/* Used for launching accept_incoming_nodes in separate thread */
void *accept_incoming_nodes_launcher(void *max_nodes);

/* Add thread th to thread pool structure */
void addtothreadpool(pthread_t th);

/* cleanup all p2p resources when shutting down */
void cleanup(void);

#endif /* #ifndef P2P_H */
