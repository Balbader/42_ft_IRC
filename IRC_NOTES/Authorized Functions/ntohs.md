# ntohs
`ntohs` : Network TO Host Short

+ ***Purpose*** : In network communications, data is typically transmitted in network byte order, which is Big-Endian. However, not all computer systems use Big-Endian as their native byte order; many use Little-Endian. `ntohs` is used to convert 16-bit numbers from network byte order to the host's native byte order.<br>
+ ***Function Prototype*** : `uint16_t ntohs(uint16_t netshort)`<br>
+ ***Usage*** : Used for converting port numbers and other 16-bit values received from a network to the host's byte order. For example, when a program reads a prot number from a network packet, `ntohs` is used to convert the port number to a format that that host system can understand. <br>
```cpp
#include <netinet/in.h>
#include <iostream>

int main() {
    uint16_t net_port = 0x5000; // Example network order port number
    uint16_t host_port = ntohs(net_port);

    std::cout << "Network order port: " << std::hex << net_port << std::endl;
    std::cout << "Host order port: " << std::hex << host_port << std::endl;

    return 0;
}

```
In this example, a port number in network byte order is converted to the host's byte order using `ntohs`. This is crucial in scenarios where a program receives data from the network and needs to interpret numerical values correctly according to the host system's architecture.