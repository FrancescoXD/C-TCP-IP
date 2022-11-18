# Index
- [Convert values](#convert-values-between-host-and-network-byte-order)
- [IP Addresses](#ip-addresses)
- [getaddrinfo()](#getaddrinfo)
- [inet_ntop](#inet_ntop)
- [socket](#socket)
- [bind](#bind)
- [connect](#connect)
- [accept](#accept)
- [send](#send)
- [recv](#recv)

## Convert values between host and network byte order
```
#include <arpa/inet.h>

htons() -> host to network short
htonl() -> host to network long
ntohs() -> network to host short
ntohl() -> network to host long
```

___

## IP Addresses
We can use `inet_pton()` that converts an IP address from text to binary form.

```
struct sockaddr_in sa;
struct sockaddr_in6 sa6;

inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr));				//IPv4
inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr));	//IPv6
```

`inet_pton()` returns 1 on success, 0 when src doesn't contain a 
valid network address and -1 when it fails.

____

## getaddrinfo()
This function helps to setup the structs we need.

```
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char* node,
		const char* service,
		const struct addrinfo* hints,
		struct addrinfo** res);
```

`const char* node` is the hostname or the IP address.  
`const char* service` is the service (which can be a port like "80").  
`const struct addrinfo* hints` is a pointer to our struct with the needed informations.  
`struct addrinfo** res` is a pointer to a linked-list of results.

Example:
```
int status;
struct addrinfo hints;
struct addrinfo *results;			// struct with results

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;		// we don't care about IPv4 or IPv6
hints.ai_socktype = SOCK_STREAM;	// tcp
hints.ai_flags = AI_PASSIVE;		// use my ip

if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status));
	exit(EXIT_FAILURE);
}

freeaddrinfo(servinfo);				// free the linked list
```

We can use an hardcoded IP instead of NULL and we can avoid also the `AI_PASSIVE`.

___

## inet_ntop()
This function converts a network address struct to a string.

```
#include <arpa/inet.h>

const char* inet_ntop(int af,
			const void* src,
			char* dst,
			socklen_t size);
```

`int af` is the family (AF_INET, AF_INET6) that we get from the struct.  
`const void* src` is the network address structure.  
`char* dst` is the destination where to write the ip.  
`socklen_t size` is the sizeof the dst.

___

## socket()
This functions returns a socket descriptor (-1 on error).

```
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain,
		int type,
		int protocol);
```
`int domain` is the domain, like: `PF_INET` or `PF_INET6`. PF stands for Protocol Family. It's related to `sin_family` and sometimes we can see that developers use `AF_INET`, but it's better PF.  
`int type` is `SOCK_STREAM` or `SOCK_DGRAM`.  
`int protocol` we can set it to 0 and the protocol is choosen looking to the type.

Example:
```
int sock, status;
struct addrinfo* res, hints;

if ((status = getaddrinfo("google.com", "http", &hints, &res)) != 0) {
	fprintf(stderr, "error while getting getaddrinfo()");
	exit(EXIT_FAILURE);
}

sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```

___

## bind()
This functions bind a socket on a port. Returns -1 on error.

```
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd,
	struct sockaddr* my_addr,
	int addrlen);
```
`int sockfd` is the socket file descriptor made by `socket()`.  
`struct sockaddr* my_addr` is the struct that contains information about our address.  
`int addrlen` is the length of address in bytes.

Example:
```
struct sockaddr hints = {
	.ai_family = AF_UNSPEC,
	.ai_socktype = SOCK_STREAM,
	.ai_flags = AI_PASSIVE
};
struct sockaddr* res;
int sockfd, status;

if ((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
	fprintf(stderr, "error in getaddrinfo()");
	exit(EXIT_FAILURE);
}

sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
bind(sockfd, res->ai_addr, res->ai_addrlen);
```

___

## connect()
This function connects to a server.

```
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd,
		struct sockaddr* serv_addr,
		int addrlen);
```

`int sockfd` is the socket file descriptor made by `socket()`.  
`struct sockaddr* serv_addr` is the struct that contains the destination port and ip.  
`int addrlen` is the length in bytes of the server address structure.

Example:
```
struct addrinfo hints = {
	.ai_family = AF_UNSPEC,
	.ai_socktype = SOCK_STREAM
}, *res;
int sockfd;

getaddrinfo("server.com", "80", &hints, &res);
sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
connect(sockfd, res->ai_addr, res->ai_addrlen);
```

___

## listen()
This function listens on a port for incoming connections. It returns -1 on error and we can use `errno` to know what error is.

```
int listen(int sockfd, int backlog);
```
`int sockfd` is the socket file descriptor.  
`int backlog` is the number of connection allowed on the incoming queue.

___

## accept()
This function accepts an incoming connection. It returns a new socket file descriptor to use for this single connection. The original `sockfd` is still listening for more new connections and the `new_fd` is ready to `send()` and `recv()`. It returns -1 on error.

```
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd,
		struct sockaddr* addr,
		socklen_t* addrlen);
```
`int sockfd` is the socket file descriptor.  
`struct sockaddr* addr` is a pointer to a `struct sockaddr_storage` that contains the incoming connection info.  
`socklen_t* addrlen` is a variable set to `sizeof (struct sockaddr_storage)`.

Example:
```
#define BACKLOG 20

getaddrinfo(NULL, MYPORT, &hints, &res);
sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
bind(sockfd, res->ai_addr, res->ai_addrlen);
listen(sockfd, BACKLOG);

// now accept an incoming connection
struct sockaddr_storage their_addr;
int new_fd;
socklen_t addr_size = sizeof their_addr;
new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size);
```

___

## send()
This function sends data to client. It returns -1 on error.

```
int send(int sockfd,
	const void* data,
	int len,
	int flags);
```
`int sockfd` is the socket file descriptor.  
`const void* data` is the data to send.  
`int len` is the length of the message.  
`int flags` just set to 0 (`man send()` for more info about flags).

Example:
```
char msg[] = "Hello!";
int len = strlen(msg);
int bytes_sent = send(new_fd, msg, len, 0);
```

___

## recv()
This function receives data from the server. It returns -1 on error.

```
int recv(int sockfd,
	void* data,
	int len,
	int flags);
```
`int sockfd` I don't want to write what this is.  
`void* data` this is the buffer, like a string.  
`int len` is the buffer length.  
`int flags` just set to 0.

Example:
```
#define BUFFER 512
char buf[BUFFER];
int recv_bytes = recv(sockfd, buf, BUFFER, 0);
buf[recv_bytes] = '\0'; // null terminator
```
