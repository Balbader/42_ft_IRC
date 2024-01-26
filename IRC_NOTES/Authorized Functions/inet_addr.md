# inet_addr
In C++ network programming, specifically when dealing with sockets, `inet_addr` is a function used to convert an IPv4 address in the dot-decimal notation (like "192.168.1.1") into its binary representation in network byte order. It's part of the POSIX sockets API and is defined in the `<arpa/inet.h>` header file on Unix-like systems.

The function signature of `inet_addr` is as follows:o
```cpp
#include <arpa/inet.h>

in_addr_t inet_addr(const char *cp);

```
- **`cp`**: A C-string containing the IPv4 address in dot-decimal notation.
- **`return`**: The function returns the address in network byte order. If the address is invalid, the function returns `INADDR_NONE`.
### Usage:

`inet_addr` is commonly used when setting up socket addresses for network communication, especially when creating a socket that needs to connect to a specific server, or when binding a socket to a specific local address.

### Example:

Here's an example of how `inet_addr` might be used in a simple program:o
```cpp
#include <iostream>
#include <arpa/inet.h>

int main() {
    const char* ip_address = "192.168.1.100";
    in_addr_t binary_address = inet_addr(ip_address);

    if (binary_address == INADDR_NONE) {
        std::cerr << "Invalid IP address format" << std::endl;
    } else {
        std::cout << "The binary representation of " << ip_address 
                  << " is " << binary_address << std::endl;
    }

    return 0;
}

```
In this example:

- The `inet_addr` function is used to convert the string `"192.168.1.100"` into its binary representation.
- It checks for `INADDR_NONE` to verify that the conversion was successful.

### Important Notes:

- **IPv4 Only**: `inet_addr` works only with IPv4 addresses. For IPv6, a different set of functions (`inet_pton`, `inet_ntop`) are used.
- **Deprecation**: The `inet_addr` function is considered deprecated in modern networking code in favor of `inet_pton` (presentation to network), which supports both IPv4 and IPv6 and has better error handling.
- **Error Handling**: `inet_addr` cannot distinguish between the invalid address "255.255.255.255" and `INADDR_NONE`, which can lead to potential errors. This is another reason `inet_pton` is preferred in new code.