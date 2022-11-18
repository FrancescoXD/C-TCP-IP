# Index
- [struct addrinfo](#struct-addrinfo)
- [struct sockaddr_in](#struct-sockaddr_in)
- [struct sockaddr_in6](#struct-sockaddr_in6)
- [struct sockaddr_storage](#struct-sockaddr_storage)

___

## struct addrinfo
```
struct addrinfo {
	int				ai_flags;		//AI_PASSIVE, AI_CANONNAME
	int				ai_family;		//AF_INET, AF_INET6, AF_UNSPEC
	int				ai_socktype;	//SOCK_STREAM, SOCK_DGRAM
	int				ai_protocol;	//0 for "any"
	size_t			ai_addrlen;		//size of ai_addr in bytes
	struct sockaddr	*ai_addr;		//sockaddr_in or sockaddr_in6
	char			*ai_canonname;	//full canonical hostname

	struct addrinfo *ai_next;		//linked list to next node
};

`AF_INET`		-> IPv4  
`AF_INET6`		-> IPv6  
`AF_UNSPEC`		-> use IPv4 or IPv6  
`SOCK_STREAM`	-> TCP  
`SOCK_DGRAM`	-> UDP  

struct sockaddr {
	unsigned short	sa_family;	//AF_xxx
	char			sa_data[14];
};
```

___

## struct sockaddr_in
To deal with `struct sockaddr` programmers created a `struct sockaddr_in` ("in" for "Internet") to be used with IPv4.  
A pointer to `struct sockaddr_in` can be casted to `struct sockaddr`. When we call `connect()` it wants a `struct sockaddr*`, we have to cast it.

```
struct sockaddr_in {
	short int			sin_family
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
	.ai_family		= AF_UNSPEC,
	.ai_socktype	= SOCK_STREAM,
	.ai_flags		= AI_PASSIVE
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

## struct sockaddr_in6

```
struct sockaddr_in6 {
	u_int16_t		sin6_family;
	u_int16_t		sin6_port;
	u_int16_t		sin6_flowinfo;
	struct in6_addr	sin6_addr;
	u_int32_t		sin6_scope_id;
};

struct in6_addr {
	unsigned char	s6_addr[16];
};
```

___

## struct sockaddr_storage

There is also a `struct sockaddr_storage` that hold both IPv4 and IPv6 structures.
```
struct sockaddr_storage {
	sa_family_t	ss_family;
	char		__ss_pad1[_SS_PAD1SIZE];
	int64_t		__ss_align;
	char		__ss_pad2[_SS_PAD2SIZE];
};
```
