#include <p2p.h>

//static int epoll_fd;
#define THREAD_ARRAY_INIT_SIZE	8

void init_node_data(int sockfd, unsigned array_init_size)
{
	node_array.nd = (Node*) malloc(array_init_size * sizeof(Node));
	thread_array.th = (Thread*) malloc(THREAD_ARRAY_INIT_SIZE * sizeof(Thread));

	if(node_array.nd == NULL || thread_array.th == NULL) {
		log_message(LOG_FATAL, "Error allocating memory: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	node_array.size = thread_array.size = 0;

	uuid_generate(node_array.nd[0].node_id);
	node_array.nd[0].socket = sockfd;
	/*if((epoll_fd = epoll_create(1)) == -1) {
		log_message(LOG_FATAL, "Error creating epoll instance: %s", strerror(errno));
		exit(EXIT_FAILURE);
	} */
}

void addtothreadpool(pthread_t th)
{
	/* if needs more mem */
	if(thread_array.size >= THREAD_ARRAY_INIT_SIZE &&
			(thread_array.size & (thread_array.size - 1)) == 0)
		thread_array.th = (Thread*) realloc(thread_array.th,
				(thread_array.size << 1) * sizeof(Thread));

	thread_array.th[thread_array.size].thread_id = th;
	thread_array.th[thread_array.size].comm_code = IDLE;
	thread_array.th[thread_array.size].data = NULL;
	thread_array.size++;
}

/*
void removefromthreadpool(pthread_t th)
{

}*/

int send_code(int sockfd, int code, int flags)
{
	int ret = 0;

	if(send(sockfd, &code, sizeof code, flags) != sizeof code)
		ret = errno;
	return ret;
}

int recv_code(int sockfd, int *code, int flags)
{
	int ret = 0;

	if(recv(sockfd, code, sizeof *code, flags) != sizeof *code)
		ret = errno;
	return ret;
}

int send_str(int sockfd, char *str, int flags)
{
	int ret = 0;
	ssize_t len = strlen(str) + 1;

	if(send(sockfd, &len, sizeof len, flags) != sizeof len ||
			send(sockfd, str, len, flags) != len)
		ret = errno;
	return ret;
}

int send_data(int sockfd, void *data, size_t len, int flags)
{
	int ret = 0;
	ssize_t send_len = len;

	if(send(sockfd, &len, sizeof len, flags) != sizeof len ||
			send(sockfd, data, len, flags) != send_len)
		ret = errno;
	return ret;
}

int receive_data(int sockfd, void **data, size_t *len, int flags)
{
	int ret = 0;
	ssize_t recv_len;

	if(recv(sockfd, len, sizeof *len, flags) != sizeof *len)
		ret = errno;
	else {
		recv_len = *len;
		if((*data = malloc(*len)) == NULL ||
			recv(sockfd, *data, *len, flags) != recv_len) {
			ret = errno;
			free(*data);
		}
	}
	return ret;
}

void *accept_incoming_nodes_launcher(void *max_nodes)
{
	accept_incoming_nodes(*(int*) max_nodes);
	pthread_exit(NULL);
}

void accept_incoming_nodes(unsigned max_nodes)
{
	struct sockaddr_in client_config;
	int node_fd, server_socket = node_array.nd[0].socket;
	char buffer[INET6_ADDRSTRLEN];
	socklen_t size = sizeof client_config;
	uuid_t incoming_uuid;

	while(max_nodes == NO_LIMIT || node_array.size < max_nodes) {
		node_fd = accept(server_socket, (struct sockaddr*) &client_config, &size);
		/* if transmission fails, close connection and let client try to reconnect */
		if(recv(node_fd, &incoming_uuid, sizeof incoming_uuid, 0) != sizeof(incoming_uuid)) {
			shutdown(node_fd, SHUT_RDWR);
			continue;
		}
		node_array.nd[node_array.size].socket = node_fd;
		uuid_copy(node_array.nd[node_array.size].node_id, incoming_uuid);
		node_array.size++;
		log_message(LOG_INFO, "Connection established to %s: %u",
			inet_ntop(AF_INET, &client_config.sin_addr, buffer, INET6_ADDRSTRLEN),
			ntohs(client_config.sin_port));
	}
}

void cleanup(void)
{
	size_t i;

	for(i = 0; i < node_array.size; i++)
		shutdown(node_array.nd[i].socket, SHUT_RDWR);
	free(node_array.th);
	for(i = 0; i < thread_array.size; i++)
		pthread_cancel(thread_array.th[i].thread_id);
	free(node_array.th);
}
