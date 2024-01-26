# htons
`htons` : Host TO Network Short

+ ***Purpose*** : It is used to convert 16-bit integer from the host's native byte order to network byte order. Network protocols typically require data to be transmitted in a consistent byte order, known as network byte order, which is Big-Endian. However, not all systems use Big-Endian byte order internally (many use Little-Endian).
+  ***Use Case*** : It is commonly used for port numbers in TCP/IP network programming. For example when specifying a port number for a socket, you should use  `htons` to ensure the port number is in the correct byte orderi.
+ ***Function Prototype*** : `uint16_t htons(uint16_t hostshort);`

```cpp
#include <netinet/in.h>
#include <iostream>

int main() {
    uint16_t host_port = 8080;
    uint16_t net_port = htons(host_port);

    std::cout << "Host order port: " << host_port << std::endl;
    std::cout << "Network order port: " << net_port << std::endl;

    return 0;
}

```
In this example, a port number `8080` is converted from host byte order to network byte order using `htons`. This is essential in network programming where you are dealing with low-level network details, such as creating and binding sockets.