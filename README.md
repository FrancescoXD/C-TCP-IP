# TPC/IP in C Notes

### References
* [Beej's Guide to Network Programming](http://www.beej.us/guide/bgnet/)
* TCP/IP Sockets in C: Practical Guide for Programmers (Morgan Kaufmann Practical Guides) 2nd Edition

## What's a communication channel?
A communication channel is a sequence of bytes from one host to another.

An information is a sequence of bytes that the program can interpretes, these bytes are generally called packets.

## What's a protocol?
A protocol is the list of rules to follow for knowing how a packet is structured.

## TCP/IP suite
The TCP/IP suite offers TCP (Transmission Control Protocol), UDP (User Datagram Protocol) and the IP (Internet Protocol). An application can access to the data provided by UDP and TCP using the Sockets API.

There are 2 loopback address:
- IPv4: 127.0.0.1
- IPv6: ::1

## What is a socket?
A socket is an abstraction through which an application may send and receive data. A socket allows an application to plug in to the network and communicate with other applications that are plugged in to the same network.

The main types of sockets in TCP/IP are stream sockets and datagram sockets.

A TCP/IP socket is identified by an IP, an end-to-end protocol (TCP or UDP) and a port.

## Basic TCP Sockets
Typical TCP client's communication:
- Create a TCP socket using socket()
- Establish a connection using connect()
- Communicate using send() and recv()
- Close the connection with close()
