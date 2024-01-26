# freeaddrinfo
In C++ and C network programming, `freeaddrinfo` is a system call used to free the memory allocated for the linked list of `addrinfo` structures, which is typically created by the `getaddrinfo` function.

When you use `getaddrinfo` to perform network address translation (like resolving a hostname to an IP address), it dynamically allocates memory for a linked list of `addrinfo` structures. Each of these structures contains information that can be used to create and configure sockets for network communication. Once you're done using this information, it's important to release the allocated memory to prevent memory leaks. This is where `freeaddrinfo` comes in.

Here's the basic usage:

1. **Call `getaddrinfo`**: First, you call `getaddrinfo` to obtain network address information. `getaddrinfo` returns a pointer to the first node in a linked list of `addrinfo` structures.
```cpp
struct addrinfo hints, *res;
// set up hints...
getaddrinfo("www.example.com", "http", &hints, &res);
```

2. **Use the Address Information**: You can then iterate over this list and use the address information to create and configure sockets.

3. **Free the Memory with `freeaddrinfo`**: After you're done using the address information, you call `freeaddrinfo` to free the entire linked list of `addrinfo` structures.
```cpp
freeaddrinfo(res);
```

- `res`: A pointer to the first node in the linked list of `addrinfo` structures returned by `getaddrinfo`.

`freeaddrinfo` is crucial for resource management in network programming to ensure that there are no memory leaks in applications that make repeated or numerous calls to `getaddrinfo`.