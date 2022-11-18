# Cheatsheet

## Functions
See `functions.md` file.

## Structures
See `structures.md` file.

___

## INET_ADDRSTRLEN and INET6_ADDRSTRLEN
`INET_ADDRSTRLEN` is a macro that defines the length of an ipv4 address.  
`INET6_ADDRSTRLEN` is a macro that defines the length of an ipv6 address.  

Explainings:  
`INET_ADDRSTRLEN` is defined as 16 because it needs 15 bytes to represent an ipv4 address (255.255.255.255).  
`INET6_ADDRSTRLEN` is defined as 46 because with ipv4 tunneling the longest form can be 45 bytes (the standard is 39 bytes), for example: ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255.  
The last byte is the string null terminator \0.

___

## Errno
Example:

```
#include <string.h>
#include <errno.h>

if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
	fprintf(stderr, "Error: %s", strerror(errno));
	exit(EXIT_FAILURE);
}
```

___

## Server example steps
```
getaddrinfo();
socket();
bind();
listen();
accept();
```
