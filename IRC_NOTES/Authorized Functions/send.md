# send
In C++ network programming, the `send` function is used to transmit data over a network connection, particularly in the context of a TCP (Transmission Control Protocol) socket. This function is part of the sockets API provided by the operating system and is used to send data to a connected remote host.

Here's a basic overview of how the `send` function is used:

1. **Establishing a Connection**: Before you can use `send`, a network connection needs to be established. For a TCP connection, this typically involves creating a socket, connecting it to a remote address (using `connect` for a client or `accept` for a server), and then using `send` to transmit data over this connection.

2. **Function Prototype**: The `send` function in C++ (and C) typically has the following prototype:
```cpp
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

- `sockfd`: The socket file descriptor that represents the connection.
- `buf`: A pointer to the buffer containing the data to be sent.
- `len`: The length of the data in bytes.
- `flags`: A set of bit flags that modify the function's behavior. Common flags include `MSG_OOB` for out-of-band data and `MSG_NOSIGNAL` to prevent `SIGPIPE` signals on errors. Usually set to 0 for normal operation.

3. **Sending Data**:
- When you call `send`, the data pointed to by `buf` of length `len` is queued for transmission over the socket.
- The function returns the number of bytes actually sent, which can be less than the number specified in `len`. This partial send can happen for various reasons, such as limited network buffer space.
- In blocking mode (the default mode for sockets), `send` will block until it can send some data. In non-blocking mode, `send` may return immediately if it can't send data right away, and it's the programmer's responsibility to handle this appropriately (often by retrying later).

4. **Error Handling**:
- `send` returns -1 on error, and the specific error can be determined by checking `errno`.
- Common errors include `ECONNRESET` (connection reset by peer), `EPIPE` (broken pipe), and `EAGAIN` or `EWOULDBLOCK` (in non-blocking mode, indicates the data can't be sent immediately).

5. **TCP Specifics**:
- In TCP, `send` guarantees that the data will be delivered in order and without duplication, as long as the connection remains intact.
- If the network connection is closed or broken while `send` is in progress, the function will return an error.

6. **Use in Network Programming**:
- `send` is a fundamental function for TCP-based network communication in server and client applications, including web servers, chat applications, and other networked software.

It's important to note that `send` is a low-level function and handling its intricacies correctly can be non-trivial. In many cases, especially in C++, higher-level networking libraries or frameworks are used to abstract these details, providing easier and safer interfaces for network communication.