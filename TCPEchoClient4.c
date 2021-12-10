#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
	char *servIP = argv[1];
	char *echoString = argv[2];

	in_port_t servPort = 7;

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
		fprintf(stderr, "socket() failed");

	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;

	int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
	if (rtnVal == 0)
		fprintf(stderr, "inet_pton() failed");
	servAddr.sin_port = htons(servPort);

	if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
		fprintf(stderr, "connect() failed");

	size_t echoStringLen = strlen(echoString);
	ssize_t numBytes = send(sock, echoString, echoStringLen, 0);
	if (numBytes < 0)
		fprintf(stderr, "send() failed");
	else if (numBytes != echoStringLen)
		fprintf(stderr, "send() failed");

	unsigned totalBytesRcvd = 0;
	fputs("Received: ", stdout);
	while (totalBytesRcvd < echoStringLen) {
		char buffer[1024];
		numBytes = recv(sock, buffer, 1023, 0);
		if (numBytes < 0)
			fprintf(stderr, "recv() failed");
		else if (numBytes == 0)
			fprintf(stderr, "recv() failed");
		totalBytesRcvd += numBytes;
		buffer[numBytes] = '\0';
		fputs(buffer, stdout);
	}

	fputc('\n', stdout);
	close(sock);
	exit(0);
}
