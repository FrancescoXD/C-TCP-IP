# Cheatsheet

## Function

```
htons() -> host to network short
htonl() -> host to netwokr long
ntohs() -> network to host short
ntohl() -> network to host long
```

## Structures

### > struct addrinfo

```
struct addrinfo {
	int		ai_flags; //AI_PASSIVE, AI_CANONNAME
	int		ai_family; //AF_INET, AF_INET6, AF_UNSPEC
	int		ai_socktype; //SOCK_STREAM, SOCK_DGRAM
	int		ai_protocol; //0 for "any"
	size_t		ai_addrlen; //size of ai_addr in bytes
	struct sockaddr	*ai_addr; //sockaddr_in or sockaddr_in6
	char		*ai_canonname; //full canonical hostname

	struct addrinfo *ai_next; //linked list to next node
};
```

`AF_INET`	-> IPv4  
`AF_INET6`	-> IPv6  
`AF_UNSPEC`	-> use IPv4 or IPv6  
`SOCK_STREAM`	-> TCP  
`SOCK_DGRAM`	-> UDP  

```
struct sockaddr {
	unsigned short	sa_family; //AF_xxx
	char		sa_data[14];
};
```
___

### > struct sockaddr_in

To deal with `struct sockaddr` programmers created a `struct sockaddr_in` ("in" for "Internet") to be used with IPv4.  
A pointer to `struct sockaddr_in` can be casted to `struct sockaddr`. So when we call `connect()` it wants a `struct sockaddr *`, so we cast it.

```
struct sockaddr_in {
	short int		sin_family
	unsigned short int	sin_port
	struct in_addr		sin_addr
	unsigned char		sin_zero[8];
};
```

This structure make easy to reference elements of the socket address. `sin_zero` should be set to zero with `memset()`. Notice that `sin_family` corresponds to `sa_family` in `struct sockaddr` and should be set to `AF_INET`. Finally the `sin_port` must be `Network Byte Order` using `htons()` (host to network short).

EDIT on `memset()`: https://www.anmolsarma.in/post/stop-struct-memset/  
Best way to initialize a struct:

```
struct addrinfo hints = {
	.ai_family	= AF_UNSPEC,
	.ai_socktype 	= SOCK_STREAM,
	.ai_flags 	= AI_PASSIVE //use my IP
};
```

Note that the `sin_addr` refers to an union.

```
struct in_addr {
	uint32_t	s_addr;
};
```

This stands for Internet Address, and is a structure for historical reasons.

___

What's about IPv6? There is `sockaddr_in6`:

```
struct sockaddr_in6 {
	u_int16_t	sin6_family; //address
	u_int16_t	sin6_port;
	u_int16_t	sin6_flowinfo;
	struct in6_addr	sin6_addr;
	u_int32_t	sin6_scope_id;
};

struct in6_addr {
	unsigned char	s6_addr[16]; //IPv6 address
};
```

There is also a `struct sockaddr_storage` that hold both IPv4 and IPv6 structures.

```
struct sockaddr_storage {
	sa_family_t	ss_family;
	char		__ss_pad1[_SS_PAD1SIZE];
	int64_t		__ss_align;
	char		__ss_pad2[_SS_PAD2SIZE];
};
```

___

## IP Addresses
We can use `inet_pton()` that converts an IP address from text to binary form.

```
struct sockaddr_in sa;
struct sockaddr_in6 sa6;

inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr)); //IPv4
inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr)); //IPv6
```

`inet_pton()` returns 1 on success, 0 when src doesn't contain a 
valid network address and -1 when it fails.

___

## > getaddrinfo() | Server & Client

This function helps to setup the structs we need.

```
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,
		const char *service,
		const struct addrinfo *hints,
		struct addrinfo **res);
```

`const char *node` is the hostname or an IP address.  
`const char *service` is the service (which can be a port like "80").  
`const struct addrinfo *hints` is a pointer to our struct with the needed informations.  
And finally the the `struct addrinfo **res` is a pointer to a linked-list of results.

Example:

```
int status;
struct addrinfo hints;
struct addrinfo *results; // struct with results

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC; // we don't care about IPv4 or IPv6
hints.ai_socktype = SOCK_STREAM; // tcp
hints.ai_flags = AI_PASSIVE; // use my ip

if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status));
	exit(1);
}

freeaddrinfo(servinfo); // free the linked list
```

We can use an hardcoded IP instead of NULL and we can avoid also the `AI_PASSIVE`;

___

### > inet_ntop() | Server & Client

This function converts a network address struct to a string.

```
#include <arpa/inet.h>

const char *inet_ntop(int af,
			const void *src,
			char *dst,
			socklen_t size);
```

