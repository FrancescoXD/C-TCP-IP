# Non blocking sockets
If we want to handle multiple file descriptors we have to use `poll` or `select` or `epoll` (preferred) or `io_uring`, they wait for file descriptors to be ready to perform I/O.
The first one is superior because we don't have a fixed limit of fds and is more precise to detect status code but is not portable (an old system that doesn't use poll is full of vulnerabilities).  
Sorted by performance:
- io_uring
- [epoll](#epoll)
- [poll](#poll)
- select

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

## epoll
```
#include <sys/epoll.h>

typedef union epoll_data {
	void* ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;

struct epoll_event {
	uint32_t events;
	epoll_data_t data;
}
// events examples:
// - EPOLLIN (is available for read())
// - EPOLLOUT (is available for write())
// - EPOLLET (requests edge-triggered notification)

// create a epoll file descriptor
int epoll_create1(int flags);
// control interface for an epoll file descriptor 
int epoll_ctl(int epollfd, int operation, int fd, struct epoll_event* event);
// wait for I/O operations
// return the number of fds ready to do a specific event
int epoll_wait(int epollfd, struct epoll_event* events, int maxevents, int timeout);
```
`int flags` can be set to 0.
`int epollfd` is the epoll file descriptor returned by `epoll_create1()`.
`int operation` is the operation to do like:
- EPOLL_CTL_ADD (add a new file descriptor to the list of events)
- EPOLL_CTL_DEL (remove a file descriptor from the list)
- EPOLL_CTL_MOD (modify a setting associated with a file descriptor)
`int fd` is the file descriptor to add to the list of monitored fds.
`struct epoll_event* event` describes the object linked to the file descriptor.
`struct epoll_event* events` is the vector of events that returns with the fds ready to do some I/O operation.
`int maxevents` is the length of the events array.
`int timeout` is the number of milliseconds that the function will block. The call will block until:
- one file descriptor delivers an event
- the call is interrupted by a signal
- timeout expires

### How it works
We pass a vector of `maxevents` elements that will receive the events.
Example:
If we have 1000 fds and 1 fd is ready `epoll_wait()` will return 1 and events[0] will contain the event. That's why it is more performant than the others.
