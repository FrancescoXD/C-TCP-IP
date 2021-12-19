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

## > getaddrinfo()

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
hints.ai_flags = AF_PASSIVE; // use my ip

if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status));
	exit(1);
}

freeaddrinfo(servinfo); // free the linked list
```

We can use an hardcoded IP instead of NULL and we can avoid also the `AF_PASSIVE`;

___

### > inet_ntop()

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
