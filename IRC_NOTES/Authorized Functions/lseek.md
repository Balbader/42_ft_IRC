# lseek
`lseek` is a system call in Unix and Unix-like operating systems, including Linux, used for repositioning the file offset of the file descriptor. In C and C++ programming, `lseek` allows you to control the location in a file from which the next read or write operation will occur. It's part of the POSIX standard and is commonly used in low-level file manipulation.

### Function Signature:

```cpp
#include <unistd.h>
off_t lseek(int fd, off_t offset, int whence);

``` 
- **`fd`**: The file descriptor of the file to be manipulated.
- **`offset`**: The new position of the file offset, relative to the position specified by `whence`.
- **`whence`**: This specifies the reference point for setting the new file offset and can be one of the following:
    - `SEEK_SET`: Set the offset to the specified number of bytes from the beginning of the file.
    - `SEEK_CUR`: Set the offset to its current location plus the specified number of bytes.
    - `SEEK_END`: Set the offset to the size of the file plus the specified number of bytes.

### Return Value:

- On success, `lseek` returns the resulting offset location as measured in bytes from the beginning of the file.
- On error, the value `(off_t) -1` is returned and `errno` is set appropriately.

### Usage:

`lseek` is used when you need more control over file reading and writing than what high-level functions like `fread` and `fwrite` offer. It's particularly useful for accessing specific parts of large files, implementing file formats that require random access, or working with device files that require positioning commands.

### Example in C++:
```cpp
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

int main() {
    // Open a file
    int fd = open("example.txt", O_RDWR);
    if (fd < 0) {
        std::cerr << "Error opening file: " << strerror(errno) << std::endl;
        return 1;
    }

    // Move the file offset 10 bytes from the start of the file
    if (lseek(fd, 10, SEEK_SET) < 0) {
        std::cerr << "Error seeking in file: " << strerror(errno) << std::endl;
        close(fd);
        return 1;
    }

    // Perform read/write operations...

    // Close the file
    close(fd);
    return 0;
}

```
In this example, `lseek` is used to move the file offset 10 bytes from the beginning of the file. After calling `lseek`, any subsequent read or write operation on the file descriptor `fd` will start from this new location.

Remember that using `lseek` and similar low-level functions requires a good understanding of file descriptors and manual memory management, as these are lower-level operations than those provided by the C++ Standard Library's file streams.