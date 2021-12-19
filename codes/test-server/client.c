#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFF_SIZE 512

int main(void) {
	int status, sockfd;
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};
	struct addrinfo *res, *p;
	
	if ((status = getaddrinfo(NULL, "3939", &hints, &res)) != 0) {
		exit(1);
	}
	
	for (p = res; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			continue;
		}
		
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			continue;
		}
		
		break;
	}
	
	char s[INET6_ADDRSTRLEN];
	inet_ntop(res->ai_family, (struct sockaddr *)res->ai_addr, s, sizeof s);
	freeaddrinfo(res);
	printf("%s\n", s);
	
	char buff[BUFF_SIZE];
	int recv_bytes;
	if((recv_bytes = recv(sockfd, buff, BUFF_SIZE - 1, 0)) == -1) {
		fprintf(stderr, "error on recv(): %s", strerror(errno));
		exit(1);
	};
	buff[recv_bytes] = '\0';
	printf("%s\n", buff);
	close(sockfd); // close connection
	
	return 0;
}
