# fstat
  
In C and C++ programming on Unix and Unix-like systems, `fstat` is a system call used to obtain information about a file based on its file descriptor. It's part of the POSIX (Portable Operating System Interface) standard and is commonly used in low-level file manipulation.
### Function Signature:
```cpp
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int fstat(int fd, struct stat *statbuf);

```

- **`fd`**: The file descriptor of the file whose information is being queried.
- **`statbuf`**: A pointer to a `struct stat` structure where the file information is stored.

### Return Value:

- On success, `fstat` returns 0.
- On error, -1 is returned, and `errno` is set to indicate the error.

### The `struct stat` Structure:

The `struct stat` structure includes information about the file such as:

- File size
- Device ID
- User ID of the file
- Group ID of the file
- File access permissions
- Last access, modification, and status change times
- Other details about the file

### Usage:

`fstat` is used when you need detailed information about a file and you have a file descriptor, which you might have obtained through calls to `open`, `socket`, or similar functions.

### Example in C++:
```cpp
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

int main() {
    // Open a file
    int fd = open("example.txt", O_RDONLY);
    if (fd < 0) {
        std::cerr << "Error opening file: " << strerror(errno) << std::endl;
        return 1;
    }

    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0) {
        std::cerr << "Error getting file stats: " << strerror(errno) << std::endl;
        close(fd);
        return 1;
    }

    std::cout << "File size: " << statbuf.st_size << " bytes" << std::endl;
    // ... Other information from statbuf can be printed similarly

    // Close the file
    close(fd);
    return 0;
}

```
In this example, `fstat` is used to get the statistics of the file "example.txt", and then the size of the file is printed. You can similarly access other fields of `statbuf` to get more information about the file.

`fstat` is a useful function in scenarios where file descriptors are used directly (as opposed to file streams), such as when working with system-level file operations, implementing custom file handling logic, or interfacing with operating system resources.