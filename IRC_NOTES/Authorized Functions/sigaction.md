# sigaction
`sigaction` is a function used in Unix and Unix-like operating systems for signal handling in C and C++ programs. It's part of the POSIX (Portable Operating System Interface) standard and is more versatile and reliable compared to the older `signal` function.

Signals are a way for the operating system to notify a process that a particular event has occurred. Common signals include `SIGINT` (interrupt from keyboard, usually Ctrl+C), `SIGTERM` (request to terminate the process), and `SIGSEGV` (segmentation fault).

The `sigaction` function allows a program to control how it responds to signals. It can be used to specify a signal handling function (called a signal handler), to ignore a signal, or to perform the default action for the signal.

```cpp
#include <signal.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

```
- **`signum`**: The signal number you want to handle (e.g., `SIGINT`, `SIGTERM`).
- **`act`**: A pointer to a `struct sigaction` specifying the action to be associated with `signum`.
- **`oldact`**: Optionally, a pointer to a `struct sigaction` to store the previous action for the signal. This can be `NULL` if you don't need the previous action.

### The `struct sigaction`:

This structure is used to describe how a signal should be handled. It includes the following members:

- **`sa_handler`**: Specifies the signal handling function or action.
- **`sa_mask`**: Specifies a set of signals to be blocked during execution of the signal handling function.
- **`sa_flags`**: Used to modify the behavior of the signal. It can include flags like `SA_RESTART` to make certain system calls restartable, or `SA_SIGINFO` to use a more detailed signal handling function.

### Example Usage:

Here's an example of using `sigaction` to handle `SIGINT`:
```cpp
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int sig) {
    write(STDOUT_FILENO, "Caught SIGINT!\n", 15);
}

int main() {
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    while (1) {
        sleep(1);  // Sleep to simulate a long-running process
    }

    return 0;
}

```

In this example:

- A signal handler `sigint_handler` is defined to handle `SIGINT`.
- The `sigaction` function is used to associate `sigint_handler` with `SIGINT`.
- The program enters a loop, sleeping for one second at a time. If `SIGINT` (Ctrl+C) is received, the `sigint_handler` function will be called.

Using `sigaction` provides more control over signal handling compared to the `signal` function and is generally preferred in modern Unix/Linux programming.