`int af` is the family (AF_INET, AF_INET6) that we get from the struct.  
`const void *src` is the network address structure.  
`char *dst` is the destination where to write the ip.  
`socklen_t size` is the sizeof the dst.

___

## INET_ADDRSTRLEN and INET6_ADDRSTRLEN

`INET_ADDRSTRLEN` is a macro that defines the length of an ipv4 address.  
`INET6_ADDRSTRLEN` is a macro that defines the length of an ipv6 address.  

Explainings:  
`INET_ADDRSTRLEN` is defined as 16 because it needs 15 bytes to represent an ipv4 address (255.255.255.255).  
`INET6_ADDRSTRLEN` is defined as 46 because with ipv4 tunneling the longest form can be 45 bytes (the standard is 39 bytes), for example: ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255.  
The last byte is the string null terminator \0.

___

## > socket() | Server & Client

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
struct addrinfo *res, hints;

if ((status = getaddrinfo("google.com", "http", &hints, &res)) != 0) {
	fprintf(stderr, "error while getting getaddrinfo()");
	exit(1);
}

sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```

___

## > bind() | Server

This functions bind a socket on a port. Returns -1 on error.

```
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd,
	struct sockaddr *my_addr,
	int addrlen);
```

`int sockfd` is the socket file descriptor made by `socket()`.  
`struct sockaddr *my_addr` is the struct that contains information about our address.  
`int addrlen` is the length of address in bytes.

Example:

```
struct sockaddr hints = {
	.ai_family = AF_UNSPEC,
	.ai_socktype = SOCK_STREAM,
	.ai_flags = AI_PASSIVE
};
struct sockaddr *res;
int sockfd, status;

if ((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
	fprintf(stderr, "error in getaddrinfo()");
	exit(1);
}

sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

bind(sockfd, res->ai_addr, res->ai_addrlen);
```

___

## > connect() | Client

This function connects to a server.

```
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd,
		struct sockaddr *serv_addr,
		int addrlen);
```

`int sockfd` is the socket file descriptor made by `socket()`.  
`struct sockaddr *serv_addr` is the struct that contains the destination port and ip.  
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

## > listen() | Server

This function listens on a port for incoming connections. It returns -1 on error and we can use `errno` to know what error is.

```
int listen(int sockfd, int backlog);
```

`int sockfd` is the socket file descriptor.  
`int backlog` is the number of connection allowed on the incoming queue.

### Errno

Example:

```
#include <string.h>
#include <errno.h>

if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
	fprintf(stderr, "Error: %s", strerror(errno));
	exit(1);
}
```

### Server example steps

```
getaddrinfo();
socket();
bind();
listen();
accept();
```

___

## > accept() | Server

This function accepts an incoming connection. It returns a new socket file descriptor to use for this single connection. The original `sockfd` is still listening for more new connections and the `new_fd` is ready to `send()` and `recv()`. It returns -1 on error.

```
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd,
		struct sockaddr *addr,
		socklen_t *addrlen);
```

`int sockfd` is the socket file descriptor.  
`struct sockaddr *addr` is a pointer to a `struct sockaddr_storage` that contains the incoming connection info.  
`socklen_t *addrlen` is a variable set to `sizeof (struct sockaddr_storage)`.

Example:

```
...
getaddrinfo(NULL, MYPORT, &hints, &res);
sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
bind(sockfd, res->ai_addr, res->ai_addrlen);
listen(sockfd, BACKLOG);

// now accept an incoming connection
struct sockaddr_storage their_addr;
int new_fd;
socklen_t addr_size = sizeof their_addr;
new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
```

___

## > send() | Server & Client

This function sends data to client. It returns -1 on error.

```
int send(int sockfd,
	const void *msg,
	int len,
	int flags);
```

`int sockfd` is the socket file descriptor.  
`const void *msg` is the message to send.  
`int len` is the length of the message.  
`int flags` just set to 0 (`man send()` for more info about flags).

Example:

```
char *msg = "Hello!";
int len = strlen(msg);
int bytes_sent = send(new_fd, msg, len, 0);
```

___

## > recv() | Client & Client

This function receives data from the server. It returns -1 on error.

```
int recv(int sockfd,
	void *buf,
	int len,
	int flags);
```

`int sockfd` I don't want to write what this is.  
`void *buf` this is the buffer, like a string.  
`int len` is the buffer length.  
`int flags` just set to 0.

Example:

```
#define BUFFER 512
char buf[BUFFER];
int recv_bytes = recv(sockfd, buf, BUFFER - 1, 0);
buf[recv_bytes] = '\0'; // null terminator
```
