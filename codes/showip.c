#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	struct addrinfo hints;		// our struct with needed infos
	struct addrinfo *res;		// linked-list result
	struct addrinfo *p;		// our iterate variable
	int status;			// getaddrinfo() result
	void *addr;			// the result of the ip
	char *ipver;			// ip version string
	char ipstr[INET6_ADDRSTRLEN];	// we need a string to contain our ipv6 address
	// INET_ADDRSTRLEN is defined as 16 because it needs 15 bytes to represent an ipv4 address (255.255.255.255)
	// INET6_ADDRSTRLEN is defined as 46 because with ipv4 tunneling the longest form can be 45 bytes (the standard is 39 bytes), for example: ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255
	// the last byte is the string null terminator \0

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo("google.com", NULL, &hints, &res)) != 0) { // if we want we can use argv[1]
		fprintf(stderr, "error while getting address info");
		exit(1);
	}

	for (p = res; p != NULL; p = p->ai_next) { // iterate the linked-list for all ip found
		if (p->ai_family == AF_INET) { // if our address family is IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr; // cast the struct to ipv4 struct
			addr = &(ipv4->sin_addr); // save the ip
			ipver = "IPv4";
		} else {
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr); // converts a network address structure to a string
		// gets the ai_family (ipv4 or ipv6)
		// gets the network address structure
		// gets the string where store the ip in character string
		// gets the size of string
		printf("%s: %s\n", ipver, ipstr); // finally prints the ip
	}

	freeaddrinfo(res); // free the linked-list

	return 0;
}
