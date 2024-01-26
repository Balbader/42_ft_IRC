# htonl
`htonl` : Host TO Network Long
It is used to ensure that integer values are represented int he correct byte order (endian format) when transmitted over a network.

+ ***Purpose*** : to convert a 32-bit integer from the host's byte order to network byte order
+ ***Function Prototype*** : `uint32_t htonl(uint32_t hostlong);` 
+ ***Usage*** : It is commonly used for converting IP addresses in a program, it's necessary to use `htonl` to convert the address to network byte order.

```cpp
#include <netinet/in.h>
#include <iostream>

int main() {
    uint32_t host_address = 0xC0A80001; // Example IP address 192.168.0.1
    uint32_t net_address = htonl(host_address);

    std::cout << "Host order address: " << std::hex << host_address << std::endl;
    std::cout << "Network order address: " << std::hex << net_address << std::endl;

    return 0;
}

```
In this example, an IP address (represented as a 32-bit integer) is converted from host byte order to network byte order using `htonl`. This conversion is crucial in network programming where precise control over the format of data being sent over the network is required.