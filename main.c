#include <main.h>

void do_work(void)
{

}

void *worker_func_write(void *arg)
{
	Node *nd = (Node*) arg;
	char str[] = "Hello World!";
	char str2[] = "F*** off!!!";
	size_t len = strlen(str) + 1;
	int ret;

	uuid_generate(nd->node_id);
	if((ret = send_data(nd->socket, (void*) str, len, 0)) != 0) {
		log_message(LOG_WARNING, "Error sending data to node %u: %s", nd->node_id,
			strerror(errno));
	}
	if((ret = send_str(nd->socket, str2, 0)) != 0) {
		log_message(LOG_WARNING, "Error sending data to node %u: %s", nd->node_id,
			strerror(errno));
	}
	pthread_exit(&ret);
}

void *worker_func_read(void *arg)
{
	Node *nd = (Node*) arg;
	char *data = NULL;
	size_t len;
	int ret;

	uuid_generate(nd->node_id);
	if((ret = receive_data(nd->socket, (void**) &data, &len, 0)) != 0) {
		log_message(LOG_WARNING, "Error receiving data from node %u: %s", nd->node_id,
				strerror(errno));
	} else {
		printf("%s$\n", data);
		free(data);
	}
	if((ret = receive_data(nd->socket, (void**) &data, &len, 0)) != 0) {
		log_message(LOG_WARNING, "Error receiving data from node %u: %s", nd->node_id,
				strerror(errno));
	} else {
		printf("%s$\n", data);
		free(data);
	}
	pthread_exit(&ret);
}

int main(int argc, char *argv[])
{
	int serv, portnum = PORTNUM, c, max_neighbors = NO_LIMIT;
	Node client;
	pthread_t thread;

	init_log(stderr, LOG_DEBUG);
/*	log_message(LOG_INFO, "Hello World!");
	log_message(LOG_WARNING, "Hello World!");
	log_message(LOG_ERROR, "Hello World!");
	log_message(LOG_FATAL, "Hello World!");
	return 0; */

	while((c = getopt(argc, argv, "hm:p:")) != -1) {
		switch(c) {
			case 'h':
				usage(argv[0], stdout);
				exit(EXIT_SUCCESS);
			case 'm':
				max_neighbors = atoi(optarg);
				if(max_neighbors == 0) {
					if(strcmp(optarg, "NO_LIMIT") == 0)
						max_neighbors = NO_LIMIT;
					else {
						fputs("Invalid argument\n", stderr);
						usage(argv[0], stderr);
						exit(EXIT_FAILURE);
					}
				}
				break;
			case 'p':
				portnum = atoi(optarg);
				if(portnum == 0) {
					fputs("Invalid argument\n", stderr);
					usage(argv[0], stderr);
					exit(EXIT_FAILURE);
				}
				break;
			default:
				usage(argv[0], stderr);
				exit(EXIT_FAILURE);
		}
	}

	serv = create_server(portnum);
/*	if(serv != CONNECT_ERROR) {
		init_node_data(serv, NODE_ARRAY_INIT_SIZE);
		if(argc - optind == 1)
			addtothreadpool(launch_thread(&worker_func_read, &client, DETACH_THREAD));
		thread = launch_thread(&accept_incoming_nodes_launcher, &max_neighbors, NO_DETACH_THREAD);

		if(thread == 0)
			log_message(LOG_FATAL, "Error launching thread");
	} */
	/* DEMO PROG */
	Node server;
	   client.socket = connect_to("localhost", portnum);
	   server.socket = get_single_client(serv);
	   if(client.socket == CONNECT_ERROR || server.socket == CONNECT_ERROR) {
	   log_message(LOG_ERROR, "Failed to create client/server");
	   }

	   thread = launch_thread(&worker_func_read, &client, DETACH_THREAD);
	   thread = launch_thread(&worker_func_write, &server, DETACH_THREAD); //*/

	/* prevent main() from exiting before other threads finish */
	   pthread_exit(0);
	 sleep(1); //*/

	cleanup();

	return 0;
}

