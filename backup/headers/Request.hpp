#pragma once
#include "Server.hpp"

class Request {
public:
    std::vector<std::string> args;
    std::string	command;
    bool invalidMessage;

    Request();
    ~Request();
    Request( const Request &x );
    Request & operator = ( const Request &rhs );
};
