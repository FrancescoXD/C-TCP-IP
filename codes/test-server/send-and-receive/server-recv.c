#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define BUFF_SIZE 512

int main(void) {
	int status, sockfd, new_fd;
	struct addrinfo *res;
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};
	
	// steps:
	// getaddrinfo()
	// socket()
	// bind()
	// listen()
	// accept()
	
	// here is useless to iterate with a for because it's only one server.
	
	if ((status = getaddrinfo(NULL, "3939", &hints, &res)) != 0) {
		fprintf(stderr, "error on getaddrinfo()\n");
		exit(EXIT_FAILURE);
	}
	
	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		fprintf(stderr, "error on socket(): %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
		
	if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
		fprintf(stderr, "error on bind(): %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	freeaddrinfo(res); // we don't need the results anymore
	
	if (listen(sockfd, 5) == -1) {
		fprintf(stderr, "error on listen()\n");
		exit(1);
	}
	
	struct sockaddr_storage their_addr;
	char client_ip[INET6_ADDRSTRLEN];
	
	printf("%s\n", "Waiting for connections...");
	
	while (1) { // main accept() loop
		socklen_t addr_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size); // accept the new client
		if (new_fd == -1) {
			fprintf(stderr, "error on accept\n");
			continue;
		}
		
		inet_ntop(their_addr.ss_family, (struct sockaddr *)&their_addr, client_ip, sizeof client_ip); // get the client ip
		printf("New connection from: %s\n", client_ip);
		
		if (!fork()) {
			close(sockfd);
			char *msg = "Hello Client! I am the server!\n";
			int len = strlen(msg);
			if (send(new_fd, msg, len, 0) == -1) {
				fprintf(stderr, "error: %s", strerror(errno));
				exit(0);
			}
			// receive data from client
			char res_c[BUFF_SIZE];
			int b = recv(new_fd, res_c, BUFF_SIZE - 1, 0);
			res_c[b] = '\0';
			printf("%s sent: %s\n", client_ip, res_c); 
			
			close(new_fd); // close client connection
			exit(0);
		}
		close(new_fd); // close client connection
	}
	
	return 0;
}
