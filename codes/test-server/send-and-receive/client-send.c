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

char *findchr(char *str, char ch) {
	while (*str && *str != ch) str++;
	return str;
}

int main(void) {
	int status, sockfd;
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};
	struct addrinfo *res;
	
	if ((status = getaddrinfo(NULL, "3939", &hints, &res)) != 0) {
		fprintf(stderr, "error on bind(): %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		fprintf(stderr, "error on bind(): %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
		
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		fprintf(stderr, "error on bind(): %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	char s[INET6_ADDRSTRLEN];
	inet_ntop(res->ai_family, (struct sockaddr *)res->ai_addr, s, sizeof s);
	freeaddrinfo(res);
	printf("%s\n", s);
	
	char buff[BUFF_SIZE];
	int recv_bytes;
	if((recv_bytes = recv(sockfd, buff, BUFF_SIZE - 1, 0)) == -1) {
		fprintf(stderr, "error on recv(): %s\n", strerror(errno));
		exit(1);
	};
	buff[recv_bytes] = '\0';
	printf("%s\n", buff);
	
	// send data to the server
	printf("Send some data to the server: ");
	fgets(buff, BUFF_SIZE, stdin);
	char *nl = findchr(buff, '\n');
	if (*nl == '\n') *nl = 0;
	send(sockfd, buff, strlen(buff), 0);
	
	close(sockfd); // close connection
	
	return 0;
}
