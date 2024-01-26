# poll
`poll` is another system call in C++ (and C) used for monitoring multiple file descriptors, similar to `select`. It's part of the POSIX standard, making it available on Unix-like operating systems including Linux and macOS, and on Windows through compatibility layers. `poll` provides a way to wait for one of a set of file descriptors to become ready for I/O operations like reading or writing.

Here's an overview of `poll` and how it works:

1. **File Descriptors**: As with `select`, `poll` is used to monitor file descriptors, which can represent files, sockets, pipes, and other I/O channels.
    
2. **Function Prototype**:

```cpp
int poll(struct polfd *fds, nfds_t nfds, int timeout);
```

- `fds`: An array of `pollfd` structures, each representing a file descriptor and the events you're interested in.
- `nfds`: The number of items in the `fds` array.
- `timeout`: Time in milliseconds to wait, or -1 to wait indefinitely.

3. **Struct pollfd**: This structure is used to specify the file descriptors and events you're interested in monitoring:

```cpp
struct pollfd {
	int fd; // file descriptor
	short events; // Events to watch for
	short revents; // Events that occured
}
```

- `fd`: The file descriptor.
- `events`: Bitmask specifying the events to watch (e.g., `POLLIN` for readable, `POLLOUT` for writable).
- `revents`: Bitmask indicating the events that actually occurred.

4. **Behavior**: The `poll` function monitors the file descriptors in the `fds` array for the specified events. It blocks until one of the events occurs, the call is interrupted by a signal, or the timeout elapses.

5. **Use Cases**: `poll` is commonly used in applications that need to monitor multiple file descriptors, especially when the number of descriptors is dynamic or potentially large. It's often used in network programming, for instance, in servers that need to handle many simultaneous connections.

6. **Advantages over `select`**:

- Scalability: `poll` can handle more than `FD_SETSIZE` file descriptors.
- Efficiency: The `fds` array does not need to be re-initialized on every call, unlike the `fd_set` with `select`.
- Clearer Semantics: The use of a dedicated structure for each file descriptor makes code more readable and less prone to errors like exceeding `FD_SETSIZE`.

However, `poll` is still subject to some limitations, such as linear scaling with the number of file descriptors. For even higher efficiency, especially with a large number of connections, system calls like `epoll` (Linux-specific) or `kqueue` (BSD-specific) are often preferred in modern applications.