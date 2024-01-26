# ntohl
`ntohl` : Network TO host Long

+ ***Purpose*** : In network communications, data is typically transmitted in network byte order, which is Big-Endian. However, not all computer systems use Big-Endian as their native byte order; many use Little-Endian. `ntohs` is used to convert 32-bit numbers from network byte order to the host's native byte order.<br>
+ ***Function Prototype*** : `uint32_t ntohl(uint32_t netlong);`<br>
+ ***Usage*** : It is commonly used for converting 32-bit numerical values, such as IP addresses, received from a network to the host's byte order. For example, when a program receives an IP address from a network packet, `ntohl` is used to convert the address to the host system's byte order.

```cpp
#include <netinet/in.h>
#include <iostream>

int main() {
    uint32_t net_address = 0x12345678; // Example network order value
    uint32_t host_address = ntohl(net_address);

    std::cout << "Network order value: " << std::hex << net_address << std::endl;
    std::cout << "Host order value: " << std::hex << host_address << std::endl;

    return 0;
}

```
In this example, a 32-bit number in network byte order is converted to the host's byte order using `ntohl`. This conversion is essential in network programming when dealing with data that is sent and received over a network, ensuring that the numerical values are interpreted correctly on different systems.