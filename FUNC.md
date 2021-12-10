# Cheatsheet

## Function

```
htons() -> host to network short
htonl() -> host to netwokr long
ntohs() -> network to host short
ntohl() -> network to host long
```

## structs

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
}
```

`AF_INET` -> IPv4  
`AF_INET6` -> IPv6  
`AF_UNSPEC` -> use IPv4 or IPv6  
`SOCK_STREAM` -> TCP  
`SOCK_DGRAM` -> UDP  

```
struct sockaddr {
	unsigned short	sa_family; //AF_xxx
	char		sa_data[14];
}
```

To deal with `struct sockaddr` programmers created a `struct sockaddr_in` ("in" for "Internet") to be used with IPv4.
A pointer to `struct sockaddr_in` can be casted to `struct sockaddr`. So when we call `connect()` it wants a `struct sockaddr*`, so we cast it.
