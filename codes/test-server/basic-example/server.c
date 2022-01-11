#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

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
	// socket() //iterate
	// bind() //iterate
	// listen()
	// accept()
	
	if ((status = getaddrinfo(NULL, "3939", &hints, &res)) != 0) {
		fprintf(stderr, "error on getaddrinfo()\n");
		exit(1);
	}
	
	struct addrinfo *p;
	
	// loop through all the results and bind the first we can
	for (p = res; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			fprintf(stderr, "error on socket(): %s\n", strerror(errno));
			continue;
		}
		
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) != 0) {
			fprintf(stderr, "error on bind(): %s\n", strerror(errno));
			continue;
		}
		
		break;
	}
	
	freeaddrinfo(res); // we don't need the results anymore
	
	if (listen(sockfd, 5) == -1) {
		fprintf(stderr, "error on listen()\n");
		exit(1);
	}
	
	struct sockaddr_storage their_addr;
	char s[INET6_ADDRSTRLEN];
	
	printf("%s\n", "Waiting for connections...");
	
	while (1) { // main accept() loop
		socklen_t addr_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size); // accept the new client
		if (new_fd == -1) {
			fprintf(stderr, "error on accept\n");
			continue;
		}
		
		inet_ntop(their_addr.ss_family, (struct sockaddr *)&their_addr, s, sizeof s); // get the client ip
		printf("New connection from: %s\n", s);
		
		if (!fork()) {
			close(sockfd);
			char *msg = "Hello Client! I am the server!\n";
			int len = strlen(msg);
			if (send(new_fd, msg, len, 0) == -1) {
				fprintf(stderr, "error: %s", strerror(errno));
				exit(0);
			}
			close(new_fd); // close client connection
			exit(0);
		}
		close(new_fd); // close client connection
	}
	
	return 0;
}
