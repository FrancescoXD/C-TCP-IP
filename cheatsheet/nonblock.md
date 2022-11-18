# Non blocking sockets
If we want to handle multiple file descriptors we have to use `poll` or `select`, they wait for file descriptors to be ready to perform I/O.
The first one is superior because we don't have a fixed limit of fds and is more precise to detect status code but is not portable (an old system that doesn't use poll is full of vulnerabilities).

## poll
`poll()` checks every fd in the fds array waiting events.

```
#include <poll.h>
int poll(struct pollfd* fds, nfds_t nfds, int timeout);

struct pollfd {
	int		fd;			// file descriptor
	short 	events;		// requested events
	short	revents;	// returned events
}
```
`int timeout` is the number of milliseconds that `poll()` should block waiting for a file descriptor to become ready, if that time elapses it returns 0.
`short events` are the events to wait:
- POLLIN (Data is ready to be read)
- POLLOUT (Writing is possible, pipe is blocking unless O_NONBLOCK is set)

To set O_NONBLOCK we have to use `fcntl()`:
```
#include <fcntl.h>

int fnctl(int fd, int cmd, ... args ...);
```

Example:
```
fcntl(file_descriptor, F_SETFL, O_NONBLOCK);
```
