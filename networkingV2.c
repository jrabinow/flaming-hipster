#include <networkingV2.h>

int connect_to(char *server_name, unsigned port)
{
	struct addrinfo *adresses = NULL, *iterator = NULL;
	char ip_addr[INET6_ADDRSTRLEN] = { 0 }, port_buff[11] = { 0 };
	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	snprintf(port_buff, 11, "%u", port);
	if(sockfd == -1) {
		log_message(LOG_ERROR, "Error creating socket: %s", strerror(errno));
		sockfd = CONNECT_ERROR;
	} else if(getaddrinfo(server_name, port_buff, NULL, &adresses) != 0) {
		/* DNS resolution. getaddrinfo creates a linked list (pointed to by adresses),
		 * 1 element == 1 IP Address. Function IPv6 compatible. */
		log_message(LOG_WARNING, "Unable to resolve '%s' to a valid IP address", server_name);
		sockfd = CONNECT_ERROR;
	} else {
		log_message(LOG_INFO, "DNS lookup succesful");
		iterator = adresses;
		// Iterate over linked list until we connect successfully or we run out of addresses.
		while(connect(sockfd, iterator->ai_addr, (socklen_t) sizeof *iterator->ai_addr)
				== CONNECT_ERROR) {
			iterator = iterator->ai_next;
			if(iterator == NULL) {
				sockfd = CONNECT_ERROR;
				break;
			}
		}
		if(iterator != NULL)
			log_message(LOG_INFO, "Connection established with %s (%s:%d)", server_name,
					inet_ntop(iterator->ai_family,
						&((struct sockaddr_in*) iterator->ai_addr)->sin_addr,
						ip_addr, sizeof ip_addr),
					ntohs(((struct sockaddr_in*) iterator->ai_addr)->sin_port));
		freeaddrinfo(adresses);
	}
	return sockfd;
}

int create_server(unsigned port)
{
	int sockfd;
	unsigned queue = 1;
	struct sockaddr_in config;

	bzero(&config, sizeof config);
	config.sin_family = PF_INET;
	config.sin_addr.s_addr = htonl(INADDR_ANY);
	config.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		log_message(LOG_FATAL, "Error creating socket: %s", strerror(errno));
		sockfd = CONNECT_ERROR;
	} else {
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &queue, sizeof queue) != 0)
			log_message(LOG_ERROR, "Error setting socket option SO_REUSEADDR: %s", strerror(errno));
		/* print error message but keep on going. Failing here will not prevent program from
		 * working correctly */
		queue = SOMAXCONN;
		if(bind(sockfd, (struct sockaddr*) &config, sizeof config) == CONNECT_ERROR || 
				listen(sockfd, queue) != 0) {
			shutdown(sockfd, SHUT_RDWR);
			log_message(LOG_FATAL, "Error binding server %d: %s", port, strerror(errno));
			sockfd = CONNECT_ERROR;
		} else
			log_message(LOG_INFO, "Server initialized on port %u", port);
	}

	return sockfd;
}

int get_single_client(int server_socket)
{
	struct sockaddr_in client_config;
	int client_fd;
	char buffer[INET6_ADDRSTRLEN];
	socklen_t size = sizeof client_config;

	client_fd = accept(server_socket, (struct sockaddr*) &client_config, &size);
	log_message(LOG_INFO, "Connection established to %s:%u",
			inet_ntop(AF_INET, &client_config.sin_addr, buffer, INET6_ADDRSTRLEN),
			ntohs(client_config.sin_port));

	return client_fd;
}

