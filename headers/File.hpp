#pragma once
#include "Server.hpp"

class File {
public:
    std::string	Name;
    std::string Path;
    std::string sender;
    std::string receiver;

    File(std::string Name, std::string Path, std::string Sender, std::string Receiver);
    File(const File & x);
    ~File();
    File & operator=(const File & rhs);

private:
    File();
};
